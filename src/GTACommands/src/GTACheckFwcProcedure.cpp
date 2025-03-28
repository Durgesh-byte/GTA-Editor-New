#include "GTACheckFwcProcedure.h"
#include "GTACheckBase.h"
GTACheckFwcProcedure::GTACheckFwcProcedure() :GTACheckBase(GTACheckBase::FWC_PROCEDURE)
{
}
GTACheckFwcProcedure::~GTACheckFwcProcedure()
{

}
GTACheckFwcProcedure::GTACheckFwcProcedure(const GTACheckFwcProcedure& rhs):GTACheckBase(rhs)
{
  // setActionOnFail(rhs.getActionOnFail());
}
void GTACheckFwcProcedure::setCondition(const QString & iVal)
{
    _Condition = iVal;
}
 //void GTACheckFwcProcedure::setOnDisplay(const QString & iVal)
 //{
 //   _OnDisplay = iVal;
 //}

void GTACheckFwcProcedure::insertItem(int index, const QString & iVal)
{
    _ProcTable.insert(index,iVal);
}

QString GTACheckFwcProcedure::getCondition() const
{
    return _Condition;
}

//QString GTACheckFwcProcedure::getOnDisplay() const
//{
//    return _OnDisplay;
//}
int GTACheckFwcProcedure::getCount() const
{
    return  _ProcTable.count();
}

QString GTACheckFwcProcedure::getItem(int index) const
{
    QString oVal;
    if(_ProcTable.contains(index))
    {
        oVal = _ProcTable.value(index);
    }
    return oVal;
}

QString GTACheckFwcProcedure::getStatement() const
{
    QString oCheck = "check FWC Procedure ";
    for(int i = 0 ; i< _ProcTable.count();i++)
    {

        oCheck += "\"" +  getItem(i) + "\" ";
    }
    oCheck += " is " + getCondition() /*+ " displayed on " + getOnDisplay()*/;
    return oCheck;
}
QList<GTACommandBase*>& GTACheckFwcProcedure::getChildren(void) const
{
	return *_lstChildren;
}
bool GTACheckFwcProcedure::insertChildren(GTACommandBase*, const int&)
{
	return false;
}

void  GTACheckFwcProcedure::setParent(GTACommandBase* iParent)
{
	_parent=iParent;
}
GTACommandBase* GTACheckFwcProcedure::getParent(void) const
{
	return _parent;
}
GTACommandBase* GTACheckFwcProcedure::getClone() const
{
    return NULL;
}
bool GTACheckFwcProcedure::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
{
    bool rc;

    if (ibTextSearch && this->getComment().contains(iRegExp))
        rc = true;
    else if (ibTextSearch && this->getStatement().contains(iRegExp))
        rc = true;
    else
        rc=false;
    return rc;
}
