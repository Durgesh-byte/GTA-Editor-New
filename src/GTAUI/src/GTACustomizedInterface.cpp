#include "AINGTACustomizedInterface.h"

AINGTACustomizedInterface::AINGTACustomizedInterface()
{
    setWidgetParams("continue", 3, 0, 3);
}

AINGTACustomizedInterface::AINGTACustomizedInterface(QString name) : _name(name)
{
    setWidgetParams("continue", 3, 0, 3);
}

void AINGTACustomizedInterface::setChildWidgetParams(const QList<QPair<QString, QString>>& params)
{
    _childWgtParams = params;
}

void AINGTACustomizedInterface::appendChildWidgetParam(const Param& param)
{
    _childWgtParams.append(param);
}

void AINGTACustomizedInterface::setWidgetParams(const QString& onFail, const int& timeOut, const int& precision, const int& confTime)
{
    _wgtParams = {
        {"OnFailCB", onFail},
        {"TimeOutLE", QString::number(timeOut)},
        {"PrecisionLE", QString::number(precision)},
        {"ConfTimeLE", QString::number(confTime)}
    };
}

void AINGTACustomizedInterface::reset()
{
    _name = "";
    _childWgtParams.clear();
    setWidgetParams("continue", 3, 0, 3);
}

AINGTACustomizedCheck::AINGTACustomizedCheck()
{
    setWidgetParams("continue", 3, 0, 3);
}

AINGTACustomizedCheck::AINGTACustomizedCheck(const QString& name)
{
    _name = name;
    setWidgetParams("continue", 3, 0, 3);
}

std::tuple<QString, QList<QPair<QString, QString>>, QList<QPair<QString, QString>>> AINGTACustomizedCheck::split()
{
    return { _name, _childWgtParams, _wgtParams }; 
}

void AINGTACustomizedCheck::makeBiteMessageWidgetParams(
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

void AINGTACustomizedCheck::makeFwcWarningWidgetParams(
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

QList<QPair<QString, QString>> AINGTACustomizedCheck::makeExprForChckVal(
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

void AINGTACustomizedCheck::makeValueWidgetParams(
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

AINGTACustomizedAction::AINGTACustomizedAction()
{
    setWidgetParams("continue", 3, 0, 3);
}

AINGTACustomizedAction::AINGTACustomizedAction(const QString& name, const QString& complement)
{
    _name = name;
    _complement = complement;
    setWidgetParams("continue", 3, 0, 3);
}

std::tuple<QString, QString, QList<QPair<QString, QString>>, QList<QPair<QString, QString>>> AINGTACustomizedAction::split()
{
    return { _name, _complement, _childWgtParams, _wgtParams };
}

void AINGTACustomizedAction::reset()
{
    _name = "";
    _complement = "";
    _childWgtParams.clear();
    setWidgetParams("continue", 3, 0, 0);
}

void AINGTACustomizedAction::makeConditionIfAction(const QString& paramName, const QString& paramValue, const QString& condition)
{
    reset();
    _name = "condition";
    _complement = "if";
    setChildWidgetParams({ { "ParamNameLE", paramName }, {"ValueLE", paramValue}, {"Condition", condition} });
}
void AINGTACustomizedAction::makeWaitforAction(const QString value)
{
    reset();
    _name = "wait";
    _complement = "for";
    setChildWidgetParams({ {"", value} });
}

void AINGTACustomizedAction::makeWaitUntilAction(const QString &paramName, const QString& paramValue, const QString& condition )
{
    reset();
    _name = "wait";
    _complement = "until";
    setChildWidgetParams({ { "ParamNameLE",  paramName },{"ValueLE", paramValue}, {"Condition", condition} });
}

void AINGTACustomizedAction::makeTitleWidgetParams(const QString& title, const QString& comment)
{
    reset();
    _name = "title";
    _complement = "";
    setChildWidgetParams({ {"TitleLE", title } , {"CommentLE", comment} });
}

void AINGTACustomizedAction::makePrintMessageWidgetParams(const QString& msg)
{
    reset();
    _name = "print";
    _complement = "message";
    setChildWidgetParams({ {"_pLineEdit", msg} });
}

void AINGTACustomizedAction::makeInitPIRParams(const QList<QString>& checkPirParams)
{
    reset();
    _name = "Init PIR List";
    _complement = "parameter";
    for (auto paramName : checkPirParams)
    {
        appendChildWidgetParam({ "parameterLE", paramName });
    }
    
}

void AINGTACustomizedAction::makeReleasePIRParams()
{
    reset();
    _name = "Release PIR List";
    _complement = "parameter";
    setChildWidgetParams({ });
}
void AINGTACustomizedAction::makeCallParams(const QString& typeCall ,const QString& name, const Params& arguments)
{
    reset();
    _name = "call";
    _complement = typeCall;
    Params params;
    params.append({ typeCall, name });
    params.append(arguments);
    setChildWidgetParams(params);
}

void AINGTACustomizedAction::makePrintFormatedTimeParams()
{
    reset();
    _name = "print";
    _complement = "time";
    setChildWidgetParams({ {"TimeTypeCB", "Formated"} });
}

void AINGTACustomizedAction::makePrintUtcTimeParams()
{
    reset();
    _name = "print";
    _complement = "time";
    setChildWidgetParams({ {"TimeTypeCB", "UTC"} });
}

void AINGTACustomizedAction::makePrintParameterValueParams(const QStringList& parameterValueList)
{
    reset();
    _name = "print";
    _complement = "parameter value";
    for (auto value : parameterValueList)
        appendChildWidgetParam({ "parameterLE", value });
}

void AINGTACustomizedAction::makePrintTableParams(const QList<std::tuple<QString, QString, QString>>& tableParams)
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

void AINGTACustomizedAction::makeSetParams(
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

void AINGTACustomizedAction::makeMultiSetParams(
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
