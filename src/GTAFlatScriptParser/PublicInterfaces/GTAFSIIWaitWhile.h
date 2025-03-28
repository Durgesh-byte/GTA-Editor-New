#ifndef AINGTAFSIIWAITWHILE_H
#define AINGTAFSIIWAITWHILE_H
#include "AINGTAFSIICommandBase.h"
#include <QString>

class AINGTAFlatScriptParser_LIBRARY_SHARED_EXPORT AINGTAFSIIWaitWhile:public AINGTAFSIICommandBase
{
public:
    AINGTAFSIIWaitWhile();
    AINGTAFSIIWaitWhile(const QString &iCommandName);
    void setCondition(const AINGTAFSIICondition &iCondition);
    AINGTAFSIICondition getCondition()const;

    void setUnit(const QString &iUnit);
    QString getUnit()const;

    void setTimeOut(const QString &iTimeout);
    QString getTimeOut()const;

    void setErrorOnTimeOut(const QString &iError);
    bool getErrorOnTimeOut()const;

private:

    AINGTAFSIICondition _Condition;
    QString _Unit;
    QString _Timeout;
    bool _ErrorOnTimeout;
};

#endif // AINGTAFSIIWAITWHILE_H
