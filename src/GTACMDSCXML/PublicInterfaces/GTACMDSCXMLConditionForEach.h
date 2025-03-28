#ifndef GTACMDSCXMLConditionForEach_H
#define GTACMDSCXMLConditionForEach_H


#include "GTACMDSCXMLBase.h"
#include "GTACMDSCXML.h"
#include "GTAActionForEach.h"

class GTACMDSCXML_SHARED_EXPORT GTACMDSCXMLConditionForEach : public GTACMDSCXMLBase
{
public:
    GTACMDSCXMLConditionForEach();
    GTACMDSCXMLConditionForEach(GTAActionForEach *pCmdNode, const QString &iStateId, const QString &iTargetId);
    ~GTACMDSCXMLConditionForEach();

    virtual QString getSCXMLString() const;
    virtual GTASCXMLState getSCXMLState()const;
    GTASCXMLState getDecisionState()const;
    GTASCXMLState getParameterState()const;
    void setLoopTimeoutInInternalStates(bool iIncludeLoopTimeout = false){_IncludeLoopTimeoutInSubStates = iIncludeLoopTimeout;}

private:
    GTAActionForEach *_ForEachCmd;
    bool _IncludeLoopTimeoutInSubStates;
private:
};

#endif // GTACMDSCXMLConditionForEach_H
