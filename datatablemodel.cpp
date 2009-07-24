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
        return m_data.at(index.row()).at(index.column());
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
            return QString("%1").arg(section + 1);
        }
        else {
            return QVariant();
        }
    }
    return QVariant();
}
