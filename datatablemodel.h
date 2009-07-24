#ifndef DATATABLEMODEL_H
#define DATATABLEMODEL_H

#include <QAbstractTableModel>
#include <QModelIndex>

class Study;

class DataTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    DataTableModel(QObject *parent = 0, Study *study = 0);

    // read only access
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    // edit access
    // TODO...

private:
    int m_rows;
    int m_cols;
    Study *m_study;

    QList< QList<qreal> > m_data; // not sure if this is the best data structure...we'll see.
};

#endif // DATATABLEMODEL_H
