#pragma once

#include <QWidget>
#include <QMdiSubWindow>
#include <QModelIndex>
#include <QUndoStack>
#include <QMenu>

struct CustomizedCommandInterface
{
    using Param = QPair<QString, QString>;
    using Params = QList<Param>;

    CustomizedCommandInterface();
    CustomizedCommandInterface(QString name);
    void setChildWidgetParams(const Params& params);
    void appendChildWidgetParam(const Param& param);
    void setWidgetParams(const QString& onFail, const int& timeOut, const int& precision, const int& confTime);
    virtual void reset();

    QString _name;
    Params _childWgtParams;
    Params _wgtParams;
    bool _isAction = true;
};

struct CustomizedCheck : public  CustomizedCommandInterface
{
    CustomizedCheck();
    CustomizedCheck(const QString& name);
    std::tuple<QString, QList<QPair<QString, QString>>, QList<QPair<QString, QString>>> split();
    void makeBiteMessageWidgetParams(
        const QString& msg,
        const int& waitForBuffer,
        const QString& oprator = "",
        const QString& onFail = "continue"
    );
    void makeFwcWarningWidgetParams(
        const QString& msg,
        const QString& header,
        const QString& color,
        const int& waitForBuffer,
        const QString& oprator = "",
        const QString& onFail = "continue"
    );
    QList<QPair<QString, QString>> makeExprForChckVal(
        const QString& param,
        const QString& compOper,
        const QString& value,
        const QString& whichList = "first",
        const QString& precision = "0",
        const QString& precType = "Value",
        const QString& FS = "NOT_LOST",
        const QString& SDI = "00",
        const QString& parity = "0",
        const QString& checkOnly = "0"
    );
    void makeValueWidgetParams(
        const QList<QList<QPair<QString, QString>>>& expressionList,
        const QString& onFail = "continue",
        const int& timeOut = 3,
        const int& confTime = 0
    );
    bool _isAction = true;
};

struct CustomizedAction : public CustomizedCommandInterface
{
    CustomizedAction();
    CustomizedAction(const QString& name, const QString& complement);
    std::tuple<QString, QString, QList<QPair<QString, QString>>, QList<QPair<QString, QString>>> split();
    void reset() override;
    void makeConditionIfAction(const QString& paramName, const QString& paramValue, const QString& condition);
    void makeWaitforAction(const QString value);
    void makeWaitUntilAction(const QString& paramName, const QString& paramValue, const QString& condition);
    void makeTitleWidgetParams(const QString& title, const QString& comment);
    void makePrintMessageWidgetParams(const QString& msg);
    void makeInitPIRParams(const QList<QString>& checkPirParams);
    void makeReleasePIRParams();
    void makeCallParams(const QString& typeCall, const QString& name, const Params& arguments);
    void makePrintFormatedTimeParams();
    void makePrintUtcTimeParams();
    void makePrintParameterValueParams(const QStringList& parameterValueList);
    void makePrintTableParams(const QList<std::tuple<QString, QString, QString>>& tableParams);
    void makeSetParams(
        const QString& paramName,
        const QString& value,
        const QString& functionStatus,
        const QString& onFail = "continue",
        const int& timeOut = 3
    );
    void makeAmxRemoteServerExternalToolWidgetParams(
        const QString& iIndex,
        const QString& iEtat,
        const int& timeOut = 3
    );

    struct multiSetParams {
        QString param;
        QString value;
        QString functionStatus;
        multiSetParams(const QString& in_param, const QString& in_val, const QString& in_functionStatus) :
            param(in_param), value(in_val), functionStatus(in_functionStatus) {}
    };
    void makeMultiSetParams(
        const QList<multiSetParams>& multiSetParams,
        const QString& onFail,
        const int& timeOut
    );
    QString _complement;
    bool _isAction = false;
};
