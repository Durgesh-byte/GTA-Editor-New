#include "AINCMNSqlDbLoader.h"
using namespace AINCMN::DbUtils;

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QSqlError>
#include <QVariant>
#include <QSqlDriver>


bool AINCMNSqlDbLoader::importFromSqlFile(const QString& iPath, QSqlDatabase* iopDb)
{
    bool bOk (false);

    if(! iopDb)
        return bOk;

    Q_ASSERT (iopDb->isOpen());
    Q_ASSERT (iopDb->isValid());

    QFile sqlFile(iPath);
    if(sqlFile.open(QFile::ReadOnly|QFile::Text))
    {
        bOk = importFromSqlFile(sqlFile, iopDb);
    }

    return bOk;
}

bool AINCMNSqlDbLoader::importFromSqlFile(QFile& iFile, QSqlDatabase* iopDb)
{
    bool bOk (false);

    Q_ASSERT (!! iopDb);
    Q_ASSERT (iopDb->isOpen());
    Q_ASSERT (iopDb->isValid());

    QTextStream textStream(&iFile);
    QString text = textStream.readAll();
    text.remove("\t");
    text.remove("\n");

    QStringList lstSqlCmds (text.split(";"));
    for(int i=0; i<lstSqlCmds.count(); ++i)
    {
        QSqlQuery* pResult = AINCMNSqlDbLoader::fireSql(lstSqlCmds[i], iopDb);
        if(!! pResult)
        {
            if(pResult->lastError().isValid())
            {
                qDebug () << pResult->lastError().text();
                bOk = false;
            }
            else
            {
                bOk = true;
            }

            delete pResult;
            pResult = NULL;
        }
    }

    return bOk;
}

QSqlQuery* AINCMNSqlDbLoader::fireSql(const QString& iSqlCmd, QSqlDatabase* iopDb)
{
     QSqlQuery* pResult = NULL;

    if(! iopDb)
        return pResult;

    Q_ASSERT (iopDb->isOpen());
    Q_ASSERT (iopDb->isValid());

    if(iSqlCmd.isEmpty() || iSqlCmd.isNull())
        return pResult;

    pResult = new QSqlQuery(*iopDb);
    pResult->exec(iSqlCmd);
    return pResult;
}

QSqlDatabase* AINCMNSqlDbLoader::createInMemoryDbFromDbFile(const QString& FileName)
{
    bool bOk (false);
    QSqlDatabase* pDb = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", "InMemoryDatabase"));
    pDb->setDatabaseName(":memory:");
    bOk = pDb->open();
    bOk = AINCMNSqlDbLoader::loadFromDbFile(pDb, FileName);
    return pDb;
}


bool AINCMNSqlDbLoader::loadFromDbFile(QSqlDatabase*& iopDb, const QString& iFileName)
{
    bool bOk (false);

    if(! iopDb)
        return bOk;

    if(! iopDb->isValid())
        return bOk;

    if(! iopDb->isOpen())
        iopDb->open();

    QString sql;
    sql = QString("ATTACH DATABASE ") + QString("'") + iFileName + QString("'") + QString(" AS fileDB");
    QSqlQuery result = iopDb->exec(sql);
    qDebug() << result.lastError().text();

    sql = "SELECT name, sql FROM fileDB.sqlite_master where type='table'";
    result = iopDb->exec(sql);
    qDebug() << result.lastError().text();

    while (result.next())
    {
        QString tlb = result.value(0).toString();
        QString tlbSql = result.value(1).toString();

        // create table in db
        QSqlQuery r = iopDb->exec(tlbSql);
        qDebug() << r.lastError().text();

        // copy all the data between tables
        sql = "INSERT INTO " + tlb + " SELECT * FROM fileDB." + tlb;
        QSqlQuery r2 = iopDb->exec(sql);
        qDebug() << r2.lastError().text();

        /*sql = "CREATE TABLE "+ tlb +" AS SELECT * FROM fileDB." + tlb;
        qDebug() << sql;
        QSqlQuery r = iopDb->exec(sql);
        qDebug() << r.lastError().text();*/
    }

    // re-index the db
    sql = "SELECT name, sql FROM fileDB.sqlite_master where type='index'";
    result = iopDb->exec(sql);
    qDebug() << result.lastError().text();

    while (result.next())
    {
        QString index = result.value(0).toString();
        QString indexSql = result.value(1).toString();

        QSqlQuery r = iopDb->exec(indexSql);
        qDebug() << r.lastError().text();
    }

    result = iopDb->exec("DETACH fileDB");
    qDebug() << result.lastError().text();

    return true;
}

bool AINCMNSqlDbLoader::writeToDbFile(QSqlDatabase* pFromDb, const QString& FileName)
{
    bool bOk (false);

    if(! pFromDb)
        return bOk;

    QSqlQuery result;

    QString sql;
    sql = QString("ATTACH DATABASE ") + QString("'") + FileName + QString("'") + QString(" AS newDB");
    result = pFromDb->exec(sql);
    qDebug() << result.lastError().text();

    // copy all tables
    sql = "SELECT name, sql FROM sqlite_master where type='table'";
    result = pFromDb->exec(sql);
    qDebug() << result.lastError().text();

    while (result.next())
    {
        QString tlb = result.value(0).toString();
        QString tlbSql = result.value(1).toString();

        // create table
        tlbSql = tlbSql.replace(tlb, QString("newDB." + tlb));
        QSqlQuery r = pFromDb->exec(tlbSql);
        qDebug() << r.lastError().text();

        // transfer data between tables
        sql = "INSERT INTO newDB." + tlb + " SELECT * FROM " + tlb;
        QSqlQuery r2 = pFromDb->exec(sql);
        qDebug() << r2.lastError().text();

        /*sql = "CREATE TABLE newDB."+ tlb +" AS SELECT * FROM " + tlb;
        qDebug() << sql;
        QSqlQuery r = pFromDb->exec(sql);
        qDebug() << r.lastError().text();*/
    }

    // re-index the db
    sql = "SELECT name, sql FROM sqlite_master where type='index'";
    result = pFromDb->exec(sql);
    qDebug() << result.lastError().text();

    while (result.next())
    {
        QString index = result.value(0).toString();
        QString indexSql = result.value(1).toString();
        QString newIndex = "newDB." + index;

        indexSql = indexSql.replace(index, newIndex);

        QSqlQuery r = pFromDb->exec(indexSql);
        qDebug() << r.lastError().text();
    }

    result = pFromDb->exec("DETACH newDB");
    bOk = result.lastError().isValid();

    return bOk;
}
