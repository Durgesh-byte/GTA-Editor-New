#include "GTACMDSCXMLContainerLoop.h"
#include "GTASCXMLState.h"
#include "GTAUtil.h"
#include "GTACMDSCXMLLog.h"
#include "GTACMDSCXMLUtils.h"

GTACMDSCXMLContainerLoop::GTACMDSCXMLContainerLoop(GTACommandBase *pCmdNode, const QString &iStateId, const QString &iTargetId):
        GTACMDSCXMLBase(pCmdNode,iStateId,iTargetId)
{
    _LoopCmd = pCmdNode;
}

GTACMDSCXMLContainerLoop::GTACMDSCXMLContainerLoop()
{

}

GTACMDSCXMLContainerLoop::~GTACMDSCXMLContainerLoop()
{

}
QString GTACMDSCXMLContainerLoop::getSCXMLString() const
{
    return getSCXMLState().getSCXMLString(_isUSCXMLEn);
}

GTASCXMLState GTACMDSCXMLContainerLoop::getSCXMLState()const
{
    GTASCXMLState loopState;
    loopState.setId(_StateId);

    for(int i=0;i<_Children.count();i++)
    {
        GTACMDSCXMLBase* pCmd = _Children.at(i);
        if(pCmd != NULL)
            loopState.insertStates(pCmd->getSCXMLState());
    }

    GTASCXMLTransition tx;
    tx.setStatus("OK");
    tx.setTarget(_TargetId);

    loopState.insertTransitions(tx);

    return loopState;
}

