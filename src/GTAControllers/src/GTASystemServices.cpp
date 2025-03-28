#include "GTASystemServices.h"
#include "GTAUtil.h"
#include "GTAControllers.h"
#include <GTAAppController.h>

#pragma warning(push, 0)
#include <QFile>
#include <QApplication>
#include <QSettings>
#include <QProcess>
#include <QTextStream>
#include <QDir>
#include <QDirIterator>
#include "QStringList"
#pragma warning(pop)

GTASystemServices* GTASystemServices::_pSystemServices = NULL;
GTASystemServices::GTASystemServices()
{
}

GTASystemServices::~GTASystemServices()
{
	/*if (_pSystemServices!=NULL)
		delete _pSystemServices;
	_pSystemServices=NULL;*/
}
void GTASystemServices::initialize()
{	 
	TestConfig* testConf = TestConfig::getInstance();
	_LibraryPath = QString::fromStdString(testConf->getLibraryPath());
	_RepositoryPath = QString::fromStdString(testConf->getRepositoryPath());
	_LogDirPath = QString::fromStdString(testConf->getLogDirPath());
	_GTAControllerFile = QString::fromStdString(testConf->getGTAControllerPath());
	_AutoLauncherWatchFolder = QString::fromStdString(testConf->getAutoLauncherPath());
	_AutoLauncherConfigPath = QString::fromStdString(testConf->getAutoLauncherConfigPath());
	
	_DatabasePath = QDir::cleanPath(QString("%1%2%3").arg(_LibraryPath, QDir::separator(), DATABASE_FILE));
	_FwcBDSDirPath = QDir::cleanPath(QString("%1%2%3").arg(_LibraryPath, QDir::separator(), FWC_DIR));
	_currentToolId = readAndGetToolIdFromXml();

	_LastEmologFilePath = QString::fromStdString(testConf->getLastLogPath());
	_LastEmoTestElemPath = QString::fromStdString(testConf->getLastLaunchElemPath());

	MakeApplicationPaths();
}

bool GTASystemServices::updateServiceSettings(
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
	bool ibSaveElementDuringExportScxml,
	bool ibChannelSelectionSetting,
	const QString& engineSetting,
	bool ibPirInvokeForSCXML,
	bool ibGenericSCXMLState,
	bool iPirWaitTime,
	const QString& iTime,
	bool isNewSCXMLStruct,
	bool iStandAloneExecMode,
	bool iEnInvokeForLocalVar,
	bool iNewLogFormat,
	bool iInitSubscribeStart,
	bool iInitSubscribeEnd,
	bool iUSCXMLBasedSCXMLEn,
	QString idebugIP,
	int iDebugPort,
	bool iValidationEnable,
	const QList<GTALiveToolParams>& liveToolList,
	QString bisgGTALiveIP,
	int bisgGTALivePort)

{
	TestConfig* testConf = TestConfig::getInstance();

	bool rc = false;
	initialize();

	if (pathExists(_LibraryPath) && pathExists(_RepositoryPath))
		rc = true;

	if (rc)
	{
		if (!_LogDirPath.isEmpty())
		{
			QFileInfo info(_LogDirPath);
			if (!info.exists())
			{
				_LogDirPath = getDefaultLogDirectory();
			}
		}
	}

	GTAUtil::setSchedulerType(iUSCXMLBasedSCXMLEn);

	testConf->setLogDirPath(_LogDirPath.toStdString());
	testConf->setLogMessages(iLogMessage.toStdString());
	testConf->setLogEnable(iLogEnable);
	testConf->setSaveBeforeExport(ibSaveElementDuringExportScxml);
	testConf->setChannelSelection(ibChannelSelectionSetting);
	testConf->setPIRInvokeForSCXML(ibPirInvokeForSCXML);
	testConf->setGenericSCXML(ibGenericSCXMLState);
	testConf->setPIRWaitTimeVal(iTime.toStdString());
	testConf->setPIRWaitTime(iPirWaitTime);
	testConf->setNewSCXMLStruct(isNewSCXMLStruct);
	testConf->setVirtualPMR(iVirtualPMR);
	testConf->setResizeRow(iResizeRow);
	testConf->setStandAloneExecMode(iStandAloneExecMode);
	testConf->setENInvokeLocalVar(iEnInvokeForLocalVar);
	testConf->setInitSubscribeStart(iInitSubscribeStart);
	testConf->setInitSubscribeEnd(iInitSubscribeEnd);
	testConf->setBenchDBStartup(iBenchDbStartUp);
	testConf->setProcDBStartup(iProcDbStartup);
	testConf->setPrintTableStatus(iPrintTableStatus);
	testConf->setNewLogFormat(iNewLogFormat);
	testConf->setManualActionTimeoutStatus(iManualActTimeoutStat);
	testConf->setManualActionTimeoutValue(iManualActTimeoutVal.toStdString());
	testConf->setUSCXMLBasedSCXMLEN(iUSCXMLBasedSCXMLEn);
	testConf->setDebugModeIP(idebugIP.toStdString());
	testConf->setDebugModePort(iDebugPort);
	testConf->setValidationEnable(iValidationEnable);

	//set GTA live BISG IP, Port & toolList
	QString toolListStr;
	for (int i = 0; i < liveToolList.count(); i++)
		toolListStr.append(QString("%1,%2,%3:").arg(liveToolList.at(i).toolName, liveToolList.at(i).toolIP, QString::number(liveToolList.at(i).port)));

	testConf->setToolList(toolListStr.toStdString());
	testConf->setBISGIP(bisgGTALiveIP.toStdString());
	testConf->setBISGPort(bisgGTALivePort);

	//GTAUtil::setSchedulerType(iUSCXMLBasedSCXMLEn);

	if (!iDecimalPlace.isEmpty())
	{
		testConf->setDecimalPlaces(iDecimalPlace.toStdString());
		_DecimalPlaceSetting = iDecimalPlace;
	}
	if (engineSetting.isEmpty() == false)
		testConf->setEngineSelection(engineSetting.toStdString());
	
	return rc;
}

bool GTASystemServices::pathExists(const QString& iFilePath)
{
	if (!iFilePath.isEmpty())
	{
		QFileInfo info(iFilePath);
		if (!info.exists())
			return false;

	}
	else
		return false;

	return true;
}


QString GTASystemServices::getScxmlLaunchExeFilePath()const
{
	return QDir::cleanPath(QString("%1%2%3").arg(getToolDataPath(), QDir::separator(), LAUNCH_SCXMLEXE_FILE_NAME));
}

QString GTASystemServices::getCommandPropertyConfigXMLPath()const
{
	return QDir::cleanPath(QString("%1%2%3").arg(getToolDataPath(), QDir::separator(), COMMAND_PROPERTY_XML));
}
QString GTASystemServices::getScxmlExitExeFilePath()const
{
	return QDir::cleanPath(QString("%1%2%3").arg(getToolDataPath(), QDir::separator(), KILL_SCXMLEXE_FILE_NAME));
}

QString GTASystemServices::getInternalParamInfoFilePath()const
{
	return QDir::cleanPath(QString("%1%2%3").arg(getToolDataPath(), QDir::separator(), INTERNAL_PARAM_INFO_XML));
}
QString GTASystemServices::getDatabaseFile()
{
	_DatabasePath = QDir::cleanPath(QString("%1%2%3").arg(_LibraryPath, QDir::separator(), DATABASE_FILE));
	return _DatabasePath;

}

void GTASystemServices::MakeApplicationPaths()
{
	createRepositoryPath();
	createLibraryPath();
}

QString GTASystemServices::getHeaderTemplateFilePath(const QString& isProgram, const QString& isEquipment, const QString& isStandard) const
{
	QString headerFilePath;
	QString headerTemplateFolderPath = _LibraryPath;
	headerTemplateFolderPath.append(QString("/Header_Templates"));
	QString headerFileName = isProgram + QString("_") + isEquipment + QString("_") + isStandard + QString(".xml");
	headerFilePath = QString("%1/%2").arg(headerTemplateFolderPath, headerFileName);
	return headerFilePath;
}
QString GTASystemServices::getHeaderTemplateFilePath() const
{
	QString headerFilePath = QString("%1%2%3%2%4").arg(getToolDataPath(), QDir::separator(), HDR_TEMPLATE_DIR, HDR_TEMPLATE_FILE);
	return QDir::cleanPath(headerFilePath);
}


QString GTASystemServices::getPIRDirectory()
{
	QString dir = QDir::cleanPath(QString("%1%2%3").arg(_LibraryPath, QDir::separator(), PIR_DIR));
	return dir;
}

QString GTASystemServices::getAASDirectory()
{
	return getPIRDirectory();
}

QStringList GTASystemServices::getAASFiles()
{
	QStringList oFiles;
	QString pirDir = getAASDirectory();
	QDir dir(pirDir);
	QStringList files = dir.entryList(QStringList("*.aas"), QDir::Files | QDir::NoSymLinks);
	foreach(QString file, files)
	{
		QString fullPath = QDir::cleanPath(pirDir + "/" + file);
		oFiles.append(fullPath);
	}
	return oFiles;
}

QStringList GTASystemServices::getPIRFiles()
{
	QStringList oFiles;
	QString pirDir = getPIRDirectory();
	QDir dir(pirDir);
	QStringList files = dir.entryList(QStringList("*.pir"), QDir::Files | QDir::NoSymLinks);
	foreach(QString file, files)
	{
		QString fullPath = QDir::cleanPath(pirDir + "/" + file);
		oFiles.append(fullPath);
	}
	return oFiles;
}

QString GTASystemServices::getPMRDirectory()
{
	QString dir = QDir::cleanPath(QString("%1%2%3").arg(_LibraryPath, QDir::separator(), PMR_DIR));
	return dir;
}
QString GTASystemServices::getModelICDDir()
{
	QString dir = QDir::cleanPath(QString("%1%2%3").arg(_LibraryPath, QDir::separator(), MODEL_ICD_DIR));
	return dir;
}
QString GTASystemServices::getVCOMDir()
{
	QString dir = QDir::cleanPath(QString("%1%2%3").arg(_LibraryPath, QDir::separator(), VCOM_DIR));
	return dir;
}
QStringList GTASystemServices::getPMRFiles()
{
	QStringList oFiles;
	QString pirDir = getPMRDirectory();
	QDir dir(pirDir);
	QStringList files = dir.entryList(QStringList("*.xml"), QDir::Files | QDir::NoSymLinks);
	foreach(QString file, files)
	{
		QString fullPath = QDir::cleanPath(pirDir + "/" + file);
		oFiles.append(fullPath);
	}
	return oFiles;
}
QStringList GTASystemServices::getModelICDFiles()
{
	QStringList oFiles;
	QString pirDir = getModelICDDir();
	QDir dir(pirDir);
	QStringList files = dir.entryList(QStringList("*.xml"), QDir::Files | QDir::NoSymLinks);
	foreach(QString file, files)
	{
		QString fullPath = QDir::cleanPath(pirDir + "/" + file);
		oFiles.append(fullPath);
	}
	return oFiles;
}
QStringList GTASystemServices::getVCOMFiles()
{
	QStringList oFiles;
	QString pirDir = getVCOMDir();
	QDir dir(pirDir);
	QStringList files = dir.entryList(QStringList("*.xml"), QDir::Files | QDir::NoSymLinks);
	foreach(QString file, files)
	{
		QString fullPath = QDir::cleanPath(pirDir + "/" + file);
		oFiles.append(fullPath);
	}
	return oFiles;
}
QString GTASystemServices::getICDDirectory()
{
	QString dir = QDir::cleanPath(QString("%1%2%3").arg(_LibraryPath, QDir::separator(), ICD_DIR));
	return dir;
}
QString GTASystemServices::getLibraryPath()const
{
	QString dir = QDir::cleanPath(QString("%1").arg(_LibraryPath));
	return dir;
}
QStringList GTASystemServices::getICDFiles()
{
	QStringList oFiles;
	QString pirDir = getICDDirectory();
	QDir dir(pirDir);
	QStringList files = dir.entryList(QStringList("*.csv"), QDir::Files | QDir::NoSymLinks);
	foreach(QString file, files)
	{
		QString fullPath = QDir::cleanPath(pirDir + "/" + file);
		oFiles.append(fullPath);
	}
	return oFiles;
}
QString GTASystemServices::getImageDirectory() const
{
	QString dir = QDir::cleanPath(QString("%1%2%3").arg(_LibraryPath, QDir::separator(), PIC_DIR));
	return dir;
}
QStringList GTASystemServices::getImageFiles() const
{
	QStringList oFiles;
	QString path = getImageDirectory();
	QDir dir(path);
	QStringList files = dir.entryList(QStringList("*.jpg"), QDir::Files | QDir::NoSymLinks);
	foreach(QString file, files)
	{
		QString fullPath = QDir::cleanPath(path + "/" + file);
		oFiles.append(fullPath);
	}
	return oFiles;
}
QString GTASystemServices::getApplicationConfigXmlFilePath()
{
	bool isGenericSCXML = TestConfig::getInstance()->getGenericSCXML();

	QString appDirPath = QApplication::applicationDirPath();
	QString xmlFile = "";

	if (!isGenericSCXML)
		xmlFile = QDir::cleanPath(QString("%1/%2").arg(appDirPath, "GTA.xml"));
	else
		xmlFile = QDir::cleanPath(QString("%1/%2").arg(appDirPath, "GTA_Generic.xml"));
	return xmlFile;
}
QString GTASystemServices::getFavoritesFilePath()const
{
	QString xmlFile = QDir::cleanPath(QString("%1%2%3").arg(_RepositoryPath, QDir::separator(), FAV_FILENAME));
	return xmlFile;
}
bool GTASystemServices::createRepositoryPath()
{
	bool rc = true;

	QString logDir = getLogDirectory();
	QString dataDir = getDataDirectory();
	QString externDir = getExternalDataDirectory();
	QString templDir = getTemplateDir();
	QString scxmlDir = getScxmlDirectory();
	QString exportPath = getExportDocDirectory();

	QDir dir;
	if (rc && (!QDir(logDir).exists()))
		rc = dir.mkpath(logDir);
	if (rc && (!QDir(dataDir).exists()))
		rc = dir.mkpath(dataDir);
	if (rc && (!QDir(externDir).exists()))
		rc = dir.mkpath(externDir);
	if (rc && (!QDir(templDir).exists()))
		rc = dir.mkpath(templDir);
	if (rc && (!QDir(scxmlDir).exists()))
		rc = dir.mkpath(scxmlDir);
	if (rc && (!QDir(exportPath).exists()))
		rc = dir.mkpath(exportPath);
	return rc;
}
bool GTASystemServices::createLibraryPath()
{
	bool rc = true;

	QString icdDirectory = getICDDirectory();
	QString pmrDir = getPMRDirectory();
	QString pirDir = getPIRDirectory();
	QString micdDir = getMICDPath();
	QString irtLibPath = getIRTLibPath();
	QString audioRecPath = getAudioRecogConfigPath();
	QString oneClickConfigPath = getOneClickConfigPath();
	QString fwcPath = getFwcBDSFilePath();
	QString vcom = getVCOMDir();
	QString modelIcd = getModelICDDir();
	QString toolConfigPath = getToolConfigurationPath();
	QString genericDBPath = getGenericDBPath();
	QString tablesPath = getTableDirectory();

	QDir dir;
	if (rc && (!QDir(icdDirectory).exists()))
		rc = dir.mkpath(icdDirectory);
	if (rc && (!QDir(pmrDir).exists()))
		rc = dir.mkpath(pmrDir);
	if (rc && (!QDir(pirDir).exists()))
		rc = dir.mkpath(pirDir);
	if (rc && (!QDir(micdDir).exists()))
		rc = dir.mkpath(micdDir);
	if (rc && (!QDir(irtLibPath).exists()))
		rc = dir.mkpath(irtLibPath);
	if (rc && (!QDir(audioRecPath).exists()))
		rc = dir.mkpath(audioRecPath);
	if (rc && (!QDir(oneClickConfigPath).exists()))
		rc = dir.mkpath(oneClickConfigPath);
	if (rc && (!QDir(fwcPath).exists()))
		rc = dir.mkpath(fwcPath);
	if (rc && (!QDir(vcom).exists()))
		rc = dir.mkpath(vcom);
	if (rc && (!QDir(modelIcd).exists()))
		rc = dir.mkpath(modelIcd);
	if (rc && (!QDir(toolConfigPath).exists()))
		rc = dir.mkpath(toolConfigPath);
	if (rc && (!QDir(genericDBPath).exists()))
		rc = dir.mkpath(genericDBPath);
	if (rc && (!QDir(tablesPath).exists()))
		rc = dir.mkpath(tablesPath);
	return rc;
}
QString GTASystemServices::getDataDirectory() const
{
	//QString outDir = QDir::cleanPath(QString("%1/%2/%3/%4/").arg(_RepositoryPath,getProgram(),getEquipment(),getStandard()));
	QString dataDir = QDir::cleanPath(QString("%1%2%3").arg(_RepositoryPath, QDir::separator(), DATA_DIR));
	return dataDir;
}

QString GTASystemServices::getDefaultLogDirectory() const
{
	QString dataDir = QDir::cleanPath(QString("%1%2%3").arg(_RepositoryPath, QDir::separator(), LOG_DIR));
	return dataDir;
}
QString GTASystemServices::getLogDirectory() const
{
	//QString outDir = QDir::cleanPath(QString("%1/%2/%3/%4/").arg(_RepositoryPath,getProgram(),getEquipment(),getStandard()));
	return QDir::cleanPath(_LogDirPath);
}

/**
* The function return the GTA Controller exe path i.e. GTA Controller exe path in Configuration setting
* return GTA Controller exe path if exists in configuration settings otherwise empty string is returned
*/
QString GTASystemServices::getGTAControllerFilePath() const
{
	return QDir::cleanPath(_GTAControllerFile);
}

/**
* The function return the AutoLauncher Watch Folder path i.e. AutoLauncher Watch Folder path in Configuration setting
* return AutoLauncher Watch folder path if exists in configuration settings otherwise empty string is returned
*/
QString GTASystemServices::getAutoLauncherWatchDirectory() const
{
	return QDir::cleanPath(_AutoLauncherWatchFolder);
}

/**
* The function return the AutoLauncher Config File Path i.e. AutoLauncher Config File path in Configuration setting
* @return AutoLauncher Config File path if exists in configuration settings otherwise empty string is returned
*/
QString GTASystemServices::getAutoLauncherConfigPath() const
{
	return QDir::cleanPath(_AutoLauncherConfigPath);
}

void GTASystemServices::setLogDirectory(const QString& iPath)
{
	_LogDirPath = iPath;
}

QString GTASystemServices::getScxmlDirectory()
{
	//QString outDir = QDir::cleanPath(QString("%1/%2/%3/%4/%5").arg(_RepositoryPath,getProgram(),getEquipment(),getStandard(),SCXML_DIR));
	QString outDir = QDir::cleanPath(QString("%1%2%3").arg(_RepositoryPath, QDir::separator(), SCXML_DIR));
	return outDir;
}
QString GTASystemServices::getExportDocDirectory()
{
	//QString outDir = QDir::cleanPath(QString("%1/%2/%3/%4/%5").arg(_RepositoryPath,getProgram(),getEquipment(),getStandard(),SCXML_DIR));
	QString outDir = QDir::cleanPath(QString("%1%2%3").arg(_RepositoryPath, QDir::separator(), EXPORT_ELEMENT_DIR));
	return outDir;
}
QString GTASystemServices::getExternalDataDirectory() const
{
	// QString outDir = QDir::cleanPath(QString("%1/%2/%3/%4").arg(_RepositoryPath,getProgram(),getEquipment(),getStandard()));
	QString externDir = QDir::cleanPath(QString("%1%2%3").arg(_RepositoryPath, QDir::separator(), EXTERNAL_DATA_DIR));
	return externDir;
}
QString GTASystemServices::getTemplateDir() const
{
	//QString outDir = QDir::cleanPath(QString("%1/%2/%3/%4").arg(_RepositoryPath,getProgram(),getEquipment(),getStandard()));
	QString templDir = QDir::cleanPath(QString("%1%2%3").arg(_RepositoryPath, QDir::separator(), TEMPLATE_DIR));
	return templDir;
}

QString GTASystemServices::getNewSCXMLTemplateDirectory()
{
	return  QDir::cleanPath(QString("%1%2%3").arg(getToolDataPath(), QDir::separator(), SCXML_TEMPLATE_DIR));
}
QString GTASystemServices::getScxmlTemplateDir()
{
	return  QDir::cleanPath(QString("%1%2%3").arg(getToolDataPath(), QDir::separator(), SCXML_TEMPLATE_DIR));
}

QStringList GTASystemServices::getDocumentByExtension(const QString dirPath, const QString iExtn) const
{
	QStringList paths;
	QDir dataDir(dirPath);
	QFileInfoList files = dataDir.entryInfoList(QStringList(iExtn), QDir::Files);
	for (int i = 0; i < files.count(); i++)
	{
		QFileInfo file = files.at(i);
		paths << file.absoluteFilePath();
	}
	return paths;
	//    return dataDir.entryList(QStringList(iExtn),QDir::Files);
}

//recursive search
QStringList GTASystemServices::getDocumentByExtension(const QString iExtn) const
{
	QString dirPath = getDataDirectory();
	QDir dataDir(dirPath);

	QStringList usedFileList;
	QStringList filesList = getDocumentByExtension(dirPath, iExtn);
	usedFileList.append(filesList);

	dataDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
	dataDir.setSorting(QDir::Name);
	QFileInfoList folders = dataDir.entryInfoList();


	for (int i = 0; i < folders.count(); i++)
	{
		QFileInfo file = folders.at(i);
		if (file.isDir())
		{
			QString path = file.absoluteFilePath();
			QStringList fileList = getDocumentByExtension(path, iExtn);
			usedFileList.append(fileList);
		}
	}
	return usedFileList;
}

//QStringList GTASystemServices::getDocumentByExtension(const QString iExtn) const
//{
//    QString dataPath = getDataDirectory();
//    QDir dataDir(dataPath);
//    return dataDir.entryList(QStringList(iExtn),QDir::Files);// | QDir::AllDirs | QDir::NoDotAndDotDot,QDir::Name);
//QStringList fileList;
//QDirIterator it(dataPath, QStringList() << iExtn, QDir::Files) QDirIterator::Subdirectories);
//while (it.hasNext())
//{
//	QString fileName = it.next();
//	QFileInfo info(fileName);
//	QString absPath = info.absoluteFilePath();
//	absPath = absPath.replace(dataPath+"/","");
//	fileList << absPath;

//}
//return fileList;


//}
bool GTASystemServices::isGenerateGenericSCXML()
{
	return TestConfig::getInstance()->getGenericSCXML();
}

QString GTASystemServices::getRoboArmSequenceFilePath(const QString& iFileName)
{
	QString oFilePath = QString("%1/%2").arg(getRoboArmDirectory(), iFileName);

	return QDir::cleanPath(oFilePath);
}
QString GTASystemServices::getIRTSequenceFilePath(const QString& iFileName)
{
	QString oFilePath = QString("%1/%2").arg(getIRTLibPath(), iFileName);

	return QDir::cleanPath(oFilePath);
}

QString GTASystemServices::getOneClickConfigFilePath(const QString& iFileName)
{
	QString oFilePath = QString("%1/%2").arg(getOneClickConfigPath(), iFileName);
	return QDir::cleanPath(oFilePath);
}
QString GTASystemServices::getIRTLibPath() const
{
	QString oPath = QString("%1%2%3%2IRT%2Sequences").arg(_LibraryPath, QDir::separator(), LIB_TOOL_DIR);
	return QDir::cleanPath(oPath);
}
QString GTASystemServices::getOneClickConfigPath() const
{
	QString oPath = QString("%1%2%3%2OneClick%2Config").arg(_LibraryPath, QDir::separator(), LIB_TOOL_DIR);
	return QDir::cleanPath(oPath);
}
QString GTASystemServices::getOneClickLaunchAppConfigPath() const
{
	QString oPath = QString("%1%2%3%2OneClick%2LaunchApp").arg(_LibraryPath, QDir::separator(), LIB_TOOL_DIR);
	return QDir::cleanPath(oPath);
}

QString GTASystemServices::getAudioRecogConfigPath() const
{
	QString oPath = QString("%1%2%3%2AudioRecog").arg(_LibraryPath, QDir::separator(), LIB_TOOL_DIR);
	return QDir::cleanPath(oPath);
}
QString GTASystemServices::getToolConfigurationPath() const
{
	QString oPath = QString("%1%2%3").arg(_LibraryPath, QDir::separator(), LIB_TOOL_CONFIG_DIR/*,QDir::separator()*/);
	return QDir::cleanPath(oPath);
}

QString GTASystemServices::getGenericDBPath() const
{
	QString oPath = QString("%1%2%3").arg(_LibraryPath, QDir::separator(), LIB_GENERIC_DB_DIR/*,QDir::separator()*/);
	return QDir::cleanPath(oPath);
}

QString GTASystemServices::getRoboArmDirectory() const
{
	QString oPath = QString("%1%2%3%2Robotic Arm%2Sequences").arg(_LibraryPath, QDir::separator(), LIB_TOOL_DIR);
	return QDir::cleanPath(oPath);
}
/**
  * Return the list of file name of IRT sequence file
  */
QStringList GTASystemServices::getIRTSeqeunceFileList() const
{
	QString dataPath = getIRTLibPath();
	QDir dataDir(dataPath);
	QStringList fileList = dataDir.entryList(QStringList("*.xml"), QDir::Files, QDir::Name);
	QStringList items;
	foreach(QString file, fileList)
	{
		items.append(file);
	}
	return items;
}
QStringList GTASystemServices::getRoboArmSeqeunceFileList() const
{
	QString dataPath = getRoboArmDirectory();
	QDir dataDir(dataPath);
	QStringList fileList = dataDir.entryList(QStringList("*.xml"), QDir::Files, QDir::Name);
	QStringList items;
	foreach(QString file, fileList)
	{
		items.append(file);
	}
	return items;
}
QStringList GTASystemServices::getOneClickLauncAppConfigFileList() const
{
	QString dataPath = getOneClickLaunchAppConfigPath();
	QDir dataDir(dataPath);
	QStringList fileList = dataDir.entryList(QStringList("*.xml"), QDir::Files);
	QStringList items;
	foreach(QString file, fileList)
	{
		items.append(file);
	}
	return items;
}
QStringList GTASystemServices::getOneClickConfigFileList() const
{
	QString dataPath = getOneClickConfigPath();
	QDir dataDir(dataPath);
	QStringList fileList = dataDir.entryList(QStringList("*.xml"), QDir::Files, QDir::Name);
	QStringList items;
	foreach(QString file, fileList)
	{
		items.append(file);
	}
	return items;
}

bool GTASystemServices::createTemplateDbFile()
{
	bool rc = false;
	QFile dbFile(_DatabasePath);
	if (!dbFile.exists())
	{
		const QString DRIVER("QSQLITE");
		if (QSqlDatabase::isDriverAvailable(DRIVER)) {
			QSqlDatabase db = QSqlDatabase::addDatabase(DRIVER,"Parameter_db");
			db.setDatabaseName(_DatabasePath);
			QSqlQuery query;
			rc = db.open();
			if (rc)
				query.exec("CREATE TABLE IF NOT EXISTS PARAMETERS "
					"(NAME varchar(255) PRIMARY KEY,"
					"UPPERNAME varchar(255),"
					"PARAMTYPE varchar(255),"
					"SIGNALTYPE varchar(255),"
					"UNIT varchar(255),"
					"VALUETYPE varchar(255),"
					"MNVAL varchar(255),"
					"MXVAL varchar(255),"
					"LABEL varchar(255),"
					"PARAMETER_LOCAL_NAME varchar(255),"
					"BIT_POS varchar(255),"
					"EQUIPNAME varchar(255),"
					"FILE varchar(255),"
					"FILELASTMODIFIED varchar(255),"
					"MESSAGE varchar(255),"
					"FS_NAME varchar(255),"
					"SUFFIX varchar(255),"
					"PRECISION varchar(255),"
					"DIRECTION varchar(255),"
					"MEDIA_TYPE	varchar(255),"
					"TOOL_NAME varchar(255),"
					"TOOL_TYPE varchar(255),"
					"REFRESH_RATE varchar(255))");
			else
				rc = false;
		}
	}
	return rc;
}

QString GTASystemServices::getUserDatabasePath()
{
	QString sessionDbPath = QDir::cleanPath(QString("%1%2%3%2%4").arg(getToolDataPath(), QDir::separator(), USER_DATA_DIR, USER_DATA_DATABASE));
	if (pathExists(sessionDbPath))
	{
		return sessionDbPath;
	}
	else
	{
		return QString("");
	}

}

QString GTASystemServices::getSessionInfoFilePath()
{
	QString sessionInfoFilePath = QDir::cleanPath(QString("%1%2%3%2%4").arg(getToolDataPath(), QDir::separator(), USER_DATA_DIR, GTA_SESSION_INFO));
	if (pathExists(sessionInfoFilePath))
	{
		return sessionInfoFilePath;
	}
	else
	{
		return QString("");
	}
}

bool GTASystemServices::renameFile(const QString& iOldFileName, const QString& iNewFileName, QString* oNewName)
{
	bool rc = false;
	QFile dbFile(iOldFileName);
	QFileInfo dbFileInfo(iOldFileName);
	*oNewName = dbFileInfo.absolutePath() + QDir::separator() + iNewFileName;
	if (dbFile.exists())
	{
		rc = dbFile.rename(iOldFileName, *oNewName);
	}
	return rc;
}

bool GTASystemServices::removeFile(const QString& iFile)
{
	bool rc = false;
	QFile dbFile(iFile);
	if (dbFile.exists())
	{
		rc = dbFile.remove(iFile);
	}
	return rc;
}

QString GTASystemServices::getFwcBDSFilePath()
{
	return _FwcBDSDirPath;
}
QString GTASystemServices::getLTRATemplateFolderPath() const
{
	return QDir::cleanPath(QString("%1%2%3").arg(getToolDataPath(), QDir::separator(), LTRA_TEMPLATES_DIR));
}
QString GTASystemServices::getLTRATemplateFilePath() const
{
	return QDir::cleanPath(QString("%1%2%3%2%4").arg(getToolDataPath(), QDir::separator(), LTRA_TEMPLATES_DIR, LTRA_TEMPLATE_FILE_NAME));
}
QString GTASystemServices::getTableDirectory() const
{
	QString path = QDir::cleanPath(QString("%1%2%3").arg(_RepositoryPath, QDir::separator(), TABLES_DIR));
	return path;
}
QString GTASystemServices::getTemporaryDirectory() const
{

	QString tempDirPath = QDir::cleanPath(QString("%1%2%3").arg(_RepositoryPath, QDir::separator(), "TEMP"));
	QFile tmpDir(tempDirPath);
	QFileInfo tmpDirInfo(tmpDir);
	if (!tmpDirInfo.exists())
	{
		QDir repDir(QDir::cleanPath(_RepositoryPath));
		repDir.mkdir("TEMP");
	}

	return tempDirPath;
}

QString GTASystemServices::getMICDPath()const
{
	QString path;
	path = QDir::cleanPath(QString("%1%2%3").arg(_LibraryPath, QDir::separator(), MICD_DIR));
	return path;

}
QStringList GTASystemServices::getAudioRecogConfigFileList() const
{
	QString dataPath = getAudioRecogConfigPath();
	QDir dataDir(dataPath);
	QStringList fileList = dataDir.entryList(QStringList("*.txt"), QDir::Files, QDir::Name);
	QStringList items;
	foreach(QString file, fileList)
	{
		if (!file.isEmpty())
			items.append(QString("%1%2%3").arg(getAudioRecogConfigPath(), QDir::separator(), file));
	}
	return items;
}
GTASystemServices* GTASystemServices::getSystemServices()
{
	if (NULL == _pSystemServices)
		_pSystemServices = new GTASystemServices;

	return _pSystemServices;
}

bool GTASystemServices::toolFilesExists(QString& errorMsg)
{
	errorMsg = QString();
	bool rc = false;
	QString sHeaderPath = getHeaderTemplateFilePath();
	if (!pathExists(sHeaderPath))
	{
		errorMsg = "Failed to locate Header template file";
		return rc;
	}

	QString sLTRATemplatePath = getLTRATemplateFilePath();
	if (!pathExists(sLTRATemplatePath))
	{
		errorMsg = "Failed to locate LTRA template";
		return rc;
	}

	QString scxmlTemplatePath = getScxmlTemplateDir();
	if (!pathExists(scxmlTemplatePath))
	{
		errorMsg = "scxml template folder not found";
		return rc;
	}


	return true;

}
//QString GTASystemServices::getSVNWorkingDirectory()
//{
//    return _SvnWorkingCopyPath;
//}
void GTASystemServices::svnCommitBeforeExit()
{


}

QString GTASystemServices::getTemplateFilePath(const QString& iTmplFileName)
{
	QString fileName = iTmplFileName;
	fileName.replace(" ", "_");
	QString templateFilePath = QDir::cleanPath(QString("%1%2%3%4%5.tmpl").arg(_RepositoryPath, QDir::separator(), TEMPLATE_DIR, QDir::separator(), fileName));
	return templateFilePath;
}
void GTASystemServices::commitLastLaunchPath()
{
	TestConfig* testConf = TestConfig::getInstance();
	_LastEmologFilePath = QString::fromStdString(testConf->getLastLogPath());
	_LastEmoTestElemPath = QString::fromStdString(testConf->getLastLaunchElemPath());
}
void GTASystemServices::setLastEmoLogPath(const QString& iLastLogFile, const QString& iLastEmoTestElemPath)
{
	_LastEmologFilePath = iLastLogFile;
	_LastEmoTestElemPath = iLastEmoTestElemPath;

}
QString GTASystemServices::getGenestaSymbolDefinitionFilePath()
{
	return QDir::cleanPath(QString("%1%2%3").arg(getToolDataPath(), QDir::separator(), GENESTA_SYMBOL_FILE));
}
QStringList GTASystemServices::setBenchDBConfigFiles(const QString& iniFilePath, bool removeOldEntries)
{

	QString batchConfigDirectory = QDir::cleanPath(QString("%1%2%3").arg(_LibraryPath, QDir::separator(), "BENCH_CONFIG"));
	QDir benchDirObj(batchConfigDirectory);

	if (!benchDirObj.exists())
	{
		//bool rc = 
		benchDirObj.mkpath(batchConfigDirectory);
		// int a = 10;
	}
	QStringList  batchConfigdbFiles;
	QStringList  batchFilesNotFound;

	if (removeOldEntries)
	{


		//remove Old Entries
		QFileInfoList fileInfoList = benchDirObj.entryInfoList(QDir::Files);
		foreach(QFileInfo absPath, fileInfoList)
		{
			if (absPath.exists())
			{
				QString filePath = absPath.absoluteFilePath();
				benchDirObj.remove(filePath);
			}
		}
	}



	QSettings batchConfigSettings(QDir::cleanPath(iniFilePath), QSettings::IniFormat);

	QString oPath;
	batchConfigSettings.beginGroup("ICD_FILES");
	QStringList groupKeys = batchConfigSettings.childKeys();
	foreach(QString key, groupKeys)
	{
		if (key.contains("file_name", Qt::CaseInsensitive))
		{
			QByteArray sFilePath = batchConfigSettings.value(key).toByteArray();
			QString sICDFilePath = QDir::cleanPath(QString(sFilePath));
			QFileInfo info(sICDFilePath);
			if (info.exists())
				batchConfigdbFiles.append(sICDFilePath);
			else
				batchFilesNotFound.append(sICDFilePath);


		}
	}

	batchConfigSettings.endGroup();
	groupKeys.clear();
	batchConfigSettings.beginGroup("ADIS_FILES");
	groupKeys = batchConfigSettings.childKeys();
	foreach(QString key, groupKeys)
	{
		if (key.contains("file_name", Qt::CaseInsensitive))
		{
			QByteArray sFilePath = batchConfigSettings.value(key).toByteArray();
			QString sICDFilePath = QDir::cleanPath(QString(sFilePath));
			QFileInfo info(sICDFilePath);
			if (info.exists())
				batchConfigdbFiles.append(sICDFilePath);
			else
				batchFilesNotFound.append(sICDFilePath);
		}
	}

	//copy the files to batchConfigDirectory
	foreach(QString sFilePath, batchConfigdbFiles)
	{
		QFileInfo info(sFilePath);
		QString toCopyPath = QDir::cleanPath(QString("%1%2%3").arg(batchConfigDirectory, QDir::separator(), info.fileName()));
		QFile::copy(sFilePath, toCopyPath);
	}

	batchConfigSettings.endGroup();


	return batchFilesNotFound;
}
QString GTASystemServices::getGenericToolSchemaFile() const
{
	return QDir::cleanPath(QString("%1%2%3").arg(getToolDataPath(), QDir::separator(), "GENERIC_TOOL.xsd"));
}
QStringList GTASystemServices::getBenchDBConfigFiles()
{
	QStringList filePaths;
	QString batchConfigDirectory = QDir::cleanPath(QString("%1%2%3").arg(_LibraryPath, QDir::separator(), "BENCH_CONFIG"));
	QDir bechDirObj(batchConfigDirectory);
	QFileInfoList fileInfoList = bechDirObj.entryInfoList(QDir::Files);
	foreach(QFileInfo absPath, fileInfoList)
	{
		if (absPath.exists())
		{
			QString filePath = absPath.absoluteFilePath();
			filePaths.append(filePath);
		}
	}
	return filePaths;
}

QString GTASystemServices::getEquipmentToolMapFilePath()
{
	return QDir::cleanPath(QString("%1%2%3").arg(getToolDataPath(), QDir::separator(), EQUIPMENT_TOOL_MAP_FILE));
}

QString GTASystemServices::getSCXMLToolIDFilePath()
{
	return QDir::cleanPath(QString("%1%2%3").arg(getToolDataPath(), QDir::separator(), SCXML_TOOL_ID_FILE));
}

QString GTASystemServices::getTempEquipmentQueryFilePath()
{
	return QDir::cleanPath(QString("%1%2%3").arg(getToolDataPath(), QDir::separator(), TEMP_EQUIPMENT_QUERY_FILE));
}

QStringList GTASystemServices::getTableFiles()
{
	QStringList oFiles;
	QString tableDir = getTableDirectory();
	QDir dir(tableDir);
	QStringList files = dir.entryList(QStringList("*.csv"), QDir::Files | QDir::NoSymLinks);
	foreach(QString file, files)
	{
		QString fullPath = QDir::cleanPath(tableDir + "/" + file);
		oFiles.append(fullPath);
	}
	return oFiles;
}

QString GTASystemServices::getToolDataPath() const
{
	return QString::fromStdString(TestConfig::getInstance()->getToolDataPath());
}

/**
  * @brief: returns the powershell script path to identify if a server is active or not.
  *         the script is created in tghe Tool_Data path if it is not available there.
  * @return: Path of the powershell script to check server status.
  */
QString GTASystemServices::getServerCheckPowershellFilePath()const
{
	QString path = QDir::cleanPath(QString("%1%2%3").arg(getToolDataPath(), QDir::separator(), CHECK_REMOTE_SERVER));
	QFileInfo powershellScript(path);
	if (!powershellScript.exists())
	{
		QString script = QString("param([string]$server=\"\",[Int32]$port=0 ) \n $client = New-Object Net.Sockets.TcpClient; \n try \n { \n  $client.Connect($server, $port);  \n  Write-Output $true \n } \n catch \n { \n   Write-Output $false \n } \n finally \n  { \n   $client.Dispose() \n }");
		QFile PS_Script(path);
		PS_Script.open(QFile::WriteOnly | QFile::Text);
		QTextStream writer(&PS_Script);
		writer << script;
		PS_Script.close();
	}
	return QDir::cleanPath(QString("%1%2%3").arg(getToolDataPath(), QDir::separator(), CHECK_REMOTE_SERVER));
}

/* This function allows to read and get the ToolId (if exists) from Scxml_Tool_Ids.xml file
* @return: QString - Contains the value which will be insert in the database
*/
QString GTASystemServices::readAndGetToolIdFromXml()
{
	_equipmentToolName.clear();
	QString defaultId = "NO";
	QString scxmlToolIDPath = this->getSCXMLToolIDFilePath();
	QFile toolIdFile(scxmlToolIDPath);
	if (toolIdFile.exists())
	{
		QFile xmlFile(scxmlToolIDPath);
		if (xmlFile.open(QIODevice::ReadOnly | QFile::Text))
		{
			QDomDocument* pDomDoc = new QDomDocument();
			if (pDomDoc != NULL)
			{
				QString errMsg;
				int errLine;
				int errCol;
				if (pDomDoc->setContent(&xmlFile, false, &errMsg, &errLine, &errCol))
				{
					QDomElement rootElement = pDomDoc->documentElement();
					if (rootElement.hasChildNodes())
					{
						QDomNodeList toolIdNodeList = rootElement.childNodes();
						for (int i = 0; i < toolIdNodeList.count(); i++)
						{
							QDomNode toolIdNode = toolIdNodeList.at(i);
							if (toolIdNode.nodeName() == "tool")
							{
								QDomNamedNodeMap toolIdAttr = toolIdNode.attributes();
								QString id = toolIdAttr.namedItem("id").nodeValue();
								QString toolType = toolIdAttr.namedItem("toolType").nodeValue();
								QString defaultVal = toolIdAttr.namedItem("default").nodeValue();
								QString status = toolIdAttr.namedItem("activate").nodeValue();
								QString equipName = toolIdAttr.namedItem("equipName").nodeValue();								

								// Default tool_name
								if (defaultVal.compare("true", Qt::CaseInsensitive) == 0)
								{
									if (!id.isEmpty())
									{
										defaultId = id;
									}
								}
								// Specific tool_name for a list of equipments
								if (!equipName.isEmpty())
								{
									if (status.compare("true", Qt::CaseInsensitive) == 0)
									{
										_equipmentToolName.insert(id, equipName.split(','));
									}
								}
							}
						}
					}

					delete pDomDoc;
					pDomDoc = NULL;

				}
				else
				{
					delete pDomDoc;
					pDomDoc = NULL;
				}
			}
		}
	}
	return defaultId;
}
