#include "GTACheckRefresh.h"
#include "GTACheckBase.h"
#include "GTAUtil.h"
GTACheckRefresh::GTACheckRefresh() : GTACheckBase(GTACheckBase::REFRESH)
{
   
}
GTACheckRefresh::GTACheckRefresh(const GTACheckRefresh& rhs): GTACheckBase(GTACheckBase::REFRESH)
{
    QString sTimeOut, sTimeUnit, sPrecision, sPrecisionUnit;
    rhs.getTimeOut(sTimeOut,sTimeUnit);
    rhs.getPrecision(sPrecision,sPrecisionUnit);

    setTimeOut(sTimeOut,sTimeUnit);


    setPrecision(sPrecision,sPrecisionUnit);
    setActionOnFail(rhs.getActionOnFail());
    
    setParameter(rhs.getParameter());
    setRefreshTime(rhs.getRefreshTime());
    setDumpList(rhs.getDumpList());
    this->setComment(rhs.getComment());
}
GTACheckRefresh::~GTACheckRefresh()
{

}

void GTACheckRefresh::setParameter(const QString & iVal)
{
    _Parameter = iVal;
}

void GTACheckRefresh::setRefreshTime(const QString & iVal)
{
    _RefreshValue = iVal;
}


QString GTACheckRefresh::getParameter() const
{
    return _Parameter;
}

QString GTACheckRefresh::getRefreshTime() const
{
    return _RefreshValue;
}

QString GTACheckRefresh::getStatement() const
{
    QString oCheck = QString("Check %1 of %2 is %3mSecs").arg(CHK_REFRESH,getParameter(),getRefreshTime());
    return  oCheck ;
}
QString GTACheckRefresh::getSCXMLStateName()const
{

    
    return getStatement();

}
QList<GTACommandBase*>& GTACheckRefresh::getChildren(void) const
{
	return *_lstChildren;
}
bool GTACheckRefresh::insertChildren(GTACommandBase*, const int&)
{
	return false;
}

void  GTACheckRefresh::setParent(GTACommandBase* iParent)
{
	_parent=iParent;
}
GTACommandBase* GTACheckRefresh::getParent(void) const
{
	return _parent;
}
GTACommandBase* GTACheckRefresh::getClone() const
{
    return new GTACheckRefresh(*this);
}
bool GTACheckRefresh::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
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
bool GTACheckRefresh::setDumpList(const QStringList& iDumpList)
{
    _dumpList.clear();
    _dumpList.append(iDumpList);
    return true;
}
QStringList GTACheckRefresh::getDumpList()const
{
    return _dumpList;

}
