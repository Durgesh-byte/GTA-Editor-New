#pragma once
#include "GTAAppController.h"
#include "GTAMainWindow.h"
#include "GTALogFrame.h"

#pragma warning(push, 0)
#include <qtextstream.h>
#pragma warning(pop)
 
class ArgumentHandler {
public:
	/**
	 * @brief Enum class for actions. 
	 * IMPORTANT! Actions are stored in order of execution, see handleArguments
	*/
	enum class Action {
		kHelp,
		kVersion,
		kSetInitFile,
		kTestInitFile,
		kForce,
		kEnableUnsubscribeStart,
		kEnableUnsubscribeEnd,
		kEnableUnsubscribeFileParamOnly,
		kCompatibilityCheck,
		kOutput,
		kInput,
		kTestReport,
		kLTRAReport,
		kLTRAReportFormat,
		kUnknown
	};

	struct Argument {
	public:
		Argument(Action action, const QStringList& aliases, const QString& description, bool isActive = false)
			: _aliases(aliases), _description(description), _action(action), _isActive(isActive) {}

		QStringList _aliases;
		QString _description;
		Action _action;
		bool _isActive = false;
	};

	ArgumentHandler(GTAAppController& appCtrl, QString& iniPath);

	void setMainWindow(GTAMainWindow* mainWindow);
	void addOptionalSecondReportArgument(const QStringList& arguments, const QString& currentArg);
	void registerArgumetns(const QStringList& arguments);
	void executeArguments();
	void handleConfigInit(const QStringList& arguments);
	bool wasActionPerformed(Action action);
	bool shouldLaunchGui();
	bool doesNeedConfig();

private:
	void registerArgument(const Argument& arg) { _registeredArguments.push_back(arg); }

	void performAction(Action action, const QString& argText);

	void handleHelp();
	void handleVersion();
	void handleSetIniFile(const QString& argText);
	void handleForce();
	void handleUnsubscribeStart(const QString& argText);
	void handleUnsubscribeEnd(const QString& argText);
	void handleUnsubscribeFileParamOnly(const QString& argText);
	void handleCompatibilityCheck(const QString& argText);
	void handleOutput(const QString& argText);
	void handleInput(const QString& argText);
	void handleTestIniFile();
	void generateLtraReport(const QString& fullLogPath, const QString& fullProcedurePath, const QString& saveDocxPath);
	QString getFilePathAcordingToExtension(const QStringList& input, const QString& extension);
	void handleTestReport(const QString& argText);
	void handleLTRAReport(const QString& argText);
	void handleLTRAReportFormat(const QString& argText);

	QString findProcedureFile(const QString& argText);
	void launchEmotest(const QString& argText);
	QString getLogFile(const QString& procedureName);
	bool logFileIsGood(const QString& fullLogPath);
	QString generateTestReport(const QString& fullLogPath);
	bool checkOutputPath(const QString& inputPath);
	void setPathFromArg(const QString& argText, QString& varToSet, const QString& varName);
	void printMainWindowLogs();
	QString getErrorType(ErrorMessage::ErrorType type);
  
	void appendBlockGuiInfo();

	void log(const QString& log);

	QString& _iniPath;
	QString  _outputPath;
	QString _ltraFormat = "DOCX";
	GTAAppController& _appCtrl;
	GTAMainWindow* _mainWindow;
	std::vector<Argument> _registeredArguments;
	std::vector<Action> _blockGuiActions;
	std::vector<Action> _doesntNeedConfig;
	std::vector<std::pair<Action, QString>> _toBePerormed;
	bool _forceOverwrite;
	bool _hasUnsubscribeAtStart;
	bool _hasUnsubscribeAtEnd;
	bool _hasUnsubscribeFileParamOnly;
};
