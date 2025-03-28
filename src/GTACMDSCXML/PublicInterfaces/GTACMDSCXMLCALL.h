#ifndef GTACMDSCXMLCALL_H
#define GTACMDSCXMLCALL_H


#include "GTACMDSCXMLBase.h"
#include "GTACMDSCXML.h"
#include "GTAActionExpandedCall.h"

class GTACMDSCXML_SHARED_EXPORT GTACMDSCXMLCALL : public GTACMDSCXMLBase
{
public:
    GTACMDSCXMLCALL();
    GTACMDSCXMLCALL(GTAActionExpandedCall *pCmdNode, const QString &iStateId, const QString &iTargetId);
    virtual ~GTACMDSCXMLCALL();

    virtual QString getSCXMLString() const;
    virtual GTASCXMLState getSCXMLState()const;


private:
    GTAActionExpandedCall *_CallCmd;

private:
    GTASCXMLState getSCXMLCALL() const;
};

#endif // GTACMDSCXMLCALL_H
