#include "GTAActionFunctionReturn.h"
#include "GTAUtil.h"

#pragma warning(push, 0)
#include <QHash>
#include <QDateTime>
#pragma warning(pop)

GTAActionFunctionReturn::GTAActionFunctionReturn()
{
    setAction(ACT_FUNC_RETURN);
    

}

GTAActionFunctionReturn::~GTAActionFunctionReturn()
{

}
GTAActionFunctionReturn::GTAActionFunctionReturn(const GTAActionFunctionReturn& iCmd):GTAActionBase(iCmd)
{
    /*QString precisionVal, precisionType,ConfTime,ConfUnit;
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
    setReturnItem(iCmd.getReturnItem());
    setDumpList(iCmd.getDumpList());
}
QString GTAActionFunctionReturn::getStatement() const
{
  
        return QString("Return:%1").arg(_ReturnItem);
       
}

QList<GTACommandBase*>& GTAActionFunctionReturn::getChildren(void) const
{
	return *_lstChildren;
}
bool GTAActionFunctionReturn::insertChildren(GTACommandBase*, const int&)
{
	return false;
}

void GTAActionFunctionReturn ::setParent(GTACommandBase* iParent)
{
	_parent=iParent;
}
GTACommandBase* GTAActionFunctionReturn::getParent(void) const
{
	return _parent;
}

GTACommandBase* GTAActionFunctionReturn::getClone() const
{
    return  (new GTAActionFunctionReturn(*this));
}
bool GTAActionFunctionReturn::canHaveChildren() const
{
    return false;//true;
}

QStringList GTAActionFunctionReturn::getVariableList() const
{
    QStringList lstOfVars;
    
    bool isNum = false;
    _ReturnItem.toDouble(&isNum);

    if (_ReturnItem.startsWith("'")&& _ReturnItem.endsWith("'"))
    {
        return lstOfVars;
    }
    else if (isNum)
    {
        return lstOfVars;
    }
    else 
        lstOfVars.append(_ReturnItem);
    
    return lstOfVars;

}

void GTAActionFunctionReturn:: ReplaceTag(const QMap<QString,QString>& iTagValueMap) 
{
    iTagValueMap;
}
void GTAActionFunctionReturn::stringReplace(const QRegExp&, const QString&)
{
   
}
bool GTAActionFunctionReturn::searchString(const QRegExp&,bool) const
{
    bool rc = false;
   
    

    return rc;
}
bool GTAActionFunctionReturn::setDumpList(const QStringList&)
{
    
    return false;
}
QStringList GTAActionFunctionReturn::getDumpList()const
{
    return _dumpList;

}
QString GTAActionFunctionReturn::getSCXMLStateName()const
{
    
    QString stateName = getStatement();
    return stateName;
}
 bool GTAActionFunctionReturn::hasLoop() const
 {
    return false;
 }

 void GTAActionFunctionReturn::setReturnItem(const QString& iTime)
 {
    _ReturnItem = iTime;
 }
QString GTAActionFunctionReturn::getReturnItem()const
 {
    return _ReturnItem;
 }
 bool GTAActionFunctionReturn:: hasChannelInControl()const

 {
     if (GTACommandBase::hasChannelInCtrl(_ReturnItem))
        return true;

     return false;

 }