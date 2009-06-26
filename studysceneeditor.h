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

#ifndef STUDYSCENEEDITOR_H
#define STUDYSCENEEDITOR_H

#include "ui_studysceneeditor.h"

#include "studyscenedelegate.h"

#include <QSqlRecord>

class Study;
class StudySceneModel;
class QSignalMapper;
class QSqlDatabase;
class QSqlRelationalDelegate;

class StudySceneEditor : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(StudySceneEditor)
public:
    StudySceneEditor(Study *study, QSqlDatabase *database, QWidget *parent = 0);
    ~StudySceneEditor();

Q_SIGNALS:
    void clicked(const QString &);
    
private Q_SLOTS:
    bool addNewScene(const QString &buttonText);
    bool removeScene();
    void populateNewRowValues(int row, QSqlRecord &record);
    void changeRemoveButtonState(QModelIndex);
    void submit();
    void submitAndClose();

private:
    Ui::StudySceneEditor m_ui;
    StudySceneModel *m_model;
    StudySceneDelegate *m_delegate;
    Study *m_study;
    QSignalMapper *m_signalMapper;
    int m_studyIndex;
};

#endif // STUDYSCENEEDITOR_H
