#include "AINGTAFSIIPause.h"

AINGTAFSIIPause::AINGTAFSIIPause()
{
}

AINGTAFSIIPause::AINGTAFSIIPause(const QString &iCommandName):AINGTAFSIICommandBase(iCommandName)
{
}

void AINGTAFSIIPause::setMessage(const QString &iMessage)
{
    _Message = iMessage;
}

QString AINGTAFSIIPause::getMessage()const
{
    return _Message;
}

