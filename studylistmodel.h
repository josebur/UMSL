#ifndef STUDYLISTMODEL_H
#define STUDYLISTMODEL_H

#include <QSqlQueryModel>
#include <QSqlDatabase>

class StudyListModel : public QSqlQueryModel
{
public:
    StudyListModel(QSqlDatabase *database, QObject *parent = 0);
    ~StudyListModel();

    QSqlDatabase *database() const;

private:
    QSqlDatabase *m_database;
};

#endif // STUDYLISTMODEL_H
