#ifndef AINGTAFSIIPAUSE_H
#define AINGTAFSIIPAUSE_H
#include "AINGTAFSIICommandBase.h"
#include <QString>
#include "AINGTAFlatScriptParser.h"

class AINGTAFlatScriptParser_LIBRARY_SHARED_EXPORT AINGTAFSIIPause:public AINGTAFSIICommandBase
{
public:
    AINGTAFSIIPause();
    AINGTAFSIIPause(const QString &iCommandName);

    void setMessage(const QString &iMessage);
    QString getMessage()const;

private:
    QString _Message;
};

#endif // AINGTAFSIIPAUSE_H
