#include "AINGTAFSIIFunctionCall.h"

AINGTAFSIIFunctionCall::AINGTAFSIIFunctionCall()
{
   
}

AINGTAFSIIFunctionCall::AINGTAFSIIFunctionCall(const QString &iCommandName):AINGTAFSIICommandBase(iCommandName)
{

}

void AINGTAFSIIFunctionCall::setFunctionName(const QString &iName)
{
    _Name = iName;
}

QString AINGTAFSIIFunctionCall::getFunctionName()
{
    return _Name;
}

