#ifndef STUDYLISTMODEL_H
#define STUDYLISTMODEL_H

#include <QModelIndex>
#include <QSqlDatabase>
#include <QSqlTableModel>

class StudyListModel : public QSqlTableModel
{
public:
    StudyListModel(QObject *parent = 0, QSqlDatabase database = QSqlDatabase());
    ~StudyListModel();
};

#endif // STUDYLISTMODEL_H
