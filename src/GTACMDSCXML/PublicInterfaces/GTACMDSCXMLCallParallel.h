#ifndef GTACMDSCXMLCALLPARALLEL_H
#define GTACMDSCXMLCALLPARALLEL_H

#include "GTACMDSCXMLBase.h"
#include "GTACMDSCXML.h"
#include "GTAActionExpandedCall.h"
#include "GTAActionCallProcedures.h"

class GTACMDSCXML_SHARED_EXPORT GTACMDSCXMLCallParallel: public GTACMDSCXMLBase
{
public:
    GTACMDSCXMLCallParallel();
    GTACMDSCXMLCallParallel(GTAActionCallProcedures *pCmdNode, const QString &iStateId, const QString &iTargetId);
    ~GTACMDSCXMLCallParallel();

    virtual QString getSCXMLString() const;
    virtual GTASCXMLState getSCXMLState()const;


private:
    GTAActionCallProcedures *_CallParallel;

private:
    GTASCXMLState getSCXMLCALL() const;
};

#endif // GTACMDSCXMLCALLPARALLEL_H
