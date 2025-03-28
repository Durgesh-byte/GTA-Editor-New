#ifndef GTACMDSCXMLCONDITIONELSE_H
#define GTACMDSCXMLCONDITIONELSE_H


#include "GTACMDSCXMLBase.h"
#include "GTACMDSCXML.h"
#include "GTAActionElse.h"

class GTACMDSCXML_SHARED_EXPORT GTACMDSCXMLConditionElse : public GTACMDSCXMLBase
{
public:
    GTACMDSCXMLConditionElse();
    GTACMDSCXMLConditionElse(GTAActionElse *pCmdNode, const QString &iStateId, const QString &iTargetId);
    ~GTACMDSCXMLConditionElse();

    virtual QString getSCXMLString() const;
    virtual GTASCXMLState getSCXMLState()const;

private:
    GTAActionElse *_ElseCmd;

private:
};

#endif // GTACMDSCXMLCONDITIONELSE_H
