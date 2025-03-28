#include "GTAGenericToolData.h"

GTAGenericToolData::GTAGenericToolData()
{
    _Functions.clear();
    _Definitions.clear();
}

void GTAGenericToolData::setToolName(const QString &iToolName)
{
    _ToolName = iToolName;
    if(_ToolDisplayName.isEmpty())
        _ToolDisplayName = _ToolName;
}

QString GTAGenericToolData::getToolName()const
{
    return _ToolName;
}

bool GTAGenericToolData::isTimeOut()
{
    return _HasTimeOut;
}

void GTAGenericToolData::setTimeOut(bool iTimeOut)
{
    _HasTimeOut = iTimeOut;
}

bool GTAGenericToolData::isActionOnFail()
{
    return _HasActionOnFail;
}

void GTAGenericToolData::setActionOnFail(bool iActionOnfail)
{
    _HasActionOnFail = iActionOnfail;
}


QString GTAGenericToolData::getFunctionType()
{
    return _FunctionsType;
}

void GTAGenericToolData::setFunctionType(const QString iFunctionType)
{
    _FunctionsType = iFunctionType;
}

void GTAGenericToolData::setFunctions(QList<GTAGenericFunction> iFunctionList)
{
    _Functions.clear();
    _Functions = iFunctionList;
}

QList<GTAGenericFunction> GTAGenericToolData::getFunctions()
{
    return _Functions;
}

void GTAGenericToolData::setDefinitions(QList<GTAGenericParamData> iDefinitions)
{
    _Definitions = iDefinitions;

}

QList<GTAGenericParamData> GTAGenericToolData::getDefinitions()
{
    return _Definitions;
}

void GTAGenericToolData::setHasComment(const bool &iVal)
{
    _HasComment = iVal;
}

bool GTAGenericToolData::hasComment()const
{
    return _HasComment;
}

void GTAGenericToolData::setHasDumpList(const bool &iVal)
{
    _HasDumpList = iVal;
}

bool GTAGenericToolData::hasDumpList()const
{
    return _HasDumpList;
}

void GTAGenericToolData::setToolDisplayName(const QString &iDispName)
{
    _ToolDisplayName = iDispName;
}

QString GTAGenericToolData::getToolDisplayName() const
{
    return _ToolDisplayName;
}

void GTAGenericToolData::setUserDB(const QList<GTAGenericDB> &iDB)
{
    _UserDB = iDB;

}

QList<GTAGenericDB> GTAGenericToolData::getUserDB()
{
    return _UserDB;
}
