#include "studylistmodel.h"

#include <QSqlQueryModel>
#include <QSqlQuery>

StudyListModel::StudyListModel(QSqlDatabase *database, QObject *parent)
    : QSqlQueryModel(parent), m_database(database)
{
    setQuery("select name from studies", *m_database);
}

StudyListModel::~StudyListModel()
{
    delete m_database;
}

