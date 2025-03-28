#include "GTAGenericDbAttribute.h"

GTAGenericDbAttribute::GTAGenericDbAttribute()
{
}


GTAGenericDbAttribute::GTAGenericDbAttribute(const int &iId, const QString &iName, const int &iCol)
{
    _Id = iId;
    _Name = iName;
    _Column = iCol;
}


void GTAGenericDbAttribute::setId(const int &iVal)
{
    _Id = iVal;
}

int GTAGenericDbAttribute::getId()const
{
    return _Id;
}

void GTAGenericDbAttribute::setName(const QString &iVal)
{
    _Name = iVal;

}

QString GTAGenericDbAttribute::getName()const
{
    return _Name;
}

void GTAGenericDbAttribute::setColumn(const int &iVal)
{
    _Column = iVal;
}

int GTAGenericDbAttribute::getColumn()const
{
    return _Column;
}
