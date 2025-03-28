#include "GTACMDSCXMLWaitUntil.h"
#include "GTASCXMLState.h"
#include "GTAUtil.h"
#include "GTACMDSCXMLLog.h"
#include "GTACMDSCXMLUtils.h"

GTACMDSCXMLWaitUntil::GTACMDSCXMLWaitUntil(GTAActionWait *pCmdNode, const QString &iStateId, const QString &iTargetId) :
        GTACMDSCXMLBase(pCmdNode,iStateId,iTargetId)
{
    _WaitUntilCmd = pCmdNode;
}

GTACMDSCXMLWaitUntil::GTACMDSCXMLWaitUntil()
{

}

GTACMDSCXMLWaitUntil::~GTACMDSCXMLWaitUntil()
{

}
QString GTACMDSCXMLWaitUntil::getSCXMLString() const
{
    return getSCXMLState().getSCXMLString(_isUSCXMLEn);
}

QString GTACMDSCXMLWaitUntil::getExecStateID() const
{
    bool rc = getDebugModeStatus();
    QString ID;
    if (rc)
        ID = QString("%1_exec").arg(_StateId);
    else
        ID = _StateId;

    return ID;
}

QString GTACMDSCXMLWaitUntil::getExecTargetID() const
{
    bool rc = getDebugModeStatus();
    QString ID;
    if (rc)
        ID = QString("%1_debug").arg(_StateId);
    else
        ID = _TargetId;

    return ID;
}

QString GTACMDSCXMLWaitUntil::getFinalTargetID() const
{
    bool rc = getDebugModeStatus();
    QString ID;
    if (rc)
        ID = QString("%1_final").arg(_StateId);
    else
        ID = "";

    return ID;
}

QString GTACMDSCXMLWaitUntil::getPreDebugStateID() const
{
    bool rc = getDebugModeStatus();
    QString ID;
    if (rc)
        ID = QString("%1_preDebug").arg(_StateId);
    else
        ID = _TargetId;

    return ID;
}

QString GTACMDSCXMLWaitUntil::getPreDebugVariable() const
{
    bool rc = getDebugModeStatus();
    QString ID;
    if (rc)
        ID = QString("%1_preDebugVar").arg(getPreDebugStateID());
    else
        ID = "";

    return ID;
}

GTASCXMLState GTACMDSCXMLWaitUntil::getSCXMLState()const
{
    GTASCXMLState state;
    if(_WaitUntilCmd != NULL)
    {
        state = getSCXMLWaitUntil();

        //insert logs
        GTACMDSCXMLLog log;
        log.setReportType(GTACMDSCXMLLog::REP_ACTION,GTACMDSCXMLLog::REP_ACTCHK);
        log.setCommandName(_CmdName);
        log.setCommandInstanceId(_WaitUntilCmd->getInstanceName());
        log.setCommandDescription(_WaitUntilCmd->getComment());
        log.setCommandStatement(_WaitUntilCmd->getStatement());
        QStringList requirements = _WaitUntilCmd->getRequirements();
        log.insertRequirements(requirements);
        QStringList varList;// = state.getVariableList();
        GTACMDSCXMLUtils::insertVariablesInLogReport(_ParamInfoList,varList,log);

        GTASCXMLOnExit onExit = state.getOnExit();
        if(_GenNewLogFormat)
            onExit.insertExtraInfo(log.getLogElem());
        state.setOnExit(onExit);
        GTACMDSCXMLUtils::logAllParameters(_WaitUntilCmd->getInstanceName(),_ParamInfoList,state);
    }
    return state;
}

GTASCXMLState GTACMDSCXMLWaitUntil::getSCXMLWaitUntil() const
{
    QString confTime,confTimeUnit;
    double dConfTime;
    _WaitUntilCmd->getConfCheckTime(confTime,confTimeUnit);
    bool isDoubleOk =false;
    dConfTime = confTime.toDouble(&isDoubleOk);
    if(confTimeUnit == ACT_CONF_TIME_UNIT_SEC)
    {
        dConfTime = dConfTime *1000;
    }
    confTime = QString::number(dConfTime);

    bool isWaitUntilEn = true;

    double dTimeoutInMs = _WaitUntilCmd->getTimeOutInMs();



    GTASCXMLState checkValueState;

    bool rc = getDebugModeStatus();
    GTASCXMLState megaState;
//    QString executableStateID,executableTargetID,finaltargetId,preDebugVar;
//    QString preDebugStateId = QString("%1_predebug").arg(_StateId);
//    QString debugStateId = QString("%1_debug").arg(_StateId);
//    if (rc)
//    {
//        executableStateID = QString("%1_exec").arg(_StateId);
//        executableTargetID = QString("%1_debug").arg(_StateId);
//        preDebugVar = QString("%1_preDebugVar").arg(preDebugStateId);
//        finaltargetId = QString("%1_final").arg(_StateId);
//    }
//    else
//    {
//        executableStateID = _StateId;
//        executableTargetID = _TargetId;
//        preDebugVar = "";
//        finaltargetId = "";
//    }

    if(dConfTime > 0)
    {
        QString chkValInitCondStateId = QString("%1_ChkValInitCond").arg(getExecStateID());
        QString chkValConfChkStateId = QString("%1_ChkValConfChk").arg(getExecStateID());
        QString chkValFinalStateId = QString("%1_final").arg(getExecStateID());

        GTASCXMLState chkValInitCondState;

        QString stateId = QString("%1_getParameters").arg(chkValInitCondStateId);
        QString evalStateId  = QString("%1_evalCond").arg(chkValInitCondStateId);
        QString ConfState1Final = QString("%1_final").arg(chkValInitCondStateId);

        GTASCXMLState paramState = getParamState(stateId,evalStateId);
        GTASCXMLState evalState = evalutateConditionAndTxState(evalStateId,ConfState1Final,stateId,false,paramState.getStates().isEmpty()? (paramState.getInvokes().isEmpty()? false : true) : paramState.getStates().first().getInvokes().isEmpty());

        chkValInitCondState.insertStates(paramState);
        chkValInitCondState.insertStates(evalState);

        GTASCXMLTransition tx;
        tx.setTarget(chkValConfChkStateId);
        chkValInitCondState.insertTransitions(tx);

        chkValInitCondState.setId(chkValInitCondStateId);

        QString conftimeoutEvent = QString("%1_confEvent").arg(getExecStateID());
        QString conftimeoutID = QString("%1_confID").arg(getExecStateID());

        //cancelling confirmation time on exit
        //start it again on entry in conf state
        //do not cancel on exit of init state
        //scheduler cannot handle event cancellation and start immediately
        //GTASCXMLCancel cancel;
        //GTASCXMLOnExit onExit = chkValInitCondState.getOnExit();
        //cancel.setSendEventId(conftimeoutID);
        //onExit.insertCancel(cancel);
        //chkValInitCondState.setOnExit(onExit);

        GTASCXMLTransition confSuccessTx;
        GTASCXMLTransition confFailTx;
        confSuccessTx.setTarget(chkValFinalStateId);
        confFailTx.setTarget(chkValFinalStateId);
        if (!evalState.getDataModel().getDataItems().isEmpty())
        {
            QString condVar = evalState.getDataModel().getDataItems().first().getId();
            confSuccessTx.setCondition(QString("%1 == true").arg(condVar));
            confFailTx.setCondition(QString("%1 == false").arg(condVar));
        }
        confSuccessTx.setEvent(conftimeoutEvent);
        confFailTx.setEvent(conftimeoutEvent);

        GTASCXMLLog successLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG,_WaitUntilCmd->getInstanceName(),"true","true");
        GTASCXMLLog failLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::FAIL_LOG,_WaitUntilCmd->getInstanceName(),"false","false");
        confSuccessTx.insertLog(successLog);
        confFailTx.insertLog(failLog);

        chkValInitCondState.insertTransitions(confSuccessTx);
        chkValInitCondState.insertTransitions(confFailTx);

        checkValueState.insertStates(chkValInitCondState);

        GTASCXMLState chkValConfChkState;

        //setting confirmation time on entry
        GTASCXMLOnEntry onEntry;
        GTASCXMLSend sendConfEvent;
        sendConfEvent.setEvent(conftimeoutEvent);
        sendConfEvent.setId(conftimeoutID);
        sendConfEvent.setDelay(QString("%1ms").arg(confTime));
        onEntry.insertSend(sendConfEvent);
        chkValConfChkState.setOnEntry(onEntry);

        stateId = QString("%1_getParameters").arg(chkValConfChkStateId);
        evalStateId  = QString("%1_evalCond").arg(chkValConfChkStateId);
        QString ConfState2Final = QString("%1_getParameters").arg(chkValInitCondStateId);

        paramState = getParamState(stateId,evalStateId);
        evalState = evalutateConditionAndTxState(evalStateId,ConfState2Final,stateId,true,paramState.getStates().isEmpty()? (paramState.getInvokes().isEmpty()? false : true) : paramState.getStates().first().getInvokes().isEmpty());

        chkValConfChkState.insertStates(paramState);
        chkValConfChkState.insertStates(evalState);

        if (!evalState.getDataModel().getDataItems().isEmpty())
        {
            QString condVar = evalState.getDataModel().getDataItems().first().getId();
            confSuccessTx.setCondition(QString("%1 == true").arg(condVar));
            confFailTx.setCondition(QString("%1 == false").arg(condVar));
        }

        chkValConfChkState.insertTransitions(confSuccessTx);
        chkValConfChkState.insertTransitions(confFailTx);

        GTASCXMLTransition tx2;
        tx2.setTarget(chkValFinalStateId);
        chkValConfChkState.insertTransitions(tx2);

        chkValConfChkState.setId(chkValConfChkStateId);

        checkValueState.insertStates(chkValConfChkState);
    }
    else
    {
        QString loopBackStateId = "";
        QString stateId = QString("%1_getParameters").arg(getExecStateID());
        QString evalStateId  = QString("%1_evalCond").arg(getExecStateID());
        QString finalStateId = QString("%1_final").arg(getExecStateID());
        GTASCXMLState paramState = getParamState(stateId,evalStateId);

        if(isWaitUntilEn)
            loopBackStateId = stateId;
        GTASCXMLState evalState = evalutateConditionAndTxState(evalStateId,finalStateId,loopBackStateId,false,paramState.getStates().isEmpty()? (paramState.getInvokes().isEmpty()? false : true) : paramState.getStates().first().getInvokes().isEmpty());
        checkValueState.insertStates(paramState);
        checkValueState.insertStates(evalState);
    }

    QString timeoutEventName = QString("%1_timeoutEvent").arg(getExecStateID());
    QString timeoutEventID = QString("%1_timeoutEventID").arg(getExecStateID());
    QString actOnFail = _WaitUntilCmd->getActionOnFail();
    QString finalStateId = QString("%1_final").arg(getExecStateID());
    if(dTimeoutInMs >0)
    {
        GTASCXMLOnEntry onEntry;
        GTASCXMLSend sendTimeoutEvent;
        sendTimeoutEvent.setEvent(timeoutEventName);
        sendTimeoutEvent.setId(timeoutEventID);
        sendTimeoutEvent.setDelay(QString("%1ms").arg(QString::number(dTimeoutInMs)));
        onEntry.insertSend(sendTimeoutEvent);
        checkValueState.setOnEntry(onEntry);

        //cancel the timeout event onExit
        GTASCXMLCancel cancel;
        cancel.setSendEventId(timeoutEventID);
        GTASCXMLOnExit onExit;
        onExit.insertCancel(cancel);
        checkValueState.setOnExit(onExit);

        GTASCXMLTransition timeOutTx;
        timeOutTx.setEvent(timeoutEventName);
        GTASCXMLLog timeoutLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::TIMEOUT_LOG,_WaitUntilCmd->getInstanceName(),"Timeout","Timeout");
        if(actOnFail == "stop")
            timeOutTx.setTarget(SCXML_FINAL_STATE_ID);
        else
        {
            timeOutTx.setTarget(getExecTargetID());
            if (!getPreDebugVariable().isEmpty())
            {

                GTASCXMLAssign assign;
                assign.setLocation(QString("%1_Return").arg(getExecStateID()));

                assign.setExpression("{ 'Status' : 'Timeout' }");
                timeOutTx.insertAssign(assign);

                timeOutTx.setCondition(QString("(%1 == 'true')").arg(getPreDebugVariable()));

                //additional tx
                GTASCXMLTransition tx1;
                tx1.setEvent(timeoutEventName);
                tx1.setTarget(getFinalTargetID());
                tx1.setCondition(QString("(%1 != 'true')").arg(getPreDebugVariable()));
                tx1.insertLog(timeoutLog);
                tx1.insertAssign(assign);
                checkValueState.insertTransitions(tx1);
            }
        }

        timeOutTx.insertLog(timeoutLog);
        checkValueState.insertTransitions(timeOutTx);
    }

    checkValueState.setId(getExecStateID());

    GTASCXMLTransition tx;
    tx.setTarget(getExecTargetID());
    if (!getPreDebugVariable().isEmpty())
    {
        tx.setCondition(QString("(%1 == 'true')").arg(getPreDebugVariable()));

        //additional tx
        GTASCXMLTransition tx1;
        tx1.setTarget(getFinalTargetID());
        tx1.setCondition(QString("(%1 != 'true')").arg(getPreDebugVariable()));
        checkValueState.insertTransitions(tx1);
    }
    checkValueState.insertTransitions(tx);

    if (rc)
    {
        megaState.setId(_StateId);

        GTASCXMLTransition subTx;
        subTx.setTarget(_TargetId);
        megaState.insertTransitions(subTx);

        GTASCXMLState preDebugState = GTACMDSCXMLUtils::getPreDebugState(getPreDebugStateID(),getExecStateID(),_PrevStateId,_WaitUntilCmd->getInstanceName());
        megaState.insertStates(preDebugState);
        megaState.insertStates(checkValueState);
        GTASCXMLState debugState = GTACMDSCXMLUtils::getDebugState(getExecTargetID(),getFinalTargetID(),checkValueState.getVariableList(),_ParamInfoList);
        megaState.insertStates(debugState);

        megaState.setInitId(getPreDebugStateID());
        return megaState;
    }
    else
    {
        return checkValueState;
    }
//    return checkValueState;
}

GTASCXMLState GTACMDSCXMLWaitUntil::getParamState(const QString &iStateId, const QString &iTargetId) const
{
    GTASCXMLState paramState;
    QString timeoutEventName = QString("%1_timeoutEvent").arg(_StateId);
    QString actOnFail = _WaitUntilCmd->getActionOnFail();
    bool isActOnFail = false;
    if(actOnFail == "stop")
        isActOnFail = true;

    QString parameter = _WaitUntilCmd->getParameter();
    QString value = _WaitUntilCmd->getValue();
    paramState = GTACMDSCXMLUtils::createGetParamInvokeState(_ParamInfoList,iStateId,iTargetId,"",isActOnFail,_WaitUntilCmd->getInstanceName(),false,_HiddenDelay);


    double dTimeout = _WaitUntilCmd->getTimeOutInMs();
    QString finalStateId = QString("%1_final").arg(_StateId);
    if(dTimeout>0)
    {
        GTASCXMLTransition timeOutTx;
        timeOutTx.setEvent(timeoutEventName);
        if(actOnFail == "stop")
            timeOutTx.setTarget(SCXML_FINAL_STATE_ID);
        else
            timeOutTx.setTarget(finalStateId);

        GTASCXMLLog timeoutLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::TIMEOUT_LOG,_WaitUntilCmd->getInstanceName(),"Timeout","Timeout");

        timeOutTx.insertLog(timeoutLog);

		//DEV: only the parent state needs a timeout transition with scheduler 1.2.8.1
        //paramState.insertTransitions(timeOutTx,true);
    }
    return paramState;
}
GTASCXMLState GTACMDSCXMLWaitUntil::evalutateConditionAndTxState(const QString &iStateId, const QString &iTargetId, const QString &iLoopBackStateId, bool isConfTimeEn, bool ParamStateHasInvokes) const
{
    GTASCXMLState evalState;

    QString actOnFail = _WaitUntilCmd->getActionOnFail();
    QHash<QString,QString> genToolData = GTACMDSCXMLUtils::getGenToolRetData();
    QStringList icdParamList;
    QStringList localParamList;
    for(int i=0;i<_ParamInfoList.count();i++)
    {
        GTAICDParameter param = _ParamInfoList.at(i);
        if(param.getToolName() != "NA")
        {
			if (!param.getTempParamName().isEmpty())
				icdParamList.append(param.getTempParamName());
			else
				icdParamList.append(param.getName());
        }
        else if (param.getToolType() == INTERNAL_PARAM)
        {
            QString loopid = QString("%1%2").arg(param.getTempParamName(),param.getUUID());
            localParamList.append(loopid);
        }
        else
        {
            localParamList.append(param.getTempParamName());
        }
    }

    QString timeoutEventName = QString("%1_timeoutEvent").arg(_StateId);
    QString engSet = getEngSettings();
    QString condStmt;
    QString CondParam = QString("%1_CondVar").arg(iStateId);

    bool rc = _WaitUntilCmd->getSCXMLConditionStatement(icdParamList,condStmt,engSet,false,localParamList);
    GTASCXMLUtils::validateExpression(condStmt);

    GTASCXMLTransition trueTx;
    GTASCXMLTransition falseTx;

    if(!iLoopBackStateId.trimmed().isEmpty())
    {
        if(isConfTimeEn)
        {
            falseTx.setTarget(iTargetId);
            trueTx.setTarget(iLoopBackStateId);

            QString conftimeoutID = QString("%1_confID").arg(_StateId);
            GTASCXMLCancel cancel;
            cancel.setSendEventId(conftimeoutID);
            falseTx.insertCancel(cancel);
        }
        else
        {
            //set target as getParam state only if there is a parameter to check
            falseTx.setTarget(ParamStateHasInvokes? iTargetId : iLoopBackStateId);
            trueTx.setTarget(iTargetId);
        }
    }
    else
    {
        trueTx.setTarget(iTargetId);
        falseTx.setTarget(iTargetId);
    }

    if(rc)
    {
        GTASCXMLData condData;
        condData.setDataExpr(condStmt);
        condData.setDataId(CondParam);

        GTASCXMLDataModel datamodel;
        datamodel.insertDataItem(condData);
        evalState.setDataModel(datamodel);

        if (!ParamStateHasInvokes /*&& isConfTimeEn*/)
        {
            GTASCXMLOnEntry onEntry;
            GTASCXMLSend send;
            QString sendDelayDEvent = QString("%1_delayEvent").arg(iStateId);
            QString sendDelayID = QString("%1_delayID").arg(iStateId);
            send.setDelay(_HiddenDelay);
            send.setEvent(sendDelayDEvent);
            send.setId(sendDelayID);
            onEntry.insertSend(send);

            trueTx.setEvent(sendDelayDEvent);
            falseTx.setEvent(sendDelayDEvent);
            evalState.setOnEntry(onEntry);
        }

        trueTx.setCondition(QString("%1 == true").arg(CondParam));
        falseTx.setCondition(QString("%1 == false").arg(CondParam));

        GTASCXMLLog successLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG,_WaitUntilCmd->getInstanceName(),CondParam,"true");
        GTASCXMLLog failLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::FAIL_LOG,_WaitUntilCmd->getInstanceName(),CondParam,"false");

        QString expr = successLog.getExpression();
        QString expr1 = failLog.getExpression();
        expr.replace(CondParam,QString("'+%1+'").arg(CondParam));
        expr1.replace(CondParam,QString("'+%1+'").arg(CondParam));
        successLog.setExpression(expr);
        failLog.setExpression(expr1);

        trueTx.insertLog(successLog);

        if(_RepeatLogs || ParamStateHasInvokes || (actOnFail == "stop"))
            falseTx.insertLog(failLog);

        if (!getPreDebugVariable().isEmpty())
        {
            GTASCXMLAssign assign;
            assign.setLocation(QString("%1_Return").arg(getExecStateID()));

            assign.setExpression("{ 'Status' : 'OK' }");
            trueTx.insertAssign(assign);
            assign.setExpression("{ 'Status' : 'KO' }");
            falseTx.insertAssign(assign);
        }

        evalState.insertTransitions(trueTx);
        evalState.insertTransitions(falseTx);
    }
    else
    {
        GTASCXMLLog successLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG,_WaitUntilCmd->getInstanceName(),CondParam,"true");

        QString expr = successLog.getExpression();
        expr.replace(CondParam,QString("'+%1+'").arg(CondParam));
        successLog.setExpression(expr);

        trueTx.insertLog(successLog);

        if (!getPreDebugVariable().isEmpty())
        {
            GTASCXMLAssign assign;
            assign.setLocation(QString("%1_Return").arg(getExecStateID()));

            assign.setExpression("{ 'Status' : 'OK' }");
            trueTx.insertAssign(assign);
        }

        evalState.insertTransitions(trueTx);
    }



    evalState.setId(iStateId);
    return evalState;
}
