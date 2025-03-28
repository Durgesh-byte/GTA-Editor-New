#ifndef AINGTAFSIILOOP_H
#define AINGTAFSIILOOP_H
#include "AINGTAFSIICommandBase.h"
#include <QList>
#include "AINGTAFlatScriptParser.h"

class AINGTAFlatScriptParser_LIBRARY_SHARED_EXPORT AINGTAFSIILoop:public AINGTAFSIICommandBase
{
    AINGTAFSIICondition _Condition;
    QList<AINGTAFSIICommandBase*> _FunctionBlockCommands;

public:
    AINGTAFSIILoop();
    AINGTAFSIILoop(const QString &iCommandName);

    void setCondition(const AINGTAFSIICondition &iCond);
    AINGTAFSIICondition getCondiiton();

    void addFunctionBlockCommand(AINGTAFSIICommandBase* pCmd);
    QList<AINGTAFSIICommandBase*> getFunctionBlockCommands();
};

#endif // AINGTAFSIILOOP_H
