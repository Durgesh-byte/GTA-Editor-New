#ifndef GTACMDSCXMLCONDITIONDOWHILE_H
#define GTACMDSCXMLCONDITIONDOWHILE_H


#include "GTACMDSCXMLBase.h"
#include "GTACMDSCXML.h"
#include "GTAActionDoWhile.h"

class GTACMDSCXML_SHARED_EXPORT GTACMDSCXMLConditionDoWhile : public GTACMDSCXMLBase
{
public:
    GTACMDSCXMLConditionDoWhile();
    GTACMDSCXMLConditionDoWhile(GTAActionDoWhile *pCmdNode, const QString &iStateId, const QString &iTargetId);
    ~GTACMDSCXMLConditionDoWhile();

    virtual QString getSCXMLString() const;
    virtual GTASCXMLState getSCXMLState()const;
    GTASCXMLState getDecisionState()const;
    GTASCXMLState getParameterState()const;
    void setLoopTimeoutInInternalStates(bool iIncludeLoopTimeout = false){_IncludeLoopTimeoutInSubStates = iIncludeLoopTimeout;}
private:
    GTAActionDoWhile *_DoWhileCmd;
    bool _IncludeLoopTimeoutInSubStates;

private:
};

#endif // GTACMDSCXMLCONDITIONDOWHILE_H
