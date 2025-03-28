#ifndef GTACMDSCXMLRELEASEVMACPARAMETERS_H
#define GTACMDSCXMLRELEASEVMACPARAMETERS_H


#include "GTACMDSCXMLBase.h"
#include "GTACMDSCXML.h"
#include "GTAActionRelease.h"

class GTACMDSCXML_SHARED_EXPORT GTACMDSCXMLReleaseVMACParameters : public GTACMDSCXMLBase
{
public:
    GTACMDSCXMLReleaseVMACParameters();
    GTACMDSCXMLReleaseVMACParameters(GTAActionRelease *pCmdNode, const QString &iStateId, const QString &iTargetId);
    ~GTACMDSCXMLReleaseVMACParameters();

    virtual QString getSCXMLString() const;
    virtual GTASCXMLState getSCXMLState()const;


private:
    GTAActionRelease *_ReleaseCmd;

private:
    GTASCXMLState getSCXMLForReleaseParameters() const;
};

#endif // GTACMDSCXMLReleaseVMACParameters_H
