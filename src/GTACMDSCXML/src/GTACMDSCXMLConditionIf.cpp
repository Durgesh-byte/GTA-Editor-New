#include "GTACMDSCXMLConditionIf.h"
#include "GTASCXMLState.h"
#include "GTAUtil.h"
#include "GTACMDSCXMLLog.h"
#include "GTACMDSCXMLUtils.h"

GTACMDSCXMLConditionIf::GTACMDSCXMLConditionIf(GTAActionIF *pCmdNode, const QString &iStateId, const QString &iTargetId):
        GTACMDSCXMLBase(pCmdNode,iStateId,iTargetId)
{
    _IfCmd = pCmdNode;
}

GTACMDSCXMLConditionIf::GTACMDSCXMLConditionIf()
{

}

GTACMDSCXMLConditionIf::~GTACMDSCXMLConditionIf()
{

}
QString GTACMDSCXMLConditionIf::getSCXMLString() const
{
    return getSCXMLState().getSCXMLString(_isUSCXMLEn);
}

GTASCXMLState GTACMDSCXMLConditionIf::getSCXMLState()const
{
    GTASCXMLState IfState;
    IfState.setId(_StateId);

    for(int i=0;i<_Children.count();i++)
    {
        GTACMDSCXMLBase* pCmd = _Children.at(i);
        if(pCmd != NULL)
            IfState.insertStates(pCmd->getSCXMLState());
    }

    GTASCXMLTransition tx;
    tx.setStatus("OK");
    tx.setTarget(_TargetId);

    IfState.insertTransitions(tx);

    return IfState;
}

