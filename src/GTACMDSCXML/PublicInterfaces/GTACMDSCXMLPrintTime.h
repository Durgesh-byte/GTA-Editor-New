#ifndef GTACMDSCXMLPRINTTIME_H
#define GTACMDSCXMLPRINTTIME_H


#include "GTACMDSCXMLBase.h"
#include "GTACMDSCXML.h"
#include "GTAActionPrintTime.h"

class GTACMDSCXML_SHARED_EXPORT GTACMDSCXMLPrintTime : public GTACMDSCXMLBase
{
public:
    GTACMDSCXMLPrintTime();
    GTACMDSCXMLPrintTime(GTAActionPrintTime *pCmdNode, const QString &iStateId, const QString &iTargetId);
    ~GTACMDSCXMLPrintTime();

    virtual QString getSCXMLString() const;
    virtual GTASCXMLState getSCXMLState()const;


private:
    GTAActionPrintTime *_PrintCmd;

private:
    GTASCXMLState getSCXMLForPrintTime(const QString iStateId,const QString iTargetId,const QString &iFinalId=QString(),const QString &iDecisionVar=QString()) const;
};

#endif // GTACMDSCXMLPRINTTIME_H
