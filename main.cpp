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

#include <QtGui/QApplication>
#include "mainwindow.h"

#include <QDebug>
#include <QDir>
#include <QDesktopServices>
#include <QFile>
#include <QString>
#include <QStringList>

#include <iostream>

using namespace std;

bool exportDatabase(const QString &filename)
{
    bool success = false;
    QString database = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    database += QDir::separator() + QString("umsl.db");

    if (!filename.isEmpty()) {
        if (filename.endsWith(".db")) {
            success = QFile::copy(database, filename);
        }
        else {
            cout << "filename must end with the extension \".db\"\n";
        }
    }

    return success;
}

bool importDatabase(const QString &filename)
{
    bool success = false;
    QString database = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    QString databaseFile(database + QDir::separator() + "umsl.db");

    QDir dir;

    if (!dir.exists(dir.relativeFilePath(databaseFile))) {
        dir.mkpath(database);
    }
    else {
        dir.remove(dir.relativeFilePath(databaseFile));
    }

    if (!filename.isEmpty()) {
        if (filename.endsWith(".db")) {
            success = QFile::copy(filename, databaseFile);
        }
        else {
            cout << "filename must end with the extension \".db\"\n";
        }
    }

    return success;
}

int main(int argc, char *argv[])
{
    //QApplication::setStyle("cleanlooks");
    QApplication a(argc, argv);

    a.setOrganizationName("UMSL");
    a.setApplicationName("umsl");

    QStringList args = a.arguments();

    if (args.count() > 3) {
        cout << "usage: UMSL [--export-database] filename.db "
                "[--import-database] filename.db [--help]\n";
        return -1;
    }

    if (args.count() > 1 && args.at(1) == "--help") {
        cout << "usage: UMSL [--export-database] filename.db "
                "[--import-database] filename.db [--help]\n";
        return 0;
    }
    else if (args.count() >= 2 && args.at(1) == "--export-database") {
        if (args.count() == 2) {
            cout << "--export-database needs a filename argument\n";
            return -1;
        }
        else if (exportDatabase(args.at(2))) {
            cout << "Database Exported Sucessfully\n";
        }
    }
    else if (args.count() >= 2 && args.at(1) == "--import-database") {
        if (args.count() == 2) {
            cout << "--import-database needs a filename argument\n";
        }
        else if (importDatabase(args.at(2))) {
            cout << "Database Imported Sucessfully\n";
        }
    }
    else {
        MainWindow w;
        w.show();
        return a.exec();
    }
}
