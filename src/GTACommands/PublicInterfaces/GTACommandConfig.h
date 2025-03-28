#ifndef GTACOMMANDCONFIG_H
#define GTACOMMANDCONFIG_H
#include "GTACommands.h"
#include "GTACommandBase.h"

#pragma warning(push, 0)
#include <QString>
#include <QHash>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTACommandConfig 
{
public:



    GTACommandConfig();
    GTACommandConfig(const GTACommandConfig& rhs);
    ~GTACommandConfig();   

    QString getProperty(const QString& )const;
    void setProperty(const QString&, const QString&);
    void setCommandName(const QString& );
    QString getCommandName()const;

private:
   
    QString _CommandName;
    QHash<QString,QString> _property;
};

#endif // GTACommandConfig_H
