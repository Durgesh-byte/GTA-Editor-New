#include "GTAGenericParamData.h"

GTAGenericParamData::GTAGenericParamData()
{
}


QString GTAGenericParamData::getParamDataName()
{
    return _Name;
}

void GTAGenericParamData::setParamDataName(const QString iParamName)
{
    _Name = iParamName;
}

QString GTAGenericParamData::getParamDataType()
{
    return _Type;
}

void GTAGenericParamData::setParamDataType(const QString iParamDataType)
{
    _Type = iParamDataType;
}

QList<GTAGenericAttribute> GTAGenericParamData::getParamAttributeList()
{
    return _Attributes;
}

void GTAGenericParamData::setParamAttributeList(const QList<GTAGenericAttribute> iAttrList)
{
    _Attributes = iAttrList;
}
