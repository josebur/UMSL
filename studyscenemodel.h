#ifndef STUDYSCENEMODEL_H
#define STUDYSCENEMODEL_H

#include <QSqlDatabase>
#include <QSqlTableModel>

class StudySceneModel : public QSqlTableModel
{
    Q_OBJECT
public:
    StudySceneModel(QObject *parent = 0, QSqlDatabase db = QSqlDatabase());

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
};

#endif // STUDYSCENEMODEL_H
