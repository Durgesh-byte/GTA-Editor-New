#ifndef GTACMDSCXMLSCXML_H
#define GTACMDSCXMLSCXML_H


#include "GTACMDSCXMLBase.h"
#include "GTACMDSCXML.h"
#include "GTASCXMLSCXML.h"

class GTACMDSCXML_SHARED_EXPORT GTACMDSCXMLSCXML
{
public:
    GTACMDSCXMLSCXML();

    QString getSCXMLString(bool isEnUSCXML) const;
    GTASCXMLSCXML getSCXMLState()const;
    void setCommandList(const QList<GTACMDSCXMLBase *> &iCmdList);
    void setDebugMode(bool iEnDebugMode = false);
    void setStandaloneMode(bool iEnStandaloneMode = false);
    void setUnSubStart(bool iInitUnSubStart = false);
    void setUnSubEnd(bool iInitUnSubEnd = false);
    void setSubUnsubTimeout(QString iTimeoutVal);
    void setUnsubscribeFileParamOnly(const bool& unsubscribeFileParamOnly);
private:
    QList<GTACMDSCXMLBase *> _CmdList;
    bool _isDebugModeEn;
    bool _isStandAloneModeEn;
    bool _isInitUnSubscribeStart;
    bool _isInitUnSubscribeEnd;
    QString _TimeoutVal;
    bool _unsubscribeFileParamOnly;
};

#endif // GTACMDSCXMLPRINT_H
