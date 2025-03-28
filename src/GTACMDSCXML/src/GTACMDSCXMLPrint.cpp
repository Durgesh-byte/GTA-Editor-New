#include "GTACMDSCXMLPrint.h"
#include "GTASCXMLState.h"
#include "GTAUtil.h"
#include "GTACMDSCXMLLog.h"
#include "GTACMDSCXMLUtils.h"

GTACMDSCXMLPrint::GTACMDSCXMLPrint(GTAActionPrint *pCmdNode, const QString &iStateId, const QString &iTargetId):
        GTACMDSCXMLBase(pCmdNode,iStateId,iTargetId)
{
    _PrintCmd = pCmdNode;
}

GTACMDSCXMLPrint::GTACMDSCXMLPrint()
{

}

GTACMDSCXMLPrint::~GTACMDSCXMLPrint()
{

}
QString GTACMDSCXMLPrint::getSCXMLString() const
{
    return getSCXMLState().getSCXMLString(_isUSCXMLEn);
}

GTASCXMLState GTACMDSCXMLPrint::getSCXMLState()const
{
    GTASCXMLState state;

    bool rc = getDebugModeStatus();
    GTASCXMLState megaState;
    QString executableStateID,executableTargetID,finaltargetId,preDebugVar;
    QString preDebugStateId = QString("%1_predebug").arg(_StateId);
    QString debugStateId = QString("%1_debug").arg(_StateId);
    if (rc)
    {
        executableStateID = QString("%1_exec").arg(_StateId);
        executableTargetID = QString("%1_debug").arg(_StateId);
        preDebugVar = QString("%1_preDebugVar").arg(preDebugStateId);
        finaltargetId = QString("%1_final").arg(_StateId);
    }
    else
    {
        executableStateID = _StateId;
        executableTargetID = _TargetId;
        preDebugVar = "";
        finaltargetId = "";
    }

    if(_PrintCmd != NULL)
    {
        if(_PrintCmd->getComplement() == COM_PRINT_MSG)
        {
            state = getSCXMLForPrintMessage(executableStateID,executableTargetID,finaltargetId,preDebugVar);
        }
        else if(_PrintCmd->getComplement() == COM_PRINT_PARAM)
        {
            state = getSCXMLForPrintParams(executableStateID,executableTargetID,finaltargetId,preDebugVar);
        }
        else
            return state;
        GTACMDSCXMLUtils::logAllParameters(_PrintCmd->getInstanceName(),_ParamInfoList,state);
    }

    if (rc)
    {
        megaState.setId(_StateId);

        GTASCXMLTransition tx;
        tx.setTarget(_TargetId);
        megaState.insertTransitions(tx);

        GTASCXMLState preDebugState = GTACMDSCXMLUtils::getPreDebugState(preDebugStateId,executableStateID,_PrevStateId,_PrintCmd->getInstanceName());
        megaState.insertStates(preDebugState);
        megaState.insertStates(state);
        GTASCXMLState debugState = GTACMDSCXMLUtils::getDebugState(debugStateId,finaltargetId,state.getVariableList(),_ParamInfoList);
        megaState.insertStates(debugState);

        megaState.setInitId(preDebugStateId);
        return megaState;
    }
    else
    {
        return state;
    }
}

/* This function allows to write SCXML part for a PRINT Message action
* @return: GTASCXMLState
*/
GTASCXMLState GTACMDSCXMLPrint::getSCXMLForPrintMessage(const QString iStateId, const QString iTargetId, const QString &iFinalId, const QString &iDecisionVar) const
{
    QStringList values = _PrintCmd->getValues();
    QString instanceName = _PrintCmd->getInstanceName();
    QString commandName = QString("%1_%2").arg(_PrintCmd->getAction(),_PrintCmd->getComplement());
    QString message = values.first();

    commandName.replace(" ","_");
    message.replace("\n","<br></br>");
    message.replace(GTA_CIC_IDENTIFIER,"_");

    GTASCXMLState printMessageState;
    printMessageState.setId(iStateId);

    GTASCXMLOnExit onExit;

    GTACMDSCXMLLog log;
    log.setReportType(GTACMDSCXMLLog::REP_ACTION,(GTACMDSCXMLLog::REPORT_SUB_TYPE)0);
    log.setCommandName(commandName);
    log.setCommandInstanceId(instanceName);
    log.setCommandDescription(_PrintCmd->getComment());
    log.setCommandStatement(_PrintCmd->getStatement());
    log.insertRequirements(_PrintCmd->getRequirements());

    LogVariableInfo var1;

    var1._VarName = QString("%1_Message").arg(iStateId);
    var1._expectedValue = QString("'%1'").arg(message);
    var1._VarType = "INTERNAL";

    if (getDebugModeStatus())
    {
        printMessageState.insertVarList(QStringList(var1._VarName));
    }
	
    log.insertVariables(var1);
    if(_GenNewLogFormat)
        onExit.insertExtraInfo(log.getLogElem());

    printMessageState.setOnExit(onExit);

    GTASCXMLData data;
    data.setDataExpr(var1._expectedValue);
    data.setDataId(var1._VarName);

    GTASCXMLDataModel datamodel;
    datamodel.insertDataItem(data);

    GTAScxmlLogMessage logMessage1;
    logMessage1.CurrentValue = message;
	logMessage1.ExpectedValue = QString();
    logMessage1.Identifier = instanceName;
    logMessage1.FunctionStatus = "NA";

    logMessage1.Result = GTAScxmlLogMessage::OK;
    logMessage1.Status = GTAScxmlLogMessage::Success;
    logMessage1.LOD = GTAScxmlLogMessage::Main;
    logMessage1.isCurrentValParam = false;
    QString log1 = logMessage1.getLogMessage();
    log1 = log1.remove("+");
    log1 = log1.remove("'");

    GTASCXMLLog log2;
    log2.setLabel("Info");
    log2.setExpression(QString("'%1'").arg(log1));

    GTASCXMLTransition tx;
    tx.setTarget(iTargetId);
    if (!iDecisionVar.isEmpty())
    {
        GTASCXMLData subData;
        subData.setDataExpr("");
        subData.setDataId(QString("%1_Result").arg(iStateId));
        datamodel.insertDataItem(subData);

        GTASCXMLAssign assign;
        assign.setExpression(var1._VarName);
        assign.setLocation(QString("%1_Result").arg(iStateId));

        tx.setCondition(QString("(%1 == 'true')").arg(iDecisionVar));
        tx.insertAssign(assign);

        //additional tx
        GTASCXMLTransition tx1;
        tx1.setTarget(iFinalId);
        tx1.setCondition(QString("(%1 != 'true')").arg(iDecisionVar));
        tx1.setStatus("OK");
        tx1.insertLog(log2);
        tx1.insertAssign(assign);
        printMessageState.insertTransitions(tx1);
    }

    printMessageState.setDataModel(datamodel);

    tx.setStatus("OK");
    tx.insertLog(log2);
    printMessageState.insertTransitions(tx);

    GTAICDParameter icdParam;
    icdParam.setToolName("NA");
    icdParam.setTempParamName(var1._VarName);
    icdParam.setSignalName(var1._VarName);
    QList<GTAICDParameter> localParams;
    localParams.append(icdParam);
    GTACMDSCXMLUtils::logAllParameters(_PrintCmd->getInstanceName(),localParams, printMessageState);

    return printMessageState;
}

/* This function allows to write SCXML part for a PRINT Parameter action
* @return: GTASCXMLState
*/
GTASCXMLState GTACMDSCXMLPrint::getSCXMLForPrintParams(const QString iStateId,const QString iTargetId, const QString &iFinalId, const QString &iDecisionVar) const
{
    QString instanceName = _PrintCmd->getInstanceName();
    QString commandName = QString("%1_%2").arg(_PrintCmd->getAction(),_PrintCmd->getComplement());

    GTASCXMLState printParamState;

    printParamState = GTACMDSCXMLUtils::createGetParamInvokeState(_ParamInfoList,iStateId,iTargetId,"",false,instanceName,true,_HiddenDelay);

    GTASCXMLOnExit onExit;

    GTACMDSCXMLLog log;
    log.setReportType(GTACMDSCXMLLog::REP_ACTION,(GTACMDSCXMLLog::REPORT_SUB_TYPE)0);
    log.setCommandName(commandName);
    log.setCommandInstanceId(instanceName);
    log.setCommandDescription(_PrintCmd->getComment());
    log.setCommandStatement(_PrintCmd->getStatement());

    QString timeoutEventName = QString("%1_timeout").arg(iStateId);
    QString koEventName = QString("error.execution");
    QString timeoutEventID = QString("%1_timeoutID").arg(iStateId);
    GTASCXMLSend timeout;
    if (!_HiddenDelay.isEmpty())
    {
        int delay = (_HiddenDelay.split("ms").first()).toInt();
        delay += 3000;
        timeout.setDelay(QString("%1ms").arg(QString::number(delay)));
    }
    else
        timeout.setDelay("3000ms");
    timeout.setEvent(timeoutEventName);
    timeout.setId(timeoutEventID);

    GTASCXMLOnEntry onEntry;
    onEntry.insertSend(timeout);

    printParamState.setOnEntry(onEntry);

    GTASCXMLTransition timeoutTx;
    GTASCXMLTransition koTx;
	GTASCXMLLog timeoutLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::TIMEOUT_LOG, _PrintCmd->getInstanceName(), "true", "true");
    GTASCXMLLog failLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::KO_FAIL_LOG, _PrintCmd->getInstanceName(), "ko", "ko");
	if (!iDecisionVar.isEmpty())

	/*QStringList varList = QStringList() << _PrintCmd->getValues();
	GTASCXMLLog timeoutLog;
	
	// For Each variable we have to iterate and insert a new log
	foreach(QString iValue, varList)
	{
		timeoutLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::TIMEOUT_LOG, _PrintCmd->getInstanceName(), QString("Timeout (%1)").arg(iValue), iValue);
		QString expr1 = timeoutLog.getExpression();
		expr1.replace("(" + iValue + ")", QString("('+%1+')").arg(iValue));
		timeoutLog.setExpression(expr1);
		timeoutTx.insertLog(timeoutLog);
	}
	
	if (!iDecisionVar.isEmpty())
    */{
        GTASCXMLTransition ExistingTx = printParamState.getTransitions().at(0);
        GTASCXMLTransition NewTx;
        NewTx.setStatus(ExistingTx.getStatus());
        NewTx.insertLog(ExistingTx.getLogs().at(0));
        NewTx.setTarget(ExistingTx.getTarget());
        NewTx.setCondition(QString("(%1 == 'true')").arg(iDecisionVar));
        NewTx.setEvent(ExistingTx.getEvent());
        printParamState.removeTransition(ExistingTx);
        printParamState.insertTransitions(NewTx);
        //additional tx
        NewTx.setTarget(iFinalId);
        NewTx.setCondition(QString("(%1 != 'true')").arg(iDecisionVar));
        printParamState.insertTransitions(NewTx);

        timeoutTx.setCondition(QString("(%1 == 'true')").arg(iDecisionVar));

        //additional tx
        GTASCXMLTransition tx1;
        tx1.setCondition(QString("(%1 != 'true')").arg(iDecisionVar));
        tx1.setTarget(iFinalId);
        tx1.setEvent(timeoutEventName);
        tx1.setStatus("Timeout");
        tx1.insertLog(timeoutLog);
        printParamState.insertTransitions(tx1);
    }

    timeoutTx.setTarget(iTargetId);
    timeoutTx.setEvent(timeoutEventName);
    timeoutTx.setStatus("Timeout");
	timeoutTx.insertLog(timeoutLog);

    koTx.setTarget(iTargetId);
    koTx.setEvent(koEventName);
    koTx.setStatus("KO");
    koTx.insertLog(failLog);

    // JIRA-133 
    printParamState.insertTransitions(timeoutTx);
    printParamState.insertTransitions(koTx);

    GTASCXMLCancel cancel;
    cancel.setSendEventId(timeoutEventID);
    onExit.insertCancel(cancel);

	for (int i = 0; i<_ParamInfoList.count(); i++)
	{

		GTAICDParameter param = _ParamInfoList.at(i);

		GTACMDSCXMLUtils::updateWithGenToolParam(param);


	/*foreach(GTAICDParameter iParam, _ParamInfoList)
	{
		GTAICDParameter param = iParam; 
		QString lName = param.getName();
		param.setTempParamName(lName);
		*/bool dbParam = false;
		dbParam = resolveParams(param);

        LogVariableInfo var1;

        var1._VarName = param.getTempParamName();
        var1._expectedValue = "";
		if (dbParam && !param.getSourceFile().isEmpty())
		{
			var1._VarType = "DATABASE";
		}
		else
		{
			var1._VarType = "LOCAL";
		}
        log.insertVariables(var1);
    }
    if(_GenNewLogFormat)
        onExit.insertExtraInfo(log.getLogElem());

    printParamState.setOnExit(onExit);
    return printParamState;
}
