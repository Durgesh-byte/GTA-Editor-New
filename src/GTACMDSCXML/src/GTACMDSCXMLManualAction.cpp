#include "GTACMDSCXMLManualAction.h"
#include "GTASCXMLState.h"
#include "GTAUtil.h"
#include "GTACMDSCXMLLog.h"
#include "GTACMDSCXMLUtils.h"
#include "GTAUtil.h"
GTACMDSCXMLManualAction::GTACMDSCXMLManualAction(GTAActionManual *pCmdNode, const QString &iStateId, const QString &iTargetId):
        GTACMDSCXMLBase(pCmdNode,iStateId,iTargetId)
{
    _ManCmd = pCmdNode;
}

GTACMDSCXMLManualAction::GTACMDSCXMLManualAction()
{

}

GTACMDSCXMLManualAction::~GTACMDSCXMLManualAction()
{

}
QString GTACMDSCXMLManualAction::getSCXMLString() const
{
    return getSCXMLState().getSCXMLString(_isUSCXMLEn);
}

GTASCXMLState GTACMDSCXMLManualAction::getSCXMLState()const
{
    GTASCXMLState state;

    if(_ManCmd->isIgnoredInSCXML())
    {
        return getIgnoredState(_StateId,_TargetId);
    }
    if(_ManCmd != NULL)
    {
        if(!_ManCmd->getUserFeedback())
            state = getManualActionPopUpState(_StateId,_TargetId);
        else
            state = getManualActionFeedBackState(_StateId,_TargetId);

        GTASCXMLOnEntry onEntry = state.getOnEntry();
        GTASCXMLOnExit onExit = state.getOnExit();

        bool isTimeoutOk = false;
        double dtimeout = _Timeout.toDouble(&isTimeoutOk);
        if(isTimeoutOk && (dtimeout>0))
        {
            QString timeoutEventName = QString("%1_timeout").arg(_StateId);
            QString timeoutEventID = QString("%1_timeoutID").arg(_StateId);
            GTASCXMLSend send;
            send.setEvent(timeoutEventName);
            send.setId(timeoutEventID);
            send.setDelay(QString::number(dtimeout)+"ms");
            onEntry.insertSend(send);
            state.setOnEntry(onEntry);

            //always cancel the timeout while exiting the state
            //it has served it's purpose
            GTASCXMLCancel canceltimeout;
            canceltimeout.setSendEventId(timeoutEventID);
            onExit.insertCancel(canceltimeout);

            GTASCXMLTransition timeoutTx;
            timeoutTx.setEvent(timeoutEventName);
            timeoutTx.setStatus(SCXML_STATUS_TIMEOUT);
            timeoutTx.setTarget(_TargetId);

            GTASCXMLLog timeoutLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::TIMEOUT_LOG,_ManCmd->getInstanceName(),"","");
            timeoutTx.insertLog(timeoutLog);
            //state.insertTransitions(timeoutTx,true);
			//keeping one timeout transition
			state.insertTransitions(timeoutTx);
        }

        //innsert logs
        GTACMDSCXMLLog log;
        log.setReportType(GTACMDSCXMLLog::REP_ACTION,GTACMDSCXMLLog::REP_ACTCHK);
        log.setCommandName(_CmdName);
        log.setCommandInstanceId(_ManCmd->getInstanceName());
        log.setCommandDescription(_ManCmd->getComment());
        log.setCommandStatement(_ManCmd->getStatement());
        QStringList requirements = _ManCmd->getRequirements();
        log.insertRequirements(requirements);
        QStringList varList;
        GTACMDSCXMLUtils::insertVariablesInLogReport(_ParamInfoList,varList,log);


        if(_GenNewLogFormat)
            onExit.insertExtraInfo(log.getLogElem());
        state.setOnExit(onExit);
        GTACMDSCXMLUtils::logAllParameters(_ManCmd->getInstanceName(),_ParamInfoList,state);
    }

    return state;
}
GTASCXMLState GTACMDSCXMLManualAction::getManualActionFeedBackState(const QString &iStateId, const QString &iTargetId) const
{

    GTASCXMLState manActState;
    GTASCXMLState setParamState;
    GTASCXMLState popupState;

    QString message;
    QString userMessage;

    bool isUSCXML = GTAUtil::isUSCXMLScheduler();

    message = _ManCmd->getHtmlMessage();
    if (!isUSCXML)
    {
        userMessage = message.replace("\n","<br></br>");
        userMessage.replace("`","_");
    }
    userMessage.replace("\"","$&GTA_DOUBLE_QUOTE&$");

    updateParameterWithValues(iStateId, userMessage);

    QString instanceId = _ManCmd->getInstanceName();
    QString retType = _ManCmd->getUserFeedback() ? _ManCmd->getReturnType() : "";
    GTAICDParameter lHSParam;
    GTAICDParameter rHSParam;

    QString mainStateFinalId = QString("%1_final").arg(iStateId);
    QString popupStateId = QString("%1_Popup").arg(iStateId);
    QString localParam = QString("%1_localParam").arg(iStateId);

    QString popupTargetId = mainStateFinalId;

    if(!_ParamInfoList.isEmpty())
    {
        lHSParam = _ParamInfoList.at(0);
    }
    if(lHSParam.getApplicationName() != "")
    {

        popupTargetId = QString("%1_SetParameter").arg(iStateId);

        rHSParam.setSignalName(localParam);
        rHSParam.setTempParamName(localParam);
        rHSParam.setValueType(retType);
        rHSParam.setToolName("NA");

//        setParamState = GTACMDSCXMLUtils::createSetParamInvokeState(lHSParam,rHSParam,popupTargetId,mainStateFinalId,false,QString(""),QString(),_HiddenDelay);
        //DEV: keeping FS value as NO until XML-RPC Server is fixed
        setParamState = GTACMDSCXMLUtils::createSetParamInvokeState(lHSParam,rHSParam,popupTargetId,mainStateFinalId,false,QString(ACT_FSSSM_NO),QString(),_HiddenDelay);

        GTASCXMLTransition tx1;
        tx1.setTarget(mainStateFinalId);
        tx1.setStatus(TX_STATUS_OK);

        setParamState.insertTransitions(tx1);
    }
    else
    {
        rHSParam = lHSParam;
        rHSParam.setValueType(retType);
        rHSParam.setToolName("NA");
    }

    popupState = GTACMDSCXMLUtils::createParamPopUpInvokeState(userMessage,popupStateId,popupTargetId,rHSParam,instanceId);

    manActState.insertStates(popupState);
    manActState.insertStates(setParamState);

    manActState.setId(iStateId);

    GTASCXMLTransition tx;
    tx.setTarget(iTargetId);
    tx.setStatus(TX_STATUS_OK);

    GTASCXMLLog successLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::INFO_LOG,_ManCmd->getInstanceName(),"message",QString("%1").arg(_ManCmd->getMessage()),false);

    tx.insertLog(successLog);


    manActState.insertTransitions(tx);


    QString timeoutEventName = QString("%1_timeout").arg(iStateId);
    GTASCXMLSend timeout;
    timeout.setDelay("3000ms");
    timeout.setEvent(timeoutEventName);

    GTASCXMLOnEntry onEntry;
    //onEntry.insertSend(timeout);

    manActState.setOnEntry(onEntry);

    GTASCXMLTransition timeoutTx;
    timeoutTx.setEvent(timeoutEventName);
    timeoutTx.setStatus("Timeout");
    timeoutTx.setTarget(iTargetId);

    GTASCXMLLog timeoutLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::TIMEOUT_LOG,_ManCmd->getInstanceName(),"","");

    timeoutTx.insertLog(timeoutLog);

    //manActState.insertTransitions(timeoutTx,true);



    GTASCXMLData data;
    if(retType == "float")
        data.setDataExpr("0.0");
    else if(retType == "int")
        data.setDataExpr("0");
    else if(retType == "bool")
        data.setDataExpr("false");
    else
        data.setDataExpr("");

    data.setDataId(localParam);

    GTASCXMLDataModel datamodel;
    datamodel.insertDataItem(data);

    manActState.setDataModel(datamodel);

    return manActState;
}

GTASCXMLState GTACMDSCXMLManualAction::getManualActionPopUpState(const QString &iStateId, const QString &iTargetId) const
{
    QString instanceName = _ManCmd->getInstanceName();
    QString commandName = QString("%1_%2").arg(_ManCmd->getAction(),_ManCmd->getComplement());

//    GTAAppSettings settings;

    QString message;
    QString userMessage;
    bool isUSCXML = GTAUtil::isUSCXMLScheduler();
    message = _ManCmd->getHtmlMessage();
    if (!isUSCXML)
    {
        userMessage = message.replace("\n","<br></br>");
        userMessage.replace("`","_");
    }
    userMessage.replace("\"","$&GTA_DOUBLE_QUOTE&$");

    updateParameterWithValues(iStateId, userMessage);

    bool isAckTrue = _ManCmd->getAcknowledgetment();

    GTASCXMLState ManActState;

    ManActState = GTACMDSCXMLUtils::createPopUpInvokeState(userMessage,iStateId,iTargetId,isAckTrue,instanceName);

    GTASCXMLOnExit onExit;

    GTACMDSCXMLLog log;
    log.setReportType(GTACMDSCXMLLog::REP_ACTION,(GTACMDSCXMLLog::REPORT_SUB_TYPE)0);
    log.setCommandName(commandName);
    log.setCommandInstanceId(instanceName);
    log.setCommandDescription(_ManCmd->getComment());
    log.setCommandStatement(_ManCmd->getStatement());

    LogVariableInfo var1;

    var1._VarName = QString("%1_Message").arg(_StateId);
    var1._expectedValue = QString("'%1'").arg(userMessage);

    var1._VarType = "INTERNAL";

    log.insertVariables(var1);

    for(int i=0;i<_ParamInfoList.count();i++)
    {
        LogVariableInfo var2;

        GTAICDParameter param = _ParamInfoList.at(i);
        var2._VarName = param.getTempParamName();

        if(param.getTempParamName() == QString("%1_localParam").arg(_StateId))
            var2._VarType = "INTERNAL";
        else if(param.getApplicationName() != "")
        {
            var2._VarType = "DATABASE";
        }
        else
        {
            var2._VarType = "LOCAL";
        }
    }


    if(isAckTrue)
    {
        QString timeoutEventName = QString("%1_timeout").arg(iStateId);
        GTASCXMLSend timeout;
        timeout.setDelay("3000ms");
        timeout.setEvent(timeoutEventName);

        GTASCXMLOnEntry onEntry;
        //onEntry.insertSend(timeout);

        ManActState.setOnEntry(onEntry);

        GTASCXMLTransition timeoutTx;
        timeoutTx.setEvent(timeoutEventName);
        timeoutTx.setStatus("Timeout");
        timeoutTx.setTarget(iTargetId);

        GTASCXMLLog timeoutLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::TIMEOUT_LOG,_ManCmd->getInstanceName(),"","");


        timeoutTx.insertLog(timeoutLog);

        //ManActState.insertTransitions(timeoutTx,true);
    }
    if(_GenNewLogFormat)
        onExit.insertExtraInfo(log.getLogElem());

    ManActState.setOnExit(onExit);

    return ManActState;
}
GTASCXMLState GTACMDSCXMLManualAction::getIgnoredState(const QString &iStateId, const QString &iTargetId) const
{
    GTASCXMLState state;
    if(_ManCmd)
    {
        GTASCXMLOnEntry onEntry;
        _ManCmd->setIgnoreInSCXML(true);

        //log for ignore manual action is mentioned as a Detail Log instead of Main Log
        GTASCXMLLog ignoreLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::INFO_LOG,_ManCmd->getInstanceName(),QString(),QString(),false);

        //converting log into Main Log for managing linenumbers in LTRA
        QString logexpr = QString("%1").arg(ignoreLog.getExpression());
        logexpr.replace(":;:1",":;:0");
        ignoreLog.setExpression(logexpr);

        onEntry.insertLog(ignoreLog);

        state.setOnEntry(onEntry);

        //innsert logs
        GTACMDSCXMLLog log;
        log.setReportType(GTACMDSCXMLLog::REP_ACTION,GTACMDSCXMLLog::REP_ACTCHK);
        log.setCommandName(_CmdName);
        log.setCommandInstanceId(_ManCmd->getInstanceName());
        log.setCommandDescription(_ManCmd->getComment());
        log.setCommandStatement(_ManCmd->getStatement());
        QStringList requirements = _ManCmd->getRequirements();
        log.insertRequirements(requirements);
        QStringList varList;
        GTACMDSCXMLUtils::insertVariablesInLogReport(_ParamInfoList,varList,log);

        GTASCXMLOnExit onExit = state.getOnExit();
        if(_GenNewLogFormat)
            onExit.insertExtraInfo(log.getLogElem());
        state.setOnExit(onExit);
    }
    state.setId(iStateId);
    GTASCXMLTransition tx;
    tx.setTarget(iTargetId);
    tx.setStatus("OK");
	state.insertTransitions(tx);
    return state;
}

/**
 * @brief Update the popup message if message contains some for_each loop parameters
 * @param iStateId 
 * @param iUserMessage 
*/
void GTACMDSCXMLManualAction::updateParameterWithValues(const QString &iStateId, QString &iUserMessage) const
{
    QStringList paramMessage;
    QStringList contents = _ManCmd->getMessage().split(" ");
    foreach(QString content, contents)
    {
        content.trimmed();
        if (content.isEmpty())
        {
            continue;
        }

        QStringList parameters = content.split(",");
        for (int i = 0; i < parameters.count();i++)            
        {
            QString line = parameters.at(i);
            if (line.contains("line.") && !paramMessage.contains(line))
            {
                paramMessage.append(line);
            }
        }
    }
    foreach(QString param, paramMessage)
    {
        if (iUserMessage.contains(param))
        {
            QString stateRef = iStateId.split("ForEachStates").first();
            QString paramName = param.split("__").last();
            QString paramValue = QString("'+%1line_arr[%1arr_cntr].%2+'").arg(stateRef, paramName);
            iUserMessage.replace(param, QString("<br></br>%1 = %2").arg(paramName, paramValue));
            iUserMessage.replace(",<br></br>", "<br></br>");
        }
    }
}