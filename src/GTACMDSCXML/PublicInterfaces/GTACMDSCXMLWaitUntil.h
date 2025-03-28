#ifndef GTACMDSCXMLWAITUNTIL_H
#define GTACMDSCXMLWAITUNTIL_H


#include "GTACMDSCXMLBase.h"
#include "GTACMDSCXML.h"
#include "GTAActionWait.h"


class GTACMDSCXML_SHARED_EXPORT GTACMDSCXMLWaitUntil : public GTACMDSCXMLBase
{
public:
    GTACMDSCXMLWaitUntil();
    GTACMDSCXMLWaitUntil(GTAActionWait *pCmdNode, const QString &iStateId, const QString &iTargetId);
    ~GTACMDSCXMLWaitUntil();

    virtual QString getSCXMLString() const;
    virtual GTASCXMLState getSCXMLState()const;


private:
    GTAActionWait *_WaitUntilCmd;

private:
    GTASCXMLState getSCXMLWaitUntil() const;
    GTASCXMLState getParamState(const QString &iStateId, const QString &iTargetId) const;
    GTASCXMLState evalutateConditionAndTxState(const QString &iStateId, const QString &iTargetId, const QString &iLoopBackStateId, bool isConfTimeEn = false, bool ParamStateHasInvokes = false) const;
    QString getExecStateID() const;
    QString getExecTargetID() const;
    QString getFinalTargetID() const;
    QString getPreDebugStateID() const;
    QString getPreDebugVariable() const;
};

#endif // GTACMDSCXMLWAITUNTIL_H
