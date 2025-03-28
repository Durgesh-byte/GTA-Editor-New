#ifndef AINGTAFSIIWAIT_H
#define AINGTAFSIIWAIT_H
#include "AINGTAFSIICommandBase.h"
#include <QString>

class AINGTAFlatScriptParser_LIBRARY_SHARED_EXPORT AINGTAFSIIWait:public AINGTAFSIICommandBase
{
public:
    AINGTAFSIIWait();
    AINGTAFSIIWait(const QString &iCommandName);
    void setUnit(const QString &iUnit);
    QString getUnit()const;

    void setCounterValue(const QString &iVal);
    QString getCounterValue()const;


private:
    QString _Unit;
    QString _Counter; 
};

#endif // AINGTAFSIIWAIT_H
