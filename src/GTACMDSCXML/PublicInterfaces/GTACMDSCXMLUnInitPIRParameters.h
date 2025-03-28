#ifndef GTACMDSCXMLUNINITPIRPARAMETERS_H
#define GTACMDSCXMLUNINITPIRPARAMETERS_H


#include "GTACMDSCXMLBase.h"
#include "GTACMDSCXML.h"
#include "GTAActionUnSubscribe.h"
#include "GTAActionForEach.h"

class GTACMDSCXML_SHARED_EXPORT GTACMDSCXMLUnInitPIRParameters : public GTACMDSCXMLBase
{
public:
    GTACMDSCXMLUnInitPIRParameters();
    GTACMDSCXMLUnInitPIRParameters(GTAActionUnSubscribe *pCmdNode, const QString &iStateId, const QString &iTargetId);
    ~GTACMDSCXMLUnInitPIRParameters();

    virtual QString getSCXMLString() const;
    virtual GTASCXMLState getSCXMLState()const;


private:
    GTAActionUnSubscribe *_UnSubCmd;

private:
    GTASCXMLState getSCXMLForParameterUnSubs(QList<GTAICDParameter> &iParamList) const;
};

#endif // GTACMDSCXMLUNINITPIRPARAMETERS_H
