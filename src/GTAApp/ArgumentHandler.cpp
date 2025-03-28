#include "ArgumentHandler.h"
#include "GTAAppConfig.h"
#include "GTAVersion.h"
#include "GTACheckCompatibility.h"
#include <QDir>

/**
 * @brief Helper class for handling arguments in main
 * @param appCtrl AppController will be used for performing actions required by arguments 
 * @param iniPath IniPath which will be changed if Action::kSetInitFile action is performed
*/
ArgumentHandler::ArgumentHandler(GTAAppController& appCtrl, QString& iniPath)
 : _appCtrl(appCtrl), _iniPath(iniPath) {
	registerArgument({Action::kHelp,
					 { "--help", "-h" },
					 "Lists possible arguments and their function.\n"});

	registerArgument({ Action::kVersion,
					{ "--version", "-v"},
					"Returns current version of GTA Editor.\n" });

	registerArgument({ Action::kSetInitFile,
					{ "--config=", "-c="},
					"Sets init file to given value\n"
					"ie. -c=C:/My/Path/my.init." });

	registerArgument({ Action::kTestInitFile,
					{ "--testconfig", "-t"},
					"Tests if config is correct, terminates program if it's not\n"});

	registerArgument({ Action::kForce,
					{ "--force", "-f"},
					"Forces overwrite of output file even if it already exists\n"
					"ie. -o=existing/file.scxml --force" });

	registerArgument({ Action::kInput,
					 { "--input=", "-i="},
					 "Input for conversion.\n"
					 ".csv files will be converted to .pro\n"
					 ".pro files will be converted to scxml\n"
					 "Can be used together with output argument. Otherwise default output path will be used\n"});

	registerArgument({ Action::kEnableUnsubscribeStart,
					 {"--enableUnsubscribeStart=","-us="},
					 "Enables Unsubscribe At Start of the .scxml by default set to true\n"
					 "Should be used together with input argument\n"});

	registerArgument({ Action::kEnableUnsubscribeEnd,
					 {"--enableUnsubscribeEnd=","-ue="},
					 "Enables Unsubscribe At End of the .scxml by default set to true\n"
					 "Should be used together with input argument\n"});

	registerArgument({ Action::kEnableUnsubscribeFileParamOnly,
					 {"--enableUnsubscribeFileParamOnly=","-ufo="},
					 "Enables Unsubscribe File Param only at end of the .scxml by default set to true\n"
					 "Should be used together with input argument\n" });

	registerArgument({ Action::kCompatibilityCheck,
					 {"--compatibilityCheck=","-cc="},
					 "Checks the compatibility of the .pro file\n"
					 "Optional: The user can specify the folder where to generate the test report, by default it's generated in /output directory.\n"
					 "ie. --compatibilityCheck=C:any.pro OR -cc=any.pro C:/Path\n" });

	registerArgument({ Action::kOutput,
					 { "--output=", "-o="},
					 "Output for conversion done by input command.\n"
					 "Can be directory (in which case filename will be copied from input) or full path with filename.\n"
					 "Already existing files will not be overwritten unless --force argument is also used.\n"
					 "ie. -o=C:/My/Path OR -o=C:/My/Path/my.scxml\n"});

	registerArgument({ Action::kTestReport,
					 { "--report=", "-r="},
					 "Test report generation of the procedure file.\n"
					 "Need to be name of .pro file.\n"
					 "Optional: The user can specify the folder where to generate the test report, by default it's generated in /output directory.\n"
					 "ie. --report=any.pro OR -r=any.pro C:/directory\n" });

	registerArgument({ Action::kLTRAReportFormat,
					 { "--ltraF=", "-lf="},
					 "Format of LTRA report generation of the procedure file.\n"					 
					 "Format can be either DOCX (default) or XML or DOCX_XML.\n"
					 "ie. --ltraF=DOCX OR -lf=DOCX \n" });

	registerArgument({ Action::kLTRAReport,
					 { "--ltra=", "-l="},
					 "LTRA report generation of the procedure file.\n"
					 "Need to be the name of .pro or .seq file.\n"
					 "Optional: The user can specify the folder where to generate the test report, by default it's generated in /output directory.\n"
					 "ie. --ltra=any.pro OR -l=any.pro C:/directory\n" });

	_blockGuiActions = { Action::kHelp, Action::kVersion, Action::kTestInitFile, Action::kInput, Action::kOutput,Action::kCompatibilityCheck , Action::kTestReport, Action::kLTRAReport};
	_doesntNeedConfig = { Action::kHelp, Action::kVersion };
	_forceOverwrite = false;
	_hasUnsubscribeAtStart = true;
	_hasUnsubscribeAtEnd = true;
	_hasUnsubscribeFileParamOnly = true;

	appendBlockGuiInfo();
}

void ArgumentHandler::setMainWindow(GTAMainWindow* mainWindow) {
	_mainWindow = mainWindow;
}

void ArgumentHandler::addOptionalSecondReportArgument(const QStringList& arguments, const QString& currentArg)
{
	QString argAlias;
	currentArg.contains("=") ? argAlias = currentArg.left(currentArg.lastIndexOf("=") + 1) : argAlias = currentArg;

	if ((argAlias == "-r=" || argAlias == "--report=" || argAlias == "-l=" || argAlias == "--ltra=" || argAlias == "-cc=" || argAlias == "--compatibilityCheck=") && !arguments.isEmpty()) {
		auto idx = arguments.indexOf(currentArg);
		auto lastIdx = arguments.lastIndexOf(arguments.last());

		if (idx < lastIdx) {
			auto nextIdx = idx + 1;
			auto nextArg = arguments[nextIdx];

			if (!nextArg.isEmpty() && nextArg != "-f" && nextArg != "--force") {
				if (!_toBePerormed.empty())
					_toBePerormed.back().second.append(";" + nextArg);

				if (nextIdx < lastIdx) {
					nextArg = arguments[++nextIdx];
					if (!nextArg.isEmpty() && nextArg != "-f" && nextArg != "--force") {
						if (!_toBePerormed.empty())
							_toBePerormed.back().second.append(";" + nextArg);
					}
				}
			}
		}
	}
}

void ArgumentHandler::registerArgumetns(const QStringList& arguments) {
	QTextStream out(stdout);
	for (const auto& arg : arguments) {
		for (auto& registeredArg : _registeredArguments) {
			if (registeredArg._action != Action::kSetInitFile) {
				for (const auto& alias : registeredArg._aliases) {
					QString argAlias;
					arg.contains("=") ? argAlias = arg.left(arg.lastIndexOf("=") + 1) : argAlias = arg;
					if (!argAlias.compare(alias)) {
						registeredArg._isActive = true;
						_toBePerormed.emplace_back(std::make_pair(registeredArg._action, arg));
						addOptionalSecondReportArgument(arguments, arg);
					}
				}
			}
		}
	}

	std::sort(_toBePerormed.begin(), _toBePerormed.end(), [](const std::pair <Action, QString>& a, const std::pair <Action, QString>& b)
		{
			return static_cast<int>(a.first) < static_cast<int>(b.first);
		});
}

/**
 * @brief Handles all arguments with exception of set ini file which has to be performed earlier
*/
void ArgumentHandler::executeArguments() {
	for (const auto& action : _toBePerormed) {
		performAction(action.first, action.second);
	}
}

/**
 * @brief Separate handle only for setting init - it needs to be done before initializing main window
		  and initialisedmain window is required for actions which use _appCtrl
*/
void ArgumentHandler::handleConfigInit(const QStringList& arguments) {
	for (const auto& arg : arguments) {
		for (auto& registeredArg : _registeredArguments) {
			if (registeredArg._action == Action::kSetInitFile) {
				for (const auto& alias : registeredArg._aliases) {
					QString argAlias;
					arg.contains("=") ? argAlias = arg.left(arg.lastIndexOf("=") + 1) : argAlias = arg;
					if (!argAlias.compare(alias)) {
						registeredArg._isActive = true;
						performAction(registeredArg._action, arg);
					}
				}
			}
		}
	}
}

void ArgumentHandler::performAction(Action action, const QString& argText) {
	if (action == Action::kHelp)
		handleHelp();
	else if (action == Action::kVersion)
		handleVersion();
	else if (action == Action::kSetInitFile)
		handleSetIniFile(argText);
	else if (action == Action::kTestInitFile)
		handleTestIniFile();
	else if (action == Action::kForce)
		handleForce();
	else if (action == Action::kEnableUnsubscribeStart)
		handleUnsubscribeStart(argText);
	else if (action == Action::kEnableUnsubscribeEnd)
		handleUnsubscribeEnd(argText);
	else if (action == Action::kCompatibilityCheck)
		handleCompatibilityCheck(argText);
	else if (action == Action::kOutput)
		handleOutput(argText);
	else if (action == Action::kInput)
		handleInput(argText);
	else if (action == Action::kTestReport)
		handleTestReport(argText);
	else if (action == Action::kLTRAReport)
		handleLTRAReport(argText);
	else if (action == Action::kEnableUnsubscribeFileParamOnly)
		handleUnsubscribeFileParamOnly(argText);
}

void ArgumentHandler::handleHelp() {
    QTextStream out(stdout);
	out << "List of possible arguments with description:\n\n";
	for (const auto& regArg : _registeredArguments) {
		for (const auto& alias : regArg._aliases) {
			out << alias << " ";
		}
		out << ": " << regArg._description << "\n\n";
	}
} 

void ArgumentHandler::handleVersion() {
	QTextStream out(stdout);
	out << "Version: " << QString(TOOL_VERSION_STR) << "\n";
}

void ArgumentHandler::handleSetIniFile(const QString& argText) {
	setPathFromArg(argText, _iniPath, "Init config file");
}

void ArgumentHandler::handleForce() {
	_forceOverwrite = true;
	_mainWindow->setForceOverwrite(true);
}

void ArgumentHandler::handleOutput(const QString& argText) {
	setPathFromArg(argText, _outputPath, "Output path");
}

void ArgumentHandler::handleUnsubscribeStart(const QString& argText) {
	QString qChoice = argText;
	qChoice.remove("-us=").remove("--disableUnsubscribeStart=");
	if (qChoice.toLower() == "true")
		_hasUnsubscribeAtStart = true;
	else if (qChoice.toLower() == "false")
		_hasUnsubscribeAtStart = false;
	
}

void ArgumentHandler::handleUnsubscribeEnd(const QString& argText) {
	QString qChoice = argText;
	qChoice.remove("-ue=").remove("--disableUnsubscribeEnd=");
	if (qChoice.toLower() == "true")
		_hasUnsubscribeAtEnd = true;
	if (qChoice.toLower() == "false")
		_hasUnsubscribeAtEnd = false;
}

/**
 * @brief handles UnsubscribeFileParamOnly parameter value. Sets true if argument value is true else false
 * @param argText: argument text passed for UnsubscribeFileParamOnly
*/
void ArgumentHandler::handleUnsubscribeFileParamOnly(const QString& argText)
{
	QString qChoice = argText;
	qChoice.remove("-ufo=").remove("--disableUnsubscribeFileParamOnly=");

	_hasUnsubscribeFileParamOnly = qChoice.toLower() == "true";
}

void ArgumentHandler::handleCompatibilityCheck(const QString& argText)
{
	GTACheckCompatibility checkComp;
	bool firstElementIgnored = false;
	QString saveCompatibilityReportPath = "";
	QStringList inputs = argText.split(";");
	QString iProcedureName = getFilePathAcordingToExtension(inputs, ".pro");
	iProcedureName.remove("-cc=").remove("--compatibilityCheck=");
	QString sDataDirectory = _mainWindow->getAppController()->getGTADataDirectory();
	QString dbFile = _mainWindow->getAppController()->getGTADbFilePath();
	_mainWindow->setIsConsole(true);
	_mainWindow->openDocument(iProcedureName);
	GTAEditorWindow* pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(_mainWindow->getActiveSubWindow());
	if (pCurrSubWindow)
	{
		GTAElement* pElement = pCurrSubWindow->getElement();
		QString callElemUuid = pElement->getUuid();
		_appCtrl.getElementFromDocument(sDataDirectory+'/'+ iProcedureName, pElement, false, false, true);
		QDateTime currentDateTime = QDateTime::currentDateTime();
		QString dateTime = currentDateTime.toString("yyyyMMddhhmmss");
		log(" * Info : Starting the Check of compatibility for "+ iProcedureName +" ....");
		ErrorMessageList errorList = checkComp.CheckCompatibility(sDataDirectory, dbFile, pElement);
		if (errorList.isEmpty())
		{
			log(" * Info : This procedure is compatible with actual standard!");
		}
		else
		{
			log(" * Error : Some errors/warnings found in the compatibility report!");
			log(" * Info : iProcedureName=" + iProcedureName);

			std::string iProcedureNameStr = iProcedureName.toStdString();
			iProcedureNameStr = iProcedureNameStr.substr(iProcedureNameStr.find_last_of("/\\") + 1);
			iProcedureNameStr = iProcedureNameStr.substr(0, iProcedureNameStr.find_last_of('.'));

			QString ProcedureNameNoExtension = QString::fromUtf8(iProcedureNameStr.c_str());
			log(" * Info : ProcedureNameNoExtension=" + ProcedureNameNoExtension);
			if(inputs.size() == 1)
			{
				QDir folder("output");
				if (!folder.exists())
				{
					if (folder.mkpath("."))
					{
						log(" * Info : Default folder /output created successfully!");
						saveCompatibilityReportPath = "output/Compatibility_Check_" + ProcedureNameNoExtension + "_" + dateTime + ".log";
					}
				}
				else
				{
					log(" * Info : Default folder /output found!");
					saveCompatibilityReportPath = "output/Compatibility_Check_" + ProcedureNameNoExtension + "_" + dateTime + ".log";
				}
			}

			for (const auto& element : inputs)
			{
				if (!firstElementIgnored)
				{
					firstElementIgnored = true;
					continue; // Skip the first element (the .pro file/directory)
				}
				else if (element.contains(".log"))
				{
					saveCompatibilityReportPath = getFilePathAcordingToExtension(inputs, ".log");
				}
				else 
				{
					saveCompatibilityReportPath = element + "/" + "Compatibility_Check_" + ProcedureNameNoExtension + "_" + dateTime + ".log";
				}
			}
		}
		if (!saveCompatibilityReportPath.isEmpty())
		{
			QString ReportFilePath = QDir::cleanPath(QString("%1").arg(saveCompatibilityReportPath));
			log(" * Info : ReportFilePath = " + ReportFilePath);
			QFile file(ReportFilePath);						
			QStringList logList;
			file.open(QIODevice::WriteOnly);
			QTextStream out(&file);
			for (const auto& error : errorList)
			{
				QString msg = QString("%1 : %2 in %3 at line(%4)").arg(getErrorType(error.errorType), error.description, error.fileName, error.lineNumber);
				logList.append(msg);
				log(" * " + msg);
			}
			out << logList.join("\n");
			file.close();


			if(!file.exists())
				log(" * Error : Not found " + ReportFilePath);
		}
	}
	_mainWindow->setIsConsole(false);
}

void ArgumentHandler::handleInput(const QString& argText) {
	QString inputPath;
	setPathFromArg(argText, inputPath, "Input path");	
	if (inputPath.isEmpty()) {
		log("Input path invalid, aborting action\n");
		return;
	}
	_appCtrl.setProgramConfiguration();
	_appCtrl.updateTreeStructure(false, false);
	if (!checkOutputPath(inputPath)) {
		log("[Error] Output path check failed\nAborting.\n");
		return;
	}
	if (inputPath.endsWith(".pro")) {
		QTextStream out(stdout);
		log("Converting .pro to .scxml\n");
		_appCtrl.setUnsubscribeFileParamOnly(_hasUnsubscribeFileParamOnly);
		if (_appCtrl.exportDocToScxml(inputPath, _outputPath, _hasUnsubscribeAtStart, _hasUnsubscribeAtEnd, true)) {
			log("Done\n");
		}
		else {
			log("Failed\n");
		}
	}
	else if (inputPath.endsWith(".csv")) {
		log("Converting .csv to .pro\n");
		QFileInfo outputInfo(_outputPath);
		QFileInfo outDirInfo(_outputPath);
		_mainWindow->_customPath = outDirInfo.path();
		_mainWindow->_customFileName = outputInfo.baseName();
		_mainWindow->_last_csv_selection_file = inputPath;
		_mainWindow->setIsConsole(true);
		auto fileData = _mainWindow->_csvConverter.setAndCheckCsv(inputPath);
		if (fileData) {
			log("Converting file. This may take a while\n");
			_mainWindow->onConvertCsvToProClicked();			
			log("Finished converting file\n");
		}
		else {
			log("Failed to parse csv\n");
		}
		printMainWindowLogs();
	}
	else {
		log(QString("Did not recognise extension in input path: %1 \n Aborting.\n").arg(inputPath));
		return;
	}
	_mainWindow->setIsConsole(false);
}

void ArgumentHandler::handleTestIniFile() {
	log(QString("Checking config from path: %1\n").arg(_iniPath));
	TestConfig* testConf = TestConfig::getInstance();

	if (testConf->loadConf(_iniPath, true)) {
		log("Success - Config valid!\n");
	}
	else {
		log("[Eror] Config invalid\n");
	}
}

void ArgumentHandler::generateLtraReport(const QString& fullLogPath, const QString& fullProcedurePath, const QString& saveDocxPath)
{
	GTALogFrame* newLogFrame = _mainWindow->createNewResultPage();
	newLogFrame->setCliMode(true);
	newLogFrame->setLog(fullLogPath);
	newLogFrame->setElementRelativePath(fullProcedurePath);
	newLogFrame->setSavePath(saveDocxPath);
	newLogFrame->setElementType("Procedure");
	newLogFrame->analyzeResults();
	if (newLogFrame->exportReport(true, _ltraFormat) && !fullLogPath.isEmpty())
		log(" *info: the test successfully reported.\n");
	else
		log(" *error: reporting test encountered some problems.\n\tNot all the processes finished successfully.\n");
}

QString ArgumentHandler::getFilePathAcordingToExtension(const QStringList& input, const QString& extension)
{
	for (const auto& element : input) {
		QString lowerCaseElement = element.toLower();
		QString lowerCaseExtension = extension.toLower();
		if (lowerCaseElement.contains(lowerCaseExtension))
			return element;
	}
	return "";
}

void ArgumentHandler::handleTestReport(const QString& argText)
{
	log("Test report status:\n");
	QStringList inputs = argText.split(";");
	QString iProcedureName = getFilePathAcordingToExtension(inputs, ".pro");
	iProcedureName.remove("-r=").remove("--report=");
	QString saveCsvPath = "";
	bool firstElementIgnored = false;
	QDateTime currentDateTime = QDateTime::currentDateTime().toUTC();
	QString dateTime = currentDateTime.toString("yyyyMMddhhmmss");

	for (const auto& element : inputs)
	{
		QString ProcedureNameNoExtension = iProcedureName;
		if (!firstElementIgnored)
		{
			firstElementIgnored = true;
			continue; // Skip the first element (the .pro file/directory)
		}
		if (element.contains(".csv"))
		{
			saveCsvPath = getFilePathAcordingToExtension(inputs, ".csv");
		}
		else
		{
			ProcedureNameNoExtension.replace(".pro", "");
			saveCsvPath = element + "/" + ProcedureNameNoExtension + "_" + dateTime + "_Test_Report.csv";
		}
	}

	QString fullProcedurePath = findProcedureFile(iProcedureName);
	QString relativeProcedurePath = fullProcedurePath;
	relativeProcedurePath.remove(_mainWindow->getAppController()->getGTADataDirectory());
	_mainWindow->setTestReportCsvSavePath(saveCsvPath);
	if (QFile::exists(fullProcedurePath))
	{
		QString fullLogPath = getLogFile(iProcedureName);
		if (!fullLogPath.isEmpty())
		{
			QString fullTablePath = generateTestReport(fullLogPath);
			if(!fullTablePath.isEmpty())
				log(" *info: test report have been successfully generated.\n\tPath to the file: " + fullTablePath);
		}
		else
			log(" *error: log file of " + iProcedureName + " isn't good. Report generation aborted.\n");
	}
	else
		log(" *error: " + iProcedureName + " file doesn't exists. Report generation aborted.\n");
}

void ArgumentHandler::handleLTRAReportFormat(const QString& argText)
{
	QStringList handledLtraFormat = {"DOCX", "XML"};	
	_ltraFormat = argText.split("-lf=").last().split(";").first();
	_ltraFormat = handledLtraFormat.contains(_ltraFormat) ? _ltraFormat : handledLtraFormat.first();
	log("argText: " + argText);
	log("LTRA format: " + _ltraFormat + "\n");
}

void ArgumentHandler::handleLTRAReport(const QString& argText)
{
	log("LTRA status:\n");
	QStringList inputs = argText.split(";");
	QString iProcedureName = getFilePathAcordingToExtension(inputs, ".pro");
	iProcedureName.remove("-l=").remove("--ltra=");
	QString saveDocxPath = "";
	bool firstElementIgnored = false;

	for (const auto& element : inputs)
	{
		QString ProcedureNameNoExtension = iProcedureName;
		if (!firstElementIgnored)
		{
			firstElementIgnored = true;
			continue; // Skip the first element (the .pro file/directory)
		}
		if (element.contains(".docx"))
		{
			saveDocxPath = getFilePathAcordingToExtension(inputs, ".docx");
		}
		else
		{
			ProcedureNameNoExtension.replace(".pro", "");
			saveDocxPath = element + "/" + ProcedureNameNoExtension + ".docx";
		}
	}
	QString fullProcedurePath = findProcedureFile(iProcedureName);
	if (QFile::exists(fullProcedurePath))
	{
		QString fullLogPath = getLogFile(iProcedureName);
		if (!fullLogPath.isEmpty())
		{
			handleLTRAReportFormat(argText);
			saveDocxPath.replace("docx", _ltraFormat.toLower());
			log("saveLtraPath: " + saveDocxPath);
			generateLtraReport(fullLogPath, fullProcedurePath, saveDocxPath);
		}
		else
		{
			QString errorText = " *error: log file of " + iProcedureName + " isn't good. LTRA generation aborted.\n";
			perror(errorText.toStdString().c_str());
		}		
	}
	else
		log(" *error: " + iProcedureName + " file doesn't exists. LTRA generation aborted.\n");
}

QString ArgumentHandler::findProcedureFile(const QString& procedureName)
{

	QFileInfo procedureFileInfo(procedureName);
	if (procedureFileInfo.suffix() != "pro")
	{
		return QString();
	}
	if (QFileInfo::exists(procedureName))
	{
		return procedureName;
	}
	QString gtaDirectory = _appCtrl.getGTADataDirectory();
	QString procedureFullPath = gtaDirectory + "/" + procedureName;
	if (QFileInfo::exists(procedureFullPath))
	{
		return procedureFullPath;
	}
	// Look for the procedure under all DATA subdirectories
	QDirIterator it(gtaDirectory, QStringList() << "*.pro", QDir::NoFilter, QDirIterator::Subdirectories);
	while (it.hasNext())
	{
		if (it.fileName() != procedureName)
		{
			it.next();
		}
		else
		{
			return it.filePath();
		}
	}
	return QString();
}

void ArgumentHandler::launchEmotest(const QString& fullProcedurePath)
{

}

QString ArgumentHandler::getLogFile(const QString& procedureName)
{
	QString bareName = procedureName.split("/").last().split('\\').last();
	bareName.remove(".pro").remove(".seq").remove(".obj").remove(".fun");
	QString fullLogPath = _mainWindow->findLatestLogFile(_appCtrl.getGTALogDirectory(), bareName);
	log("fullLogPath = " + fullLogPath);

	if (QFile::exists(fullLogPath))
	{
		if (logFileIsGood(fullLogPath))
			return fullLogPath;
		else
			return "";
	}
	else
	{
		perror(" *error: log file doesn't exist.\n");
		return "";
	}
}

bool ArgumentHandler::logFileIsGood(const QString& fullLogPath)
{
	QString lastRecord = _mainWindow->getLastRecordFromLogFile(fullLogPath);
	if (lastRecord.contains("End_procedure"))
		return true;
	else
	{
		perror("*error: log file doesn't contain end of procedure line.\n");
		return false;
	}
}

QString ArgumentHandler::generateTestReport(const QString& fullLogPath)
{
	QString bareName = fullLogPath.split("/").last();
	bareName.remove(".log");

	_mainWindow->generateReport(fullLogPath, bareName, true);
	QString csvTableFullPath = _mainWindow->getCsvTableFullPath(bareName);

	if (QFile::exists(csvTableFullPath))
		return csvTableFullPath;
	else
	{
		log(" *error: test report haven't been generated.\n");
		return "";
	}
}

void ArgumentHandler::setPathFromArg(const QString& argText, QString& varToSet, const QString& varName) {
	int start = argText.indexOf("=");
	//TODO: More advanced check for path validity? Check for = might be redudant as it's already checked in handleArguments
	if (start != -1) {
		auto path = argText.split("=").at(1);
		varToSet = QDir::cleanPath(path);
		log(QString("Set %1 to: %2\n").arg(varName).arg(varToSet));
	}
	else {
		log(QString("Invalid %1 path").arg(varName));
	}
}

bool ArgumentHandler::checkOutputPath(const QString& inputPath) {
	QFileInfo outputInfo(_outputPath), inputInfo(inputPath);
	if (inputInfo.suffix().contains("csv") && outputInfo.suffix().contains("scxml") ||
		inputInfo.suffix().contains("pro") && outputInfo.suffix().contains("csv")) {
		//TODO: Direct conversion from csv to scxml
		log(QString("Conversion from %1 to %2 is not supported").arg(inputInfo.suffix()).arg(outputInfo.suffix()));
		return false;
	}
	if (_outputPath.isEmpty()) {
		if (inputInfo.fileName().contains(".pro")) {
			_outputPath = _appCtrl.getGTAScxmlDirectory();
		}
		else if (inputInfo.fileName().contains(".csv")) {
			_outputPath = _appCtrl.getGTADataDirectory();
		}
		log(QString("[Warning] Output not given, was set to default: %1 \n").arg(_outputPath));
	}
	if (!outputInfo.suffix().contains("scxml") && !outputInfo.suffix().contains("pro")) {
		if (inputInfo.fileName().contains(".pro")) {
			_outputPath += "/" + inputInfo.completeBaseName() + ".scxml";
			_outputPath = QDir::cleanPath(_outputPath);
			log(QString("[Warning] Output filename not given, was set to mimic input: %1\n").arg(_outputPath));
		}
		else if (inputInfo.fileName().contains(".csv")) {
			auto fileData = _mainWindow->_csvConverter.setAndCheckCsv(inputPath);
			if (!fileData->fileName.isEmpty()) {
				_outputPath += "/" + fileData->fileName + ".pro";
				_outputPath = QDir::cleanPath(_outputPath);
				log(QString("[Warning] Output filename not given, was set to name found in csv file: %1\n").arg(_outputPath));
			}
			else {
				_outputPath += "/" + inputInfo.completeBaseName() + ".pro";
				_outputPath = QDir::cleanPath(_outputPath);
				log(QString("[Warning] Output filename not given and not found in csv file, was set to mimic input: %1\n").arg(_outputPath));
			}
		}

	}

	QDir outDir(_outputPath);
	QFileInfo outDirInfo(_outputPath);
	QFile outFile(_outputPath);
	
	if (!outFile.exists() || _forceOverwrite) {
		if (outDir.mkpath(outDirInfo.dir().path())) {
				if (outFile.open(QFile::WriteOnly | QFile::Text)) {
					log(QString("Opened output file: %1\n").arg(_outputPath));
					outFile.close();
					return true;
				}
				else {
					QTextStream out(stdout);
					log(QString("Failed to open output file: %1\n").arg(_outputPath));
					return false;
				}
		}
		else {
			log(QString("[Error] Failed to create directory path: %1\n").arg(_outputPath));
			return false;
		}
	}
	else {
		log(QString("[Error] File %1 already exists, if you wish to overwrite it use --force argument\n").arg(_outputPath));
		return false;
	}
}

void ArgumentHandler::printMainWindowLogs() {
	QTextStream out(stdout);
	auto csvLogs = _mainWindow->getLogs();
	if (!csvLogs.empty()) {
		log("Following logs were encountered during conversion:\n");
		for (const auto& err : csvLogs) {
			log(QString("%1: %2").arg(getErrorType(err.errorType)).arg(err.description));
			log("\n");
		}
	}
}

QString ArgumentHandler::getErrorType(ErrorMessage::ErrorType type) {
	switch (type) {
	case ErrorMessage::ErrorType::kError:
		return "Error";

	case ErrorMessage::ErrorType::kWarning:
		return "Warning";

	case ErrorMessage::ErrorType::kInfo:
		return "Info";

	case ErrorMessage::ErrorType::kNa:
		return "";

	}

	return "";
}

bool ArgumentHandler::wasActionPerformed(Action action) {
	for (const auto& regArg : _registeredArguments) {
		if (regArg._action == action && regArg._isActive == true) {
			return true;
		}
	}

	return false;
}

bool ArgumentHandler::shouldLaunchGui() {
	for (const auto& regArg : _registeredArguments) {
		if (regArg._isActive) {
			for (const auto& blockAction : _blockGuiActions) {
				if (regArg._action == blockAction) {
					return false;
				}
			}
		}
	}

	return true;
}

bool ArgumentHandler::doesNeedConfig() {
	for (const auto& regArg : _registeredArguments) {
		if (regArg._isActive) {
			for (const auto& blockAction : _doesntNeedConfig) {
				if (regArg._action == blockAction) {
					return false;
				}
			}
		}
	}

	return true;
}

void ArgumentHandler::appendBlockGuiInfo() {
	for (auto& regArg : _registeredArguments) {
		for (const auto& blockAction : _blockGuiActions) {
			if (regArg._action == blockAction) {
				regArg._description += "This action blocks GUI from launching.";
			}
		}
	}
}

void ArgumentHandler::log(const QString& log) {
	QTextStream out(stdout);
	out << log << (log.endsWith('\n') ? "" : "\n");
}
