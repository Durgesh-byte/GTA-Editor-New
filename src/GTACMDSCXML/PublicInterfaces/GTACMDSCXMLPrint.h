#ifndef GTACMDSCXMLPRINT_H
#define GTACMDSCXMLPRINT_H


#include "GTACMDSCXMLBase.h"
#include "GTACMDSCXML.h"
#include "GTAActionPrint.h"

class GTACMDSCXML_SHARED_EXPORT GTACMDSCXMLPrint : public GTACMDSCXMLBase
{
public:
    GTACMDSCXMLPrint();
    GTACMDSCXMLPrint(GTAActionPrint *pCmdNode, const QString &iStateId, const QString &iTargetId);
    ~GTACMDSCXMLPrint();

    virtual QString getSCXMLString() const;
    virtual GTASCXMLState getSCXMLState()const;


private:
    GTAActionPrint *_PrintCmd;

private:
    GTASCXMLState getSCXMLForPrintMessage(const QString iStateId,const QString iTargetId,const QString &iFinalId=QString(),const QString &iDecisionVar=QString()) const;
    GTASCXMLState getSCXMLForPrintParams(const QString iStateId,const QString iTargetId,const QString &iFinalId=QString(),const QString &iDecisionVar=QString()) const;
};

#endif // GTACMDSCXMLPRINT_H
