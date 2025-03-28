#include "GTACommandConfig.h"
#include "GTAUtil.h"


GTACommandConfig::GTACommandConfig()
{

}
GTACommandConfig::GTACommandConfig(const GTACommandConfig& rhs)
{
    _CommandName  = rhs.getCommandName();
    foreach(QString propertyName, _property.keys())
    {
        setProperty(propertyName,rhs.getProperty(propertyName));
    }
}
GTACommandConfig::~GTACommandConfig()
{

}

QString GTACommandConfig::getProperty(const QString& iPropertyName)const
{
    return _property.value(iPropertyName,QString());

}
void GTACommandConfig::setProperty(const QString& iPropertyName, const QString& iPropertyValue)
{
    if(!iPropertyName.isEmpty() && !iPropertyValue.isEmpty())
        _property[iPropertyName]=iPropertyValue;
}
void GTACommandConfig::setCommandName(const QString& iCmdName)
{
    _CommandName = iCmdName;
}
QString GTACommandConfig::getCommandName()const
{
    return _CommandName;
}