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
#include <QStringList>

#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    QApplication::setStyle("cleanlooks");
    QApplication a(argc, argv);

    a.setOrganizationName("UMSL");
    a.setApplicationName("umsl");

    QStringList args = a.arguments();

    if (args.count() > 3) {
        cout << "usage: UMSL [--export-database] filename "
                "[--import-database] filename [--help]\n";
        return 0;
    }

    if (args.count() > 1 && args.at(1) == "--help") {
        cout << "TODO: help here\n";
    }
    else if (args.count() > 1 && args.at(1) == "--export-database") {
        cout << "check for second argument and export the database\n";
    }
    else if (args.count() > 1 && args.at(1) == "--import-database") {
        cout << "check for second argument and import the database\n";
    }
    else {
        MainWindow w;
        w.show();
        return a.exec();
    }
}
