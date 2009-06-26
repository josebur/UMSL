#include "studylistmodel.h"

#include <QDebug>
#include <QSqlQueryModel>
#include <QSqlQuery>

StudyListModel::StudyListModel(QObject *parent, QSqlDatabase database)
    : QSqlTableModel(parent, database)
{
    setTable("studies");
    select();
}

StudyListModel::~StudyListModel()
{
}
