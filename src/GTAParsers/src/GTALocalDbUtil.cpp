#include "GTALocalDbUtil.h"
#include "GTASqliteDriver.h"
#include "GTAUtil.h"
#include "GTAICDParameter.h"
#include "GTAEquipmentMap.h"

#pragma warning(push, 0)
#include <QtConcurrentMap>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrentFilter>
#include <QtConcurrentRun>
#include <QFile>
#include <QTextStream>
#include <QRegExp>
#include <QDomDocument>
#include <QMutexLocker>
#include <QFutureWatcher>
#include <QtConcurrentRun>
#include <QtConcurrentMap>
#include <QtConcurrentFilter>
#include <QXmlStreamReader>
#include <QFileInfo>
#include <QDateTime>
#include <QTextCodec>
#include <QDir>
#include <QSqlQuery>
#include "qsqlerror.h"
#include <QString>
#include <QVariantList>
#include <QSet>
#include <QStringList>
#include <QSqlQueryModel>
#include "sqlite3.h"
#include <QSqlRecord>
#pragma warning(pop)

#define A429_OUTPUT_LABEL "A429_OUTPUT_LABEL"
#define A429_INPUT_LABEL "A429_INPUT_LABEL"
#define ANALOGUE_OUTPUT_SIGNAL "ANALOGUE_OUTPUT_SIGNAL"
#define ANALOGUE_INPUT_SIGNAL "ANALOGUE_INPUT_SIGNAL"
#define DISCRETE_OUTPUT_SIGNAL "DISCRETE_OUTPUT_SIGNAL"
#define DISCRETE_INPUT_SIGNAL "DISCRETE_INPUT_SIGNAL"
#define CAN_OUTPUT_MESSAGE "CAN_OUTPUT_MESSAGE"
#define CAN_INPUT_MESSAGE "CAN_INPUT_MESSAGE"
#define AFDX_INPUT_MESSAGE "AFDX_INPUT_MESSAGE"
#define AFDX_OUTPUT_MESSAGE "AFDX_OUTPUT_MESSAGE"
#define MIL1553_INPUT_DATA "MIL1553_INPUT_DATA"
#define MIL1553_OUTPUT_DATA "MIL1553_OUTPUT_DATA"
#define MIL1553_MESSAGE "MIL1553_MESSAGE"

#define PMRNODE "PMR"
#define MODEL_ICD "ICDL"
#define VCOM_MESSAGES "Messages"
#define VARIABLES_KEY "Variables"
#define INTERFACES "Interfaces"
#define BUSES "Buses"
#define BISG "Bisg"
#define NAME_KEY "name"
#define MAPPING "mapping"
#define TYPE_KEY "Type"
#define EQUIPMENTS_KEY "Equipments"
#define EQUIPMENT_KEY "Equipement"
#define INTERFACE_KEY "Interface"
#define BUS_KEY "Bus"
#define PARAMETERS_KEY "Parameters"
#define TABLE_NAME "PARAMETERS"

//Instead of using external PARAMETERS.csv, the following QStringList contains the column names of the ParameterDB model 
QStringList GTALocalDbUtil::ParameterDBComponents = {
     DB_NAME,
     DB_UPPER_NAME,
     DB_PARAM_TYPE,
     DB_SIGNAL_TYPE,
     DB_UNIT,
     DB_VALUE_TYPE,
     DB_MNVAL,
     DB_MXVAL,
     DB_LABEL,
     DB_PARAMETER_LOCAL_NAME,
     DB_BIT_POS,
     DB_EQUIPNAME,
     DB_FILE,
     DB_FILE_LAST_MODIFIED,
     DB_MESSAGE,
     DB_FS_NAME,
     DB_SUFFIX,
     DB_PRECISION,
     DB_DIRECTION,
     DB_MEDIA_TYPE,
     DB_TOOL_NAME,
     DB_TOOL_TYPE,
     DB_REFRESH_RATE
};

QFile   GTALocalDbUtil::_DbWriterFile;
QSqlDatabase GTALocalDbUtil::_ParamDb;
QSqlDatabase GTALocalDbUtil::_UserDb;
QString GTALocalDbUtil::_UserDbFilePath;
QList<GTAICDParameter> GTALocalDbUtil::_NewParameterList;
QList<QString> GTALocalDbUtil::_FileModifiedList;
QString GTALocalDbUtil::_ErrMessage;
QHash<QString, QString> GTALocalDbUtil::_currentBase;

QHash<GTALocalDbUtil::ParseFlag,bool> GTALocalDbUtil::_ParsingFalgeData;
QHash<QString,QString> GTALocalDbUtil::_MIL1553MessageBusMap;
QString GTALocalDbUtil::_ParamDbFilePath;
QString GTALocalDbUtil::_currentToolIdToUse;

GTALocalDbUtil::GTALocalDbUtil()
{
    setDefaultParseFlag();
	_currentToolIdToUse = QString();
}

bool GTALocalDbUtil::openUserDatabase(const QString &iFile)
{
    bool rc;
    QFileInfo fileInfo(iFile);
    rc = fileInfo.exists();
    if(rc)
    {
        _UserDb = QSqlDatabase::addDatabase("QSQLITE","GTAApp_USERDB");
        _UserDb.setDatabaseName(iFile);
        _UserDbFilePath = iFile;
        //bool val = _UserDb.isValid();
        rc = _UserDb.open();
        if(!rc)
        {
            QString error;
            error = _UserDb.lastError().text();
        }
    }
    return rc;
}

bool GTALocalDbUtil::commitUserDatabase()
{
    bool rc = true;
    if(_UserDb.isOpen())
    {
        if(! _UserDb.commit())
        {
            rc = false;
        }
    }
	return rc;
}

void GTALocalDbUtil::closeUserDatabase()
{
    if(_UserDb.isOpen())
    {
        _UserDb.close();
    }
}


bool GTALocalDbUtil::openDatabase(const QString & iFile)
{
    bool rc;
    QFileInfo fileInfo(iFile);
    rc = fileInfo.exists();
    if(rc)
    {
        _ParamDb = QSqlDatabase::addDatabase("QSQLITE","GTAApp_PARAMDB");
        _ParamDb.setDatabaseName(iFile);
        _ParamDbFilePath = iFile;

        rc = _ParamDb.open();
        if(!rc)
        {
            QString error;
            error = _ParamDb.lastError().text();
        }
    }
    return rc;
}



bool GTALocalDbUtil::commitDatabase()
{
    bool rc = true;
    if(_ParamDb.isOpen())
    {
        if(! _ParamDb.commit())
        {
            rc = false;
        }
    }
	return rc;
}

void GTALocalDbUtil::closeDatabase()
{
    if(_ParamDb.isOpen())
    {
        _ParamDb.close();
    }
    _currentBase.clear();
}


/* This function allow to insert, update or delete signals depending on their state
* @ input: iLabraryPath - Path containing the current PARAMETER_DB file
*/
void GTALocalDbUtil::insertParameterInDatabase(const QString& iLibraryPath)
{
    QStringList queries;
    QStringList tables = _ParamDb.tables();
    // Template Queries
	QString queryTmpl = QString("INSERT INTO %1(%2) VALUES ").arg(PARAMETER_TABLE, ParameterDBComponents.join(","));
	QString queryTmplUpdate = QString("UPDATE %1 SET ").arg(PARAMETER_TABLE);
	QString queryTmplDelete = QString("DELETE FROM %1 WHERE ").arg(PARAMETER_TABLE);
    
	int counter = 0;
    QStringList toBeInserted;
	QStringList toBeUpdated;
	QStringList toBeDeleted;
	QString newDate = QString();
	
	// Contains all current Parameters
    QHash<QString, QString> currentParamInDB;
	foreach(QString fileName, _currentBase.keys())
        currentParamInDB.insert(fileName, _currentBase.value(fileName));
	
	if (tables.contains(PARAMETER_TABLE))
	{
		bool isParamAlreadyInDB = false;
		// _NewParameterList contains all parameters to be updated (Name / Name,DateTime)
		for (int i = 0; i < _NewParameterList.count(); i++)
		{
			GTAICDParameter Parameter = _NewParameterList.at(i);

			// Get the Name AND the SourceFile of the parameter
			QString paramName = Parameter.getName();
			QString sourceFile = Parameter.getSourceFile();
			sourceFile = sourceFile.replace("'", "");
			newDate = sourceFile;

			if (!_FileModifiedList.contains(sourceFile))
			{
				_FileModifiedList.append(sourceFile);
			}

			// Check if the parameter is already in Database
			isParamAlreadyInDB = _currentBase.contains(paramName);

			if (!isParamAlreadyInDB)
			{
				// ToBeINSERTED
				_currentBase.insert(paramName, sourceFile);
			}
            QString paramUpperName = Parameter.getUpperName();
			QString paramType = Parameter.getParameterType();
			QString signalType = Parameter.getSignalType();
			QString unit = Parameter.getUnit();
			QString valueType = Parameter.getValueType();

			if (valueType.toLower() == "enumerate")
				valueType = "Integer";
			if (valueType.toLower() == "char")
				valueType = "String";
			if (valueType.toLower() == "float")
				valueType = "Float";
			if (valueType.toLower() == "string")
				valueType = "String";
			if (valueType.toLower() == "boolean")
				valueType = "Boolean";
			if (valueType.toLower() == "integer")
				valueType = "Integer";
			if (paramType.toLower().contains("vmac"))
				valueType = "Float";

			QString minVal = Parameter.getMinValue();
			minVal = minVal.replace("'", "");

			QString maxVal = Parameter.getMaxValue();
			maxVal = maxVal.replace("'", "");

			QString label = Parameter.getLabel();
			//QString identifier = Parameter.getIdentifier();
			QString bit = Parameter.getBit();
			QString equipName = Parameter.getEquipmentName();

			/*QString sourceFileType = QString();// Parameter.getSourceType();
			if (sourceFile.endsWith(".csv"))
				sourceFileType = "ICD";
			if (sourceFile.endsWith(".xml"))
				sourceFileType = "PMR";
			if (sourceFile.endsWith(".pir"))
				sourceFileType = "PIR";*/
			
			bool rcFile = false;
			QFileInfo fileInfo(iLibraryPath + sourceFile);
			rcFile = fileInfo.exists();
			QString fileLastModified = QString("No_value");
			if (rcFile)
			{
				QDateTime buildTime = fileInfo.lastModified();
				fileLastModified = buildTime.toString("'M'M'd'd'y'yy_hh:mm:ss");
			}
			/*QString application = Parameter.getApplicationName();
			QString busName = Parameter.getBusName();
			QString signalName = Parameter.getSignalName();
			QString media = Parameter.getMedia();
			media = media.replace("-", "_");*/
			QString direction = Parameter.getDirection();
			QString message = Parameter.getMessageName();
			QString ParameterLocalName = Parameter.getParameterLocalName();
			QString FSName = Parameter.getFSName();
			QString suffix = Parameter.getSuffix();
			QString precision = Parameter.getPrecision();
			/*QString tempName = Parameter.getTempParamName();
			tempName = tempName.replace("-", "_");*/
			QString mediaType = Parameter.getMediaType();
			QString toolName = GTALocalDbUtil::_currentToolIdToUse;
			if (!toolName.isEmpty() && toolName != "NA" && toolName != Parameter.getToolName())
			{
				toolName = GTALocalDbUtil::_currentToolIdToUse;
			}
			else
			{
				toolName = Parameter.getToolName();
			}
			QString toolType = Parameter.getToolType();
			QString refreshRate = Parameter.getRefreshRate();
			//QString PossibleValues = Parameter.getPossibleValues();

			// Query to INSERT new values
			if (!isParamAlreadyInDB)
			{
				QString vals1 = QString("\"%1\",\"%2\",\"%3\",\"%4\",\"%5\",\"%6\",\"%7\",\"%8\",\"%9\",").arg(paramName,paramUpperName, paramType, signalType, unit, valueType, minVal, maxVal, label);
				QString vals2 = QString("\"%1\",\"%2\",\"%3\",\"%4\",\"%5\",").arg(/*identifier*/ParameterLocalName, bit, equipName, sourceFile, fileLastModified/*, sourceFileType, application, busName*/);
				QString vals3 = QString("\"%1\",\"%2\",\"%3\",\"%4\",\"%5\",").arg(/*signalName, media, */message, /*ParameterLocalName, */FSName, suffix, precision, /*tempName, */direction);
				QString vals4 = QString("\"%1\",\"%2\",\"%3\",\"%4\"").arg(mediaType, toolName, toolType, refreshRate/*, PossibleValues*/);

				QString vals = vals1 + vals2 + vals3 + vals4;
				toBeInserted.append("(" + vals + ")");
			}
			else // Query to UPDATE fileLastModified field
			{
				QStringList valuesList;
				valuesList << fileLastModified;
				toBeUpdated.clear();
				toBeUpdated.append(fileLastModified);
			}

			// If we manipulate a lot of data - Need to concatenate Queries
			if (counter >= 1500)
			{
				QString insert = QString("%1 %2").arg(queryTmpl, toBeInserted.join(","));
				counter = 0;
				queries.append(insert);
				toBeInserted.clear();
			}
			counter++;
		}

		QList<QString> elmToBeRemoved;
		// _currentBase contains all parameters to be updated (Name / Name,DateTime)
		QHashIterator<QString, QString> i(_currentBase);
		while (i.hasNext())
		{
			i.next();
			// Get the Name & SourceFile of the parameter
			QString paramName = i.key();
			QString sourceFile = i.value();
			sourceFile = sourceFile.replace("'", "");

			if (currentParamInDB.contains(paramName) && _FileModifiedList.contains(sourceFile))
			{
				elmToBeRemoved.append(paramName);
			}
		}
		
		// _currentBase contains all parameters to be updated (Name / Name,DateTime)
		for (int j = 0; j < _NewParameterList.count(); j++)
		{
			GTAICDParameter Parameter = _NewParameterList.at(j);

			// Get the Name of the parameter
			QString paramName = Parameter.getName();

			if (elmToBeRemoved.contains(paramName))
				elmToBeRemoved.removeOne(paramName);
		}
		foreach(QString elm, elmToBeRemoved)
		{
			// We only have parameters to be removed
			toBeDeleted.append(elm);

			_currentBase.remove(elm);
		}

        if(counter>0)
        {
			// Delete first
			if (!toBeDeleted.isEmpty())
			{
				elmToBeRemoved.clear();
				foreach(QString name, toBeDeleted)
				{
					QString delet = QString("%1 NAME = '%2'").arg(queryTmplDelete, name);
					counter = 0;
					queries.append(delet);
				}
				toBeDeleted.clear();
			}
			// Then Insert new elements
			if (!toBeInserted.isEmpty())
			{
				QString insert = QString("%1 %2").arg(queryTmpl,toBeInserted.join(","));
				counter = 0;
				queries.append(insert);
				toBeInserted.clear();
			}
			// Update the fileLastModified field for the files
			if(!toBeUpdated.isEmpty()) 
			{
				QString update = QString("%1 FILELASTMODIFIED = '%2' WHERE FILE LIKE '%%3'").arg(queryTmplUpdate, toBeUpdated.value(0), newDate.replace("/","_"));
				counter = 0;
				queries.append(update);
				toBeUpdated.clear();
			}
        }
        GTASqliteDriver::executeSqliteQuery(_ParamDbFilePath,queries);
        //TODO: Possibly add error handling for executeSqliteQuery

        _NewParameterList.clear();
		_FileModifiedList.clear();
    }
}


void GTALocalDbUtil::setDefaultParseFlag()
{
    _ParsingFalgeData[A429OUT] = true;
    _ParsingFalgeData[A429IN] = true;
    _ParsingFalgeData[ANAOUT] = true;
    _ParsingFalgeData[ANAIN] = true;
    _ParsingFalgeData[DESCOUT] = true;
    _ParsingFalgeData[DESCIN] = true;
    _ParsingFalgeData[CANOUT] = true;
    _ParsingFalgeData[CANIN] = true;
    _ParsingFalgeData[AFDXOUT] = true;
    _ParsingFalgeData[AFDXIN] = true;
    _ParsingFalgeData[PMRF] = true;
    _ParsingFalgeData[MODELF] = true;
    _ParsingFalgeData[VCOMSTATIC] = true;
    _ParsingFalgeData[PIRF] = true;
    _ParsingFalgeData[MIL_1553_DATA] = true;
    _ParsingFalgeData[MIL_1553_MESSAGE] = true;
}
void GTALocalDbUtil::setParseFlag(const QHash<ParseFlag, bool> &iFlags)
{
    _ParsingFalgeData = iFlags;
}

void GTALocalDbUtil::setICDValidFields()
{
    /*_ValidICDFields<<A429_OUTPUT_LABEL<<A429_INPUT_LABEL<<ANALOGUE_OUTPUT_SIGNAL;
    _ValidICDFields<<ANALOGUE_INPUT_SIGNAL<<DISCRETE_OUTPUT_SIGNAL<<DISCRETE_INPUT_SIGNAL;
    _ValidICDFields<<CAN_OUTPUT_MESSAGE<<AFDX_INPUT_MESSAGE<<AFDX_OUTPUT_MESSAGE<<CAN_INPUT_MESSAGE;
    _ValidICDFields<<MIL1553_INPUT_DATA<<MIL1553_MESSAGE<<MIL1553_OUTPUT_DATA;*/
}

/* This function handle to add parameters from files according to its extension
* @input: iFileToParse - QString
* @input: iFileType - QString 
* @input: iLibraryPath - QString
* @input: iToolId - QString containing the current ToolId to use
* @return: bool
*
*/
bool GTALocalDbUtil::updateParameterInFile(const QString & iFileToParse, const QString & iFileType, const QString& iLibraryPath, const QString& iToolId)
{
    bool rc = false;
    _NewParameterList.clear();
	_FileModifiedList.clear();
	setToolIdToUse(iToolId);

    if ((iFileToParse.endsWith(".csv", Qt::CaseInsensitive)) && (iFileType == ICD_DIR))
    {
        rc = updateDbFromIcdCsvFile(iFileToParse, iLibraryPath);
    }
    else if ((iFileToParse.endsWith(".xml", Qt::CaseInsensitive)) && (iFileType == PMR_DIR))
    {
        rc = updateDbFromPmrXmlFile(iFileToParse,iLibraryPath);
    }
    else if ((iFileToParse.endsWith(".pir", Qt::CaseInsensitive)) && (iFileType == PIR_DIR))
    {
        rc = updateDbFromPirFile(iFileToParse,iLibraryPath);
    }
    else if ((iFileToParse.endsWith(".xml", Qt::CaseInsensitive)) && (iFileType == MODEL_ICD_DIR))
    {
        rc = getParameterFromModelICD(iFileToParse,iLibraryPath);
    }
    else if ((iFileToParse.endsWith(".xml", Qt::CaseInsensitive)) && (iFileType == VCOM_DIR))
    {
        rc = getParameterFromVCOM(iFileToParse,iLibraryPath);
    }
    else
    {
        _ErrMessage = "Error: GTALocalDbUtil: Invalid file , fail to parse";
    }

    if(rc)
    {
        insertParameterInDatabase(iLibraryPath);
    }
	else
	{
		_ErrMessage = "Error: GTALocalDbUtil: Invalid content in file , failed to parse";
	}
    return rc;
}


QString GTALocalDbUtil::getLastError()
{
    return _ErrMessage;
}

QSqlDatabase GTALocalDbUtil::getParamDatabase()
{
    if(!_ParamDb.isOpen())
    {
        _ParamDb.open();
    }
    return _ParamDb;
}

bool GTALocalDbUtil::updateDbFromIcdCsvFile(const QString& iFileToParse, const QString& iLibraryPath)
{
    bool rc = false;
    QFile fileObj(iLibraryPath+iFileToParse);
    rc = fileObj.open(QFile::ReadOnly | QFile::Text);

    if (! rc)
    {
        _ErrMessage = "Error: Fail to open file " + QString(iFileToParse);
        return rc;
    }

    bool IsGeneration = false;
    if(iFileToParse.contains("Generation"))
        IsGeneration = true;
    QString equipmentName;
    _MIL1553MessageBusMap.clear();
	QStringList _ValidICDFields = { "A429_OUTPUT_LABEL","A429_INPUT_LABEL","ANALOGUE_OUTPUT_SIGNAL","ANALOGUE_INPUT_SIGNAL","DISCRETE_OUTPUT_SIGNAL","DISCRETE_INPUT_SIGNAL","CAN_OUTPUT_MESSAGE","AFDX_INPUT_MESSAGE","AFDX_OUTPUT_MESSAGE","CAN_INPUT_MESSAGE","MIL1553_INPUT_DATA","MIL1553_MESSAGE","MIL1553_OUTPUT_DATA" };

    while(!fileObj.atEnd())
    {
        QString line = fileObj.readLine();
        if(! line.isEmpty() && (!line.startsWith("#")))
        {
            if(line.startsWith("EQUIPMENT"))
            {
                QStringList equipData = line.split(";");
                if(equipData.count() >2)
                    equipmentName = equipData.at(1);

                continue;
            }
			
            foreach(QString key, _ValidICDFields)
            {
                if( line.startsWith(key))
                {
                    GTAICDParameter Parameter;
                    bool rC = getParameterFromICD(line,Parameter);

                    if(rC)
                    {
                        QString paramName = Parameter.getName();
                        Parameter.setName(paramName);
                        Parameter.setUpperName(paramName.toUpper());
                        Parameter.setEquipmentName(equipmentName);
                        Parameter.setSourceFile(iFileToParse);
                        _NewParameterList.append(Parameter);
                        rc = true;
                    }

                    break;
                }
            }
        }
    }
    fileObj.close();
    return rc;
}

bool GTALocalDbUtil::getParameterFromICD(const QString & iICDDataLine, GTAICDParameter & oParam)
{
    bool rc = false;
    if(! iICDDataLine.isEmpty())
    {
        QString paramName;
        QString tempName;

        QStringList icdParamData = iICDDataLine.split(";",QString::KeepEmptyParts);
        QString icdType = icdParamData.at(0);

        QString appName ;
        QString associatedBus;
        QString signalName;
        QString identifier;
        QString label ="NA";
        QString signalType = "NA";
        QString valueType = "NA";
        QString bit;
        QString unit = "NA";
        QString minVal = "NA";
        QString maxVal = "NA";
        QString paramType = "FIB";
        QString media= "NA";
        QString message = "NA";
        QString ParameterLocalName = "NA";
        QString fsName;
        QString precision = "NA";
        QString direction = "NA";
        QString mediaType = "NA";
        QString refreshRate = "NA";
        QList<GTAICDParameterValues> possibleVals;
        int fieldCount = icdParamData.count();

        if(icdType.contains("input",Qt::CaseInsensitive))
        {
            direction = "INPUT";
        }
        else if(icdType.contains("output",Qt::CaseInsensitive))
        {
            direction = "OUTPUT";
        }

        if (_ParsingFalgeData[A429OUT] && icdType == A429_OUTPUT_LABEL )
        {

            media="A429";
            message=icdParamData.at(5).simplified();
            appName = icdParamData.at(1).simplified();
            associatedBus = icdParamData.at(4).simplified();
            signalName = icdParamData.at(36).simplified();
            //identifier = icdParamData.at(27);
            label = icdParamData.at(6);
            valueType = icdParamData.at(37);
            fsName = icdParamData.at(5).simplified();
            mediaType = icdParamData.at(11);
            ParameterLocalName = icdParamData.at(27).simplified();
            refreshRate = icdParamData.at(7).simplified();
            signalType = QString("ARINC_%1").arg(valueType);
            if((valueType == BOOLEAN_TYPE) && (fieldCount >= 46))
            {
                bit = icdParamData.at(41);
                minVal = icdParamData.at(46); //bool false definition
                maxVal = icdParamData.at(45); //bool true definition
                GTAICDParameterValues val1,val2;
                val1.key = QString("false");
                val1.value = minVal;

                val2.key = QString("true");
                val2.value = maxVal;

                possibleVals.append(val1);
                possibleVals.append(val2);
            }
            else if((valueType == FLOAT_TYPE) && (fieldCount >= 56))
            {
                //bit = icdParamData.at(41);
                unit = icdParamData.at(56);
                minVal = icdParamData.at(54);
                maxVal = icdParamData.at(55);
                GTAICDParameterValues val1,val2;
                val1.key = QString("Min");
                val1.value = minVal;

                val2.key = QString("Max");
                val2.value = maxVal;

                possibleVals.append(val1);
                possibleVals.append(val2);
            }
            else if((valueType == INTERGER_TYPE) && (fieldCount >= 66))
            {
                unit = icdParamData.at(66);
                minVal = icdParamData.at(64);
                maxVal = icdParamData.at(65);
                GTAICDParameterValues val1,val2;
                val1.key = QString("Min");
                val1.value = minVal;

                val2.key = QString("Max");
                val2.value = maxVal;

                possibleVals.append(val1);
                possibleVals.append(val2);
            }
            else if((valueType == ENUMERATE_TYPE) && (fieldCount >= 53))
            {
                QString enumVal = icdParamData.at(53);
                QList<GTAICDParameterValues> lst = parseEnumerationValues(enumVal);
                possibleVals.append(lst);
            }

            if(valueType.toLower() == "float")
            {
                precision = icdParamData.at(57).simplified();
            }
            if(valueType.toLower() == "integer")
            {
                precision = icdParamData.at(67).simplified();
            }
            else
                precision= "NA";
        }
        else if(_ParsingFalgeData[A429IN] && icdType == A429_INPUT_LABEL)
        {
            media="A429";
            message=icdParamData.at(5).simplified();
            appName = icdParamData.at(1).simplified();
            associatedBus = icdParamData.at(4).simplified();
            signalName = icdParamData.at(37).simplified();
            fsName = icdParamData.at(5).simplified();
            //identifier = icdParamData.at(27);
			ParameterLocalName = icdParamData.at(27).simplified();
            label = icdParamData.at(6);
            valueType = icdParamData.at(38);
            signalType = QString("ARINC_%1").arg(valueType);
            mediaType = icdParamData.at(12);
            refreshRate = icdParamData.at(7).simplified();
            if((valueType == BOOLEAN_TYPE) && (fieldCount >=46))
            {
                bit = icdParamData.at(41);
                unit = "NA";
                minVal = icdParamData.at(46); // bool false defintion
                maxVal = icdParamData.at(45); // bool true definition

                GTAICDParameterValues val1,val2;
                val1.key = QString("false");
                val1.value = minVal.trimmed().isEmpty()?"0":minVal;

                val2.key = QString("true");
                val2.value = maxVal.trimmed().isEmpty()?"1":maxVal;

                possibleVals.append(val1);
                possibleVals.append(val2);

            }
            else if((valueType == FLOAT_TYPE) && (fieldCount >=56))
            {
                unit = icdParamData.at(57);
                minVal = icdParamData.at(54);
                maxVal = icdParamData.at(55);
                GTAICDParameterValues val1,val2;
                val1.key = QString("Min");
                val1.value = minVal;

                val2.key = QString("Max");
                val2.value = maxVal;

                possibleVals.append(val1);
                possibleVals.append(val2);
            }
            else if((valueType == INTERGER_TYPE) && (fieldCount >=66))
            {
                unit = icdParamData.at(67);
                minVal = icdParamData.at(64);
                maxVal = icdParamData.at(65);
                GTAICDParameterValues val1,val2;
                val1.key = QString("Min");
                val1.value = minVal;

                val2.key = QString("Max");
                val2.value = maxVal;

                possibleVals.append(val1);
                possibleVals.append(val2);
            }
            else if((valueType == ENUMERATE_TYPE) && (fieldCount >= 54))
            {
                QString enumVal = icdParamData.at(54);
                QList<GTAICDParameterValues> lst = parseEnumerationValues(enumVal);
                possibleVals.append(lst);
            }
            ParameterLocalName = icdParamData.at(28).simplified();
            if(valueType.toLower() == "float")
            {
                precision = icdParamData.at(58).simplified();
            }
            if(valueType.toLower() == "integer")
            {
                precision = icdParamData.at(68).simplified();
            }
            else
                precision= "NA";
        }
        else if(fieldCount > 20 && (_ParsingFalgeData[ANAOUT] || _ParsingFalgeData[ANAIN])
                && ( (icdType == ANALOGUE_OUTPUT_SIGNAL)
                     || (icdType == ANALOGUE_INPUT_SIGNAL) ) )
        {
            media="ANA";
            appName = icdParamData.at(1).simplified();
            associatedBus = icdParamData.at(4).simplified();
            signalName = icdParamData.at(20).simplified();
            //identifier = icdParamData.at(20);
			ParameterLocalName = icdParamData.at(20).simplified();
            label = "NA";
            signalType = "ANALOGUE";
            valueType = "Float";
            unit = icdParamData.at(18);
            minVal = icdParamData.at(16);
            maxVal = icdParamData.at(17);
            GTAICDParameterValues val1,val2;
            val1.key = QString("Min");
            val1.value = minVal;

            val2.key = QString("Max");
            val2.value = maxVal;

            possibleVals.append(val1);
            possibleVals.append(val2);

            ParameterLocalName = icdParamData.at(6).simplified();
            paramType = "FIB";
            if(valueType.toLower() == "float")
            {
                precision = icdParamData.at(19).simplified();
            }

        }
        else if(fieldCount > 16 && (_ParsingFalgeData[DESCOUT] || _ParsingFalgeData[DESCIN]) && ((icdType == DISCRETE_OUTPUT_SIGNAL) || (icdType == DISCRETE_INPUT_SIGNAL) ))
        {
            media="DIS";
            //identifier = icdParamData.at(5);
            appName = icdParamData.at(1).simplified();
            associatedBus = icdParamData.at(4).simplified();
            signalName = icdParamData.at(16).simplified();
            ParameterLocalName = icdParamData.at(5).simplified();

            label = "NA";
            signalType = "DISCRETE";
            valueType = "Boolean";
            unit = "NA";
            minVal = icdParamData.at(15);
            maxVal = icdParamData.at(14);
            GTAICDParameterValues val1,val2;
            val1.key = QString("Min");
            val1.value = minVal;

            val2.key = QString("Max");
            val2.value = maxVal;

            possibleVals.append(val1);
            possibleVals.append(val2);
        }
        else if((_ParsingFalgeData[CANOUT] && icdType == CAN_OUTPUT_MESSAGE))
        {
            media = "CAN";
            message=icdParamData.at(5).simplified();
            //identifier = icdParamData.at(18);
            appName = icdParamData.at(1).simplified();
            associatedBus = icdParamData.at(4).simplified();
            signalName = icdParamData.at(28).simplified();
            ParameterLocalName = icdParamData.at(18).simplified();
            label = "NA";
            signalType = "CAN_" + icdParamData.at(29);
            valueType = icdParamData.at(29);
            fsName = icdParamData.at(20).simplified();
            refreshRate = icdParamData.at(16).simplified();
            if(( valueType.toLower() == "float" ) && ( fieldCount >= 48))
            {
                unit = icdParamData.at(48);
                minVal = icdParamData.at(46);
                maxVal = icdParamData.at(47);

                GTAICDParameterValues val1,val2;
                val1.key = QString("Min");
                val1.value = minVal;

                val2.key = QString("Max");
                val2.value = maxVal;

                possibleVals.append(val1);
                possibleVals.append(val2);
            }
            else if(( valueType.toLower() == "integer" ) && ( fieldCount >= 58))
            {
                unit = icdParamData.at(58);
                minVal = icdParamData.at(46);
                maxVal = icdParamData.at(47);

                GTAICDParameterValues val1,val2;
                val1.key = QString("Min");
                val1.value = minVal;

                val2.key = QString("Max");
                val2.value = maxVal;

                possibleVals.append(val1);
                possibleVals.append(val2);
            }
            else if(( valueType.toLower() == "boolean" ) && ( fieldCount >= 38))
            {
                unit = "NA";
                minVal = icdParamData.at(38);  //bool false definition
                maxVal = icdParamData.at(37);  //bool true definition

                GTAICDParameterValues val1,val2;
                val1.key = QString("false");
                val1.value = minVal.trimmed().isEmpty()?"0":minVal;

                val2.key = QString("true");
                val2.value = maxVal.trimmed().isEmpty()?"1":maxVal;

                possibleVals.append(val1);
                possibleVals.append(val2);
            }
            else if((valueType == ENUMERATE_TYPE) && (fieldCount >= 45))
            {
                QString values = icdParamData.at(45);
                QList<GTAICDParameterValues> lst = parseEnumerationValues(values);
                possibleVals.append(lst);
            }
            if(valueType.toLower() == "float")
            {
                precision = icdParamData.at(49).simplified();
            }
            if(valueType.toLower() == "integer")
            {
                precision = icdParamData.at(59).simplified();
            }
            else
                precision= "NA";
        }
        else if((_ParsingFalgeData[CANIN] && icdType == CAN_INPUT_MESSAGE) )
        {
            message=icdParamData.at(5).simplified();
            media = "CAN";
            //identifier = icdParamData.at(18);
            appName = icdParamData.at(1).simplified();
            associatedBus = icdParamData.at(4).simplified();
            signalName = icdParamData.at(29).simplified();
            fsName = icdParamData.at(20).simplified();
            ParameterLocalName = icdParamData.at(18).simplified();
            label = "NA";
            valueType = icdParamData.at(30);
            signalType = "CAN_" + icdParamData.at(30);
            refreshRate = icdParamData.at(16).simplified();
            if(( valueType == "Float" ) && (fieldCount > 48))
            {
                unit = icdParamData.at(49);
                minVal = icdParamData.at(47);
                maxVal = icdParamData.at(48);
                GTAICDParameterValues val1,val2;
                val1.key = QString("Min");
                val1.value = minVal;

                val2.key = QString("Max");
                val2.value = maxVal;

                possibleVals.append(val1);
                possibleVals.append(val2);
            }
            else if(( valueType == "Boolean" ) && (fieldCount > 40))
            {
                unit = "NA";
                minVal = icdParamData.at(40);
                maxVal = icdParamData.at(41);
                GTAICDParameterValues val1,val2;
                val1.key = QString("Min");
                val1.value = minVal.trimmed().isEmpty()?"0":minVal;

                val2.key = QString("Max");
                val2.value = maxVal.trimmed().isEmpty()?"1":maxVal;

                possibleVals.append(val1);
                possibleVals.append(val2);
            }
            else if(( valueType == "Integer" ) && (fieldCount > 58))
            {
                unit = icdParamData.at(59);
                minVal = icdParamData.at(57);
                maxVal = icdParamData.at(58);
                GTAICDParameterValues val1,val2;
                val1.key = QString("Min");
                val1.value = minVal;

                val2.key = QString("Max");
                val2.value = maxVal;

                possibleVals.append(val1);
                possibleVals.append(val2);
            }
            else if((valueType == ENUMERATE_TYPE ) && (fieldCount >= 46))
            {
                QString values = icdParamData.at(46);
                QList<GTAICDParameterValues> lst = parseEnumerationValues(values);
                possibleVals.append(lst);
            }


            if(valueType.toLower() == "float")
            {
                precision = icdParamData.at(50).simplified();
            }
            if(valueType.toLower() == "integer")
            {
                precision = icdParamData.at(60).simplified();
            }
            else
                precision= "NA";
        }
        else if(fieldCount > 33 && (_ParsingFalgeData[AFDXOUT] || _ParsingFalgeData[AFDXIN])&&((icdType == AFDX_INPUT_MESSAGE) || (icdType == AFDX_OUTPUT_MESSAGE) ))
        {
            media = "AFDX";
            message=icdParamData.at(6).simplified();
            //identifier = icdParamData.at(24);
            appName = icdParamData.at(1).simplified();
            associatedBus = icdParamData.at(4).simplified();
            signalName = icdParamData.at(33).simplified();
            valueType = icdParamData.at(34).simplified();
            ParameterLocalName = icdParamData.at(24).simplified();
            fsName = icdParamData.at(20).simplified();
            refreshRate = icdParamData.at(14).simplified();
            if(valueType == BOOLEAN_TYPE && fieldCount >= 42)
            {
                maxVal = icdParamData.at(40); // bool true definition
                minVal = icdParamData.at(41); // bool false definition
                GTAICDParameterValues val1,val2;
                val1.key = QString("false");
                val1.value = minVal.trimmed().isEmpty()?"0":minVal;

                val2.key = QString("true");
                val2.value = maxVal.trimmed().isEmpty()?"1":maxVal;

                possibleVals.append(val1);
                possibleVals.append(val2);
            }
            else if(valueType == INTERGER_TYPE && fieldCount >= 56)
            {
                minVal = icdParamData.at(54);
                maxVal = icdParamData.at(55);
                GTAICDParameterValues val1,val2;
                val1.key = QString("Min");
                val1.value = minVal;

                val2.key = QString("Max");
                val2.value = maxVal;

                possibleVals.append(val1);
                possibleVals.append(val2);
            }
            else if(valueType == FLOAT_TYPE && fieldCount >= 51)
            {
                minVal = icdParamData.at(49);
                maxVal = icdParamData.at(50);
                GTAICDParameterValues val1,val2;
                val1.key = QString("Min");
                val1.value = minVal;

                val2.key = QString("Max");
                val2.value = maxVal;

                possibleVals.append(val1);
                possibleVals.append(val2);
            }
            else if(valueType == ENUMERATE_TYPE && fieldCount >= 48)
            {
                QString values = icdParamData.at(48);
                QList<GTAICDParameterValues> lst = parseEnumerationValues(values);
                possibleVals.append(lst);
            }
            if(valueType.toLower() == "float")
            {
                unit = icdParamData.at(51);
                precision = icdParamData.at(52).simplified();
            }
            if(valueType.toLower() == "integer")
            {
                unit = icdParamData.at(56);
                precision = icdParamData.at(57).simplified();
            }
            else
            {
                unit = "NA";
                precision= "NA";
            }
        }

        else if(icdType == MIL1553_MESSAGE && _ParsingFalgeData[MIL_1553_MESSAGE])
        {
            message = icdParamData.at(5);
            associatedBus = icdParamData.at(1);
            _MIL1553MessageBusMap.insert(message,associatedBus);
        }
        else if((_ParsingFalgeData[MIL_1553_DATA])&&((icdType == MIL1553_INPUT_DATA) || (icdType == MIL1553_OUTPUT_DATA) ))
        {
            media = "MIL_1553";
            appName = icdParamData.at(1);
            message = icdParamData.at(2);
            signalName = icdParamData.at(14);
            signalType = "MIL_1553_" + icdParamData.at(15);
            ParameterLocalName = icdParamData.at(5);
            valueType = icdParamData.at(15);
            associatedBus = _MIL1553MessageBusMap.value(message);

            if(( valueType == "Float" ) && (fieldCount > 32))
            {
                unit = icdParamData.at(32);
                precision = icdParamData.at(33);
                minVal = icdParamData.at(30);
                maxVal = icdParamData.at(31);
            }
            else if(( valueType == "Boolean" ) && (fieldCount > 22))
            {
                unit = "NA";
                precision = "NA";
                minVal = icdParamData.at(21);
                maxVal = icdParamData.at(22);
            }
            else if(( valueType == "Integer" ) && (fieldCount > 42))
            {
                unit = icdParamData.at(42);
                precision = icdParamData.at(43);
                minVal = icdParamData.at(40);
                maxVal = icdParamData.at(41);
            }
        }
        if(! appName.isEmpty() && (! associatedBus.isEmpty()) && (! signalName.isEmpty()) && (icdType != MIL1553_MESSAGE))
        {
            rc = true;
            paramName = QString("%1.%2.%3").arg(appName,associatedBus,signalName);
            tempName = QString("%1.%2.%3").arg(appName,associatedBus,signalName);
            oParam.setName(paramName);
            oParam.setUpperName(paramName.toUpper());
            oParam.setParameterType(paramType);
            oParam.setBit(bit);
            oParam.setLabel(label);
            oParam.setMaxValue(maxVal);
            oParam.setMinValue(minVal);
            oParam.setSignalType(signalType);
            oParam.setUnit(unit);
            oParam.setValueType(valueType);
            oParam.setMessageName(message);
            oParam.setParameterLocalName(ParameterLocalName);
            oParam.setFSName(fsName);
            oParam.setPrecision(precision);
            oParam.setDirection(direction);
            oParam.setMediaType(mediaType);
            oParam.setRefreshRate(refreshRate);

        }
    }
    return rc;
}

bool GTALocalDbUtil::updateDbFromPmrXmlFileClassic(const QString & iFileToParse, const QString& iLibraryPath)
{
    bool rc = false;
    QFile fileObj(iLibraryPath+iFileToParse);
    rc = fileObj.open(QFile::ReadOnly | QFile::Text);

    if (!rc)
    {
        _ErrMessage = "Error: File " + QString(iFileToParse) + " could not open";
        return rc;
    }

    QDomDocument domDoc;
    rc= domDoc.setContent(&fileObj);
    if(!rc)
    {
        _ErrMessage = "Error: Fail to open file "+ QString(iFileToParse) +" a xml file";
        fileObj.close();
        return rc;
    }

    QDomElement domElem =  domDoc.documentElement();
    if(domElem.nodeName().compare(PMRNODE,Qt::CaseInsensitive) == 0)
    {
        QDomNodeList actors = domDoc.elementsByTagName("Actor");
        QDomNodeList varsList = domElem.elementsByTagName(VARIABLES_KEY);
        QString actor = "";
        if(!actors.isEmpty())
            actor = actors.at(0).attributes().namedItem("name").nodeValue();
        for(int i=0; i<= varsList.count();i++)
        {
            QDomNode varsNode = varsList.at(i);
            QDomNodeList varList = varsNode.childNodes();
            for(int j=0; j< varList.count();j++)
            {
                GTAICDParameter Parameter;
                rc = getParameterFromPMR(varList.at(j),Parameter);
                if(rc)
                {
                    Parameter.setSourceFile(iFileToParse);
                    Parameter.setPrecision("NA");
                    Parameter.setDirection("I/O");
                    _NewParameterList.append(Parameter);
                }
            }
        }
    }
    else if(domElem.nodeName().compare(VCOM_MESSAGES,Qt::CaseInsensitive) == 0)
    {
        QString busName;
        QString interfaceName;
        QString equipment;
        QString suffix;
        QFileInfo info(iFileToParse);
        QString fileName = info.fileName();
        QString media= "VCOM";
        fileName = fileName.remove(".xml");
        QStringList items = fileName.split("_");
        if(!items.isEmpty())
        {
            bool isMsg = false;
            QStringList newItems;
            foreach(QString itemVal, items)
            {
                if(isMsg)
                {
                    newItems.append(itemVal);
                }
                if(itemVal.contains("message",Qt::CaseInsensitive))
                {
                    isMsg  =true;
                }
            }
            if(!newItems.isEmpty())
                suffix = newItems.join("_");
        }
        if(_ParsingFalgeData[VCOMSTATIC])
            busName = "nom";
        else
        {
            if(domElem.elementsByTagName(BUSES).count()>0)
            {
                QDomElement busesElem = domElem.elementsByTagName(BUSES).at(0).toElement();
                if(busesElem.elementsByTagName(BUS_KEY).count()>0)
                {
                    QDomNode busElem = busesElem.elementsByTagName(BUS_KEY).at(0);
                    if(busElem.attributes().contains(NAME_KEY))
                        busName=  busElem.attributes().namedItem(NAME_KEY).nodeValue();
                }
            }
        }


        if(domElem.elementsByTagName(INTERFACES).count()>0)
        {
            QDomElement interfacesElem = domElem.elementsByTagName(INTERFACES).at(0).toElement();
            if( interfacesElem.elementsByTagName(INTERFACE_KEY).count()>0)
            {
                QDomNode interfaceNode = interfacesElem.elementsByTagName(INTERFACE_KEY).at(0);
                if(interfaceNode.attributes().contains(NAME_KEY))
                    interfaceName=  interfaceNode.attributes().namedItem(NAME_KEY).nodeValue();
            }
        }

        if(domElem.elementsByTagName(EQUIPMENTS_KEY).count()>0)
        {
            QDomElement equipmetsElem = domElem.elementsByTagName(EQUIPMENTS_KEY).at(0).toElement();
            if( equipmetsElem.elementsByTagName(EQUIPMENT_KEY).count()>0)
            {
                QDomNode equipNode = equipmetsElem.elementsByTagName(EQUIPMENT_KEY).at(0);
                if(equipNode.attributes().contains(NAME_KEY))
                    equipment=  equipNode.attributes().namedItem(NAME_KEY).nodeValue();
            }
        }

        QDomNodeList paramsList = domElem.elementsByTagName(PARAMETERS_KEY);

        for(int i=0; i< paramsList.count();i++)
        {
            QDomNode paramsNode = paramsList.at(i);
            QDomNodeList paramNodeList = paramsNode.childNodes();
            for(int j=0; j< paramNodeList.count();j++)
            {
                GTAICDParameter Parameter;
                rc = getParameterFromICDL(paramNodeList.at(j),Parameter);
                if(rc)
                {
                    Parameter.setSourceFile(iFileToParse);

                    QDomNode param = paramNodeList.at(j);
                    QString temp = param.attributes().namedItem("Type").nodeValue();
                    QString sigName =param.attributes().namedItem("Name").nodeValue();
                    QString valueType;
                    if(temp.startsWith("I"))
                        valueType = "Integer";
                    else if(temp.startsWith("F"))
                        valueType = "Float";
                    else if(temp.startsWith("C"))
                        valueType = "String";
                    QString signalType = "VCOM_"+valueType;
                    if(!sigName.isEmpty())
                    {
                        QString paramName =  QString("%1.%2.%3").arg("MODEL_"+suffix,busName,sigName);
                        Parameter.setApplicationName("MODEL_"+suffix);
                        Parameter.setBusName(busName);
                        Parameter.setMedia(media);
                        Parameter.setMessageName("");
                        Parameter.setSignalName(sigName);
                        Parameter.setSignalType(signalType);
                        //When generating the scxml procedure, vmac, vcom and model parameters of type Bool, int or Double shall be forced to type Float (limitation on osl)
                        if((valueType.toLower().contains("int")) || (valueType.toLower().contains("bool"))|| (valueType.toLower().contains("double")))
                            valueType = "Float";

                        Parameter.setValueType(valueType);
                        Parameter.setSourceFile(iFileToParse);
                        Parameter.setSourceType("VCOM_STATIC");
                        Parameter.setParameterType("VCOM");
                        Parameter.setBit("NA");
                        Parameter.setLabel("NA");
                        Parameter.setUnit("NA");
                        Parameter.setIdentifier("NA");
                        Parameter.setEquipmentName("NA");
                        Parameter.setName(paramName);
                        Parameter.setUpperName(paramName.toUpper());
                        Parameter.setTempParamName(paramName);
                        Parameter.setSuffix("MODEL_"+suffix);
                        Parameter.setPrecision("NA");
                        Parameter.setDirection("I/O");
                        _NewParameterList.append(Parameter);
                    }

                }
            }
        }
    }
    else if(domElem.nodeName().compare(MODEL_ICD,Qt::CaseInsensitive) == 0)
    {
        GTAICDParameter Parameter;
        QString msgName;
        QString busName;
        QString appName;
        QString sigName;
        QString media;
        QString valueType;
        QString signalType;
        QDomNode ICDNode = domElem.namedItem("ICD");


        if(ICDNode.isNull() || !ICDNode.hasChildNodes())
            return false;

        QDomNode BUSNode = ICDNode.namedItem("Buses");
        QDomNode EqNode = ICDNode.namedItem("Equipments");
        QDomNode InterfaceNode = ICDNode.namedItem("Interfaces");

        if(BUSNode.isNull() || ! BUSNode.hasChildNodes())
            return false;
        if(EqNode.isNull() || ! EqNode.hasChildNodes())
            return false;
        if(InterfaceNode.isNull() || ! InterfaceNode.hasChildNodes())
            return false;


        QDomNode busNode = BUSNode.namedItem("Bus");
        if(!busNode.isNull())
            busName = busNode.attributes().namedItem("name").nodeValue();

        QDomNode EquipmentNode = EqNode.namedItem("Equipement");
        if(!EquipmentNode.isNull())
            msgName= appName = EquipmentNode.attributes().namedItem("name").nodeValue();

        QDomNode IntNode = InterfaceNode.namedItem("Interface");
        if(!IntNode.isNull())
            media = IntNode.attributes().namedItem("name").nodeValue();


        QDomNode paramsNode = ICDNode.namedItem("Parameters");
        if(paramsNode.isNull() || !paramsNode.hasChildNodes())
            return false;
        QDomNodeList paramListNode = paramsNode.toElement().elementsByTagName("Parameter");
        if(paramListNode.isEmpty())
            return false;
        for(int i =0;i<paramListNode.count();i++)
        {
            QDomNode param = paramListNode.at(i);
            sigName = param.attributes().namedItem("name").nodeValue();
            valueType = param.attributes().namedItem("type").nodeValue();
            signalType = "MODEL_"+valueType;
            if(!sigName.isEmpty())
            {
                QString paramName =  QString("%1.%2.%3").arg(appName,busName,sigName);
                //Parameter.setTempParamName(paramName);
				//Parameter.setApplicationName(appName);
				//Parameter.setApplicationName(paramName);
                //Parameter.setBusName(paramName);
                //Parameter.setMedia("MODEL");
                Parameter.setMessageName(msgName);
                //Parameter.setSignalName(paramName);
                Parameter.setSignalType(signalType);
                //When generating the scxml procedure, vmac, vcom and model parameters of type Bool, int or Double shall be forced to type Float (limitation on osl)
                if((valueType.toLower().contains("int")) || (valueType.toLower().contains("bool"))|| (valueType.toLower().contains("double")))
                    valueType = "Float";
                Parameter.setValueType(valueType);
                Parameter.setSourceFile(iFileToParse);
                //Parameter.setSourceType("MODEL_ICD");
                Parameter.setParameterType("MODEL");
                Parameter.setBit("NA");
                Parameter.setLabel("NA");
                Parameter.setUnit("NA");
                Parameter.setIdentifier("NA");
				Parameter.setParameterLocalName("NA");
                Parameter.setEquipmentName("NA");
                Parameter.setName(paramName);
                Parameter.setUpperName(paramName.toUpper());
                Parameter.setSuffix("NA");
                Parameter.setPrecision("NA");
                Parameter.setDirection("I/O");
                _NewParameterList.append(Parameter);
                rc = true;

            }
        }
    }
    return rc;
}
const QString GTALocalDbUtil::getDatabaseVersion()
{
    QStringList tables = _ParamDb.tables();
    QString version;
    if (tables.contains(DATABASE_VERSION))
    {
        QSqlQuery selectQuery(_ParamDb);
        QString selectQueryStr = QString("SELECT DB_Version FROM %1").arg(DATABASE_VERSION);
        if (selectQuery.exec(selectQueryStr))
        {
            while (selectQuery.next())
                version = selectQuery.value(0).toString();
        }

    }
    else
        version = "NULL";

    return version;
}

bool  GTALocalDbUtil::getParameterFromPMR(const QDomNode & iVarNode,GTAICDParameter & oParameter)
{
    bool rc = false;
    if(iVarNode.isElement())
    {
        QDomNodeList varChildren = iVarNode.childNodes();
        QString paramName, paramType = "Model", valueType;
        for(int i =0; i<= varChildren.count();i++)
        {
            QDomNode childNode = varChildren.at(i);
            if(childNode.nodeName().compare(BISG,Qt::CaseInsensitive) == 0)
            {
                QDomNamedNodeMap attributes = childNode.attributes();
                if(attributes.contains(MAPPING))
                {
                    QString nodeVal = attributes.namedItem(MAPPING).nodeValue();
                    paramName = nodeVal.replace(",",".");
                }
            }
            else if(childNode.nodeName().compare(TYPE_KEY,Qt::CaseInsensitive) == 0)
            {
                valueType = childNode.toElement().text();

            }
        }
        if(! paramName.isEmpty())
        {
            oParameter.setSourceType("PMR");
            oParameter.setParameterType(paramType);
            oParameter.setValueType(valueType);
            oParameter.setDirection("I/O");
            rc = true;
        }
    }
    return rc;
}
bool GTALocalDbUtil::getParameterFromICDL(const QDomNode &iParamNode,GTAICDParameter & oParameter)
{
    bool rc = false;

    if(iParamNode.isElement())
    {

        QDomNamedNodeMap attributes = iParamNode.attributes();
        QString paramName;
        QString valueType;
        QString paramType = "FIB";
        if(attributes.contains("name"))
            paramName = attributes.namedItem("name").nodeValue();
        if(attributes.contains("type"))
            valueType = attributes.namedItem("type").nodeValue();

        if(!paramName.isEmpty())
        {
            rc = true;
            oParameter.setName(paramName);
            oParameter.setUpperName(paramName.toUpper());
            oParameter.setTempParamName(paramName);
            oParameter.setParameterType(paramType);
            oParameter.setSourceType("PMR");
            oParameter.setDirection("I/O");
        }
    }
    return rc;
}
bool GTALocalDbUtil::getParameterFromModelICD(const QString & iFileToParse, const QString& iLibraryPath)
{
    bool rc = false;
    QTextCodec::setCodecForLocale(QTextCodec::codecForUtfText("ASCII"));
    QXmlStreamReader XmlReader;
    QFile File(iLibraryPath+iFileToParse);
    rc = File.open(QFile::Text | QFile::ReadOnly);

    if(rc)
    {
        XmlReader.setDevice(&File);
        QString sPMREquipment;
        rc = XmlReader.hasError();
        if(! rc)
        {
            rc = XmlReader.readNextStartElement();

            if(rc)
            {
                QString elemName = XmlReader.name().toString();
                if(elemName == MODEL_ICD)
                {
                    XmlReader.clear();
                    File.close();
                    GTAICDParameter Parameter;
                    QString msgName;
                    QString busName;
                    QString appName;
                    QString sigName;
                    QString media;
                    QString valueType;
                    QString signalType;
                    QDomDocument doc;
                    rc = File.open(QFile::Text | QFile::ReadOnly);
                    if (!rc || (!doc.setContent(&File))) {
                        File.close();
                        return 0;
                    }
                    QDomNode ICDNode = doc.namedItem("ICD");


                    if(ICDNode.isNull() || !ICDNode.hasChildNodes())
                        return false;

                    QDomNode BUSNode = ICDNode.namedItem("Buses");
                    QDomNode EqNode = ICDNode.namedItem("Equipments");
                    QDomNode InterfaceNode = ICDNode.namedItem("Interfaces");

                    if(BUSNode.isNull() || ! BUSNode.hasChildNodes())
                        return false;
                    if(EqNode.isNull() || ! EqNode.hasChildNodes())
                        return false;
                    if(InterfaceNode.isNull() || ! InterfaceNode.hasChildNodes())
                        return false;


                    QDomNode busNode = BUSNode.namedItem("Bus");
                    if(!busNode.isNull())
                        busName = busNode.attributes().namedItem("name").nodeValue();

                    QDomNode EquipmentNode = EqNode.namedItem("Equipement");
                    if(!EquipmentNode.isNull())
                        msgName= appName = EquipmentNode.attributes().namedItem("name").nodeValue();

                    QDomNode IntNode = InterfaceNode.namedItem("Interface");
                    if(!IntNode.isNull())
                        media = IntNode.attributes().namedItem("name").nodeValue();


                    QDomNode paramsNode = ICDNode.namedItem("Parameters");
                    if(paramsNode.isNull() || !paramsNode.hasChildNodes())
                        return false;
                    QDomNodeList paramListNode = paramsNode.toElement().elementsByTagName("Parameter");
                    if(paramListNode.isEmpty())
                        return false;
                    for(int i =0;i<paramListNode.count();i++)
                    {
                        QDomNode param = paramListNode.at(i);
                        sigName = param.attributes().namedItem("name").nodeValue();
                        valueType = param.attributes().namedItem("type").nodeValue();
                        signalType = "MODEL_"+valueType;
                        if(!sigName.isEmpty())
                        {
                            QString paramName =  QString("%1.%2.%3").arg(appName,busName,sigName);
                            Parameter.setName(paramName);
                            Parameter.setUpperName(paramName.toUpper());
                            Parameter.setMessageName(msgName);
                            Parameter.setSignalType(signalType);
                            //When generating the scxml procedure, vmac, vcom and model parameters of type Bool, int or Double shall be forced to type Float (limitation on osl)
                            if((valueType.toLower().contains("int")) || (valueType.toLower().contains("bool"))|| (valueType.toLower().contains("double")))
                                valueType = "Float";
                            Parameter.setValueType(valueType);
                            Parameter.setSourceType("MODEL_ICD");
                            Parameter.setParameterType("MODEL");
                            Parameter.setBit("NA");
                            Parameter.setLabel("NA");
                            Parameter.setUnit("NA");
							Parameter.setParameterLocalName("NA");
                            Parameter.setEquipmentName("NA");
                            Parameter.setSuffix("NA");
                            Parameter.setPrecision("NA");
                            Parameter.setDirection("I/O");
                            _NewParameterList.append(Parameter);
                        }
                    }
                }

            }
            else
            {

                XmlReader.clear();
                File.close();
                rc = updateDbFromPmrXmlFileClassic(iFileToParse,iLibraryPath);
            }
        }
        else
        {
            rc = updateDbFromPmrXmlFileClassic(iFileToParse,iLibraryPath);
            _ErrMessage= QString("Error: GTALocalDbUtil: Fail to read xml file %1\n").arg(iFileToParse);
            _ErrMessage+=XmlReader.errorString();
        }

        XmlReader.clear();
        if(File.isOpen())
            File.close();
    }
    else
    {
        _ErrMessage= QString("Error: GTALocalDbUtil: Fail to read xml file %1\n").arg(iFileToParse);
        _ErrMessage+=File.errorString();
    }
    return rc;
}
bool GTALocalDbUtil::getParameterFromVCOM(const QString & iFileToParse, const QString& iLibraryPath)
{

    bool rc = false;
    QTextCodec::setCodecForLocale(QTextCodec::codecForUtfText("ASCII"));
    QXmlStreamReader XmlReader;
    QFile File(iLibraryPath+iFileToParse);
    rc = File.open(QFile::Text | QFile::ReadOnly);

    if(rc)
    {
        XmlReader.setDevice(&File);
        QString sPMREquipment;
        rc = XmlReader.hasError();
        if(! rc)
        {
            rc = XmlReader.readNextStartElement();

            if(rc)
            {
                QString elemName = XmlReader.name().toString();
                if(elemName == VCOM_MESSAGES)
                {
                    XmlReader.clear();
                    File.close();
                    GTAICDParameter Parameter;
                    QString msgName;
                    QString busName;
                    QString appName;
                    QString sigName;
                    QString media= "VCOM";
                    QString suffix;
                    QString valueType;
                    QString signalType;
                    QFileInfo info(iFileToParse);
                    QString fileName = info.fileName();
                    fileName = fileName.remove(".xml");
                    QStringList items = fileName.split("_");
                    if(!items.isEmpty())
                    {
                        bool isMsg = false;
                        QStringList newItems;
                        foreach(QString itemVal, items)
                        {
                            if(isMsg)
                            {
                                newItems.append(itemVal);
                            }
                            if(itemVal.contains("message",Qt::CaseInsensitive))
                            {
                                isMsg  =true;
                            }
                        }
                        if(!newItems.isEmpty())
                            suffix = newItems.join("_");
                    }
                    QDomDocument doc;
                    rc = File.open(QFile::Text | QFile::ReadOnly);
                    if (!rc || (!doc.setContent(&File))) {
                        File.close();
                        return 0;
                    }
                    QDomNodeList msgList = doc.elementsByTagName("Messages");
                    if(msgList.isEmpty())
                        return false;
                    if(msgList.at(0).nodeName() == "Messages")
                    {
                        QDomNodeList messages = doc.elementsByTagName("Message");
                        for(int i =0;i<messages.count();i++)
                        {
                            QDomNode msg = messages.at(i);
                            busName = msgName = msg.attributes().namedItem("Name").nodeValue();
                            QDomNodeList producers = msg.toElement().elementsByTagName("ProducerApplications");

                            if(!producers.isEmpty())
                            {
                                QDomNode appNode = producers.at(0).namedItem("ProducerApplication");
                                appName = appNode.attributes().namedItem("Name").nodeValue();
                            }

                            QDomNode paramtersNode = msg.namedItem("Parameters");
                            QDomNodeList paramters = paramtersNode.toElement().elementsByTagName("Parameter");

                            for(int j=0;j<paramters.count();j++)
                            {
                                QDomNode param = paramters.at(j);
                                QString temp = param.attributes().namedItem("Type").nodeValue();
                                sigName =param.attributes().namedItem("Name").nodeValue();

                                if(temp.startsWith("I"))
                                    valueType = "Integer";
                                else if(temp.startsWith("F"))
                                    valueType = "Float";
                                else if(temp.startsWith("C"))
                                    valueType = "String";
                                signalType = "VCOM_"+valueType;
                                if(!sigName.isEmpty())
                                {
                                    QString paramName =  QString("%1.%2.%3").arg("MODEL_"+suffix,busName,sigName);
                                    Parameter.setUpperName(paramName.toUpper());
                                    Parameter.setApplicationName("MODEL_"+suffix);
                                    Parameter.setBusName(busName);
                                    Parameter.setMedia(media);
                                    Parameter.setMessageName(msgName);
                                    Parameter.setSignalName(sigName);
                                    Parameter.setSignalType(signalType);
                                    //When generating the scxml procedure, vmac, vcom and model parameters of type Bool, int or Double shall be forced to type Float (limitation on osl)
                                    if((valueType.toLower().contains("int")) || (valueType.toLower().contains("bool"))|| (valueType.toLower().contains("double")))
                                        valueType = "Float";

                                    Parameter.setValueType(valueType);
                                    Parameter.setSourceFile(iFileToParse);
                                    Parameter.setSourceType("VCOM_STATIC");
                                    Parameter.setParameterType("VCOM");
                                    Parameter.setBit("NA");
                                    Parameter.setLabel("NA");
                                    Parameter.setUnit("NA");
                                    //Parameter.setIdentifier("NA");
									Parameter.setParameterLocalName("NA");
                                    Parameter.setEquipmentName("NA");
                                    Parameter.setName(paramName);
                                    Parameter.setUpperName(paramName.toUpper());
                                    Parameter.setTempParamName(paramName);
                                    Parameter.setSuffix("MODEL_"+suffix);
                                    Parameter.setPrecision("NA");
                                    Parameter.setDirection("I/O");
                                    _NewParameterList.append(Parameter);
                                }
                            }
                        }
                    }
                }
            }
            else
            {

                XmlReader.clear();
                File.close();
                rc = updateDbFromPmrXmlFileClassic(iFileToParse,iLibraryPath);
            }
        }
        else
        {
            rc = updateDbFromPmrXmlFileClassic(iFileToParse,iLibraryPath);
            _ErrMessage= QString("Error: GTALocalDbUtil: Fail to read xml file %1\n").arg(iFileToParse);
            _ErrMessage+=XmlReader.errorString();
        }

        XmlReader.clear();
        if(File.isOpen())
            File.close();
    }
    else
    {
        _ErrMessage= QString("Error: GTALocalDbUtil: Fail to read xml file %1\n").arg(iFileToParse);
        _ErrMessage+=File.errorString();
    }
    return rc;
}
bool GTALocalDbUtil::updateDbFromPmrXmlFile(const QString & iFileToParse, const QString& iLibraryPath)
{
    bool rc = false;
    QTextCodec::setCodecForLocale(QTextCodec::codecForUtfText("ASCII"));
    QXmlStreamReader XmlReader;
    QFile File(iLibraryPath+iFileToParse);
    rc = File.open(QFile::Text | QFile::ReadOnly);

    if(rc)
    {
        XmlReader.setDevice(&File);
        QString sPMREquipment;
        rc = XmlReader.hasError();

        if(!rc)
        {
            rc = XmlReader.readNextStartElement();
            if(rc)
            {
                QString elemName = XmlReader.name().toString();

                if(elemName == "Pmr")
                {
                    QString actiorName = "";
                    while(!XmlReader.atEnd() && !XmlReader.hasError())
                    {
                        XmlReader.readNext();
                        QString nodeName = XmlReader.name().toString();
                        if(nodeName == "Actor")
                        {
                            actiorName = XmlReader.attributes().value("name").toString();
                            if(!actiorName.isEmpty())
                                sPMREquipment =  actiorName;
                        }
                        if(XmlReader.isStartElement() && XmlReader.name().toString() == "Variable")
                        {
                            XmlReader.readNextStartElement();
                            QString paramName, valueType;
                            QString paramType = "FIB";
                            int timeOut = 0;
                            while(paramName.isEmpty() || valueType.isEmpty())
                            {
                                if(XmlReader.readNext() == QXmlStreamReader::StartElement)
                                {
                                    if( XmlReader.name().toString() == BISG)
                                    {
                                        paramName =XmlReader.attributes().value(MAPPING).toString();
                                    }
                                    else if( XmlReader.name().toString() == TYPE_KEY)
                                    {
                                        valueType = XmlReader.readElementText();
                                    }
                                }
                                if(timeOut == 30)
                                    break;

                                timeOut++;
                            }

                            if(!paramName.isEmpty())
                            {
                                rc = true;
                                QStringList itemsPMR;
                                if(paramName.contains(","))
                                {
                                    itemsPMR = paramName.split(",");
                                }
                                paramName = paramName.replace(",",".");
                                
                                GTAICDParameter Parameter;
                                Parameter.setName(paramName);
                                Parameter.setUpperName(paramName.toUpper());
                                //When generating the scxml procedure, vmac, vcom and model parameters of type Bool, int or Double shall be forced to type Float (limitation on osl)
                                if((valueType.toLower().contains("int")) || (valueType.toLower().contains("bool"))|| (valueType.toLower().contains("double")))
                                    valueType = "Float";
                                Parameter.setValueType(valueType);
                                Parameter.setParameterType("VMAC");
                                Parameter.setSourceFile(iFileToParse);

                                /*if(itemsPMR.count() == 3)
                                {
                                    Parameter.setApplicationName(itemsPMR.at(0).simplified());
                                    Parameter.setBusName(itemsPMR.at(1).simplified());
                                    Parameter.setSignalName(itemsPMR.at(2).simplified());
                                }
                                else
                                {
                                    Parameter.setApplicationName("NA");
                                    Parameter.setBusName("NA");
                                    Parameter.setSignalName("NA");
                                }*/
                                Parameter.setBit("NA");
                                Parameter.setLabel("NA");
                                Parameter.setUnit("NA");
								Parameter.setParameterLocalName("NA");
                                Parameter.setPrecision("NA");
                                Parameter.setEquipmentName(sPMREquipment);
                                Parameter.setDirection("I/O");
                                _NewParameterList.append(Parameter);
                            }

                        }
                    }
                }

            }
            else
            {

                XmlReader.clear();
                File.close();
                rc = updateDbFromPmrXmlFileClassic(iFileToParse,iLibraryPath);
            }
        }
        else
        {
            rc = updateDbFromPmrXmlFileClassic(iFileToParse,iLibraryPath);
            _ErrMessage= QString("Error: GTALocalDbUtil: Fail to read xml file %1\n").arg(iFileToParse);
            _ErrMessage+=XmlReader.errorString();
        }

        XmlReader.clear();
        if(File.isOpen())
            File.close();
    }
    else
    {
        _ErrMessage= QString("Error: GTALocalDbUtil: Fail to read xml file %1\n").arg(iFileToParse);
        _ErrMessage+=File.errorString();
    }
    return rc;
}


bool GTALocalDbUtil::updateDbFromPirFile(const QString & iFileToParse, const QString& iLibraryPath)
{
    bool rc = false;

    rc = isValidPIRFile(iLibraryPath+iFileToParse);
    QStringList aasValue;
    bool isAAS = isAASAvailable(iFileToParse,aasValue);
    if(! rc)
    {
        _ErrMessage = "Error: File " + QString(iFileToParse) + " does not exists or have invalid format";
        return rc;
    }

    QFile fileObj(iLibraryPath+iFileToParse);
    rc = fileObj.open(QFile::ReadOnly | QFile::Text);
    if(rc)
    {
        while (!fileObj.atEnd())
        {
            QString line = fileObj.readLine();
            QStringList elems = line.split(';');
            if(elems.count() >= 7)
            {
                QString equipName = elems.at(1).simplified();
                QString sigName = elems.at(0).simplified();
                QString Media = elems.at(3).simplified();
                if(Media == "ARINC")
                    Media="A429";
                QString paramName = QString("%1.%2.%3").arg(equipName,Media,sigName);
                QString signalType = elems.at(3).simplified();
                QString valueType = "NA";
                if((elems.at(4).simplified() == "B") || (elems.at(4).simplified() == "O") )
                {
                    valueType = "Boolean";
                }
                else  if((elems.at(4).simplified() == "C") || (elems.at(4).simplified() == "W") || (elems.at(4).simplified() == "L")|| (elems.at(4).simplified() == "M") )
                {
                    valueType = "Integer"; // Opaque
                }
                else  if((elems.at(4).simplified() == "F") || (elems.at(4).simplified() == "R") || (elems.at(4).simplified() == "D"))
                {
                    valueType = "Float";
                }
                else  if((elems.at(4).simplified() == "Q") || (elems.at(4).simplified() == "H") || (elems.at(4).simplified() == "I"))
                {
                    valueType = "Integer";
                }
                else  if((elems.at(4).simplified() == "E"))
                {
                    valueType = "Enumerate";
                }
                else
                {
                    valueType = "NA";
                }
                QString paramType = "FIB-IPR";

                GTAICDParameter Parameter;
                Parameter.setName(paramName);
                Parameter.setUpperName(paramName.toUpper());
                Parameter.setParameterType(paramType);
                Parameter.setValueType(valueType);
                Parameter.setSignalType(signalType);
                Parameter.setSourceFile(iFileToParse );
                Parameter.setBit("NA");
                Parameter.setLabel("NA");
                Parameter.setUnit("NA");
				Parameter.setParameterLocalName("NA");
                Parameter.setEquipmentName(equipName);
                Parameter.setPrecision("NA");
                Parameter.setDirection("I/O");
                if(isAAS)
                {
                    foreach(QString val,aasValue)
                    {
                        GTAICDParameter Parameter1;
                        Parameter1 = Parameter;
                        Parameter1.setApplicationName(val);
                        QString newParamName = QString("%1.%2.%3").arg(val,Media,sigName);
                        Parameter1.setName(newParamName);
                        Parameter1.setUpperName(newParamName.toUpper());
                        QString tempName = QString("%1.%2.%3").arg(val,Media,sigName);
                        Parameter1.setTempParamName(tempName);
                        _NewParameterList.append(Parameter1);
                    }
                }
                else
                {
                    _NewParameterList.append(Parameter);
                }
            }
        }
    }
    else
        _ErrMessage = "Error:  File " + QString(iFileToParse) + " could not open";

    return rc;
}
bool GTALocalDbUtil::isAASAvailable(const QString &iPIRFile,QStringList &oValues)
{
    bool rc = false;
    QFileInfo info(iPIRFile);
    QString path = info.absolutePath();
    QString fileName = info.fileName();
    QString baseName;
    if(!fileName.isEmpty())
    {
        QStringList items = fileName.split(".");
        items.removeLast();
        baseName = items.join(".");
    }
    if(!baseName.isEmpty())
    {
        QString aasFile = QString("%1/%2%3").arg(path,baseName,".aas");
        QFile aas(aasFile);
        if(aas.exists())
        {
            rc = parseAASFiles(QStringList(aasFile),oValues);
        }
    }
    return rc;
}

bool GTALocalDbUtil::isValidPIRFile(const QString & iFileToParse)
{
    bool rc = false;
    QFile fileObj(iFileToParse);
    rc = fileObj.open(QFile::ReadOnly | QFile::Text);
    if(rc)
    {
        QString line = fileObj.readLine();
        if(! line.isEmpty() && line.count(';') >= 6)
        {
            rc = true;
        }
        else rc = false;
        fileObj.close();
    }
    return rc;
}

bool GTALocalDbUtil::parseAASFiles(const QStringList & iAASFiles,QStringList &oPIRAASList)
{
    bool rc = true;
    if(iAASFiles.isEmpty())
        rc =false;
    foreach(QString aasFile,iAASFiles)
    {
        QFile file(aasFile);
        if(file.open(QFile::ReadOnly))
        {
            QTextStream in(&file);
            while(!in.atEnd())
            {
                QString line = in.readLine();
                if(!line.isEmpty())
                {
                    QStringList  fields = line.split(";",QString::SkipEmptyParts);
                    if(fields.count()>1)
                    {
                        rc =true;
                        oPIRAASList.append(fields);
                    }
                    else
                    {
                        rc =false;
                    }
                }
                else
                {
                    rc =false;
                }
            }
        }
    }
    return rc;
}

bool GTALocalDbUtil::parseFWCBDSFile(const QString & iBDSFile, QList<GTAFWCData> &oFWCData)
{
    QFile BDSFile(iBDSFile);
    bool rc = BDSFile.open(QFile::ReadOnly | QFile::Text);
    if(rc)
    {
        QHash<QString,QString> subTypeColorMap;

        subTypeColorMap.insert("APPR PROC","amber");
        subTypeColorMap.insert("INFO","green");
        subTypeColorMap.insert("INOP SYS","amber");
        subTypeColorMap.insert("LEVEL 1","amber");
        subTypeColorMap.insert("LEVEL 2","amber");
        subTypeColorMap.insert("LEVEL 3","red");
        subTypeColorMap.insert("LIMITATION","green");
        subTypeColorMap.insert("MAINTENANCE","white");
        subTypeColorMap.insert("PROC","cyan");

        while (!BDSFile.atEnd())
        {
            GTAFWCData fwcData;
            QString line = BDSFile.readLine();
            QStringList items = line.split("!",QString::KeepEmptyParts);
            if(items.count()>=10)
            {
                QString header = items.at(7);
                QString message = items.at(9);
                QString color = subTypeColorMap.value(items.at(12));
                fwcData.setMessage(message);
                fwcData.setHeader(header);
                fwcData.setFWCColor(color);
                oFWCData.append(fwcData);
            }
        }
    }
    return rc;
}
//==============================
bool GTALocalDbUtil::updateTableVersionInParamDB(const QString& iDbFile)
{
    bool rc = false;

    QFile dbFile(iDbFile);
    if (dbFile.exists())
    {
        QStringList tables = _ParamDb.tables();
        if (tables.contains(DATABASE_VERSION))
        {
            QString query = QString("UPDATE %1 SET %2 = %3").arg(DATABASE_VERSION, DV_DB_VERSION_COL, GTA_DATABASE_VERSION_STR);
            QSqlQuery createTableQuery(_ParamDb);
            rc = createTableQuery.exec(query);
        }
        else
        {
            QString query = QString("CREATE TABLE IF NOT EXISTS %1 (%2 TEXT NOT NULL primary key);").arg(DATABASE_VERSION, DV_DB_VERSION_COL);
            bool check = _ParamDb.isOpen();
            if (!check)
                check = openDatabase(_ParamDbFilePath);

            if (check)
            {
                QSqlDatabase database = getParamDatabase();
                QSqlQuery createTableQuery(database);
                rc = createTableQuery.exec(query);

                if (rc)
                {
                    QString updateQuery = QString("INSERT INTO %1(%2) Values (%3)").arg(DATABASE_VERSION, DV_DB_VERSION_COL, GTA_DATABASE_VERSION_STR);
                    rc = createTableQuery.exec(updateQuery);
                }
            }

        }
    }
    return rc;
}
bool GTALocalDbUtil::createTableInParamDB(const QString &iDbFile)
{
    bool rc = false;

    QFile dbFile(iDbFile);
    if(dbFile.exists())
    {
        QString vals;
        QString name = ParameterDBComponents.at(0);
        QString primaryKey = QString("PRIMARY KEY (%1)").arg(name);
        for(auto &param : ParameterDBComponents)
        {
            QString str1 = param;
            QString str2 = QString("%1 varchar(255)%2").arg(str1,QString(","));
            vals += str2;
        }
        vals += primaryKey;

        QString createQuery = QString("CREATE TABLE %2 (%1);").arg(vals,PARAMETER_TABLE);
        bool check = _ParamDb.isOpen();
        if(!check)
        {
            check = openDatabase(_ParamDbFilePath);
        }
        if(check)
        {
            QSqlDatabase database = getParamDatabase();
            QSqlQuery createTableQuery(database);
            rc = createTableQuery.exec(createQuery);
        }
    }
    return rc;
}

bool GTALocalDbUtil::deleteTableInParamDb(const QString &iDbPath,const QString iTableName)
{
    bool rc = false;
    bool check = _ParamDb.isOpen();
    if(!check)
    {
        check = openDatabase(iDbPath);
    }
    if(check)
    {
        QString strQuery = QString("DROP TABLE %1").arg(iTableName);
        QSqlDatabase database = getParamDatabase();
        QSqlQuery dropTableQuery(database);
        if(dropTableQuery.exec(strQuery))
        {
            rc = true;
            _NewParameterList.clear();
            _currentBase.clear();
			_FileModifiedList.clear();
        }
    }
    return rc;
}


bool GTALocalDbUtil::removeContentOfFilesFromDB(const QString & iDbFile, const QStringList &iFileList)
{
    bool rc =false;
    bool check = _ParamDb.isOpen();
    if(!check)
    {
        check = openDatabase(iDbFile);
    }
    if(check)
    {
        QStringList tables = _ParamDb.tables();
        if(tables.contains(PARAMETER_TABLE))
        {

            foreach (QString file,iFileList)
            {
                QFileInfo info(file);

                QString conditionStr = QString(" FILE LIKE '%"+info.fileName()+"'");
                QString deleteRecord = QString("DELETE FROM %1 WHERE %2;").arg(PARAMETER_TABLE,conditionStr);
                QSqlQuery deleteQuery(_ParamDb);
                if(deleteQuery.exec(deleteRecord))
                {
                    rc = true;
                }
                else
                {
                    rc = false;
                }

            }
        }
    }

    return rc;
}

bool GTALocalDbUtil::getParametersFromHistory(QStringList &oWordList)
{
    bool rc = false;
    bool check = _ParamDb.isOpen();
    if(!check)
    {
        check = openDatabase(_ParamDbFilePath);
    }
    if(check)
    {
        QSqlQuery selectQuery(_ParamDb);
        QString selectQueryStr = QString("SELECT PARAMETER FROM %1").arg(PARAM_HISTORY_TABLE);
        rc = selectQuery.exec(selectQueryStr);
        if(rc)
        {
            if(selectQuery.isActive() && selectQuery.isSelect())
            {
                while(selectQuery.next())
                {
                    QString name = selectQuery.value(0).toString();
                    oWordList.append(name);
                }
                rc = true;

            }
        }
    }
    return rc;
}


bool GTALocalDbUtil::updateHistoryTable(const QStringList &iUpdateList)
{
    bool rc = false;
    bool check = _ParamDb.isOpen();
    if(!check)
    {
        check = openDatabase(_ParamDbFilePath);
    }
    if(check)
    {
        QSqlQuery deleteQuery(_ParamDb);
        QString deleteQueryStr = QString("DELETE FROM %1;").arg(PARAM_HISTORY_TABLE);
        rc = deleteQuery.exec(deleteQueryStr);
        if(!rc)
        {
            rc = false;
        }
        else
        {
            QSqlQuery insertQuery(_ParamDb);
            QVariantList id;
            QVariantList names;

            for(int i = 0; i < iUpdateList.count(); i++)
            {
                id << i;
                names << iUpdateList.at(i);
            }

            QString vals = QString(":id,:name");
            QString insertQueryStr = QString("INSERT INTO %1 VALUES(%2);").arg(PARAM_HISTORY_TABLE,vals);
            insertQuery.prepare(insertQueryStr);
            insertQuery.bindValue(QString(":id"),id);
            insertQuery.bindValue(QString(":name"),names);
            if (insertQuery.execBatch())
            {
                rc = true;
            }
        }

    }

    return rc;
}


bool GTALocalDbUtil::createSessionTable(const QString &iDbFile)
{
    bool rc = true;
    QFile dbFile(iDbFile);
    if(dbFile.exists())
    {
        bool check = _UserDb.isOpen();
        if(!check)
        {
            check = openUserDatabase(iDbFile);
        }
        if(check)
        {
            QStringList tables = _UserDb.tables();
            if(!tables.contains(SESSION_TABLE))
            {
                QSqlQuery createTableQuery(_UserDb);
                QString createQueryStr = QString("CREATE TABLE %1 (ID int AUTO_INCREMENT, SESSION_NAME varchar(255), FILE_NAME varchar(255), PRIMARY KEY(ID));").arg(SESSION_TABLE);
                if(createTableQuery.exec(createQueryStr))
                {
                    rc = true;
                }
                else
                {
                    rc = false;
                }
            }
        }
    }

    return rc;
}

QStringList GTALocalDbUtil::getSessionFiles(const QString &iSessionName)
{
    QStringList lstFiles;
    bool check = _UserDb.isOpen();
    if(!check)
    {
        check = openDatabase(_UserDbFilePath);
    }
    if(check)
    {
        QSqlQuery selectQuery(_UserDb);
        QString selectQueryStr = QString("SELECT FILE_NAME FROM %1 WHERE SESSION_NAME = '%2';").arg(SESSION_TABLE,iSessionName);
        bool rc = selectQuery.exec(selectQueryStr);
        if(rc)
        {
            if(selectQuery.isActive() && selectQuery.isSelect())
            {
                while(selectQuery.next())
                {
                    QString fileName = selectQuery.value(0).toString();
                    lstFiles << fileName;
                }
            }
        }
    }
    return lstFiles;

}

QStringList GTALocalDbUtil::getSessionNames()
{
    QStringList lstSessionNames;
    bool check = _UserDb.isOpen();
    if(!check)
    {
        check = openDatabase(_UserDbFilePath);
    }
    if(check)
    {
        QSqlQuery selectQuery(_UserDb);
        QString selectQueryStr = QString("SELECT DISTINCT SESSION_NAME FROM %1;").arg(SESSION_TABLE);
        bool rc = selectQuery.exec(selectQueryStr);
        if(rc)
        {
            if(selectQuery.isActive() && selectQuery.isSelect())
            {
                while(selectQuery.next())
                {
                    QString sessionName = selectQuery.value(0).toString();
                    lstSessionNames << sessionName;
                }
            }
        }
    }
    return lstSessionNames;
}

bool GTALocalDbUtil::saveSession(const QString &iSessionName, const QStringList &iFileNames)
{
    bool rc = false;
    bool check = _UserDb.isOpen();
    if(!check)
    {
        check = openUserDatabase(_UserDbFilePath);
    }
    if(check)
    {
        rc = addNewSession(iSessionName,iFileNames);
    }

    return rc;
}

bool GTALocalDbUtil::addNewSession(const QString &iSessionName, const QStringList &iFileNames)
{
    bool rc = false;
    bool check = _UserDb.isOpen();
    if(!check)
    {
        check = openUserDatabase(_UserDbFilePath);
    }
    if(check)
    {

        QStringList queryStrList;
        QStringList queries;
        for(int i = 0; i < iFileNames.count(); i++)
        {
            QString val = QString("\"%1\",\"%2\"").arg(iSessionName,iFileNames.at(i));
            queryStrList.append("("+val+")");
        }

        QString insertQueryStr = QString("INSERT INTO %1 (SESSION_NAME,FILE_NAME) VALUES %2;").arg(SESSION_TABLE,queryStrList.join(","));
        queries.append(insertQueryStr);
        GTASqliteDriver::executeSqliteQuery(_UserDbFilePath,queries);
        //TODO: Possibly add error handling for executeSqliteQuery

    }
    return rc;
}

bool GTALocalDbUtil::deleteSession(const QStringList &iSelectedItems)
{
    bool rc = false;
    bool check = _UserDb.isOpen();
    if(!check)
    {
        check = openUserDatabase(_UserDbFilePath);
    }
    if(check)
    {
        QStringList conditionStmt;
        foreach(QString item, iSelectedItems)
        {
            conditionStmt.append(QString("SESSION_NAME = '%1'").arg(item));
        }

        QString deleteQueryStr = QString("DELETE FROM %1 WHERE %2;").arg(SESSION_TABLE,conditionStmt.join(" AND "));
        QSqlQuery deleteQuery(_UserDb);
        rc = deleteQuery.exec(deleteQueryStr);
    }

    return rc;
}


bool GTALocalDbUtil::createHistoryTable(const QString &iDbFile)
{
    //ID, PARAMETER
    bool rc = false;
    QFile dbFile(iDbFile);
    if(dbFile.exists())
    {
        bool check = _ParamDb.isOpen();
        if(!check)
        {
            check = openDatabase(iDbFile);
        }
        if(check)
        {
            QStringList tables = _ParamDb.tables();
            if(!tables.contains(PARAM_HISTORY_TABLE))
            {
                QSqlQuery createTableQuery(_ParamDb);
                QString createQueryStr = QString("CREATE TABLE %1 (ID int, PARAMETER varchar(255), PRIMARY KEY(ID));").arg(PARAM_HISTORY_TABLE);
                if(createTableQuery.exec(createQueryStr))
                {
                    rc = true;
                }
                else
                {
                    rc = false;
                }
            }
        }
    }

    return rc;
}

bool GTALocalDbUtil::isTableExisting(const QString &iTableName)
{
    bool check = _ParamDb.isOpen();
    if(!check)
    {
        check = openDatabase(_ParamDbFilePath);
    }
    if(check)
    {
        QStringList tables = _ParamDb.tables();
        if(tables.contains(iTableName))
            return true;
        else
            return false;
    }
	return check;
}

bool GTALocalDbUtil::fillQuery(const QString & iQuery,QList<GTAICDParameter> & oParamList)
{
    bool rc= false;
    QSqlDatabase db = GTALocalDbUtil::getParamDatabase();
    if(db.isOpen())
    {
        QSqlQuery getParamQ(iQuery,db);
        rc= false;

        while(getParamQ.next())
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
            oParamList.append(paramDetail);
        }
		QString LastError = getParamQ.lastError().text();
        getParamQ.clear();
    }

    return rc;
}

QString GTALocalDbUtil::getDatabasePath()
{
    QString path = _ParamDb.databaseName();
    return path;
}

QStringList GTALocalDbUtil::getDbTableColumnList(QSqlDatabase db, const QString &iTableName)
{
    QStringList columnList;
    QSqlQuery selectQuery(db);
    QString selectQueryStr = QString("PRAGMA table_info('%1');").arg(iTableName);
    bool rc = selectQuery.exec(selectQueryStr);
    if(rc)
    {
        if(selectQuery.isActive() && selectQuery.isSelect())
        {
            while(selectQuery.next())
            {
                QString name = selectQuery.value(1).toString();
                columnList.append(name);
            }
            rc = true;

        }
    }

    return columnList;
}


QList<GTAICDParameterValues> GTALocalDbUtil::parseEnumerationValues(const QString &iEnumVal)
{
    QList<GTAICDParameterValues> EnumValues;
    QString enumVal = iEnumVal;
    enumVal.replace("{","");
    enumVal.replace("}","");
    QStringList values = enumVal.split("&",QString::SkipEmptyParts);
    foreach(QString value,values)
    {
        value.replace("[","");
        value.replace("]","");
        QStringList keyPairLst = value.split(":");
        if(keyPairLst.count() == 2)
        {
            GTAICDParameterValues val;
            val.key = keyPairLst.at(0);
            val.value = keyPairLst.at(1);
            EnumValues.append(val);
        }
    }

    return EnumValues;
}


QStringList GTALocalDbUtil::getUpdateQueryListForEquipments()
{
    QStringList queryLst;
    QMap<QString, EquipmentDataReader> equipmentHash;
    bool rc = false;
    bool check = _ParamDb.isOpen();
    if(!check)
    {
        check = openDatabase(_ParamDbFilePath);
    }
    if(check)
    {
        QSqlQuery selectQuery(_ParamDb);
        //QString selectQueryStr = QString("SELECT EQUIPNAME, FILE, APPLICATION, MEDIA, TOOL_NAME FROM PARAMETERS GROUP BY EQUIPNAME, FILE, APPLICATION, MEDIA;");
		QString selectQueryStr = QString("SELECT EQUIPNAME, FILE, NAME, SIGNALTYPE, TOOL_NAME FROM PARAMETERS GROUP BY EQUIPNAME, FILE, NAME, SIGNALTYPE;");
        rc = selectQuery.exec(selectQueryStr);
        if(rc)
        {
            if(selectQuery.isActive() && selectQuery.isSelect())
            {


                while(selectQuery.next())
                {

                    EquipmentDataReader Equipment;
                    QString toolName = selectQuery.value(4).toString();
                    if(toolName.isEmpty() || toolName == "NA")
                        continue;
                    Equipment.equipment = selectQuery.value(0).toString();
                    Equipment.file = selectQuery.value(1).toString();
                    Equipment.application = selectQuery.value(2).toString();
                    Equipment.media = selectQuery.value(3).toString();

                    equipmentHash.insertMulti(toolName,Equipment);

                }
            }
            if(equipmentHash.count())
            {
                QStringList keys = equipmentHash.keys();
				keys.removeDuplicates();
                if(keys.count() > 1)
                {
                    for(int i = 0; i < keys.count(); i++)
                    {
                        QString key = keys.at(i);
                        QList<EquipmentDataReader> dataList = equipmentHash.values(key);
                        if(dataList.count() > 1)
                        {
                            QStringList equipmentLst;
                            QStringList fileLst;
                            QStringList appLst;
                            QStringList mediaLst;

                            for(int j = 0; j < dataList.count(); j++)
                            {
                                EquipmentDataReader Data = dataList.at(j);
                                equipmentLst.append(QString("'%1'").arg(Data.equipment));
                                fileLst.append(QString("'%1'").arg(Data.file));
                                appLst.append(QString("'%1'").arg(Data.application));
                                mediaLst.append(QString("'%1'").arg(Data.media));
                            }

                            equipmentLst.removeDuplicates();
                            QString equipmentClause;
                            if(equipmentLst.count() > 1)
                            {
                                equipmentClause = equipmentLst.join(",");
                                equipmentClause = QString("%1 IN (%2)").arg(DB_EQUIPNAME,equipmentClause);
                            }
                            else if(equipmentLst.count() == 1)
                            {
                                equipmentClause = QString("%1 = %2").arg(DB_EQUIPNAME,equipmentLst.first());
                            }


                            fileLst.removeDuplicates();
                            QString fileClause;
                            if(fileLst.count() > 1)
                            {
                                fileClause = fileLst.join(",");
                                fileClause = QString("%1 IN (%2)").arg(DB_FILE,fileClause);
                            }
                            else if(fileLst.count() == 1)
                            {
                                fileClause = QString("%1 = %2").arg(DB_FILE,fileLst.first());
                            }

                            appLst.removeDuplicates();
                            QString appClause;
                            if(appLst.count() > 1)
                            {
                                appClause = appLst.join(",");
                                appClause = QString("%1 IN (%2)").arg("APPLICATION"/*DB_APPLICATION*/,appClause);
                            }
                            else if(appLst.count() == 1)
                            {
                                appClause = QString("%1 = %2").arg("APPLICATION"/*DB_APPLICATION*/,appLst.first());
                            }

                            mediaLst.removeDuplicates();
                            QString mediaClause;
                            if(mediaLst.count() > 1)
                            {
                                mediaClause = mediaLst.join(",");
                                mediaClause = QString("%1 IN (%2)").arg(DB_SIGNAL_TYPE/*DB_MEDIA*/,mediaClause);
                            }
                            else if(mediaLst.count() == 1)
                            {
                                mediaClause = QString("%1 = %2").arg(DB_SIGNAL_TYPE/*DB_MEDIA*/,mediaLst.first());
                            }

                            QString whereClause = QString("WHERE %1 AND %2 AND %3 AND %4").arg(equipmentClause,fileClause,appClause,mediaClause);
                            QString updateStmt = QString("UPDATE PARAMETERS SET TOOL_NAME = '%1' %2;").arg(key,whereClause);
                            queryLst.append(updateStmt);


                        }
                        else if(dataList.count() == 1)
                        {
                            // Only a single distinct record
                            EquipmentDataReader DataObj = dataList.first();
                            QString query1 = QString("UPDATE PARAMETERS SET TOOL_NAME = '%1' WHERE %2='%3' AND %4='%5' AND ").arg(DataObj.toolName,DB_EQUIPNAME,DataObj.equipment,DB_FILE,DataObj.file);
                            QString query2 = QString("%1='%2' AND %3='%4';").arg("APLICATION"/*DB_APPLICATION*/,DataObj.application,"SIGNALTYPE"/*DB_MEDIA*/,DataObj.media);
                            QString query = query1 + query2;
                            queryLst.append(query);

                        }
                    }
                }
                else if(keys.count() == 1)
                {
                    QString query = QString("UPDATE PARAMETERS SET %2 = '%1';").arg(keys.at(0),DB_TOOL_NAME);
                    queryLst.append(query);
                }
            }
            rc = true;
        }
    }
    return queryLst;
}

bool GTALocalDbUtil::updateEquipmentData(const QStringList &iQueryList)
{
    bool rc = false;
    
        int retVal = GTASqliteDriver::executeSqliteQuery(_ParamDbFilePath,iQueryList);
        if(retVal != SQLITE_OK)
        {
            rc = false;
        }
        else
            rc = true;
   
    return rc;

}

