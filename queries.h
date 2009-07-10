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

#ifndef QUERIES_H
#define QUERIES_H

#include <QString>

const QString CREATE_SCENES_QUERY = "create table scenes (id INTEGER PRIMARY KEY, "
                                    "name TEXT, length INTEGER, polling BOOLEAN, study INTEGER)";
const QString CREATE_STUDIES_QUERY = "create table studies (id INTEGER PRIMARY KEY, "
                                     "name TEXT)";

const QString TEST_STUDIES = "insert into studies (name)"
                             " values ('Test Study')";

const QString TEST_SCENES1 = "insert into scenes (name, length, polling, study) "
                             "values ('scene1', 10, 'true', 1)";
const QString TEST_SCENES2 = "insert into scenes (name, length, polling, study) "
                             "values ('scene2', 10, 'true', 1)";

#endif // QUERIES_H
