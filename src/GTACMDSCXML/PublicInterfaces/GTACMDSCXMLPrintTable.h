#ifndef GTACMDSCXMLPRINTTABLE_H
#define GTACMDSCXMLPRINTTABLE_H


#include "GTACMDSCXMLBase.h"
#include "GTACMDSCXML.h"
#include "GTAActionPrintTable.h"

class GTACMDSCXML_SHARED_EXPORT GTACMDSCXMLPrintTable : public GTACMDSCXMLBase
{
public:
    GTACMDSCXMLPrintTable();
    GTACMDSCXMLPrintTable(GTAActionPrintTable *pCmdNode, const QString &iStateId, const QString &iTargetId);
    ~GTACMDSCXMLPrintTable();

    virtual QString getSCXMLString() const;
    virtual GTASCXMLState getSCXMLState()const;


private:
    GTAActionPrintTable *_PrintCmd;

private:
    GTASCXMLState getSCXMLForPrintParams(const QString iStateId,const QString iTargetId,
                                            const QString &iFinalId,
                                            const QString &iDecisionVar) const;
};

#endif // GTACMDSCXMLPRINTTABLE_H
