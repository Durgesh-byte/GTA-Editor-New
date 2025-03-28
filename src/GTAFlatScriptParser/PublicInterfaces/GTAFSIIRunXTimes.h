#ifndef AINGTAFSIIRUNXTIMES_H
#define AINGTAFSIIRUNXTIMES_H
#include "AINGTAFSIICommandBase.h"
#include <QList>

class AINGTAFlatScriptParser_LIBRARY_SHARED_EXPORT AINGTAFSIIRunXTimes:public AINGTAFSIICommandBase
{
    int _Iterations;
    QList<AINGTAFSIICommandBase*> _FunctionalBlockCommands;

public:
    AINGTAFSIIRunXTimes();
    AINGTAFSIIRunXTimes(const QString &iCommandName);

    void setNumberOfIterations(const QString &iVal);
    int getNumberOfIterations();

    void addFunctionBlockCommand(AINGTAFSIICommandBase *pCmd);
    QList<AINGTAFSIICommandBase*> getFunctionalBlockCommand();
};

#endif // AINGTAFSIIRUNXTIMES_H
