#include "CustomizedCommandInterface.h"

CustomizedCommandInterface::CustomizedCommandInterface()
{
    setWidgetParams("continue", 3, 0, 3);
}

CustomizedCommandInterface::CustomizedCommandInterface(QString name) : _name(name)
{
    setWidgetParams("continue", 3, 0, 3);
}

void CustomizedCommandInterface::setChildWidgetParams(const QList<QPair<QString, QString>>& params)
{
    _childWgtParams = params;
}

void CustomizedCommandInterface::appendChildWidgetParam(const Param& param)
{
    _childWgtParams.append(param);
}

void CustomizedCommandInterface::setWidgetParams(const QString& onFail, const int& timeOut, const int& precision, const int& confTime)
{
    _wgtParams = {
        {"OnFailCB", onFail},
        {"TimeOutLE", QString::number(timeOut)},
        {"PrecisionLE", QString::number(precision)},
        {"ConfTimeLE", QString::number(confTime)}
    };
}

void CustomizedCommandInterface::reset()
{
    _name = "";
    _childWgtParams.clear();
    setWidgetParams("continue", 3, 0, 3);
}

CustomizedCheck::CustomizedCheck()
{
    setWidgetParams("continue", 3, 0, 3);
    _isAction = false;
}

CustomizedCheck::CustomizedCheck(const QString& name)
{
    _name = name;
    setWidgetParams("continue", 3, 0, 3);
    _isAction = false;
}

std::tuple<QString, QList<QPair<QString, QString>>, QList<QPair<QString, QString>>> CustomizedCheck::split()
{
    return { _name, _childWgtParams, _wgtParams };
}

void CustomizedCheck::makeBiteMessageWidgetParams(
    const QString& msg,
    const int& waitForBuffer,
    const QString& oprator,
    const QString& onFail
)
{
    reset();
    _name = "BITE Message";
    setWidgetParams(onFail, 3, 0, 3);
    setChildWidgetParams({
        {"MessageLE", msg},
        {"waitForbufferLE", QString::number(waitForBuffer)},
        {"OperatorCB", oprator}
        });
}

void CustomizedCheck::makeFwcWarningWidgetParams(
    const QString& msg,
    const QString& header,
    const QString& color,
    const int& waitForBuffer,
    const QString& oprator,
    const QString& onFail
)
{
    reset();
    _name = "FWC Warning";
    setWidgetParams(onFail, 3, 0, 3);
    setChildWidgetParams({
        {"WarningMsgLE", msg},
        {"headerLE", header},
        {"colorLE", color},
        {"waitForBufferLE", QString::number(waitForBuffer)},
        {"OperatorCB", oprator}
        });
}

QList<QPair<QString, QString>> CustomizedCheck::makeExprForChckVal(
    const QString& param,
    const QString& compOper,
    const QString& value,
    const QString& whichList,
    const QString& precision,
    const QString& precType,
    const QString& FS,
    const QString& SDI,
    const QString& parity,
    const QString& checkOnly
)
{
    /// The order of this expression.append is very important (Careful if modified) 
    QList<QPair<QString, QString>> expression;
    expression.append(QPair<QString, QString>("Param1LE", param));
    expression.append(QPair<QString, QString>("Operator1CB", compOper));
    expression.append(QPair<QString, QString>("Param2LE", value));
    expression.append(QPair<QString, QString>("functionalStatusCB", FS));
    expression.append(QPair<QString, QString>("CheckOnly", checkOnly));
    expression.append(QPair<QString, QString>("ChosenList", whichList));
    expression.append(QPair<QString, QString>("editPrecisionLE", precision));
    expression.append(QPair<QString, QString>("precisionTypeCB", precType));
    expression.append(QPair<QString, QString>("CheckSDICB", SDI));
    expression.append(QPair<QString, QString>("CheckParityCB", parity));
    return expression;
}

void CustomizedCheck::makeValueWidgetParams(
    const QList<QList<QPair<QString, QString>>>& expressionList,
    const QString& onFail,
    const int& timeOut,
    const int& confTime
)
{
    reset();
    _name = "Value";
    setWidgetParams(onFail, timeOut, 0, confTime);
    for (auto expression : expressionList)
        _childWgtParams.append(expression);
}

CustomizedAction::CustomizedAction()
{
    setWidgetParams("continue", 3, 0, 3);
    _isAction = true;
}

CustomizedAction::CustomizedAction(const QString& name, const QString& complement)
{
    _name = name;
    _complement = complement;
    setWidgetParams("continue", 3, 0, 3);
    _isAction = true;
}

std::tuple<QString, QString, QList<QPair<QString, QString>>, QList<QPair<QString, QString>>> CustomizedAction::split()
{
    return { _name, _complement, _childWgtParams, _wgtParams };
}

void CustomizedAction::reset()
{
    _name = "";
    _complement = "";
    _childWgtParams.clear();
    setWidgetParams("continue", 3, 0, 0);
}

void CustomizedAction::makeConditionIfAction(const QString& paramName, const QString& paramValue, const QString& condition)
{
    reset();
    _name = "condition";
    _complement = "if";
    setChildWidgetParams({ { "ParamNameLE", paramName }, {"ValueLE", paramValue}, {"Condition", condition} });
}
void CustomizedAction::makeWaitforAction(const QString value)
{
    reset();
    _name = "wait";
    _complement = "for";
    setChildWidgetParams({ {"", value} });
}

void CustomizedAction::makeWaitUntilAction(const QString& paramName, const QString& paramValue, const QString& condition)
{
    reset();
    _name = "wait";
    _complement = "until";
    setChildWidgetParams({ { "ParamNameLE",  paramName },{"ValueLE", paramValue}, {"Condition", condition} });
}

void CustomizedAction::makeTitleWidgetParams(const QString& title, const QString& comment)
{
    reset();
    _name = "title";
    _complement = "";
    setChildWidgetParams({ {"TitleLE", title } , {"CommentLE", comment} });
}

void CustomizedAction::makePrintMessageWidgetParams(const QString& msg)
{
    reset();
    _name = "print";
    _complement = "message";
    setChildWidgetParams({ {"_pLineEdit", msg} });
}

void CustomizedAction::makeInitPIRParams(const QList<QString>& checkPirParams)
{
    reset();
    _name = "Init PIR List";
    _complement = "parameter";
    for (auto paramName : checkPirParams)
    {
        appendChildWidgetParam({ "parameterLE", paramName });
    }

}

void CustomizedAction::makeReleasePIRParams()
{
    reset();
    _name = "Release PIR List";
    _complement = "parameter";
    setChildWidgetParams({ });
}
void CustomizedAction::makeCallParams(const QString& typeCall, const QString& name, const Params& arguments)
{
    reset();
    _name = "call";
    _complement = typeCall;
    Params params;
    params.append({ typeCall, name });
    params.append(arguments);
    setChildWidgetParams(params);
}

void CustomizedAction::makePrintFormatedTimeParams()
{
    reset();
    _name = "print";
    _complement = "time";
    setChildWidgetParams({ {"TimeTypeCB", "Formated"} });
}

void CustomizedAction::makePrintUtcTimeParams()
{
    reset();
    _name = "print";
    _complement = "time";
    setChildWidgetParams({ {"TimeTypeCB", "UTC"} });
}

void CustomizedAction::makePrintParameterValueParams(const QStringList& parameterValueList)
{
    reset();
    _name = "print";
    _complement = "parameter value";
    for (auto value : parameterValueList)
        appendChildWidgetParam({ "parameterLE", value });
}

void CustomizedAction::makePrintTableParams(const QList<std::tuple<QString, QString, QString>>& tableParams)
{
    reset();
    _name = "print";
    _complement = "table";
    for (auto value : tableParams)
    {
        auto [tableName, titleName, parameter] = value;
        appendChildWidgetParam({ "tableNameLE", tableName });
        appendChildWidgetParam({ "titleNameLE", titleName });
        appendChildWidgetParam({ "parameterLE", parameter });
    }
}

void CustomizedAction::makeSetParams(
    const QString& paramName,
    const QString& value,
    const QString& functionStatus,
    const QString& onFail,
    const int& timeOut
)
{
    reset();
    _name = "set";
    _complement = "";
    appendChildWidgetParam({ "ParamNameLE", paramName });
    appendChildWidgetParam({ "ValueLE", value });
    appendChildWidgetParam({ "functionStatusCB", functionStatus });
    setWidgetParams(onFail, timeOut, 0, 0);
}

void CustomizedAction::makeMultiSetParams(
    const QList<multiSetParams>& multiSetParams,
    const QString& onFail,
    const int& timeOut
)
{
    reset();
    _name = "set";
    _complement = "";
    for (auto params : multiSetParams)
    {
        appendChildWidgetParam({ "ParamNameLE", params.param });
        appendChildWidgetParam({ "ValueLE", params.value });
        appendChildWidgetParam({ "functionStatusCB", params.functionStatus });
    }
    setWidgetParams(onFail, timeOut, 0, 0);
}


void CustomizedAction::makeAmxRemoteServerExternalToolWidgetParams(const QString& iIndex, const QString& iEtat, const int& timeOut)
{
    reset();
    _name = "External Tool";
    _complement = "";

    appendChildWidgetParam({ "toolNameCB", "AMX Remote Server"});
    appendChildWidgetParam({ "FunctionListWidget", "SetRelay"});
    appendChildWidgetParam({ "FuncArgsTWIndex", iIndex});
    appendChildWidgetParam({ "FuncArgsTWEtat", iEtat});
}