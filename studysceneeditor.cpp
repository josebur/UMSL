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

#include "studysceneeditor.h"

#include "study.h"
#include "studyscenedelegate.h"
#include "studyscenemodel.h"

#include <QDebug>
#include <QMessageBox>
#include <QModelIndex>
#include <QSignalMapper>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQuery>

StudySceneEditor::StudySceneEditor(Study *study, QSqlDatabase *database, QWidget *parent) :
    QDialog(parent)
{
    m_ui.setupUi(this);
    m_ui.removeSceneButton->setEnabled(false);

    m_model = new StudySceneModel(this, *database);
    m_delegate = new StudySceneDelegate(this);
    m_study = study;
    m_studyIndex = -1;

    QString title = windowTitle();
    setWindowTitle(m_study->name() + " - " + title);

    QSqlQuery query(*database);
    query.prepare("select id from studies where name = ?");
    query.addBindValue(m_study->name());

    // if there is already a study with that name, show the scenes for it
    if (query.exec() && query.next()) {
         m_studyIndex = query.value(0).toInt();
    }

    m_model->setTable("scenes");
    //m_model->setEditStrategy(QSqlTableModel::OnFieldChange);
    QString filter = QString("study = %1").arg(m_studyIndex);
    m_model->setFilter(filter);
    m_model->setHeaderData(1, Qt::Horizontal, "Name", Qt::DisplayRole);
    m_model->setHeaderData(2, Qt::Horizontal, "Length \n (in seconds)", Qt::DisplayRole);
    m_model->setHeaderData(3, Qt::Horizontal, "Type", Qt::DisplayRole);
    m_model->select();

    // Set up the view
    m_ui.sceneView->setModel(m_model);
    m_ui.sceneView->setColumnHidden(0, true);
    m_ui.sceneView->setColumnHidden(4, true);
    m_ui.sceneView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    m_ui.sceneView->setItemDelegate(m_delegate);
    m_ui.sceneView->update();

    m_signalMapper = new QSignalMapper(this);
    connect(m_ui.addSceneButton, SIGNAL(clicked()), m_signalMapper, SLOT(map()));
    m_signalMapper->setMapping(m_ui.addSceneButton, m_ui.addSceneButton->text());
    connect(m_ui.addBreakSceneButton, SIGNAL(clicked()), m_signalMapper, SLOT(map()));
    m_signalMapper->setMapping(m_ui.addBreakSceneButton, m_ui.addBreakSceneButton->text());
    connect(m_signalMapper, SIGNAL(mapped(QString)), this, SIGNAL(clicked(const QString &)));
    connect(this, SIGNAL(clicked(QString)), this, SLOT(addNewScene(QString)));


    connect(m_ui.closeButton, SIGNAL(clicked()), this, SLOT(submitAndClose()));
    connect(m_ui.removeSceneButton, SIGNAL(clicked()), this, SLOT(removeScene()));
    connect(m_ui.sceneView, SIGNAL(clicked(QModelIndex)), this, SLOT(changeRemoveButtonState(QModelIndex)));
    connect(m_model, SIGNAL(primeInsert(int, QSqlRecord&)),
            this, SLOT(populateNewRowValues(int, QSqlRecord&)));
}

StudySceneEditor::~StudySceneEditor()
{
    delete m_model;
}

bool StudySceneEditor::addNewScene(const QString &buttonText)
{
    submit();
    int rowCount = m_model->rowCount();
    bool ok = m_model->insertRows(rowCount, 1);
    if (ok) {
        submit();
    }

    // if we're adding a break scene, set the polling to false
    if (buttonText == "Add &Non-Polling Scene") {
        QModelIndex index = m_model->index(rowCount, 3);
        m_model->setData(index, false);
    }
    QModelIndex index = m_model->index(rowCount, 1);
    m_ui.sceneView->selectionModel()->setCurrentIndex(index,
                                                      QItemSelectionModel::Select);
    m_ui.sceneView->scrollTo(index);
    m_ui.sceneView->setFocus();
    return ok;
}

bool StudySceneEditor::removeScene()
{
    QItemSelectionModel *selectionModel = m_ui.sceneView->selectionModel();
    QModelIndex index = selectionModel->currentIndex();
    if (!index.isValid()) {
        return false;
    }
    int row = selectionModel->currentIndex().row();
    if (m_model->removeRows(row, 1)) {
        //m_ui.sceneView->hideRow(row);
        return true;
    }
    else {
        return false;
    }
}

void StudySceneEditor::populateNewRowValues(int row, QSqlRecord &record)
{
    Q_UNUSED(row);

    record.setValue("name", "Enter New Scene Name");
    record.setValue("length", 1);
    record.setValue("polling", true);
    record.setValue("study", m_studyIndex);
}

void StudySceneEditor::changeRemoveButtonState(QModelIndex index)
{
    if (index.isValid()) {
        m_ui.removeSceneButton->setEnabled(true);
    }
}

void StudySceneEditor::submit()
{
    m_model->database().transaction();
    if (m_model->submitAll()) {
        m_model->database().commit();
    }
}

void StudySceneEditor::submitAndClose()
{
    submit();
    close();
}
