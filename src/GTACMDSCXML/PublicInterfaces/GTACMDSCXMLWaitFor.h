#ifndef GTACMDSCXMLWAITFOR_H
#define GTACMDSCXMLWAITFOR_H


#include "GTACMDSCXMLBase.h"
#include "GTACMDSCXML.h"
#include "GTAActionWait.h"

class GTACMDSCXML_SHARED_EXPORT GTACMDSCXMLWaitFor : public GTACMDSCXMLBase
{
public:
    GTACMDSCXMLWaitFor();
    GTACMDSCXMLWaitFor(GTAActionWait *pCmdNode, const QString &iStateId, const QString &iTargetId);
    ~GTACMDSCXMLWaitFor();

    virtual QString getSCXMLString() const;
    virtual GTASCXMLState getSCXMLState()const;


private:
    GTAActionWait *_WaitCmd;

private:
    GTASCXMLState getSCXMLForWaitForCommand(const QString iStateId,const QString iTargetId,const QString &iFinalId=QString(),const QString &iDecisionVar=QString()) const;
};

#endif // GTACMDSCXMLWAITFOR_H
