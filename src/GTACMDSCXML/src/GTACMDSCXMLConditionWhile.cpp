#include "GTACMDSCXMLConditionWhile.h"
#include "GTASCXMLState.h"
#include "GTAUtil.h"
#include "GTACMDSCXMLLog.h"
#include "GTACMDSCXMLUtils.h"



GTACMDSCXMLConditionWhile::GTACMDSCXMLConditionWhile(GTAActionWhile *pCmdNode, const QString &iStateId, const QString &iTargetId):
        GTACMDSCXMLBase(pCmdNode,iStateId,iTargetId)
{
    _WhileCmd = pCmdNode;
    _IncludeLoopTimeoutInSubStates = false;
}

GTACMDSCXMLConditionWhile::GTACMDSCXMLConditionWhile()
{

}

GTACMDSCXMLConditionWhile::~GTACMDSCXMLConditionWhile()
{

}
QString GTACMDSCXMLConditionWhile::getSCXMLString() const
{
    return getSCXMLState().getSCXMLString(_isUSCXMLEn);
}

GTASCXMLState GTACMDSCXMLConditionWhile::getSCXMLState()const
{
    // basestate which will consist compounded states (getParam, decision, content)
    GTASCXMLState WhileState;
    if(_WhileCmd != NULL)
    {
        WhileState.setId(_StateId);

        //action on fail enabled/disabled
	QString actOnFail = _WhileCmd->getActionOnFail();
	QString finalStateId = QString("%1_final").arg(_StateId);

        //timeout enabled/disabled
        double dTimeoutVal = _WhileCmd->getTimeOutInMs();
        QString timeoutEventName = QString("%1_timeoutEvent").arg(_StateId);

        //timeout transition
        if(dTimeoutVal >0)
        {
            GTASCXMLOnEntry onEntry;
            GTASCXMLSend sendTimeOut;
            sendTimeOut.setDelay(QString("%1ms").arg(QString::number(dTimeoutVal)));
            sendTimeOut.setEvent(timeoutEventName);
            onEntry.insertSend(sendTimeOut);
            //WhileState.setOnEntry(onEntry);
        }

        //get loop param states
        GTASCXMLState getParamState = getParameterState();
        WhileState.insertStates(getParamState);

        ////decision state
        GTASCXMLState decisionState = getDecisionState();
        WhileState.insertStates(decisionState);


        //inserting states within while according to relevant action/check
        for(int i=0;i<_Children.count();i++)
        {
            GTACMDSCXMLBase* pCmd = _Children.at(i);
            if(pCmd != NULL)
            {
                GTASCXMLState tempState = pCmd->getSCXMLState();
                //appending the states inside the loop
                WhileState.insertStates(tempState);
            }
        }

        //timeout event
        GTASCXMLTransition timeoutTx;
        timeoutTx.setEvent(timeoutEventName);
        timeoutTx.setStatus("TIMEOUT");
        if(actOnFail == "stop")
            timeoutTx.setTarget(SCXML_FINAL_STATE_ID);
        else
            timeoutTx.setTarget(_TargetId);

        if(dTimeoutVal >0)
        {
            GTASCXMLLog timeoutLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::TIMEOUT_LOG,_WhileCmd->getInstanceName(),"Timeout","Timeout");

            timeoutTx.insertLog(timeoutLog);
            //WhileState.insertTransitions(timeoutTx,_IncludeLoopTimeoutInSubStates);
        }


        //exit transition to next state
        GTASCXMLTransition tx;
        tx.setStatus("OK");
        tx.setTarget(_TargetId);

        WhileState.insertTransitions(tx);


        //innsert logs
        GTACMDSCXMLLog log;
        log.setReportType(GTACMDSCXMLLog::REP_ACTION,GTACMDSCXMLLog::REP_ACTCHK);
        log.setCommandName(_CmdName);
        log.setCommandInstanceId(_WhileCmd->getInstanceName());
        log.setCommandDescription(_WhileCmd->getComment());
        log.setCommandStatement(_WhileCmd->getStatement());
        QStringList requirements = _WhileCmd->getRequirements();
        log.insertRequirements(requirements);
        QStringList varList;// = WhileState.getVariableList();
        GTACMDSCXMLUtils::insertVariablesInLogReport(_ParamInfoList,varList,log);

        GTASCXMLOnExit onExit = WhileState.getOnExit();
        GTASCXMLLog log1 = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG, _WhileCmd->getInstanceName(), "true", "true");
        QString tempLog = log1.getExpression();
        tempLog.replace("#while", "#END_while row");
        GTASCXMLLog endlog; 
        endlog.setExpression(tempLog);
        onExit.insertLog(endlog);

        if(_GenNewLogFormat)
            onExit.insertExtraInfo(log.getLogElem());
        WhileState.setOnExit(onExit);
        GTACMDSCXMLUtils::logAllParameters(_WhileCmd->getInstanceName(),_ParamInfoList,WhileState);
    }

    return WhileState;
}


GTASCXMLState GTACMDSCXMLConditionWhile::getParameterState()const
{
    QString getParamStateId = QString("%1_GetParametrs").arg(_StateId);
    QString decisionStateId = QString("%1_Decision").arg(_StateId);
    QString parentFinalId = QString("%1_final").arg(_StateId);

    GTASCXMLState getParamState;

    QString actOnFail = _WhileCmd->getActionOnFail();
    bool isActOnFail = false;
    if(actOnFail == "stop")
        isActOnFail = true;
    getParamState = GTACMDSCXMLUtils::createGetParamInvokeState(_ParamInfoList,getParamStateId,decisionStateId,QString("%1_timeoutEvent").arg(_StateId),isActOnFail,_WhileCmd->getInstanceName(),false,_HiddenDelay,parentFinalId);

    //timeout enabled/disabled
    double dTimeoutVal = _WhileCmd->getTimeOutInMs();
    QString timeoutEventName = QString("%1_timeoutEvent").arg(_StateId);
    QString timeoutEventID = QString("%1_timeoutID").arg(_StateId);

    //timeout event
    GTASCXMLTransition timeoutTx;
    timeoutTx.setEvent(timeoutEventName);
    timeoutTx.setStatus("TIMEOUT");
    if(actOnFail == "stop")
        timeoutTx.setTarget(SCXML_FINAL_STATE_ID);
    else
        timeoutTx.setTarget(parentFinalId);

    //timeout transition only for the getparameter state
    //the event is cancelled on transition to a new state
    if(dTimeoutVal >0)
    {
        GTASCXMLOnEntry onEntry;
        GTASCXMLSend sendTimeOut;
        sendTimeOut.setId(timeoutEventID);
        sendTimeOut.setDelay(QString("%1ms").arg(QString::number(dTimeoutVal)));
        sendTimeOut.setEvent(timeoutEventName);
        onEntry.insertSend(sendTimeOut);
        getParamState.setOnEntry(onEntry);

        GTASCXMLOnExit onExit;
        GTASCXMLCancel cancelTimeoutEvent;
        cancelTimeoutEvent.setSendEventId(timeoutEventID);
        onExit.insertCancel(cancelTimeoutEvent);
        getParamState.setOnExit(onExit);
    }

    return getParamState;
}

GTASCXMLState GTACMDSCXMLConditionWhile::getDecisionState()const
{

    double dTimeoutVal = _WhileCmd->getTimeOutInMs();
    QString actOnFail = _WhileCmd->getActionOnFail();

    QString finalStateId = QString("%1_final").arg(_StateId);
    QString timeoutEventName = QString("%1_timeoutEvent").arg(_StateId);
    QString decisionStateId = QString("%1_Decision").arg(_StateId);
    QString whileStatesId = QString("%1_WhileStates").arg(_StateId);

    GTASCXMLState decisionState;
    decisionState.setId(decisionStateId);


    GTASCXMLOnEntry onEntry;

    QString dummyEventName = QString("%1_dummy_event").arg(decisionStateId);
    GTASCXMLSend dummyEventSend;
    dummyEventSend.setDelay("50ms");
    dummyEventSend.setEvent(dummyEventName);
    onEntry.insertSend(dummyEventSend);

    QStringList icdParams;
    QStringList localParams;
    for(int i=0;i<_ParamInfoList.count();i++)
    {
        GTAICDParameter param = _ParamInfoList.at(i);
        if(param.getToolName() != "NA")
        {
			if (!param.getTempParamName().isEmpty())
				icdParams.append(param.getTempParamName());
			else
				icdParams.append(param.getName());
        }
                else if (param.getToolType() == INTERNAL_PARAM)
        {
            QString loopid = QString("%1%2").arg(param.getTempParamName(),param.getUUID());
            localParams.append(loopid);
        }
        else
        {
            localParams.append(param.getTempParamName());
        }
    }

    QString condStmt;
    QString engSet = getEngSettings();

	bool isCondWhileOk = _WhileCmd->getSCXMLConditionStatement(icdParams,condStmt,engSet,false,localParams);

    GTASCXMLUtils::validateExpression(condStmt);

    GTASCXMLTransition trueTx;
    GTASCXMLTransition falseTx;
    GTASCXMLTransition timeout;

    if(dTimeoutVal >0 && (localParams.count()>0) )
    {
        timeout.setEvent(timeoutEventName);
        timeout.setStatus("TIMEOUT");

        if(actOnFail == "stop")
            timeout.setTarget(SCXML_FINAL_STATE_ID);
        else
            timeout.setTarget(finalStateId);

        GTASCXMLLog timeoutLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::TIMEOUT_LOG,_WhileCmd->getInstanceName(),"Timeout","Timeout");
        timeout.insertLog(timeoutLog);
        decisionState.insertTransitions(timeout);
    }

    QString CondParam = QString("%1_CondVar").arg(_StateId);

    if(isCondWhileOk)
    {
        GTASCXMLData condData;
        condData.setDataExpr(condStmt);
        condData.setDataId(CondParam);

        GTASCXMLDataModel datamodel;
        datamodel.insertDataItem(condData);
        decisionState.setDataModel(datamodel);

        trueTx.setCondition(QString("%1 == true").arg(CondParam));
        falseTx.setCondition(QString("%1 == false").arg(CondParam));

        trueTx.setTarget(whileStatesId);
        falseTx.setTarget(finalStateId);

		GTASCXMLLog successLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG, _WhileCmd->getInstanceName(), CondParam, "true");
		GTASCXMLLog failLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::FAIL_LOG, _WhileCmd->getInstanceName(), CondParam, "false");

		QString expr = successLog.getExpression();
		QString expr1 = failLog.getExpression();
		expr.replace(CondParam, QString("'+%1+'").arg(CondParam));
		expr1.replace(CondParam, QString("'+%1+'").arg(CondParam));
		successLog.setExpression(expr);
		failLog.setExpression(expr1);

		/*QString exprCondition = QString("%1 %2 %3").arg(_WhileCmd->getParameter(), _WhileCmd->getCondition(), _WhileCmd->getValue());
		GTASCXMLLog successLog;
		GTASCXMLLog failLog;

		// JIRA-133
		// We have to take in account CIC/NCIC Parameter to fill currentValue and expectedValue
		if (_WhileCmd->getParameter().contains(GTA_CIC_IDENTIFIER))
		{
			successLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG, _WhileCmd->getInstanceName(), CondParam, QString("'+%1+'").arg(icdParams[0]));
			failLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG, _WhileCmd->getInstanceName(), CondParam, QString("'+%1+'").arg(icdParams[0]));
		}
		else
		{
			successLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG, _WhileCmd->getInstanceName(), CondParam, CondParam);
			failLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG, _WhileCmd->getInstanceName(), CondParam, CondParam);
		}*/

        trueTx.insertLog(successLog);

        if(_RepeatLogs)
            falseTx.insertLog(failLog);


        trueTx.setEvent(dummyEventName);
        falseTx.setEvent(dummyEventName);

        decisionState.insertTransitions(trueTx);
        decisionState.insertTransitions(falseTx);
    }
    else
    {
        GTASCXMLLog successLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG,_WhileCmd->getInstanceName(),CondParam,"true");
        trueTx.insertLog(successLog);

        trueTx.setTarget(finalStateId);
        trueTx.setEvent(dummyEventName);
        decisionState.insertTransitions(trueTx);
    }

    decisionState.setOnEntry(onEntry);

    return decisionState;
}


