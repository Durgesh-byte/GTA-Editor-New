#ifndef GTAUSERSETTINGS_H
#define GTAUSERSETTINGS_H

#include "GTAControllers.h"

#pragma warning(push, 0)
#include <QSettings>
#include <QMap>
#pragma warning(pop)

class GTAControllers_SHARED_EXPORT GTAUserSettings : public QSettings
{
    Q_OBJECT
public:
    explicit GTAUserSettings(QObject *parent = 0);
    /**
      * This function save user preference on the application color scheme
      * @iTheme: is a key-value pair, where key is the context and the value is color value
      */
    void setTheme(const QMap<QString,QString> & iTheme);

    /**
      * This function return the saved user preference on the application color scheme
      * return value is a key-value pair, where key is the context and the value is color value
      */
    QMap<QString,QString> getTheme();

    ///**
    //  * This function save the last application configuration on which user has loaded
    //  * @iProgram : name of program loaded
    //  * @iEquipmentName: name of equipment loaded
    //  * @iStandardName: name of standard load
    //  */
    //void setAppConfiguration(const QString & iProgramName, const QString & iEquipmentName, const QString & iStandardName);

    ///**
    //  * This function retrieve the last loaded application program
    //  * @oProgramName : name of program last saved
    //  * @oEquipmentName: name of equipment last saved
    //  * @oStandardName: name of standard last saved
    //  */
    //void getAppConfiguration(QString & oProgramName,QString & oEquipmentName,QString & oStandardName);


signals:

public slots:

};

#endif // GTAUSERSETTINGS_H
