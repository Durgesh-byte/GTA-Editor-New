#ifndef GTACMDSCXMLMANUALACTION_H
#define GTACMDSCXMLMANUALACTION_H


#include "GTACMDSCXMLBase.h"
#include "GTACMDSCXML.h"
#include "GTAActionManual.h"

class GTACMDSCXML_SHARED_EXPORT GTACMDSCXMLManualAction : public GTACMDSCXMLBase
{
public:
    GTACMDSCXMLManualAction();
    GTACMDSCXMLManualAction(GTAActionManual *pCmdNode, const QString &iStateId, const QString &iTargetId);
    ~GTACMDSCXMLManualAction();

    virtual QString getSCXMLString() const;
    virtual GTASCXMLState getSCXMLState()const;

    void setIgnoredInSCXML(bool &iIsIgnored){_IsIgnored = iIsIgnored;}
    void setTimeout(const QString &iTimeOut){_Timeout = iTimeOut;}

private:
    GTAActionManual *_ManCmd;
    bool _IsIgnored;
    QString _Timeout;
private:
    GTASCXMLState getManualActionPopUpState(const QString &iStateId, const QString &iTargetId) const;
    GTASCXMLState getManualActionFeedBackState(const QString &iStateId, const QString &iTargetId) const;
    GTASCXMLState getIgnoredState(const QString &iStateId, const QString &iTargetId) const;
    void updateParameterWithValues(const QString &iStateId,QString &iUserMessage) const;
};

#endif // GTACMDSCXMLMANUALACTION_H
