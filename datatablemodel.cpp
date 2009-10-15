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

#include "datatablemodel.h"
#include "study.h"

#include <QAbstractTableModel>
#include <QDebug>
#include <QList>

DataTableModel::DataTableModel(QObject *parent, Study *study)
    : QAbstractTableModel(parent), m_study(study)
{
    if (m_study) {
        m_rows = 8;
        QList<AbstractScene *> scenes = m_study->pollingScenes();
        int length = 0;
        foreach (AbstractScene *scene, scenes) {
            length += scene->length();
        }
        m_cols = length;
    } else {
        m_rows = 0;
        m_cols = 0;
    }

    for (int i = 0; i < m_rows; ++i) {
        QList<qreal> list;
        for (int j = 0; j < m_cols; ++j) {
            list.append(0.0);
        }
        m_data.append(list);
    }
}

int DataTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_rows;
}

int DataTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_cols;
}

QVariant DataTableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        qreal data = m_data.at(index.row()).at(index.column());
        return data;
    }

    return QVariant();
}

QVariant DataTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Vertical) {
            return QString("Seat %1").arg(section + 1);
        }
        else if (orientation == Qt::Horizontal) {
            QList<AbstractScene *> scenes = m_study->pollingScenes();
            int second = section+1;
            int sceneNumber = 1;
            foreach (AbstractScene *scene, scenes) {
                if (second > scene->length()) {
                    second -= scene->length();
                    sceneNumber++;
                }
            }
            return QString("S %1 %2").arg(sceneNumber).arg(second);
        }
        else {
            return QVariant();
        }
    }
    return QVariant();
}

bool DataTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::EditRole && index.isValid()) {
        int row = index.row();
        int col = index.column();
        QList<qreal> rowValues = m_data.value(row);
        rowValues.replace(col, value.toDouble());
        m_data.replace(row, rowValues);
        emit dataChanged(index, index);
        return true;
    }
    return false;
}
