#include "AINGTAFSIICommandBase.h"

AINGTAFSIICommandBase::AINGTAFSIICommandBase()
{
    _Deactivate = false;
    _Log = true;
    _StopOnFail = QString("false");
}

AINGTAFSIICommandBase::AINGTAFSIICommandBase(const QString &iCommandName)
{
    _CommandName = iCommandName;
    _Deactivate = false;
    _Log = true;
    _StopOnFail = QString("false");
    _CommandValidity = true;
}

AINGTAFSIICommandBase::~AINGTAFSIICommandBase()
{

}

void AINGTAFSIICommandBase::setStopOnFail(const QString &iVal)
{
    _StopOnFail = iVal;
}

void AINGTAFSIICommandBase::setLog(const QString &iVal)
{
    if(iVal == "true")
        _Log = true;
    else if(iVal == "false")
        _Log = false;
}

QString AINGTAFSIICommandBase::isStopOnFail()
{
    return _StopOnFail;
}

bool AINGTAFSIICommandBase::isLoggingOn()
{
    return _Log;
}

void AINGTAFSIICommandBase::setDeactivate(const QString &iVal)
{
    if(iVal == "true")
        _Deactivate = true;
    else if(iVal == "false")
        _Deactivate = false;
}

bool AINGTAFSIICommandBase::isCommandDeactivated()const
{
    return _Deactivate;
}

void AINGTAFSIICommandBase::setCommandName(const QString &iName)
{
    _CommandName = iName;
}

QString AINGTAFSIICommandBase::getCommandName()const
{
    return _CommandName;
}

void AINGTAFSIICommandBase::setIsCommandValid(const bool &iVal)
{
    _CommandValidity = iVal;
}

bool AINGTAFSIICommandBase::IsCommandValid()
{
    return _CommandValidity;
}


//==========================================STRUCT CONDITION========================

bool AINGTAFSIICondition::getIsSetOnlyFS() const
{
    return _IsSetOnlyFS;
}
void AINGTAFSIICondition::setIsSetOnlyFS(const bool iIsSetOnlyFS)
{
    _IsSetOnlyFS = iIsSetOnlyFS;
}

bool AINGTAFSIICondition::getIsSetOnlyValue() const
{
    return _IsCheckOnlyVal;
}
void AINGTAFSIICondition::setIsSetOnlyValue(const bool iIsSetOnlyFS)
{
    _IsCheckOnlyVal = iIsSetOnlyFS;
}


void AINGTAFSIICondition::setLHS(const QString &iLHS)
{
    _LHS = iLHS;
}

QString AINGTAFSIICondition::getLHS()const
{
    return _LHS;
}

void AINGTAFSIICondition::setRHS(const QString &iRHS)
{
    _RHS = iRHS;

}

QString AINGTAFSIICondition::getRHS()const
{
    return _RHS;
}

void AINGTAFSIICondition::setOperator(const QString &iOperator)
{
    if(iOperator == "==")
        _Operator = "=";
    else if(iOperator == "&gt;=")
        _Operator = ">=";
    else if(iOperator  == "&lt;=")
        _Operator = "<=";
    else if(iOperator == "&lt")
        _Operator = "<";
    else if(iOperator == "&gt")
        _Operator = ">";
    else
        _Operator = iOperator;
}

QString AINGTAFSIICondition::getOperator()const
{
    return _Operator;
}


