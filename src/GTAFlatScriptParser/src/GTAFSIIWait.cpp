#include "AINGTAFSIIWait.h"

AINGTAFSIIWait::AINGTAFSIIWait()
{

}
AINGTAFSIIWait::AINGTAFSIIWait(const QString &iCommandName):AINGTAFSIICommandBase(iCommandName)
{

}

void AINGTAFSIIWait::setUnit(const QString &iUnit)
{
    _Unit = iUnit;
}

QString AINGTAFSIIWait::getUnit()const
{
    return _Unit;
}

void AINGTAFSIIWait::setCounterValue(const QString &iVal)
{

    _Counter = iVal;

}

QString AINGTAFSIIWait::getCounterValue()const
{
    return _Counter;
}

