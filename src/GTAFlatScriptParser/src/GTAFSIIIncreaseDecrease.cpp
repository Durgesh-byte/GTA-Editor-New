#include "AINGTAFSIIIncreaseDecrease.h"
#include "AINGTAFSIIUtil.h"

AINGTAFSIIIncreaseDecrease::AINGTAFSIIIncreaseDecrease()
{
}

AINGTAFSIIIncreaseDecrease::AINGTAFSIIIncreaseDecrease(const QString &iCommandName):AINGTAFSIISet(iCommandName)
{
    if(iCommandName == CMD_TYPE_INCREASE)
        _IsCmdDecrease = false;
    else
        _IsCmdDecrease = true;
}

void AINGTAFSIIIncreaseDecrease::setParamValue(const QString &iVal)
{
    if(_IsCmdDecrease) //decrease
    {
        _Value = QString("%1%2%3").arg(_Value,"-",iVal);
    }
    else    //increase
    {
        _Value = QString("%1%2%3").arg(_Value,"+",iVal);
    }
}


