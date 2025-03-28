#ifndef AINGTAAppSettings_H
#define AINGTAAppSettings_H

#include "AINGTAControllers.h"
#include <QSettings>
#include <AINGTAUtil.h>
#include "QStringList"
#include <AINGTAAppController.h>

class AINGTAControllers_SHARED_EXPORT AINGTAAppSettings
{
public:
    AINGTAAppSettings(const QString & iINIFilePath = QString());
    ~AINGTAAppSettings();
    void AINGTAAppSettings::preProcess(const QString appSettingsFilePath);
    void setLibraryPath(const QString & iLibraryPath );
    void setRepositoryPath(const QString & iLibraryPath );
    void setOcasimeExeProgramFilePath(const QString & iSchedularProgramFilePath);
	void setToolDataPath(const QString & iToolDataPath);
    void setBenchConfigFilePath(const QString & iSchedularProgramFilePath);
    void setSVNRepositoryPath(const QString & iLibraryPath );
    void setPirWaitTime(bool isTrue,const QString &iTime);
    bool getPirWaitTime(QString &oTime);

    
    //void addProgramConfiguration(const QString & iProgram,const QString & iEquipment, const QString & iStandard);

    void setIRTRepositoryPath(const QString & iIRTLibPath);
    QString getLibraryPath() ;
    QString getRepositoryPath() ;
    QString getOcasimeLaunchProgramFilePath() ;
    QString getSVNRepoPath();

    void setLogPath(const QString & iLogPath);
    QString getLogPath();

    void setSVNUserNamePassword(const QString & iUserName, const QString& iPassword );
    void getSVNUserNamePassword( QString & oUserName,  QString& oPassword );
    //bool getRepeatedLoggingSetting();
    //void setRepeatedLoggingSetting(bool iState);
    void setSaveBeforeExportSetting(bool iState);
    void setChannelSelectionSetting(bool iState);
    bool getChannelSelectionSetting();
    bool getSaveBeforeExportSetting();
    QString getEngineSetting();
    void setEngineSetting(const QString& iEngineType);
    QString getDecimalPlacesSetting();
    void setDecimalPlacesSetting(const QString& iDecimalPlaces);
    /**
      * Author: Ankit
      * @brief get the log message option set by the user
      *	@return logMessage
      */
    QString getLogMessageSetting();
    /**
      * Author: Ankit
      * @brief set - retrieve log message which is set by the user
      *	@ilogMessage - returns the option selected for log message to display.
      */
    void setLogMessageSetting(const QString& ilogMessage);
    /**
      * Author: Ankit
      * @brief set - check wether log message is enabled
      *	@return wether the checkbox is set to true or not
      */
    bool getLogMessageEnable();
    /**
      * Author: Ankit
      * @brief set - check wether log message is enabled or not
      *	@ilogEnable wether the log message is enabled or not
    */
    void setLogMessageEnable(bool ilogEnable);
    void setLoopTimeoutState(bool iState);
    void setStandaloneExecMode(bool iState);
    bool getStandaloneExecMode();
    bool getEnInvokeLocalVarStatus();
    void setEnInvokeLocalVarStatus(bool iState);
    bool getVirtualPMRSetting();
    void setVirtualPMRSetting(bool iVal);
    bool getResizeRow();
    bool getLoopTimeoutState();
    void setResizeRow(bool iVal);
    void setLastLaunchInformation(const QString& isLogPath, const QString& isElementPath);
    void getLastLaunchInformation(QString& isLogPath, QString& isElementPath);

    void setSVNCommitBeforeExit(bool iState);
    void setSVNUpdateOnStart(bool iState);
    void setSVNAddBeforeExit(bool iState);
    bool getSVNCommitBeforeExit();
    bool getSVNUpdateOnStart();
    bool getSVNAddBeforeExit();
    bool getPIRInvokeInSCXML();
    void setSPIRInvokeInSCXML(bool iState);
    void setIncludeIgnoreStepsStatus(bool iState);
    bool getIncludeIgnoreStepsStatus();

    bool getGenerateGenericSCXML();
    void setGenerateGenericSCXML(bool iState);
    bool getGenerateNewSCXMLStruct();
    void setGenerateNewSCXML(bool iState);

    void setEnPrintTableGeneration(bool iState);
    bool getEnPrintTableGeneration();

    bool getDbStartup();
    void setDbStartup(bool iState);

    bool getPrintTableStatus();
    void setPrintTableStatus(bool iState);


    bool getInitSubscribeStartStatus();
    void setInitSubscribeStartStatus(bool iState);
    bool getInitSubscribeEndStatus();
    void setInitSubscribeEndStatus(bool iState);
    bool getNewLogFormatStatus();
    void setNewLogFormatStatus(bool iState);

    bool getManualActionTimeoutStatus();

    void setManualActionTimeoutStatus(bool iState);

    void setUSCXMLBasedSCXMLEn(bool iState);
    bool getUSCXMLBasedSCXMLEn();

    QString getManualActionTimeoutValue();
    void setManualActionTimeoutValue(const QString& iDecimalPlaces);

    void setIgnoreManualActionStatus(bool iState);
    bool getIgnoreManualActionStatus();

    void setSubscribeUnsubscribeTimeoutStatus(bool iState);
    bool getSubscribeUnsubscribeTimeoutStatus();
    void setSubscribeUnsubscribeTimeoutValue(const QString& iDecimalPlaces);
    QString getSubscribeUnsubscribeTimeoutValue();


    void setResultViewOptions(const QStringList& iResultList);
    QStringList getResultViewOptions();
    /**
      * @brief set - Get the validation status by user
    */
    bool getValidationEnabled();
	/**
      * @brief set - Set the validation status by user
      *	@iValidationEnable : get the validation info
    */
    void setValidationEnabled(bool iValidationEnable);

    /**
      * This function sets the executed column state of document viewer reading from .ini file
      * @iHeaderList: Get the header column list which are set/unset.
      */
    void setDVViewOptions(const QStringList& iHeaderList);
    /**
      * This function gets the last executed column state of document viewer
      * Returns column list which are to be set/unset.
      */
    QStringList getDVViewOptions();

    QString getdebugModeIPSelected();
    void setDebugModeIP(QString &ival);
    int getdebugPortSelected();
    void setdebugPort(int ival);
    void setResultPageGeometry(const QByteArray &state);
    QByteArray getResultPageGeometry();
    void setDataTVGeometry(const QByteArray &state);
    QByteArray getDataTVGeometry();
    void setGeometry(const QByteArray &state);
    QByteArray getGeometry();
    void setWindowState(const QByteArray &state);
    QByteArray getWindowState();

    /**
      * This function reads the tool data path from ini file
      * if path is empty, writes the default tool_data path from application directory into ini file and returns the same.
      * Returns tool data path
      */
    QString getToolDataPath();

    /**
      * This function sets the tool list from GTA live tool widget to ini file 
      * @iToolList: Tools which are defined in emotest conf file
      */
    void setLiveViewToolList(const QList<GTALiveToolParams> &iToolList);
	
    /**
      * This function gets the tool list from ini file
      */	
    QList<GTALiveToolParams> getLiveViewToolList();
    /**
      * This function gets the IP of BISG/CGIB ini file
      */
    QString getGtaLiveBisgIP();
    /**
      * This function gets the port number of BISG/CGIB ini file
      */
    int getGtaLiveBisgPort();

    /**
      * This function sets the GTA live BISG IP to ini file 
      * @iBisgIP: BISG_IP
      */
    void setGtaLiveBisgIP(const QString& iBisgIP);
	 /**
      * This function sets the GTA live BISG port to ini file 
      * @iBisgIP: BISG port
      */
    void setGtaLiveBisgPort(int& iBisgPort);

private:
    static QString _appSettingsFilePath;
    QSettings * _pSettings;

};

#endif // AINGTAAppSettings_H
