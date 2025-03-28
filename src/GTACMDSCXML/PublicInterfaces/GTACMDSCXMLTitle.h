#ifndef GTACMDSCXMLTITLE
#define GTACMDSCXMLTITLE_H


#include "GTACMDSCXMLBase.h"
#include "GTACMDSCXML.h"
#include "GTAActionParentTitle.h"

class GTACMDSCXML_SHARED_EXPORT GTACMDSCXMLTitle : public GTACMDSCXMLBase
{
public:
    GTACMDSCXMLTitle();
    GTACMDSCXMLTitle(GTAActionParentTitle *pCmdNode, const QString &iStateId, const QString &iTargetId);
    ~GTACMDSCXMLTitle();

    virtual QString getSCXMLString() const;
    virtual GTASCXMLState getSCXMLState()const;
private:
    GTAActionParentTitle *_TitleCmd;

};

#endif // GTACMDSCXMLTITLE
