#include "AINGTAFSIICheck.h"

AINGTAFSIICheck::AINGTAFSIICheck()
{

}

AINGTAFSIICheck::AINGTAFSIICheck(const QString &iCommandName):AINGTAFSIICommandBase(iCommandName)
{

}

void AINGTAFSIICheck::setLogicalConnector(const QString &iVal)
{
    _LogicalConnector = iVal;
}

QString AINGTAFSIICheck::getLogicalConnector()const
{
    return _LogicalConnector;
}

QList<AINGTAFSIICondition> AINGTAFSIICheck::getConditionList()const
{
    return _ConditionList;
}

void AINGTAFSIICheck::addCondition(const AINGTAFSIICondition &objCon)
{
    _ConditionList.append(objCon);
}

void AINGTAFSIICheck::setFSOnly(const bool &iVal)
{
    _FSOnly = iVal;
}

bool AINGTAFSIICheck::isFSOnly()const
{
    return _FSOnly;

}
