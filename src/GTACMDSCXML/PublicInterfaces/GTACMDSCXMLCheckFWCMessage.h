#ifndef GTACMDSCXMLCHECKFWCMESSAGE_H
#define GTACMDSCXMLCHECKFWCMESSAGE_H


#include "GTACMDSCXMLBase.h"
#include "GTACMDSCXML.h"
#include "GTACheckFwcWarning.h"

class GTACMDSCXML_SHARED_EXPORT GTACMDSCXMLCheckFWCMessage : public GTACMDSCXMLBase
{
public:
    GTACMDSCXMLCheckFWCMessage();
    GTACMDSCXMLCheckFWCMessage(GTACheckFwcWarning *pCmdNode, const QString &iStateId, const QString &iTargetId);
    ~GTACMDSCXMLCheckFWCMessage();

    virtual QString getSCXMLString() const;
    virtual GTASCXMLState getSCXMLState()const;

private:
    GTACheckFwcWarning *_ChkFwcCmd;
private:
    GTASCXMLState getFWCWarningMessageState() const;
};

#endif // GTACMDSCXMLCHECKFWCMESSAGE_H
