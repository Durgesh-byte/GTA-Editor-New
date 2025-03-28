#include "GTACMDSCXMLWaitFor.h"
#include "GTASCXMLState.h"
#include "GTAUtil.h"
#include "GTACMDSCXMLLog.h"
#include "GTACMDSCXMLUtils.h"

GTACMDSCXMLWaitFor::GTACMDSCXMLWaitFor(GTAActionWait *pCmdNode, const QString &iStateId, const QString &iTargetId):
        GTACMDSCXMLBase(pCmdNode,iStateId,iTargetId)
{
    _WaitCmd = pCmdNode;
}

GTACMDSCXMLWaitFor::GTACMDSCXMLWaitFor()
{

}

GTACMDSCXMLWaitFor::~GTACMDSCXMLWaitFor()
{

}
QString GTACMDSCXMLWaitFor::getSCXMLString() const
{
    return getSCXMLState().getSCXMLString(_isUSCXMLEn);
}

GTASCXMLState GTACMDSCXMLWaitFor::getSCXMLState()const
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

    if(_WaitCmd != NULL)
    {
        if(_WaitCmd->getComplement() == COM_WAIT_FOR)
        {
            state = getSCXMLForWaitForCommand(executableStateID,executableTargetID,finaltargetId,preDebugVar);
        }
        else if(_WaitCmd->getComplement() == COM_WAIT_UNTIL)
        {
            state = getSCXMLForWaitForCommand(executableStateID,executableTargetID,finaltargetId,preDebugVar);
        }
        else
            return state;


        GTACMDSCXMLUtils::logAllParameters(_WaitCmd->getInstanceName(),_ParamInfoList,state);
        //innsert logs
        GTACMDSCXMLLog log;
        log.setReportType(GTACMDSCXMLLog::REP_ACTION,GTACMDSCXMLLog::REP_ACTCHK);
        log.setCommandName(_CmdName);
        log.setCommandInstanceId(_WaitCmd->getInstanceName());
        log.setCommandDescription(_WaitCmd->getComment());
        log.setCommandStatement(_WaitCmd->getStatement());
        QStringList requirements = _WaitCmd->getRequirements();
        log.insertRequirements(requirements);
        QStringList varList;// = state.getVariableList();
        GTACMDSCXMLUtils::insertVariablesInLogReport(_ParamInfoList,varList,log);

        GTASCXMLOnExit onExit = state.getOnExit();
        if(_GenNewLogFormat)
            onExit.insertExtraInfo(log.getLogElem());
        state.setOnExit(onExit);
    }

    if (rc)
    {
        megaState.setId(_StateId);

        GTASCXMLTransition tx;
        tx.setTarget(_TargetId);
        megaState.insertTransitions(tx);

        GTASCXMLState preDebugState = GTACMDSCXMLUtils::getPreDebugState(preDebugStateId,executableStateID,_PrevStateId,_WaitCmd->getInstanceName());
        megaState.insertStates(preDebugState);
        megaState.insertStates(state);
        GTASCXMLState debugState = GTACMDSCXMLUtils::getDebugState(debugStateId,finaltargetId,state.getVariableList(), _ParamInfoList);
        megaState.insertStates(debugState);

        megaState.setInitId(preDebugStateId);
        return megaState;
    }
    else
    {
        return state;
    }
}

GTASCXMLState GTACMDSCXMLWaitFor::getSCXMLForWaitForCommand(const QString iStateId,const QString iTargetId,
                                                                  const QString &iFinalId,
                                                                  const QString &iDecisionVar) const
{
	QString delay = QString();
	// Get the current instance
	QString expression = _WaitCmd->getInstanceName();
	QStringList expr_list;
	// Clean and truncate the expression
	expr_list = expression.split("wait for");
	expression = expr_list[1].simplified();
	expression.remove(-1, 1);
	// Check if we are in presence of a numeric or string value
	bool testDouble = false;
	bool local_var = false;
	expression.toDouble(&testDouble);
	/* Wait For 10 s
	*  Wait For @time@ s
	*  Wait For time s
	*/
	if (testDouble)
	{
		delay = expression;
	}
	else
	{
		if (expression.startsWith("@") && expression.endsWith("@"))
		{
			delay = expression;
		}
		else
		{
			// We need to make the difference betwwen a Wait For (local_var) and a Wait For using an argument
			local_var = true;
			delay = QString("%1%2%3").arg("+", expression, "*1000+"); 
		}
	}
    QString instanceName = _WaitCmd->getInstanceName();
    QString commandName = QString("%1_%2").arg(_WaitCmd->getAction(),_WaitCmd->getComplement());
	
    bool isNumber = false;
    double dSeconds = delay.toDouble(&isNumber);
    QString sTimeVal("0");
    if (isNumber == true)
    {
        dSeconds = dSeconds * 1000;
        sTimeVal = QString::number(dSeconds, 'f', 0);
        sTimeVal.append("ms");
    }
	else
	{
		sTimeVal = delay; 
		if (local_var)
		{
			sTimeVal.append("'ms'");
		}
		else
		{
			sTimeVal.append("ms");
		}
	}

    QString delayEventName = QString("%1_dealy_event").arg(iStateId);
	GTASCXMLSend send;
	send.setDelay(sTimeVal);
	send.setEvent(delayEventName);
	
    GTASCXMLState waitMessageState;
    waitMessageState.setId(iStateId);

    GTASCXMLOnExit onExit;

    GTASCXMLOnEntry onEntry;
    onEntry.insertSend(send);
    waitMessageState.setOnEntry(onEntry);
	
    GTACMDSCXMLLog log;
    log.setReportType(GTACMDSCXMLLog::REP_ACTION,(GTACMDSCXMLLog::REPORT_SUB_TYPE)0);
    log.setCommandName(commandName);
    log.setCommandInstanceId(instanceName);
    log.setCommandDescription(_WaitCmd->getComment());
    log.setCommandStatement(_WaitCmd->getStatement());

    LogVariableInfo var1;

    var1._VarName = QString("%1_Delay").arg(iStateId);
	if (isNumber)
	{
		var1._expectedValue = QString("'%1Seconds'").arg(delay);
	}
	else
	{
		// Argument
		if (delay.startsWith("@") && delay.endsWith("@"))
		{
			var1._expectedValue = QString("'%1Seconds'").arg(delay);
		}
		else // local_variable
		{
			QString newDelay = QString("%1%2%3").arg("+", expression, "+"); 
			var1._expectedValue = QString("%1 'Seconds'").arg(newDelay);
		}
	}
    var1._VarType = "INTERNAL";

    log.insertVariables(var1);
    if(_GenNewLogFormat)
        onExit.insertExtraInfo(log.getLogElem());

    waitMessageState.setOnExit(onExit);

    GTASCXMLData data;
    data.setDataExpr(var1._expectedValue);
    data.setDataId(var1._VarName);

    GTASCXMLDataModel datamodel;
    datamodel.insertDataItem(data);

    GTAScxmlLogMessage logMessage1;
    logMessage1.CurrentValue = _WaitCmd->getStatement();
    logMessage1.ExpectedValue = "Message";
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
        GTASCXMLData data1;
        data1.setDataExpr("");
        data1.setDataId(QString("%1_Result").arg(iStateId));
        datamodel.insertDataItem(data1);

        GTASCXMLAssign assign;
        assign.setExpression(QString("'Waited for %1 seconds'").arg(delay));
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
        tx1.setEvent(delayEventName);
        waitMessageState.insertTransitions(tx1);
    }
    waitMessageState.setDataModel(datamodel);
    tx.setStatus("OK");
    tx.insertLog(log2);
    tx.setEvent(delayEventName);
    waitMessageState.insertTransitions(tx);
    return waitMessageState;
}
