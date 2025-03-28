#include "AINGTAFSIITestProcedure.h"


AINGTAFSIITestProcedure::AINGTAFSIITestProcedure()
{

}

QList<AINGTAFSIIFunctionCall> AINGTAFSIITestProcedure::getTestProcedureFunctionCalls()
{
    return _FunctionCalls;
}

void AINGTAFSIITestProcedure::setTestProcedureFunctionCalls(QList<AINGTAFSIIFunctionCall> &lstFunctionCalls)
{
    _FunctionCalls = lstFunctionCalls;
}
