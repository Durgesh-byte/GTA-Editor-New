#include "GTAActionPrintTime.h"
#include "GTAUtil.h"

#pragma warning(push, 0)
#include <QHash>
#include <QDateTime>
#pragma warning(pop)

GTAActionPrintTime::GTAActionPrintTime()
{
    setAction(ACT_PRINT);
    setComplement(COM_PRINT_TIME);

}

GTAActionPrintTime::~GTAActionPrintTime()
{

}
GTAActionPrintTime::GTAActionPrintTime(const GTAActionPrintTime& rhs):GTAActionBase(rhs)
{
    /* QString precisionVal, precisionType,ConfTime,ConfUnit;
    iCmd.getPrecision(precisionVal,precisionType);
    QString timeOut, unit;
    iCmd.getTimeOut(timeOut,unit);


    setAction(iCmd.getAction());
    setComplement(iCmd.getComplement());
    setCommandType(iCmd.getCommandType());
    setActionOnFail(iCmd.getActionOnFail());
    setPrecision(precisionVal,precisionType);
    setTimeOut(timeOut,unit);
    setComment(iCmd.getComment());*/
    setTimeType(rhs.getTimeType());
    setDumpList(rhs.getDumpList());
}
QString GTAActionPrintTime::getStatement() const
{
    if(_TimeType == UTC)
        return QString("print %1: %2").arg(COM_PRINT_TIME,"Current Time in UTC");
    
    return QString("print %1: %2").arg(COM_PRINT_TIME,"Current Time");
}
QString GTAActionPrintTime::getLTRAStatement()  const
{
    if(_TimeType == UTC)
        return QString("#c#PRINT#c# %1").arg("Current Time in UTC");
    
    return QString("#c#PRINT#c# %1").arg("Current Time");
}

QList<GTACommandBase*>& GTAActionPrintTime::getChildren(void) const
{
    return *_lstChildren;
}
bool GTAActionPrintTime::insertChildren(GTACommandBase*, const int&)
{
    return false;
}

void GTAActionPrintTime ::setParent(GTACommandBase* iParent)
{
    _parent=iParent;
}
GTACommandBase* GTAActionPrintTime::getParent(void) const
{
    return _parent;
}

GTACommandBase* GTAActionPrintTime::getClone() const
{
    return  (new GTAActionPrintTime(*this));
}
bool GTAActionPrintTime::canHaveChildren() const
{
    return false;//true;
}

QStringList GTAActionPrintTime::getVariableList() const
{
    QStringList lstOfVars;
    
    return lstOfVars;

}
void GTAActionPrintTime:: ReplaceTag(const QMap<QString,QString>& iTagValueMap) 
{
    iTagValueMap;
}
void GTAActionPrintTime::stringReplace(const QRegExp&, const QString&)
{

}
bool GTAActionPrintTime::searchString(const QRegExp&, bool) const
{
    bool rc = false;

    

    return rc;
}
bool GTAActionPrintTime::setDumpList(const QStringList&)
{
    
    return false;
}
QStringList GTAActionPrintTime::getDumpList()const
{
    return _dumpList;

}
QString GTAActionPrintTime::getSCXMLStateName()const
{
    
    QString stateName = getStatement();
    return stateName;
}
bool GTAActionPrintTime::hasLoop() const
{
    return false;
}

void GTAActionPrintTime::setTimeType(TimeType iTime)
{
    _TimeType = iTime;
}
GTAActionPrintTime::TimeType GTAActionPrintTime::getTimeType()const
{
    return _TimeType;
}
