#include "studyscenemodel.h"

#include <QDebug>
#include <QSqlTableModel>

StudySceneModel::StudySceneModel(QObject *parent, QSqlDatabase db)
    : QSqlTableModel(parent, db)
{
}

Qt::ItemFlags StudySceneModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QSqlTableModel::flags(index);
    if (index.column() == 3) {
        flags = Qt::ItemIsEnabled & Qt::ItemIsSelectable;
    }

    return flags;
}

QVariant StudySceneModel::data(const QModelIndex &index, int role) const
{
    return QSqlTableModel::data(index, role);
}
