#ifndef GTACMDSCXMLCHECKBITEMESSAGE_H
#define GTACMDSCXMLCHECKBITEMESSAGE_H


#include "GTACMDSCXMLBase.h"
#include "GTACMDSCXML.h"
#include "GTACheckBiteMessage.h"

class GTACMDSCXML_SHARED_EXPORT GTACMDSCXMLCheckBITEMessage : public GTACMDSCXMLBase
{
public:
    GTACMDSCXMLCheckBITEMessage();
    GTACMDSCXMLCheckBITEMessage(GTACheckBiteMessage *pCmdNode, const QString &iStateId, const QString &iTargetId);
    ~GTACMDSCXMLCheckBITEMessage();

    virtual QString getSCXMLString() const;
    virtual GTASCXMLState getSCXMLState()const;

private:
    GTACheckBiteMessage *_ChkBiteCmd;
private:
    GTASCXMLState getBITEMessageState() const;
};

#endif // GTACMDSCXMLCHECKBITEMESSAGE_H
