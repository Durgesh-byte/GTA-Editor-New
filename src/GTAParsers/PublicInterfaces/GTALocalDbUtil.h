#ifndef GTALOCALDBUTIL_H
#define GTALOCALDBUTIL_H

#include "GTAParsers.h"
#include "GTAFWCData.h"
#include "GTAEquipmentMap.h"
#include "GTAICDParameter.h"

#pragma warning(push, 0)
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QList>
#include <QDomNode>
#include <QFutureWatcher>
#include <QFile>
#include <QSqlDatabase>
#include <QVariantList>
#pragma warning(pop)


class GTAICDParameter;
class GTAParsers_SHARED_EXPORT GTALocalDbUtil
{

public:
    enum ParseFlag{A429OUT,A429IN, ANAOUT,ANAIN,DESCOUT,DESCIN,CANOUT,CANIN,AFDXOUT,AFDXIN,PMRF,MODELF,VCOMSTATIC,PIRF,MIL_1553_DATA,MIL_1553_MESSAGE};

    GTALocalDbUtil();
    static void setDefaultParseFlag();
    static void setParseFlag(const QHash<ParseFlag,bool> & iFlags);
    static void setICDValidFields();

    /**
      * This function open the database file, return true if open successfully else false
      */
    //   static bool openDatabaseFile(const QString & iFile);
    /**
      * This function close the database file
      */
    //  static void closeDatabaseFile();

    static bool createTableInParamDB(const QString &iDbFile);
    static bool deleteTableInParamDb(const QString &iDbPath,const QString iTableName);
    static bool openDatabase(const QString & iFile);
    static bool commitDatabase();
    static void closeDatabase();

    static bool openUserDatabase(const QString &iFile);
    static bool commitUserDatabase();
    static void closeUserDatabase();

    static bool removeContentOfFilesFromDB(const QString & iDbFile, const QStringList & iFileList);
    //    static bool createFileReferenceTable(const QString &iDbFile);
    //    static bool insertRecordsIntoFileReferences(const QString &iDbFilePath, QVariantList &iIDList, QVariantList &iFileNameList, QVariantList &iDirPathList, QVariantList &iRelativePathList,  QVariantList &iExtensionList, QVariantList &iDescriptionLst);
    //    static bool deleteFileReferencesTable(const QString &iDbFilePath);
    static bool isTableExisting(const QString &iTableName);
    static bool updateHistoryTable(const QStringList &iUpdateList);
    static bool createHistoryTable(const QString &iDbFile);
    static bool createSessionTable(const QString &iDbFile);
    static bool addNewSession(const QString &iSessionName, const QStringList &iFileNames);
    static bool saveSession(const QString &iSessionName, const QStringList &iFileNames);
    static bool deleteSession(const QStringList &iSelectedItems);
    static bool getParametersFromHistory(QStringList &oWordList);
    static QStringList getSessionNames();
    static QStringList getSessionFiles(const QString &iSessionName);

    static QSqlDatabase getParamDatabase();
    /**
      * This function parse the file and update parameter in local database file
      */
    static bool updateParameterInFile(const QString & iFileToParse,const QString & iFileType, const QString& iLibraryPath, const QString& iToolId);
    static QString getLastError();

    static bool parseFWCBDSFile(const QString & iBDSFile, QList<GTAFWCData> & oFWCData);
    static bool updateTableVersionInParamDB(const QString& iDbFile);
    static bool parseAASFiles(const QStringList & iAASFiles,QStringList &oPIRAASList);
    /**
      * Update parameter in database
      */
    static void insertParameterInDatabase(const QString& iLibraryPath);
    static bool fillQuery(const QString & iQuery,QList<GTAICDParameter> & oParamList);
    static QString getDatabasePath();
    static const QString getDatabaseVersion();
    static QStringList getDbTableColumnList(QSqlDatabase db, const QString &iTableName);
    static QList<GTAICDParameterValues> parseEnumerationValues(const QString &iEnumVal);
    static QStringList getUpdateQueryListForEquipments();
    static bool updateEquipmentData(const QStringList &iQueryList);
	/**
	* This function handle to get the current ToolId to use
	*/
	static QString getToolIdToUse() { return _currentToolIdToUse; }
	/**
	* This function handle to set the current ToolId to use
	*/
	static void setToolIdToUse(const QString& iCurrentToolIdToUse) { _currentToolIdToUse = iCurrentToolIdToUse; }

private:

    static QStringList ParameterDBComponents;
    static QFile   _DbWriterFile;
    static QSqlDatabase _ParamDb;
    static QSqlDatabase _UserDb;
    static QString _ParamDbFilePath;
    static QString _UserDbFilePath;
    static QList <GTAICDParameter> _NewParameterList;
	static QList <QString> _FileModifiedList;
    QString _OutputFile;
    static QString _ErrMessage;
    static QHash<ParseFlag,bool> _ParsingFalgeData;
	static QHash<QString,QString> _MIL1553MessageBusMap;
	static QHash<QString,QString> _currentBase;
	static void insertHeaderInDatabase();
	static QString _currentToolIdToUse;
	
    /**
      * This function parse the PMR xml file
      * This method is fast to read the pmr file using QXmlStreamReader approach
      */
    static bool updateDbFromPmrXmlFileClassic(const QString & iFileToParse, const QString& iLibraryPath);
    static bool updateDbFromPmrXmlFile(const QString & iFileToParse, const QString& iLibraryPath);

    static bool getParameterFromPMR(const QDomNode & iVarNode,GTAICDParameter & oParameter);
    static bool getParameterFromICDL(const QDomNode &iParamNode,GTAICDParameter & oParameter);

    static bool getParameterFromModelICD(const QString & iFileToParse, const QString& iLibraryPath);
    static bool getParameterFromVCOM(const QString & iFileToParse, const QString& iLibraryPath);
    /**
      * This function parse the ICD csv file
      */
    static bool updateDbFromIcdCsvFile(const QString & iFileToParse, const QString& iLibraryPath);

    static bool updateDbFromPirFile(const QString & iFileToParse, const QString& iLibraryPath);

    GTAICDParameter * getParameterFromICDL(const QDomNode & iParamNode);

    //    /**
    //       * This function return the parameter read from ICD data
    //       */
    //    GTAICDParameter * getParameterFromICD(const QString & iICDDataLine);

    static bool getParameterFromICD(const QString & iICDDataLine, GTAICDParameter & oParam);
    static bool isValidPIRFile(const QString & iFileToParse);
    static bool isAASAvailable(const QString &iPIRFile,QStringList &oValues);

};

struct EquipmentDataReader
{
    QString equipment;
    QString file;
    QString application;
    QString media;
    QString toolName;
};

#endif // GTALocalDbUtil_H
