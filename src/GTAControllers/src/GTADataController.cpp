#include "GTADataController.h"
#include "GTAICDParser.h"
#include "GTAICDParameter.h"
#include "GTALocalDbUtil.h"
#include "GTASystemServices.h"
#include "GTAAppController.h"
#include "GTAEquipmentMap.h"

#pragma warning(push, 0)
#include <QApplication>
#include <QtConcurrentMap>
#include <QFuture>
#include <QFutureWatcher>
#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>
#include <QFileInfo>
#include <QDateTime>
#include <QtConcurrentRun>
#include <QProgressBar>
#include <QMessageBox>
#pragma warning(pop)

GTADataController::GTADataController(QObject* parent) : QObject(parent)
{
    _LastError = "";
    _DbFilePath = "";
}
GTADataController::GTADataController(const QString iLocalDb, QObject* parent) :
    QObject(parent)
{
    _LastError = "";
    setLocalDatabaseFile(iLocalDb);
}
GTADataController::~GTADataController()
{
    // GTALocalDbUtil::closeDatabase();
}

void GTADataController::setLocalDatabaseFile(const QString iLocalDb)
{
    _DbFilePath = iLocalDb;
}

bool GTADataController::openUserDatabase(const QString& iFilePath)
{
    QFile File(iFilePath);
    bool rc = File.exists();
    if (!rc)
    {
        _LastError = QString("Error: GTADataController: File %1 does not exists");
        return rc;
    }
    if (rc)
    {
        rc = GTALocalDbUtil::openUserDatabase(iFilePath);
    }
    return rc;
}

bool GTADataController::openDatabase()
{
    QFile File(_DbFilePath);
    bool rc = File.exists();
    if (!rc)
    {
        _LastError = QString("Error: GTADataController: File %1 does not exists");
        return rc;
    }
    if (rc)
    {
        rc = GTALocalDbUtil::openDatabase(_DbFilePath);
    }
    return rc;
}


void GTADataController::closeDatabase()
{
    GTALocalDbUtil::closeDatabase();
}

bool GTADataController::deleteTableInParamDb(const QString& iDbPath)
{
    bool rc = GTALocalDbUtil::deleteTableInParamDb(iDbPath, PARAMETER_TABLE);
    return rc;
}

bool GTADataController::isTableExisting(const QString& iTableName)
{
    bool rc = GTALocalDbUtil::isTableExisting(iTableName);
    return rc;
}

bool GTADataController::createTableInParamDB()
{
    QString appPath = QApplication::applicationDirPath();
    GTASystemServices* pSystemService = GTAAppController::getGTAAppController()->getSystemService();
    QString sDbFile = pSystemService->getDatabaseFile();
    QFile dbFile(sDbFile);

    if (dbFile.exists())
    {

        bool rc = GTALocalDbUtil::createTableInParamDB(sDbFile);
        return rc;

    }
    else
    {
        return false;
    }
}

bool GTADataController::updateTableVersionInParamDB()
{
    GTASystemServices* pSystemService = GTAAppController::getGTAAppController()->getSystemService();
    QString sDbFile = pSystemService->getDatabaseFile();
    QFile dbFile(sDbFile);

    if (dbFile.exists())
    {
        bool rc = GTALocalDbUtil::updateTableVersionInParamDB(sDbFile);
        return rc;
    }
    else
        return false;
}
bool GTADataController::updateParameterFromFiles(const QStringList& iICDFiles, const QString& iToolId)
{
    bool rc = false;
    if (iICDFiles.count() > 0)
    {
        QSqlDatabase db = GTALocalDbUtil::getParamDatabase();
        if (db.isOpen())
        {
            GTALocalDbUtil::setDefaultParseFlag();

            QString libraryPath = QString::fromStdString(TestConfig::getInstance()->getLibraryPath());
            foreach(QString file, iICDFiles)
            {
                QFileInfo dbFileInfo(file);
                QString dbFilePath = dbFileInfo.absolutePath();
                QString fileType = "";
                if (dbFilePath.endsWith(ICD_DIR, Qt::CaseInsensitive))
                    fileType = ICD_DIR;
                else if (dbFilePath.endsWith(PMR_DIR, Qt::CaseInsensitive))
                    fileType = PMR_DIR;
                else if (dbFilePath.endsWith(PIR_DIR, Qt::CaseInsensitive))
                    fileType = PIR_DIR;
                else if (dbFilePath.endsWith(MODEL_ICD_DIR, Qt::CaseInsensitive))
                    fileType = MODEL_ICD_DIR;
                else if (dbFilePath.endsWith(VCOM_DIR, Qt::CaseInsensitive))
                    fileType = VCOM_DIR;

                rc = GTALocalDbUtil::updateParameterInFile(file, fileType, libraryPath, iToolId);

                if (!rc)
                {
                    _LastError += QString("Fail to update data from file %1\n").arg(file);
                    _LastError += GTALocalDbUtil::getLastError() + "\n";
                }
            }
        }
        else
        {
            _LastError = GTALocalDbUtil::getLastError();
            _LastError.append("\nDatabase is locked");
        }
    }
    else
        _LastError = "File list is empty";

    if (rc)
    {
        QSqlDatabase db = GTALocalDbUtil::getParamDatabase();
        if (!db.isOpen())
        {
            db.open();
        }
    }
    return rc;
}

void GTADataController::onParseFinish()
{

}


QList<GTAICDParameter> GTADataController::searchParameter(const QString& iStr1, const QString& iStr2, const QString& iOperator,
    bool byName, bool byIdentifier, bool byLabel)
{
    byName = byName;//todo: code to supress warning, requires further investigation why this is used.
    QList<GTAICDParameter> oParamList;

    QString queryStr = "SELECT * FROM PARAMETER WHERE ";
    if (!iStr1.isEmpty())
    {
        queryStr += QString("NAME LIKE %1").arg(iStr1);
    }

    if (!iStr2.isEmpty())
    {
        queryStr += QString("%1 NAME LIKE %2").arg(iOperator, iStr2);
    }

    if (byIdentifier)
    {
        if (!iStr1.isEmpty())
            queryStr += QString("%1 IDENTIFIER LIKE %2").arg(iOperator, iStr1);
        if (!iStr2.isEmpty())
            queryStr += QString("%1 IDENTIFIER LIKE %2").arg(iOperator, iStr2);
    }
    if (byLabel)
    {
        if (!iStr1.isEmpty())
            queryStr += QString("%1 LABEL LIKE %2").arg(iOperator, iStr1);
        if (!iStr2.isEmpty())
            queryStr += QString("%1 LABEL LIKE %2").arg(iOperator, iStr2);
    }

    //failsafe to check query doesn't fail
    queryStr.replace("'", "");

    QSqlDatabase db = GTALocalDbUtil::getParamDatabase();
    if (db.isOpen())
    {
        QSqlQuery sqlQuery(db);
        bool rc = sqlQuery.exec(queryStr);
        if (rc)
        {

            while (sqlQuery.next())
            {
                QSqlRecord record = sqlQuery.record();
                QString name = record.value("NAME").toString();
                QString uppername = record.value("UPPERNAME").toString();
                QString bit = record.value("BIT").toString();
                QString file = record.value("FILE").toString();
                QString ident = record.value("IDENTIFIER").toString();
                QString label = record.value("LABEL").toString();
                QString minVal = record.value("MNVAL").toString();
                QString maxVal = record.value("MXVAL").toString();
                QString paramType = record.value("PARAMTYPE").toString();
                QString signalType = record.value("SIGNALTYPE").toString();
                QString unit = record.value("UNIT").toString();
                QString valueType = record.value("VALUETYPE").toString();
                QString equipmentName = record.value("EQUIPMENTNAME").toString();
                QString tempName = record.value("TEMPPARAMNAME").toString();
                GTAICDParameter Param;
                Param.setName(name);
                Param.setUpperName(uppername);
                Param.setTempParamName(tempName);
                Param.setBit(bit);
                Param.setSourceFile(file);
                Param.setIdentifier(ident);
                Param.setLabel(label);
                Param.setMinValue(minVal);
                Param.setMaxValue(maxVal);
                Param.setParameterType(paramType);
                Param.setSignalType(signalType);
                Param.setUnit(unit);
                Param.setValueType(valueType);
                Param.setEquipmentName(equipmentName);
                oParamList.append(Param);

            }

            if (!sqlQuery.lastError().text().isEmpty())
            {
                _LastError = sqlQuery.lastError().text();
            }
        }
        else
        {
            _LastError = "Error: GTADataController: Fail to execute query\n";
            _LastError += sqlQuery.lastError().text();
        }
    }
    return oParamList;
}

void GTADataController::generateLocalDatabase(QStringList fileList, QString iExtn)
{

    bool rc = false;
    QSqlDatabase db = GTALocalDbUtil::getParamDatabase();
    if (db.isOpen())
    {
        GTALocalDbUtil::setDefaultParseFlag();

        foreach(QString  file, fileList)
        {
            QFileInfo dbFileInfo(file);
            QString dbFilePath = dbFileInfo.absoluteFilePath();
            QString fileType = "";
            if (dbFilePath.endsWith(ICD_DIR, Qt::CaseInsensitive))
                fileType = ICD_DIR;
            else if (dbFilePath.endsWith(PMR_DIR, Qt::CaseInsensitive))
                fileType = PMR_DIR;
            else if (dbFilePath.endsWith(PIR_DIR, Qt::CaseInsensitive))
                fileType = PIR_DIR;
            else if (dbFilePath.endsWith(MODEL_ICD_DIR, Qt::CaseInsensitive))
                fileType = MODEL_ICD_DIR;
            else if (dbFilePath.endsWith(VCOM_DIR, Qt::CaseInsensitive))
                fileType = VCOM_DIR;

            GTASystemServices* pSystemService = GTAAppController::getGTAAppController()->getSystemService();
            QString ToolId = pSystemService->readAndGetToolIdFromXml();

            rc = GTALocalDbUtil::updateParameterInFile(file, fileType, QString::fromStdString(TestConfig::getInstance()->getLibraryPath()), ToolId);
        }
    }
}

QSqlDatabase GTADataController::getLocalDatabase()
{
    return GTALocalDbUtil::getParamDatabase();
}

/* This function allow to identify files to be updated and those to be removed from the Database
* @input: iList - List of all present files in the current bench database folder
* @output: oFileToUpdate - List containing the file name to be updated
* @output: oFileToRemove - List containing the file name to be removed
* @return: bool
*/
bool GTADataController::getFileListToUpdateParameter(const QStringList& iList, QStringList& oFileToUpdate, QStringList& oFileToRemove)
{
    bool rc = false;
    QSqlDatabase _Database = GTALocalDbUtil::getParamDatabase();
    if (_Database.isOpen())
    {
        QStringList tables = _Database.tables();

        if (tables.contains(PARAMETER_TABLE))
        {
            QString tableName = PARAMETER_TABLE;

            // Prepared request to get the information about file in database
            QHash<QString, QString> FileLastModifTable;
            QString queryFileList = QString("SELECT FILE,FILELASTMODIFIED FROM [%1]").arg(tableName);
            QSqlQuery SqlQuery(queryFileList, _Database);

            while (SqlQuery.next())
            {
                QSqlRecord SqlRecord = SqlQuery.record();
                QString filePath = SqlRecord.value("FILE").toString();
                QString lastModifiedDate = SqlRecord.value("FILELASTMODIFIED").toString();
                FileLastModifTable.insert(filePath, lastModifiedDate);
            }

            // Determine if the file has been modified since the last modifcation date
            foreach(QString filePath, FileLastModifTable.keys())
            {
                if (!filePath.isEmpty())
                {
                    if (iList.contains(filePath))
                    {
                        QString lastModifDate = FileLastModifTable.value(filePath);
                        QFileInfo fileInfo;
                        QString libraryPath = QString::fromStdString(TestConfig::getInstance()->getLibraryPath());
                        fileInfo.setFile(libraryPath + filePath);
                        QDateTime buildTime = fileInfo.lastModified();
                        QString currentDate = buildTime.toString("'M'M'd'd'y'yy_hh:mm:ss");
                        if (lastModifDate != currentDate)
                        {
                            oFileToUpdate.append(filePath);
                        }
                    }
                    else
                        oFileToRemove.append(filePath);
                }
            }
            // This is a security, in case of new file undected to be added
            foreach(QString filePath, iList)
            {
                if (!filePath.isEmpty())
                    if (!FileLastModifTable.contains(filePath))
                        oFileToUpdate.append(filePath);
            }
            rc = true;
        }
    }
    return rc;
}

/* This function allow to remove any files from the database
* @input: iList - List contianing the files to be removed
* @return: bool (condition on the global message in the Log Window)
*/
bool GTADataController::removeParameterInFiles(const QStringList& iList)
{
    bool rc = false;
    GTASystemServices* pSystemService = GTAAppController::getGTAAppController()->getSystemService();
    QString dbFile = pSystemService->getDatabaseFile();

    rc = GTALocalDbUtil::removeContentOfFilesFromDB(dbFile, iList);

    return rc;
}

/* This function allow to get all the details of a Parameter
* @input: iParamName - Parameter name to search in Database
* @output: paramDetail - Struct containing all the details of the Parameter
* @return: bool
*/
bool GTADataController::getParameterDetail(const QString& iParamName, const QString&, GTAICDParameter& paramDetail)
{
    bool rc = false;
    QSqlDatabase db = GTALocalDbUtil::getParamDatabase();
    if (db.isOpen())
    {
        // Modify the Query by adding the UPPER condition on the NAME column for a research
        QString SelectQuery = QString("SELECT * FROM %1 WHERE UPPER(NAME) LIKE '%2';").arg(PARAMETER_TABLE, iParamName);
        QSqlQuery getParamQ(SelectQuery, db);

        if (getParamQ.next())
        {
            paramDetail.setName(getParamQ.value(0).toString());
            paramDetail.setParameterType(getParamQ.value(1).toString());
            paramDetail.setSignalType(getParamQ.value(2).toString());
            paramDetail.setUnit(getParamQ.value(3).toString());
            paramDetail.setValueType(getParamQ.value(4).toString());
            paramDetail.setMinValue(getParamQ.value(5).toString());
            paramDetail.setMaxValue(getParamQ.value(6).toString());
            paramDetail.setLabel(getParamQ.value(7).toString());
            paramDetail.setParameterLocalName(getParamQ.value(8).toString());
            paramDetail.setBit(getParamQ.value(9).toString());
            paramDetail.setEquipmentName(getParamQ.value(10).toString());
            paramDetail.setSourceFile(getParamQ.value(11).toString());
            paramDetail.setLastModifiedDate(getParamQ.value(12).toString());
            paramDetail.setMessageName(getParamQ.value(13).toString());
            paramDetail.setFSName(getParamQ.value(14).toString());
            paramDetail.setSuffix(getParamQ.value(15).toString());
            paramDetail.setPrecision(getParamQ.value(16).toString());
            paramDetail.setDirection(getParamQ.value(17).toString());
            paramDetail.setMediaType(getParamQ.value(18).toString());
            getParamQ.clear();
            rc = true;
        }
        else
        {
            rc = false;
            _LastError = getParamQ.lastError().text();
        }
    }
    else
    {
        rc = false;
        _LastError = GTALocalDbUtil::getLastError();
    }

    return rc;
}

bool GTADataController::getParametersDetail(const QStringList& iParamNames, const QString&, QHash<QString, GTAICDParameter>& oParamDetails, QStringList& oInvalidParams)
{
    bool rc = false;
    QSqlDatabase db = GTALocalDbUtil::getParamDatabase();
    bool isNotOpen = false;
    bool containsUpperName = false;
    if (!db.isOpen())
    {
        isNotOpen = true;
        db.open();
    }
    if (db.isOpen())
    {
        rc = false;
        QString QueryUpperName = QString("SELECT COUNT(*) AS Cmpt FROM pragma_table_info('%1') WHERE name='%2'").arg(PARAMETER_TABLE, "UPPERNAME");
        QSqlQuery getQ(QueryUpperName, db);
        if (getQ.value(0) != 0)
            containsUpperName = true;

        oInvalidParams = iParamNames;
        QStringList loopHeaderItems;

        //incorporating loop parameters - foreach implementation
        //implementation to be changed by pushing in a seperate TABLE in DB [Future]
        QRegExp regExp("_uuid\\[\\{[a-z0-9]{8}-([a-z0-9]{4}-){3}[a-z0-9]{12}\\}\\]");

        QStringList paramItems;
        foreach(QString paramName, iParamNames)
        {
            if (paramName.contains(regExp))
            {
                loopHeaderItems.append(paramName);
            }

            paramName.replace("'", "");
            if (containsUpperName)
                paramItems.append("UPPERNAME='" + paramName.toUpper() + "'");
            else
                paramItems.append("UPPER(NAME)='" + paramName + "'");
        }
        QString whereClause = paramItems.join(" OR ");
        QString selectQuery = QString("SELECT * FROM %1 WHERE %2;").arg(PARAMETER_TABLE, whereClause);
        QSqlQuery getParamQ(selectQuery, db);
        GTAICDParameter paramDetail;

        while (getParamQ.next())
        {
            rc = true;
            QString paramName = getParamQ.value(0).toString();
            paramDetail.setName(paramName);
            paramDetail.setUpperName(getParamQ.value(1).toString());
            paramDetail.setParameterType(getParamQ.value(2).toString());
            paramDetail.setSignalType(getParamQ.value(3).toString());
            paramDetail.setUnit(getParamQ.value(4).toString());
            paramDetail.setValueType(getParamQ.value(5).toString());
            paramDetail.setMinValue(getParamQ.value(6).toString());
            paramDetail.setMaxValue(getParamQ.value(7).toString());
            paramDetail.setLabel(getParamQ.value(8).toString());
            paramDetail.setParameterLocalName(getParamQ.value(9).toString());
            paramDetail.setBit(getParamQ.value(10).toString());
            paramDetail.setEquipmentName(getParamQ.value(11).toString());
            paramDetail.setSourceFile(getParamQ.value(12).toString());
            paramDetail.setLastModifiedDate(getParamQ.value(13).toString());
            paramDetail.setMessageName(getParamQ.value(14).toString());
            paramDetail.setFSName(getParamQ.value(15).toString());
            paramDetail.setSuffix(getParamQ.value(16).toString());
            paramDetail.setPrecision(getParamQ.value(17).toString());
            paramDetail.setDirection(getParamQ.value(18).toString());
            paramDetail.setMediaType(getParamQ.value(19).toString());
            paramDetail.setToolName(getParamQ.value(20).toString());
            paramDetail.setToolType(getParamQ.value(21).toString());
            paramDetail.setRefreshRate(getParamQ.value(22).toString());

            oParamDetails.insert(paramName,paramDetail);
            
            //if(oInvalidParams.contains(paramName))
            //{
                QString paramNameUpper = paramName.toUpper();
                for (const auto& param : oInvalidParams)
                {
                    if (param.toUpper() == paramNameUpper)
                        oInvalidParams.removeAll(param);
                }
            //}
        }
        
        if(!rc)
        {
            rc = false;
            _LastError = getParamQ.lastError().text();
        }
        getParamQ.clear();
    }
    if (isNotOpen && db.isOpen())
        db.close();

    return rc;
}


bool GTADataController::getFwcData(QList<GTAFWCData>& oFwcData/*,const QString & iProgram, const QString & iEquiment, const QString & iStandard*/) const
{
    GTASystemServices* pSystemService = GTAAppController::getGTAAppController()->getSystemService();
    QString fwcBDSFilePath = pSystemService->getFwcBDSFilePath();
    QDir fwcDir(fwcBDSFilePath);
    QStringList bdsFiles = fwcDir.entryList(QStringList("*.txt"));
    bool rc = false;
    foreach(QString bdsFile, bdsFiles)
    {
        rc |= GTALocalDbUtil::parseFWCBDSFile(fwcBDSFilePath + QDir::separator() + bdsFile, oFwcData);
    }
    return rc;
}

bool GTADataController::fillQuery(const QString&, const QString& iQuery, QHash<QString, GTAICDParameter>& iParamList)
{
    bool rc = false;
    QSqlDatabase db = GTALocalDbUtil::getParamDatabase();
    if (db.isOpen())
    {

        QSqlQuery getParamQ(iQuery, db);
        rc = false;
        QString serror = getParamQ.lastError().text();

        while (getParamQ.next())
        {
            GTAICDParameter paramDetail;
            rc = true;
            QString paramName = getParamQ.value(0).toString();
            paramDetail.setName(paramName);
            paramDetail.setUpperName(getParamQ.value(1).toString());
            paramDetail.setParameterType(getParamQ.value(2).toString());
            paramDetail.setSignalType(getParamQ.value(3).toString());
            paramDetail.setUnit(getParamQ.value(4).toString());
            paramDetail.setValueType(getParamQ.value(5).toString());
            paramDetail.setMinValue(getParamQ.value(6).toString());
            paramDetail.setMaxValue(getParamQ.value(7).toString());
            paramDetail.setLabel(getParamQ.value(8).toString());
            paramDetail.setParameterLocalName(getParamQ.value(9).toString());
            paramDetail.setBit(getParamQ.value(10).toString());
            paramDetail.setEquipmentName(getParamQ.value(11).toString());
            paramDetail.setSourceFile(getParamQ.value(12).toString());
            paramDetail.setLastModifiedDate(getParamQ.value(13).toString());
            paramDetail.setMessageName(getParamQ.value(14).toString());
            paramDetail.setFSName(getParamQ.value(15).toString());
            paramDetail.setSuffix(getParamQ.value(16).toString());
            paramDetail.setPrecision(getParamQ.value(17).toString());
            paramDetail.setDirection(getParamQ.value(18).toString());
            paramDetail.setMediaType(getParamQ.value(19).toString());
            paramDetail.setToolName(getParamQ.value(20).toString());
            paramDetail.setToolType(getParamQ.value(21).toString());
            paramDetail.setRefreshRate(getParamQ.value(22).toString());
            if(!iParamList.contains(paramName))
            {
                iParamList.insert(paramName, paramDetail);
            }
        }
        serror = getParamQ.lastError().text();
        getParamQ.clear();
    }
    return rc;
}

bool GTADataController::fillQuery(const QString&, const QString& iQuery, QStringList& iParamList)
{
    bool rc = false;
    QSqlDatabase db = GTALocalDbUtil::getParamDatabase();
    if (db.isOpen())
    {
        QSqlQuery getParamQ(iQuery, db);
        rc = false;

        while (getParamQ.next())
        {
            rc = true;
            QString paramName = getParamQ.value(0).toString();
            iParamList.append(paramName);
        }
        getParamQ.clear();
    }
    return rc;
}

bool GTADataController::getAASMap(QHash<QString, QString>&)
{
    bool rc = false;
    GTASystemServices* pSystemService = GTAAppController::getGTAAppController()->getSystemService();
    QStringList aasFiles = pSystemService->getAASFiles();
    return rc;
}

bool GTADataController::commitDatabase()
{
    bool rc = GTALocalDbUtil::commitDatabase();
    return rc;
}

bool GTADataController::updateHistoryTable(const QStringList& iUpdateList)
{
    bool rc = GTALocalDbUtil::updateHistoryTable(iUpdateList);
    return rc;
}

bool GTADataController::createSessionTable(const QString& iDbFilePath)
{
    bool rc = GTALocalDbUtil::createSessionTable(iDbFilePath);
    return rc;
}


bool GTADataController::createHistoryTable(const QString& iDbFilePath)
{
    bool rc = GTALocalDbUtil::createHistoryTable(iDbFilePath);
    return rc;
}

QStringList GTADataController::getParametersFromHistory()
{
    QStringList lstVars;
    GTALocalDbUtil::getParametersFromHistory(lstVars);
    return lstVars;
}

QString GTADataController::getDatabasePath()const
{
    QString path;
    path = GTALocalDbUtil::getDatabasePath();
    return path;
}

QString GTADataController::getDatabaseVersion()
{
    QString version = GTALocalDbUtil::getDatabaseVersion();
    return version;
}
QStringList GTADataController::getUpdateQueryListForEquipments()
{
    QStringList queryList;
    queryList = GTALocalDbUtil::getUpdateQueryListForEquipments();
    return queryList;
}

bool GTADataController::updateEquipmentData(const QStringList& iQueryList)
{
    bool rc = GTALocalDbUtil::updateEquipmentData(iQueryList);
    return rc;
}

bool GTADataController::saveSession(const QString& iSessionName, const QStringList& iFileNames)
{
    bool rc = GTALocalDbUtil::saveSession(iSessionName, iFileNames);
    return rc;
}

bool GTADataController::addNewSession(const QString& iSessionName, const QStringList& iFileNames)
{
    bool rc = GTALocalDbUtil::addNewSession(iSessionName, iFileNames);
    return rc;
}

bool GTADataController::deleteSession(const QStringList& iSelectedItems)
{
    bool rc = GTALocalDbUtil::deleteSession(iSelectedItems);
    return rc;
}

QStringList GTADataController::getSessionNames()
{
    return GTALocalDbUtil::getSessionNames();
}

QStringList GTADataController::getSessionFiles(const QString& iSessionName)
{
    return GTALocalDbUtil::getSessionFiles(iSessionName);
}

bool GTADataController::commitUserDatabase()
{
    bool rc = GTALocalDbUtil::commitUserDatabase();
    return rc;
}

void GTADataController::closeUserDatabase()
{
    GTALocalDbUtil::closeUserDatabase();
}