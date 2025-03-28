#ifndef AINGTAFSIILOGENTRY_H
#define AINGTAFSIILOGENTRY_H
#include "AINGTAFSIICommandBase.h"
#include <QString>
#include "AINGTAFlatScriptParser.h"

class AINGTAFlatScriptParser_LIBRARY_SHARED_EXPORT AINGTAFSIILogEntry:public AINGTAFSIICommandBase
{
public:
    AINGTAFSIILogEntry();
    AINGTAFSIILogEntry(const QString &iCommandName);
    void setMessage(const QString &iMessage);
    QString getMessage()const;


private:
    QString _Message;
};

#endif // AINGTAFSIILOGENTRY_H
