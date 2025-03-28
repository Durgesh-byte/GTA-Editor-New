#include "AINGTAFSIILoop.h"

AINGTAFSIILoop::AINGTAFSIILoop()
{
}


AINGTAFSIILoop::AINGTAFSIILoop(const QString &iCommandName):AINGTAFSIICommandBase(iCommandName)
{

}


void AINGTAFSIILoop::setCondition(const AINGTAFSIICondition &iCond)
{
    _Condition = iCond;
}

AINGTAFSIICondition AINGTAFSIILoop::getCondiiton()
{
    return _Condition;
}

void AINGTAFSIILoop::addFunctionBlockCommand(AINGTAFSIICommandBase* pCmd)
{
    _FunctionBlockCommands.append(pCmd);
}

QList<AINGTAFSIICommandBase*> AINGTAFSIILoop::getFunctionBlockCommands()
{
    return _FunctionBlockCommands;
}
