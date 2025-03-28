#pragma once
#include "GTAActionWidgetInterface.h"
#include "CustomizedCommandInterface.h"
#include "GTAEditorWindow.h"
#include "GTAHeaderWidget.h"
//#include "GTANewFileCreationDialog.h"

#include <memory>
#include <optional>

/**
 * @brief Object containing information about parsed CSV
 * @param meta -  metadata about the file (author name etc)
 * @param table - data contained in csv table
*/
struct CsvParsedObject {
    QList<QVector<QString>> meta;
    QList<QVector<QString>> table;
};

/**
 * @brief Object  containing filename and author of file
*/
struct FileData {
    FileData(const QString& inFileName, const QString inAuthorName) : fileName(inFileName), authorName(inAuthorName) {};
    QString fileName;
    QString authorName;
};

/**
 * @brief Object containing Parameters of commands
*/
struct CommandParams {
    CommandParams(const QString& commandName_, const QString& parameterName_, const QString parameterValue_)
        : commandName(commandName_), parameterName(parameterName_), parameterValue(parameterValue_) {};
    QString commandName;
    QString parameterName;
    QString parameterValue;
};

/**
 * @brief Class responsible for converting prepared CSV files into GTA procedures
*/
class GTAUI_SHARED_EXPORT CsvToProConverter {
public:
    using QStringMap = std::map<QString, QString>;
    //Constants used to describe commands
    static constexpr auto kSetValue = "SET_VAL";
    static constexpr auto kSetFs = "SET_FS";
    static constexpr auto kMultisetVal = "MULTISET_VAL";
    static constexpr auto kMultisetFs = "MULTISET_FS";
    static constexpr auto kCheckValue = "CHECK_VAL";
    static constexpr auto kCheckFs = "CHECK_FS";
    static constexpr auto kCheckPirValue = "CHECK_PIR_VAL";
    static constexpr auto kCheckBiteMsg = "CHECK_BITE_MESSAGE";
    static constexpr auto kCheckFwcMsg = "CHECK_FWC_MESSAGE";
    static constexpr auto kTestCase = "TC";
    static constexpr auto kPrintVal = "PRINT_VAL";
    static constexpr auto kPrintTab = "PRINT_TAB";
    static constexpr auto kPrintMsg = "PRINT_MESSAGE";
    static constexpr auto kFunction = "Function";
    static constexpr auto kObject = "Object";
    static constexpr auto kCmdWaitS = "CMD_WAIT_s";
    static constexpr auto kCmdWaitUntil = "CMD_WAIT_Until";
    static constexpr auto kConditionIf = "CONDITION_IF";
    static constexpr auto kGmtStart = "GMT_Start()";
    static constexpr auto kGmtEnd = "GMT_End()";
    static constexpr auto kUtcStart = "UTC_End()";
    static constexpr auto kUtcEnd = "UTC_Start()";
    static constexpr auto AmxCmd = "AMX_Command";
    QMap<QString, QString> Calculators = { 
        {"FWC1", "11"},    {"FWC2", "12"},   {"SDAC1", "13"},
        {"SDAC2", "14"},   {"ECP", "15"},    {"DMC1", "16"},
        {"DMC2", "17"},    {"DMC3", "18"},   {"PFD CAPT", "19"},
        {"ND CAPT", "20"}, {"E/WD", "21"},   {"SD", "22"},
        {"ND F/O", "23"},  {"PFD F/O", "24"}
    };

    static constexpr auto kDefaultTestCase = "not set";

    static constexpr auto kDefaultLogLevel = ErrorMessage::ErrorType::kError;
    static constexpr auto kDefaultLogSource = ErrorMessage::MsgSource::kExportDoc;


    bool convert(GTAEditorWindow* editorWindow, const QString& proFileName);
    std::optional<FileData> setAndCheckCsv(const QString& csvFilePath);
    void setParent(QWidget* parent) { _parent = parent; }
    ErrorMessageList& getLogs();
    void clearLogs();
    bool isViableCsvObject() { if (_csvParsedObject) return true; else return false; };
    void reset() { _csvParsedObject.reset(); _errorLogs.clear(); };
    void setIsConsole(bool isConsole) { _isConsole = isConsole; };


private:
    void appendLogsPositions(size_t rowIdx, size_t colIdx);
    bool areAllRowsSameSize(const QList<QVector<QString>>& tableData);
    enum class CsvPart { kMeta, kProcedure};
    void handleDifferentRowsSize(CsvPart csvPart, const CsvParsedObject& csvObj);

    bool isSet(const QString& cmdName);
    bool isAnySet(const QString& cmdName);
    bool isAnyCheck(const QString& cmdName);
    bool isTestCase(const QString& cmdName);
    bool isMultiset(const QString& cmdName);
    bool isGmt(const QString& paramName);
    bool isUtc(const QString& paramName);
    bool isFunctionLike(const QString& cmdName, const QString& paramName);
    bool isRecognizedCmdName(const QString& cmdName);
    bool isParameterEmpty(const QString& parameterName, const QString& parameterValue, const QString& commandName);
    void verifyTriplets(const CommandParams& cmdParams);

    /**
     * @brief Handlers for specific categories of actions, will create correct parameters and append the action to editor window
    */
    void handleWaits(GTAEditorWindow* editorWindow, std::shared_ptr<CustomizedAction> action, const CommandParams& cmdParams);
    void handleTestCase(GTAEditorWindow* editorWindow, std::shared_ptr<CustomizedAction> action, const CommandParams& cmdParams);
    void handleFunctionLike(GTAEditorWindow* editorWindow, std::shared_ptr<CustomizedAction> action, const CommandParams& cmdParams);
    void handleTimeFunctions(GTAEditorWindow* editorWindow, std::shared_ptr<CustomizedAction> action, const CommandParams& cmdParams);
    void handleChecks(GTAEditorWindow* editorWindow, std::shared_ptr<CustomizedCheck> check, const CommandParams& cmdParams);
    void handlePirChecks(GTAEditorWindow* editorWindow, std::shared_ptr<CustomizedAction> action, std::shared_ptr<CustomizedCheck> check, 
                        const CommandParams& cmdParams, QStringMap& checkPirParams);
    void handleSets(GTAEditorWindow* editorWindow, std::shared_ptr<CustomizedAction> action, const CommandParams& cmdParams,
                    QStringMap& setVal, QStringMap& setFs);
    void handleMultisets(GTAEditorWindow* editorWindow, std::shared_ptr<CustomizedAction> action, const CommandParams& cmdParams,
                         QStringMap& multisetVal, QStringMap& multisetFs, QList<CustomizedAction::multiSetParams>& multisetValActionList);
    void handleEndOfMultiset(GTAEditorWindow* editorWindow, std::shared_ptr<CustomizedAction> action, const CommandParams& cmdParams,
                             const QString& previousCommand, QList<CustomizedAction::multiSetParams>& multisetValActionList);
    void handlePrints(GTAEditorWindow* editorWindow, std::shared_ptr<CustomizedAction> action, const CommandParams& cmdParams);
    void handleAmxCommands(GTAEditorWindow* editorWindow, std::shared_ptr<CustomizedAction> action, const CommandParams& cmdParams);

    size_t findGmtEndPosition(const QVector<QString>& parameterRow);

    std::shared_ptr<QProgressDialog> createCsvProgressBar();
    void createHeaderFile(const QString& headerName);

    std::optional<CsvParsedObject> parseCsv(const QString& iFilePath);
    std::optional<CustomizedCommandInterface::Params> parseFunctionInputs(const QString& inputCell);

    QString numberToExcelColumn(int n);

    QWidget* _parent = nullptr;
    std::optional<CsvParsedObject> _csvParsedObject = std::nullopt;
    QString _proFileName = "";
    QString _customPath = "";
    QString _customFileName = "";
    QString _lastTestCase = kDefaultTestCase;

    //TODO: rename to just logs
    ErrorMessageList _errorLogs;
    size_t _previousLogSize = 0;
    bool _isConsole = false;
    GTAHeaderWidget* _pHeaderWidget;
    GTAHeaderFieldEdit* _pHdrFieldEditDlg;
};
