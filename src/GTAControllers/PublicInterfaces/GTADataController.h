#ifndef GTADATACONTROLLER_H
#define GTADATACONTROLLER_H
#include "GTAControllers.h"
#include "GTAFWCData.h"
#include "GTAEquipmentMap.h"

#pragma warning(push, 0)
#include <QObject>
#include <QFile>
#include <QSqlDatabase>
#include <QHash>
#include <QVariantList>
#include <QString>
#pragma warning(pop)

class GTAICDParser;
class GTAICDParameter;
class GTAControllers_SHARED_EXPORT GTADataController : public QObject
{
    Q_OBJECT
public:
    GTADataController(QObject *parent = 0);
    explicit GTADataController(const QString iLocalDb,QObject *parent = 0);
    ~GTADataController();


    bool getParameterDetail(const QString &iParamName,const QString & dbFile,GTAICDParameter & oParameter);

    bool getParametersDetail(const QStringList & iParamNames,const QString & dbFile, QHash<QString,GTAICDParameter> & oParamDetails, QStringList & oInvalidParams);

    /**
      * This function reads all the ICDs from the repository and generated the model.
      */
    //bool updateLocalDatabase(QString iCSVPath,QString iExtn);
    QSqlDatabase getLocalDatabase();
    void generateLocalDatabase(QStringList iPath, QString iExtn);

    bool openUserDatabase(const QString &iFilePath);

    /**
        * This function open the local database, if succeeded return true otherwise false
        */
    bool openDatabase();
    /**
      * This function set the path of local database file
      * This function must be called immediate after this class instanciation, if default constructure is used
      */
    void setLocalDatabaseFile(const QString iLocalDb);
    bool updateTableVersionInParamDB();
    bool deleteTableInParamDb(const QString &iDbPath);
    bool createTableInParamDB();
    bool isTableExisting(const QString &iTableName);
    /**
      * This function parse the ICD files provided
      * iICDFiles : list of ICD files (.xml,.pir & .csv)
      */
    bool updateParameterFromFiles(const QStringList & iICDFiles, const QString& iToolId);

    bool updateParameterFromFilesParallel(const QStringList & iICDFiles);

    /**
      * This function search the parameter in local database which is set by ctor/setLocalDatabaseFile method
      * return empty list if the database does not exists
      * iStr1 & iStr2 : string to be search
      * iOperator : operation between the search creterion e.g. iStr1 And iStr2 , iStr1 Or iStr2, iStr1 And Not iStr2
      * byIdentifier: if true, string is searched in idenfier of ICD parameter
      * byLabel: if true, string is searched in Label of ICD parameter
      */
    QList<GTAICDParameter> searchParameter(const QString & iStr1, const QString & iStr2,const QString & iOperator,
                                              bool byName = true, bool byIdentifier = false, bool byLabel = false);

    /**
      * This function return the error captured when any of function, provided in this class, fails
      */
    inline QString getLastError()const {return _LastError;}


    /**
      * This function returns the list of file's parameter to be updated in database
      * - Function check the last modified time stamp in provided file and compare it with file in database
      * if timestamp does not match , file appear in removed and update list of file
      * if file not found in database it appears in update list of file
      */
    bool getFileListToUpdateParameter(const QStringList & iList, QStringList & oFileToUpdate, QStringList & oFileToRemove);

    /**
      * This function removes the parameter, which exists in provided file
      */
    bool removeParameterInFiles(const QStringList & iList);

    /**
      * This function close the local database, if succeeded return true otherwise false
      */
    void closeDatabase();


    bool getFwcData(QList<GTAFWCData> &oFwcData/*,const QString & iProgram, const QString & iEquiment, const QString & iStandard*/) const;


    bool fillQuery(const QString & dbFile,const QString & iQuery,QHash<QString,GTAICDParameter> & iParamList);
    bool fillQuery(const QString & dbFile,const QString & iQuery,QStringList & iParamList);
    bool getAASMap(QHash<QString,QString> &oPIRAASMap);
//    bool insertRecordsIntoFileReferences(const QString &iDbFilePath, QVariantList &iIDList, QVariantList &iFileNameList, QVariantList &iDirPathList, QVariantList &iRelativePathList,QVariantList &iExtensionList,QVariantList &iDescriptionLst);
//    bool createFileReferenceTable(const QString &iDbFilePath);
    bool commitDatabase();
    bool updateHistoryTable(const QStringList &iUpdateList);
    bool createHistoryTable(const QString &iDbFilePath);
    bool createSessionTable(const QString &iDbFilePath);
    QStringList getParametersFromHistory();
    QString getDatabasePath()const;
    QList<GTAEquipmentMap> getEquipDataFromDB(QHash<int,GTAEquipmentMap> &oList);
    bool updateToolNameInParameters(const QStringList &iUpdateQuery); 
    QString getDatabaseVersion();
    QStringList getUpdateQueryListForEquipments();
    bool updateEquipmentData(const QStringList &iQueryList);
    bool addNewSession(const QString &iSessionName, const QStringList &iFileNames);
    bool saveSession(const QString &iSessionName, const QStringList &iFileNames);
    bool deleteSession(const QStringList &iSelectedItems);
    QStringList getSessionNames();
    QStringList getSessionFiles(const QString &iSessionName);
    bool commitUserDatabase();
    void closeUserDatabase();


signals:



public slots:
    /**
      * This function is called when ICD file parsing is finished
      * ICD parsing takes place asynochronously,
      * On this function call, parameter retrieved from files are inserted in local database
      */
    void onParseFinish();


private:

    QString _DbFilePath;
    QString _LastError;

//    enum DB_FILE_TYPE{ICD=0,PMR,PIR,VCOM,MODELICD};

};

#endif // GTADATACONTROLLER_H
