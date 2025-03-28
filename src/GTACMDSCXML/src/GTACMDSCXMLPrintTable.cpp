#include "GTACMDSCXMLPrintTable.h"
#include "GTASCXMLState.h"
#include "GTAUtil.h"
#include "GTACMDSCXMLLog.h"
#include "GTACMDSCXMLUtils.h"

GTACMDSCXMLPrintTable::GTACMDSCXMLPrintTable(GTAActionPrintTable *pCmdNode, const QString &iStateId, const QString &iTargetId):
        GTACMDSCXMLBase(pCmdNode,iStateId,iTargetId)
{
    _PrintCmd = pCmdNode;
}

GTACMDSCXMLPrintTable::GTACMDSCXMLPrintTable()
{

}

GTACMDSCXMLPrintTable::~GTACMDSCXMLPrintTable()
{

}
QString GTACMDSCXMLPrintTable::getSCXMLString() const
{
    return getSCXMLState().getSCXMLString(_isUSCXMLEn);
}

GTASCXMLState GTACMDSCXMLPrintTable::getSCXMLState()const
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
        if(_PrintCmd->getComplement() == COM_PRINT_TABLE)
        {
            state = getSCXMLForPrintParams(executableStateID,executableTargetID,finaltargetId,preDebugVar);
            GTACMDSCXMLUtils::logAllParameters(_PrintCmd->getInstanceName(),_ParamInfoList,state);
        }
        else
            return state;

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
//    return state;
}
GTASCXMLState GTACMDSCXMLPrintTable::getSCXMLForPrintParams(const QString iStateId,const QString iTargetId,
                                                                  const QString &iFinalId,
                                                                  const QString &iDecisionVar) const
{
    QString instanceName = _PrintCmd->getInstanceName();
    QString commandName = QString("%1_%2").arg(_PrintCmd->getAction(),_PrintCmd->getComplement());

    GTASCXMLState printParamState;

    printParamState = GTACMDSCXMLUtils::createGetParamInvokeState(_ParamInfoList,iStateId,iTargetId,QString(),false,_PrintCmd->getInstanceName(),false,_HiddenDelay);

    GTACMDSCXMLLog log;
    log.setReportType(GTACMDSCXMLLog::REP_ACTION,(GTACMDSCXMLLog::REPORT_SUB_TYPE)0);
    log.setCommandName(commandName);
    log.setCommandInstanceId(instanceName);
    log.setCommandDescription(_PrintCmd->getComment());
    log.setCommandStatement(_PrintCmd->getStatement());
    log.insertRequirements(_PrintCmd->getRequirements());
    log.insertExtraInfo("print_table",_PrintCmd->getTableName());
    log.insertExtraInfo("print_table_title",_PrintCmd->getTitleName());
    for(int i=0;i<_ParamInfoList.count();i++)
    {

        GTAICDParameter param = _ParamInfoList.at(i);


        LogVariableInfo var1;

        var1._VarName = param.getTempParamName();
        var1._expectedValue = "";
        if(!param.getApplicationName().isEmpty())
            var1._VarType = "DATABASE";
        else
            var1._VarType = "LOCAL";

        log.insertVariables(var1);

    }
    if (!iDecisionVar.isEmpty())
    {
        GTASCXMLTransition ExistingTx = printParamState.getTransitions().at(0);
        GTASCXMLTransition NewTx;
        NewTx.setCondition(QString("(%1 == 'true')").arg(iDecisionVar));
        NewTx.setStatus(ExistingTx.getStatus());
        NewTx.setTarget(ExistingTx.getTarget());

        printParamState.removeTransition(ExistingTx);
        printParamState.insertTransitions(NewTx);

        //additional tx with different target and condition
        NewTx.setCondition(QString("(%1 != 'true')").arg(iDecisionVar));
        NewTx.setTarget(iFinalId);
        printParamState.insertTransitions(NewTx);
    }
    GTASCXMLLog log1 = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG,_PrintCmd->getInstanceName(),QString("true"),QString("true"));
    GTASCXMLOnExit onExit;
    onExit.insertLog(log1);
    if(_GenNewLogFormat)
        onExit.insertExtraInfo(log.getLogElem());

    printParamState.setOnExit(onExit);
    return printParamState;
}
