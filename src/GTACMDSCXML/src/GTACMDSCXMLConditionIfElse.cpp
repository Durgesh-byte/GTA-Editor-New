#include "GTACMDSCXMLConditionIfElse.h"
#include "GTASCXMLState.h"
#include "GTAUtil.h"
#include "GTACMDSCXMLLog.h"
#include "GTACMDSCXMLUtils.h"



GTACMDSCXMLConditionIfElse::GTACMDSCXMLConditionIfElse(GTAActionIF *pCmdNode, const QString &iStateId, const QString &iTargetId):
        GTACMDSCXMLBase(pCmdNode,iStateId,iTargetId)
{
    _IfCmd = pCmdNode;
	
}

GTACMDSCXMLConditionIfElse::GTACMDSCXMLConditionIfElse()
{

}

GTACMDSCXMLConditionIfElse::~GTACMDSCXMLConditionIfElse()
{

}
QString GTACMDSCXMLConditionIfElse::getSCXMLString() const
{
    return getSCXMLState().getSCXMLString(_isUSCXMLEn);
}

GTASCXMLState GTACMDSCXMLConditionIfElse::getSCXMLState()const
{

    GTASCXMLState IfElseState;
    if(_IfCmd != NULL)
    {
        IfElseState.setId(_StateId);

        double dTimeoutVal = _IfCmd->getTimeOutInMs();
        QString timeoutEventName = QString("%1_timeoutEvent").arg(_StateId);

        if(dTimeoutVal >0)
        {
            GTASCXMLOnEntry onEntry;
            GTASCXMLSend sendTimeOut;
            sendTimeOut.setDelay(QString("%1ms").arg(QString::number(dTimeoutVal)));
            sendTimeOut.setEvent(timeoutEventName);
            onEntry.insertSend(sendTimeOut);
            IfElseState.setOnEntry(onEntry);
        }
		

		GTASCXMLState getParamState = getParameterState();
		IfElseState.insertStates(getParamState);
        GTASCXMLState decisionState = getDecisionState();
        IfElseState.insertStates(decisionState);

        for(int i=0;i<_Children.count();i++)
        {
            GTACMDSCXMLBase* pCmd = _Children.at(i);
            if(pCmd != NULL)
                IfElseState.insertStates(pCmd->getSCXMLState());
        }

        GTASCXMLTransition tx;
        tx.setStatus("OK");
        tx.setTarget(_TargetId);

        IfElseState.insertTransitions(tx);


        //innsert logs
        GTACMDSCXMLLog log;
        log.setReportType(GTACMDSCXMLLog::REP_ACTION,GTACMDSCXMLLog::REP_ACTCHK);
        log.setCommandName(_CmdName);
        log.setCommandInstanceId(_IfCmd->getInstanceName());
        log.setCommandDescription(_IfCmd->getComment());
        log.setCommandStatement(_IfCmd->getStatement());
        QStringList requirements = _IfCmd->getRequirements();
        log.insertRequirements(requirements);
        QStringList varList;// = IfElseState.getVariableList();
		QStringList stateVarList = IfElseState.getVariableList();
		stateVarList.removeDuplicates();
		QList<GTAICDParameter> filteredParamList;
		if (_ParamInfoList.size() > 1) {
			for (const auto& param : _ParamInfoList) {
				for (const auto& varName : stateVarList) {
					if (param.getSignalName().contains(varName))
						filteredParamList.append(param);
				}
			}
			GTACMDSCXMLUtils::insertVariablesInLogReport(filteredParamList, varList, log);
		}
		else {
			GTACMDSCXMLUtils::insertVariablesInLogReport(_ParamInfoList, varList, log);
		}
        

        GTASCXMLOnExit onExit = IfElseState.getOnExit();
        if(_GenNewLogFormat)
        onExit.insertExtraInfo(log.getLogElem());
        IfElseState.setOnExit(onExit);
		if (_ParamInfoList.size() > 1) 
			GTACMDSCXMLUtils::logAllParameters(_IfCmd->getInstanceName(), filteredParamList, IfElseState);
		else
			GTACMDSCXMLUtils::logAllParameters(_IfCmd->getInstanceName(),_ParamInfoList,IfElseState);
    }

    return IfElseState;
}

GTASCXMLState GTACMDSCXMLConditionIfElse::getParameterState()const
{
    QString timeoutEventName = QString("%1_timeoutEvent").arg(_StateId);
    QString getParamStateId = QString("%1_GetParametrs").arg(_StateId);
    QString decisionStateId = QString("%1_Decision").arg(_StateId);
    QString finalState = QString("%1_final").arg(_StateId);
    bool isActOnFail = (_IfCmd->getActionOnFail() == "stop")?true:false;

    GTASCXMLState getParamState;

	GTAActionForEach *pForEach = nullptr;
	if (_IfCmd->getParameter().startsWith("line") || _IfCmd->getValue().startsWith("line")) {

		int cmdType = _IfCmd->getParent()->getCommandType();
		if (cmdType == 0) {
			GTAActionBase *pActCmd = dynamic_cast<GTAActionBase *> (_IfCmd->getParent());
			QString action = pActCmd->getAction();
			QString complement = pActCmd->getComplement();
			if (action == ACT_CONDITION && complement == COM_CONDITION_FOR_EACH) {
				pForEach = dynamic_cast<GTAActionForEach*> (_IfCmd->getParent());
			}
			else {
				while (pForEach == nullptr)
				{
					pActCmd = dynamic_cast<GTAActionBase *> (pActCmd->getParent());
					if (pActCmd == nullptr) {
						break;
					}
					QString parentAction = pActCmd->getAction();
					QString parentComplement = pActCmd->getComplement();
					if (parentAction == ACT_CONDITION && parentComplement == COM_CONDITION_FOR_EACH)
					{
						pForEach = dynamic_cast<GTAActionForEach*> (pActCmd);
					}
				}
			}
        }
		QStringList paramForEach;
		if (_IfCmd->getParameter().startsWith("line") && pForEach != nullptr)
		{
			QString columnName = _IfCmd->getParameter().split("__").last();
			paramForEach.append(pForEach->getCSVColumnByTitle(columnName));
		}
		else
			paramForEach.append(_IfCmd->getParameter());

		if (_IfCmd->getValue().startsWith("line") && pForEach != nullptr)
		{
			QString columnName = _IfCmd->getParameter().split("__").last();
            paramForEach.append(pForEach->getCSVColumnByTitle(columnName));
		}
		else
			paramForEach.append(_IfCmd->getValue());

		QList<GTAICDParameter> filteredParams;
		for (int i = 0; i < _ParamInfoList.size(); i++) {
			if (paramForEach.contains(_ParamInfoList.at(i).getName())) {
				filteredParams.append(_ParamInfoList.at(i));
			}
		}
		getParamState = GTACMDSCXMLUtils::createGetParamInvokeState(filteredParams, getParamStateId, decisionStateId, "", isActOnFail, _IfCmd->getInstanceName(), false, _HiddenDelay);

	}
	else
		getParamState = GTACMDSCXMLUtils::createGetParamInvokeState(_ParamInfoList,getParamStateId,decisionStateId,"",isActOnFail,_IfCmd->getInstanceName(),false,_HiddenDelay);

    GTASCXMLLog timeoutLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::TIMEOUT_LOG,_IfCmd->getInstanceName(),"","",false);


    GTASCXMLTransition timeoutTx;
    timeoutTx.setEvent(timeoutEventName);
    timeoutTx.setStatus(TX_STATUS_TIMEOUT);
    timeoutTx.setTarget(finalState);
    timeoutTx.insertLog(timeoutLog);
    getParamState.insertTransitions(timeoutTx,true);

    return getParamState;
}

GTASCXMLState GTACMDSCXMLConditionIfElse::getDecisionState()const
{

    double dTimeoutVal = _IfCmd->getTimeOutInMs();
    QString actOnFail = _IfCmd->getActionOnFail();

    QString finalStateId = QString("%1_final").arg(_StateId);
    QString timeoutEventName = QString("%1_timeoutEvent").arg(_StateId);
    QString decisionStateId = QString("%1_Decision").arg(_StateId);
    QString ifStateId = QString("%1_If").arg(_StateId);
    QString elseStateId = QString("%1_Else").arg(_StateId);

    GTASCXMLState decisionState;
    decisionState.setId(decisionStateId);

    QStringList icdParams;
    QStringList localParams;
    for(int i=0;i<_ParamInfoList.count();i++)
    {
        GTAICDParameter param = _ParamInfoList.at(i);
        if(param.getToolName() != "NA")
        {
            if(!param.getTempParamName().isEmpty())
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

	bool isCondIfElseOk = _IfCmd->getSCXMLConditionStatement(icdParams,condStmt,engSet,false,localParams);

    GTASCXMLUtils::validateExpression(condStmt);

    GTASCXMLTransition trueTx;
    GTASCXMLTransition falseTx;

    QString CondParam = QString("%1_CondVar").arg(_StateId);

    if(isCondIfElseOk)
    {
        GTASCXMLData condData;
        condData.setDataExpr(condStmt);
        condData.setDataId(CondParam);

        GTASCXMLData condIfReturnData;
        condIfReturnData.setDataExpr("{ 'Status' : 'KO' }");
        condIfReturnData.setDataId(_StateId + "_if_Return");

        GTASCXMLDataModel datamodel;
        datamodel.insertDataItem(condData);
        datamodel.insertDataItem(condIfReturnData);
        decisionState.setDataModel(datamodel);

        trueTx.setCondition(QString("%1 == true").arg(CondParam));
        falseTx.setCondition(QString("%1 == false").arg(CondParam));

        GTASCXMLAssign assign;
        assign.setLocation(_StateId + "_if_Return");
        assign.setExpression( "{ 'Status' : 'OK' }");

        trueTx.insertAssign(assign);
        trueTx.setTarget(ifStateId);
        falseTx.setTarget(elseStateId);

		GTASCXMLLog successLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG, _IfCmd->getInstanceName(), CondParam, "true");
		GTASCXMLLog failLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG, _IfCmd->getInstanceName(), CondParam, "false");

		QString expr = successLog.getExpression();
		QString expr1 = failLog.getExpression();
		expr.replace(CondParam, QString("'+%1+'").arg(CondParam));
		expr1.replace(CondParam, QString("'+%1+'").arg(CondParam));
		successLog.setExpression(expr);
		failLog.setExpression(expr1);

		/*QString exprCondition = QString("%1 %2 %3").arg(_IfCmd->getParameter(), _IfCmd->getCondition(), _IfCmd->getValue());
		GTASCXMLLog successLog;
		GTASCXMLLog failLog;
		
		// JIRA-133
		// We have to take in account CIC/NCIC Parameter to fill currentValue and expectedValue
		if (_IfCmd->getParameter().contains(GTA_CIC_IDENTIFIER))
		{
			successLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG, _IfCmd->getInstanceName(), CondParam, QString("'+%1+'").arg(icdParams[0]));
			failLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG, _IfCmd->getInstanceName(), CondParam, QString("'+%1+'").arg(icdParams[0]));
		}
		else
		{
			successLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG, _IfCmd->getInstanceName(), CondParam, CondParam);
			failLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG, _IfCmd->getInstanceName(), CondParam, CondParam);
		}*/

        trueTx.insertLog(successLog);
        falseTx.insertLog(failLog);

        decisionState.insertTransitions(trueTx);
        decisionState.insertTransitions(falseTx);
    }
    else
    {
        GTASCXMLLog successLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG,_IfCmd->getInstanceName(),CondParam,"true");

        trueTx.insertLog(successLog);

        trueTx.setTarget(finalStateId);
        decisionState.insertTransitions(trueTx);
    }
    GTASCXMLTransition timeoutTx;
    timeoutTx.setEvent(timeoutEventName);
    timeoutTx.setStatus("TIMEOUT");
    if(actOnFail == "stop")
        timeoutTx.setTarget(SCXML_FINAL_STATE_ID);
    else
        timeoutTx.setTarget(finalStateId);

    if(dTimeoutVal >0)
    {
        GTASCXMLLog timeoutLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::TIMEOUT_LOG,_IfCmd->getInstanceName(),"Timeout","Timeout");
        trueTx.insertLog(timeoutLog);
        decisionState.insertTransitions(timeoutTx);
    }

    return decisionState;
}

