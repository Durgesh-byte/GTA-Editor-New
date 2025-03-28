#include "AINGTASEQUENCERConfigDbManager.h"
#include <QDir>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>
#include <QApplication>

AINGTASEQUENCERConfigDbManager::AINGTASEQUENCERConfigDbManager()
{
    QString appPath = QApplication::applicationDirPath();

    _dbPath = QDir::cleanPath( QString("%1%2%3").arg(appPath,QDir::separator(),USER_DATA_DATABASE));
}

void AINGTASEQUENCERConfigDbManager::saveConfiguration(const QString &configName, const QStringList &lstScripts, const QStringList &lstTools, const QString &IPConnected)
{
    QString appPath = QApplication::applicationDirPath();
    //= QString("%1/%2/%3").arg(appPath,USER_DATA_DIR,USER_DATA_DATABASE);
    _dbPath = QDir::cleanPath( QString("%1%2%3").arg(appPath,QDir::separator(),USER_DATA_DATABASE));
    QFile dbFile(_dbPath);
    qDebug()<<_dbPath;
    if(!dbFile.exists())
    {
        createDatabase();
    }
    addNewConfig(configName,lstScripts,lstTools,IPConnected);
}

void AINGTASEQUENCERConfigDbManager::createDatabase()
{
    QStringList headerData;
    headerData<<"CONFIGNAME"<<"SCRIPT"<<"TOOL"<<"IPCONNECTED";
    QString appPath = QApplication::applicationDirPath();
    QString iDbFile = QDir::cleanPath( QString("%1%2%3").arg(appPath,QDir::separator(),USER_DATA_DATABASE));

    QString vals;
    QString name = headerData.at(0);
    QString primaryKey = QString("PRIMARY KEY (%1)").arg(name);
    for(int  i= 0; i < headerData.count(); i++)
    {
        QString str1 = headerData.at(i);
        QString str2 = QString("%1 varchar(255)%2").arg(str1,QString(","));
        vals += str2;
    }
    vals += primaryKey;


    QString createQuery = QString("CREATE TABLE USER_DATA_DB (%1);").arg(vals);

    _UserDb = QSqlDatabase::addDatabase("QSQLITE",USER_DATA_DATABASE);
    _UserDb.setDatabaseName(iDbFile);
    bool val = _UserDb.isValid();
    bool rc = _UserDb.open();
    if(!rc)
    {
        QString error;
        error = _UserDb.lastError().text();
        qDebug() << "Error : connection with database failed - " <<error;
    }
    else
    {
        QSqlQuery createTableQuery(_UserDb);
        rc = createTableQuery.exec(createQuery);
        if(!rc)
        {
            qDebug() << "Error :" << createTableQuery.lastError().text();

        }
    }
    closeDatabase();
}

void AINGTASEQUENCERConfigDbManager::closeDatabase()
{
    _UserDb.commit();
    _UserDb.close();
}

void AINGTASEQUENCERConfigDbManager::addNewConfig(const QString &configName, const QStringList &lstScripts, const QStringList &lstTools, const QString &IPConnected)
{
    bool check = _UserDb.isOpen();
    if(!check)
    {
        check = openUserDatabase(_dbPath);
    }
    if(check)
    {

        QString deleteQueryStr = QString("DELETE FROM %1 WHERE CONFIGNAME = \"%2\";").arg(USER_DATA_DATABASE,configName);
        QSqlQuery createTableQuery(_UserDb);
        if (!(createTableQuery.exec(deleteQueryStr)))
            qDebug()<<createTableQuery.lastError().text();
        QString scripts = lstScripts.join(",");
        QString tools = lstTools.join(",");
        QString ip(QString::null);
        QString val = QString("\"%1\",\"%2\",\"%3\",\"%4\"").arg(configName,scripts,tools,IPConnected);
        QString insertQueryStr = QString("INSERT INTO %1 (CONFIGNAME,SCRIPT,TOOL,IPCONNECTED) VALUES (%2);").arg(USER_DATA_DATABASE,val);

        if (!(createTableQuery.exec(insertQueryStr)))
            qDebug()<<createTableQuery.lastError().text();
    }
    closeDatabase();
}

bool AINGTASEQUENCERConfigDbManager::openUserDatabase(const QString &iFile)
{
    bool rc;
    QFileInfo fileInfo(iFile);
    rc = fileInfo.exists();
    if(rc)
    {
        _UserDb = QSqlDatabase::addDatabase("QSQLITE",USER_DATA_DATABASE);
        _UserDb.setDatabaseName(iFile);
        bool val = _UserDb.isValid();
        rc = _UserDb.open();
        if(!rc)
        {
            QString error;
            error = _UserDb.lastError().text();
            qDebug() << "Error : connection with database failed - " <<error;
        }
    }
    return rc;
}

QStringList AINGTASEQUENCERConfigDbManager::getConfigNames()
{
    QStringList lstConfigNames;
    bool check = _UserDb.isOpen();
    if(!check)
    {
        check = openUserDatabase(_dbPath);
    }
    if(check)
    {
        QSqlQuery selectQuery(_UserDb);
        QString selectQueryStr = QString("SELECT DISTINCT CONFIGNAME FROM %1;").arg(USER_DATA_DATABASE);
        bool rc = selectQuery.exec(selectQueryStr);
        if(rc)
        {
            if(selectQuery.isActive() && selectQuery.isSelect())
            {
                while(selectQuery.next())
                {
                    QString sessionName = selectQuery.value(0).toString();
                    lstConfigNames << sessionName;
                }
            }
        }
        else
        {
            qDebug() << "Error : " << selectQuery.lastError().text();
        }
    }
    else
    {
        qDebug() << "Error while opening USER_DATA_DB";
    }
    closeDatabase();
    return lstConfigNames;
}

void AINGTASEQUENCERConfigDbManager::deleteConfigs(const QStringList &iSelectedItems)
{
    bool check = _UserDb.isOpen();
    if(!check)
    {
        check = openUserDatabase(_dbPath);
    }
    if(check)
    {
        QStringList conditionStmt;
        foreach(QString item, iSelectedItems)
        {
            conditionStmt.append(QString("CONFIGNAME = '%1'").arg(item));
        }

        QString deleteQueryStr = QString("DELETE FROM %1 WHERE %2;").arg(USER_DATA_DATABASE,conditionStmt.join(" AND "));
        QSqlQuery deleteQuery(_UserDb);
        bool  rc = deleteQuery.exec(deleteQueryStr);
        if(!rc)
        {
            qDebug() << "Error while deleting session information" << deleteQuery.lastError().text();
        }
    }
    else
    {
        qDebug() << "Error while opening USER_DATA_DB";
    }
    closeDatabase();
}

QStringList AINGTASEQUENCERConfigDbManager::getConfigFiles(const QString &iConfigName)
{
    QStringList lstFiles;
    bool check = _UserDb.isOpen();
    if(!check)
    {
        check = openUserDatabase(_dbPath);
    }
    if(check)
    {
        QSqlQuery selectQuery(_UserDb);
        QString selectQueryStr = QString("SELECT * FROM %1 WHERE CONFIGNAME = '%2';").arg(USER_DATA_DATABASE,iConfigName);
        bool rc = selectQuery.exec(selectQueryStr);
        if(rc)
        {
            if(selectQuery.isActive() && selectQuery.isSelect())
            {
                while(selectQuery.next())
                {
                    QString fileName;
                    for(int i=1;i<4;i++)
                    {
                        fileName = selectQuery.value(i).toString();
                        lstFiles.append(fileName);
                    }
                    //lstFiles << fileName;

                }
            }
        }
        else
        {
            qDebug() << "Error : " << selectQuery.lastError().text();
        }
    }
    else
    {
        qDebug() << "Error while opening USER_DATA_DB";
    }
    closeDatabase();
    return lstFiles;

}
