#include "AINGTAFSIILogEntry.h"

AINGTAFSIILogEntry::AINGTAFSIILogEntry()
{
}

AINGTAFSIILogEntry::AINGTAFSIILogEntry(const QString &iCommandName):AINGTAFSIICommandBase(iCommandName)
{

}

void AINGTAFSIILogEntry::setMessage(const QString &iMessage)
{
    _Message = iMessage;
}

QString AINGTAFSIILogEntry::getMessage()const
{
    return _Message;
}
