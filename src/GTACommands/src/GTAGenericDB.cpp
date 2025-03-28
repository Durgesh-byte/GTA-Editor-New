#include "GTAGenericDB.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

GTAGenericDB::GTAGenericDB()
{
    _ConcatString = " "; //default value = space
    _Delimiter = ",";
}

void GTAGenericDB::setName(const QString &iName)
{
    _Name = iName;
}

QString GTAGenericDB::getName()const
{
    return _Name;
}

void GTAGenericDB::setPath(const QString &iPath)
{
    _Path = iPath;
}

QString GTAGenericDB::getPath()const
{
    return _Path;
}

void GTAGenericDB::setRelativePath(const QString& iRelativePath)
{
    _RelativePath = iRelativePath;
}

QString GTAGenericDB::getRelativePath() const
{
    return _RelativePath;
}

void GTAGenericDB::addAttribute(const int &iID, const QString &iName, const int &iCol)
{
    GTAGenericDbAttribute attribute(iID,iName,iCol);

    _Attributes.append(attribute);
}

QList<GTAGenericDbAttribute> GTAGenericDB::getAttributes()
{
    return _Attributes;
}

GTAGenericDbAttribute GTAGenericDB::getAttribute(const int &index)
{
    GTAGenericDbAttribute attribute;
    if(index < _Attributes.count())
    {
        attribute = _Attributes.at(index);
    }
    return attribute;
}


QList<int> GTAGenericDB::getColumnList()const
{
    QList<int> columns;
    for(int i = 0; i < _Attributes.count();i++)
    {
        GTAGenericDbAttribute attr = _Attributes.at(i);
        int col = attr.getColumn();
        columns.append(col);
    }
    return columns;
}

QList<QString> GTAGenericDB::getHeadingList()const
{
    QList<QString> headings;

    for(int i = 0; i < _Attributes.count();i++)
    {
        GTAGenericDbAttribute attr = _Attributes.at(i);
        QString name = attr.getName();
        headings.append(name);
    }
    return headings;
}

void GTAGenericDB::setDelimiter(const QString &iDelimiter)
{
    if(!iDelimiter.isEmpty())
        _Delimiter = iDelimiter;
    else
        _Delimiter =",";
}

QString GTAGenericDB::getDelimiter()const
{
    return _Delimiter;
}

void GTAGenericDB::setColsConcatList(const QList<int> &iVal)
{
    _ColsToConcatList = iVal;
}

QList<int> GTAGenericDB::getColsConcatList()const
{
    return _ColsToConcatList;
}

void GTAGenericDB::setConcatString(const QString &iConcatStr)
{
    _ConcatString = iConcatStr;
}

QString GTAGenericDB::getConcatString()const
{
    return _ConcatString;
}
