#include "GTACMDSCXMLMaths.h"
#include "GTASCXMLState.h"
#include "GTAUtil.h"
#include "GTACMDSCXMLLog.h"
#include "GTACMDSCXMLUtils.h"

GTACMDSCXMLMaths::GTACMDSCXMLMaths(GTAActionMaths *pCmdNode, const QString &iStateId, const QString &iTargetId):
        GTACMDSCXMLBase(pCmdNode,iStateId,iTargetId)
{
    _mathCmd = pCmdNode;
}

GTACMDSCXMLMaths::GTACMDSCXMLMaths()
{
}

GTACMDSCXMLMaths::~GTACMDSCXMLMaths()
{

}
QString GTACMDSCXMLMaths::getSCXMLString() const
{
    return getSCXMLState().getSCXMLString(_isUSCXMLEn);
}

GTASCXMLState GTACMDSCXMLMaths::getSCXMLState()const
{
    GTASCXMLState mathsState;
    if (_mathCmd == NULL)
        return mathsState;

    // mathsState state ID set
    mathsState.setId(_StateId);

    // obtaining lhs value and rhs expressions
    QString expr = _mathCmd->getSCXMLExpr();
    QString lhs = _mathCmd->getFirstVariable();

    QString getParamStateId = QString("%1_MathsGetParams").arg(_StateId);
    QString setParamStateId = QString("%1_MathsSetParam").arg(_StateId);
    QString finalStateId = QString("%1_final").arg(_StateId);

    //identifying ICD/local parameter in rhs
    GTAICDParameter lhsParam;
    QList<GTAICDParameter> getParameters;
    // _ParamInfoList provides list of all params loaded with GTASCXMLBase currently
    for(int i=0;i<_ParamInfoList.count();i++)
    {
        GTAICDParameter param = _ParamInfoList.at(i);
		QString paramName;
		if (param.getTempParamName().isEmpty())
			paramName = param.getName();
		else
			paramName = param.getTempParamName();
        if(expr.contains(paramName))
        {
            if(param.getToolName() != "NA")
            {
                getParameters.append(param);
                expr.replace(paramName,QString("%1.Value").arg(paramName));
            }
        }
        
		if(lhs == paramName)
        {
            lhsParam = param;
        }
    }


    QString rhs = QString("%1_LocMathsExpr").arg(_StateId);

    //creating a local rhs parameter
    GTAICDParameter rhsParam;
    rhsParam.setSignalName(rhs);
    rhsParam.setTempParamName(rhs);
    rhsParam.setToolName("NA");

    // calling a getparam to get all rhs ICD parameters
    GTASCXMLState getParamState = GTACMDSCXMLUtils::createGetParamInvokeState(getParameters,getParamStateId,setParamStateId,"",false,_mathCmd->getInstanceName(),false,_HiddenDelay);
    // calling a setparam to set lhs param
    GTASCXMLState setParamState = GTACMDSCXMLUtils::createSetParamInvokeState(lhsParam,rhsParam,setParamStateId,finalStateId,false,/*DEV_COMMENT: to be changed to QString() once changes to xml-rpc server has been made*/QString(ACT_FSSSM_NO),_mathCmd->getInstanceName(),_HiddenDelay);



    // assigning the getparam for rhs to a value on exit from the state
    GTASCXMLOnExit getParamExit = getParamState.getOnExit();

    GTASCXMLAssign assign;
    assign.setExpression(expr);
    assign.setLocation(rhs);
    getParamExit.insertAssign(assign);
    getParamState.setOnExit(getParamExit);

    //adding a timeout to getparamState if any get operations have been done
	QString timeoutEventName = QString("%1_timeout").arg(_StateId);
    GTASCXMLLog timeoutLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::TIMEOUT_LOG,_mathCmd->getInstanceName(),"","",false);
    GTASCXMLTransition timeoutTx;
    timeoutTx.setEvent(timeoutEventName);
    timeoutTx.setStatus(TX_STATUS_TIMEOUT);
    timeoutTx.setTarget(finalStateId);
    timeoutTx.insertLog(timeoutLog);

    getParamState.insertTransitions(timeoutTx,true);

    // assigning the data model for mathsState
    GTASCXMLData data;
    data.setDataExpr("");
    data.setDataId(rhs);

    GTASCXMLDataModel datamodel;
    datamodel.insertDataItem(data);

    mathsState.setDataModel(datamodel);

    // inserting get and set states in math state
    mathsState.insertStates(getParamState);
    mathsState.insertStates(setParamState);


    GTASCXMLTransition tx;
    tx.setTarget(_TargetId);
    tx.setStatus(TX_STATUS_OK);

    // inserting transitions for math state
    mathsState.insertTransitions(tx);

    //forcing a timeout of 3s in mathState
    // the idea is that the set and get complete within 3s
    GTASCXMLSend send;
    GTASCXMLOnEntry onEntry = mathsState.getOnEntry();
    send.setEvent(timeoutEventName);
    send.setDelay(QString("3000")+"ms");
    onEntry.insertSend(send);
    mathsState.setOnEntry(onEntry);

    //adding a timeout to get
    //QString timeoutEventGet = QString("%1_timeout").arg(getParamStateId);
    GTASCXMLTransition timeoutTxGet;
    timeoutTxGet.setEvent(timeoutEventName);
    timeoutTxGet.setStatus(SCXML_STATUS_TIMEOUT);
    timeoutTxGet.setTarget(finalStateId);
    GTASCXMLLog timeoutGetLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::TIMEOUT_LOG,_mathCmd->getInstanceName(),"","");
    timeoutTxGet.insertLog(timeoutGetLog);
    getParamState.insertTransitions(timeoutTxGet,true);

    //adding a timeout to set
    //QString timeoutEventSet = QString("%1_timeout").arg(setParamStateId);
    GTASCXMLTransition timeoutTxSet;
    timeoutTxSet.setEvent(timeoutEventName);
    timeoutTxSet.setStatus(SCXML_STATUS_TIMEOUT);
    timeoutTxSet.setTarget(finalStateId);
    GTASCXMLLog timeoutSetLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::TIMEOUT_LOG,_mathCmd->getInstanceName(),"","");
    timeoutTxSet.insertLog(timeoutSetLog);
    setParamState.insertTransitions(timeoutTxSet,true);

    //insert logs
    GTACMDSCXMLLog log;
    log.setReportType(GTACMDSCXMLLog::REP_ACTION,GTACMDSCXMLLog::REP_ACTCHK);
    log.setCommandName(_CmdName);
    log.setCommandInstanceId(_mathCmd->getInstanceName());
    log.setCommandDescription(_mathCmd->getComment());
    log.setCommandStatement(_mathCmd->getStatement());
    QStringList requirements = _mathCmd->getRequirements();
    log.insertRequirements(requirements);
    QStringList varList;
    GTACMDSCXMLUtils::insertVariablesInLogReport(_ParamInfoList,varList,log);

    GTASCXMLOnExit onExit = mathsState.getOnExit();

	if(_GenNewLogFormat)
		onExit.insertExtraInfo(log.getLogElem());


    mathsState.setOnExit(onExit);
    GTACMDSCXMLUtils::logAllParameters(_mathCmd->getInstanceName(),_ParamInfoList,mathsState);

    return mathsState;


//    the suggested state mentioned
//    <state id = mathsState>
//                <state id=getParamState>
//                          <transition to setParamState>
//                          </state>
//                <state id=setParamState>
//                          <transition to final state of mathsState>
//                          </state>
//     </state>


}

