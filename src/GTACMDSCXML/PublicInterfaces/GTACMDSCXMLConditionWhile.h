#ifndef GTACMDSCXMLCONDITIONWHILE_H
#define GTACMDSCXMLCONDITIONWHILE_H


#include "GTACMDSCXMLBase.h"
#include "GTACMDSCXML.h"
#include "GTAActionWhile.h"

class GTACMDSCXML_SHARED_EXPORT GTACMDSCXMLConditionWhile : public GTACMDSCXMLBase
{
public:
    GTACMDSCXMLConditionWhile();
    GTACMDSCXMLConditionWhile(GTAActionWhile *pCmdNode, const QString &iStateId, const QString &iTargetId);
    ~GTACMDSCXMLConditionWhile();

    virtual QString getSCXMLString() const;
    virtual GTASCXMLState getSCXMLState()const;
    GTASCXMLState getDecisionState()const;
    GTASCXMLState getParameterState()const;
    void setLoopTimeoutInInternalStates(bool iIncludeLoopTimeout = false){_IncludeLoopTimeoutInSubStates = iIncludeLoopTimeout;}

private:
    GTAActionWhile *_WhileCmd;
    bool _IncludeLoopTimeoutInSubStates;
private:
};

#endif // GTACMDSCXMLCONDITIONWHILE_H
