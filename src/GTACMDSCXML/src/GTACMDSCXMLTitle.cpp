#include "GTACMDSCXMLTitle.h"
#include "GTASCXMLState.h"
#include "GTACMDSCXMLLog.h"
#include "GTACMDSCXMLUtils.h"

GTACMDSCXMLTitle::GTACMDSCXMLTitle(GTAActionParentTitle *pCmdNode, const QString &iStateId, const QString &iTargetId)
    :GTACMDSCXMLBase(pCmdNode,iStateId,iTargetId)
{
    _TitleCmd = pCmdNode;
}
GTACMDSCXMLTitle::GTACMDSCXMLTitle()
{

}

GTACMDSCXMLTitle::~GTACMDSCXMLTitle()
{

}
GTASCXMLState GTACMDSCXMLTitle::getSCXMLState()const
{
    GTASCXMLState titleState;

    if(_TitleCmd != NULL)
    {
        titleState.setId(_StateId);

        for(int i=0;i<_Children.count();i++)
        {
            GTACMDSCXMLBase* pCmd = _Children.at(i);
            if(pCmd != NULL)
                titleState.insertStates(pCmd->getSCXMLState());
        }

        GTASCXMLTransition tx;
        tx.setTarget(_TargetId);

        GTASCXMLLog successLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG,_TitleCmd->getInstanceName(),"","");

        tx.insertLog(successLog);

        titleState.insertTransitions(tx);

        //innsert logs
        GTACMDSCXMLLog log;
        log.setReportType(GTACMDSCXMLLog::REP_TITLE,GTACMDSCXMLLog::REP_ACTCHK);
        log.setCommandName(_CmdName);
        log.setCommandInstanceId(_TitleCmd->getInstanceName());
        log.setCommandDescription(_TitleCmd->getComment());
        log.setCommandStatement(_TitleCmd->getStatement());
        QStringList requirements = _TitleCmd->getRequirements();
        log.insertRequirements(requirements);
        QStringList varList;// = titleState.getVariableList();
        GTACMDSCXMLUtils::insertVariablesInLogReport(_ParamInfoList,varList,log);

        GTASCXMLOnExit onExit = titleState.getOnExit();
        if(_GenNewLogFormat)
            onExit.insertExtraInfo(log.getLogElem());
        titleState.setOnExit(onExit);
        GTACMDSCXMLUtils::logAllParameters(_TitleCmd->getInstanceName(),_ParamInfoList,titleState);
    }
    return titleState;
}
QString GTACMDSCXMLTitle::getSCXMLString() const
{
    return getSCXMLState().getSCXMLString(_isUSCXMLEn);
}
