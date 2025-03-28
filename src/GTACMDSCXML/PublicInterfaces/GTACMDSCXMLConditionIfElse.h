#ifndef GTACMDSCXMLCONDITIONIFELSE_H
#define GTACMDSCXMLCONDITIONIFELSE_H


#include "GTACMDSCXMLBase.h"
#include "GTACMDSCXML.h"
#include "GTAActionIF.h"
#include "GTAActionElse.h"
#include "GTAActionForEach.h"

class GTACMDSCXML_SHARED_EXPORT GTACMDSCXMLConditionIfElse : public GTACMDSCXMLBase
{
public:
    GTACMDSCXMLConditionIfElse();
    GTACMDSCXMLConditionIfElse(GTAActionIF *pCmdNode, const QString &iStateId, const QString &iTargetId);
    ~GTACMDSCXMLConditionIfElse();

    virtual QString getSCXMLString() const;
    virtual GTASCXMLState getSCXMLState()const;
    GTASCXMLState getIfCmdState() const;
    GTASCXMLState getElseCmdState() const;
    GTASCXMLState getParameterState() const;
    GTASCXMLState getDecisionState() const;
private:
    GTAActionIF *_IfCmd;
private:
};

#endif // GTACMDSCXMLCONDITIONIFELSE_H
