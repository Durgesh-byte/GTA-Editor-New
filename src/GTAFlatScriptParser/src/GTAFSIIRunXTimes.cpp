#include "AINGTAFSIIRunXTimes.h"

AINGTAFSIIRunXTimes::AINGTAFSIIRunXTimes()
{
}

AINGTAFSIIRunXTimes::AINGTAFSIIRunXTimes(const QString &iCommandName):AINGTAFSIICommandBase(iCommandName)
{

}


void AINGTAFSIIRunXTimes::setNumberOfIterations(const QString &iVal)
{
    bool ok;
    int val = iVal.toInt(&ok);
    if(ok == true)
    {
        _Iterations = val;
    }
}

int AINGTAFSIIRunXTimes::getNumberOfIterations()
{
    return _Iterations;
}

void AINGTAFSIIRunXTimes::addFunctionBlockCommand(AINGTAFSIICommandBase *pCmd)
{
    _FunctionalBlockCommands.append(pCmd);
}

QList<AINGTAFSIICommandBase*> AINGTAFSIIRunXTimes::getFunctionalBlockCommand()
{
    return _FunctionalBlockCommands;
}
