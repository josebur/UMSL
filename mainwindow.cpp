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

#include "phidget21.h"

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

    QModelIndex index = m_studyListModel->index(0, 1);
    if (index.isValid()) {
        m_ui.studyListView->setCurrentIndex(index);
        studyChanged(index);
    }

    m_ui.dataView->setEnabled(false);
    m_ui.pauseButton->setEnabled(false);

    // Enable when the methods actually work.
    m_ui.menuDatabase->setDisabled(true);

    connect(m_ui.playButton, SIGNAL(clicked()), this, SLOT(startStudy()));
    connect(m_ui.pauseButton, SIGNAL(clicked()), this, SLOT(pauseStudy()));
    connect(m_studyListModel, SIGNAL(dataChanged(const QModelIndex, const QModelIndex)),
            this, SLOT(studyChanged(QModelIndex)));
    connect(m_ui.studyListView, SIGNAL(clicked(QModelIndex)), this, SLOT(studyChanged(QModelIndex)));
    connect(m_ui.studyListView, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(showStudyMenu(QPoint)));
    connect(m_ui.actionEditStudyScenes, SIGNAL(triggered()), this, SLOT(editStudyScenes()));
    connect(m_ui.actionAddNewStudy, SIGNAL(triggered()), this, SLOT(addNewStudy()));
    connect(m_ui.actionRemoveStudy, SIGNAL(triggered()), this, SLOT(removeStudy()));
    connect(m_studyListModel, SIGNAL(primeInsert(int,QSqlRecord&)),
            this, SLOT(setNewStudyName(int, QSqlRecord&)));
    connect(m_ui.actionImportDatabase, SIGNAL(triggered()), this, SLOT(importDatabase()));
    connect(m_ui.actionExportDatabase, SIGNAL(triggered()), this, SLOT(exportDatabase()));
    connect(m_ui.actionQuit, SIGNAL(triggered()), this, SLOT(close()));
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
    qDebug() << "startStudy called";

    if (!m_currentStudy) {
        return;
    }

    m_ui.playButton->setDisabled(true);
    m_ui.pauseButton->setEnabled(true);
    m_ui.studyListView->setDisabled(true);
    m_ui.menuStudy->setDisabled(true);
    if (m_currentStudy->isPaused()) {
        m_currentStudy->resume();
    }
    else {
        m_currentStudy->start();
    }
}

void MainWindow::endStudy()
{
    m_ui.playButton->setEnabled(true);
    m_ui.pauseButton->setEnabled(false);
    m_ui.studyListView->setEnabled(true);
    m_ui.menuStudy->setEnabled(true);

    QModelIndex index = m_ui.studyListView->selectionModel()->currentIndex();
    studyChanged(index);
}

void MainWindow::pauseStudy()
{
    m_ui.playButton->setEnabled(true);
    m_ui.pauseButton->setDisabled(true);

    m_currentStudy->pause();
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

   m_ui.averageSpinBox->setValue(settings.value("averageInterval", 1).toInt());
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
}


//FIXME: These currently don't work ... going to disable them in the GUI for now.
//       What probably needs to happen is either, disconnect from the database,
//       copy the file, and then reconnect for exporting. Importing will be the same
//       thing, only reversed.
void MainWindow::exportDatabase()
{
    QString exportFileName = QFileDialog::getSaveFileName(this, "Export Database",
                                                          QString(), "Database files (*.db)");
    if (!exportFileName.isEmpty()) {
        if (!exportFileName.endsWith(".db")) {
            exportFileName.append(".db");
        }

        QFile::copy(m_databaseFile, exportFileName);
    }
}

void MainWindow::importDatabase()
{
    if (QMessageBox::warning(this, "Overwrite Database", "This will overwrite all of your existing "
                            "studies and all of their scenes. Do you want to continue?",
                            QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes) {
        QString databaseFileName = QFileDialog::getOpenFileName(this, "Import Database", QString(),
                                             "Database files (*.db)");
        QDir dir;
        if (!dir.exists(dir.relativeFilePath(m_databaseFile))) {
            dir.mkpath(dir.relativeFilePath(m_databaseDir));
        }

        qDebug() << m_databaseFile;
        qDebug() << databaseFileName;

        qDebug() << QFile::remove(m_databaseFile);
        qDebug() << QFile::copy(databaseFileName, m_databaseFile);
    }
}
