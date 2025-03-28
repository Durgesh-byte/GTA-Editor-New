#include "GTACMDSCXMLPrintTime.h"
#include "GTASCXMLState.h"
#include "GTAUtil.h"
#include "GTACMDSCXMLLog.h"
#include "GTACMDSCXMLUtils.h"

GTACMDSCXMLPrintTime::GTACMDSCXMLPrintTime(GTAActionPrintTime *pCmdNode, const QString &iStateId, const QString &iTargetId):
        GTACMDSCXMLBase(pCmdNode,iStateId,iTargetId)
{
    _PrintCmd = pCmdNode;
}

GTACMDSCXMLPrintTime::GTACMDSCXMLPrintTime()
{

}

GTACMDSCXMLPrintTime::~GTACMDSCXMLPrintTime()
{

}
QString GTACMDSCXMLPrintTime::getSCXMLString() const
{
    return getSCXMLState().getSCXMLString(_isUSCXMLEn);
}

GTASCXMLState GTACMDSCXMLPrintTime::getSCXMLState()const
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
        if(_PrintCmd->getComplement() == COM_PRINT_TIME)
        {
            state = getSCXMLForPrintTime(executableStateID,executableTargetID,finaltargetId,preDebugVar);
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



GTASCXMLState GTACMDSCXMLPrintTime::getSCXMLForPrintTime(const QString iStateId,const QString iTargetId,
                                                               const QString &iFinalId,
                                                               const QString &iDecisionVar) const
{
    QString instanceName = _PrintCmd->getInstanceName();
    QString commandName = QString("%1_%2").arg(_PrintCmd->getAction(),_PrintCmd->getComplement());

    GTAActionPrintTime::TimeType eTimetime = _PrintCmd->getTimeType();
    QString timeExpr= "";
    if(eTimetime == GTAActionPrintTime::Formatted)
        timeExpr = "Date()";
    else
        timeExpr = "Date.now()";

    GTASCXMLAssign dateTimeParam;
    dateTimeParam.setExpression(timeExpr);
    dateTimeParam.setLocation("_dateTime");

    GTASCXMLOnEntry onEntry;
    onEntry.insertAssign(dateTimeParam);

    GTASCXMLState printTimeState;
    printTimeState.setOnEntry(onEntry);

    printTimeState.setId(iStateId);

    GTAScxmlLogMessage logMsg;
    logMsg.Identifier = _PrintCmd->getInstanceName();
    logMsg.Result = GTAScxmlLogMessage::OK;
    logMsg.Status = GTAScxmlLogMessage::Success;
    logMsg.ExpectedValue = "Time";
    logMsg.CurrentValue = "_dateTime";
    logMsg.LOD = GTAScxmlLogMessage::Main;
    logMsg.isCurrentValParam = true;
    QString sLogMsg = logMsg.getLogMessage();
    GTASCXMLLog log1;
    log1.setExpression(sLogMsg);
    log1.setLabel("'Info'");

    GTASCXMLTransition tx;
    tx.setTarget(iTargetId);
    if (!iDecisionVar.isEmpty())
    {
        tx.setCondition(QString("(%1 == 'true')").arg(iDecisionVar));

        //additional tx
        GTASCXMLTransition tx1;
        tx1.setTarget(iFinalId);
        tx1.setCondition(QString("(%1 != 'true')").arg(iDecisionVar));
        tx1.insertLog(log1);
        printTimeState.insertTransitions(tx1);
    }

    tx.insertLog(log1);
    printTimeState.insertTransitions(tx);


    GTASCXMLOnExit onExit;

    GTACMDSCXMLLog log;
    log.setReportType(GTACMDSCXMLLog::REP_ACTION,(GTACMDSCXMLLog::REPORT_SUB_TYPE)0);
    log.setCommandName(commandName);
    log.setCommandInstanceId(instanceName);
    log.setCommandDescription(_PrintCmd->getComment());
    log.setCommandStatement(_PrintCmd->getStatement());
    log.insertRequirements(_PrintCmd->getRequirements());


    LogVariableInfo var1;

    var1._VarName = QString("_dateTime");
    var1._expectedValue = timeExpr;
    var1._VarType = "INTERNAL";

    log.insertVariables(var1);

    if(_GenNewLogFormat)
    onExit.insertExtraInfo(log.getLogElem());

    printTimeState.setOnExit(onExit);

    GTASCXMLData data;
    data.setDataExpr(var1._expectedValue);
    data.setDataId(var1._VarName);

    GTASCXMLDataModel datamodel;
    datamodel.insertDataItem(data);

    printTimeState.setDataModel(datamodel);

    return printTimeState;
}
