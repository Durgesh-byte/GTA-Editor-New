#include "aingtaappsettings.h"
#include <QApplication>
#include <QDir>


QString AINGTAAppSettings::_appSettingsFilePath = QString();

AINGTAAppSettings::AINGTAAppSettings(const QString & iINIFilePath) : _pSettings(nullptr)
{

    if(_appSettingsFilePath.isEmpty() && iINIFilePath.isEmpty())
    {
        _appSettingsFilePath = QApplication::applicationFilePath().replace(".exe", ".ini");
        //_pSettings = new QSettings(QApplication::applicationFilePath().replace(".exe",".ini"),QSettings::IniFormat);
    }
    else if(!_appSettingsFilePath.isEmpty())
    {
        //_pSettings = new QSettings(_appSettingsFilePath,QSettings::IniFormat);
    }
    else
    {
        _appSettingsFilePath = iINIFilePath;
        //_pSettings = new QSettings(_appSettingsFilePath,QSettings::IniFormat);

    }
    //preProcess (_appSettingsFilePath);
    _pSettings = new QSettings(_appSettingsFilePath, QSettings::IniFormat);
}

AINGTAAppSettings::~AINGTAAppSettings()
{
    if(_pSettings != nullptr)
    {
        delete _pSettings;
        _pSettings = nullptr;
    }
}

void AINGTAAppSettings::preProcess(const QString appSettingsFilePath)
{
    QFile data(appSettingsFilePath);
    data.open(QIODevice::Text | QIODevice::ReadOnly);
    QString dataText = data.readAll();

    //QRegularExpression re("\\");
    //QString replacementText("\\\\");
    //dataText.replace(re, replacementText);
    dataText.replace("\\", "\\\\");

    QFile newData(appSettingsFilePath);
    if (newData.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&newData);
        out << dataText;
    }
    newData.close();
}

void AINGTAAppSettings::setLibraryPath(const QString & iLibraryPath )
{
    _pSettings->beginGroup("PATH");
    _pSettings->setValue("LIBRARYPATH",iLibraryPath);
    _pSettings->endGroup();
}

void AINGTAAppSettings::setRepositoryPath(const QString & iLibraryPath )
{
    _pSettings->beginGroup("PATH");
    _pSettings->setValue("REPOSITORYPATH",iLibraryPath);
    _pSettings->endGroup();
}

/* This function handles to set the ToolDataPath in the *.ini file
* @input: iToolDataPath - QString containing the absolute path to the ToolData folder 
*/
void AINGTAAppSettings::setToolDataPath(const QString & iToolDataPath)
{
	_pSettings->beginGroup("PATH");
	_pSettings->setValue("TOOLDATAPATH", iToolDataPath);
	_pSettings->endGroup();
}

void AINGTAAppSettings::setOcasimeExeProgramFilePath(const QString & iSchedularProgramFilePath)
{
    _pSettings->beginGroup("PATH");
    _pSettings->setValue("OCASIMELAUNCHEXE",iSchedularProgramFilePath);
    _pSettings->endGroup();
}
void AINGTAAppSettings::setBenchConfigFilePath(const QString & iSchedularProgramFilePath)
{
    _pSettings->beginGroup("PATH");
    _pSettings->setValue("BENCHCONFIG",iSchedularProgramFilePath);
    _pSettings->endGroup();
}

QString AINGTAAppSettings::getOcasimeLaunchProgramFilePath()
{
    QString oPath;
    _pSettings->beginGroup("PATH");
    if(_pSettings->contains("OCASIMELAUNCHEXE"))
        oPath =  _pSettings->value("OCASIMELAUNCHEXE").toString();
    _pSettings->endGroup();
    return QDir::cleanPath(oPath);

}

void AINGTAAppSettings::setIRTRepositoryPath(const QString & iIRTLibPath)
{
    _pSettings->beginGroup("PATH");
    _pSettings->setValue("IRTREPOSITORYPATH",iIRTLibPath);
    _pSettings->endGroup();
}

QString AINGTAAppSettings::getLibraryPath()
{
    QString oPath;
    _pSettings->beginGroup("PATH");
    if(_pSettings->contains("LIBRARYPATH"))
        oPath =  _pSettings->value("LIBRARYPATH").toString();

    _pSettings->endGroup();
    return QDir::cleanPath(oPath);
}
QString AINGTAAppSettings::getRepositoryPath()
{
    QString oPath;
    _pSettings->beginGroup("PATH");
    if(_pSettings->contains("REPOSITORYPATH"))
        oPath =  _pSettings->value("REPOSITORYPATH").toString();

    _pSettings->endGroup();
    return QDir::cleanPath(oPath);
}

QString AINGTAAppSettings::getLogPath()
{
    QString oPath;
    _pSettings->beginGroup("PATH");
    if(_pSettings->contains("LOGDIRPATH"))
        oPath =  _pSettings->value("LOGDIRPATH").toString();

    _pSettings->endGroup();
    return QDir::cleanPath(oPath);
}

void AINGTAAppSettings::setLogPath(const QString & iLogPath)
{
    _pSettings->beginGroup("PATH");
    _pSettings->setValue("LOGDIRPATH",iLogPath);
    _pSettings->endGroup();
}

void AINGTAAppSettings::setSVNRepositoryPath(const QString & iLibraryPath )
{
    _pSettings->beginGroup("PATH");
    _pSettings->setValue("SVNREPOPATH",iLibraryPath);
    _pSettings->endGroup();
}
QString AINGTAAppSettings::getSVNRepoPath()
{
    QString oPath;
    _pSettings->beginGroup("PATH");
    if(_pSettings->contains("SVNREPOPATH"))
        oPath =  _pSettings->value("SVNREPOPATH").toString();

    _pSettings->endGroup();
    return QDir::cleanPath(oPath);
}
void AINGTAAppSettings::setSVNUserNamePassword(const QString & iUserName, const QString& iPassword )
{
    _pSettings->beginGroup("SVNLOGIN");
    _pSettings->setValue("USERNAME",iUserName);
    _pSettings->setValue("PASSWORD",iPassword);
    _pSettings->endGroup();
}
void AINGTAAppSettings::getSVNUserNamePassword( QString & oUserName,  QString& oPassword )
{

    _pSettings->beginGroup("SVNLOGIN");
    if(_pSettings->contains("USERNAME"))
        oUserName =  _pSettings->value("USERNAME").toString();
    if(_pSettings->contains("PASSWORD"))
        oPassword =  _pSettings->value("PASSWORD").toString();

    _pSettings->endGroup();
}
//bool AINGTAAppSettings::getRepeatedLoggingSetting()
//{
//    bool bStatus = false;
//    _pSettings->beginGroup("USERSETTINGS");
//    if(_pSettings->contains("REPEATEDLOGGING"))
//        bStatus= _pSettings->value("REPEATEDLOGGING").toString().toUpper() == "ON"?true:false;

//    _pSettings->endGroup();
//    return bStatus;


//}
bool AINGTAAppSettings::getChannelSelectionSetting()
{
    bool bStatus = false;
    _pSettings->beginGroup("USERSETTINGS");
    if(_pSettings->contains("CHANNEL_SELECTION"))
        bStatus= _pSettings->value("CHANNEL_SELECTION").toString().toUpper() == "ON"?true:false;

    _pSettings->endGroup();
    return bStatus;


}
QString AINGTAAppSettings::getEngineSetting()
{
    QString engineSelection;
    _pSettings->beginGroup("USERSETTINGS");
    if(_pSettings->contains("ENGINE_SELECTION"))
        engineSelection= _pSettings->value("ENGINE_SELECTION").toString();

    _pSettings->endGroup();
    return engineSelection;
}
void AINGTAAppSettings::setEngineSetting(const QString& iEngineType)
{
    _pSettings->beginGroup("USERSETTINGS");
    _pSettings->setValue("ENGINE_SELECTION",iEngineType);
    _pSettings->endGroup();
}

QString AINGTAAppSettings::getDecimalPlacesSetting()
{
    QString decimalPlace;
    _pSettings->beginGroup("USERSETTINGS");
    if(_pSettings->contains("DECIMAL_PLACES"))
        decimalPlace= _pSettings->value("DECIMAL_PLACES").toString();

    _pSettings->endGroup();
    return decimalPlace;
}

QString AINGTAAppSettings::getLogMessageSetting()
{
    QString logSettingsMsg;
    _pSettings->beginGroup("USERSETTINGS");
    if(_pSettings->contains("LOG_MESSAGES"))
        logSettingsMsg =_pSettings->value("LOG_MESSAGES").toString();
    _pSettings->endGroup();
    return logSettingsMsg;
}

void AINGTAAppSettings::setDecimalPlacesSetting(const QString& iDecimalPlaces)
{
    _pSettings->beginGroup("USERSETTINGS");
    _pSettings->setValue("DECIMAL_PLACES",iDecimalPlaces);
    _pSettings->endGroup();
}

void AINGTAAppSettings::setLogMessageSetting(const QString& logMessages)
{
    _pSettings->beginGroup("USERSETTINGS");
    _pSettings->setValue("LOG_MESSAGES", logMessages);
    _pSettings->endGroup();
}

bool AINGTAAppSettings::getLogMessageEnable()
{
    bool logEnable;
    _pSettings->beginGroup("USERSETTINGS");
    if(_pSettings->contains("LOG_ENABLE"))
        logEnable = _pSettings->value("LOG_ENABLE").toBool();
    _pSettings->endGroup();
    return logEnable;
}

void AINGTAAppSettings::setLogMessageEnable(bool logEnable)
{
    _pSettings->beginGroup("USERSETTINGS");
    _pSettings->setValue("LOG_ENABLE",logEnable);
    _pSettings->endGroup();
}

void AINGTAAppSettings::setLoopTimeoutState(bool iState)
{
    _pSettings->beginGroup("USERSETTINGS");
    if(iState)
        _pSettings->setValue("LOOP_TIMEOUT_SUBSTEPS","ON");
    else
        _pSettings->setValue("LOOP_TIMEOUT_SUBSTEPS","OFF");

    _pSettings->endGroup();
}
void AINGTAAppSettings::setStandaloneExecMode(bool iState)
{
    _pSettings->beginGroup("USERSETTINGS");
    if(iState)
        _pSettings->setValue("STANDALONE_EXEC_MODE","ON");
    else
        _pSettings->setValue("STANDALONE_EXEC_MODE","OFF");

    _pSettings->endGroup();
}
bool AINGTAAppSettings::getResizeRow()
{
    QString resize;
    bool val;
    _pSettings->beginGroup("USERSETTINGS");
    if(_pSettings->contains("RESIZE_ROW"))
        resize= _pSettings->value("RESIZE_ROW").toString();

    if(resize == "yes")
        val = true;
    else if(resize == "no")
        val = false;

    _pSettings->endGroup();
    return val;
}
bool AINGTAAppSettings::getLoopTimeoutState()
{
    QString resize;
    bool val;
    _pSettings->beginGroup("USERSETTINGS");
    if(_pSettings->contains("LOOP_TIMEOUT_SUBSTEPS"))
        resize= _pSettings->value("LOOP_TIMEOUT_SUBSTEPS").toString();

    if(resize == "OFF")
        val = false;
    else if(resize == "ON")
        val = true;

    _pSettings->endGroup();
    return val;
}
bool AINGTAAppSettings::getStandaloneExecMode()
{
    QString resize;
    bool val;
    _pSettings->beginGroup("USERSETTINGS");
    if(_pSettings->contains("STANDALONE_EXEC_MODE"))
        resize= _pSettings->value("STANDALONE_EXEC_MODE").toString();

    if(resize == "OFF")
        val = false;
    else if(resize == "ON")
        val = true;

    _pSettings->endGroup();
    return val;
}
void AINGTAAppSettings::setEnInvokeLocalVarStatus(bool iState)
{
    _pSettings->beginGroup("USERSETTINGS");
    QString val;
    if(iState)
        val = "ON";
    else
        val = "OFF";
    _pSettings->setValue("EN_INVOKE_LOCAL_VAR",val);
    _pSettings->endGroup();
}

bool AINGTAAppSettings::getEnInvokeLocalVarStatus()
{
    QString resize;
    bool val;
    _pSettings->beginGroup("USERSETTINGS");
    if(_pSettings->contains("EN_INVOKE_LOCAL_VAR"))
        resize= _pSettings->value("EN_INVOKE_LOCAL_VAR").toString();

    if(resize == "OFF")
        val = false;
    else if(resize == "ON")
        val = true;

    _pSettings->endGroup();
    return val;
}

void AINGTAAppSettings::setResizeRow(bool iVal)
{
    _pSettings->beginGroup("USERSETTINGS");
    QString val;
    if(iVal)
        val = "yes";
    else
        val = "no";
    _pSettings->setValue("RESIZE_ROW",val);
    _pSettings->endGroup();
}


bool AINGTAAppSettings::getVirtualPMRSetting()
{
    QString virtualPMR;
    bool val;
    _pSettings->beginGroup("USERSETTINGS");
    if(_pSettings->contains("VIRTUAL_PMR"))
        virtualPMR= _pSettings->value("VIRTUAL_PMR").toString();

    if(virtualPMR == "yes")
        val = true;
    else if(virtualPMR == "no")
        val = false;

    _pSettings->endGroup();
    return val;

}



void AINGTAAppSettings::setVirtualPMRSetting(bool iVal)
{
    _pSettings->beginGroup("USERSETTINGS");
    QString val;
    if(iVal)
        val = "yes";
    else
        val = "no";
    _pSettings->setValue("VIRTUAL_PMR",val);
    _pSettings->endGroup();
}


//void AINGTAAppSettings::setRepeatedLoggingSetting(bool iState)
//{
//    _pSettings->beginGroup("USERSETTINGS");
//    if(iState)
//        _pSettings->setValue("REPEATEDLOGGING","ON");
//    else
//        _pSettings->setValue("REPEATEDLOGGING","OFF");

//    _pSettings->endGroup();
//}
void AINGTAAppSettings::setSaveBeforeExportSetting(bool iState)
{
    _pSettings->beginGroup("USERSETTINGS");
    if(iState)
        _pSettings->setValue("SAVEBEFOREEXPORT","ON");
    else
        _pSettings->setValue("SAVEBEFOREEXPORT","OFF");

    _pSettings->endGroup();
}
void AINGTAAppSettings::setChannelSelectionSetting(bool iState)
{
    _pSettings->beginGroup("USERSETTINGS");
    if(iState)
        _pSettings->setValue("CHANNEL_SELECTION","ON");
    else
        _pSettings->setValue("CHANNEL_SELECTION","OFF");

    _pSettings->endGroup();
}
void AINGTAAppSettings::setPirWaitTime(bool isTrue,const QString &iTime)
{
    _pSettings->beginGroup("USERSETTINGS");
    if(isTrue)
    {
        _pSettings->setValue("PIR_WAIT_TIME","ON");
        _pSettings->setValue("PIR_WAIT_TIME_VAL",QString("%1").arg(iTime));
    }
    else
    {
        _pSettings->setValue("PIR_WAIT_TIME","OFF");
    }

    _pSettings->endGroup();
}

bool AINGTAAppSettings::getPirWaitTime(QString &oTime)
{
    bool bStatus = false;

    _pSettings->beginGroup("USERSETTINGS");
    if(_pSettings->contains("PIR_WAIT_TIME"))
        bStatus= _pSettings->value("PIR_WAIT_TIME").toString().toUpper() == "ON"?true:false;
    if(_pSettings->contains("PIR_WAIT_TIME_VAL"))
        oTime= _pSettings->value("PIR_WAIT_TIME_VAL").toString();

    _pSettings->endGroup();
    return bStatus;
}

void AINGTAAppSettings::setSVNCommitBeforeExit(bool iState)
{
    _pSettings->beginGroup("USERSETTINGS");
    if(iState)
        _pSettings->setValue("SVN_COMMIT_ON_EXIT","ON");
    else
        _pSettings->setValue("SVN_COMMIT_ON_EXIT","OFF");

    _pSettings->endGroup();
}
void AINGTAAppSettings::setSVNUpdateOnStart(bool iState)
{
    _pSettings->beginGroup("USERSETTINGS");
    if(iState)
        _pSettings->setValue("SVN_UPDATE_ON_START","ON");
    else
        _pSettings->setValue("SVN_UPDATE_ON_START","OFF");

    _pSettings->endGroup();
}
void  AINGTAAppSettings::setSVNAddBeforeExit(bool iState)
{
    _pSettings->beginGroup("USERSETTINGS");
    if(iState)
        _pSettings->setValue("SVN_ADD_ON_EXIT","ON");
    else
        _pSettings->setValue("SVN_ADD_ON_EXIT","OFF");

    _pSettings->endGroup();
}
bool AINGTAAppSettings::getSVNAddBeforeExit()
{
    bool bStatus = false;
    _pSettings->beginGroup("USERSETTINGS");
    if(_pSettings->contains("SVN_ADD_ON_EXIT"))
        bStatus= _pSettings->value("SVN_ADD_ON_EXIT").toString().toUpper() == "ON"?true:false;

    _pSettings->endGroup();
    return bStatus;
}
bool AINGTAAppSettings::getPIRInvokeInSCXML()
{
    bool bStatus = false;
    _pSettings->beginGroup("USERSETTINGS");
    if(_pSettings->contains("PIR_INVOKE_FOR_SCXML"))
        bStatus= _pSettings->value("PIR_INVOKE_FOR_SCXML").toString().toUpper() == "ON"?true:false;

    _pSettings->endGroup();
    return bStatus;
}
void  AINGTAAppSettings::setSPIRInvokeInSCXML(bool iState)
{
    _pSettings->beginGroup("USERSETTINGS");
    if(iState)
        _pSettings->setValue("PIR_INVOKE_FOR_SCXML","ON");
    else
        _pSettings->setValue("PIR_INVOKE_FOR_SCXML","OFF");

    _pSettings->endGroup();
}
bool AINGTAAppSettings::getIncludeIgnoreStepsStatus()
{
    bool bStatus = false;
    _pSettings->beginGroup("USERSETTINGS");
    if(_pSettings->contains("INCLUDE_IGNORE_STEPS"))
    {
        QString data = _pSettings->value("INCLUDE_IGNORE_STEPS").toString().toUpper();
        if(data == "ON")
        {
            bStatus = true;
        }
        else
        {
            bStatus = false;
        }
    }

    _pSettings->endGroup();
    return bStatus;
}
void  AINGTAAppSettings::setIncludeIgnoreStepsStatus(bool iState)
{
    _pSettings->beginGroup("USERSETTINGS");
    if(iState)
        _pSettings->setValue("INCLUDE_IGNORE_STEPS","ON");
    else
        _pSettings->setValue("INCLUDE_IGNORE_STEPS","OFF");

    _pSettings->endGroup();
}

bool AINGTAAppSettings::getGenerateGenericSCXML()
{
    bool bStatus = false;
    _pSettings->beginGroup("USERSETTINGS");
    if(_pSettings->contains("GENERIC_SCXML"))
        bStatus= _pSettings->value("GENERIC_SCXML").toString().toUpper() == "ON"?true:false;

    _pSettings->endGroup();
    return bStatus;
}

bool AINGTAAppSettings::getGenerateNewSCXMLStruct()
{
    bool bStatus = true;
    _pSettings->beginGroup("USERSETTINGS");
    if(_pSettings->contains("NEW_SCXML_STRUCT"))
        bStatus=_pSettings-> value("NEW_SCXML_STRUCT").toString().toUpper() == "ON"?true:false;

    _pSettings->endGroup();
    return bStatus;
}
void AINGTAAppSettings::setGenerateNewSCXML(bool iState)
{
    _pSettings->beginGroup("USERSETTINGS");
    if(iState)
        _pSettings->setValue("NEW_SCXML_STRUCT","ON");
    else
        _pSettings->setValue("NEW_SCXML_STRUCT","OFF");

    _pSettings->endGroup();

}




bool AINGTAAppSettings::getDbStartup()
{
    bool bStatus = false;
    _pSettings->beginGroup("USERSETTINGS");
    if(_pSettings->contains("DB_STARTUP"))
        bStatus= _pSettings->value("DB_STARTUP").toString().toUpper() == "ON"?true:false;

    _pSettings->endGroup();
    return bStatus;
}
void AINGTAAppSettings::setDbStartup(bool iState)
{
    _pSettings->beginGroup("USERSETTINGS");
    if(iState)
        _pSettings->setValue("DB_STARTUP","ON");
    else
        _pSettings->setValue("DB_STARTUP","OFF");

    _pSettings->endGroup();

}

void AINGTAAppSettings::setGenerateGenericSCXML(bool iState)
{
    _pSettings->beginGroup("USERSETTINGS");
    if(iState)
        _pSettings->setValue("GENERIC_SCXML","ON");
    else
        _pSettings->setValue("GENERIC_SCXML","OFF");

    _pSettings->endGroup();

}

bool AINGTAAppSettings::getSVNCommitBeforeExit()
{
    bool bStatus = false;
    _pSettings->beginGroup("USERSETTINGS");
    if(_pSettings->contains("SVN_COMMIT_ON_EXIT"))
        bStatus= _pSettings->value("SVN_COMMIT_ON_EXIT").toString().toUpper() == "ON"?true:false;

    _pSettings->endGroup();
    return bStatus;
}
bool AINGTAAppSettings::getSVNUpdateOnStart()
{
    bool bStatus = false;
    _pSettings->beginGroup("USERSETTINGS");
    if(_pSettings->contains("SVN_UPDATE_ON_START"))
        bStatus= _pSettings->value("SVN_UPDATE_ON_START").toString().toUpper() == "ON"?true:false;

    _pSettings->endGroup();
    return bStatus;
}
bool AINGTAAppSettings::getSaveBeforeExportSetting()
{
    bool bStatus = false;
    _pSettings->beginGroup("USERSETTINGS");
    if(_pSettings->contains("SAVEBEFOREEXPORT"))
        bStatus= _pSettings->value("SAVEBEFOREEXPORT").toString().toUpper() == "ON"?true:false;

    _pSettings->endGroup();
    return bStatus;
}
void AINGTAAppSettings::setLastLaunchInformation(const QString& isLogPath, const QString& isElementPath)
{
    _pSettings->beginGroup("USERSETTINGS");

    _pSettings->setValue("LASTLOGPATH",isLogPath);
    _pSettings->setValue("LASTLAUNCHELEMPATH",isElementPath);

    _pSettings->endGroup();
}
void AINGTAAppSettings::getLastLaunchInformation(QString& osLogPath, QString& osElementPath)
{
    _pSettings->beginGroup("USERSETTINGS");
    if(_pSettings->contains("LASTLOGPATH"))
        osLogPath= _pSettings->value("LASTLOGPATH").toString();
    if(_pSettings->contains("LASTLOGPATH"))
        osElementPath= _pSettings->value("LASTLAUNCHELEMPATH").toString();

    _pSettings->endGroup();
}

bool AINGTAAppSettings::getPrintTableStatus()
{
    bool bStatus = false;
    _pSettings->beginGroup("USERSETTINGS");
    if(_pSettings->contains("PRINT_TABLE_STATUS"))
        bStatus=_pSettings->value("PRINT_TABLE_STATUS").toString().toUpper() == "ON"?true:false;

    _pSettings->endGroup();
    return bStatus;
}

void AINGTAAppSettings::setPrintTableStatus(bool iState)
{
    _pSettings->beginGroup("USERSETTINGS");
    if(iState)
        _pSettings->setValue("PRINT_TABLE_STATUS","ON");
    else
        _pSettings->setValue("PRINT_TABLE_STATUS","OFF");

    _pSettings->endGroup();
}

bool AINGTAAppSettings::getInitSubscribeStartStatus()
{
    bool bStatus = true;
//    _pSettings->beginGroup("USERSETTINGS");
//    if(_pSettings->contains("INIT_SUBSCRIBE_START"))
//    {
//        QString data = value("INIT_SUBSCRIBE_START").toString().toUpper();
//        if(data == "ON")
//        {
//            bStatus = true;
//        }
//        else
//        {
//            bStatus = false;
//        }
//    }

//    _pSettings->endGroup();
    return bStatus;
}
void  AINGTAAppSettings::setInitSubscribeStartStatus(bool iState)
{
    iState = true;
    _pSettings->beginGroup("USERSETTINGS");
    if(iState)
        _pSettings->setValue("INIT_SUBSCRIBE_START","ON");
    else
        _pSettings->setValue("INIT_SUBSCRIBE_START","OFF");

    _pSettings->endGroup();
}


bool AINGTAAppSettings::getInitSubscribeEndStatus()
{
    bool bStatus = true;
//    _pSettings->beginGroup("USERSETTINGS");
//    if(_pSettings->contains("INIT_SUBSCRIBE_END"))
//    {
//        QString data = value("INIT_SUBSCRIBE_END").toString().toUpper();
//        if(data == "ON")
//        {
//            bStatus = true;
//        }
//        else
//        {
//            bStatus = false;
//        }
//    }

//    _pSettings->endGroup();
    return bStatus;
}
void  AINGTAAppSettings::setInitSubscribeEndStatus(bool iState)
{
    iState = true;
    _pSettings->beginGroup("USERSETTINGS");
    if(iState)
        _pSettings->setValue("INIT_SUBSCRIBE_END","ON");
    else
        _pSettings->setValue("INIT_SUBSCRIBE_END","OFF");

    _pSettings->endGroup();
}


bool AINGTAAppSettings::getNewLogFormatStatus()
{
    bool bStatus = false;
    _pSettings->beginGroup("USERSETTINGS");
    if(_pSettings->contains("NEW_LOG_FORMAT"))
    {
        QString data = _pSettings->value("NEW_LOG_FORMAT").toString().toUpper();
        if(data == "ON")
        {
            bStatus = true;
        }
        else
        {
            bStatus = false;
        }
    }

    _pSettings->endGroup();
    return bStatus;
}
void  AINGTAAppSettings::setNewLogFormatStatus(bool iState)
{
    _pSettings->beginGroup("USERSETTINGS");
    if(iState)
        _pSettings->setValue("NEW_LOG_FORMAT","ON");
    else
        _pSettings->setValue("NEW_LOG_FORMAT","OFF");

    _pSettings->endGroup();
}

bool AINGTAAppSettings::getManualActionTimeoutStatus()
{
    bool bStatus = false;
    _pSettings->beginGroup("USERSETTINGS");
    if(_pSettings->contains("MANUAL_ACTION_TIMEOUT_STATUS"))
    {
        QString data = _pSettings->value("MANUAL_ACTION_TIMEOUT_STATUS").toString().toUpper();
        if(data == "ON")
        {
            bStatus = true;
        }
        else
        {
            bStatus = false;
        }
    }

    _pSettings->endGroup();
    return bStatus;

}

bool AINGTAAppSettings::getUSCXMLBasedSCXMLEn()
{
    bool bStatus = false;
    _pSettings->beginGroup("USERSETTINGS");
    if(_pSettings->contains("USCXML_BASED_SCXML_EN"))
    {
        QString data = _pSettings->value("USCXML_BASED_SCXML_EN").toString().toUpper();
        if(data == "ON")
        {
            bStatus = true;
        }
        else
        {
            bStatus = false;
        }
    }

    _pSettings->endGroup();
    return bStatus;

}

void AINGTAAppSettings::setManualActionTimeoutStatus(bool iState)
{
    _pSettings->beginGroup("USERSETTINGS");
    if(iState)
        _pSettings->setValue("MANUAL_ACTION_TIMEOUT_STATUS","ON");
    else
        _pSettings->setValue("MANUAL_ACTION_TIMEOUT_STATUS","OFF");

    _pSettings->endGroup();
}

void AINGTAAppSettings::setSubscribeUnsubscribeTimeoutStatus(bool iState)
{
    _pSettings->beginGroup("USERSETTINGS");
    if(iState)
        _pSettings->setValue("SUBSCRIBE_UNSUBSCRIBE_TIMEOUT_STATUS","ON");
    else
        _pSettings->setValue("SUBSCRIBE_UNSUBSCRIBE_TIMEOUT_STATUS","OFF");

    _pSettings->endGroup();
}

bool AINGTAAppSettings::getSubscribeUnsubscribeTimeoutStatus()
{
    bool bStatus = false;
    _pSettings->beginGroup("USERSETTINGS");
    if(_pSettings->contains("SUBSCRIBE_UNSUBSCRIBE_TIMEOUT_STATUS"))
    {
        QString data = _pSettings->value("SUBSCRIBE_UNSUBSCRIBE_TIMEOUT_STATUS").toString().toUpper();
        if(data == "ON")
        {
            bStatus = true;
        }
        else
        {
            bStatus = false;
        }
    }

    _pSettings->endGroup();
    return bStatus;

}

void AINGTAAppSettings::setSubscribeUnsubscribeTimeoutValue(const QString& iDecimalPlaces)
{
    _pSettings->beginGroup("USERSETTINGS");
    _pSettings->setValue("SUBSCRIBE_UNSUBSCRIBE_TIMEOUT_VALUE",iDecimalPlaces);
    _pSettings->endGroup();
}

QString AINGTAAppSettings::getSubscribeUnsubscribeTimeoutValue()
{
    QString decimalPlace;
    _pSettings->beginGroup("USERSETTINGS");
    if(_pSettings->contains("SUBSCRIBE_UNSUBSCRIBE_TIMEOUT_VALUE"))
        decimalPlace = _pSettings->value("SUBSCRIBE_UNSUBSCRIBE_TIMEOUT_VALUE").toString();

    _pSettings->endGroup();
    return decimalPlace;
}

QStringList AINGTAAppSettings::getResultViewOptions()
{
    _pSettings->beginGroup("USERSETTINGS");
    QStringList ResultList;
    if(_pSettings->contains("RESULT_VIEW_SAVED_LIST"))
    {
        ResultList = _pSettings->value("RESULT_VIEW_SAVED_LIST").toString().split(":");
    }
    _pSettings->endGroup();
    return ResultList;
}

void AINGTAAppSettings::setResultViewOptions(const QStringList& iResultList)
{
    _pSettings->beginGroup("USERSETTINGS");
    QString ResultList;
    for (int i=0;i<iResultList.count();i++)
    {
        ResultList.append(QString("%1:").arg(iResultList.at(i)));
    }
    _pSettings->setValue("RESULT_VIEW_SAVED_LIST",ResultList);
    _pSettings->endGroup();
}

QStringList AINGTAAppSettings::getDVViewOptions()
{
    _pSettings->beginGroup("USERSETTINGS");
    QStringList ResultList;
    if(_pSettings->contains("DV_SAVED_LIST"))
    {
        ResultList = _pSettings->value("DV_SAVED_LIST").toString().split(":");
    }
    _pSettings->endGroup();
    return ResultList;
}

void AINGTAAppSettings::setDVViewOptions(const QStringList& iHeaderList)
{
    _pSettings->beginGroup("USERSETTINGS");
    QString HeaderList;
    for (int i=0;i<iHeaderList.count();i++)
    {
        HeaderList.append(QString("%1:").arg(iHeaderList.at(i)));
    }
    _pSettings->setValue("DV_SAVED_LIST",HeaderList);
    _pSettings->endGroup();
}

void AINGTAAppSettings::setLiveViewToolList(const QList<GTALiveToolParams> &iToolList)
{
    _pSettings->beginGroup("LIVEVIEW_USERSETTINGS");
    QString toolList;
    for(int i = 0; i<iToolList.count();i++)
    {
        toolList.append(QString("%1,%2,%3:").arg(iToolList.at(i).toolName, iToolList.at(i).toolIP, QString::number(iToolList.at(i).port)));
        qDebug()<<toolList.at(i);
    }
    _pSettings->setValue("TOOL_LIST",toolList);
    _pSettings->endGroup();
}

QList<GTALiveToolParams> AINGTAAppSettings::getLiveViewToolList()
{
    _pSettings->beginGroup("LIVEVIEW_USERSETTINGS");
    QList<QString> toolList, tempList;
    QList<GTALiveToolParams> gtaLiveToolList;
    GTALiveToolParams toolParam;
    if(_pSettings->contains("TOOL_LIST"))
    {
        toolList = _pSettings->value("TOOL_LIST").toString().split(":");
        toolList.removeLast();
        for(int i = 0; i< toolList.size(); i++)
        {
            tempList = toolList.at(i).split(',');
            toolParam.toolName = tempList.first();
            toolParam.toolIP = tempList.at(1);
            toolParam.port = tempList.last().toInt();
            gtaLiveToolList.insert(i, toolParam);
        }
    }
    _pSettings->endGroup();
    return gtaLiveToolList;
}

void AINGTAAppSettings::setGtaLiveBisgIP(const QString &iBisgIP)
{
    _pSettings->beginGroup("LIVEVIEW_USERSETTINGS");

    if(!iBisgIP.isEmpty())
        _pSettings->setValue("BISG_IP",iBisgIP);
    else
        _pSettings->setValue("BISG_IP","127.0.0.1");

    _pSettings->endGroup();
}

void AINGTAAppSettings::setGtaLiveBisgPort(int& iBisgPort)
{
    _pSettings->beginGroup("LIVEVIEW_USERSETTINGS");

    if (iBisgPort != 0)
        _pSettings->setValue("BISG_PORT",iBisgPort);
    else
        _pSettings->setValue("BISG_PORT",2405);

    _pSettings->endGroup();
}

QString AINGTAAppSettings::getGtaLiveBisgIP()
{
    QString bisgIP;
    _pSettings->beginGroup("LIVEVIEW_USERSETTINGS");
    if(_pSettings->contains("BISG_IP"))
        bisgIP = _pSettings->value("BISG_IP").toString();
    if(bisgIP.isEmpty())
        bisgIP = "127.0.0.1";
    _pSettings->endGroup();
    return bisgIP;
}

int AINGTAAppSettings::getGtaLiveBisgPort()
{
    _pSettings->beginGroup("LIVEVIEW_USERSETTINGS");
    QString port;
    int bisgPort;
    if(_pSettings->contains("BISG_PORT"))
        port = _pSettings->value("BISG_PORT").toString();
    if(port.isEmpty())
        port = "2405";

    bisgPort= port.toInt();
    _pSettings->endGroup();
    return bisgPort;
}

void AINGTAAppSettings::setUSCXMLBasedSCXMLEn(bool iState)
{
    _pSettings->beginGroup("USERSETTINGS");
    if(iState)
        _pSettings->setValue("USCXML_BASED_SCXML_EN","ON");
    else
        _pSettings->setValue("USCXML_BASED_SCXML_EN","OFF");

    _pSettings->endGroup();
}



QString AINGTAAppSettings::getManualActionTimeoutValue()
{
    QString decimalPlace;
    _pSettings->beginGroup("USERSETTINGS");
    if(_pSettings->contains("MANUAL_ACTION_TIMEOUT_VALUE"))
        decimalPlace = _pSettings->value("MANUAL_ACTION_TIMEOUT_VALUE").toString();

    _pSettings->endGroup();
    return decimalPlace;


}
void AINGTAAppSettings::setManualActionTimeoutValue(const QString& iDecimalPlaces)
{
    _pSettings->beginGroup("USERSETTINGS");
    _pSettings->setValue("MANUAL_ACTION_TIMEOUT_VALUE",iDecimalPlaces);
    _pSettings->endGroup();
}


void AINGTAAppSettings::setIgnoreManualActionStatus(bool iState)
{
    _pSettings->beginGroup("USERSETTINGS");
    if(iState)
        _pSettings->setValue("IGNORE_MANUAL_ACTION","ON");
    else
        _pSettings->setValue("IGNORE_MANUAL_ACTION","OFF");

    _pSettings->endGroup();

}

bool AINGTAAppSettings::getIgnoreManualActionStatus()
{
    bool bStatus = false;
    _pSettings->beginGroup("USERSETTINGS");
    if(_pSettings->contains("IGNORE_MANUAL_ACTION"))
    {
        QString data = _pSettings->value("IGNORE_MANUAL_ACTION").toString().toUpper();
        if(data == "ON")
        {
            bStatus = true;
        }
        else
        {
            bStatus = false;
        }
    }

    _pSettings->endGroup();
    return bStatus;
}

/**
 * Author: Saurav
 * This function saves the debug mode IP into the INI file.
 * @ival: IP for debug mode
*/
void AINGTAAppSettings::setDebugModeIP(QString &ival)
{
    _pSettings->beginGroup("USERSETTINGS");
    _pSettings->setValue("DEBUG_MODE_IP",ival);
    _pSettings->endGroup();
}

/**
 * Author: Saurav
 * This function gets the saved debug mode IP from the INI file.
 * @return: IP for debug mode
*/
QString AINGTAAppSettings::getdebugModeIPSelected()
{
    QString debugIP;
    _pSettings->beginGroup("USERSETTINGS");
    if(_pSettings->contains("DEBUG_MODE_IP"))
        debugIP = _pSettings->value("DEBUG_MODE_IP").toString();

    _pSettings->endGroup();
    return debugIP;
}

/**
 * Author: Saurav
 * This function saves the debug mode Port number into the INI file.
 * @ival: Port for debug mode
*/
void AINGTAAppSettings::setdebugPort(int ival)
{
    _pSettings->beginGroup("USERSETTINGS");
    _pSettings->setValue("DEBUG_MODE_PORT",QString::number(ival));
    _pSettings->endGroup();
}

/**
 * Author: Saurav
 * This function gets the saved debug mode Port from the INI file.
 * @return: Port for debug mode
*/
int AINGTAAppSettings::getdebugPortSelected()
{
    QString debugPort;
    _pSettings->beginGroup("USERSETTINGS");
    if(_pSettings->contains("DEBUG_MODE_PORT"))
        debugPort = _pSettings->value("DEBUG_MODE_PORT").toString();

    _pSettings->endGroup();
    bool ok;
    int port = debugPort.toInt(&ok);
    if (ok)
        return port;
    else
        return 65432;
}

/**
 * Author: Swarup
 * This function gets the validation enable info from the INI file.
 * @return: Status of validation enable
*/
bool AINGTAAppSettings::getValidationEnabled()
{
    bool validationEnable;
    _pSettings->beginGroup("USERSETTINGS");
    if(_pSettings->contains("VALIDATION_ENABLE"))
        validationEnable = _pSettings->value("VALIDATION_ENABLE").toBool();
    _pSettings->endGroup();
    return validationEnable;
}

/**
 * Author: Swarup
 * This function saves the validation enable info into the INI file.
 * @iValidationEnable: Status of validation enable
*/
void AINGTAAppSettings::setValidationEnabled(bool iValidationEnable)
{
    _pSettings->beginGroup("USERSETTINGS");
    _pSettings->setValue("VALIDATION_ENABLE",iValidationEnable);
    _pSettings->endGroup();
}

void AINGTAAppSettings::setResultPageGeometry(const QByteArray &state)
{
    _pSettings->setValue("Result_Page_Geometry",state);
}

QByteArray AINGTAAppSettings::getResultPageGeometry()
{
   QByteArray state;
    if(_pSettings->contains("Result_Page_Geometry"))
        state = _pSettings->value("Result_Page_Geometry").toByteArray();
    return state;
}

void AINGTAAppSettings::setDataTVGeometry(const QByteArray &state)
{
    _pSettings->setValue("DataTV_Geometry",state);
}

QByteArray AINGTAAppSettings::getDataTVGeometry()
{
   QByteArray state;
    if(_pSettings->contains("DataTV_Geometry"))
        state = _pSettings->value("DataTV_Geometry").toByteArray();
    return state;
}

void AINGTAAppSettings::setWindowState(const QByteArray &state)
{
    _pSettings->setValue("windowState", state);
}

QByteArray AINGTAAppSettings::getWindowState()
{
   QByteArray state;
    if(_pSettings->contains("windowState"))
        state = _pSettings->value("windowState").toByteArray();
    return state;
}

void AINGTAAppSettings::setGeometry(const QByteArray &state)
{
    QByteArray a = state;
    _pSettings->setValue("geometry",state);
}

QByteArray AINGTAAppSettings::getGeometry()
{
   QByteArray state;
    if(_pSettings->contains("geometry"))
        state = _pSettings->value("geometry").toByteArray();
    return state;
}

QString AINGTAAppSettings::getToolDataPath()
{
    QString folderPath;
    _pSettings->beginGroup("PATH");
    if(_pSettings->contains("TOOLDATAPATH"))
        folderPath = _pSettings->value("TOOLDATAPATH").toString();
    // To avoid some bugs after moving the GTA installer
    if (!QDir(folderPath).exists())
	{
        // We must delete the folderPath so as not to search for a path that does not exist !
		folderPath.clear();
	}
	if(folderPath.isEmpty())
    {
        folderPath = QDir::cleanPath(QString("%1%2%3").arg(QApplication::applicationDirPath(),QDir::separator(),APP_TOOL_DIR));
        _pSettings->setValue("TOOLDATAPATH",folderPath);
    }
    _pSettings->endGroup();
    return folderPath;
}


