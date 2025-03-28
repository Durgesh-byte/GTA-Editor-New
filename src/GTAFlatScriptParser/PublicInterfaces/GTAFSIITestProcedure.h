#ifndef AINGTAFSIITESTPROCEDURE_H
#define AINGTAFSIITESTPROCEDURE_H
#include "AINGTAFSIIFunctionCall.h"
#include <QList>
class AINGTAFlatScriptParser_LIBRARY_SHARED_EXPORT AINGTAFSIITestProcedure
{

    QList<AINGTAFSIIFunctionCall> _FunctionCalls;

public:
    AINGTAFSIITestProcedure();
    QList<AINGTAFSIIFunctionCall> getTestProcedureFunctionCalls();
    void setTestProcedureFunctionCalls(QList<AINGTAFSIIFunctionCall> &lstFunctionCalls);
};

#endif // AINGTAFSIITESTPROCEDURE_H
