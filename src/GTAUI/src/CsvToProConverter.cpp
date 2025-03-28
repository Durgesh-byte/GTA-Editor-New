#include "CsvToProConverter.h"

#include "GTAUtil.h"

#include <qfile.h>
#include <qprogressdialog.h>
#include <qmessagebox.h>

/**
 * @brief Converts a csv file to pro. Csv file has to be set with setAndCheckCSV beforehad.
 * @param editorWindow
 * @param proFileName
 * @return Operation status
*/
bool CsvToProConverter::convert(GTAEditorWindow* editorWindow, const QString& proFileName) {
    _proFileName = proFileName;
    _errorLogs.clear();
    if (!_csvParsedObject.has_value()) {
        _errorLogs.push_back({ "CSV not parsed" });
        return false;
    }

    QList<CustomizedAction::multiSetParams> multisetValActionList;
    QPair<QStringList, QStringList> multisetFSActionList;

    auto& tableData = _csvParsedObject->table;
    const auto& cmdNameRow = tableData.front();
    const auto& parameterRow = *(std::next(tableData.begin()));

    std::shared_ptr<QProgressDialog> csvProgressBar = nullptr;
    if (!_isConsole) {
        csvProgressBar = createCsvProgressBar();
        csvProgressBar->show();
    }
    size_t progressCounter = 0;

    QStringMap setVal, setFs, multisetVal, multisetFs, callObject, checkPirParams;

    //TODO use appndLogs?
    size_t metaRowOffset = _csvParsedObject->meta.size() + 2; //+2 for #Meta and #MetaEnd cells
    size_t gmtEndPos = findGmtEndPosition(parameterRow);

    for (size_t colIdx = 0; colIdx < gmtEndPos; ++colIdx) {
        const auto& cmdName = cmdNameRow[colIdx];
        if (!isRecognizedCmdName(cmdName)) {
            QString row = QString("%1").arg(1 + metaRowOffset);
            QString col = numberToExcelColumn(colIdx + 1);
            _errorLogs.push_back({ QString("Command name %1 is not recognized").arg(cmdName),
                                   ErrorMessage::ErrorType::kError,
                                   ErrorMessage::MsgSource::kExportDoc,
                                   _proFileName,
                                   QString(" [row: %1; col: %2; TC:%3]").arg(row).arg(col).arg(_lastTestCase) });
        }
    }

    _previousLogSize = _errorLogs.size();

    for (size_t rowIdx = 2; rowIdx < tableData.size(); ++rowIdx) {
        auto action = std::make_shared<CustomizedAction>();
        auto check = std::make_shared<CustomizedCheck>();
        const auto& row = tableData[rowIdx];
        std::size_t checkPirValIdx = 0;
        std::size_t initPirValIdx = 0;
        QString previousCommand = "";
        size_t previousLogSize = _errorLogs.size();

        for (size_t colIdx = 0; colIdx < gmtEndPos+1; ++colIdx) {
            CommandParams cmdParams(cmdNameRow[colIdx], parameterRow[colIdx], row[colIdx]);
            const auto& [commandName, parameterName, parameterValue] = cmdParams;

            handleEndOfMultiset(editorWindow, action, cmdParams, previousCommand ,multisetValActionList);
            verifyTriplets(cmdParams);

            if (!isParameterEmpty(parameterName, parameterValue, commandName)) {
                handleWaits(editorWindow, action, cmdParams);
                handleTestCase(editorWindow, action, cmdParams);
                handleFunctionLike(editorWindow, action, cmdParams);
                handleChecks(editorWindow, check, cmdParams);
                handleSets(editorWindow, action, cmdParams, setVal, setFs);
                handleMultisets(editorWindow, action, cmdParams, setVal, setFs, multisetValActionList);
                handlePrints(editorWindow, action, cmdParams);
                handleAmxCommands(editorWindow, action, cmdParams);

                if (commandName == kCheckPirValue && ((colIdx < row.size() - 4 && cmdNameRow[colIdx + 1] != kCheckPirValue) || (++checkPirValIdx) % 8 == 0)) {
                    handlePirChecks(editorWindow, action, check, cmdParams, checkPirParams);
                    checkPirValIdx = 0;
                }
                else if (commandName == kCheckPirValue) {
                    checkPirParams[parameterName] = parameterValue;
                }

                else if (commandName == kConditionIf)
                {
                    // Implemented but TBD
                    // Example :
                    // action->makeConditionIfAction("100","10",ARITH_LTEQ);
                    // pSubWindow->appendCustomizedAction(action);
                }
                else {
                    //Unrecognized commandName is handled earlier in isRecognizedCmdName
                }
            }
            appendLogsPositions(rowIdx, colIdx);
            previousCommand = commandName;
            if (!_isConsole) {
                csvProgressBar->setValue(++progressCounter);
                if (csvProgressBar->wasCanceled()) {
                    break;
                }
            }
        }
        if (!_isConsole) {
            if (csvProgressBar->wasCanceled()) {
                break;
            }
        }
    }

    _lastTestCase = kDefaultTestCase;
    createHeaderFile(proFileName);
}

/**
 * @brief Appends log positions to the latest batch of logs
 * @param rowIdx 
 * @param colIdx 
*/
void CsvToProConverter::appendLogsPositions(size_t rowIdx, size_t colIdx) {
    size_t metaRowOffset = _csvParsedObject->meta.size() + 2; //+2 for #Meta and #MetaEnd cells
    size_t diff = _errorLogs.size() - _previousLogSize;
    for (auto log = std::prev(_errorLogs.end()); log != std::prev(_errorLogs.end()) - diff; --log) {
        //rowIdx + 1 and colIdx + 1 for indexing from 1 not 0
        QString row = QString("%1").arg(rowIdx + 1 + metaRowOffset);
        QString col = numberToExcelColumn(colIdx + 1);
        log->lineNumber = QString(" [row: %1; col: %2; TC:%3]").arg(row).arg(col).arg(_lastTestCase);
        log->fileName = _proFileName;
        if (log->errorType == ErrorMessage::ErrorType::kNa)
            log->errorType = kDefaultLogLevel;
        if (log->source == ErrorMessage::MsgSource::kNoSrc)
            log->source = kDefaultLogSource;
    }

    _previousLogSize = _errorLogs.size();
}

/**
 * @brief verify if the command parameter for check and set are triplets, logs warning otherwise
 * @param cmdParams 
*/
void CsvToProConverter::verifyTriplets(const CommandParams& cmdParams) {
    const auto& [commandName, parameterName, parameterValue] = cmdParams;
    if (isAnySet(commandName) || isAnyCheck(commandName)) {
        if (!parameterName.isEmpty()) {
            QStringList valTriplet = parameterName.split(".");
            if (valTriplet.size() != 3) {
                _errorLogs.push_back({ QString("No Triplet found for command %1. Parameter %2 treated as local").arg(commandName).arg(parameterName), ErrorMessage::ErrorType::kWarning });
            }
        }
    }
}

/**
 * @brief Makes wait action and appends it to the editor window
 * @param editorWindow 
 * @param action 
 * @param cmdParams 
*/
void CsvToProConverter::handleWaits(GTAEditorWindow* editorWindow, std::shared_ptr<CustomizedAction> action, const CommandParams& cmdParams) {
    const auto& [commandName, parameterName, parameterValue] = cmdParams;
    if (commandName == kCmdWaitS) {
        action->makeWaitforAction(parameterValue);
        editorWindow->appendCustomizedAction(action, _errorLogs);
    }
    else if (commandName == kCmdWaitUntil) {
        // Implemented but TBD
        // Example :
        //action->makeWaitUntilAction("THROTTLE_EXC.EEC_ECU_1_CH_B_RESOLVER_EXC.TRA_EXC_EEC1_CHB",
        //                            "THROTTLE_EXC.EEC_ECU_1_CH_A_RESOLVER_EXC.TRA_EXC_EEC1_CHA",
        //                            ARITH_GT);
        //pSubWindow->appendCustomizedAction(action);

    }
}

/**
 * @brief Makes test case action and appends it to the editor window
 * @param editorWindow 
 * @param action 
 * @param cmdParams 
*/
void CsvToProConverter::handleTestCase(GTAEditorWindow* editorWindow, std::shared_ptr<CustomizedAction> action, const CommandParams& cmdParams) {
    const auto& [commandName, parameterName, parameterValue] = cmdParams;
    if (commandName == kTestCase) {
        action->makeTitleWidgetParams("Test_Case_" + parameterValue, "");
        editorWindow->appendCustomizedAction(action, _errorLogs);
        _lastTestCase = QString("%1").arg(parameterValue);
    }
}

/**
 * @brief Makes function-like (see isFunctionLike method) action and appends it to the editor window
 * @param editorWindow 
 * @param action 
 * @param cmdParams 
*/
void CsvToProConverter::handleFunctionLike(GTAEditorWindow* editorWindow, std::shared_ptr<CustomizedAction> action, const CommandParams& cmdParams) {
    const auto& [commandName, parameterName, parameterValue] = cmdParams;
    if (commandName == kFunction) {
        if (isGmt(parameterName) || isUtc(parameterName)) {
            handleTimeFunctions(editorWindow, action, cmdParams);
        }
        else {
            auto parsedValue = parseFunctionInputs(parameterValue);
            if (parsedValue.has_value()) {
                action->makeCallParams(COM_CALL_FUNC, parameterName, parsedValue.value());
                editorWindow->appendCustomizedAction(action, _errorLogs);
            }
        }
    }
    else if (commandName == kObject) {
        auto parsedValue = parseFunctionInputs(parameterValue);
        if (parsedValue.has_value()) {
            action->makeCallParams(COM_CALL_OBJ, parameterName, parsedValue.value());
            editorWindow->appendCustomizedAction(action, _errorLogs);
        }
    }
}

/**
 * @brief Makes time related action and appends it to the editor window
 * @param editorWindow 
 * @param action 
 * @param cmdParams 
*/
void CsvToProConverter::handleTimeFunctions(GTAEditorWindow* editorWindow, std::shared_ptr<CustomizedAction> action, const CommandParams& cmdParams) {
    const auto& [commandName, parameterName, parameterValue] = cmdParams;
    if (isGmt(parameterName)) {
        if (parameterValue == "1") {
            action->makePrintFormatedTimeParams();
            editorWindow->appendCustomizedAction(action, _errorLogs);
        }
        else {
            _errorLogs.push_back({ "GMT functions only accept \"1\" as input, if you wish to skip execution leave cell empty", ErrorMessage::ErrorType::kWarning });
        }
    }
    else if (isUtc(parameterName)) {
        if (parameterValue == "1") {
            action->makePrintUtcTimeParams();
            editorWindow->appendCustomizedAction(action, _errorLogs);
        }
        else {
            _errorLogs.append({ "UTC functions only accept \"1\" as input, if you wish to skip execution leave cell empty", ErrorMessage::ErrorType::kWarning });
        }
    }
}

/**
 * @brief Makes check-like action and appends it to the editor window
 * @param editorWindow 
 * @param check 
 * @param cmdParams 
*/
void CsvToProConverter::handleChecks(GTAEditorWindow* editorWindow, std::shared_ptr<CustomizedCheck> check, const CommandParams& cmdParams) {
    const auto& [commandName, parameterName, parameterValue] = cmdParams;
    if (commandName == kCheckValue) {
        check->makeValueWidgetParams({ check->makeExprForChckVal(parameterName,"=",parameterValue,"first","0","Value",parameterValue,"00","0","Value") }, "continue", 3, 0);
        editorWindow->appendCustomizedCheck(check, _errorLogs);
    }
    else if (commandName == kCheckFs) {
        check->makeValueWidgetParams({
                check->makeExprForChckVal(parameterName,"=",parameterName,"first","0","Value",parameterValue,"00","0","FS")},
                "continue", 3, 0);
        editorWindow->appendCustomizedCheck(check, _errorLogs);

    }
    else if (commandName == kCheckBiteMsg) {
        check->makeBiteMessageWidgetParams(parameterValue, 1, "NOT", "continue");
        editorWindow->appendCustomizedCheck(check, _errorLogs);
    }
    else if (commandName == kCheckFwcMsg) {
        // TBD - Example :
        //check->makeFwcWarningWidgetParams("wiadomosc", "naglowek", "kolor", 3, "NOT", "stop");
        //pSubWindow->appendCustomizedCheck(check);
    }
}

/**
 * @brief Makes checkPir action and appends it to the editor window
 * @param editorWindow 
 * @param action 
 * @param check 
 * @param cmdParams 
 * @param checkPirParams 
*/
void CsvToProConverter::handlePirChecks(GTAEditorWindow* editorWindow, std::shared_ptr<CustomizedAction> action, std::shared_ptr<CustomizedCheck> check,
                                        const CommandParams& cmdParams, std::map<QString, QString>& checkPirParams) {
    const auto& [commandName, parameterName, parameterValue] = cmdParams;
    //Verify if the command is a CHECK_PIR_VAL in order to initialize a package of 8 PIR parameters max
    QStringList paramNames;
    checkPirParams[parameterName] = parameterValue;
    for (const auto pair : checkPirParams)
    {
        paramNames.append(pair.first);
    }
    action->makeInitPIRParams(paramNames);
    editorWindow->appendCustomizedAction(action, _errorLogs);

    for (const auto pair : checkPirParams)
    {
        check->makeValueWidgetParams(
            {
                check->makeExprForChckVal(pair.first,"=",pair.second,"first","0","Value",pair.second,"00","0","Value")
            }, "continue", 3, 0
        );
        editorWindow->appendCustomizedCheck(check, _errorLogs);
    }
    action->makeReleasePIRParams();
    editorWindow->appendCustomizedAction(action, _errorLogs);
    checkPirParams.clear();
    paramNames.clear();
}

/**
 * @brief Makes set action and appends it to the editor window
 * @param editorWindow 
 * @param action 
 * @param cmdParams 
 * @param setVal used in case of normal set
 * @param setFs used in case of set fs
*/
void CsvToProConverter::handleSets(GTAEditorWindow* editorWindow, std::shared_ptr<CustomizedAction> action, const CommandParams& cmdParams,
                                   QStringMap& setVal, QStringMap& setFs) {
    const auto& [commandName, parameterName, parameterValue] = cmdParams;
    if (commandName == kSetValue && parameterValue != "") {
        auto itr = setVal.find(parameterName);
        if (itr == setVal.end() || itr->second != parameterValue) {
            setVal[parameterName] = parameterValue;
            action->makeSetParams(parameterName, parameterValue, "NO");
            editorWindow->appendCustomizedAction(action, _errorLogs);
        }
        else {
            //TODO: Should this work like this? What about new TestCase?
            _errorLogs.push_back({ QString("Set val for param %1 skipped, param already has value %2").arg(parameterName).arg(parameterValue),                                     ErrorMessage::ErrorType::kInfo });
        }
    }
    else if (commandName == kSetFs && parameterValue != "") {
        auto itr = setFs.find(parameterName);
        if (itr == setFs.end() || itr->second != parameterValue) {
            setFs[parameterName] = parameterValue;
            action->makeSetParams(parameterName, "", parameterValue);
            editorWindow->appendCustomizedAction(action, _errorLogs);
        }
        else {
            //TODO: Should this work like this? What about new TestCase?
            _errorLogs.push_back({ QString("Set val for param %1 skipped, param already has value %2").arg(parameterName).arg(parameterValue),                                     ErrorMessage::ErrorType::kInfo });
        }
    }
}

/**
 * @brief Appends set action to multisetValActionList, the list contains sets which come in a row, this is later used by handleEndOfMultiset to actually append the multiset action
 * @param editorWindow 
 * @param action 
 * @param cmdParams 
 * @param multisetVal 
 * @param multisetFs 
 * @param multisetValActionList list of sets which will be used for creation of 
*/
void CsvToProConverter::handleMultisets(GTAEditorWindow* editorWindow, std::shared_ptr<CustomizedAction> action, const CommandParams& cmdParams,
                                        QStringMap& multisetVal, QStringMap& multisetFs, QList<CustomizedAction::multiSetParams>& multisetValActionList) {
    const auto& [commandName, parameterName, parameterValue] = cmdParams;
    if (commandName == kMultisetVal) {
        auto itr = multisetVal.find(parameterName);
        if (itr == multisetVal.end() || itr->second != parameterValue) {
            multisetVal[parameterName] = parameterValue;
            multisetValActionList.append({ parameterName, parameterValue, "NO" });
        }
        else {
            _errorLogs.push_back({ QString("Multiset val for param %1 skipped, param already has value %2").arg(parameterName).arg(parameterValue),                                 ErrorMessage::ErrorType::kInfo });
        }
    }
    else if (commandName == kMultisetFs && parameterValue != "") {
        auto itr = multisetFs.find(parameterName);
        if (itr == multisetFs.end() || itr->second != parameterValue) {
            multisetFs[parameterName] = parameterValue;
            //TODO: IMPORTANT: This should be multiSetFSActionList
            multisetValActionList.append({ parameterName, "", parameterValue });
        }
        else {
            _errorLogs.push_back({ QString("MultisetFS val for param %1 skipped, param already has value %2").arg(parameterName).arg(parameterValue),                               ErrorMessage::ErrorType::kInfo });
        }
    }
}

/**
 * @brief After multiset is ended (by creation of command other than set), the appended sets are added as one multiset action
 * @param editorWindow 
 * @param action 
 * @param cmdParams 
 * @param previousCommand 
 * @param multisetValActionList 
*/
void CsvToProConverter::handleEndOfMultiset(GTAEditorWindow* editorWindow, std::shared_ptr<CustomizedAction> action, const CommandParams& cmdParams,
                         const QString& previousCommand, QList<CustomizedAction::multiSetParams>& multisetValActionList) {
    const auto& [commandName, parameterName, parameterValue] = cmdParams;
    if (isMultiset(previousCommand) && !isMultiset(commandName) && !multisetValActionList.isEmpty()) {
        action->makeMultiSetParams(multisetValActionList, "continue", 3);
        editorWindow->appendCustomizedAction(action, _errorLogs);
        multisetValActionList.clear();
    }
}


/**
 * @brief Makes print action and appends it to the editor window
 * @param editorWindow 
 * @param action 
 * @param cmdParams 
*/
void CsvToProConverter::handlePrints(GTAEditorWindow* editorWindow, std::shared_ptr<CustomizedAction> action, const CommandParams& cmdParams) {
    const auto& [commandName, parameterName, parameterValue] = cmdParams;
    if (commandName == kPrintVal) {
            action->makePrintParameterValueParams({ parameterValue });
            editorWindow->appendCustomizedAction(action, _errorLogs);
    }
    else if (commandName == kPrintTab && parameterValue != "") {
        // Implemented but TBD
        // Example :
        //action->makePrintTableParams({
        //   std::make_tuple<QString, QString, QString>("tab name", "title", "parameter"),
        //   std::make_tuple<QString, QString, QString>("2tab name", "2title", "2parameter")
        //   });
        //pSubWindow->appendCustomizedAction(action);
    }
    else if (commandName == kPrintMsg) {
        action->makePrintMessageWidgetParams(parameterValue);
        editorWindow->appendCustomizedAction(action, _errorLogs);
    }
}

/**
 * @brief Makes amx action and appends it to the editor window
 * @param editorWindow 
 * @param action 
 * @param cmdParams 
*/
void CsvToProConverter::handleAmxCommands(GTAEditorWindow* editorWindow, std::shared_ptr<CustomizedAction> action, const CommandParams& cmdParams)
{
    const auto& [commandName, parameterName, parameterValue] = cmdParams;
    if (commandName == AmxCmd)
    {
        QStringList pCalculators = parameterName.split("|", QString::SkipEmptyParts);

        for (const QString& calc : pCalculators)
        {
            const QString pIndex = Calculators[calc]; //each calculator has a unique numerical index
            action->makeAmxRemoteServerExternalToolWidgetParams(pIndex, parameterValue);
            editorWindow->appendCustomizedAction(action, _errorLogs);
        }
    }
}

/**
 * @brief Parses the csv file
 * @param iFilePath 
 * @return 
*/
std::optional<CsvParsedObject> CsvToProConverter::parseCsv(const QString& iFilePath)
{
    // some definitions of variables and lambda functions
    auto isMeta = [](QString line) -> bool {
        static bool meta = false;
        if (line.contains("#MetaEnd")) meta = false;
        else if (line.contains("#Meta")) meta = true;
        return meta;
    };

    auto delimiterCounter = [](QString line) -> int {
        int counter = 0;
        for (const QChar& c : line)
            if (c == ';' || c == ",") counter++;
        return counter;
    };

    auto parseLine = [&delimiterCounter](QString line) -> QVector<QString> {
        static int nrOfPositions = delimiterCounter(line) + 1;
        QString position = "";
        QVector<QString> row;
        row.reserve(nrOfPositions);

        for (const QChar& c : line) {
            if (c == ';' || c == ",") {
                row.push_back(position);
                position = "";
            }
            else {
                position.append(c);
            }
        }
        row.push_back(position.simplified());
        return row;
    };

    auto isClear = [](QVector<QString> row) -> bool {
        for (const QString& pos : row)
            if (pos != "") return false;
        return true;
    };

    QFile file(iFilePath);
    CsvParsedObject object;

    // the main method procedure
    if (!file.open(QIODevice::ReadOnly))
        return std::nullopt;

    while (!file.atEnd())
    {
        QString line = file.readLine();
        if (isMeta(line)) {
            if (!line.contains("#Meta"))
            {
                auto row = parseLine(line);
                if (!isClear(row)) object.meta.append(row);
            }
        }
        else {
            if (object.meta.isEmpty()) {
                QMessageBox::critical(nullptr, "Critical Error", "Wrong csv file. The #Meta section Empty or not detected.");
                return std::nullopt;
            }
            if (!line.contains("#MetaEnd"))
            {
                auto row = parseLine(line);
                if (!isClear(row)) object.table.append(row);
            }
        }
    }

    return object;
}

/**
 * @brief Sets csv for conversion and checks if the fiven file is valid
 * @param csvFilePath
 * @return FileData of given csv file if succesful, nullopt otherwise
*/
std::optional<FileData> CsvToProConverter::setAndCheckCsv(const QString& csvFilePath) {
    auto csvParsedObject = parseCsv(csvFilePath);

    if (!csvParsedObject.has_value()) {
        _errorLogs.push_back({ "Failed to parse csv" });
        return std::nullopt;
    }

    QString fileName = "", authorName = "";
    if (!areAllRowsSameSize(csvParsedObject->meta)) {
        handleDifferentRowsSize(CsvPart::kMeta, csvParsedObject.value());
        return std::nullopt;
    }
    for (const QVector<QString>& line : csvParsedObject->meta) {
        if (line[0] == "Procedure Title" && !line[1].isEmpty()) {
            fileName = line[1];
            if (!fileName.isEmpty() && !authorName.isEmpty())
                break;
        }
        else if (line[0] == "Author" && !line[1].isEmpty()) {
            authorName = line[1];
            if (!fileName.isEmpty() && !authorName.isEmpty())
                break;
        }
    }

       if (fileName.isEmpty() && authorName.isEmpty()) {
        _errorLogs.push_back({ "Procedure Title and Author not found in #Meta section" });
        QMessageBox::critical(nullptr, "Critical Error", "Procedure Title and Author not found in #Meta section");
        return std::nullopt;
    }
    else if (fileName.isEmpty()) {
        _errorLogs.push_back({ "Procedure Title not found in #Meta section" });
        QMessageBox::critical(nullptr, "Critical Error", "Procedure Title not found in #Meta section");
        return std::nullopt;
    }
    else if (authorName.isEmpty()) {
        _errorLogs.push_back({ "Author not found in #Meta section" });
        QMessageBox::critical(nullptr, "Critical Error", "Author not found in #Meta section");
        return std::nullopt;
    }
    else if (!areAllRowsSameSize(csvParsedObject->table)) {
        handleDifferentRowsSize(CsvPart::kProcedure, csvParsedObject.value());
        return std::nullopt;
    }
    else {
        _csvParsedObject = *csvParsedObject;
        return FileData(fileName, authorName);
    }
}

/**
 * @brief Parses function-like action inputs from cell.
 * @param inputCell - example input {var1Name:var1Value|var2Name:var2Value|...varNVName:varNValue}
 * @return 
*/
std::optional<CustomizedCommandInterface::Params> CsvToProConverter::parseFunctionInputs(const QString& inputCell) {
    QString cell = inputCell;
    const QString& startChar = "{";
    const QString& endChar = "}";
    const QString& emptyCell = startChar + endChar;
    const auto& delimiterChar = "|";
    const auto& delimiterVarChar = ":";
    CustomizedAction::Params params;
    if (cell == emptyCell) {
        return params;
    }
    cell.remove(startChar);
    cell.remove(endChar);

    QStringList namesAndValues = cell.split(delimiterChar);
    for (const auto& el : namesAndValues) {
        if (!el.isEmpty() && el.contains(delimiterVarChar)) {
            auto nameAndValue = el.split(delimiterVarChar);
            params.append({ nameAndValue[0], nameAndValue[1] });
        }
        else {
            _errorLogs.append(QString("Cell '%1' could not be parsed as function argument. "
                "Correct format is: {var1Name:var1Value|var2Name:var2Value|...}. For no arguments: {}").arg(inputCell));
            return std::nullopt;
        }
    }

    return params;
};

/**
 * @brief Method for converting index into a corresponding excel column letter combination
 *        0 -> A, 1 -> B, 26 -> AA etc.
 * @param n 
 * @return 
*/
QString CsvToProConverter::numberToExcelColumn(int n) {
    char result[10];
    int i = 0;

    while (n > 0) {
        int rem = n % 26;
        // If remainder is 0, then a 'Z' must be there in output
        if (rem == 0) {
            result[i++] = 'Z';
            n = (n / 26) - 1;
        }
        else {
            result[i++] = (rem - 1) + 'A';
            n = n / 26;
        }
    }
    result[i] = '\0';
    std::reverse(result, result + strlen(result));

    return result;
}

/**
 * @brief log getter
*/
ErrorMessageList& CsvToProConverter::getLogs() {
    return _errorLogs;
}

/**
 * @brief clear _errorLogs
*/
void CsvToProConverter::clearLogs() {
    _errorLogs.clear();
}

/**
 * @brief Checks if all rows in TableData are the same size
 * @param tableData data from CsvParsedObject (can be either meta or table)
 * @return 
*/
bool CsvToProConverter::areAllRowsSameSize(const QList<QVector<QString>>& tableData) {
    size_t previousRowSize = tableData[0].size();
    for (const auto& row : tableData) {
        if (row.size() != previousRowSize) {
            return false;
        }
        previousRowSize = row.size();
    }
    return true;
}

/**
 * @brief Creates logs reporting different row size for either meta or table part of csv
 * @param csvPart 
 * @param csvObj 
*/
void CsvToProConverter::handleDifferentRowsSize(CsvPart csvPart, const CsvParsedObject& csvObj) {
    QString part = (csvPart == CsvPart::kMeta) ? "Meta" : "Procedure";
    QString errMsg = QString("%1 data rows are not the same size. This is most often caused by using alt + enter linebreaks inside excel documents. \n"
        "Check csv file in a text editor\n"
        "Table sizes are:").arg(part);
    auto& data = (csvPart == CsvPart::kMeta) ? csvObj.meta : csvObj.table;
    for (const auto& row : data) {
        errMsg.append(QString(" %1").arg(row.size()));
    }
    if (!_isConsole) {
        QMessageBox::critical(nullptr, "Critical Error", errMsg);
    }
    _errorLogs.push_back(errMsg);
}

/**
 * @brief checks if command is a set
 * @param cmdName
*/
bool CsvToProConverter::isSet(const QString& cmdName) {
    return cmdName == kSetValue || cmdName == kSetFs;
}

/**
 * @brief checks if command is type any set
 * @param cmdName
*/
bool CsvToProConverter::isAnySet(const QString& cmdName) {
    return cmdName == kSetValue || cmdName == kSetFs || cmdName == kMultisetVal || cmdName == kMultisetFs;
}

/**
 * @brief checks if command is any type of check
 * @param cmdName
*/
bool CsvToProConverter::isAnyCheck(const QString& cmdName) {
    return cmdName == kCheckValue || cmdName == kCheckFs || cmdName == kCheckPirValue;
}

/**
 * @brief checks if command is test case
 * @param cmdName
*/
bool CsvToProConverter::isTestCase(const QString& cmdName) {
    return cmdName == kTestCase;
}

/**
 * @brief Checks if command is multiset
 * @param cmdName
*/
bool CsvToProConverter::isMultiset(const QString& cmdName) {
    return cmdName == kMultisetVal || cmdName == kMultisetFs;
}

/**
 * @brief Checks if command is gmt
 * @param paramName
*/
bool CsvToProConverter::isGmt(const QString& paramName) {
    return (paramName == kGmtStart || paramName == kGmtEnd);
}

/**
 * @brief Checks if command is utc
 * @param paramName
*/
bool CsvToProConverter::isUtc(const QString& paramName) {
    return (paramName == kUtcStart || paramName == kUtcEnd);
}

/**
 * @brief Checks if action is function-like. Function like objects accept the {var1:1 | ...} format as input (see parseFunctionInputs) 
 * @param cmdName 
 * @param paramName 
 * @return 
*/
bool CsvToProConverter::isFunctionLike(const QString& cmdName, const QString& paramName) {
    return !isUtc(paramName) && !isGmt(paramName) && (cmdName == kFunction || cmdName == kObject);
}

/**
 * @brief Checks if command name is recognized by the system
*/
bool CsvToProConverter::isRecognizedCmdName(const QString& cmdName) {
    return cmdName == kSetValue || cmdName == kSetFs || cmdName == kMultisetVal || cmdName == kMultisetFs || cmdName == kCheckValue || cmdName == kCheckFs ||
        cmdName == kCheckPirValue || cmdName == kCheckBiteMsg || cmdName == kCheckFwcMsg || cmdName == kTestCase || cmdName == kPrintVal || cmdName == kPrintTab ||
        cmdName == kPrintMsg || cmdName == kFunction || cmdName == kObject || cmdName == kCmdWaitS || cmdName == kCmdWaitUntil || cmdName == kConditionIf ||
        cmdName == kGmtStart || cmdName == kGmtEnd || cmdName == kUtcStart || cmdName == kUtcEnd || cmdName == AmxCmd;
}

/**
 * @brief Checks if parameter is empty. Logs errors.
*/
bool CsvToProConverter::isParameterEmpty(const QString& parameterName, const QString& parameterValue, const QString& commandName) {
    if (parameterValue.isEmpty()) {
        if (isAnyCheck(commandName)) {
            _errorLogs.push_back({ QString("Check action: value for %1 is empty. Check actions always need a value.").arg(parameterName) });
        }
        else if (isSet(commandName)) {
            _errorLogs.push_back({ QString("Set action: value for %1 is empty. Set actions always need a value.").arg(parameterName) });
        }
        else if (isMultiset(commandName)) {
            _errorLogs.push_back({ QString("Multiset action: value for %1 is empty. Multiset actions always need a value.").arg(parameterName) });
        }
        else if (isGmt(parameterName)) {
            _errorLogs.push_back({ QString("GMT action: value for %1 is empty. GMT actions always need a value.").arg(parameterName) });
        }
        else if (isUtc(parameterName)) {
            _errorLogs.push_back({ QString("UTC action: value for %1 is empty. UTC actions always need a value.").arg(parameterName) });
        }
        else if (isTestCase(commandName)) {
            _errorLogs.push_back({ QString("TestCase: value for %1 is empty. TestCase actions always need a value.").arg(parameterName) });
        }
        else if (isFunctionLike(parameterName, commandName)) {
            _errorLogs.push_back({ QString("Function action: Value for %1 is empty. If you wish to call function with no parameters use \"{}\" instead.").arg(parameterName), ErrorMessage::ErrorType::kInfo });
        }
        return true;
    }
    else {
        return false;
    }
}

/**
 * @brief Finds the last Gmt End. Data after that is not parsed.
 * @param parameterRow 
 * @return 
*/
size_t CsvToProConverter::findGmtEndPosition(const QVector<QString>& parameterRow) {
    size_t gmtEndPos = 0;
    for (const auto& param : parameterRow) {
        if (param == kGmtEnd) {
            break;
        }
        gmtEndPos++;
    }

    return gmtEndPos;
}

/**
 * @brief Creates a progress bar which displays current state of csv to pro conversion
*/
std::shared_ptr<QProgressDialog> CsvToProConverter::createCsvProgressBar() {
    auto& tableData = _csvParsedObject->table;
    size_t progressMax = 0;
    for (auto rowIt = std::next(tableData.begin(), 2); rowIt != tableData.end(); ++rowIt) {
        const auto& row = *rowIt;
        for (std::size_t i = 0; i < row.size(); ++i) {
            progressMax++;
        }
    }

    auto csvProgressBar = std::make_shared<QProgressDialog>("Converting .csv to .pro", "Cancel", 0, progressMax, _parent);
    csvProgressBar->setWindowModality(Qt::WindowModal);
    csvProgressBar->setValue(0);
    csvProgressBar->setWindowTitle("Progress Status");

    return csvProgressBar;
}

/**
 * @brief creates header file
*/
void CsvToProConverter::createHeaderFile(const QString& headerName)
{
    GTAAppController* pAppController = GTAAppController::getGTAAppController();
    QString dataDir = pAppController->getGTADataDirectory();
    std::ofstream outputFile((dataDir+"/" + headerName).toStdString() + ".hdr");
    if (outputFile.is_open())
    {
        outputFile << "<HEADER NAME=\"" << headerName.toStdString() << "\">";
        for (const QVector<QString>& line : _csvParsedObject->meta)
        {
            outputFile << "\n\t<ITEM NAME=\"" << line[0].toStdString() << "\" VALUE=\"" << line[1].toStdString() << "\" DESCRIPTION=\"\" MANDATORY= \"" << "0"<< "\"/>";
        }
        outputFile << "\n</HEADER>";
        outputFile.close();
    }
}

