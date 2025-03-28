#include "GTACMDSCXMLCallParallel.h"
#include "GTASCXMLState.h"
#include "GTAUtil.h"
#include "GTACMDSCXMLLog.h"
#include "GTACMDSCXMLUtils.h"

GTACMDSCXMLCallParallel::GTACMDSCXMLCallParallel()
{
}

GTACMDSCXMLCallParallel::GTACMDSCXMLCallParallel(GTAActionCallProcedures *pCmdNode, const QString &iStateId, const QString &iTargetId) :
        GTACMDSCXMLBase(pCmdNode,iStateId,iTargetId)
{
    _CallParallel = pCmdNode;
}

GTACMDSCXMLCallParallel::~GTACMDSCXMLCallParallel()
{

}

QString GTACMDSCXMLCallParallel::getSCXMLString() const
{
    return getSCXMLState().getSCXMLString(_isUSCXMLEn);
}

GTASCXMLState GTACMDSCXMLCallParallel::getSCXMLState()const
{
    GTASCXMLState parallelState;
    parallelState.setType(GTASCXMLState::PARALLEL);
    parallelState.setId(_StateId);

    GTASCXMLState exitState;
    QString exitStateId = QString("%1_exit").arg(_StateId);
    exitState.setId(exitStateId);
    GTASCXMLTransition tx;
    QString txEvent;

    //inserting states within while according to relevant action/check
    for(int i=0;i<_Children.count();i++)
    {
        GTACMDSCXMLBase* pCmd = _Children.at(i);
        if(pCmd != NULL)
        {
            GTASCXMLState tempState = pCmd->getSCXMLState();
            QString stateid = pCmd->getStateId();

            txEvent = QString("%1 done.state.%2").arg(txEvent,stateid);

            //remove the additional transition before the final
            QList<GTASCXMLTransition> tXs = tempState.getTransitions();
            for (const auto& tmpTx : tXs)
            {
                if (tmpTx.getStatus() == "OK" && tmpTx.getEvent().isEmpty())
                    tempState.removeTransition(tmpTx);
            }

            //appending the states inside the loop
            parallelState.insertStates(tempState);
        }
    }
    tx.setEvent(txEvent);
    tx.setTarget(_TargetId);
    GTASCXMLLog log = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG,_CallParallel->getInstanceName(),"true","true");
    tx.insertLog(log);
    exitState.insertTransitions(tx);

    parallelState.insertStates(exitState);

    GTASCXMLTransition exitTx;
    exitTx.setTarget(_TargetId);
//    QString parallelEvent = QString("done.state.%1").arg(_StateId);
//    exitTx.setEvent(parallelEvent);
//    exitTx.setEvent(txEvent);
    exitTx.insertLog(log);
    parallelState.insertTransitions(exitTx);

    return parallelState;
}
