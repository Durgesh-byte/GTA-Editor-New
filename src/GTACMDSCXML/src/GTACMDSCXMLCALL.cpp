#include "GTACMDSCXMLCALL.h"
#include "GTASCXMLState.h"
#include "GTAUtil.h"
#include "GTACMDSCXMLLog.h"
#include "GTACMDSCXMLUtils.h"

GTACMDSCXMLCALL::GTACMDSCXMLCALL(GTAActionExpandedCall *pCmdNode, const QString &iStateId, const QString &iTargetId) :
        GTACMDSCXMLBase(pCmdNode,iStateId,iTargetId)
{
    _CallCmd = pCmdNode;
}

GTACMDSCXMLCALL::GTACMDSCXMLCALL()
{
    
}

GTACMDSCXMLCALL::~GTACMDSCXMLCALL()
{

}

QString GTACMDSCXMLCALL::getSCXMLString() const
{
    return getSCXMLState().getSCXMLString(_isUSCXMLEn);
}

GTASCXMLState GTACMDSCXMLCALL::getSCXMLState()const
{
    GTASCXMLState state;
    if(_CallCmd != NULL)
    {
        state = getSCXMLCALL();

        QString elemName = _CallCmd->getElement();

        //innsert logs
        GTACMDSCXMLLog log;
        if(elemName.endsWith(".pro"))
            log.setReportType(GTACMDSCXMLLog::REP_CALL_CMD,GTACMDSCXMLLog::REP_CALL_CMD_PRO);
        else if(elemName.endsWith(".pro"))
            log.setReportType(GTACMDSCXMLLog::REP_CALL_CMD,GTACMDSCXMLLog::REP_CALL_CMD_FUN);
        else if(elemName.endsWith(".obj"))
            log.setReportType(GTACMDSCXMLLog::REP_CALL_CMD,GTACMDSCXMLLog::REP_CALL_CMD_OBJ);

        log.setCommandName(_CmdName);
        log.setCommandInstanceId(_CallCmd->getInstanceName());
        log.setCommandDescription(_CallCmd->getComment());
        log.setCommandStatement(_CallCmd->getStatement());
        QStringList requirements = _CallCmd->getRequirements();
        log.insertRequirements(requirements);
        QStringList varList = state.getVariableList();
        GTACMDSCXMLUtils::insertVariablesInLogReport(_ParamInfoList,varList,log);

        if(_GenNewLogFormat)
        {
            GTASCXMLOnExit onExit = state.getOnExit();
            onExit.insertExtraInfo(log.getLogElem());
            state.setOnExit(onExit);
        }

        GTACMDSCXMLUtils::logAllParameters(_CallCmd->getInstanceName(),_ParamInfoList,state);
    }

    return state;
}

GTASCXMLState GTACMDSCXMLCALL::getSCXMLCALL() const
{
    QString complement = _CallCmd->getComplement();

    if (complement == COM_CALL_MON)
    {
        GTASCXMLState parallelState;
        parallelState.setType(GTASCXMLState::PARALLEL);
        parallelState.setId(_StateId);

        GTASCXMLState exitState;
        QString exitStateId = QString("%1_exit").arg(_StateId);
        exitState.setId(exitStateId);
        GTASCXMLTransition tx;
        QString txEvent;

        for(int i=0;i<_Children.count();i++)
        {
            GTACMDSCXMLBase* pCmd = _Children.at(i);
            if(pCmd != NULL)
            {
                GTASCXMLState tempState = pCmd->getSCXMLState();
                QString stateid = pCmd->getStateId();

                txEvent = QString("%1 done.state.%2").arg(txEvent,stateid);

                QList<GTASCXMLTransition> tXs = tempState.getTransitions();
                for (const auto& tmpTx : tXs)
                {
                    if (tmpTx.getStatus() == "OK" && tmpTx.getEvent().isEmpty())
                        tempState.removeTransition(tmpTx);
                }
				parallelState.insertStates(tempState);
            }
        }
        tx.setEvent(txEvent);
        tx.setTarget(_TargetId);
        GTASCXMLLog log = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG,_CallCmd->getInstanceName(),"true","true");
        tx.insertLog(log);
        exitState.insertTransitions(tx);

        parallelState.insertStates(exitState);

        GTASCXMLTransition exitTx;
        exitTx.setTarget(_TargetId);
        exitTx.insertLog(log);
        parallelState.insertTransitions(exitTx);

        return parallelState;
    }
    GTASCXMLState callState;
    callState.setId(_StateId);

    for(int i=0;i<_Children.count();i++)
    {
        GTACMDSCXMLBase* pCmd = _Children.at(i);
        if(pCmd != NULL)
            callState.insertStates(pCmd->getSCXMLState());
    }

    GTASCXMLTransition tx;
    tx.setTarget(_TargetId);
    tx.setStatus("OK");

    callState.insertTransitions(tx);
    return callState;
}
