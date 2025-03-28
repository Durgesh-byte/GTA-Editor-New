#ifndef GTACMDSCXMLINITPIRPARAMETERS_H
#define GTACMDSCXMLINITPIRPARAMETERS_H


#include "GTACMDSCXMLBase.h"
#include "GTACMDSCXML.h"
#include "GTAActionSubscribe.h"
#include "GTAActionForEach.h"

class GTACMDSCXML_SHARED_EXPORT GTACMDSCXMLInitPIRParameters : public GTACMDSCXMLBase
{
    
public:
    
    GTACMDSCXMLInitPIRParameters();
    GTACMDSCXMLInitPIRParameters(GTAActionSubscribe *pCmdNode, const QString &iStateId, const QString &iTargetId);
    ~GTACMDSCXMLInitPIRParameters();

    virtual QString getSCXMLString() const;
    virtual GTASCXMLState getSCXMLState()const;


private:
    GTAActionSubscribe* _SubCmd;

private:
    GTASCXMLState getSCXMLForParameterSubs(QList<GTAICDParameter> &iParamList) const;
};

#endif // GTACMDSCXMLINITPIRPARAMETERS_H
