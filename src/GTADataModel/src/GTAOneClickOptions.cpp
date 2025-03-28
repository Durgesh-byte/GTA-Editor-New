#include "GTAOneClickOptions.h"

GTAOneClickOption::GTAOneClickOption()
{

}
GTAOneClickOption::~GTAOneClickOption()
{

}

void GTAOneClickOption::setOptionCondition(const QString &iOptionCondition)
{
    _OptionCondition = iOptionCondition;
}
void GTAOneClickOption::setOptionDefaultvalue(const QString &iOptionDefaultValue)
{
    _OptionDefaultValue = iOptionDefaultValue;
}

void GTAOneClickOption::setOptionHasValues(bool ihasValues)
{
    _HasValues = ihasValues;
}

void GTAOneClickOption::setOptionName(const QString &iOptionName)
{
    _OptionName = iOptionName;
}
void GTAOneClickOption::setOptionType(const QString &iOptionType)
{
    _OptionType = iOptionType;
}
void GTAOneClickOption::setOptionConfigPath(const QString &iOptionConfigPath)
{
    _OptionConfigPath = iOptionConfigPath;
}
void GTAOneClickOption::setOptionDefIniPath(const QString &iOptionDefIniPath)
{
    _OptionDefIniPath = iOptionDefIniPath;
}
void GTAOneClickOption::setOptionValues(const QStringList &iOptionValues)
{
    _OptionValues = iOptionValues;
}
QString GTAOneClickOption::getOptionDefIniPath() const 
{
    return _OptionDefIniPath;
}
QString GTAOneClickOption::getOptionConfigPath() const
{
    return _OptionConfigPath;
}

QString GTAOneClickOption::getOptionName() const
{
    return _OptionName;
}
QStringList GTAOneClickOption::getOptionValues() const
{
    return _OptionValues;
}
bool GTAOneClickOption::optionHasValues() const
{
    return _HasValues;
}

QString GTAOneClickOption::getOptionType() const
{
    return _OptionType;
}

QString GTAOneClickOption::getOptionDefaultValue() const
{
    return _OptionDefaultValue;
}

QString GTAOneClickOption::getOptionCondition() const
{
    return _OptionCondition;
}

GTAOneClickOptions::GTAOneClickOptions()
{
}

GTAOneClickOptions::~GTAOneClickOptions()
{

}

void GTAOneClickOptions::setDefOption(QString &iOptionName, GTAOneClickOption &iOption)
{
    if(!_DefOptionMap.contains(iOptionName))
    {
        _DefOptionMap.insert(iOptionName,iOption);
    }
}

void GTAOneClickOptions::setCondOptions(QString &iOptionName, GTAOneClickOption &iOption)
{
    QList<GTAOneClickOption> optionList;
    if(_CondOptionMap.contains(iOptionName))
    {
        optionList = _CondOptionMap.value(iOptionName);
        if(!optionList.isEmpty())
        {
            bool rc = false;
            foreach(GTAOneClickOption option,optionList)
            {
                if((option.getOptionName() == iOptionName) && (option.getOptionCondition()  == iOption.getOptionCondition()))
                {
                    rc = true;
                }
            }
            if(!rc)
            {
                optionList.append(iOption);
            }
            _CondOptionMap.remove(iOptionName);
            _CondOptionMap.insert(iOptionName,optionList);
        }
        else
        {
            optionList.append(iOption);
            _CondOptionMap.insert(iOptionName,optionList);
        }
    }
    else
    {
        optionList.append(iOption);

        QList<GTAOneClickOption> tempOptionList;
        tempOptionList.append(iOption);
        _CondOptionMap.insert(iOptionName,tempOptionList);
    }
}

void GTAOneClickOptions::setOptionType(QString &iOptionName, QString &iOptionType)
{
    if(!_OptionTypeMap.contains(iOptionName))
        _OptionTypeMap.insert(iOptionName,iOptionType);
}

void GTAOneClickOptions::addOptionToList(QString &iOption)
{
    if(!_OptionList.contains(iOption))
    {
        _OptionList.append(iOption);
    }
}

QString GTAOneClickOptions::getOptionType(QString &iOptionName)const
{
    return _OptionTypeMap.value(iOptionName);
}

QStringList GTAOneClickOptions::getAllOptions() const
{
    return _OptionList;
}

GTAOneClickOption GTAOneClickOptions::getOptionforCond(QString &iOptionName, QString &iCondName, QString &CondVal)
{
    QList<GTAOneClickOption> optionList;
    GTAOneClickOption option;
    if(_CondOptionMap.contains(iOptionName))
    {
        optionList = _CondOptionMap.value(iOptionName);
        foreach(GTAOneClickOption tempOption,optionList)
        {
            QString cond = tempOption.getOptionCondition();
            if(!cond.isEmpty() && !cond.isNull() && cond.contains("=="))
            {
                QStringList items = cond.split("==",QString::SkipEmptyParts);
                if(items.count() == 2)
                {
                    QString optionCondName = items.at(0).trimmed();
                    QString optionCondVal = items.at(1).trimmed();
                    if(tempOption.getOptionType().toLower() == "yesno")
                    {
                        optionCondVal = optionCondVal.toLower();
						CondVal = CondVal.toLower();
                    }
                    if((optionCondName == iCondName) && (optionCondVal == CondVal))
                    {
                        option = tempOption;
                        break;
                    }
                }
            }
            else
            {
                return option;
            }
        }
    }
    return option;
}

GTAOneClickOption GTAOneClickOptions::getDefOption(QString &iOptionName)
{
    return _DefOptionMap.value(iOptionName);
}

















