#include "GTACMDSCXMLConditionDoWhile.h"
#include "GTASCXMLState.h"
#include "GTAUtil.h"
#include "GTACMDSCXMLLog.h"
#include "GTACMDSCXMLUtils.h"



GTACMDSCXMLConditionDoWhile::GTACMDSCXMLConditionDoWhile(GTAActionDoWhile *pCmdNode, const QString &iStateId, const QString &iTargetId):
        GTACMDSCXMLBase(pCmdNode,iStateId,iTargetId)
{
    _DoWhileCmd = pCmdNode;
    _IncludeLoopTimeoutInSubStates = false;
}

GTACMDSCXMLConditionDoWhile::GTACMDSCXMLConditionDoWhile()
{

}

GTACMDSCXMLConditionDoWhile::~GTACMDSCXMLConditionDoWhile()
{

}
QString GTACMDSCXMLConditionDoWhile::getSCXMLString() const
{
    return getSCXMLState().getSCXMLString(_isUSCXMLEn);
}

GTASCXMLState GTACMDSCXMLConditionDoWhile::getSCXMLState()const
{
    GTASCXMLState WhileState;

    if(_DoWhileCmd != NULL)
    {
        WhileState.setId(_StateId);

        QString actOnFail = _DoWhileCmd->getActionOnFail();
        QString finalStateId = QString("%1_final").arg(_StateId);

        double dTimeoutVal = _DoWhileCmd->getTimeOutInMs();
        QString timeoutEventName = QString("%1_timeoutEvent").arg(_StateId);

        if(dTimeoutVal >0)
        {
            GTASCXMLOnEntry onEntry;
            GTASCXMLSend sendTimeOut;
            sendTimeOut.setDelay(QString("%1ms").arg(QString::number(dTimeoutVal)));
            sendTimeOut.setEvent(timeoutEventName);
            onEntry.insertSend(sendTimeOut);
            //WhileState.setOnEntry(onEntry);
        }

        GTASCXMLState getParamState = getParameterState();


        GTASCXMLState decisionState = getDecisionState();



        for(int i=0;i<_Children.count();i++)
        {
            GTACMDSCXMLBase* pCmd = _Children.at(i);
            if(pCmd != NULL)
                WhileState.insertStates(pCmd->getSCXMLState());
        }

        GTASCXMLTransition timeoutTx;
        timeoutTx.setEvent(timeoutEventName);
        timeoutTx.setStatus("TIMEOUT");
        if(actOnFail == "stop")
            timeoutTx.setTarget(SCXML_FINAL_STATE_ID);
        else
            timeoutTx.setTarget(_TargetId);

        if(dTimeoutVal >0)
        {
            GTASCXMLLog timeoutLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::TIMEOUT_LOG,_DoWhileCmd->getInstanceName(),"Timeout","Timeout");
            timeoutTx.insertLog(timeoutLog);
            //WhileState.insertTransitions(timeoutTx,_IncludeLoopTimeoutInSubStates);
        }

        WhileState.insertStates(getParamState);
        WhileState.insertStates(decisionState);


        GTASCXMLTransition tx;
        tx.setStatus("OK");
        tx.setTarget(_TargetId);

        WhileState.insertTransitions(tx);

        //insert logs
        GTACMDSCXMLLog log;
        log.setReportType(GTACMDSCXMLLog::REP_ACTION,GTACMDSCXMLLog::REP_ACTCHK);
        log.setCommandName(_CmdName);
        log.setCommandInstanceId(_DoWhileCmd->getInstanceName());
        log.setCommandDescription(_DoWhileCmd->getComment());
        log.setCommandStatement(_DoWhileCmd->getStatement());
        QStringList requirements = _DoWhileCmd->getRequirements();
        log.insertRequirements(requirements);
        QStringList varList;// = WhileState.getVariableList();
        GTACMDSCXMLUtils::insertVariablesInLogReport(_ParamInfoList,varList,log);

        GTASCXMLOnExit onExit = WhileState.getOnExit();
        if(_GenNewLogFormat)
        onExit.insertExtraInfo(log.getLogElem());
        WhileState.setOnExit(onExit);
        GTACMDSCXMLUtils::logAllParameters(_DoWhileCmd->getInstanceName(),_ParamInfoList,WhileState);
    }

    return WhileState;
}


GTASCXMLState GTACMDSCXMLConditionDoWhile::getParameterState()const
{
    QString getParamStateId = QString("%1_GetParametrs").arg(_StateId);
    QString decisionStateId = QString("%1_Decision").arg(_StateId);
    QString parentFinalId = QString("%1_final").arg(_StateId);

    GTASCXMLState getParamState;
    QString actOnFail = _DoWhileCmd->getActionOnFail();
    bool isActOnFail = false;
    if(actOnFail == "stop")
        isActOnFail = true;
    getParamState = GTACMDSCXMLUtils::createGetParamInvokeState(_ParamInfoList,getParamStateId,decisionStateId,QString("%1_timeoutEvent").arg(_StateId),isActOnFail,_DoWhileCmd->getInstanceName(),false,_HiddenDelay,parentFinalId);

    //timeout enabled/disabled
    double dTimeoutVal = _DoWhileCmd->getTimeOutInMs();
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

GTASCXMLState GTACMDSCXMLConditionDoWhile::getDecisionState()const
{

    QString finalStateId = QString("%1_final").arg(_StateId);
    QString decisionStateId = QString("%1_Decision").arg(_StateId);
    QString whileStatesId = QString("%1_DoWhileStates").arg(_StateId);

    GTASCXMLState decisionState;
    decisionState.setId(decisionStateId);

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

    QHash<QString, QString> genToolMap = GTACMDSCXMLUtils::getGenToolRetData();

    QString engSet = getEngSettings();
	bool isCondWhileOk = _DoWhileCmd->getSCXMLConditionStatement(icdParams,condStmt,engSet,false,localParams);

    GTASCXMLUtils::validateExpression(condStmt);

    GTASCXMLTransition trueTx;
    GTASCXMLTransition falseTx;

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

		GTASCXMLLog successLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG, _DoWhileCmd->getInstanceName(), CondParam, "true");
		GTASCXMLLog failLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::FAIL_LOG, _DoWhileCmd->getInstanceName(), CondParam, "false");

		QString expr = successLog.getExpression();
		QString expr1 = failLog.getExpression();
		expr.replace(CondParam, QString("'+%1+'").arg(CondParam));
		expr1.replace(CondParam, QString("'+%1+'").arg(CondParam));
		successLog.setExpression(expr);
		failLog.setExpression(expr1);

		/*QString exprCondition = QString("%1 %2 %3").arg(_DoWhileCmd->getParameter(), _DoWhileCmd->getCondition(), _DoWhileCmd->getValue());
		GTASCXMLLog successLog;
		GTASCXMLLog failLog;

		// JIRA-133
		// We have to take in account CIC/NCIC Parameter to fill currentValue and expectedValue
		if (_DoWhileCmd->getParameter().contains(GTA_CIC_IDENTIFIER))
		{
			successLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG, _DoWhileCmd->getInstanceName(), CondParam, QString("'+%1+'").arg(icdParams[0]));
			failLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG, _DoWhileCmd->getInstanceName(), CondParam, QString("'+%1+'").arg(icdParams[0]));
		}
		else
		{
			successLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG, _DoWhileCmd->getInstanceName(), CondParam, exprCondition);
			failLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG, _DoWhileCmd->getInstanceName(), CondParam, exprCondition);
		}*/
		
        trueTx.insertLog(successLog);

        if(_RepeatLogs)
            falseTx.insertLog(failLog);

        decisionState.insertTransitions(trueTx);
        decisionState.insertTransitions(falseTx);
    }
    else
    {
        GTASCXMLLog successLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG,_DoWhileCmd->getInstanceName(),CondParam,"true");
        trueTx.insertLog(successLog);

        trueTx.setTarget(finalStateId);
        decisionState.insertTransitions(trueTx);

    }

    return decisionState;
}


