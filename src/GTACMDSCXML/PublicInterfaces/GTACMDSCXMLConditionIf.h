#ifndef GTACMDSCXMLCONDITIONIF_H
#define GTACMDSCXMLCONDITIONIF_H


#include "GTACMDSCXMLBase.h"
#include "GTACMDSCXML.h"
#include "GTAActionIF.h"

class GTACMDSCXML_SHARED_EXPORT GTACMDSCXMLConditionIf : public GTACMDSCXMLBase
{
public:
    GTACMDSCXMLConditionIf();
    GTACMDSCXMLConditionIf(GTAActionIF *pCmdNode, const QString &iStateId, const QString &iTargetId);
    ~GTACMDSCXMLConditionIf();

    virtual QString getSCXMLString() const;
    virtual GTASCXMLState getSCXMLState()const;


private:
    GTAActionIF *_IfCmd;

private:
};

#endif // GTACMDSCXMLCONDITIONIF_H
