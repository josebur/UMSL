#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"

#include <QSqlDatabase>

class MainWindow : public QMainWindow {
    Q_OBJECT
    Q_DISABLE_COPY(MainWindow)
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public Q_SLOTS:
    void createStudy();

private:
    bool connectToDatabase();

    QSqlDatabase m_database;
    Ui::MainWindow m_ui;
};

#endif // MAINWINDOW_H
