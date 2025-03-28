#ifndef AINGTAFSIICHECK_H
#define AINGTAFSIICHECK_H
#include "AINGTAFSIICommandBase.h"
#include <QList>



class AINGTAFlatScriptParser_LIBRARY_SHARED_EXPORT AINGTAFSIICheck:public AINGTAFSIICommandBase
{
public:
    AINGTAFSIICheck();
    AINGTAFSIICheck(const QString &iCommandName);
    QList<AINGTAFSIICondition> getConditionList()const;
    void addCondition(const AINGTAFSIICondition &objCon);
    void setLogicalConnector(const QString &iVal);
    QString getLogicalConnector()const;

    void setFSOnly(const bool &iVal = false);
    bool isFSOnly()const;

private:
    QList<AINGTAFSIICondition> _ConditionList;
    QString _LogicalConnector;
    bool _FSOnly;

};


#endif // AINGTAFSIICHECK_H
