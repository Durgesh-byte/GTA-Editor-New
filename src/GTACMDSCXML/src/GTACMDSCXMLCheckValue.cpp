#include "GTACMDSCXMLCheckValue.h"
#include "GTASCXMLState.h"
#include "GTAUtil.h"
#include "GTACMDSCXMLLog.h"
#include "GTACMDSCXMLUtils.h"
#include "GTASCXMLCancel.h"



GTACMDSCXMLCheckValue::GTACMDSCXMLCheckValue(GTACheckValue *pCmdNode, const QString &iStateId, const QString &iTargetId) :
    GTACMDSCXMLBase(pCmdNode,iStateId,iTargetId)
{
    _CheckValueCmd = pCmdNode;
	if (_CheckValueCmd->getParameter(0).startsWith("line")) {
		_isParamInCSV = true;
	}
}

GTACMDSCXMLCheckValue::GTACMDSCXMLCheckValue()
{

}

GTACMDSCXMLCheckValue::~GTACMDSCXMLCheckValue()
{

}
QString GTACMDSCXMLCheckValue::getSCXMLString() const
{
    return getSCXMLState().getSCXMLString(_isUSCXMLEn);
}

QString GTACMDSCXMLCheckValue::getExecStateID() const
{
    bool rc = getDebugModeStatus();
    QString ID;
    if (rc)
        ID = QString("%1_exec").arg(_StateId);
    else
        ID = _StateId;

    return ID;
}

QString GTACMDSCXMLCheckValue::getExecTargetID() const
{
    bool rc = getDebugModeStatus();
    QString ID;
    if (rc)
        ID = QString("%1_debug").arg(_StateId);
    else
        ID = _TargetId;

    return ID;
}

QString GTACMDSCXMLCheckValue::getFinalTargetID() const
{
    bool rc = getDebugModeStatus();
    QString ID;
    if (rc)
        ID = QString("%1_final").arg(_StateId);
    else
        ID = "";

    return ID;
}

QString GTACMDSCXMLCheckValue::getPreDebugStateID() const
{
    bool rc = getDebugModeStatus();
    QString ID;
    if (rc)
        ID = QString("%1_preDebug").arg(_StateId);
    else
        ID = _TargetId;

    return ID;
}

QString GTACMDSCXMLCheckValue::getPreDebugVariable() const
{
    bool rc = getDebugModeStatus();
    QString ID;
    if (rc)
        ID = QString("%1_preDebugVar").arg(getPreDebugStateID());
    else
        ID = "";

    return ID;
}

QString GTACMDSCXMLCheckValue::getLineArray(const QStringList &templist)const
{
    QString forEachArray=NULL;
    if (templist.contains("ForEachStates")) {
        QString cntr;
        QString arr;
        int i = templist.size() - 1;
        while (i >= 0) {
            if (templist.at(i) == "ForEachStates") {
                for (int j = 0; j < i; j++) {
                    if (j != 0) {
                        cntr += "_" + templist.at(j);
                        arr += "_" + templist.at(j);
                    }
                    else {
                        cntr += templist.at(j);
                        arr += templist.at(j);
                    }

                }
                cntr += "_arr_cntr";
                arr += "_line_arr";
                i = -1;
            }
            i--;
        }
        forEachArray=arr + '[' + cntr + ']';
    }
    return forEachArray;
    
}


GTASCXMLState GTACMDSCXMLCheckValue::getSCXMLState()const
{
    GTASCXMLState state;

    bool rc = getDebugModeStatus();
    GTASCXMLState megaState;

    if(_CheckValueCmd != NULL)
    {
        state = getSCXMLCheckValue();
        //innsert logs
        GTACMDSCXMLLog log;
        log.setReportType(GTACMDSCXMLLog::REP_CHECK,GTACMDSCXMLLog::REP_ACTCHK);
        log.setCommandName(_CmdName);
        log.setCommandInstanceId(_CheckValueCmd->getInstanceName());
        log.setCommandDescription(_CheckValueCmd->getComment());
        log.setCommandStatement(_CheckValueCmd->getStatement());
        QStringList requirements = _CheckValueCmd->getRequirements();
        log.insertRequirements(requirements);
        QStringList varList= state.getVariableList();
		QList<GTAICDParameter> tempParamList;
        QStringList _lstparam = _CheckValueCmd->getListParameters();
        for (int j = 0; j < _lstparam.size(); j++)
        {
            QStringList nameparam = _lstparam.at(j).split("__");
            
            if (nameparam[0].startsWith("line"))
            {
                QString value = _CheckValueCmd->getValue(j);
                if (value.startsWith("line"))
                {
                    QStringList valueparam = value.split("__");
                    for (const auto& param_info : _ParamInfoList)
                    {
                        if (param_info.getToolType() != "NA" && (param_info.getTempParamName().contains(nameparam[1]) || param_info.getTempParamName().contains(valueparam[1])))
                            tempParamList.append(param_info);
                    }
                }
                else 
                {
                    for (const auto& param_info : _ParamInfoList)
                    {
                        if (param_info.getToolType() != "NA" && param_info.getTempParamName().contains(nameparam[1]))
                            tempParamList.append(param_info);
                    }
                }

                
            }
            
            else
            {
                for (const auto& param_info : _ParamInfoList)
                {
                    if (param_info.getToolType() != "NA")
                        tempParamList.append(param_info);
                }
            }
        }
		if (_isParamInCSV) {
			GTAICDParameter forEachLog = _ParamInfoList.at(0);
			if (_ParamInfoList.at(0).getSignalName().startsWith("line")) 
			{
				/*Add constructor for array and index to rebuild line.param*/
				QStringList templist = state.getId().split('_');
				QString cntr;
				QString arr;
				int i = templist.size() - 1;
				while (i >= 0) {
					if (templist.at(i) == "ForEachStates") {
						for (int j = 0; j < i; j++) {
							if (j != 0) {
								cntr += "_" + templist.at(j);
								arr += "_" + templist.at(j);
							}
							else {
								cntr += templist.at(j);
								arr += templist.at(j);
							}

						}
						cntr += "_arr_cntr";
						arr += "_line_arr";
						i = -1;
					}
					i--;
				}
				GTAICDParameter forEachParam = _ParamInfoList.at(0);
				QString name = arr + '[' + cntr + "]." + _ParamInfoList.at(0).getSignalName().split("__").takeLast();

				forEachParam.setSignalName(name);
				forEachParam.setTempParamName(name);
				tempParamList.replace(0, forEachParam);
			}
			forEachLog.setSignalName(state.getId() + "_getParameters_returnCheck");
			forEachLog.setTempParamName(state.getId() + "_getParameters_returnCheck");
			tempParamList.append(forEachLog);
			GTACMDSCXMLUtils::insertVariablesInLogReport(tempParamList, varList, log);
		}
		else 
		{
			GTACMDSCXMLUtils::insertVariablesInLogReport(_ParamInfoList, varList, log);
			/*QList<GTAICDParameter> filteredParamList;
			QStringList paramNamesList =  _CheckValueCmd->getVariableList();
			foreach(GTAICDParameter param, _ParamInfoList)
			{
				QString removeAfterTest = param.getName(); //If forgotten feel free to erase
				if (paramNamesList.contains(param.getName()) || paramNamesList.contains(param.getTempParamName()))
				{
					filteredParamList.append(param);
				}
				// We must check if we are in presence of CIC/NCIC Parameters
				/*QString iCheckParam = _CheckValueCmd->getParameter(0);
				if(!getEngSettings().isEmpty() && iCheckParam.contains(getEngSettings()))
				{
					QStringList iEngineName = tempParamList[0].getName().split(getEngSettings());
					QString iEngineNb = iEngineName[1].at(0);

					QHash<QString, QString> engineVarMap;
					GTAUtil::getChannelSelectionSignals(getEngSettings(), engineVarMap);
					if (!engineVarMap.isEmpty())
					{
						QString iTriplet = engineVarMap.value(iEngineNb);
						GTAICDParameter param;
						param.setTempParamName(iTriplet);
						// It is necessary to add setTempParamName before resolving the content of the ICD Parameter
						resolveParams(param);
						tempParamList.append(param);
					}
				}
			}*/
			//GTACMDSCXMLUtils::insertVariablesInLogReport(filteredParamList, varList, log); 
			//GTACMDSCXMLUtils::insertVariablesInLogReport(tempParamList, varList, log);
		}
		
        GTASCXMLOnExit onExit = state.getOnExit();
        if(_GenNewLogFormat)
            onExit.insertExtraInfo(log.getLogElem());
        state.setOnExit(onExit);
		if (_isParamInCSV) {
			GTACMDSCXMLUtils::logAllParameters(_CheckValueCmd->getInstanceName(), tempParamList, state);
		} 
		else
		{
			GTACMDSCXMLUtils::logAllParameters(_CheckValueCmd->getInstanceName(), _ParamInfoList, state);
		}
    }
    if (rc)
    {
        megaState.setId(_StateId);

        GTASCXMLTransition tx;
        tx.setTarget(_TargetId);
        megaState.insertTransitions(tx);

        GTASCXMLState preDebugState = GTACMDSCXMLUtils::getPreDebugState(getPreDebugStateID(),getExecStateID(),_PrevStateId,_CheckValueCmd->getInstanceName());
        megaState.insertStates(preDebugState);
        megaState.insertStates(state);
        GTASCXMLState debugState = GTACMDSCXMLUtils::getDebugState(getExecTargetID(),getFinalTargetID(),state.getVariableList(),_ParamInfoList);
        megaState.insertStates(debugState);

        megaState.setInitId(getPreDebugStateID());
        return megaState;
    }
    else
    {
        return state;
    }
}

GTASCXMLState GTACMDSCXMLCheckValue::getSCXMLCheckValue() const
{
    QString confTime,confTimeUnit;
    double dConfTime;
    QString timeOut, timeOutUnit;
    QStringList timeList;
    QString confTimeLine, timeOutLine;
     

    _CheckValueCmd->getConfCheckTime(confTime,confTimeUnit);
    _CheckValueCmd->getTimeOut(timeOut, timeOutUnit);

    if (confTime.contains("line."))
    {
        confTimeLine = confTime;
    }

    if (timeOut.contains("line."))
    {
        timeOutLine = timeOut;
    }

    bool isDoubleOk =false;
    dConfTime = confTime.toDouble(&isDoubleOk);
    
    if(confTimeUnit == ACT_CONF_TIME_UNIT_SEC)
    {
        dConfTime = dConfTime *1000;
    }
    confTime = QString::number(dConfTime);

    bool isWaitUntilEn = _CheckValueCmd->getWaitUntil();

    double dTimeoutInMs = _CheckValueCmd->getTimeOutInMs();
  

    GTASCXMLState checkValueState;
    QString forEachArr;

    if (!getPreDebugVariable().isEmpty())
    {
        GTASCXMLDataModel dataModel;
        GTASCXMLData data;
        data.setDataExpr("");
        data.setDataId(QString("%1_Return").arg(getExecStateID()));
        dataModel.insertDataItem(data);
        checkValueState.setDataModel(dataModel);
    }
    
    if((dConfTime > 0 && isWaitUntilEn) || (confTimeLine.contains("line.")))
    {
        QString chkValInitCondStateId = QString("%1_ChkValInitCond").arg(getExecStateID());
        QString chkValConfChkStateId = QString("%1_ChkValConfChk").arg(getExecStateID());
        QString chkValFinalStateId = QString("%1_final").arg(getExecStateID());

        GTASCXMLState chkValInitCondState;

        QString stateId = QString("%1_getParameters").arg(chkValInitCondStateId);
        QString evalStateId  = QString("%1_evalCond").arg(chkValInitCondStateId);
        QString ConfState1Final = QString("%1_final").arg(chkValInitCondStateId);

        GTASCXMLState paramState = getParamState(stateId,evalStateId);
        
        QStringList templist = evalStateId.split('_');
        forEachArr = getLineArray(templist);
               

        GTASCXMLState evalState = evalutateConditionAndTxState(evalStateId,ConfState1Final,stateId,false,paramState.getStates().isEmpty()? (paramState.getInvokes().isEmpty()? false : true) : paramState.getStates().first().getInvokes().isEmpty());

        chkValInitCondState.insertStates(paramState);
        chkValInitCondState.insertStates(evalState);

        GTASCXMLTransition tx;
        tx.setTarget(chkValConfChkStateId);
        chkValInitCondState.insertTransitions(tx);

        chkValInitCondState.setId(chkValInitCondStateId);

        QString conftimeoutEvent = QString("%1_confEvent").arg(getExecStateID());
        QString conftimeoutID = QString("%1_confID").arg(getExecStateID());

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

		GTASCXMLLog successLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG, _CheckValueCmd->getInstanceName(), "true", "true");
		GTASCXMLLog failLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::FAIL_LOG, _CheckValueCmd->getInstanceName(), "false", "false");
		
		/*GTASCXMLLog successLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG,_CheckValueCmd->getInstanceName(),"true", _CheckValueCmd->getInstanceName());
        GTASCXMLLog failLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::FAIL_LOG,_CheckValueCmd->getInstanceName(),"false", _CheckValueCmd->getInstanceName());*/
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
        if (confTimeLine.contains("line."))
        {
            QString columnName = confTimeLine.split("__").at(1);
            confTime = forEachArr + '.' + columnName;
            sendConfEvent.setDelay(QString("%1").arg(confTime));
            
           
        }
        else
        {
            sendConfEvent.setDelay(QString("%1ms").arg(confTime));
        }
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
        QStringList templist = evalStateId.split('_');
        forEachArr = getLineArray(templist);

        GTASCXMLState paramState = getParamState(stateId,evalStateId);

        if(isWaitUntilEn)
            loopBackStateId = stateId;
        GTASCXMLState evalState = evalutateConditionAndTxState(evalStateId,finalStateId,loopBackStateId,false,paramState.getStates().isEmpty()? (paramState.getInvokes().isEmpty()? false : true) : paramState.getStates().first().getInvokes().isEmpty());
        checkValueState.insertStates(paramState);
        checkValueState.insertStates(evalState);
    }

    QString actOnFail = _CheckValueCmd->getActionOnFail();
    if((dTimeoutInMs >0 && isWaitUntilEn) || (timeOutLine.contains("line.")))
    {
        //set the timeout event on entry
        QString timeoutEvent = QString("%1_timeoutEvent").arg(getExecStateID());
        QString timeoutEventID = QString("%1_timeoutEventID").arg(getExecStateID());
        GTASCXMLOnEntry onEntry;
        GTASCXMLSend sendTimeoutEvent;
        sendTimeoutEvent.setEvent(timeoutEvent);
        sendTimeoutEvent.setId(timeoutEventID);
                
        if (timeOutLine.contains("line."))
        {
            QString columnName = timeOutLine.split("__").at(1);
            timeOut = forEachArr + '.' + columnName;
            sendTimeoutEvent.setDelay(QString("%1").arg(timeOut));
            
        }
        else
        {
            sendTimeoutEvent.setDelay(QString("%1ms").arg(QString::number(dTimeoutInMs)));
        }
        onEntry.insertSend(sendTimeoutEvent);
        checkValueState.setOnEntry(onEntry);

        //cancel the timeout event on exit
        GTASCXMLCancel cancel;
        cancel.setSendEventId(timeoutEventID);
        GTASCXMLOnExit onExit = checkValueState.getOnExit();
        onExit.insertCancel(cancel);
        checkValueState.setOnExit(onExit);

        GTASCXMLTransition timeOutTx;
        timeOutTx.setEvent(timeoutEvent);
		GTASCXMLLog timeoutLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::TIMEOUT_LOG, _CheckValueCmd->getInstanceName(), "Timeout", "Timeout");
		
		if (!getPreDebugVariable().isEmpty())
			
		/*QString exprCondition = QString("%1 %2 %3").arg(_CheckValueCmd->getParameter(0), _CheckValueCmd->getCondition(0), _CheckValueCmd->getValue(0));
		
		GTASCXMLLog timeoutLog;
		if (_CheckValueCmd->getParameter(0).contains(GTA_CIC_IDENTIFIER))
		{
			timeoutLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::TIMEOUT_LOG, _CheckValueCmd->getInstanceName(), QString("Timeout (%1)").arg(_ParamInfoList[0].getName()), _ParamInfoList[0].getName());
		}
		else
		{
			timeoutLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::TIMEOUT_LOG, _CheckValueCmd->getInstanceName(), QString("Timeout (%1)").arg(_CheckValueCmd->getParameter(0)), exprCondition);
		}
		
		QString expr1 = timeoutLog.getExpression();
		expr1.replace("("+_CheckValueCmd->getParameter(0)+")",QString("('+%1+')").arg(_CheckValueCmd->getParameter(0)));
		timeoutLog.setExpression(expr1);
		if (!getPreDebugVariable().isEmpty())
        */{
            GTASCXMLAssign assign;
            assign.setLocation(QString("%1_Return").arg(getExecStateID()));
            assign.setExpression("{ 'Status' : 'Timeout' }");

            timeOutTx.setCondition(QString("(%1 == 'true')").arg(getPreDebugVariable()));
            timeOutTx.insertAssign(assign);

            //additional tx
            GTASCXMLTransition tx1;
            tx1.setTarget(getFinalTargetID());
            tx1.setCondition(QString("(%1 != 'true')").arg(getPreDebugVariable()));
            tx1.setEvent(timeoutEvent);
            tx1.setStatus("Timeout");
            tx1.insertLog(timeoutLog);
            tx1.insertAssign(assign);
            checkValueState.insertTransitions(tx1);
        }

        if(actOnFail == "stop")
            timeOutTx.setTarget(SCXML_FINAL_STATE_ID);
        else
            timeOutTx.setTarget(getExecTargetID());


        timeOutTx.setStatus("Timeout");
        timeOutTx.insertLog(timeoutLog);
        checkValueState.insertTransitions(timeOutTx);
    }

    checkValueState.setId(getExecStateID());

    GTASCXMLTransition tx;
    if (!getPreDebugVariable().isEmpty())
    {
        tx.setCondition(QString("(%1 == 'true')").arg(getPreDebugVariable()));

        //additional tx
        GTASCXMLTransition tx1;
        tx1.setTarget(getFinalTargetID());
        tx1.setCondition(QString("(%1 != 'true')").arg(getPreDebugVariable()));
        checkValueState.insertTransitions(tx1);
    }
    tx.setTarget(getExecTargetID());
    checkValueState.insertTransitions(tx);

	if (_isParamInCSV) {
		GTASCXMLData data = GTASCXMLData();
		data.setDataId(checkValueState.getId() + "_getParameters_" + "returnCheck");
		data.setDataExpr("");
		GTASCXMLDataModel dm = GTASCXMLDataModel();
		dm.insertDataItem(data);
		checkValueState.setDataModel(dm);
	}
    return checkValueState;
}

GTASCXMLState GTACMDSCXMLCheckValue::getParamState(const QString &iStateId, const QString &iTargetId) const
/**
 * This function have the goal to prepare check function inside a for each.
 * 
 * @param iStateId
 * @param iTargetId
 * @return GTASCXMLState, it's the correct scxml invoke in the scxml file. 
 */
{
	GTASCXMLState paramState;
	QString timeoutEventName = QString("%1_timeoutEvent").arg(getExecStateID());
	QString actOnFail = _CheckValueCmd->getActionOnFail();
	bool isActOnFail = false;
	if (actOnFail == "stop")
		isActOnFail = true;

	paramState = GTACMDSCXMLUtils::createGetParamInvokeState(_ParamInfoList, iStateId, iTargetId, "", isActOnFail, _CheckValueCmd->getInstanceName(), false, _HiddenDelay);

	double dTimeout = _CheckValueCmd->getTimeOutInMs();
	QString finalStateId = QString("%1_final").arg(getExecStateID());
	if (dTimeout > 0)
	{
		GTASCXMLTransition timeOutTx;
		timeOutTx.setEvent(timeoutEventName);
		if (actOnFail == "stop")
			timeOutTx.setTarget(SCXML_FINAL_STATE_ID);
		else
			timeOutTx.setTarget(finalStateId);

		GTASCXMLLog timeoutLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::TIMEOUT_LOG, _CheckValueCmd->getInstanceName(), "Timeout", "Timeout");
		timeOutTx.insertLog(timeoutLog);
	}
	QList<GTASCXMLState> subStates = paramState.getStates();
	if (_isParamInCSV && subStates.size() > 0)
	{
		//getting substate to modify it
		GTASCXMLState subState = subStates.at(0);

		//Modifying datamodel to add a value container used in foreach
		GTASCXMLData subData = GTASCXMLData();
		subData.setDataId(paramState.getId() + "_" + "forEachValue");
		subData.setDataExpr("");
		GTASCXMLDataModel subDm = subState.getDataModel();
		subDm.insertDataItem(subData);
		subState.setDataModel(subDm);

		//Modifying onEntry to assign a different value after each loop
		GTASCXMLOnEntry onEntryTemp;
		GTASCXMLAssign assignTemp = subState.getOnEntry().getAssigns().at(0);
		if (subStates.size() > 1) {
			GTASCXMLTransition tempTrans = subStates.at(0).getTransitions().at(0);
			tempTrans.setTarget(subStates.at(subStates.size() - 1).getTransitions().at(0).getTarget());
			subState.clearTransitions();
			subState.insertTransitions(tempTrans);
			GTACommandBase * cmdBase = _CheckValueCmd->getParent();
			GTAActionForEach * actionForEach = dynamic_cast<GTAActionForEach*>(cmdBase);
			QString title = _CheckValueCmd->getParameter(0).split("__").at(0);
			QStringList paramsInGTA = _CheckValueCmd->getListParameters();
			QStringList CSVContent = actionForEach->getCSVColumnByTitle(title);
			QString locationTemplate = assignTemp.getLocation().split('[').at(0);
			int paramIndex = 0;
			GTASCXMLAssign assignToAdd;
			if (paramsInGTA.size() == 1) {
				foreach(QString param, CSVContent)
				{
					assignToAdd.setExpression(param);
					assignToAdd.setLocation(locationTemplate + QString("[%1]").arg(paramIndex));
					onEntryTemp.insertAssign(assignToAdd);
					paramIndex++;
				}
			}
		}
		else {
			onEntryTemp = subState.getOnEntry();
		}
		QStringList templist = paramState.getId().split('_');
		QString cntr;
		QString arr;
		int i = templist.size() - 1;
		while(i>=0){
			if (templist.at(i) == "ForEachStates") {
				for (int j = 0; j < i; j++) {
					if (j != 0 ) {
						cntr += "_" + templist.at(j);
						arr += "_" + templist.at(j);
					}
					else {
						cntr += templist.at(j);
						arr += templist.at(j);
					}
					
				}
				cntr += "_arr_cntr";
				arr += "_line_arr";
				i = -1;
			}
			i--;
		}
		QString tempString = assignTemp.getLocation();
        assignTemp.setLocation(paramState.getId() + "_" + "GetParamList");

		assignTemp.setExpression(tempString.split('[').at(0) + '[' + cntr + ']');
		assignTemp.setExpression(arr + '[' + cntr + "]."+ _CheckValueCmd->getParameter(0).split("__").at(1));
		onEntryTemp.insertAssign(assignTemp);
		subState.setOnEntry(onEntryTemp);

		//Modifying invoke
		QList<GTASCXMLInvoke> invokeTemp;
		invokeTemp = subState.getInvokes();
		subState.clearInvokes();
		GTASCXMLInvoke invoke = invokeTemp.at(0);
		GTASCXMLXMLRPC funcContainer = invoke.getXMLRPCFunction();
		GTASCXMLXMLRPC::funcArg func;
		funcContainer.getArgumentByName("ParameterList", func);
        func.value = tempString.split('[').at(0);
		func.type = "array";
		funcContainer.updateArgumentByName("ParameterList", func);
		funcContainer.setFunctionName(GET_MULTI_PARAM_VALUE_SINGLE_SAMPLE);

		assignTemp = invoke.getAssigns().at(0);
		tempString = assignTemp.getLocation() + "[" + arr + '[' + cntr + "]."+ _CheckValueCmd->getParameter(0).split("__").at(1) + "]";
		invoke.clearAssigns();
		invoke.insertAssign(assignTemp);

		assignTemp.setLocation(paramState.getId() + "_" + "returnCheck");
		assignTemp.setExpression(tempString);
		invoke.insertAssign(assignTemp);

		assignTemp.setLocation(tempString.split('[').at(0));
		assignTemp.setExpression("[]");
		invoke.insertAssign(assignTemp);
		
		invoke.setInvokeFunction(funcContainer);
		subState.insertInvokes(invoke);

		paramState.clearStates();
		paramState.insertStates(subState);

	}
    return paramState;
}
GTASCXMLState GTACMDSCXMLCheckValue::evalutateConditionAndTxState(const QString &iStateId, const QString &iTargetId, const QString &iLoopBackStateId, bool isConfTimeEn, bool ParamStateHasInvokes) const
{
    GTASCXMLState evalState;

    bool isWaitUntil = _CheckValueCmd->getWaitUntil();
    QString actOnFail = _CheckValueCmd->getActionOnFail();
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
            localParamList.append(param.getTempParamName());
    }

	QString engSet = getEngSettings(); 
	QString condStmt;
    QString CondParam = QString("%1_CondVar").arg(iStateId);
    icdParamList.removeDuplicates();

	_CheckValueCmd->setEvalCondId(iStateId);
	bool rc = _CheckValueCmd->getSCXMLConditionStatement(icdParamList, condStmt, genToolData, engSet, false, localParamList); 
	//bool rc = _CheckValueCmd->getSCXMLConditionStatement(icdParamList,condStmt,genToolData,getEngSettings(),false,localParamList);
    GTASCXMLUtils::validateExpression(condStmt);

    GTASCXMLTransition trueTx;
    GTASCXMLTransition falseTx;

    if(!iLoopBackStateId.trimmed().isEmpty())
    {
        if(isConfTimeEn)
        {
            falseTx.setTarget(iTargetId);
            trueTx.setTarget(iLoopBackStateId);

            QString conftimeoutID = QString("%1_confID").arg(getExecStateID());
            GTASCXMLCancel cancel;
            cancel.setSendEventId(conftimeoutID);
            falseTx.insertCancel(cancel);
        }
        else
        {
            //set target as getParam state only if there is a parameter to check
            falseTx.setTarget(ParamStateHasInvokes? iTargetId: iLoopBackStateId);
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

        GTASCXMLData checkReturn;
        checkReturn.setDataExpr("{ 'Status' : 'KO' }");
        checkReturn.setDataId(iStateId + "_check_value_Return");

        GTASCXMLDataModel datamodel;
        datamodel.insertDataItem(condData);
        datamodel.insertDataItem(checkReturn);
        evalState.setDataModel(datamodel);

        if (!ParamStateHasInvokes)
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
		QString exprCondition = QString("%1 %2 %3").arg(_CheckValueCmd->getParameter(0), _CheckValueCmd->getCondition(0), _CheckValueCmd->getValue(0));

		GTASCXMLLog successLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG, _CheckValueCmd->getInstanceName(), CondParam, "true");
		GTASCXMLLog failLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::FAIL_LOG, _CheckValueCmd->getInstanceName(), CondParam, "false");

		QString expr = successLog.getExpression();
		QString expr1 = failLog.getExpression();
		expr.replace(CondParam, QString("'+%1+'").arg(CondParam));
		expr1.replace(CondParam, QString("'+%1+'").arg(CondParam));
		successLog.setExpression(expr);
		failLog.setExpression(expr1);
		
		// JIRA-133
		// We have to take in account CIC/NCIC Parameter to fill currentValue and expectedValue
		/*if (_CheckValueCmd->getParameter(0).contains(GTA_CIC_IDENTIFIER))
		{
			successLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG, _CheckValueCmd->getInstanceName(), CondParam, QString("'+%1+'").arg(icdParamList[0]));
			failLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::FAIL_LOG, _CheckValueCmd->getInstanceName(), CondParam, QString("'+%1+'").arg(icdParamList[0]));
		}
		else
		{
			successLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG, _CheckValueCmd->getInstanceName(), CondParam, CondParam);
			failLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::FAIL_LOG, _CheckValueCmd->getInstanceName(), CondParam, CondParam);
		}*/

        trueTx.insertLog(successLog);

        GTASCXMLAssign assignCheckReturn;
        assignCheckReturn.setLocation(iStateId + "_check_value_Return");
        assignCheckReturn.setExpression("{ 'Status' : 'OK' }");
        trueTx.insertAssign(assignCheckReturn);

        if(_RepeatLogs || !isWaitUntil || ParamStateHasInvokes)
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



