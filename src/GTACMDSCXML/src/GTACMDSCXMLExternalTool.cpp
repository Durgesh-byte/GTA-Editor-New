#include "GTACMDSCXMLExternalTool.h"
#include "GTASCXMLState.h"
#include "GTAUtil.h"
#include "GTACMDSCXMLLog.h"
#include "GTACMDSCXMLUtils.h"
#include "GTASCXMLXMLRPC.h"

GTACMDSCXMLExternalTool::GTACMDSCXMLExternalTool(GTAGenericToolCommand *pCmdNode, const QString &iStateId, const QString &iTargetId):
        GTACMDSCXMLBase(pCmdNode,iStateId,iTargetId)
{
    _ExtToolCmd = pCmdNode;
}

GTACMDSCXMLExternalTool::GTACMDSCXMLExternalTool()
{

}

GTACMDSCXMLExternalTool::~GTACMDSCXMLExternalTool()
{

}
QString GTACMDSCXMLExternalTool::getSCXMLString() const
{
    return getSCXMLState().getSCXMLString(_isUSCXMLEn);
}

GTASCXMLState GTACMDSCXMLExternalTool::getSCXMLState()const
{
    GTASCXMLState state;
    if(_ExtToolCmd != NULL)
    {
        state = getSCXMLForExternalTool();


        //innsert logs
        GTACMDSCXMLLog log;
        log.setReportType(GTACMDSCXMLLog::REP_ACTION,GTACMDSCXMLLog::REP_ACTCHK);
        log.setCommandName(_CmdName);
        log.setCommandInstanceId(_ExtToolCmd->getInstanceName());
        log.setCommandDescription(_ExtToolCmd->getComment());
        log.setCommandStatement(_ExtToolCmd->getStatement());
        QStringList requirements = _ExtToolCmd->getRequirements();
        log.insertRequirements(requirements);
        QStringList varList;// = state.getVariableList();
        GTACMDSCXMLUtils::insertVariablesInLogReport(_ParamInfoList,varList,log);

        GTASCXMLOnExit onExit = state.getOnExit();
        if(_GenNewLogFormat)
            onExit.insertExtraInfo(log.getLogElem());
        state.setOnExit(onExit);
        //not logging all parameters as ECMA errors are flooded in
//        GTACMDSCXMLUtils::logAllParameters(_ExtToolCmd->getInstanceName(),_ParamInfoList,state);
    }

    return state;
}

GTASCXMLState GTACMDSCXMLExternalTool::getSCXMLForExternalTool() const
{
    //creating extTool state id names
    QString extToolStateId = QString("%1").arg(_StateId);
    QString finalID = QString("%1_final").arg(extToolStateId);

    double dTimeout = _ExtToolCmd->getTimeOutInMs();
    QString timeoutInMs = QString::number(dTimeout);
    QString timeoutEventName = QString("%1_timeout").arg(_StateId);
    QString timeoutEventID = QString("%1_timeoutID").arg(_StateId);
    QString actOnFail = _ExtToolCmd->getActionOnFail();
    QString toolName = _ExtToolCmd->getToolName();
    

    //setting parent class for ExtTool
    GTASCXMLState ExtToolState;
    ExtToolState.setId(extToolStateId);

    //build Timeout event on entry
    GTASCXMLOnEntry onEntry;
	GTASCXMLOnExit onExit;
	if (dTimeout > 0)
	{
		GTASCXMLSend send;
		send.setDelay(QString("%1ms").arg(timeoutInMs));
		send.setEvent(timeoutEventName);
		send.setId(timeoutEventID);
		onEntry.insertSend(send);

		//cancel the timeout on exit
		GTASCXMLCancel cancel;
		cancel.setSendEventId(timeoutEventID);
		onExit.insertCancel(cancel);
	}
    
    ExtToolState.setOnEntry(onEntry);    
    ExtToolState.setOnExit(onExit);

    //build transition for timeout
    GTASCXMLTransition TimeoutTx;
    TimeoutTx.setStatus("KO");
    TimeoutTx.setEvent(timeoutEventName);

    if(actOnFail == "stop")
        TimeoutTx.setTarget(SCXML_FINAL_STATE_ID);
    else
        TimeoutTx.setTarget(_TargetId);

    //generating log expression
        QString cmdStmt = _ExtToolCmd->getStatement();
//        GTAScxmlLogMessage objLogTimeOut;
//        objLogTimeOut.LOD = GTAScxmlLogMessage::Main;
//        objLogTimeOut.Identifier    = _ExtToolCmd->getInstanceName();
//        objLogTimeOut.ExpectedValue = cmdStmt.replace("\n","  ").replace("\'","").replace("\"","");
//        objLogTimeOut.CurrentValue = "TimeOut";
//        objLogTimeOut.Status = GTAScxmlLogMessage::TimeOut;
//        objLogTimeOut.Result = GTAScxmlLogMessage::KO;
//        QString logMsgTimeout = objLogTimeOut.getLogMessage();

//        GTASCXMLLog timeoutLog;
//        timeoutLog.setExpression(logMsgTimeout);
//        timeoutLog.setLabel("error");

        GTASCXMLLog logTimeOut =GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::TIMEOUT_LOG,_ExtToolCmd->getInstanceName(),"",false);

    TimeoutTx.insertLog(logTimeOut);

    //list of functions in the ExtToll
    QList<GTAGenericFunction> Functions = _ExtToolCmd->getCommand();

    bool createJSON = _ExtToolCmd->getSCXMLModSelected();

    for(int i = 0; i < Functions.count(); i++)
    {
        //state id generation for each function call
        GTAGenericFunction objFunc = Functions.at(i);
        QString id = extToolStateId +"_fun_" + QString::number(i);
        QString nextId = extToolStateId +"_fun_" + QString::number(i+1);
        if(i >= Functions.count() - 1)
        {
            nextId = finalID;
        }

        QString funcName = objFunc.getFunctionName();
        bool waitforAck = objFunc.getWaitForAcknowledgement();
        bool isWaitUntilTrue = false;

        QString returnParamName = objFunc.getReturnParamName();
        QString returnParamType = objFunc.getReturnParamType();
        QString location = QString("%1:%2$%3").arg(_CmdName,_ExtToolCmd->getObjId().trimmed(),objFunc.getReturnParamName().trimmed());
        location.replace(" ","_");

//        if(i==0)
//            ExtToolState.setId(id);

        QString funcReturnName = QString("%1_%2").arg(id,returnParamName);

        QString returnTrueCond;
        isWaitUntilTrue = _ExtToolCmd->isWaitUntilEnabled(returnParamName,returnParamType);
        bool rcTrueCond = _ExtToolCmd->getSCXMLConditionStatement(funcReturnName,returnParamName,true,returnTrueCond,returnParamType);
        QString returnFalseCond;
        bool rcFalseCond = _ExtToolCmd->getSCXMLConditionStatement(funcReturnName,returnParamName,false,returnFalseCond,returnParamType);

        // QString returnCond = objFunc.getSCXMLConditionStatement(funcReturnName);

        GTASCXMLData data;
        data.setDataId(funcReturnName);
        GTASCXMLDataModel datamodel;
        datamodel.insertDataItem(data);

        //DEV:: for FWC Logger
        QString functionArray = QString("%1_array").arg(funcReturnName);

        if (createJSON)
        {
            data.setDataExpr("[]");
            data.setDataId(functionArray);
            datamodel.insertDataItem(data);
        }

        ExtToolState.setDataModel(datamodel);

        GTACMDSCXMLUtils::insertGenToolReturnParams(location,funcReturnName);


        //<STATE>
        GTASCXMLState FuncStateElement;
        FuncStateElement.setId(id);

        //<INVOKE>

        GTASCXMLInvoke InvokeElem;
        InvokeElem.setInvokeType("xml-rpc");


        QString invokeId = QString("%1_invoke").arg(id);


        InvokeElem.setInvokeId(invokeId);

        //<XMLRPC>
        GTASCXMLXMLRPC xmlRpcElem;
        xmlRpcElem.setToolId(objFunc.getToolID());
        xmlRpcElem.setToolType(objFunc.getToolType());
        xmlRpcElem.setFunctionName(objFunc.getFunctionName());

        //<Argument>
        QHash<QString, QString> JSON_arg;
        QString JSON_expression = "{";
        QList<GTAGenericArgument> argList = objFunc.getAllArguments();
        for(int j = 0 ; j < argList.count(); j++)
        {

            GTAGenericArgument objArg = argList.at(j);

            QString argType = objArg.getArgumentType();
            QString argValue = objArg.getUserSelectedValue();

			// To manage the old format of STRING - Avoit to have &quot; in SCXML file
			if (argValue.contains("\"") && argType.toLower() == "string")
				argValue.replace("\"", "'");

            GTASCXMLXMLRPC::funcArg arg;
            arg.name = objArg.getArgumentName();
            arg.type = argType;
            arg.value = argValue;

            //DEV:: for FWC Logger
            if (createJSON)
            {
                if(argType.toLower() == "string")
                {
					//value in JSON must be in single quotes to denote value. or else it will be treated as a variable
					if(arg.value.contains("'"))
                    arg.value.replace("'","");
					arg.value = QString("%1").arg(arg.value);

                    JSON_expression.append(QString(" '%1':'%2' ").arg(arg.name,arg.value));
                }
                else
                {
                    JSON_arg.insertMulti(arg.name,arg.value);
                    JSON_expression.append(QString(" '%1':'' ").arg(arg.name));
                }
                if (j < (argList.count() - 1))
                    JSON_expression.append(",");
            }
            else
            {
                xmlRpcElem.insertFunctionArgument(arg.type,arg.name,arg.value);
            }
        }

        //DEV:: for FWC Logger
        if (createJSON)
        {
            JSON_expression.append("}");
            //<ONENTRY>
            GTASCXMLOnEntry subOnEntry;
            GTASCXMLAssign assign;

            QString arrayName = functionArray;
            assign.setExpression(JSON_expression);
            assign.setLocation(arrayName.append("[0]"));
            subOnEntry.insertAssign(assign);

            xmlRpcElem.insertFunctionArgument("array","Parameterlist",functionArray);
            for (const auto& key : JSON_arg.keys())
            {
                GTASCXMLAssign assign1;
                assign1.setExpression(JSON_arg.value(key));
                QString subArrayName = functionArray;
                assign1.setLocation(subArrayName.append("[0].").append(key));
                subOnEntry.insertAssign(assign1);
            }
            FuncStateElement.setOnEntry(subOnEntry);
        }
        //<return value>
        if (waitforAck)
        {
            GTASCXMLXMLRPC::funcReturn ret;
            ret.name = funcReturnName;
            xmlRpcElem.insertFunctionReturn(ret.name,QString(""));
        }


        InvokeElem.setInvokeFunction(xmlRpcElem);


        //<ASSIGN>
        GTASCXMLAssign finalizeAssignElem;
        finalizeAssignElem.setLocation(funcReturnName);
        finalizeAssignElem.setExpression(QString("_event.data.%1").arg(funcReturnName));

        if (waitforAck)
        {
            InvokeElem.insertAssign(finalizeAssignElem);
        }

        FuncStateElement.insertInvokes(InvokeElem);



        //<TRANSITION - SUCCESS>
        GTASCXMLTransition TransitionElemSuccess;
        TransitionElemSuccess.setTarget(nextId);
        TransitionElemSuccess.setStatus("OK");
        //if (waitforAck)
		if (waitforAck && returnTrueCond != "")
        {
            if(rcTrueCond)
                TransitionElemSuccess.setCondition(QString("%1").arg(returnTrueCond));
            else
                TransitionElemSuccess.setCondition(QString("%1 == 'OK'").arg(returnTrueCond));
        }
        TransitionElemSuccess.setEvent("done.invoke._all");


        //LOG for <TRANSITION - SUCCESS>
        QRegExp rx("([!&?|/\\\\<>'\"\\(\\)]+)");

        GTAScxmlLogMessage objLogSuccess;
        objLogSuccess.LOD = GTAScxmlLogMessage::Main;
//        objLogSuccess.Identifier    = _ExtToolCmd->getInstanceName().replace("\n","  ").replace("\'","").replace("\"","").replace("\\","");
//        objLogSuccess.ExpectedValue = _ExtToolCmd->getStatement().replace("\n","  ").replace("\'","").replace("\"","").replace("\\","");
        objLogSuccess.Identifier    = _ExtToolCmd->getInstanceName();
        objLogSuccess.ExpectedValue = _ExtToolCmd->getStatement().replace("\\n"," ").replace("\\r","",Qt::CaseInsensitive).replace("\\t","  ").replace("\\v","  ",Qt::CaseInsensitive).replace(rx,"");
        if (waitforAck)
            objLogSuccess.CurrentValue = funcReturnName;
        else
            objLogSuccess.CurrentValue = "true";
        objLogSuccess.Status = GTAScxmlLogMessage::Success;
        objLogSuccess.Result = GTAScxmlLogMessage::OK;

        QString logMsgSuccess = objLogSuccess.getLogMessage();
        GTASCXMLLog logSuccessElem;
        logSuccessElem.setExpression(logMsgSuccess);
        logSuccessElem.setLabel("Info");
        TransitionElemSuccess.insertLog(logSuccessElem);

        QString failId = nextId;

        if((isWaitUntilTrue) && (dTimeout>0))
            failId = id;

        if(actOnFail == "stop")
            failId = SXN_FINAL;

        //<TRANSITION - FAIL>
        GTASCXMLTransition TransitionElemFail;
        if (waitforAck)
        {
            TransitionElemFail.setTarget(failId);
            TransitionElemFail.setStatus("KO");
			if (returnFalseCond != "") {
				if (rcFalseCond)
					TransitionElemFail.setCondition(QString("%1").arg(returnFalseCond));
				else
					TransitionElemFail.setCondition(QString("%1 != 'OK'").arg(returnFalseCond));
			}
            TransitionElemFail.setEvent("done.invoke._all");

            //LOG for <TRANSITION - FAIL>

            GTAScxmlLogMessage objLogFail;
            objLogFail.LOD = GTAScxmlLogMessage::Main;
            ////        objLogFail.Identifier    = _ExtToolCmd->getInstanceName().replace("\n","  ").replace("\'","").replace("\"","").replace("\\","");
            ////        objLogFail.ExpectedValue = _ExtToolCmd->getStatement().replace("\n","  ").replace("\'","").replace("\"","").replace("\\","");
            objLogFail.Identifier    = _ExtToolCmd->getInstanceName();
            objLogFail.ExpectedValue = _ExtToolCmd->getStatement().replace("\\n"," ").replace("\\r","",Qt::CaseInsensitive).replace("\\t","  ").replace("\\v","  ",Qt::CaseInsensitive).replace(rx,"");
            objLogFail.CurrentValue = funcReturnName;
            objLogFail.Status = GTAScxmlLogMessage::Fail;
            objLogFail.Result = GTAScxmlLogMessage::KO;
            QString logMsgFail = objLogFail.getLogMessage();
            GTASCXMLLog logFailElem;
            logFailElem.setExpression(logMsgFail);
            logFailElem.setLabel("Info");
            TransitionElemFail.insertLog(logFailElem);
    }

        //<TRANSITION - PARTIAL FAIL>
        GTASCXMLTransition TransitionElemPartialSuccess;
        if (waitforAck)
        {
            TransitionElemPartialSuccess.setTarget(failId);
            TransitionElemPartialSuccess.setStatus("POK");
			if (returnFalseCond != "" && returnTrueCond != "") {
				if (rcFalseCond && rcTrueCond)
					TransitionElemPartialSuccess.setCondition(QString("(%1 && %2)").arg(returnFalseCond.replace("==", "!="), returnTrueCond.replace("==", "!=")));
				else
					TransitionElemPartialSuccess.setCondition(QString("%1 && %2 != 'OK'").arg(returnFalseCond, returnTrueCond));
			}

            TransitionElemPartialSuccess.setEvent("done.invoke._all");

            //LOG for <TRANSITION - PARTIAL FAIL>

            GTAScxmlLogMessage objLogPartial;
            objLogPartial.LOD = GTAScxmlLogMessage::Main;
            //        objLogPartial.Identifier    = _ExtToolCmd->getInstanceName().replace("\n","  ").replace("\'","").replace("\"","").replace("\\","");
            //        objLogPartial.ExpectedValue = _ExtToolCmd->getStatement().replace("\n","  ").replace("\'","").replace("\"","").replace("\\","");
            objLogPartial.Identifier    = _ExtToolCmd->getInstanceName();
            objLogPartial.ExpectedValue = _ExtToolCmd->getStatement().replace("\\n"," ").replace("\\r","",Qt::CaseInsensitive).replace("\\t","  ").replace("\\v","  ",Qt::CaseInsensitive).replace(rx,"");
            objLogPartial.CurrentValue = funcReturnName;
            objLogPartial.Status = GTAScxmlLogMessage::None;
            objLogPartial.Result = GTAScxmlLogMessage::kNa;
            QString logMsgPartial = objLogPartial.getLogMessage();
            GTASCXMLLog logPartialElem;
            logPartialElem.setExpression(logMsgPartial);
            logPartialElem.setLabel("Info");
            TransitionElemPartialSuccess.insertLog(logPartialElem);
    }


        //Insert all transitions
        FuncStateElement.insertTransitions(TransitionElemSuccess);
        if (waitforAck)
        {
            FuncStateElement.insertTransitions(TransitionElemFail);
            FuncStateElement.insertTransitions(TransitionElemPartialSuccess);
        }
		
		//FuncStateElement.insertTransitions(TimeoutTx);
		if (dTimeout > 0)
			ExtToolState.insertTransitions(TimeoutTx);
		ExtToolState.insertStates(FuncStateElement);
    }

    //<TRANSITION> for entire state
    GTASCXMLTransition mainStateTx;

    mainStateTx.setTarget(_TargetId);
    ExtToolState.insertTransitions(mainStateTx);

    return ExtToolState;
}
