#include "AINGTAFSIIWaitWhile.h"

AINGTAFSIIWaitWhile::AINGTAFSIIWaitWhile()
{
}
AINGTAFSIIWaitWhile::AINGTAFSIIWaitWhile(const QString &iCommandName):AINGTAFSIICommandBase(iCommandName)
{

}

void AINGTAFSIIWaitWhile::setCondition(const AINGTAFSIICondition &iCondition)
{
   _Condition = iCondition;
}

AINGTAFSIICondition AINGTAFSIIWaitWhile::getCondition()const
{
    return _Condition;
}

void AINGTAFSIIWaitWhile::setUnit(const QString &iUnit)
{
    _Unit = iUnit;
}

QString AINGTAFSIIWaitWhile::getUnit()const
{
    return _Unit;
}

void AINGTAFSIIWaitWhile::setTimeOut(const QString &iTimeout)
{
   _Timeout = iTimeout;

}

QString AINGTAFSIIWaitWhile::getTimeOut()const
{
    return _Timeout;
}

void AINGTAFSIIWaitWhile::setErrorOnTimeOut(const QString &iError)
{
    if(iError == "true")
        _ErrorOnTimeout = true;
    else if(iError == "false")
        _ErrorOnTimeout = false;
}

bool AINGTAFSIIWaitWhile::getErrorOnTimeOut()const
{
    return _ErrorOnTimeout;
}
