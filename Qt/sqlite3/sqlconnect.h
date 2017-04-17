#ifndef SQLCONNECT_H
#define SQLCONNECT_H
#include "QtSql/QSqlDatabase"
#include "QtSql/QSqlError"
#include "QtSql/QSqlQuery"
#include <QDebug>

static bool sqlconnect()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("SqlCom.db");
    if(!db.open())
    {
        qDebug()<<db.lastError();
        qFatal("faild!!");
    }
    QSqlQuery query;
    query.exec("create table DataCom(time varchar(30) primary key,temp int,humi int,light int)");
    return true;
}
#endif // SQLCONNECT_H

