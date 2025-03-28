#include "GTACMDSCXMLCheckFWCMessage.h"
#include "GTASCXMLState.h"
#include "GTAUtil.h"
#include "GTACMDSCXMLLog.h"
#include "GTACMDSCXMLUtils.h"

GTACMDSCXMLCheckFWCMessage::GTACMDSCXMLCheckFWCMessage(GTACheckFwcWarning *pCmdNode, const QString &iStateId, const QString &iTargetId):
        GTACMDSCXMLBase(pCmdNode,iStateId,iTargetId)
{
    _ChkFwcCmd = pCmdNode;
}

GTACMDSCXMLCheckFWCMessage::GTACMDSCXMLCheckFWCMessage()
{

}

GTACMDSCXMLCheckFWCMessage::~GTACMDSCXMLCheckFWCMessage()
{

}
QString GTACMDSCXMLCheckFWCMessage::getSCXMLString() const
{
    return getSCXMLState().getSCXMLString(_isUSCXMLEn);
}

GTASCXMLState GTACMDSCXMLCheckFWCMessage::getSCXMLState()const
{
    GTASCXMLState state;

    if(_ChkFwcCmd != NULL)
    {
        state = getFWCWarningMessageState();

        QString timeoutEventName = QString("%1_timeout").arg(_StateId);
        QString actiononFail = _ChkFwcCmd->getActionOnFail();

        GTASCXMLTransition timeoutTx;
        timeoutTx.setEvent(timeoutEventName);
        timeoutTx.setStatus(SCXML_STATUS_TIMEOUT);
        if (actiononFail !="stop")
            timeoutTx.setTarget(_TargetId);
        else
            timeoutTx.setTarget(SCXML_FINAL_STATE_ID);

        GTASCXMLLog timeoutLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::TIMEOUT_LOG,_ChkFwcCmd->getInstanceName(),"","");
        timeoutTx.insertLog(timeoutLog);
        state.insertTransitions(timeoutTx,true);


        //innsert logs
        GTACMDSCXMLLog log;
        log.setReportType(GTACMDSCXMLLog::REP_ACTION,GTACMDSCXMLLog::REP_ACTCHK);
        log.setCommandName(_CmdName);
        log.setCommandInstanceId(_ChkFwcCmd->getInstanceName());
        log.setCommandDescription(_ChkFwcCmd->getComment());
        log.setCommandStatement(_ChkFwcCmd->getStatement());
        QStringList requirements = _ChkFwcCmd->getRequirements();
        log.insertRequirements(requirements);
        QStringList varList;
        GTACMDSCXMLUtils::insertVariablesInLogReport(_ParamInfoList,varList,log);

        GTASCXMLOnExit onExit = state.getOnExit();
        if(_GenNewLogFormat)
            onExit.insertExtraInfo(log.getLogElem());
        state.setOnExit(onExit);
        GTACMDSCXMLUtils::logAllParameters(_ChkFwcCmd->getInstanceName(),_ParamInfoList,state);
    }

    return state;
}

GTASCXMLState GTACMDSCXMLCheckFWCMessage::getFWCWarningMessageState() const
{
	
    GTASCXMLState checkFwcWarningState;
    checkFwcWarningState.setId(_StateId);

    if(_ChkFwcCmd == NULL)
        return checkFwcWarningState;

    QString message = _ChkFwcCmd->getMessage();
    QString heading = _ChkFwcCmd->getHeader();
    QString color = _ChkFwcCmd->getFWCWarningColor();
    QString delay = _ChkFwcCmd->getWaitForBuffer();


    if(message.contains("<"))
        message.replace("<","&lt;");

    QString sIsDisplayed = _ChkFwcCmd->getCondition();

    QString retName = QString("_%1_return").arg(_StateId);


    GTASCXMLData data;
    data.setDataId(retName);

    GTASCXMLDataModel datamodel;
    datamodel.insertDataItem(data);

    checkFwcWarningState.setDataModel(datamodel);

    QString timeoutEventName = QString("%1_timeout").arg(_StateId);

    GTASCXMLInvoke invoke;
    GTACMDSCXMLUtils::getInvokeFromTemplate("checkFWCMessage.scxml",invoke);

    QString invokeId = QString("%1_invoke").arg(_StateId);

    invoke.setInvokeId(invokeId);

    GTASCXMLXMLRPC func=  invoke.getXMLRPCFunction();

    GTASCXMLXMLRPC::funcArg args;
    func.getArgumentByName("content",args);

    args.value = (QString("%1 %2").arg(heading,message)).trimmed();

    func.updateArgumentByName("content",args);

    func.getArgumentByName("ID",args);

    args.value = "1234";

    func.updateArgumentByName("ID",args);

    func.getArgumentByName("condition_value",args);

    args.value = QString("%1").arg(color);

    func.updateArgumentByName("condition_value",args);


    func.getArgumentByName("condition",args);

    args.value = "EQ";

    func.updateArgumentByName("condition",args);

    func.getArgumentByName("ContentType",args);

    args.value = QString("%1").arg("warning");

    func.updateArgumentByName("ContentType",args);


    func.getArgumentByName("delay",args);

    args.value = QString("%1").arg(delay);

    func.updateArgumentByName("delay",args);

    GTASCXMLXMLRPC::funcReturn ret;
    func.getReturnArgument(ret);

    ret.name =retName;

    func.updateReturnArgument(ret);

    invoke.setInvokeFunction(func);

    GTASCXMLAssign assign;

    assign.setExpression(QString("_event.data.%1").arg(retName));
    assign.setLocation(retName);

    invoke.insertAssign(assign);

    checkFwcWarningState.insertInvokes(invoke);

    GTASCXMLTransition successTx;
    GTASCXMLTransition failTx;

    successTx.setTarget(_TargetId);
    successTx.setEvent(QString("done.invoke.%1").arg(invokeId));

    QString actiononFail = _ChkFwcCmd->getActionOnFail();
    failTx.setTarget(_TargetId);
    if(actiononFail != "continue")
        failTx.setTarget(SCXML_FINAL_STATE_ID);

    failTx.setEvent(QString("done.invoke.%1").arg(invokeId));

    if(sIsDisplayed.trimmed().isEmpty())
    {
        successTx.setCondition(QString("%1.ResultStatus == 'OK'").arg(retName));
        failTx.setCondition(QString("%1.ResultStatus != 'OK'").arg(retName));
    }
    else
    {
        successTx.setCondition(QString("%1.ResultStatus != 'OK'").arg(retName));
        failTx.setCondition(QString("%1.ResultStatus == 'OK'").arg(retName));
    }

    GTASCXMLLog successLog =  GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG,_ChkFwcCmd->getInstanceName(),"ReturnStatus","OK",false);
    GTASCXMLLog failLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::FAIL_LOG,_ChkFwcCmd->getInstanceName(),"ReturnStatus","KO",false);

    successTx.insertLog(successLog);
    failTx.insertLog(failLog);

    checkFwcWarningState.insertTransitions(successTx);
    checkFwcWarningState.insertTransitions(failTx);

    GTASCXMLSend send;
    send.setEvent(timeoutEventName);

    bool isNum = false;
    int idealy = delay.toInt(&isNum);
    if(idealy)
        send.setDelay(QString("%1ms").arg(QString::number(idealy*1000 + 3000)));
    else
        send.setDelay(QString("%1ms").arg(QString::number(3000)));

    GTASCXMLOnEntry onEntry;
    onEntry.insertSend(send);


    checkFwcWarningState.setOnEntry(onEntry);

    return checkFwcWarningState;

}
