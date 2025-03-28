#include "AINGTAFSIISet.h"

AINGTAFSIISet::AINGTAFSIISet()
{
}

AINGTAFSIISet::AINGTAFSIISet(const QString &iCommandType):AINGTAFSIICommandBase(iCommandType)
{
    _FSOnly = false;
}

void AINGTAFSIISet::setParamName(const QString &iName)
{
    _ParamName = iName;
}

QString AINGTAFSIISet::getParamName()
{
    return _ParamName;
}

void AINGTAFSIISet::setParamValue(const QString &iVal)
{
    _Value = iVal;
}

QString AINGTAFSIISet::getParamValue()
{
    return _Value;
}

void AINGTAFSIISet::setFSOnly(const bool &iVal)
{
    _FSOnly = iVal;
}

bool AINGTAFSIISet::isFSOnly()const
{
    return _FSOnly;

}
void AINGTAFSIISet::setValueOnly(const bool &iVal)
{
    _ValueOnly = iVal;
}

bool AINGTAFSIISet::isValueOnly()const
{
    return _ValueOnly;

}
