#include "GTACMDSCXMLConditionElse.h"
#include "GTASCXMLState.h"
#include "GTAUtil.h"
#include "GTACMDSCXMLLog.h"
#include "GTACMDSCXMLUtils.h"

GTACMDSCXMLConditionElse::GTACMDSCXMLConditionElse(GTAActionElse *pCmdNode, const QString &iStateId, const QString &iTargetId):
        GTACMDSCXMLBase(pCmdNode,iStateId,iTargetId)
{
    _ElseCmd = pCmdNode;
}

GTACMDSCXMLConditionElse::GTACMDSCXMLConditionElse()
{

}

GTACMDSCXMLConditionElse::~GTACMDSCXMLConditionElse()
{

}
QString GTACMDSCXMLConditionElse::getSCXMLString() const
{
    return getSCXMLState().getSCXMLString(_isUSCXMLEn);
}

GTASCXMLState GTACMDSCXMLConditionElse::getSCXMLState()const
{
    GTASCXMLState ElseState;
    ElseState.setId(_StateId);


    for(int i=0;i<_Children.count();i++)
    {
        GTACMDSCXMLBase* pCmd = _Children.at(i);
        if(pCmd != NULL)
            ElseState.insertStates(pCmd->getSCXMLState());
    }

    GTASCXMLTransition tx;
    tx.setStatus("OK");
    tx.setTarget(_TargetId);

    ElseState.insertTransitions(tx);

    return ElseState;
}

