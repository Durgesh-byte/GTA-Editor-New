#include "GTACMDSCXMLUtils.h"
#include "GTASCXMLXMLRPC.h"
#include "GTAScxmlLogMessage.h"
#include "GTAUtil.h"
#include "GTACMDSCXMLLog.h"
#include "GTASCXMLData.h"
#include "GTASCXMLDataModel.h"

#pragma warning(push, 0)
#include <qdom.h>
#include <QStringList>
#include <QFile>
#include <QVariantMap>
#include <QDir>
#pragma warning(pop)

QString GTACMDSCXMLUtils::_LastError = "";
QString GTACMDSCXMLUtils::_SCXMLTemplatePath = "";
QHash<QString,QString> GTACMDSCXMLUtils::_genToolReturnMap;// = QHash<QString, QString>();
QString GTACMDSCXMLUtils::_exportingFileName = "";
GTACMDSCXMLUtils::GTACMDSCXMLUtils()
{

}

GTACMDSCXMLUtils::~GTACMDSCXMLUtils()
{

}

void GTACMDSCXMLUtils::setSCXMLTemplatePath(const QString &iTemplatePath)
{
    _SCXMLTemplatePath = QDir::cleanPath(iTemplatePath);
}

/**
 * @brief: This function returns the file name of the file whose scxml is being exported
 * @returns : file name of the file whose scxml is being exported
 */
QString GTACMDSCXMLUtils::getExportingFileName() { return _exportingFileName; }

/**
 * @brief: This function sets the file name of the file whose scxml is being exported
 * @param: name of the file whose scxml is being exported
 */
void GTACMDSCXMLUtils::setExportingFileName(QString exportingFile) { _exportingFileName = exportingFile; }

void GTACMDSCXMLUtils::groupParameters(const QList<GTAICDParameter> &iParamList, QHash<QString,QHash<QString, GTAICDParameter> > &oToolAppGroup)
{
    QHash<QString ,GTAICDParameter> ToolGroup;
    QStringList tools;

    //    ToolGroup = mapping of toolname to param in iParamList
    //    tools = toolname of each param in iParamList
    for(int i=0;i<iParamList.count();i++)
    {
        GTAICDParameter param = iParamList.at(i);
        updateWithGenToolParam(param);
        QString toolName = param.getToolName();
        ToolGroup.insertMulti(toolName,param);
        tools.append(toolName);
    }
	//removing duplicates to avoid large loop iterations
	tools.removeDuplicates();
    //creates and inserts a mapping between tool name and appgroup
    //so now ToolAppGroup has param-toolname-appname relation
    for(int i=0; i<tools.count(); i++)
    {
        QString toolName = tools.at(i);
        QList<GTAICDParameter> params = ToolGroup.values(tools.at(i));
        QHash<QString ,GTAICDParameter> AppGroup;
        for(int j=0; j<params.count(); j++)
        {
			//QString appName = params.at(i).getApplicationName();
			QString appName = params.at(j).getName().split(".")[0];
            AppGroup.insertMulti(appName,params.at(j));
        }
        oToolAppGroup.insert(toolName,AppGroup);
        AppGroup.clear();
    }
}
bool GTACMDSCXMLUtils::getInvokeFromTemplate(const QString &iTemplateFileName, GTASCXMLInvoke &oInvoke)
{
    return getCloneInvoke(iTemplateFileName,oInvoke);
}

bool GTACMDSCXMLUtils::getCloneInvoke(const QString & iTemplateFileName, GTASCXMLInvoke &oInvoke)
{
    QDomDocument oDomDoc;
    bool rc = openScxmlTemplateFile(QString("%1/%2").arg(_SCXMLTemplatePath,iTemplateFileName),oDomDoc);
    if(rc && (!oDomDoc.isNull()))
    {
        QDomElement rootElem = oDomDoc.documentElement();
        if(! rootElem.isNull())
        {
            oInvoke.translateXMLToStruct(rootElem);
        }
        else rc = false;
    }
    return rc;
}

bool GTACMDSCXMLUtils::openScxmlTemplateFile(const QString & iTemplateFile, QDomDocument & oDomDoc)
{
    bool rc = false;
    QFile xmlFileObj(iTemplateFile);
    rc = xmlFileObj.open(QFile::ReadOnly | QFile::Text);
    if(rc)
    {
        rc = oDomDoc.setContent(&xmlFileObj);
        if(rc == false)
            _LastError = QString("Error: invalid scxml format of file %1").arg(iTemplateFile);

        xmlFileObj.close();
    }
    else _LastError = QString("Error: Unable to open file %1").arg(iTemplateFile);

    return rc;
}

GTASCXMLState GTACMDSCXMLUtils::createParamPopUpInvokeState(const QString &iMsg, const QString &iStateId, const QString &iTargetId, GTAICDParameter &iParameter, const QString &iCmdId)
{
    GTASCXMLState popUpState;
    GTASCXMLInvoke invoke;
    updateWithGenToolParam(iParameter);

    bool rc = getCloneInvoke("getUserFeedback.scxml",invoke);
    if(rc)
    {
        QString invokeId = QString("%1_i0").arg(iStateId);
        invoke.setInvokeId(invokeId);
        GTASCXMLXMLRPC func = invoke.getXMLRPCFunction();
        GTASCXMLXMLRPC::funcArg arg1;
        func.getArgumentByName("message",arg1);
        arg1.name="";
        arg1.type="string";
        arg1.value=QString("%1_Message").arg(iStateId);

        GTASCXMLData data;
        data.setDataExpr(QString("'%1'").arg(iMsg));
        data.setDataId(arg1.value);

        GTASCXMLDataModel datamodel;
        datamodel.insertDataItem(data);

        popUpState.setDataModel(datamodel);

        GTASCXMLXMLRPC::funcArg arg2;
        func.getArgumentByName("ExpectedReturnType",arg2);

        arg2.name="ExpectedReturnType";
        arg2.type="string";
        arg2.value=QString("'%1'").arg(iParameter.getValueType());

        func.updateArgumentByName("message",arg1);
        func.updateArgumentByName("ExpectedReturnType",arg2);

        QString retParamName = QString("%1_Return").arg(iStateId);

        GTASCXMLXMLRPC::funcReturn ret;
        ret.name = retParamName;
        func.updateReturnArgument(ret);

        invoke.setInvokeFunction(func);


        GTASCXMLAssign assignReturn;
        assignReturn.setExpression(QString("_event.data.%1").arg(retParamName));
        assignReturn.setLocation(iParameter.getTempParamName());
		if (iParameter.getTempParamName().isEmpty())
		{
			assignReturn.setLocation(iParameter.getName());
		}
        invoke.insertAssign(assignReturn);

		GTASCXMLLog log2;

		if (iParameter.getTempParamName().isEmpty())
		{
			log2 = getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG, iCmdId, iParameter.getName(), iParameter.getName());
		}
		else
		{
			log2 = getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG, iCmdId, iParameter.getTempParamName(), iParameter.getTempParamName());
		}
        
        GTASCXMLTransition tx;
        tx.setEvent(QString("done.invoke.%1").arg(invokeId));
        tx.setTarget(iTargetId);
        tx.setStatus("OK");
        tx.insertLog(log2);

        popUpState.setId(iStateId);
        popUpState.insertInvokes(invoke);
        popUpState.insertTransitions(tx);

        GTASCXMLData id;
        id.setDataId(retParamName);
        GTASCXMLDataModel model = popUpState.getDataModel();
        popUpState.setDataModel(model);

        GTASCXMLOnExit onExit = popUpState.getOnExit();

		GTASCXMLLog infoLog;

		if (iParameter.getTempParamName().isEmpty())
		{
			infoLog = getLogMessage(GTACMDSCXMLUtils::INFO_LOG, iCmdId, iParameter.getName(), iParameter.getName());
		}
		else
		{
			infoLog = getLogMessage(GTACMDSCXMLUtils::INFO_LOG, iCmdId, iParameter.getTempParamName(), iParameter.getTempParamName());
		}

        onExit.insertLog(infoLog);

        popUpState.setOnExit(onExit);

    }
    return popUpState;
}


GTASCXMLState GTACMDSCXMLUtils::createPopUpInvokeState(const QString &iMsg, const QString &iStateId, const QString &iTargetId, bool &iWaitForAck, const QString &iCmdId)
{
    GTASCXMLState popUpState;
    GTASCXMLInvoke invoke;
    bool rc = getCloneInvoke("get_popup.scxml",invoke);
    if(rc)
    {
        QString invokeId = QString("%1_i0").arg(iStateId);
        invoke.setInvokeId(invokeId);
        GTASCXMLXMLRPC func = invoke.getXMLRPCFunction();
        GTASCXMLXMLRPC::funcArg arg1;
        func.getArgumentByName("message",arg1);
        arg1.name="";
        arg1.type="string";
        arg1.value=QString("%1_Message").arg(iStateId);

        GTASCXMLData data;
        data.setDataExpr(QString("'%1'").arg(iMsg));
        data.setDataId(arg1.value);

        GTASCXMLDataModel datamodel;
        datamodel.insertDataItem(data);

        GTASCXMLData data2;
        QString returnValue = QString("%1_RetVal").arg(iStateId);
        if(iWaitForAck)
        {
            data2.setDataExpr("");
            data2.setDataId(returnValue);
            datamodel.insertDataItem(data2);
        }

        popUpState.setDataModel(datamodel);


        GTASCXMLXMLRPC::funcArg arg2;
        func.getArgumentByName("wait_ack",arg2);

        arg2.name="wait_ack";
        arg2.type="boolean";
        if(iWaitForAck)
            arg2.value="'true'";
        else
            arg2.value="'false'";

        func.updateArgumentByName("message",arg1);
        func.updateArgumentByName("wait_ack",arg2);

        QString retParamName = QString("%1_Return").arg(iStateId);
        if(iWaitForAck)
        {
            GTASCXMLXMLRPC::funcReturn ret;
            ret.name = retParamName;
            func.insertFunctionReturn(retParamName,"");
            func.updateReturnArgument(ret);
        }

        invoke.setInvokeFunction(func);

        if(iWaitForAck)
        {
            GTASCXMLAssign assignReturn;
            assignReturn.setExpression(QString("_event.data.%1").arg(retParamName));
            assignReturn.setLocation(data2.getId());
            invoke.insertAssign(assignReturn);
        }
        
        GTAScxmlLogMessage logMessage1;
        logMessage1.CurrentValue = returnValue;
        logMessage1.ExpectedValue = "true";
        logMessage1.Identifier = iCmdId;
        logMessage1.FunctionStatus = "NA";

        logMessage1.Result = GTAScxmlLogMessage::OK;
        logMessage1.Status = GTAScxmlLogMessage::Success;
        logMessage1.LOD = GTAScxmlLogMessage::Main;
        logMessage1.isCurrentValParam = false;
        QString log1 = logMessage1.getLogMessage();
        GTASCXMLLog log2;
        log2.setLabel("Info");
        log2.setExpression(log1);

        GTASCXMLTransition tx;
        tx.setEvent(QString("done.invoke.%1").arg(invokeId));
        tx.setTarget(iTargetId);
        tx.setStatus("OK");
        if(iWaitForAck)
            tx.setCondition(QString("(%1 == true)").arg(data2.getId()));
        tx.insertLog(log2);


        GTASCXMLTransition tx2;
        if(iWaitForAck)
        {
            logMessage1.ExpectedValue = "false";
            logMessage1.Result = GTAScxmlLogMessage::KO;
            logMessage1.Status = GTAScxmlLogMessage::Fail;
            QString log = logMessage1.getLogMessage();

            GTASCXMLLog log3;
            log3.setLabel("Info");
            log3.setExpression(log);

            tx2.setEvent(QString("done.invoke.%1").arg(invokeId));
            tx2.setTarget(iTargetId);
            tx2.setStatus("KO");
            if(iWaitForAck)
                tx2.setCondition(QString("(%1 != true)").arg(data2.getId()));
            tx2.insertLog(log3);
        }

        
        popUpState.setId(iStateId);
        popUpState.insertInvokes(invoke);
        popUpState.insertTransitions(tx);
        if(iWaitForAck)
        {
            popUpState.insertTransitions(tx2);
        }

    }
    return popUpState;
}

GTASCXMLInvoke GTACMDSCXMLUtils::createDummyInvokeForLoop(const QString &iStateId)
{
    GTASCXMLInvoke InvokeElem;
    InvokeElem.setInvokeType("xml-rpc");


    QString invokeId = QString("%1_invoke").arg(iStateId);
    InvokeElem.setInvokeId(invokeId);

    GTASCXMLXMLRPC xmlRpcElem;
    xmlRpcElem.setToolId("Demonstrator");
    xmlRpcElem.setToolType("Tool");
    xmlRpcElem.setFunctionName("change_text");

    GTASCXMLXMLRPC::funcArg arg;
    arg.name = "text";
    arg.type = "string";
    arg.value = "'OK'";

    xmlRpcElem.insertFunctionArgument(arg.type,arg.name,arg.value);

    QString funcReturnName = QString("%1_ResultStruct").arg(iStateId);
    GTASCXMLXMLRPC::funcReturn ret;
    ret.name = funcReturnName;
    xmlRpcElem.insertFunctionReturn(ret.name,QString(""));

    InvokeElem.setInvokeFunction(xmlRpcElem);

    GTASCXMLAssign finalizeAssignElem;
    finalizeAssignElem.setLocation(funcReturnName);
    finalizeAssignElem.setExpression(QString("_event.data.%1").arg(funcReturnName));

    InvokeElem.insertAssign(finalizeAssignElem);

    return InvokeElem;
}

GTASCXMLState GTACMDSCXMLUtils::createGetParamInvokeState(const QList<GTAICDParameter> &iParamList,
                                                                const QString &iStateId,
                                                                const QString &iTargetId,
                                                                const QString iTimeoutEventName,
                                                                bool isActOnFail,
                                                                const QString &iIdentifier,
                                                                bool iAddLogMessageInTx,
                                                                const QString &iInvokeDelay,
                                                                const QString &itimeoutTarget)
{
    QHash<QString,QHash<QString, GTAICDParameter> > toolAppGroup;
    groupParameters(iParamList, toolAppGroup);

    QList<QString> toolsTemp = toolAppGroup.keys();
    QStringList tools = QStringList(toolsTemp);
    tools.removeDuplicates();
    foreach (QString tool,tools)
    {
        if (tool == "NA")
        {
            tools.removeAll(tool);
            break;
        }
    }
    GTASCXMLState invokeState;
    invokeState.setId(iStateId);

    GTASCXMLTransition timeoutTx;
    if(!iTimeoutEventName.isEmpty())
    {
        timeoutTx.setEvent(iTimeoutEventName);
        timeoutTx.setStatus("TIMEOUT");
        if(isActOnFail)
            timeoutTx.setTarget(SCXML_FINAL_STATE_ID);
        else
            timeoutTx.setTarget(itimeoutTarget.isEmpty()? iTargetId : itimeoutTarget);

        GTASCXMLLog timeoutLog = getLogMessage(GTACMDSCXMLUtils::TIMEOUT_LOG,iIdentifier,"false","false");
        timeoutTx.insertLog(timeoutLog);
    }
    int stateIdCounter = 0;
    //traversing through tool names from list
    for(int i=0;i<tools.count();i++)
    {
        QString toolName = tools.at(i);

        //checking only for non-local params
        if(toolName != "NA")
        {
            QHash<QString, GTAICDParameter> appNameParamList = toolAppGroup.value(toolName);

            QList<QString> appNamesTemp = appNameParamList.keys();
            QStringList appNames = QStringList(appNamesTemp);
            appNames.removeDuplicates();

            GTASCXMLState invokeParamState;

            //numbering of the getparamstate inside invokestate
            QString currStateId = QString("%1_getParamState%2").arg(iStateId,QString::number(stateIdCounter++));
            QString currTargetId = QString("%1_getParamState%2").arg(iStateId,QString::number(stateIdCounter));
            //finalizing on targetid. Move targetid to final of invokestate if no actions remain
            if((i>=(tools.count() -1)))
                currTargetId  = QString("%1_final").arg(iStateId);

            QString invokeId = QString("%1_i0").arg(currStateId);
            QString paramArray = QString("%1_GetParamList").arg(currStateId);
            GTASCXMLTransition tx;
            GTASCXMLOnEntry onEntry;
            GTASCXMLInvoke invoke;
            bool rc = getCloneInvoke("get_param.scxml",invoke);
            if(rc)
            {
                invoke.setInvokeId(invokeId);                    
                GTASCXMLAssign funcRet;
                funcRet.setExpression(QString("_event.data.%1_Return").arg(invokeId));
                funcRet.setLocation(paramArray);
                invoke.insertAssign(funcRet);
                int count_param = 0;
                for (int j = 0; j < appNames.count(); j++)
                {
                    QList<GTAICDParameter> paramList = appNameParamList.values(appNames.at(j));
                    for (int k = 0; k < paramList.count(); k++)
                    {
                        GTAICDParameter param = paramList.at(k);
                        updateWithGenToolParam(param);
                        GTASCXMLAssign assign;
                        QString fileName = GTACMDSCXMLUtils::getExportingFileName();
                        if (toolName.contains("instrum", Qt::CaseInsensitive))
                        {
                            assign.setExpression(QString("{'paramName':'%1', 'fileName':'" + fileName + "'}").arg(param.getName()));
                        }
                        else
                        {
                            assign.setExpression(QString("'%1'").arg(param.getName()));
                        }

                        assign.setLocation(QString("%1[%2]").arg(paramArray, QString::number(count_param)));
                        onEntry.insertAssign(assign);
                        GTASCXMLAssign paramInit;
                        paramInit.setExpression(QString("%1['%2']").arg(paramArray, param.getName()));
                        paramInit.setLocation(param.getName());
                        //paramInit.setLocation(param.getTempParamName());
                        invoke.insertAssign(paramInit);
                        count_param += 1;
                    }
                }
                GTASCXMLAssign arrayClear;
                arrayClear.setExpression("[]");
                arrayClear.setLocation(paramArray);
                invoke.insertAssign(arrayClear);
                GTASCXMLXMLRPC func = invoke.getXMLRPCFunction();
                func.setToolId(toolName);
                GTASCXMLXMLRPC::funcArg arg;
                func.getArgumentByName("ParameterList",arg);
                arg.value = paramArray;
                func.updateArgumentByName("ParameterList",arg);
                GTASCXMLXMLRPC::funcReturn ret;
                func.getReturnArgument(ret);
                ret.name = QString("%1_Return").arg(invokeId);
                func.updateReturnArgument(ret);
                invoke.setInvokeFunction(func);

                if(!iInvokeDelay.trimmed().isEmpty())
                {

                    GTASCXMLSend invokeDelaySend;
                    QString dealyEvent = QString("%1_dealyEvent").arg(invokeId);
                    invokeDelaySend.setDelay(iInvokeDelay);
                    invokeDelaySend.setEvent(dealyEvent);
                    invoke.insertSend(invokeDelaySend);
                    tx.setEvent(dealyEvent);
                }
                else
                {
                    tx.setEvent(QString("done.invoke.%1").arg(invokeId));
                }
				invokeParamState.insertInvokes(invoke);
                //tx.setEvent(QString("done.invoke.%1").arg(invokeId));
                if(!iTimeoutEventName.isEmpty())
                    invokeParamState.insertTransitions(timeoutTx);
            }
            invokeParamState.setId(currStateId);
            invokeParamState.insertOnEntry(onEntry);
            tx.setTarget(currTargetId);
            invokeParamState.insertTransitions(tx);
            GTASCXMLData id;
            id.setDataId(QString("%1_Return").arg(invokeId));
            GTASCXMLDataModel model = invokeParamState.getDataModel();
            model.insertDataItem(id);
            id.setDataId(paramArray);
            id.setDataExpr("[]");
            model.insertDataItem(id);
            invokeParamState.setDataModel(model);
            invokeState.insertStates(invokeParamState);
        }
    }
    GTASCXMLTransition tx1;
    tx1.setTarget(iTargetId);
    tx1.setStatus("OK");
    if(iAddLogMessageInTx)
    {
		GTASCXMLLog okLog = getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG, iIdentifier, "true", "true");
		tx1.insertLog(okLog);
		
		/*foreach(GTAICDParameter iParam, iParamList)
		{
			// ICD PARAMETER
			if (!iParam.getSourceFile().isEmpty())
			{
				GTASCXMLLog okLog = getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG, iIdentifier, QString("%2 = '+%1+'").arg(iParam.getName(), iParam.getName()), QString()); 
				tx1.insertLog(okLog);
			}
			else // LOCAL PARAMETER
			{
				GTASCXMLLog okLog;
				if (iParam.getTempParamName().isEmpty())
				{
					okLog = getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG, iIdentifier, QString("%2 = '+%1+'").arg(iParam.getName(), iParam.getName()), QString());
				}
				else
				{
					okLog = getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG, iIdentifier, QString("%2 = '+%1+'").arg(iParam.getTempParamName(), iParam.getTempParamName()), QString());
				}
				tx1.insertLog(okLog);
			}
		}*/
    }
    invokeState.insertTransitions(tx1);
    return invokeState;
}

GTASCXMLState GTACMDSCXMLUtils::createSetParamInvokeStateForEquations(GTAICDParameter &iLHSParam,const QString &iStateId, const QString &iTargetId, bool iSetOnlyFS, QString &iFSValue, QString &iIdentifier, QString &FunctionType, QString &FunctionValue, const QString &iInvokeDelay)
{
	//parent state that will have compounded state
	GTASCXMLState SetState;
	SetState.setId(iStateId);

	//check valid LHS RHS params. add local params with toolname
	updateWithGenToolParam(iLHSParam);
	
	QString setParamStateId = QString("%1_SetParam").arg(iStateId);
	QString finalStateId=  QString("%1_final").arg(iStateId);

	bool addKOTx = false;

	GTASCXMLState setParamState;

	GTASCXMLDataModel model;
	QString invokeId = QString("%1_i0").arg(setParamStateId);
	QString paramArray = QString("%1_SetParamList").arg(setParamStateId);
	GTASCXMLTransition OKTx;
	GTASCXMLTransition KOTx;
	GTASCXMLOnEntry onEntry;
	GTASCXMLInvoke invoke;

	QString retArgName = QString("%1_Return").arg(invokeId);

	//checking if LHS is not a local param
	if(iLHSParam.getToolName() != "NA")
	{
		addKOTx = true;
		bool rc = getCloneInvoke("set_param.scxml",invoke);
		if(rc)
		{
			//creating an assign to receive the return
			GTASCXMLAssign funcRet;
			funcRet.setExpression(QString("_event.data.%1").arg(retArgName));
			funcRet.setLocation(retArgName);

			//setting the invoke id and inserting assign
			invoke.setInvokeId(invokeId);
			invoke.insertAssign(funcRet);

			GTAICDParameter param = iLHSParam;

			GTASCXMLData id;
			id.setDataId(paramArray);
			id.setDataExpr("[]");

			//populating model that will be assigned to setParamState
			model.insertDataItem(id);

			//assigning location to expresssion on entry to setParamState
			GTASCXMLAssign assign;
			assign.setLocation(QString("%1[%2]").arg(paramArray,QString::number(0)));


			QString fsValue = iFSValue;
			if(fsValue.trimmed().isEmpty())
				fsValue = ACT_FSSSM_NOCHANGE;
			//check for setting only FunctionalSstatus(FS)
			if(iSetOnlyFS)
			{
				assign.setExpression(QString("{ 'Parameter': '%1', 'FunctionType':'%2' ,'Status':'%3','FunctionValue':{'Value':'NA'}}").arg(param.getName(),param.getValueType(),fsValue));
				//assigning the value of param to location
				onEntry.insertAssign(assign);
			}
			else
			{
				assign.setExpression(QString("{ 'Parameter': '%1', 'FunctionType':'%2' ,'Status':'%3',%4}").arg(param.getName(),FunctionType,fsValue,FunctionValue));
				//assigning the value of param to location
				onEntry.insertAssign(assign);
			}

			GTASCXMLAssign arrayClear;
			arrayClear.setExpression("[]");
			arrayClear.setLocation(paramArray);
			invoke.insertAssign(arrayClear);
			GTASCXMLXMLRPC func = invoke.getXMLRPCFunction();
			func.setToolId(iLHSParam.getToolName());
			GTASCXMLXMLRPC::funcArg arg;
			func.getArgumentByName("ParameterList",arg);
			arg.value = paramArray;
			func.updateArgumentByName("ParameterList",arg);
			GTASCXMLXMLRPC::funcReturn ret;
			func.getReturnArgument(ret);
			ret.name = retArgName;

			GTASCXMLData id1;
			id1.setDataId(retArgName);
			id1.setDataExpr("");
			model.insertDataItem(id1);


			func.updateReturnArgument(ret);
			invoke.setInvokeFunction(func);



			if(iInvokeDelay.trimmed().isEmpty())
			{
				OKTx.setEvent(QString("done.invoke.%1").arg(invokeId));
				KOTx.setEvent(QString("done.invoke.%1").arg(invokeId));
			}
			else
			{
				GTASCXMLSend invokeDelaySend;
				QString dealyEvent = QString("%1_dealyEvent").arg(invokeId);
				invokeDelaySend.setDelay(iInvokeDelay);
				invokeDelaySend.setEvent(dealyEvent);
				invoke.insertSend(invokeDelaySend);

				OKTx.setEvent(dealyEvent);
				KOTx.setEvent(dealyEvent);
			}

			setParamState.insertInvokes(invoke);

			OKTx.setStatus("OK");
			OKTx.setCondition(QString("%1.Status == 'OK'").arg(retArgName));
			KOTx.setStatus("KO");
			KOTx.setCondition(QString("%1.Status != 'OK'").arg(retArgName));
		}
	}
	else
	{
		//a local parameter cannot be assigned with an equation
	}
	setParamState.setId(setParamStateId);
	setParamState.setOnEntry(onEntry);
	OKTx.setTarget(finalStateId);
	KOTx.setTarget(finalStateId);

	GTASCXMLLog okLog = getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG,iIdentifier,"true","true");
	GTASCXMLLog koLog = getLogMessage(GTACMDSCXMLUtils::FAIL_LOG,iIdentifier,"true","true");

	OKTx.insertLog(okLog);
	KOTx.insertLog(koLog);

	setParamState.insertTransitions(OKTx);
	if(addKOTx)
		setParamState.insertTransitions(KOTx);

	GTASCXMLFinal setParamFinal;
	setParamFinal.setId(finalStateId);
	SetState.setFinal(setParamFinal);

	setParamState.setDataModel(model);
	SetState.insertStates(setParamState);

	GTASCXMLTransition tx;
	tx.setTarget(iTargetId);
	tx.setStatus(TX_STATUS_OK);

	SetState.insertTransitions(tx);
	return SetState;
}

//creates an invoke or assign to Set a bench/local parameter. Does not incorporate Equations
GTASCXMLState GTACMDSCXMLUtils::createSetParamInvokeState(GTAICDParameter &iLHSParam,
                                                                GTAICDParameter &iRHSParam,
                                                                const QString &iStateId, 
                                                                const QString &iTargetId, 
                                                                bool iSetOnlyFS, 
                                                                const QString &iFSValue, 
                                                                const QString &iIdentifier, 
                                                                const QString &iInvokeDelay)
{
    //parent state that will have compounded state
    GTASCXMLState SetState;
    SetState.setId(iStateId);

    //check valid LHS RHS params. add local params with toolname
    updateWithGenToolParam(iLHSParam);
    updateWithGenToolParam(iRHSParam);

    QString iRHSValue = iRHSParam.getTempParamName();
	if (iRHSValue.isEmpty())
	{
		iRHSValue = iRHSParam.getName();
	}

    QString setParamStateId = QString("%1_SetParam").arg(iStateId);
    QString finalStateId=  QString("%1_final").arg(iStateId);

    bool addKOTx = false;

    //checking if RHS is not a local param
    if(iRHSParam.getToolName() != "NA")
    {
        QString getParamStateId = QString("%1_GetParam").arg(iStateId);
        QList<GTAICDParameter> rhsParamList;
        rhsParamList.append(iRHSParam);
        //call a getParam for the bench param of RHS
        GTASCXMLState getParamState = createGetParamInvokeState(rhsParamList,getParamStateId,setParamStateId,QString(),false,QString(),false,iInvokeDelay);
        SetState.insertStates(getParamState);
        iRHSValue = iRHSValue.append(".Value");
    }

    GTASCXMLState setParamState;

    GTASCXMLDataModel model;
    QString invokeId = QString("%1_i0").arg(setParamStateId);
    QString paramArray = QString("%1_SetParamList").arg(setParamStateId);
    GTASCXMLTransition OKTx;
    GTASCXMLTransition KOTx;
    GTASCXMLOnEntry onEntry;
    GTASCXMLInvoke invoke;

    QString retArgName = QString("%1_Return").arg(invokeId);

    //checking if LHS is not a local param
    if(iLHSParam.getToolName() != "NA")
    {
        addKOTx = true;
        bool rc = getCloneInvoke("set_param.scxml",invoke);
        if(rc)
        {
            //creating an assign to receive the return
            GTASCXMLAssign funcRet;
            funcRet.setExpression(QString("_event.data.%1").arg(retArgName));
            funcRet.setLocation(retArgName);

            //setting the invoke id and inserting assign
            invoke.setInvokeId(invokeId);
            invoke.insertAssign(funcRet);

            GTAICDParameter param = iLHSParam;

            GTASCXMLData id;
            id.setDataId(paramArray);
            id.setDataExpr("[]");

            //populating model that will be assigned to setParamState
            model.insertDataItem(id);

            //assigning location to expresssion on entry to setParamState
            GTASCXMLAssign assign;
            assign.setLocation(QString("%1[%2]").arg(paramArray,QString::number(0)));
            

            QString fsValue = iFSValue;
            if(fsValue.trimmed().isEmpty())
                fsValue = ACT_FSSSM_NOCHANGE;
            //check for setting only FunctionalSstatus(FS)
            if(iSetOnlyFS)
            {
                assign.setExpression(QString("{ 'Parameter': '%1', 'FunctionType':'%2' ,'Status':'%3','FunctionValue':{'Value':'NA'}}").arg(param.getName(),param.getValueType(),fsValue));
                //assigning the value of param to location
                onEntry.insertAssign(assign);
            }
            else
            {
                assign.setExpression(QString("{ 'Parameter': '%1', 'FunctionType':'%2' ,'Status':'%3','FunctionValue':{'Value':''}}").arg(param.getName(),param.getValueType(),fsValue));
                GTASCXMLAssign assign1;
                assign1.setLocation(QString("%1[0].FunctionValue.Value").arg(paramArray));
                if((param.getValueType().toLower() == "string") ||(param.getValueType().toLower() == "char"))
                    assign1.setExpression(QString("'%1'").arg(iRHSValue));
                else
                    assign1.setExpression(iRHSValue);
                //assigning the value of param to location
                onEntry.insertAssign(assign);
                //assigning value of specific parameter of array
                onEntry.insertAssign(assign1);
            }

            GTASCXMLAssign arrayClear;
            arrayClear.setExpression("[]");
            arrayClear.setLocation(paramArray);
            invoke.insertAssign(arrayClear);
            GTASCXMLXMLRPC func = invoke.getXMLRPCFunction();
            func.setToolId(iLHSParam.getToolName());
            GTASCXMLXMLRPC::funcArg arg;
            func.getArgumentByName("ParameterList",arg);
            arg.value = paramArray;
            func.updateArgumentByName("ParameterList",arg);
            GTASCXMLXMLRPC::funcReturn ret;
            func.getReturnArgument(ret);
            ret.name = retArgName;

            GTASCXMLData id1;
            id1.setDataId(retArgName);
            id1.setDataExpr("");
            model.insertDataItem(id1);


            func.updateReturnArgument(ret);
            invoke.setInvokeFunction(func);



            if(iInvokeDelay.trimmed().isEmpty())
            {
                OKTx.setEvent(QString("done.invoke.%1").arg(invokeId));
                KOTx.setEvent(QString("done.invoke.%1").arg(invokeId));
            }
            else
            {
                GTASCXMLSend invokeDelaySend;
                QString dealyEvent = QString("%1_dealyEvent").arg(invokeId);
                invokeDelaySend.setDelay(iInvokeDelay);
                invokeDelaySend.setEvent(dealyEvent);
                invoke.insertSend(invokeDelaySend);

                OKTx.setEvent(dealyEvent);
                KOTx.setEvent(dealyEvent);
            }

            setParamState.insertInvokes(invoke);

            OKTx.setStatus("OK");
            OKTx.setCondition(QString("%1.Status == 'OK'").arg(retArgName));
            KOTx.setStatus("KO");
            KOTx.setCondition(QString("%1.Status != 'OK'").arg(retArgName));
        }
    }
    else
    {
        GTASCXMLAssign assign;
        assign.setExpression(iRHSValue);
        assign.setLocation(iLHSParam.getTempParamName());
		if (iLHSParam.getTempParamName().isEmpty())
		{
			assign.setLocation(iLHSParam.getName());
		}
        onEntry.insertAssign(assign);
        OKTx.setStatus("OK");
    }
    setParamState.setId(setParamStateId);
    setParamState.setOnEntry(onEntry);
    OKTx.setTarget(finalStateId);
    KOTx.setTarget(finalStateId);

    GTASCXMLLog okLog = getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG,iIdentifier,"true","true");
    GTASCXMLLog koLog = getLogMessage(GTACMDSCXMLUtils::FAIL_LOG,iIdentifier,"true","true");

    OKTx.insertLog(okLog);
    KOTx.insertLog(koLog);

    setParamState.insertTransitions(OKTx);
    if(addKOTx)
        setParamState.insertTransitions(KOTx);

    GTASCXMLFinal setParamFinal;
    setParamFinal.setId(finalStateId);
    SetState.setFinal(setParamFinal);

    setParamState.setDataModel(model);
    SetState.insertStates(setParamState);

    GTASCXMLTransition tx;
    tx.setTarget(iTargetId);
    tx.setStatus(TX_STATUS_OK);

    SetState.insertTransitions(tx);
    return SetState;
}

QHash<QString, QString> GTACMDSCXMLUtils::getGenToolRetData()
{
    return _genToolReturnMap;
}

void GTACMDSCXMLUtils::insertGenToolReturnParams(const QString &iId, const QString &iRetParam)
{
    if(!_genToolReturnMap.contains(iId))
        _genToolReturnMap.insert(iId,iRetParam);
}

QStringList GTACMDSCXMLUtils::getChannelSelectionSignals(const QString &iEngine)
{
    QHash<QString,QString> engineVarMAp;
    QStringList engineChannelSignals = GTAUtil::getChannelSelectionSignals(iEngine,engineVarMAp);
    return engineChannelSignals;
}

void GTACMDSCXMLUtils::logAllParameters(const QString &iIdentifier,const QList<GTAICDParameter> &iParamList,GTASCXMLState &ioState, bool isParamCSV)
{
    GTASCXMLOnExit onExit = ioState.getOnExit();
	if (!isParamCSV)//ioState.getInitId().endsWith("SetCmd"))
	{
		for (int i = 0; i < iParamList.count(); i++)
		{
			GTAICDParameter icdParam = iParamList.at(i);
			QString param = icdParam.getTempParamName().trimmed();
			if (param.isEmpty())
			{
				param = icdParam.getName().trimmed();
			}
			if (!param.isEmpty())
			{
				QString resolvedParam = param;
				updateWithGenToolParam(resolvedParam);
				if (iIdentifier.contains("line."))
				{
                    /// Moved if condition to comment (to Add more log information to scxml about the return.check) 
                   
					//if (param.contains("_line_"))
					//{
						GTASCXMLLog log = getLogMessage(GTACMDSCXMLUtils::INFO_LOG, iIdentifier, param, resolvedParam);
						onExit.insertLog(log);
					//}
				}
				else
				{
					GTASCXMLLog log = getLogMessage(GTACMDSCXMLUtils::INFO_LOG, iIdentifier, param, resolvedParam);
					onExit.insertLog(log);
				}
				//onExit.insertLog(log);
			}
		}
		ioState.setOnExit(onExit);
	}
	else 
	{
		for (int i = 0; i < iParamList.size(); i++)
		{
			if (i < 2) {
				GTAICDParameter icdParam = iParamList.at(i);
				QString param = icdParam.getTempParamName().trimmed();
                
				if (param.isEmpty())
				{
					param = icdParam.getName().trimmed();
				}
				//QString param = icdParam.getName().trimmed();
				if (!param.isEmpty())
				{
					if (param.startsWith("line")) {
						QString resolvedParam = param;
						updateWithGenToolParam(resolvedParam);
						GTASCXMLLog log = getLogMessage(GTACMDSCXMLUtils::INFO_LOG, iIdentifier, param, resolvedParam);
						QStringList logExpr = log.getExpression().split(":;:");
						QString id = ioState.getId();
						logExpr.replace(2, QString("ReturnedValue"));
						logExpr.replace(3, QString('+' + id + "_SetCmd_SetParam_0_Return" + '+'));
						log.setExpression(logExpr.join(":;:"));
						onExit.insertLog(log);
					} 
					else
					{
						QString resolvedParam = param;
						updateWithGenToolParam(resolvedParam);
						GTASCXMLLog log = getLogMessage(GTACMDSCXMLUtils::INFO_LOG, iIdentifier, param, resolvedParam);
						onExit.insertLog(log);
					}
				}
			} 
			else if (ioState.getInvokes().size() > 0)
			{
				GTAICDParameter icdParam = iParamList.at(i-1);
				//QString param = icdParam.getTempParamName().trimmed();
				QString param = icdParam.getName().trimmed();
				if (!param.isEmpty())
				{
					QString resolvedParam = param;
					updateWithGenToolParam(resolvedParam);
					GTASCXMLLog log = getLogMessage(GTACMDSCXMLUtils::INFO_LOG, iIdentifier, param, resolvedParam);
					QStringList logExpr = log.getExpression().split(":;:");
					QString id = ioState.getId();
					logExpr.replace(2, QString("ReturnedValue"));
					logExpr.replace(3, QString('+' + id + "_SetCmd_SetParam_0_Return" + '+' ));
					log.setExpression(logExpr.join(":;:"));
					onExit.insertLog(log);
				}
			}
		}
		ioState.setOnExit(onExit);
	}
}

/*void GTACMDSCXMLUtils::logPirParameters(const QString & iIdentifier, const QStringList & iLineList, GTASCXMLState & ioState)
{
	GTASCXMLOnExit onExit = ioState.getOnExit();
	QList<GTAICDParameter> lineICDParameter;
	foreach(QString line, iLineList)
	{
		if(line.split("__").size()>1)
			line = line.split("__").at(1);
		GTAICDParameter tempLineICD;
		tempLineICD.setSignalName(line);
		tempLineICD.setTempParamName(line);
		tempLineICD.setToolName("NA");
		tempLineICD.setToolType("INT_PARAM");
		QString resolvedParam = line;
		updateWithGenToolParam(resolvedParam);
		GTASCXMLLog log = getLogMessage(GTACMDSCXMLUtils::INFO_LOG, iIdentifier, line, resolvedParam);
		QStringList logExpr = log.getExpression().split('+');
		if (logExpr.size() == 3)
		{
			QString stateID = ioState.getId().split("ForEachStates").at(0);
			QString index = stateID + "arr_cntr";
			QString arr = stateID + "line_arr";
			logExpr.replace(1, arr + '[' + index + "]." + line);
			log.setExpression(logExpr.join('+'));
		}
		onExit.insertLog(log);
	}
	ioState.setOnExit(onExit);
}*/

/* This function allows to get Log Message according to the different actions. It is useful in the SCXML output file and the ResultView
* 
*/
GTASCXMLLog GTACMDSCXMLUtils::getLogMessage(LOG_TYPE iLogType,const QString &iIndentfier, const QString &iCurrentVal, const QString &iExpectedVal, bool isExpValParam)
{
    GTAScxmlLogMessage logMessage1;
	/*logMessage1.CurrentValue = iCurrentVal;
    logMessage1.ExpectedValue = iExpectedVal;*/
	logMessage1.CurrentValue = iExpectedVal;
	logMessage1.ExpectedValue = iCurrentVal;
	logMessage1.Identifier = iIndentfier;

    if(iLogType == GTACMDSCXMLUtils::SUCCESS_LOG)
    {
        logMessage1.Result = GTAScxmlLogMessage::OK;
        logMessage1.Status = GTAScxmlLogMessage::Success;
        logMessage1.LOD = GTAScxmlLogMessage::Main;
    }
    else if(iLogType == GTACMDSCXMLUtils::FAIL_LOG)
    {
        logMessage1.Result = GTAScxmlLogMessage::KO;
        logMessage1.Status = GTAScxmlLogMessage::Fail;
        logMessage1.LOD = GTAScxmlLogMessage::Main;
    }
    else if(iLogType == GTACMDSCXMLUtils::TIMEOUT_LOG)
    {
        logMessage1.Result = GTAScxmlLogMessage::KO;
        logMessage1.Status = GTAScxmlLogMessage::TimeOut;
        logMessage1.LOD = GTAScxmlLogMessage::Main;
        logMessage1.CurrentValue = "Timeout";
        logMessage1.ExpectedValue = "Timeout";
        isExpValParam =false;
    }
    else if(iLogType == GTACMDSCXMLUtils::INFO_LOG)
    {
        logMessage1.Result = GTAScxmlLogMessage::kNa;
        logMessage1.Status = GTAScxmlLogMessage::Success;
        logMessage1.LOD = GTAScxmlLogMessage::Detail;
    }
    else if(iLogType == GTACMDSCXMLUtils::KO_FAIL_LOG)
    {
        logMessage1.Result = GTAScxmlLogMessage::KO;
        logMessage1.Status = GTAScxmlLogMessage::Fail;
        logMessage1.LOD = GTAScxmlLogMessage::Main;
        logMessage1.CurrentValue = "KO";
        logMessage1.ExpectedValue = "KO";
        isExpValParam = false;
    }
    else
    {
        logMessage1.Result = GTAScxmlLogMessage::kNa;
        logMessage1.Status = GTAScxmlLogMessage::None;
        logMessage1.LOD = GTAScxmlLogMessage::Detail;
    }

    logMessage1.isCurrentValParam = false;
    QString log1 = logMessage1.getLogMessage();
    if(!isExpValParam)
    {
        log1 = log1.replace(":;:'+",":;:");
        log1 = log1.replace("+':;:",":;:");
        log1 = log1.replace("'","");
        log1 = QString("'%1'").arg(log1);
    }

    if(iLogType == GTACMDSCXMLUtils::TIMEOUT_LOG)
    {
        //log1= log1.replace("+","");
        log1= log1.replace("'","");
        log1 = QString("'%1'").arg(log1);
    }

    GTASCXMLLog log;
    log.setExpression(QString("%1").arg(log1));
    log.setLabel("Info");
    return log;
}

GTASCXMLState GTACMDSCXMLUtils::createUnSubscribeAll(const QList<GTAICDParameter> &iParams, const QString &iStateId, const QString &iTargetId,
    bool unsubscribeFileParamOnly)
{
    QHash<QString,QHash<QString, GTAICDParameter> > toolAppGroup;
    groupParameters(iParams, toolAppGroup);

    QList<QString> toolsTemp = toolAppGroup.keys();
    QStringList tools = QStringList(toolsTemp);
    tools.removeDuplicates();

    //local parameters cause additional state addition in SCXML which doesn't exist
    //removing tools with name NA to fix #Tuleap:258686
    tools.removeAll("NA");

    GTASCXMLState invokeState;
    invokeState.setId(iStateId);

    GTASCXMLTransition tx;
    tx.setTarget(iTargetId);
    invokeState.insertTransitions(tx);

    for(int i=0;i<tools.count();i++)
    {
        QString toolName = tools.at(i);

        if(toolName != "NA" && !(toolName.contains("instrum", Qt::CaseInsensitive) && unsubscribeFileParamOnly))
        {
            GTASCXMLState invokeParamState;

            QString currStateId = QString("%1_UnsubState%2").arg(iStateId,QString::number(i));
            QString currTargetId = QString("%1_UnsubState%2").arg(iStateId,QString::number(i+1));

            if(i>=(tools.count() -1))
                currTargetId  = QString("%1_final").arg(iStateId);

            QString invokeId = QString("%1_i0").arg(currStateId);
            GTASCXMLTransition successTx;
            GTASCXMLTransition FailTx;
            GTASCXMLOnEntry onEntry;
            GTASCXMLInvoke invoke;
			bool rc = getCloneInvoke("unsubscribeAll.scxml",invoke);

			if(rc)
			{
				invoke.setInvokeId(invokeId);

				GTASCXMLXMLRPC func = invoke.getXMLRPCFunction();

				GTASCXMLXMLRPC::funcReturn ret1;

				QString retName = QString("%1_Return").arg(currStateId);

				ret1.name = retName;

				func.updateReturnArgument(ret1);

				func.setToolId(toolName);

				invoke.setInvokeFunction(func);
				GTASCXMLAssign retAssign;

				retAssign.setExpression(QString("_event.data.%1").arg(retName));
				retAssign.setLocation(retName);

				invoke.insertAssign(retAssign);

				invokeParamState.insertInvokes(invoke);

				invokeParamState.setId(currStateId);
				invokeParamState.setOnEntry(onEntry);
				successTx.setTarget(currTargetId);
				FailTx.setTarget(currTargetId);
				FailTx.setStatus("KO");
				successTx.setStatus("OK");

				QString trueCond = QString("%1.Status == 'OK'").arg(retName);
				QString failCond = QString("%1.Status != 'OK'").arg(retName);
				successTx.setCondition(trueCond);
				FailTx.setCondition(failCond);

				FailTx.setEvent("done.invoke._all");
				successTx.setEvent("done.invoke._all");
				invokeParamState.insertTransitions(successTx);
				invokeParamState.insertTransitions(FailTx);
				invokeState.insertStates(invokeParamState);

				GTASCXMLData id;
				id.setDataId(retName);

				GTASCXMLDataModel model = invokeState.getDataModel();
				model.insertDataItem(id);
				invokeState.setDataModel(model);
			}
		}
        else if (toolName.contains("instrum", Qt::CaseInsensitive) && unsubscribeFileParamOnly)
        {
            createUnSubscribeProcedureParam(invokeState, iStateId, tools, i);
        }
    }
    return invokeState;
}

/**
 * @brief: This function creates xml-rpc function call for unsubscribing the file params
 * @invokeState: Used as reference to store the invoke object state
 * @iStateId: ID of the current state.
 * @tools: List of the external tools
 * @index: index of the tool from tools list for which the parameter unsubscribe function is being created 
*/
void GTACMDSCXMLUtils::createUnSubscribeProcedureParam(GTASCXMLState& invokeState, const QString& iStateId, const QStringList& tools, const int index)
{
	GTASCXMLState invokeParamState;

	QString currStateId = QString("%1_UnsubState%2").arg(iStateId, QString::number(index));
	QString currTargetId = QString("%1_UnsubState%2").arg(iStateId, QString::number(index + 1));

	if (index >= (tools.count() - 1))
		currTargetId = QString("%1_final").arg(iStateId);

	QString invokeId = QString("%1_i0").arg(currStateId);
	GTASCXMLTransition successTx;
	GTASCXMLTransition FailTx;
	GTASCXMLOnEntry onEntry;
	GTASCXMLInvoke invoke;

	bool rc = getCloneInvoke("UnsubscribeProcedureParam.scxml", invoke);

	if (rc)
	{
		invoke.setInvokeId(invokeId);

        QString toolName = tools.at(index);
		GTASCXMLXMLRPC func = invoke.getXMLRPCFunction();
        QString fileName = GTACMDSCXMLUtils::getExportingFileName();
		func.setToolId(toolName);
		GTASCXMLXMLRPC::funcArg arg;
		func.getArgumentByName("FileName", arg);
		arg.value = "[{'FileName' : '" + fileName + "'}]";
		func.updateArgumentByName("FileName", arg);

		GTASCXMLXMLRPC::funcReturn ret1;
		QString retName = QString("%1_Return").arg(currStateId);

		ret1.name = retName;

		func.updateReturnArgument(ret1);
		func.setToolId(toolName);

		invoke.setInvokeFunction(func);
		GTASCXMLAssign retAssign;

		retAssign.setExpression(QString("_event.data.%1").arg(retName));
		retAssign.setLocation(retName);

		invoke.insertAssign(retAssign);

		invokeParamState.insertInvokes(invoke);

		invokeParamState.setId(currStateId);
		invokeParamState.setOnEntry(onEntry);
		successTx.setTarget(currTargetId);
		FailTx.setTarget(currTargetId);
		FailTx.setStatus("KO");
		successTx.setStatus("OK");

		QString trueCond = QString("%1.Status == 'OK'").arg(retName);
		QString failCond = QString("%1.Status != 'OK'").arg(retName);
		successTx.setCondition(trueCond);
		FailTx.setCondition(failCond);

		FailTx.setEvent("done.invoke._all");
		successTx.setEvent("done.invoke._all");
		invokeParamState.insertTransitions(successTx);
		invokeParamState.insertTransitions(FailTx);
		invokeState.insertStates(invokeParamState);

		GTASCXMLData id;
		id.setDataId(retName);

		GTASCXMLDataModel model = invokeState.getDataModel();
		model.insertDataItem(id);
		invokeState.setDataModel(model);
	}
}

GTASCXMLState GTACMDSCXMLUtils::createSubscribeForApps(const QList<GTAICDParameter> &iParams, const QString &iStateId, const QString &iTargetId)
{
    QHash<QString,QHash<QString, GTAICDParameter> > toolAppGroup;
    groupParameters(iParams, toolAppGroup);

    QList<QString> toolsTemp = toolAppGroup.keys();
    QStringList tools = QStringList(toolsTemp);
    tools.removeDuplicates();

    //local parameters cause additional state addition in SCXML which doesn't exist
    //removing tools with name NA to fix #Tuleap:258686
    tools.removeAll("NA");

    GTASCXMLState invokeState;
    invokeState.setId(iStateId);

    int stateIdCounter = 0;
    QList<GTAICDParameter> pirParams;
    for(int i=0;i<tools.count();i++)
    {
        QString toolName = tools.at(i);
        if(toolName != "NA")
        {
            QHash<QString, GTAICDParameter> appNameParamList = toolAppGroup.value(toolName);

            QList<QString> appNamesTemp = appNameParamList.keys();
            QStringList appNames = QStringList(appNamesTemp);
            appNames.removeDuplicates();

            struct appDataPIR
            {
                appDataPIR()
                {
                    hasOnlyPIRMedia = true;
                }

                GTASCXMLDataModel model;
                bool hasOnlyPIRMedia;
            };
//            QMap<QString, GTASCXMLDataModel> appDataModelhash;
//            appDataModelhash.clear();
            QMap<QString, appDataPIR > appDataModelPIRhash;
            appDataModelPIRhash.clear();

            for(int j=0;j<appNames.count();j++)
            {
                QString appName = appNames.at(j);
                appDataPIR structVal;
                QList<GTAICDParameter> paramList = appNameParamList.values(appName);

                QHash<QString, QHash<QString, GTAICDParameter> > busMediaMap;
                for(int k=0;k<paramList.count();k++)
                {
                    GTAICDParameter param = paramList.at(k);
					param.setBusName(param.getName());
					QString busName = param.getBusName();
                    //QString media = param.getMedia();
					QString signalType = param.getSignalType();
					QString media = param.getMediaFromSignalType(signalType);
                    QString sourcetype = param.getSourceType();
                    QString paramType = param.getParameterType();
                    if(media == "PIR" || sourcetype == "PIR" || paramType == "FIB-IPR")
                    {
                        pirParams.append(param);
                        //continue;
                    }

                    if(!busMediaMap.contains(busName))
                    {
                        QHash<QString, GTAICDParameter> mediaMap;
                        mediaMap.insertMulti(media,param);
                        busMediaMap.insert(busName,mediaMap);
                    }
                    else
                    {
                        QHash<QString, GTAICDParameter> mediaMap = busMediaMap.value(busName);
                        mediaMap.insertMulti(media,param);
                        busMediaMap[busName] = mediaMap;
                    }
                }

//                if (busMediaMap.isEmpty())
//                    continue;

                QStringList buses;
                QStringList busList = busMediaMap.keys();
                for(int k=0;k<busList.count();k++)
                {
                    QString busName = busList.at(k);
                    QHash<QString, GTAICDParameter> mediaMap = busMediaMap.value(busName);
                    QList<QString> mediaList = mediaMap.keys();
                    QStringList medias(mediaList);
                    QStringList mediaNames;
                    medias.removeDuplicates();
                    for(int l=0;l<medias.count();l++)
                    {
                        QString media = medias.at(l);
                        if(!media.trimmed().isEmpty())
                            mediaNames.append(QString("'Media':'%1'").arg(media));

                        //capturing if all media is PIR
                        if (media != "PIR" && structVal.hasOnlyPIRMedia == true)
                            structVal.hasOnlyPIRMedia = false;

                    }
                    buses.append(QString("'%1':{%2}").arg(busName,mediaNames.join(",")));
                }

                QString dataId = QString("{'Name':'%1',%2}").arg(appName,buses.join(","));

                GTASCXMLDataModel model;
                GTASCXMLData id;
                id.setDataExpr(dataId);
                id.setDataId(appName.replace("-","_"));
                model.insertDataItem(id);
                for(int k=0;k<paramList.count();k++)
                {
                    GTAICDParameter param = paramList.at(k);
					QString signalType = param.getSignalType();
					QString media = param.getMediaFromSignalType(signalType);
                    //do not insert data item if the parameter is a PIR param
                    //PIR params have to be initialized and uninitialized during runtime
                    //if(param.getMedia() == "PIR" || param.getSourceType() == "PIR")
					if (param.getMediaFromSignalType(signalType) == "PIR" || param.getSourceType() == "PIR")
                    {
                        GTASCXMLData id1;
                        id1.setDataExpr(QString());
                        //id1.setDataId(param.getTempParamName());
						id1.setDataId(param.getName());
                        model.insertDataItem(id1);
                        //continue;
                    }
                    GTASCXMLData id1;
                    //for opaque parameters with Arinc label, force the paramtype to Float.
                    //without this the bridge causes an error and doesn't set an opaque parameter
                    //for AFDX value, keep it as opaque
                    //DEV: update this hardcoded opaques to relevant values once the bridge can handle string
                    id1.setDataExpr(QString("{'ParameterType':'%1'}").arg(param.getValueType().contains("opaque",Qt::CaseInsensitive)? (param.getSignalType().contains("ARINC",Qt::CaseInsensitive) ? "Float" : param.getValueType()): param.getValueType()));
                    //id1.setDataId(param.getTempParamName());
					id1.setDataId(param.getName());
                    model.insertDataItem(id1);
                }
                structVal.model = model;
//                appDataModelhash.insert(appName,model);
                appDataModelPIRhash.insert(appName,structVal);
            }
            for (int k=0;k<appDataModelPIRhash.keys().count();k++)
            {
                QString appName = appDataModelPIRhash.keys().at(k);
                appDataPIR structValue = appDataModelPIRhash.value(appName);
//                GTASCXMLDataModel model = appDataModelPIRhash.value(appName);
                GTASCXMLDataModel model = structValue.model;

                GTASCXMLState invokeParamState;

                QString currStateId = QString("%1_subAppState%2").arg(iStateId,QString::number(stateIdCounter++));
                QString currTargetId = QString("%1_subAppState%2").arg(iStateId,QString::number(stateIdCounter));

//                if((i>=(tools.count() -1)) && (j>= (appNames.count() - 1)))
//                    currTargetId  = QString("%1_final").arg(iStateId);
                if((i>=(tools.count() -1)) && (k>= (appDataModelPIRhash.keys().count() - 1)))
                    currTargetId  = QString("%1_final").arg(iStateId);

                QString invokeId = QString("%1_i0").arg(currStateId);
                GTASCXMLTransition successTx;
                GTASCXMLTransition FailTx;
                GTASCXMLOnEntry onEntry;
                GTASCXMLInvoke invoke;
                bool rc = getCloneInvoke("subscribeApp.scxml",invoke);

                if(rc)
                {
                    invoke.setInvokeId(invokeId);

                    GTASCXMLXMLRPC func = invoke.getXMLRPCFunction();

                    func.setToolId(toolName);
                    GTASCXMLXMLRPC::funcArg arg1;
                    GTASCXMLXMLRPC::funcReturn ret1;

                    func.getArgumentByName("ParameterList",arg1);

                    arg1.value = appName;

                    func.updateArgumentByName("ParameterList",arg1);

                    QString retName = QString("%1_Return").arg(currStateId);

                    ret1.name = retName;

                    func.updateReturnArgument(ret1);

                    invoke.setInvokeFunction(func);
                    GTASCXMLAssign retAssign;

                    retAssign.setExpression(QString("_event.data.%1").arg(retName));
                    retAssign.setLocation(retName);

                    invoke.insertAssign(retAssign);

                    GTASCXMLLog log;

                    log.setExpression(QString("'Subscribe:;:Following Parameters were not Subscribed:'+%1").arg(retName));
                    log.setLabel("Error");

                    if (!structValue.hasOnlyPIRMedia)
                        invokeParamState.insertInvokes(invoke);

                    invokeParamState.setDataModel(model);
                    invokeParamState.setId(currStateId);
                    invokeParamState.setOnEntry(onEntry);
                    successTx.setTarget(currTargetId);
                    FailTx.setTarget(SCXML_FINAL_STATE_ID);
                    FailTx.setStatus("KO");
                    FailTx.insertLog(log);
                    successTx.setStatus("OK");

                    QString trueCond = QString("%1.Status == 'OK'").arg(retName);
                    QString failCond = QString("%1.Status != 'OK'").arg(retName);

                    //skip the invoke if there are only PIR parameters
                    if (!structValue.hasOnlyPIRMedia)
                    {
                        successTx.setCondition(trueCond);
                        FailTx.setCondition(failCond);

                        FailTx.setEvent("done.invoke._all");
                        successTx.setEvent("done.invoke._all");
                        invokeParamState.insertTransitions(FailTx);
                    }
                    invokeParamState.insertTransitions(successTx);

                    GTASCXMLData id;
                    id.setDataId(retName);

                    GTASCXMLDataModel subModel = invokeParamState.getDataModel();
                    subModel.insertDataItem(id);
                    invokeParamState.setDataModel(subModel);
                    invokeState.insertStates(invokeParamState);
                }
            }
        }
    }

    if (!pirParams.isEmpty())
    {
        GTASCXMLDataModel datamodel;
        foreach (GTAICDParameter param, pirParams)
        {
            GTASCXMLData data;
            QString dataExpr = QString("{'Name':'%1','%2' : {'Media':'%3'}}").arg(param.getApplicationName(),param.getBusName(),param.getMedia());
            data.setDataExpr(dataExpr);
            data.setDataId(param.getApplicationName());
            datamodel.insertDataItem(data);
            QString parameterType = QString("{'ParameterType' : '%1'}").arg(param.getValueType());
            data.setDataExpr(parameterType);
            data.setDataId(param.getTempParamName());
			if (param.getTempParamName().isEmpty())
			{
				// BVU
				data.setDataId(param.getName());
			}
            datamodel.insertDataItem(data);
        }
        //invokeState.setDataModel(datamodel);
    }
    GTASCXMLTransition tx1;
    tx1.setTarget(iTargetId);
    tx1.setStatus("OK");
    invokeState.insertTransitions(tx1);
    return invokeState;
}

GTASCXMLState GTACMDSCXMLUtils::subscribeParameters(const QList<GTAICDParameter> &iParamList,const QString &iStateId, const QString &iTargetId, const QString &iIdentifier)
{
    QHash<QString,QHash<QString, GTAICDParameter> > toolAppGroup;
    groupParameters(iParamList, toolAppGroup);

    QList<QString> toolsTemp = toolAppGroup.keys();
    // coverted toolsTemp to QStringList only to use removeDuplicates
    QStringList tools = QStringList(toolsTemp);
    tools.removeDuplicates();

    //creating the invoke state inside subscribe state
    GTASCXMLState invokeState;
    invokeState.setId(iStateId);
    int stateIdCounter = 0;
    for(int i=0;i<tools.count();i++)
    {
        QString toolName = tools.at(i);

        if(toolName != "NA")
        {
            //appNameParamList have a map of appnamesand ICD params
            QHash<QString, GTAICDParameter> appNameParamList = toolAppGroup.value(toolName);

            QList<QString> appNamesTemp = appNameParamList.keys();
            QStringList appNames = QStringList(appNamesTemp);
            appNames.removeDuplicates();
            for(int j=0;j<appNames.count();j++)
            {
                QString appName = appNames.at(j);
                QList<GTAICDParameter> paramList = appNameParamList.values(appName);

                QHash<QString, QHash<QString, GTAICDParameter> > busMediaMap;
                //the below loop is run for every appname in toolname loop
                // this loop is run only to create a bus-media-param mapping
                for(int k=0;k<paramList.count();k++)
                {
                    GTAICDParameter param = paramList.at(k);
                    param.setBusName(param.getName());
                    QString busName = param.getBusName();
                    QString media = param.getMedia();
                    if(!busMediaMap.contains(busName))
                    {
                        QHash<QString, GTAICDParameter> mediaMap;
                        mediaMap.insertMulti(media,param);
                        busMediaMap.insert(busName,mediaMap);
                    }
                    else
                    {
                        QHash<QString, GTAICDParameter> mediaMap = busMediaMap.value(busName);
                        mediaMap.insertMulti(media,param);
                        busMediaMap[busName] = mediaMap;
                    }
                }

                QList<QString> busNamesTemp =busMediaMap.keys();
                QStringList busNames(busNamesTemp);
                busNames.removeDuplicates();
                //iteracting through busnames for each appname
                //note that for a single appname, all bus names are iterated
                for(int k=0;k<busNames.count();k++)
                {
                    QString busName = busNames.at(k);
                    QHash<QString, GTAICDParameter> mediaMap = busMediaMap.value(busName);
                    QList<QString> mediasTemp = mediaMap.keys();
                    QStringList medias(mediasTemp);
                    medias.removeDuplicates();
                    //a single bus name iterates through multiple media
                    for(int l=0;l<medias.count();l++)
                    {
                        QString media = medias.at(l);
                        QList<GTAICDParameter> params = mediaMap.values(media);

                        GTASCXMLState invokeParamState;

                        QString currStateId = QString("%1_subParamState%2").arg(iStateId,QString::number(stateIdCounter++));
                        QString currTargetId = QString("%1_subParamState%2").arg(iStateId,QString::number(stateIdCounter));

                        if((i>=(tools.count() -1)) && (j>= (appNames.count() - 1))&& (k>= (busNames.count() - 1))&& (l>= (medias.count() - 1)))
                            currTargetId  = QString("%1_final").arg(iStateId);

                        QString invokeId = QString("%1_i0").arg(currStateId);
                        QString paramArray = QString("%1_subParamList").arg(currStateId);
                        GTASCXMLTransition successTx;
                        GTASCXMLTransition FailTx;
                        GTASCXMLOnEntry onEntry;
                        GTASCXMLInvoke invoke;
                        bool rc = getCloneInvoke("subscribeParam.scxml",invoke);
                        if(rc)
                        {
                            invoke.setInvokeId(invokeId);
                            GTASCXMLAssign funcRet;
                            funcRet.setExpression(QString("_event.data.%1_Return").arg(invokeId));
                            funcRet.setLocation(paramArray);
                            invoke.insertAssign(funcRet);

                            GTASCXMLAssign paramMapAs;
                            paramMapAs.setLocation(paramArray);
                            QStringList signalList;
                            for(int m=0;m<params.count();m++)
                            {
                                GTAICDParameter param = params.at(m);
                                param.setSignalName(param.getName());
                                QString item = QString("'%1': {'ParameterType':'%2'}").arg(param.getSignalName(),param.getValueType());
                                signalList.append(item);
                            }
                            QString expr = QString("{ 'Name':'%1','%2':{'Media':'%3', %4}}").arg(appName,busName,media,signalList.join(","));
                            paramMapAs.setExpression(expr);
                            onEntry.insertAssign(paramMapAs);
                            GTASCXMLXMLRPC func = invoke.getXMLRPCFunction();
                            func.setToolId(toolName);
                            GTASCXMLXMLRPC::funcArg arg;
                            func.getArgumentByName("ParameterList",arg);
                            arg.value = paramArray;
                            func.updateArgumentByName("ParameterList",arg);
                            GTASCXMLXMLRPC::funcReturn ret;
                            func.getReturnArgument(ret);
                            ret.name = QString("%1_Return").arg(invokeId);
                            func.updateReturnArgument(ret);
                            invoke.setInvokeFunction(func);
                            invokeParamState.insertInvokes(invoke);

                            GTASCXMLLog successlog = getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG,iIdentifier,"true","true");
                            GTASCXMLLog Faillog = getLogMessage(GTACMDSCXMLUtils::FAIL_LOG,iIdentifier,"false","false");

                            successTx.setEvent(QString("done.invoke.%1").arg(invokeId));
                            successTx.setCondition(QString("%1.Status == 'OK'").arg(paramArray));

                            FailTx.setEvent(QString("done.invoke.%1").arg(invokeId));
                            FailTx.setCondition(QString("%1.Status != 'OK'").arg(paramArray));

                            successTx.insertLog(successlog);
                            FailTx.insertLog(Faillog);
                        }
                        invokeParamState.setId(currStateId);
                        invokeParamState.setOnEntry(onEntry);
                        successTx.setTarget(currTargetId);
                        FailTx.setTarget(currTargetId);
                        FailTx.setStatus("KO");
                        successTx.setStatus("OK");
                        invokeParamState.insertTransitions(successTx);
                        invokeParamState.insertTransitions(FailTx);
                        invokeState.insertStates(invokeParamState);
                    }
                }
            }
        }
    }
    GTASCXMLTransition tx1;
    tx1.setTarget(iTargetId);
    tx1.setStatus("OK");
    invokeState.insertTransitions(tx1);
    return invokeState;

}

GTASCXMLState GTACMDSCXMLUtils::unSubscribeParameters(const QList<GTAICDParameter> &iParamList,const QString &iStateId, const QString &iTargetId, const QString &iIdentifier)
{
    QHash<QString,QHash<QString, GTAICDParameter> > toolAppGroup;
    groupParameters(iParamList, toolAppGroup);

    QList<QString> toolsTemp = toolAppGroup.keys();

    QStringList tools = QStringList(toolsTemp);
    tools.removeDuplicates();
    GTASCXMLState invokeState;
    invokeState.setId(iStateId);
    int stateIdCounter=0;
    for(int i=0;i<tools.count();i++)
    {
        QString toolName = tools.at(i);

        if(toolName != "NA")
        {
            QHash<QString, GTAICDParameter> appNameParamList = toolAppGroup.value(toolName);

            QList<QString> appNamesTemp= appNameParamList.keys();
            QStringList appNames = QStringList(appNamesTemp);
            appNames.removeDuplicates();
            for(int j=0;j<appNames.count();j++)
            {
                QString appName = appNames.at(j);
                QList<GTAICDParameter> paramList = appNameParamList.values(appName);

                QHash<QString, QHash<QString, GTAICDParameter> > busMediaMap;
                for(int k=0;k<paramList.count();k++)
                {
                    GTAICDParameter param = paramList.at(k);
                    param.setBusName(param.getName());
                    QString busName = param.getBusName();
                    QString media = param.getMedia();
                    if(!busMediaMap.contains(busName))
                    {
                        QHash<QString, GTAICDParameter> mediaMap;
                        mediaMap.insertMulti(media,param);
                        busMediaMap.insert(busName,mediaMap);
                    }
                    else
                    {
                        QHash<QString, GTAICDParameter> mediaMap = busMediaMap.value(busName);
                        mediaMap.insertMulti(media,param);
                        busMediaMap[busName] = mediaMap;
                    }
                }
                QList<QString> busNamesTemp =busMediaMap.keys();
                QStringList busNames(busNamesTemp);
                busNames.removeDuplicates();
                for(int k=0;k<busNames.count();k++)
                {
                    QString busName = busNames.at(k);
                    if(!busName.trimmed().isEmpty())
                    {
                        QHash<QString, GTAICDParameter> mediaMap = busMediaMap.value(busName);
                        QList<QString> mediasTemp = mediaMap.keys();
                        QStringList medias(mediasTemp);
                        medias.removeDuplicates();
                        for(int l=0;l<medias.count();l++)
                        {
                            QString media = medias.at(l);
                            if(!media.trimmed().isEmpty())
                            {
                                QList<GTAICDParameter> params = mediaMap.values(media);

                                GTASCXMLState invokeParamState;

                                QString currStateId = QString("%1_subParamState%2").arg(iStateId,QString::number(stateIdCounter++));
                                QString currTargetId = QString("%1_subParamState%2").arg(iStateId,QString::number(stateIdCounter));

                                if((i>=(tools.count() -1)) && (j>= (appNames.count() - 1))&& (k>= (busNames.count() - 1))&& (l>= (medias.count() - 1)))
                                    currTargetId  = QString("%1_final").arg(iStateId);

                                QString invokeId = QString("%1_i0").arg(currStateId);
                                QString paramArray = QString("%1_subParamList").arg(currStateId);
                                GTASCXMLTransition successTx;
                                GTASCXMLTransition FailTx;
                                GTASCXMLOnEntry onEntry;
                                GTASCXMLInvoke invoke;
                                bool rc = getCloneInvoke("unSubscribeParam.scxml",invoke);
                                if(rc)
                                {
                                    invoke.setInvokeId(invokeId);
                                    GTASCXMLAssign funcRet;
                                    funcRet.setExpression(QString("_event.data.%1_Return").arg(invokeId));
                                    funcRet.setLocation(paramArray);
                                    invoke.insertAssign(funcRet);

                                    GTASCXMLAssign paramMapAs;
                                    paramMapAs.setLocation(paramArray);
                                    QStringList signalList;
                                    for(int m=0;m<params.count();m++)
                                    {
                                        GTAICDParameter param = params.at(m);
                                        param.setSignalName(param.getName());
                                        QString item = QString("'%1': {'ParameterType':'%2'}").arg(param.getSignalName(),param.getValueType());
                                        signalList.append(item);
                                    }
                                    QString expr = QString("{ 'Name':'%1','%2':{'Media':'%3', %4}}").arg(appName,busName,media,signalList.join(","));
                                    paramMapAs.setExpression(expr);
                                    onEntry.insertAssign(paramMapAs);
                                    GTASCXMLXMLRPC func = invoke.getXMLRPCFunction();
                                    func.setToolId(toolName);
                                    GTASCXMLXMLRPC::funcArg arg;
                                    func.getArgumentByName("ParameterList",arg);
                                    arg.value = paramArray;
                                    arg.name = "";
                                    func.updateArgumentByName("ParameterList",arg);
                                    GTASCXMLXMLRPC::funcReturn ret;
                                    func.getReturnArgument(ret);
                                    ret.name = QString("%1_Return").arg(invokeId);
                                    func.updateReturnArgument(ret);
                                    invoke.setInvokeFunction(func);
                                    invokeParamState.insertInvokes(invoke);

                                    GTASCXMLLog successlog = getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG,iIdentifier,"true","true");
                                    GTASCXMLLog Faillog = getLogMessage(GTACMDSCXMLUtils::FAIL_LOG,iIdentifier,"false","false");

                                    successTx.setEvent(QString("done.invoke.%1").arg(invokeId));
                                    successTx.setCondition(QString("%1.Status == 'OK'").arg(paramArray));

                                    FailTx.setEvent(QString("done.invoke.%1").arg(invokeId));
                                    FailTx.setCondition(QString("%1.Status != 'OK'").arg(paramArray));

                                    successTx.insertLog(successlog);
                                    FailTx.insertLog(Faillog);
                                }
                                invokeParamState.setId(currStateId);
                                invokeParamState.setOnEntry(onEntry);
                                successTx.setTarget(currTargetId);
                                FailTx.setTarget(currTargetId);
                                FailTx.setStatus("KO");
                                successTx.setStatus("OK");
                                invokeParamState.insertTransitions(successTx);
                                invokeParamState.insertTransitions(FailTx);
                                invokeState.insertStates(invokeParamState);
                            }

                        }

                    }
                }
            }
        }
    }
    GTASCXMLTransition tx1;
    tx1.setTarget(iTargetId);
    tx1.setStatus("OK");
    invokeState.insertTransitions(tx1);
    return invokeState;
}


GTASCXMLState GTACMDSCXMLUtils::releaseVMACParameters(const QList<GTAICDParameter> &iParamList,const QString &iStateId, const QString &iTargetId, const QString &InstanceName,const QString &iInvokeDelay)
{
    QHash<QString,QHash<QString, GTAICDParameter> > toolAppGroup;
    groupParameters(iParamList, toolAppGroup);

    QList<QString> toolsTemp = toolAppGroup.keys();
    QStringList tools = QStringList(toolsTemp);
    tools.removeDuplicates();

    GTASCXMLState invokeState;
    invokeState.setId(iStateId);
    int stateIdCounter = 0;

    //creating a default timeout
    GTASCXMLOnEntry onEntryTimeout;
    QString timeoutEvenName = QString("%1_timeout").arg(iStateId);
    QString timeoutEventID = QString("%1_timeoutID").arg(iStateId);
    GTASCXMLSend timeoutSend;
    if (!iInvokeDelay.isEmpty())
    {
        int delay = (iInvokeDelay.split("ms").first()).toInt();
        delay += 3000;
        timeoutSend.setDelay(QString("%1ms").arg(QString::number(delay)));
    }
    else
        timeoutSend.setDelay("3000ms");
    timeoutSend.setEvent(timeoutEvenName);
    timeoutSend.setId(timeoutEventID);
    onEntryTimeout.insertSend(timeoutSend);
    invokeState.setOnEntry(onEntryTimeout);

    GTASCXMLCancel cancel;
    cancel.setSendEventId(timeoutEventID);
    GTASCXMLOnExit onExit;
    onExit.insertCancel(cancel);
    invokeState.setOnExit(onExit);

    for(int i=0;i<tools.count();i++)
    {
        QString toolName = tools.at(i);

        if(toolName != "NA")
        {
            QHash<QString, GTAICDParameter> appNameParamList = toolAppGroup.value(toolName);

            QList<QString> appNamesTemp = appNameParamList.keys();
            QStringList appNames = QStringList(appNamesTemp);
            appNames.removeDuplicates();
            for(int j=0;j<appNames.count();j++)
            {
                QList<GTAICDParameter> paramList = appNameParamList.values(appNames.at(j));
                GTASCXMLState invokeParamState;

                QString currStateId = QString("%1_relParamState%2").arg(iStateId,QString::number(stateIdCounter++));
                QString currTargetId = QString("%1_relParamState%2").arg(iStateId,QString::number(stateIdCounter));

                if((i>=(tools.count() -1)) && (j>= (appNames.count() - 1)))
                    currTargetId  = QString("%1_final").arg(iStateId);

                QString invokeId = QString("%1_i0").arg(currStateId);
                QString paramArray = QString("%1_ReleaseParamList").arg(currStateId);
                GTASCXMLTransition SuccessTx;
                GTASCXMLTransition FailTx;
                GTASCXMLTransition TimeoutTx;
                GTASCXMLOnEntry onEntry;

//                QString timeoutEvenName = QString("%1_timeout").arg(currStateId);
//                GTASCXMLSend timeoutSend;
//                timeoutSend.setDelay("3000ms");
//                timeoutSend.setEvent(timeoutEvenName);
//                onEntry.insertSend(timeoutSend);

                QString returnValue = QString("%1_Return").arg(invokeId);

                //create a data model for the paramArray
                GTASCXMLData id;
                id.setDataId(returnValue);
                GTASCXMLDataModel model = invokeParamState.getDataModel();
                model.insertDataItem(id);
                id.setDataId(paramArray);
                id.setDataExpr("[]");
                model.insertDataItem(id);
                invokeParamState.setDataModel(model);                

                GTASCXMLInvoke invoke;
                bool rc = getCloneInvoke("releaseVMACParameters.scxml",invoke);
                if(rc)
                {
                    invoke.setInvokeId(invokeId);
                    GTASCXMLAssign funcRet;
                    funcRet.setExpression(QString("_event.data.%1_Return").arg(invokeId));
                    funcRet.setLocation(returnValue);
                    invoke.insertAssign(funcRet);
                    for(int k=0;k<paramList.count();k++)
                    {
                        GTAICDParameter param = paramList.at(k);
                        GTASCXMLAssign assign;
                        assign.setExpression(QString("'%1'").arg(param.getTempParamName()));
						if (param.getTempParamName().isEmpty())
						{
							assign.setExpression(QString("'%1'").arg(param.getName()));
						}

                        assign.setLocation(QString("%1[%2]").arg(paramArray,QString::number(k)));
                        onEntry.insertAssign(assign);
                    }
                    GTASCXMLAssign arrayClear;
                    arrayClear.setExpression("[]");
                    arrayClear.setLocation(paramArray);
                    invoke.insertAssign(arrayClear);
                    GTASCXMLXMLRPC func = invoke.getXMLRPCFunction();
                    func.setToolId(toolName);
                    GTASCXMLXMLRPC::funcArg arg;
                    func.getArgumentByName("ParameterID",arg);
                    arg.value = paramArray;
                    func.updateArgumentByName("ParameterID",arg);
                    GTASCXMLXMLRPC::funcReturn ret;
                    func.getReturnArgument(ret);
                    ret.name = returnValue;
                    func.updateReturnArgument(ret);
                    invoke.setInvokeFunction(func);
                    if(iInvokeDelay.trimmed().isEmpty())
                    {
                        SuccessTx.setEvent(QString("done.invoke.%1").arg(invokeId));
                        FailTx.setEvent(QString("done.invoke.%1").arg(invokeId));
                    }
                    else
                    {
                        GTASCXMLSend invokeDelaySend;
                        QString dealyEvent = QString("%1_dealyEvent").arg(invokeId);
                        invokeDelaySend.setDelay(iInvokeDelay);
                        invokeDelaySend.setEvent(dealyEvent);
                        invoke.insertSend(invokeDelaySend);

                        SuccessTx.setEvent(dealyEvent);
                        FailTx.setEvent(dealyEvent);
                    }
                    invokeParamState.insertInvokes(invoke);
                    SuccessTx.setCondition(QString("(%1.Status == 'OK')").arg(returnValue));
                    FailTx.setCondition(QString("(%1.Status != 'OK')").arg(returnValue));
                }
                invokeParamState.setId(currStateId);
                invokeParamState.setOnEntry(onEntry);
                SuccessTx.setTarget(currTargetId);
                FailTx.setTarget(currTargetId);

                SuccessTx.setStatus("OK");
                FailTx.setStatus("KO");

                GTASCXMLLog successLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG,InstanceName,"true","true");
                GTASCXMLLog failLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::FAIL_LOG,InstanceName,"false","false");
                GTASCXMLLog TimeoutLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::TIMEOUT_LOG,InstanceName,"","");

                SuccessTx.insertLog(successLog);
                FailTx.insertLog(failLog);

                TimeoutTx.setTarget(currTargetId);
                TimeoutTx.setEvent(timeoutEvenName);
                TimeoutTx.setStatus("TimeOut");
                TimeoutTx.insertLog(TimeoutLog);

                invokeParamState.insertTransitions(SuccessTx);
                invokeParamState.insertTransitions(FailTx);
                invokeParamState.insertTransitions(TimeoutTx);
                invokeState.insertStates(invokeParamState);
            }
        }
    }
    GTASCXMLTransition tx1;
    tx1.setTarget(iTargetId);
    tx1.setStatus("OK");
    invokeState.insertTransitions(tx1);
    return invokeState;
}


void GTACMDSCXMLUtils::updateWithGenToolParam(QString &ioParam)
{
    GTAUtil::updateWithGenToolParam(ioParam,_genToolReturnMap);
}

void GTACMDSCXMLUtils::updateWithGenToolParam(GTAICDParameter &ioParam)
{
    QString signalName = ioParam.getTempParamName();

    GTAUtil::updateWithGenToolParam(signalName,_genToolReturnMap);

    if(signalName != ioParam.getTempParamName())
    {
        ioParam.setTempParamName(signalName);
        ioParam.setSignalName(signalName);
    }
}

void GTACMDSCXMLUtils::insertVariablesInLogReport(const QList<GTAICDParameter> &iParamList,const QStringList iInternalParams,GTACMDSCXMLLog &ioLog)
{
    LogVariableInfo var1;
    QStringList varsDone;
    for(int i=0;i<iParamList.count();i++)
    {
        GTAICDParameter param = iParamList.at(i);
        var1._VarName = param.getTempParamName();
		//var1._VarName = param.getName();
        varsDone.append(var1._VarName);
        if(param.getToolName() != "NA")
            var1._VarType = "BD_PARAM";
        else
            var1._VarType = "LOCAL_PARAM";

        var1._expectedValue = param.getValue();
        ioLog.insertVariables(var1);
    }
    QStringList localVars = iInternalParams;
    localVars.removeDuplicates();
    for(int i=0;i<localVars.count();i++)
    {
        QString var = localVars.at(i);
        if((!var.endsWith(".Value")) && (!varsDone.contains(var)))
        {
            var1._VarName = var;
            var1._VarType = "INTERNAL";
            ioLog.insertVariables(var1);
        }
    }
}

/**
 * This function creates a post-debug state which handles response of the executed command to the debugger
 * @iStateId: ID of the current state.
 * @iTargetId: Target state to which the current state must transition.
 * @iStateVariables: Variables created within a state. All assigns inside a state.
 * @iparamList: User defined Parameters used in the command.
 * @return: GTASCXMLState object with proper attributes
*/
GTASCXMLState GTACMDSCXMLUtils::getDebugState(const QString &iStateId, const QString &iTargetId,
                                                    const QStringList &iStateVariables,
                                                    const QList<GTAICDParameter> &iparamList)
{
    GTASCXMLState state;
    state.setId(iStateId);

    GTASCXMLInvoke invoke;

    QString invokeId = QString("%1_debug").arg(iStateId);
    QString invokeReturnId = QString("%1_Return").arg(invokeId);
    //since this is a non-nested state, the invoke id is split by '_'
    //the last part is replaced by i0
    //this is done for generic scxml
    // a pretty lame thing to do but it can't be helped. Curses of our predecessors Dated: 10/04/2019
    //Hence the stupid workaround here. Check GTASCXMLState::getSCXML() [Line:385]
    QString invokeTxId = QString("%1_i0").arg(iStateId);


    //adding array in the data model
    GTASCXMLDataModel dataModel;
    GTASCXMLData data;
    QString arrayName = QString("%1_ResponseRequired").arg(iStateId);
    data.setDataExpr("[]");             //setting the array in data model
    data.setDataId(arrayName);
    dataModel.insertDataItem(data);
    data.setDataExpr("");               //setting the function return in data model
    data.setDataId(invokeReturnId);
    dataModel.insertDataItem(data);
    state.setDataModel(dataModel);

    //making a JSON for the OnEntry
    GTASCXMLOnEntry onEntry;
    GTASCXMLAssign assignArray;
    QString arrayExpr = "{'ResponseStatus':'OK' , 'ReturnResult' : ''}";
    QString arrayLoc = QString("%1[0]").arg(arrayName);
    assignArray.setExpression(arrayExpr);
    assignArray.setLocation(arrayLoc);
    onEntry.insertAssign(assignArray);
    arrayLoc = QString("%1[0].ReturnResult").arg(arrayName);


    //to be completed
    QHash<QString,QString> ReturnResultHash;
    int count = 0;
    QStringList usableVariables = iStateVariables;
    usableVariables.removeDuplicates();
    QStringList ReturnVariables = usableVariables;

    //ensuring the status of the command is fetched from the executable state
    foreach (QString stateVaraible, usableVariables)
    {
        //all commands are tweaked to have a *_Return assign for exec state status
        if (stateVaraible.contains("_Return",Qt::CaseInsensitive))
        {
            GTASCXMLAssign responseAssign;
            responseAssign.setExpression(QString("%1.Status").arg(stateVaraible));
            responseAssign.setLocation(QString("%1[0].ResponseStatus").arg(arrayName));
            onEntry.insertAssign(responseAssign);
            ReturnVariables.removeOne(stateVaraible);
            continue;
        }
    }


    //get all iStateVariables and iparamList in a single list and then create a map
    for (int j=0;j<iparamList.count();j++)
    {
        GTAICDParameter param = iparamList.at(j);
        if (param.getToolName() != "NA")
        {
            ReturnVariables.append(param.getTempParamName());
			if (param.getTempParamName().isEmpty())
			{
				// BVU
				ReturnVariables.append(param.getName());
			}
        }
    }

    //removing duplicates
    ReturnVariables.removeDuplicates();

    for (int i=0;i<ReturnVariables.count();i++)
    {
        QString stateVaraible = ReturnVariables.at(i);if (!stateVaraible.contains("SetParamList",Qt::CaseInsensitive) && !stateVaraible.contains("GetParamList",Qt::CaseInsensitive))
        {
            ReturnResultHash.insert(QString::number(count++),stateVaraible);
        }
    }

    if(!ReturnResultHash.isEmpty())
    {
        QString statement = "{ ";
        QStringList keys = ReturnResultHash.keys();
        for (int k=0;k<keys.count();k++)
        {
            QString value = ReturnResultHash.value(keys.at(k));
            value.replace(".","_");     //replacing triplets with underscores
            QString appdstatement = QString(" '%1' : '' ").arg(value);
            statement.append(appdstatement);
            if (k < keys.count() - 1)
            {
                statement.append(",");
            }
        }
        statement.append(" }");

        GTASCXMLAssign returnResult;
        returnResult.setExpression(statement);
        returnResult.setLocation(arrayLoc);
        onEntry.insertAssign(returnResult);

        foreach (QString key,ReturnResultHash.keys())
        {
            QString value = ReturnResultHash.value(key);
            returnResult.setLocation(QString("%1.%2").arg(arrayLoc,value.replace(".","_")));
            returnResult.setExpression(ReturnResultHash.value(key));
            onEntry.insertAssign(returnResult);
        }
    }
    //
    state.setOnEntry(onEntry);

    //creating an assign to receive the return
    GTASCXMLAssign funcRet;
    funcRet.setExpression(QString("_event.data.%1").arg(invokeReturnId));
    funcRet.setLocation(invokeReturnId);
    GTASCXMLXMLRPC::funcReturn funcReturn;
    funcReturn.name = invokeReturnId;

    //setting the invoke id and inserting assign
    invoke.setInvokeId(invokeId);
    invoke.setInvokeType("xml-rpc");
    //invoke.insertAssign(funcRet);

    //setting the xmlrpc function attributes and arguments
    GTASCXMLXMLRPC func = invoke.getXMLRPCFunction();
    func.setToolId("GTA_DEBUG");
    func.setFunctionName("RESULT_RESPONSE");

    GTASCXMLXMLRPC::funcArg arg;
    arg.name = "DEBUG";
    arg.type = "array";
    arg.value = arrayName;
    func.insertFunctionArgument(arg.type,arg.name,arg.value);
    //func.insertFunctionReturn(funcReturn.name,QString(""));

    invoke.setInvokeFunction(func);
    state.insertInvokes(invoke);

    //putting in transitions for next state
    GTASCXMLTransition nextState;
    GTASCXMLLog infoLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::INFO_LOG,"Debugging","true","true");

    nextState.setEvent(QString("done.invoke.%1").arg(invokeTxId));
    nextState.setStatus("Info");
    nextState.setTarget(iTargetId);
    nextState.insertLog(infoLog);

    state.insertTransitions(nextState);

    return state;
}

/**
 * This function creates a pre-debug state which handles next or previous state flow
 * @iStateId: ID of the current state.
 * @iTargetId: Target state to which the current state must transition.
 * @iPreviousStateId: Previous state to which the current state must transition.
 * @iInstanceName: Instance Name of the state to identify the line number
 * @return: GTASCXMLState object with proper attributes
*/
GTASCXMLState GTACMDSCXMLUtils::getPreDebugState(const QString &iStateId, const QString &iTargetId, const QString &iPreviousStateId, const QString &iInstanceName)
{
    //predebug
    GTASCXMLState state;
    state.setId(iStateId);

    GTASCXMLInvoke invoke;

    QString invokeId = QString("%1_debug").arg(iStateId);
    QString invokeReturnId = QString("%1_Return").arg(invokeId);
    //since this is a non-nested state, the invoke id is split by '_'
    //the last part is replaced by i0
    //this is done for generic scxml
    // a pretty lame thing to do but it can't be helped. Curses of our predecessors Dated: 10/04/2019
    //Hence the stupid workaround here. Check GTASCXMLState::getSCXML() [Line:385]
    QString invokeTxId = QString("%1_i0").arg(iStateId);

    //using a debug variable to be used later
    QString preDebugVar = QString("%1_preDebugVar").arg(iStateId);
    GTASCXMLDataModel dataModel;
    GTASCXMLData data;
    data.setDataId(preDebugVar);
    data.setDataExpr("");
    dataModel.insertDataItem(data);
    GTASCXMLAssign debugVar;
    debugVar.setExpression("'true'");
    debugVar.setLocation(preDebugVar);
    GTASCXMLOnEntry onEntry;
    onEntry.insertAssign(debugVar);     //assign the variable true in onEntry

    GTASCXMLAssign returnClear;
    returnClear.setExpression("");
    returnClear.setLocation(invokeReturnId);    
    onEntry.insertAssign(returnClear);
    state.setOnEntry(onEntry);          //insert onEntry into the state
    data.setDataId(invokeReturnId);
    dataModel.insertDataItem(data);
    state.setDataModel(dataModel);      //set the data model for the state

    //creating an assign to receive the return
    GTASCXMLAssign funcRet;
    funcRet.setExpression(QString("_event.data.%1").arg(invokeReturnId));
    funcRet.setLocation(invokeReturnId);
    GTASCXMLXMLRPC::funcReturn funcReturn;
    funcReturn.name = invokeReturnId;

    //setting the invoke id and inserting assign
    invoke.setInvokeId(invokeId);
    invoke.setInvokeType("xml-rpc");
    invoke.insertAssign(funcRet);

    //setting the xmlrpc function attributes and arguments
    GTASCXMLXMLRPC func = invoke.getXMLRPCFunction();
    func.setToolId("GTA_DEBUG");
    func.setFunctionName("BREAKPOINT");

    GTASCXMLXMLRPC::funcArg arg;
    arg.name = "DEBUG";
    arg.type = "string";
    QString instanceName = iInstanceName;
    QString value = getInstanceNameList(instanceName).join("/");
    arg.value = QString("'%1'").arg(value);
    func.insertFunctionArgument(arg.type,arg.name,arg.value);
    func.insertFunctionReturn(funcReturn.name,QString(""));

    invoke.setInvokeFunction(func);
    state.insertInvokes(invoke);

    //putting in transitions for next state or previous state
    GTASCXMLTransition nextState,previousState,continueState,stopState;

    QString txPrevious,txNext,txContinue,txStop;
    txPrevious = QString("( (%1.ResponseRequired == 'true') && (%1.UserInput == 'PREVIOUS') )").arg(invokeReturnId);
    txNext = QString("( (%1.ResponseRequired == 'true') && ( (%1.UserInput == 'NEXT') || (%1.UserInput == 'CONTINUE') ) )").arg(invokeReturnId);
    txContinue = QString("(%1.ResponseRequired == 'false')").arg(invokeReturnId);
    txStop = QString("( (%1.ResponseRequired == 'true') && (%1.UserInput == 'STOP') )").arg(invokeReturnId);

    GTASCXMLLog infoLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::INFO_LOG,"Debugging","true","true");
    QString event = QString("done.invoke.%1").arg(invokeTxId);

    nextState.setEvent(event);
    nextState.setCondition(txNext);
    nextState.setStatus("Next");
    nextState.setTarget(iTargetId);
    nextState.insertLog(infoLog);
    previousState.setEvent(event);
    previousState.setCondition(txPrevious);
    previousState.setStatus("Previous");
    previousState.setTarget(iPreviousStateId);
    previousState.insertLog(infoLog);
    continueState.setEvent(event);
    continueState.setCondition(txContinue);
    continueState.setStatus("Continue");
    continueState.setTarget(iTargetId);
    continueState.insertLog(infoLog);
    stopState.setEvent(event);
    stopState.setCondition(txStop);
    stopState.setStatus("Stop");
    stopState.setTarget(SCXML_MAIN_TP_FINAL);
    stopState.insertLog(infoLog);
    GTASCXMLAssign assign;
    assign.setLocation(preDebugVar);
    assign.setExpression(QString("%1.ResponseRequired").arg(invokeReturnId));
    continueState.insertAssign(assign);

    state.insertTransitions(continueState);
    state.insertTransitions(previousState);
    state.insertTransitions(nextState);
    state.insertTransitions(stopState);
    return state;
}

/**
 * This function identifies the line number from the command instance name
 * @iInstanceName: Instance name of the command.
 * @return: QStringlist of all line numbers in the instance name
*/
QStringList GTACMDSCXMLUtils::getInstanceNameList(QString &iInstanceName)
{
    QStringList instances = iInstanceName.split("#");
    QStringList ret;
    for (int i=0;i<instances.count() - 1;i++)
    {
        QString instance = instances.at(i);
        instance.replace("call","").replace(" - ","").replace("object","",Qt::CaseInsensitive).replace("function","",Qt::CaseInsensitive).replace("procedure","",Qt::CaseInsensitive);
        QStringList fullPath = instance.split("/");QStringList actualPath = instance.split("/");
        for (int j=0;j<fullPath.count();j++)
        {
            QString path = fullPath.at(j);
            if (!path.contains("[") && !path.contains("]"))
                actualPath.removeOne(path);
        }
        for (int k=0;k<actualPath.size();k++)
        {
            QString entry = actualPath.at(k);
            int indexstart = entry.indexOf("[");
            entry.remove(0,indexstart);
            actualPath.replace(k,entry);
        }

        ret.append(actualPath);
    }
    if (ret.isEmpty())
        ret.append(iInstanceName);
    return ret;
}
