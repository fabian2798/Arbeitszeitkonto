#ifndef DBHANDLE_H
#define DBHANDLE_H

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>
#include <QSqlQueryModel>
#include "Tagesdaten.h"
#include "monat.h"

class dbhandle
{
public:
    dbhandle();
    void create_table();
    void insert_table(Tagesdaten *data, monat *mdata);
    void show_table();
    void delete_table();
private:
    QSqlDatabase db;
    QSqlQueryModel model;
};

#endif // DBHANDLE_H
