/*
 *   Copyright 2009 by Joe Burns <josebur86@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "mainwindow.h"

#include "datatablemodel.h"
#include "phidgetpollingdevice.h"
#include "scene.h"
#include "study.h"
#include "studylistmodel.h"
#include "studysceneeditor.h"
#include "queries.h"

#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QGraphicsLinearLayout>
#include <QGraphicsScene>
#include <QGraphicsWidget>
#include <QMessageBox>
#include <QSettings>
#include <QSplitter>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRelation>
#include <QSqlRelationalDelegate>
#include <QSqlRelationalTableModel>
#include <QTableView>
#include <QTime>

const int NUMBER_OF_SEATS = 8;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_ui.setupUi(this);
    m_page = new QSplitter(parent);
    m_page->addWidget(m_ui.studiesWidget);
    m_page->addWidget(m_ui.mainWidget);
    m_page->addWidget(m_ui.studyOptionsWidget);
    setCentralWidget(m_page);

    m_databaseDir = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    m_databaseFile = m_databaseDir + QDir::separator() + "umsl.db";

    // This is a horrible way to do this but it will work for 1.0
    m_seats.append(m_ui.seat1);
    m_seats.append(m_ui.seat2);
    m_seats.append(m_ui.seat3);
    m_seats.append(m_ui.seat4);
    m_seats.append(m_ui.seat5);
    m_seats.append(m_ui.seat6);
    m_seats.append(m_ui.seat7);
    m_seats.append(m_ui.seat8);

    readSettings();

    if (!connectToDatabase()) {
        QMessageBox::critical(this, "Database Error",
                              "Unable to connect to database");
        close();
    }

    m_studyListModel = new StudyListModel(0, m_database);
    m_ui.studyListView->setModel(m_studyListModel);
    m_ui.studyListView->setModelColumn(1);
    m_ui.studyListView->setContextMenuPolicy(Qt::CustomContextMenu);

    m_currentStudy = 0;
    m_dataModel = 0;
    m_dataSaved = true;

    QModelIndex index = m_studyListModel->index(0, 1);
    if (index.isValid()) {
        m_ui.studyListView->setCurrentIndex(index);
        studyChanged(index);
        m_currentSecond = 0;
    }

    m_ui.dataView->setEnabled(true);
    m_ui.pauseButton->setEnabled(false);
    m_ui.actionSaveData->setDisabled(true);

    m_pollingDevice.init();

    // Button connections
    connect(m_ui.playButton, SIGNAL(clicked()), this, SLOT(startStudy()));
    connect(m_ui.pauseButton, SIGNAL(clicked()), this, SLOT(pauseStudy()));
    connect(m_ui.averageCheckBox, SIGNAL(toggled(bool)), m_ui.averageSpinBox, SLOT(setEnabled(bool)));

    // Study connections
    connect(m_studyListModel, SIGNAL(dataChanged(const QModelIndex, const QModelIndex)),
            this, SLOT(studyChanged(QModelIndex)));
    connect(m_ui.studyListView, SIGNAL(clicked(QModelIndex)), this, SLOT(studyChanged(QModelIndex)));
    connect(m_ui.studyListView, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(showStudyMenu(QPoint)));
    connect(m_studyListModel, SIGNAL(primeInsert(int,QSqlRecord&)),
            this, SLOT(setNewStudyName(int, QSqlRecord&)));

    // Menu connections
    connect(m_ui.actionSaveData, SIGNAL(triggered()), this, SLOT(saveDataToFile()));
    connect(m_ui.actionEditStudyScenes, SIGNAL(triggered()), this, SLOT(editStudyScenes()));
    connect(m_ui.actionAddNewStudy, SIGNAL(triggered()), this, SLOT(addNewStudy()));
    connect(m_ui.actionRemoveStudy, SIGNAL(triggered()), this, SLOT(removeStudy()));
    connect(m_ui.actionQuit, SIGNAL(triggered()), this, SLOT(close()));

    // Help Menu connections
    connect(m_ui.actionAboutUMSL, SIGNAL(triggered()), this, SLOT(about()));
    connect(m_ui.actionAboutQt, SIGNAL(triggered()), this, SLOT(aboutQt()));
}

MainWindow::~MainWindow()
{
    delete m_studyListModel;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}

void MainWindow::addNewStudy()
{
    const int rowCount = m_studyListModel->rowCount();
    m_studyListModel->insertRows(rowCount, 1);
    QModelIndex index = m_studyListModel->index(rowCount, 1);
    if (index.isValid()) {
        m_ui.studyListView->setCurrentIndex(index);
        m_ui.studyListView->scrollTo(index, QAbstractItemView::EnsureVisible);
        statusBar()->showMessage("New Study Created", 2000);
    }
}

void MainWindow::removeStudy()
{
    if (!m_currentStudy) {
        return;
    }
    if (QMessageBox::question(this, "Remove Study?",
                              QString("Are you sure your want to remove %1 and all of its scenes?")
                              .arg(m_currentStudy->name()),
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::No)
        == QMessageBox::No) {
        return;
    }

    QModelIndex index = m_ui.studyListView->selectionModel()->currentIndex();
    if (index.isValid()) {
        int id = index.sibling(index.row(), 0).data().toInt();
        QSqlQuery query;
        query.prepare("delete from scenes where study = ?");
        query.addBindValue(id);
        query.exec();

        m_studyListModel->removeRow(index.row());
        statusBar()->showMessage("Study Deleted", 2000);
    }
}

// FIXME: Make sure that there aren't multiple studies with the same name.
//        So there needs to be "New Study N" where N is the smallest number
//        that hasn't been used.
void MainWindow::setNewStudyName(int row, QSqlRecord &record)
{
    Q_UNUSED(row);

//    QModelIndex begin = m_studyListModel->index(0, 1);
//    if (begin.isValid()) {
//        QModelIndexList list = m_studyListModel->match(begin, Qt::DisplayRole, "New Study",
//                                                       -1);
//        const int n = list.count();
//        record.setValue(1, QString("New Study %1").arg(n+1));
//    }
//    else {
        record.setValue(1, "Enter a new study name");
    //}
}

void MainWindow::editStudyScenes()
{
    if (!m_currentStudy) {
        return;
    }
    StudySceneEditor editor(m_currentStudy, &m_database);
    editor.exec();
    QModelIndex index = m_ui.studyListView->selectionModel()->currentIndex();
    if (index.isValid()) {
        studyChanged(index);
    }
}

void MainWindow::studyChanged(const QModelIndex &index)
{
    if (index.isValid()) {
        const QString name = index.data(0).toString();
        if (m_currentStudy) {
            delete m_currentStudy;
        }
        m_currentStudy = new Study(name);
        updateActions(name);
        initStudy();

        connect(m_currentStudy, SIGNAL(studyEnded(Study*)),
                this, SLOT(endStudy()));
        connect(m_currentStudy, SIGNAL(studyTick()), this, SLOT(studyTick()));
    }
}

void MainWindow::updateActions(const QString &studyName)
{
    m_ui.actionEditStudyScenes->setText(QString("Edit %1's Scenes").arg(studyName));
    m_ui.actionRemoveStudy->setText(QString("Remove %1").arg(studyName));
}

void MainWindow::showStudyMenu(QPoint point)
{
    QMenu menu;
    QListView *view = m_ui.studyListView;
    QModelIndex index = view->indexAt(point);
    if (index.isValid()) {
        studyChanged(index);
        menu.addAction(m_ui.actionEditStudyScenes);
        menu.addAction(m_ui.actionRemoveStudy);
        menu.addSeparator();
    }
    menu.addAction(m_ui.actionAddNewStudy);
    menu.exec(view->mapToGlobal(point));
}

void MainWindow::startStudy()
{
    qDebug() << "startStudy called\n"
             << "average every " << m_ui.averageSpinBox->value() << " seconds";

    if (!m_currentStudy) {
        return;
    }

    if (m_currentStudy && !m_currentStudy->isPaused() && !m_dataSaved) {
        int ret = QMessageBox::warning(this, "Study Data Not Saved", "Study data has not been saved.\n"
                                       "Continue without saving?", QMessageBox::Yes | QMessageBox::No,
                                       QMessageBox::No);
        if (ret == QMessageBox::No) {
            return;
        }
    }

    m_ui.playButton->setDisabled(true);
    m_ui.pauseButton->setEnabled(true);
    m_ui.studyListView->setDisabled(true);
    m_ui.menuStudy->setDisabled(true);
    m_ui.averageSpinBox->setDisabled(true);
    m_ui.studyOptionsWidget->setDisabled(true);
    if (m_currentStudy->isPaused()) {
        m_currentStudy->resume();
        statusBar()->showMessage(m_currentStudy->name() + " Resumed", 2000);
    }
    else {
        m_currentSecond = 0;
        m_pollingSecond = 0;
        m_currentStudy->start();
        statusBar()->showMessage(m_currentStudy->name() + " Started", 2000);

        //create a new data model
        if (m_dataModel) {
            delete m_dataModel;
        }
        m_dataModel = new DataTableModel(this, m_currentStudy);
        m_ui.dataView->setModel(m_dataModel);
        m_ui.actionSaveData->setEnabled(true);
        m_dataSaved = false;
    }
}

void MainWindow::endStudy()
{
    m_ui.playButton->setEnabled(true);
    m_ui.pauseButton->setEnabled(false);
    m_ui.studyListView->setEnabled(true);
    m_ui.menuStudy->setEnabled(true);
    m_ui.averageSpinBox->setEnabled(true);
    m_ui.studyOptionsWidget->setEnabled(true);
    m_ui.dataView->setEnabled(true);

    QString name = m_currentStudy->name();
    statusBar()->showMessage(name + " Ended", 2000);

    QModelIndex index = m_ui.studyListView->selectionModel()->currentIndex();
    studyChanged(index);

    if (QMessageBox::question(this, "Save Data?", "Do you want to save the study data?",
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)
        == QMessageBox::Yes) {

        saveDataToFile();
    }
}

void MainWindow::pauseStudy()
{
    m_ui.playButton->setEnabled(true);
    m_ui.pauseButton->setDisabled(true);

    m_currentStudy->pause();
    statusBar()->showMessage(m_currentStudy->name() + " Paused", 2000);
}

void MainWindow::studyTick()
{
    qDebug() << "studyTick()";
    m_currentSecond++;
    int studySeconds = m_currentStudy->length();
    QTime currentTime(0, m_currentSecond / 60, m_currentSecond % 60);
    QTime studyTime(0, studySeconds / 60, studySeconds % 60);
    m_ui.studyTimeLabel->setText(QString("%1/%2")
                                 .arg(currentTime.toString("mm:ss"))
                                 .arg(studyTime.toString("mm:ss")));

    if (m_currentStudy->currentScene()->pollDuringScene()) {
        m_pollingSecond++;
        for (int i = 0; i < m_seats.count(); ++i) {
            if (m_seats.at(i)->isChecked()) {
                qreal value = m_pollingDevice.pollDevice(i);
                QModelIndex index = m_dataModel->index(i, m_pollingSecond-1);
                m_dataModel->setData(index, value, Qt::EditRole);
            }
        }
    }
}

void MainWindow::saveDataToFile()
{
    if (!m_dataModel) {
        return;
    }

    QString filename = QFileDialog::getSaveFileName(this, "Save Data",
                                                    m_currentStudy->name() + ".csv", "Comma Separated Value(*.csv)");
    if (!filename.isEmpty()) {
        m_dataSaved = true;

        if (!filename.endsWith(".csv")) {
            filename.append(".csv");
        }
        QFile data(filename);
        if (data.open(QFile::WriteOnly | QFile::Truncate)) {
            QTextStream out(&data);
            int rowCount = m_dataModel->rowCount();
            int columnCount = m_dataModel->columnCount();

            // write out the header
            out << ",";
            for (int i = 0; i < columnCount; ++i) {
                out << m_dataModel->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString()
                    << ",";
            }

            out << "\n";

            for (int i = 0; i < rowCount; ++i) {
                out << "Seat " << i + 1 << ",";
                for (int j = 0; j < columnCount; ++j) {
                 QModelIndex index = m_dataModel->index(i, j);
                    out << index.data(Qt::DisplayRole).toString() << ",";
                }
                out << "\n";
            }
            data.close();
        }
    }
}

void MainWindow::about()
{
    QString message("<b><c>UMSL</c></b><br>Version 1.0<br>Copyright (C) 2009 Joe Burns (josebur86@gmail.com)"
                    "<br><br>The program is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE WARRANTY "
                    "OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.");

    QMessageBox::about(this, "About UMSL", message);
}

void MainWindow::aboutQt()
{
    QMessageBox::aboutQt(this, "About Qt");
}

void MainWindow::writeSettings()
{
    QSettings settings;

    if (!isMaximized()) {
        settings.setValue("max", false);
        settings.setValue("geometry", saveGeometry());
    }
    else {
        settings.setValue("max", true);
    }
    settings.setValue("splitterSizes", m_page->saveState());

    QString seats;
    int i = 0;
    foreach(const QCheckBox *seat, m_seats) {
        if (seat->isChecked()) {
            seats.append(QString::number(i+1));
        }
        ++i;
    }
    settings.setValue("seatsChecked", seats);
    settings.setValue("averageEnabled", m_ui.averageCheckBox->isChecked());
    settings.setValue("averageInterval", m_ui.averageSpinBox->value());
}

void MainWindow::readSettings()
{
    QSettings settings;

   if (settings.value("max", false).toBool()) {
       showMaximized();
   }
   else {
       restoreGeometry(settings.value("geometry").toByteArray());
   }
   m_page->restoreState(settings.value("splitterSizes").toByteArray());

   QString seats = settings.value("seatsChecked", QVariant()).toString();
   for (int i = 0; i < NUMBER_OF_SEATS; ++i) {
       if (seats.contains(QString::number(i+1))) {
            m_seats.at(i)->setChecked(true);
       }
   }

   m_ui.averageCheckBox->setChecked(settings.value("averageEnabled").toBool());
   m_ui.averageSpinBox->setValue(settings.value("averageInterval", 1).toInt());
   m_ui.averageSpinBox->setEnabled(m_ui.averageCheckBox->isChecked());
}

bool MainWindow::connectToDatabase()
{   
    bool createDatabase = false;

    QDir dir;
    if (!dir.exists(dir.relativeFilePath(m_databaseFile))) {
        dir.mkpath(dir.relativeFilePath(m_databaseDir));
        createDatabase = true;
    }

    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(m_databaseFile);

    if (!m_database.open()) {
        qDebug() << m_database.lastError();
        return false;
    }
    if (createDatabase) {
        qDebug() << "Creating database...";
        QSqlQuery query(m_database);
        if (!query.exec(CREATE_SCENES_QUERY)) {
            qDebug() << query.lastError().text();
            return false;
        }
        if (!query.exec(CREATE_STUDIES_QUERY)) {
            qDebug() << query.lastError().text();
            return false;
        }
        qDebug() << "Database created successfully";
    }
    return true;
}

void MainWindow::initStudy()
{
    const QString name = m_currentStudy->name();
    int id = -1;
    QSqlQuery query("select (id) from studies where name = ?");
    query.addBindValue(name);
    query.exec();
    query.next();
    id = query.value(0).toInt();

    query.prepare("select id, name, length, polling from scenes where study = ? order by 1");
    query.addBindValue(id);
    query.exec();
    while (query.next()) {
        AbstractScene *scene;
        if (query.value(3).toBool()) {
            scene = new Scene(query.value(1).toString(), query.value(2).toInt());
        }
        else {
            scene = new BreakScene(query.value(1).toString(), query.value(2).toInt());
        }
        m_currentStudy->addScene(scene);
    }
    m_ui.timeline->setStudy(m_currentStudy);

    QTime time(0, m_currentStudy->length() / 60, m_currentStudy->length() % 60);
    m_ui.studyTimeLabel->setText(QString("00:00/%1").arg(time.toString("mm:ss")));
}
