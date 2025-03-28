#include "GTACMDSCXMLReleaseVMACParameters.h"
#include "GTASCXMLState.h"
#include "GTAUtil.h"
#include "GTACMDSCXMLLog.h"
#include "GTACMDSCXMLUtils.h"

GTACMDSCXMLReleaseVMACParameters::GTACMDSCXMLReleaseVMACParameters(GTAActionRelease *pCmdNode, const QString &iStateId, const QString &iTargetId):
        GTACMDSCXMLBase(pCmdNode,iStateId,iTargetId)
{
    _ReleaseCmd = pCmdNode;
}

GTACMDSCXMLReleaseVMACParameters::GTACMDSCXMLReleaseVMACParameters()
{

}

GTACMDSCXMLReleaseVMACParameters::~GTACMDSCXMLReleaseVMACParameters()
{

}
QString GTACMDSCXMLReleaseVMACParameters::getSCXMLString() const
{
    return getSCXMLState().getSCXMLString(_isUSCXMLEn);
}

GTASCXMLState GTACMDSCXMLReleaseVMACParameters::getSCXMLState()const
{
    GTASCXMLState state;
    if(_ReleaseCmd != NULL)
    {
        state = getSCXMLForReleaseParameters();
        //innsert logs
        GTACMDSCXMLLog log;
        log.setReportType(GTACMDSCXMLLog::REP_ACTION,GTACMDSCXMLLog::REP_ACTCHK);
        log.setCommandName(_CmdName);
        log.setCommandInstanceId(_ReleaseCmd->getInstanceName());
        log.setCommandDescription(_ReleaseCmd->getComment());
        log.setCommandStatement(_ReleaseCmd->getStatement());
        QStringList requirements = _ReleaseCmd->getRequirements();
        log.insertRequirements(requirements);
        QStringList varList;// = state.getVariableList();
        GTACMDSCXMLUtils::insertVariablesInLogReport(_ParamInfoList,varList,log);

        GTASCXMLOnExit onExit = state.getOnExit();
        if(_GenNewLogFormat)
        onExit.insertExtraInfo(log.getLogElem());
        state.setOnExit(onExit);
        GTACMDSCXMLUtils::logAllParameters(_ReleaseCmd->getInstanceName(),_ParamInfoList,state);
    }
    return state;
}

GTASCXMLState GTACMDSCXMLReleaseVMACParameters::getSCXMLForReleaseParameters() const
{
    GTASCXMLState RelVMACParamState;

    RelVMACParamState = GTACMDSCXMLUtils::releaseVMACParameters(_ParamInfoList,_StateId,_TargetId,_ReleaseCmd->getInstanceName(),_HiddenDelay);

    return RelVMACParamState;
}
