#include "GTAUserSettings.h"

#pragma warning(push, 0)
#include <QStringList>
#pragma warning(pop)

GTAUserSettings::GTAUserSettings(QObject *parent) :
    QSettings("Airbus","GTAApplication",parent)
{
}
/**
  * This function save user preference on the application color scheme
  * @iTheme: is a key-value pair, where key is the context and the value is color value
  */
void GTAUserSettings::setTheme(const QMap<QString,QString> & iTheme)
{
    beginGroup("USER_THEME");
    foreach(QString context, iTheme.keys())
    {
        setValue(context,iTheme.value(context));
    }
    endGroup();
}

/**
  * This function return the saved user preference on the application color scheme
  * return value is a key-value pair, where key is the context and the value is color value
  */
QMap<QString,QString> GTAUserSettings::getTheme()
{
    QMap<QString,QString> oTheme;
    beginGroup("USER_THEME");
    QStringList contextList = childKeys();
    foreach(QString context, contextList)
    {
        QString contextVal = value(context).toString();
        oTheme.insert(context,contextVal);
    }
    endGroup();
    return oTheme;
}

///**
//  * This function save the last application configuration on which user has loaded
//  * @iProgram : name of program loaded
//  * @iEquipmentName: name of equipment loaded
//  * @iStandardName: name of standard load
//  */
//void GTAUserSettings::setAppConfiguration(const QString & iProgramName, const QString & iEquipmentName, const QString & iStandardName)
//{
//    beginGroup("USER_PROGCONF");
//    setValue("PROGRAM",iProgramName);
//    setValue("EQUIPMENT",iEquipmentName);
//    setValue("STANDARD",iStandardName);
//    endGroup();
//}
//
///**
//  * This function retrieve the last loaded application program
//  * @oProgramName : name of program last saved
//  * @oEquipmentName: name of equipment last saved
//  * @oStandardName: name of standard last saved
//  */
//void GTAUserSettings::getAppConfiguration(QString & oProgramName,QString & oEquipmentName,QString & oStandardName)
//{
//    beginGroup("USER_PROGCONF");
//    oProgramName = value("PROGRAM").toString();
//    oEquipmentName = value("EQUIPMENT").toString();
//    oStandardName = value("STANDARD").toString();
//    endGroup();
//}
