#include "AINGTAFSIIFunction.h"

AINGTAFSIIFunction::AINGTAFSIIFunction()
{

}


void AINGTAFSIIFunction::addCommand(AINGTAFSIICommandBase* pCmd)
{
    _Commands.append(pCmd);
}

QList<AINGTAFSIICommandBase*> AINGTAFSIIFunction::getCommands()const
{
    return _Commands;
}

void AINGTAFSIIFunction::setSignals(const QList<AINGTAFSIISignal> &iSignalList)
{
    _Signals = iSignalList;
}

QList<AINGTAFSIISignal> AINGTAFSIIFunction::getSignal()const
{
    return _Signals;
}

void AINGTAFSIIFunction::setName(const QString &iName)
{
    _Name = iName;
}

QString AINGTAFSIIFunction::getName()const
{
    return _Name;
}

void AINGTAFSIIFunction::setDescription(const QString &iDesc)
{
    _Description = iDesc;
}

QString AINGTAFSIIFunction::getDescription()
{
    return _Description;
}
