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
#include <QFileInfo>
#include <QGraphicsScene>
#include <QMessageBox>
#include <QSplitter>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRelation>
#include <QSqlRelationalDelegate>
#include <QSqlRelationalTableModel>
#include <QTableView>

#include "phidget21.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_ui.setupUi(this);
    QSplitter *page = new QSplitter(parent);
    page->addWidget(m_ui.studiesWidget);
    page->addWidget(m_ui.mainWidget);
    setCentralWidget(page);

    if (!connectToDatabase()) {
        QMessageBox::critical(this, "Database Error",
                              "Unable to connect to database");
    }

    m_studyListModel = new StudyListModel(0, m_database);
    m_ui.studyListView->setModel(m_studyListModel);
    m_ui.studyListView->setModelColumn(1);
    m_currentStudy = 0;

    connect(m_studyListModel, SIGNAL(dataChanged(const QModelIndex, const QModelIndex)),
            this, SLOT(updateActions(QModelIndex)));
    connect(m_ui.studyListView, SIGNAL(clicked(QModelIndex)), this, SLOT(updateActions(QModelIndex)));
    connect(m_ui.actionEditStudyScenes, SIGNAL(triggered()), this, SLOT(editStudyScenes()));
}

MainWindow::~MainWindow()
{
    delete m_studyListModel;
}

void MainWindow::editStudyScenes()
{
    StudySceneEditor editor(m_currentStudy, &m_database);
    editor.exec();
}

void MainWindow::updateActions(const QModelIndex &index)
{
    if (index.isValid()) {
        const QString name = index.data().toString();
        if (m_currentStudy != 0) {
            delete m_currentStudy;
        }
        m_currentStudy = new Study(name);

        m_ui.actionEditStudyScenes->setText(QString("Edit %1's Scenes").arg(name));
    }
}

bool MainWindow::connectToDatabase()
{
    QFileInfo info("umsl.db");
    bool createDatabase = !info.exists();

    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName("umsl.db");

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
        if (!query.exec(TEST_STUDIES)) {
            qDebug() << query.lastError().text();
            return false;
        }
        if (!query.exec(TEST_SCENES1)) {
            qDebug() << query.lastError().text();
            return false;
        }
        if (!query.exec(TEST_SCENES2)) {
            qDebug() << query.lastError().text();
            return false;
        }
        qDebug() << "Database created successfully";
    }
    return true;
}
