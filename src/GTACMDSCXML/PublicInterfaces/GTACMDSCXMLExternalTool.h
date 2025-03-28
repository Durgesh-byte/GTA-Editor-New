#ifndef GTACMDSCXMLEXTERNALTOOL_H
#define GTACMDSCXMLEXTERNALTOOL_H


#include "GTACMDSCXMLBase.h"
#include "GTACMDSCXML.h"
#include "GTAGenericToolCommand.h"

class GTACMDSCXML_SHARED_EXPORT GTACMDSCXMLExternalTool : public GTACMDSCXMLBase
{
public:
    GTACMDSCXMLExternalTool();
    GTACMDSCXMLExternalTool(GTAGenericToolCommand *pCmdNode, const QString &iStateId, const QString &iTargetId);
    ~GTACMDSCXMLExternalTool();

    virtual QString getSCXMLString() const;
    virtual GTASCXMLState getSCXMLState()const;


private:
    GTAGenericToolCommand *_ExtToolCmd;

private:
    GTASCXMLState getSCXMLForExternalTool() const;

};

#endif // GTACMDSCXMLEXTERNALTOOL_H
