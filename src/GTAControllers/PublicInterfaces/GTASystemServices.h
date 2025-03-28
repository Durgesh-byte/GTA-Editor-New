#ifndef GTASystemServices_H
#define GTASystemServices_H

#include "GTAControllers.h"
#include "GTATestConfig.h"

#pragma warning(push, 0)
#include <QObject>
#pragma warning(pop)

/**
  * This structure is defined to manage the tool list for GTA live 
  */
struct GTAControllers_SHARED_EXPORT GTALiveToolParams
{
    QString toolName;
    QString toolIP;
    int port;
    bool operator ==(const GTALiveToolParams &iTool)
    {
        bool rc = false;
        if (iTool.toolName == this->toolName || (iTool.toolIP == this->toolIP && iTool.port == this->port))
            rc =true;
        return rc;
    }
};

class GTAControllers_SHARED_EXPORT GTASystemServices : public QObject
{
    Q_OBJECT
public:
    // GTASystemServices(const QString & iProgram, const QString & iEquipment, const QString & iStandard);

    //void setProgram(const QString & iProgram );
    //void setEquipment(const QString & iEquipment);
    //void setStandard(const QString & iStandard);
    //QString getProgram() const;
    //QString getEquipment() const;
    //QString getStandard() const;

    ~GTASystemServices();
    static GTASystemServices* getSystemServices();

    QString getAASDirectory();
    QStringList getAASFiles();

    QString getPIRDirectory();
    QStringList getPIRFiles();

    QString getPMRDirectory();
    QStringList getPMRFiles();

    QString getICDDirectory();
    QStringList getICDFiles();

    QString getDatabaseFile() ;
    QString getUserDatabasePath();

    
    QStringList getVCOMFiles();
    QString getVCOMDir();

    QStringList getModelICDFiles();
    QString getModelICDDir();
    
    QString getImageDirectory() const;
    QStringList getImageFiles() const;

    QString getApplicationConfigXmlFilePath();
    QString getHeaderTemplateFilePath (const QString& isProgram,const QString& isEquipment,const QString& isStandard) const;
    QString getHeaderTemplateFilePath() const;
    QString getSessionInfoFilePath();

    QString getLTRATemplateFilePath() const;
    QString getLTRATemplateFolderPath() const;
    QStringList getAllDocuments() const;
    QStringList getAudioRecogConfigFileList() const;
    /**
      * Return the list of file name of IRT sequence file
      */
    QStringList getIRTSeqeunceFileList() const;
    QStringList getRoboArmSeqeunceFileList() const;
    /**
      * Return the list of file name of Config Files for One Click
      */
    QStringList getOneClickConfigFileList() const;
    QStringList getOneClickLauncAppConfigFileList() const;

    QStringList getDocumentByExtension(const QString dirPath,const QString iExtn) const;
    QStringList getDocumentByExtension(const QString iExtn) const;

    QString getLibraryPath()const;

    /**
      * return the data directory which store object, function, procedures & sequence
      */
    QString getDataDirectory() const;

    /**
      * return the default log directory which store log files for the executed procedures or sequences
      */
    QString getLogDirectory() const;

    QString getGTAControllerFilePath() const;
    QString getAutoLauncherWatchDirectory() const;
    QString getAutoLauncherConfigPath() const;

    /**
      * returns the log directory which store log files for the executed procedures or sequences
      */
    QString getDefaultLogDirectory() const;


    /**
      * return the data directory which store the external code (such as .lua) which are being used in test procedure
      */
    QString getExternalDataDirectory() const;

    /**
      * return the directory which contains templates
      */
    QString getTemplateDir() const;

    /**
      *return the scxml directory path where the templates files are stored
      */
    QString getScxmlTemplateDir();
    QString getNewSCXMLTemplateDirectory();

    /**
      * Returns the path of the One Click Config file including
      * the file name for a perticular file name.
      */
    QString getOneClickConfigFilePath(const QString & iFileName);

    QString getOneClickLaunchAppConfigPath() const;


    /**
      * Returns the path of the IRT Sequence file including
      * the file name for a perticular file name.
      */
    QString getIRTSequenceFilePath(const QString & iFileName);
    /**
      * Returns the path of the RoboticArm Sequence file including
      * the file name for a perticular file name.
      */
    QString getRoboArmSequenceFilePath(const QString & iFileName);
    /**
      * returns the IRT Library path
      */
    QString getIRTLibPath() const;
    /**
      * returns the OneClick Library path
      */
    QString getOneClickConfigPath() const;
    /**
    * returns the Audio Recog Library path
    */
    QString getAudioRecogConfigPath() const;
    /**
   * This function return the path where the scxml files are saved
   */

    /* This function returns the xml that maintains user properties for each command
    */
    QString getCommandPropertyConfigXMLPath()const;
    QString getScxmlDirectory();

    QString getGenestaSymbolDefinitionFilePath();

    QString getScxmlLaunchExeFilePath() const;

    /**
      * @brief: returns the powershell script path to identify if a server is active or not.
      *         the script is created in tghe Tool_Data path if it is not available there.
      * @return: Path of the powershell script to check server status.
      */
    QString getServerCheckPowershellFilePath() const;

    QString getFwcBDSFilePath();

    QString getTemporaryDirectory() const; 

    /**
   * This function returns the path where the table csv files are saved.
   */
    QString getTableDirectory() const;

    /**
   * This function returns the all the file paths in TABLES folder.
   */
    QStringList getTableFiles();

    QString getFavoritesFilePath() const;

    bool createTemplateDbFile();

    bool renameFile(const QString & iOldFileName,const QString & iNewFileName, QString * oNewName);
    bool removeFile(const QString & iFile);
    void MakeApplicationPaths();
    QString getMICDPath()const;

    bool updateServiceSettings(
        const QString& iDecimalPlace,
        const QString& iLogMessage,
        bool iLogEnable,
        const QString& iManualActTimeoutVal,
        bool iManualActTimeoutStat,
        bool iVirtualPMR,
        bool iResizeRow,
        bool iPrintTableStatus,
        bool iBenchDbStartUp,
        bool iProcDbStartup,
        bool ibSaveElementDuringExportScxml = false,
        //bool ibRepeatLoggingForLog=false,
        bool ibChannelSelectionSetting = false,
        const QString& engineSetting = QString(),
        bool ibPirInvokeForSCXML = false,
        bool ibGenericSCXMLState = false,
        bool iPirWaitTime = false,
        const QString& iTime = QString(),
        bool isNewSCXMLStruct = false,
        bool iStandAloneExecMode = false,
        bool iEnInvokeForLocalVar = false,
        bool iNewLogFormat = false,
        bool iInitSubscribeStart = true,
        bool iInitSubscribeEnd = true,
        bool iUSCXMLBasedSCXMLEn = false,
        QString idebugIP = "127.0.0.1",
        int iDebugPort = 8082,
        bool iValidationEnable = false,
        const QList<GTALiveToolParams>& liveToolList = QList<GTALiveToolParams>(),
        QString bisgGTALiveIP = "127.0.0.1",
        int bisgGTALivePort = 2405);

    bool createLibraryPath();
    bool toolFilesExists(QString& errorMsg);
    QString getExportDocDirectory();
    QString getRoboArmDirectory()const;
    QString getToolConfigurationPath() const;
    QString getGenericDBPath() const;
    //    QString getSVNWorkingDirectory();
    void svnCommitBeforeExit();
    QString getTemplateFilePath(const QString &iTmplFileName);
    QString getLastEmoElementPath()const {return _LastEmoTestElemPath;} 
    QString getLastEmoLogPath()const {return _LastEmologFilePath;}
    void setLastEmoLogPath(const QString& iLastLogFile,const QString& iLastEmoTestElemPath);
    void commitLastLaunchPath();
    QString getScxmlExitExeFilePath()const;
    QStringList setBenchDBConfigFiles(const QString& iniFilePath , bool removeOldEntries = true);
    QStringList getBenchDBConfigFiles();
    QString getDecimalPlaceSetting(){return _DecimalPlaceSetting;}
    QString getGenericToolSchemaFile() const;
    bool isGenerateGenericSCXML();
    QString getEquipmentToolMapFilePath();
    QString getSCXMLToolIDFilePath();
    QString getInternalParamInfoFilePath()const;

    QString getTempEquipmentQueryFilePath();
	QString readAndGetToolIdFromXml();
	QString getCurrentToolId()const { return _currentToolId; }
	void setCurrentToolId(const QString& iCurrentToolId) { _currentToolId = iCurrentToolId; }

    /**
      * This function returns the Tool Data Path
      * @return: Tool data path (if path exists in settings ini file else the default Tool_Data folder path in application directory)
      */
    QString getToolDataPath() const;

    QHash<QString, QStringList> getEquipmentToolName() { return _equipmentToolName; }

signals:
    void settingChange();



private:

    GTASystemServices();

    //QString     _ProgramName;
    //QString     _Equipment;
    //QString     _Standard;
    QString     _RepositoryPath;
    QString     _LibraryPath ;
    QString     _LogDirPath;
    QString     _DatabasePath;
    QString     _HeaderTemplateFile;
    QString     _FwcBDSDirPath;
    QString     _LastEmologFilePath;
    QString     _LastEmoTestElemPath;
    //QString     _SvnWorkingCopyPath;
    QString     _DecimalPlaceSetting;
	QString		_currentToolId;
    QString     _GTAControllerFile;
    QString     _AutoLauncherWatchFolder;
    QString     _AutoLauncherConfigPath;
    QHash<QString, QStringList> _equipmentToolName;

    static GTASystemServices* _pSystemServices;
    void initialize();
    bool pathExists(const QString& iFilePath);
    void setLogDirectory(const QString &iPath);
private slots:

    /**
     * Create the repository folder structure
     * e.g. RepositoryPath/Program/Equipment/Standard/Data
     * RepositoryPath/Program/Equipment/Standard/Log
     * RepositoryPath/Program/Equipment/Standard/External_Data etc.
     */
    bool createRepositoryPath();

};

#endif // GTASystemServices_H
