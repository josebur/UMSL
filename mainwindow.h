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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"

#include "phidgetpollingdevice.h"

#include <QSqlDatabase>

class QSplitter;

class DataTableModel;
class Study;
class StudyListModel;

class MainWindow : public QMainWindow {
    Q_OBJECT
    Q_DISABLE_COPY(MainWindow)
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private Q_SLOTS:
    void studyChanged(const QModelIndex &index);
    void addNewStudy();
    void removeStudy();
    void setNewStudyName(int row, QSqlRecord &record);
    void editStudyScenes();
    void updateActions(const QString &studyName);
    void showStudyMenu(QPoint point);
    void startStudy();
    void endStudy();
    void pauseStudy();
    void studyTick();
    void saveDataToFile();
    void about();
    void aboutQt();

private:
    void writeSettings();
    void readSettings();
    bool connectToDatabase();
    void initStudy();

    QSqlDatabase m_database;
    QString m_databaseDir;
    QString m_databaseFile;
    StudyListModel *m_studyListModel;
    Study *m_currentStudy;
    int m_currentSecond;
    int m_pollingSecond;
    QSplitter *m_page;
    QList<QCheckBox *> m_seats;
    DataTableModel *m_dataModel;
    PhidgetPollingDevice m_pollingDevice;
    bool m_dataSaved;
    Ui::MainWindow m_ui;
};

#endif // MAINWINDOW_H
