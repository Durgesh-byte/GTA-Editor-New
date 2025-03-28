#include "AINGTAFSIIIf.h"

AINGTAFSIIIf::AINGTAFSIIIf()
{
}

AINGTAFSIIIf::AINGTAFSIIIf(const QString &iCommandName):AINGTAFSIICommandBase(iCommandName)
{

}

void AINGTAFSIIIf::setLogicalOperator(const QString &iLogicalOp)
{
    _LogicalOperator = iLogicalOp;
}

QString AINGTAFSIIIf::getLogicalOperator()const
{
    return _LogicalOperator;
}

QList<AINGTAFSIICondition> AINGTAFSIIIf::getConditions()
{
    return _Conditions;
}

void AINGTAFSIIIf::addCondition(const AINGTAFSIICondition &iCondition)
{
    _Conditions.append(iCondition);
}

QList<AINGTAFSIICommandBase*> AINGTAFSIIIf::getCommandListOnTrue()
{
    return _CommandListOnTrue;
}

void AINGTAFSIIIf::addCommandOnTrue(AINGTAFSIICommandBase* pCmd)
{
    _CommandListOnTrue.append(pCmd);
}

QList<AINGTAFSIICommandBase*> AINGTAFSIIIf::getCommandListOnFalse()
{
    return _CommandListOnFalse;
}

void AINGTAFSIIIf::addComamndOnFalse(AINGTAFSIICommandBase* pCmd)
{
    _CommandListOnFalse.append(pCmd);
}

