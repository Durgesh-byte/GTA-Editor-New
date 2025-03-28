#ifndef AINGTAFSIIFUNCTIONCALL_H
#define AINGTAFSIIFUNCTIONCALL_H
#include <QString>
#include "AINGTAFSIICommandBase.h"
#include "AINGTAFlatScriptParser.h"

class AINGTAFlatScriptParser_LIBRARY_SHARED_EXPORT AINGTAFSIIFunctionCall : public AINGTAFSIICommandBase
{
    QString _Name;
public:
    AINGTAFSIIFunctionCall();
    AINGTAFSIIFunctionCall(const QString &iCommandName);
    void setFunctionName(const QString &iName);
    QString getFunctionName();

};

#endif // AINGTAFSIIFUNCTIONCALL_H
