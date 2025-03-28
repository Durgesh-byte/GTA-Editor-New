#include "GTACMDSCXMLCheckBITEMessage.h"
#include "GTASCXMLState.h"
#include "GTAUtil.h"
#include "GTACMDSCXMLLog.h"
#include "GTACMDSCXMLUtils.h"

GTACMDSCXMLCheckBITEMessage::GTACMDSCXMLCheckBITEMessage(GTACheckBiteMessage *pCmdNode, const QString &_StateId, const QString &iTargetId):
        GTACMDSCXMLBase(pCmdNode,_StateId,iTargetId)
{
    _ChkBiteCmd = pCmdNode;
}

GTACMDSCXMLCheckBITEMessage::GTACMDSCXMLCheckBITEMessage()
{

}

GTACMDSCXMLCheckBITEMessage::~GTACMDSCXMLCheckBITEMessage()
{

}
QString GTACMDSCXMLCheckBITEMessage::getSCXMLString() const
{
    return getSCXMLState().getSCXMLString(_isUSCXMLEn);
}

GTASCXMLState GTACMDSCXMLCheckBITEMessage::getSCXMLState()const
{
    GTASCXMLState state;

    if(_ChkBiteCmd != NULL)
    {
        state = getBITEMessageState();

        QString timeoutEventName = QString("%1_timeout").arg(_StateId);
        QString actiononFail = _ChkBiteCmd->getActionOnFail();

        GTASCXMLTransition timeoutTx;
        timeoutTx.setEvent(timeoutEventName);
        timeoutTx.setStatus(SCXML_STATUS_TIMEOUT);
        if (actiononFail !="stop")
            timeoutTx.setTarget(_TargetId);
        else
            timeoutTx.setTarget(SCXML_FINAL_STATE_ID);


        GTASCXMLLog timeoutLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::TIMEOUT_LOG,_ChkBiteCmd->getInstanceName(),"","");
        timeoutTx.insertLog(timeoutLog);
        state.insertTransitions(timeoutTx,true);


        //innsert logs
        GTACMDSCXMLLog log;
        log.setReportType(GTACMDSCXMLLog::REP_ACTION,GTACMDSCXMLLog::REP_ACTCHK);
        log.setCommandName(_CmdName);
        log.setCommandInstanceId(_ChkBiteCmd->getInstanceName());
        log.setCommandDescription(_ChkBiteCmd->getComment());
        log.setCommandStatement(_ChkBiteCmd->getStatement());
        QStringList requirements = _ChkBiteCmd->getRequirements();
        log.insertRequirements(requirements);
        QStringList varList;
        GTACMDSCXMLUtils::insertVariablesInLogReport(_ParamInfoList,varList,log);

        GTASCXMLOnExit onExit = state.getOnExit();
        if(_GenNewLogFormat)
            onExit.insertExtraInfo(log.getLogElem());
        state.setOnExit(onExit);
        GTACMDSCXMLUtils::logAllParameters(_ChkBiteCmd->getInstanceName(),_ParamInfoList,state);
    }

    return state;
}

GTASCXMLState GTACMDSCXMLCheckBITEMessage::getBITEMessageState() const
{
    GTASCXMLState checkBiteState;
    checkBiteState.setId(_StateId);

    if(_ChkBiteCmd == NULL)
        return checkBiteState;


    QString message = _ChkBiteCmd->getMessage();
    QString delay = _ChkBiteCmd->getBuffDepth();
    QString sIsDisplayed = _ChkBiteCmd->getCondition();
    QString retName = QString("_%1_return").arg(_StateId);
    QString timeoutEventName = QString("%1_timeout").arg(_StateId);


    GTASCXMLData data;
    data.setDataId(retName);

    GTASCXMLDataModel datamodel;
    datamodel.insertDataItem(data);

    checkBiteState.setDataModel(datamodel);


    if(message.contains("<"))
        message.replace("<","&lt;");

    GTASCXMLInvoke invoke;
    GTACMDSCXMLUtils::getInvokeFromTemplate("checkBITEMessage.scxml",invoke);

    QString invokeId = QString("%1_invoke").arg(_StateId);


    invoke.setInvokeId(invokeId);

    GTASCXMLXMLRPC func=  invoke.getXMLRPCFunction();

    GTASCXMLXMLRPC::funcArg args;
    func.getArgumentByName("content",args);

    args.value = (QString("%1").arg(message)).trimmed();

    func.updateArgumentByName("content",args);

    func.getArgumentByName("ID",args);

    args.value = "12354";

    func.updateArgumentByName("ID",args);


    func.getArgumentByName("ContentType",args);

    args.value = QString("bite_message");

    func.updateArgumentByName("ContentType",args);

    func.getArgumentByName("condition",args);

    args.value = QString("EQ");

    func.updateArgumentByName("condition",args);

    func.getArgumentByName("condition_value",args);

    args.value = QString("none");

    func.updateArgumentByName("condition_value",args);



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


    checkBiteState.insertInvokes(invoke);

    GTASCXMLTransition successTx;
    GTASCXMLTransition failTx;

    successTx.setTarget(_TargetId);
    successTx.setEvent(QString("done.invoke.%1").arg(invokeId));

    QString actiononFail = _ChkBiteCmd->getActionOnFail();
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

    GTASCXMLLog successLog =  GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG,_ChkBiteCmd->getInstanceName(),"ReturnStatus","OK",false);
    GTASCXMLLog failLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::FAIL_LOG,_ChkBiteCmd->getInstanceName(),"ReturnStatus","OK",false);

    successTx.insertLog(successLog);
    failTx.insertLog(failLog);

    checkBiteState.insertTransitions(successTx);
    checkBiteState.insertTransitions(failTx);

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


    checkBiteState.setOnEntry(onEntry);

    return checkBiteState;
}
