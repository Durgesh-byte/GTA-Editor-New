#include "GTAGenericAttribute.h"

GTAGenericAttribute::GTAGenericAttribute()
{
}


QString GTAGenericAttribute::getAttributeName()
{
    return _Name;
}

void GTAGenericAttribute::setAttributeName(const QString iName)
{
    _Name = iName;
}

QString GTAGenericAttribute::getAttributeType()
{
    return _Type;
}

void GTAGenericAttribute::setAttributeType(const QString iType)
{
    _Type = iType;
}

QString GTAGenericAttribute::getAttributeOccurence()
{
    return _Occurence;
}
QString GTAGenericAttribute::getAttributeDescription()
{
    return _Description;
}

void GTAGenericAttribute::setAttributeOccurence(const QString iOccurence)
{
    _Occurence = iOccurence;
}
void GTAGenericAttribute::setAttributeDescription(const QString &iDescription)
{
    _Description = iDescription;
}

QStringList GTAGenericAttribute::getAttributeValues()
{
    return _Values;
}

void GTAGenericAttribute::setAttributeValues(const QStringList iValues)
{
    _Values = iValues;
}


QString GTAGenericAttribute::getReturnCode()const
{
    return _ReturnCode;
}
void GTAGenericAttribute::setReturnCode(const QString &returnCode)
{
    _ReturnCode = returnCode;
}


QString GTAGenericAttribute::getTrueCondition()const
{
    return _TrueCondition;
}
void GTAGenericAttribute::setTrueCondition(const QString &condition)
{
    _TrueCondition = condition;
}

QString GTAGenericAttribute::getFalseCondition()const
{
    return _FalseCondition;
}

void GTAGenericAttribute::setFalseCondition(const QString &condition)
{
    _FalseCondition = condition;
}


void GTAGenericAttribute::setWaitUntil(const QString &iVal)
{
	_WaitUntil = (iVal.toUpper() == "YES")?true:false;
}

bool GTAGenericAttribute::hasWaitUntil()const
{
    return _WaitUntil;
}
