#pragma warning(push, 0)
#include <QString>
#include <QStringList>
#include <QDebug>
#pragma warning(pop)

#include "GTAUtil.h"
#include "GTASqliteDriver.h"


GTASqliteDriver::GTASqliteDriver(){

}

int GTASqliteDriver::myCallback(void *, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++){
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

int GTASqliteDriver::executeSqliteQuery(const QString &iDbFilePath, const QStringList &iQueries)
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    QString dbName = iDbFilePath;
    rc = sqlite3_open(dbName.toLatin1().constData(), &db);
    if( rc ){
        qDebug() << "Can't open database: " <<  QString(sqlite3_errmsg(db));
        //        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return(1);
    }

    for(int i = 0;i < iQueries.count();i++)
    {
        rc = sqlite3_exec(db, QString(iQueries.at(i)).toLatin1().constData(), myCallback, NULL, &zErrMsg);
        if( rc != SQLITE_OK )
        {
            qDebug() << "SQL error: " << QString(zErrMsg);
           // fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
    }
    sqlite3_close(db);
    return rc;
}
