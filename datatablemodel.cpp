#include "datatablemodel.h"

#include <QAbstractTableModel>
#include <QList>

DataTableModel::DataTableModel(QObject *parent, int rows, int columns)
    : QAbstractTableModel(parent), m_rows(rows), m_cols(columns)
{
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

//Qt::ItemFlags DataTableModel::flags(const QModelIndex &index) const
//{
//
//}

QVariant DataTableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        return m_data.at(index.row()).at(index.column());
    }

    return QVariant();
}
