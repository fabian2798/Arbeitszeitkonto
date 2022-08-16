#include "dbhandle.h"

dbhandle::dbhandle()
{
    const QString DRIVER ("QSQLITE");
    if(QSqlDatabase::isDriverAvailable(DRIVER)){
        db = QSqlDatabase::addDatabase(DRIVER, "zeitdb");
        if(!db.open()){
            qWarning() << "ERROR: Connection " << db.lastError();
        }
    }
}

void dbhandle::create_table(){
    QSqlQuery query("CREATE TABLE zeitkonto("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "date TEXT NOT NULL,"
                    "type TEXT NOT NULL,"
                    "office_time TEXT NULL,"
                    "flexible_time TEXT NULL,"
                    "summary TEXT NOT NULL;");
    if(!query.isActive()){
        qWarning() << "ERROR: Create Table " << query.lastError();
    }
}

void dbhandle::insert_table(Tagesdaten *data, monat *mdata){
    QSqlQuery query("INSERT INTO zeitkonto(year,month,day,day_nr,office_time,flexible_time,summary) "
                    "VALUES(:year,:month,:day,:day_nr,:office_time,:flexible_time,:summary)");
    query.bindValue(":year", mdata->getYear());
    query.bindValue(":month", mdata->getMonth());
    query.bindValue(":day", data->getTag());
    query.bindValue(":day_nr", data->getTages_nr());
    query.bindValue(":office_time", data->getOffice_time());
    query.bindValue(":flexible_time", data->getFlexible_time());
    query.bindValue(":summary", data->getNetto_zeit());//Netto Arbeitszeit am Tag
    if(!query.exec()){
        qWarning() << "ERROR: Insert Table " << query.lastError();
    }
}
void dbhandle::show_table(){
    QSqlQuery query("SELECT * FROM zeitkonto");
    if(!query.exec()){
        qWarning() << "ERROR: Show Table" << query.lastError();
    }
    while(query.next()){
        QString id = query.value(0).toString();
        // date = year:month:day
        QString year = query.value(1).toString();
        QString month = query.value(2).toString();
        QString day = query.value(3).toString();
        QString day_nr = query.value(4).toString();
        QString of = query.value(5).toString();
        QString fl = query.value(6).toString();
        QString sum = query.value(7).toString();

        // type = workday / url / abs / krk / glt / f

        qDebug() << id + " " + year + " " + month + " " + day + " " + day_nr + " " + of + " " + fl + " " + sum;
    }
}

void dbhandle::delete_table(){
    QSqlQuery query("DELETE zeitkonto");
    if(!query.exec()){
        qWarning() << "ERROR: Delete Table" << query.lastError();
    }
}
