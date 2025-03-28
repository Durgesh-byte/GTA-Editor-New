#include "GTAGenericArgument.h"
#include "GTAUtil.h"

#pragma warning(push, 0)
#include <QMapIterator>
#pragma warning(pop)

GTAGenericArgument::GTAGenericArgument(): _isDisplayed(true)
{
    _UserSelectedHashIndex = 999;
    _isMandatory = true;
    _ConstValSet = false;
    _isDefault = false;
    _isDefaultOverriden = false;
}

QString GTAGenericArgument::getArgumentName() const
{
    return _Name;
}

void GTAGenericArgument::setArgumentName(const QString iName)
{
    _Name = iName;
    _HMILabel = _Name;
}

QStringList GTAGenericArgument::getVariableList() const
{
    QStringList items;
    items.append(_UserSelectedValue);
    return items;
}
void GTAGenericArgument::ReplaceTag(const QMap<QString,QString>& iTagValueMap)
{
    QMapIterator<QString,QString> iterTag(iTagValueMap);
    while (iterTag.hasNext())
    {
        iterTag.next();
        QString tag = QString("%1%2%3").arg(QChar(TAG_IDENTIFIER),iterTag.key(),QChar(TAG_IDENTIFIER));
        _UserSelectedValue.replace(tag,iterTag.value());
    }
}

QString GTAGenericArgument::getArgumentType() const
{
    return _Type;
}

void GTAGenericArgument::setArgumentType(const QString iType)
{
    _Type = iType;
}
QString GTAGenericArgument::getCondition() const
{
    return _Condition;
}

void GTAGenericArgument::setCondition(const QString iCondition)
{
    _Condition = iCondition;
}

QStringList GTAGenericArgument::getValues()
{
    return _Values;
}

void GTAGenericArgument::setValues(const QStringList iValues)
{
    _Values = iValues;

    if(!_Values.isEmpty())
        fillConditionList();
}


void GTAGenericArgument::fillConditionList()
{
    _conditionVals.clear();
    int isSplitValid = 0;
    for(int i = 0; i < _Values.count(); i++)
    {
        QString val = _Values.at(i);
        QStringList conditionVals = val.split(",",QString::SkipEmptyParts);
        if(conditionVals.count() > 1)
        {
            isSplitValid = isSplitValid | 1;
        }
        else
        {
            if(!_Condition.isEmpty())                       //Added to check if the argument has condition and has list of values with only one value
            {
                isSplitValid = isSplitValid | 1;
            }
            else
            {
                isSplitValid = isSplitValid | 0;
            }

        }
        _conditionVals.insert(i,conditionVals);
    }

    if(isSplitValid == 0)
    {
        _conditionVals.clear();

    }
}

bool GTAGenericArgument::isConditionValListEmpty()
{
    return _conditionVals.isEmpty();
}

QHash<int, QStringList> GTAGenericArgument::getConditionValList()const
{
    return _conditionVals;
}

QString GTAGenericArgument::getSearchType()
{
    return _SearchType;
}

void GTAGenericArgument::setSearchType(const QString iSearchType)
{
    _SearchType = iSearchType;
}

QString GTAGenericArgument::getUserSelectedValue()
{
    return _UserSelectedValue;
}
void GTAGenericArgument::setUserSelectedValue(const QString iVal)
{
    _UserSelectedValue = iVal;
}

int GTAGenericArgument::getUserSelectedHashIndex()const
{
    return _UserSelectedHashIndex;
}
void GTAGenericArgument::setUserSelectedHashIndex(const int iIndex)
{
    _UserSelectedHashIndex = iIndex;
}

bool GTAGenericArgument::operator ==(GTAGenericArgument obj2)const
{
    if(_Name == obj2.getArgumentName() && _Type == obj2.getArgumentType())
        return true;
    else
        return false;
}

bool GTAGenericArgument::operator !=(GTAGenericArgument iRhs)const
{
    bool rc = false;
    if(_Name != iRhs.getArgumentName() || _Type != iRhs.getArgumentType()
        || _SearchType != iRhs.getSearchType()
        || _HMILabel != iRhs.getHMILabel()
        || _Path != iRhs.getPath()
        || _UserDbName != iRhs.getUserDbName())
        {

        return true;
    }

    return rc;
}

void GTAGenericArgument::setDefaultValue(const QStringList &iDefaultValue)
{
    _DefaultValue = iDefaultValue;
}

QStringList GTAGenericArgument::getDefaultValue()const
{
    return _DefaultValue;
}

void GTAGenericArgument::setConstantValue(const QStringList &iConstantValue)
{
    _ConstantValue = iConstantValue;
}

QStringList GTAGenericArgument::getConstantValue()const
{
    return _ConstantValue;
}

void GTAGenericArgument::setHMILabel(const QString &iLabel)
{
    if(iLabel.isEmpty())
    {
        _HMILabel = _Name;
    }
    else
    {
        _HMILabel = iLabel;
    }
}

QString GTAGenericArgument::getHMILabel()const
{
    return _HMILabel;
}

bool GTAGenericArgument::isArgumentMandatory() const
{
    return _isMandatory;
}

void GTAGenericArgument::setMandatory(const QString &iVal)
{
    if(iVal == "yes")			//Check why compare is not working
    {
        _isMandatory = true;
    }
    else if(iVal == "no")
    {
        _isMandatory = false;
    }
}

bool GTAGenericArgument::isArgConst() const
{
    return _ConstValSet;
}

void GTAGenericArgument::setArgIsConst(const bool &iVal)
{
    _ConstValSet = iVal;
}

bool GTAGenericArgument::isArgDefault() const
{
    return _isDefault;
}

void GTAGenericArgument::setArgIsDefault(const bool &iVal)
{
    _isDefault = iVal;
}

bool GTAGenericArgument::isDefaultOverriden()const
{
    return _isDefaultOverriden;
}
void GTAGenericArgument::setDefaultOverriden(const bool &iVal)
{
    _isDefaultOverriden = iVal;
}

//bool GTAGenericArgument::operator ==(GTAGenericArgument &iArg)
//{
//    return (_Name == iArg.getArgumentName());
//}


void GTAGenericArgument::setIsDisplayed(const bool &iVal)
{
    _isDisplayed = iVal;
}

bool GTAGenericArgument::isDisplayed()const
{
    return _isDisplayed;
}

void GTAGenericArgument::setPath(const QString &iPath)
{
    _Path = iPath;
}

QString GTAGenericArgument::getPath()const
{
    return _Path;
}


void GTAGenericArgument::setId(const int &iId)
{
    _Id = iId;
}

int GTAGenericArgument::getId()const
{
    return _Id;
}

void GTAGenericArgument::setUserDbName(const QString &iName)
{
    _UserDbName = iName;
}

QString GTAGenericArgument::getUserDbName()const
{
    return _UserDbName;
}

void GTAGenericArgument::setColor(const QString& iColor)
{
    _Color = iColor;
}
QString GTAGenericArgument::getColor() const
{
    return _Color;
}

void GTAGenericArgument::setBackColor(const QString& iBackColor)
{
    _BackColor = iBackColor;
}
QString GTAGenericArgument::getBackColor() const
{
    return _BackColor;
}