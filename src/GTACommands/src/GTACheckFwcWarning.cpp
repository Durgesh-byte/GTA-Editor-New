#include "GTACheckFwcWarning.h"
#include "GTACheckBase.h"
GTACheckFwcWarning::GTACheckFwcWarning() : GTACheckBase(GTACheckBase::FWC_WARNING)
{
}
GTACheckFwcWarning::~GTACheckFwcWarning()
{

}
GTACheckFwcWarning::GTACheckFwcWarning(const GTACheckFwcWarning& rhs): GTACheckBase(rhs)
{
    QString sTimeOut, sTimeUnit, sPrecision, sPrecisionUnit, ConfTime,ConfUnit;
    rhs.getTimeOut(sTimeOut,sTimeUnit);
    rhs.getPrecision(sPrecision,sPrecisionUnit);

    setTimeOut(sTimeOut,sTimeUnit);
    setPrecision(sPrecision,sPrecisionUnit);
    setActionOnFail(rhs.getActionOnFail());

    setIgnoreInSCXML(rhs.isIgnoredInSCXML());

    setMessage	(rhs.getMessage());
    setCondition(rhs.getCondition());
    setOnDisplay(rhs.getOnDisplay());
    setWaitForBuffer(rhs.getWaitForBuffer());
    setWaitAck(rhs.getWaitAck());
    setHeader(rhs.getHeader());

    setFWCWarningColor(rhs.getFWCWarningColor());
    setDumpList(rhs.getDumpList());
    rhs.getConfCheckTime(ConfTime,ConfUnit);

    setConfCheckTime(ConfTime,ConfUnit);
    this->setComment(rhs.getComment());

	this->setInstanceName(rhs.getInstanceName());

}

void GTACheckFwcWarning::setMessage(const QString & iVal)
{
    _Message = iVal;
}

void GTACheckFwcWarning::setCondition(const QString & iVal)
{
    _Condition = iVal;
}
void GTACheckFwcWarning::setOnDisplay(const QString &iVal)
{
    _OnDisplay= iVal;
}

QString GTACheckFwcWarning::getMessage() const
{
    return _Message;
}
QString GTACheckFwcWarning::getCondition() const
{
    return _Condition;
}

QString GTACheckFwcWarning::getOnDisplay() const
{
    return _OnDisplay;
}

QString GTACheckFwcWarning::getStatement() const
{
    QString heading = getHeader();
    QString oCheck= "";
    if(heading.trimmed().isEmpty())
    {
       oCheck = QString("Check %1 Message \"%2\" with Heading \"%3\" is %4 displayed").arg("FWC Warning",getMessage(),getHeader(),getCondition());
    }
    else
    {
        oCheck = QString("Check %1 Message \"%2\" is %3 displayed").arg("FWC Warning",getMessage(),getCondition());
    }
    return oCheck;
}
QString GTACheckFwcWarning::getLTRAStatement() const
{
    QString heading = getHeader();
    QString color = _WarningColor;

    if(!color.trimmed().isEmpty())
    {
        color = "#"+color+"#";
    }
    QString oCheck= "";
    if(!color.trimmed().isEmpty())
    {
        oCheck = QString("#c#Check#c# %1 Message \"#color#%5%3 %2%5#color#\" is %4 displayed").arg("FWC Warning",getMessage(),heading,getCondition(),color);
    }
    else
    {
        oCheck = QString("#c#Check#c# %1 Message \"%3 %2\" is %4 displayed").arg("FWC Warning",getMessage(),heading,getCondition());
    }
    return oCheck;
}
QString GTACheckFwcWarning::getSCXMLStateName() const
{
    return getStatement();
}
QList<GTACommandBase*>& GTACheckFwcWarning::getChildren(void) const
{
    return *_lstChildren;
}
bool GTACheckFwcWarning::insertChildren(GTACommandBase* pBase,const int& idPos)
{
    pBase;idPos;//for supressing warning.
    return false;
}	

void  GTACheckFwcWarning::setParent(GTACommandBase* iParent)
{
    _parent=iParent;
}
GTACommandBase* GTACheckFwcWarning::getParent(void) const
{
    return _parent;
}
GTACommandBase* GTACheckFwcWarning::getClone() const
{
    return(new GTACheckFwcWarning(*this));
}
QString GTACheckFwcWarning::getHeader()const
{
    return _Header;
}
QString GTACheckFwcWarning::getWaitForBuffer()const
{
    return _WaitForBuffer;

}
bool GTACheckFwcWarning::getWaitAck()const
{
    return _WaitAck;
}

void GTACheckFwcWarning::setHeader(const QString & iVal)
{
    _Header=iVal;
}
void GTACheckFwcWarning::setWaitForBuffer(const QString & iVal)
{
    _WaitForBuffer=iVal;
}
void GTACheckFwcWarning::setWaitAck(const bool& iVal)
{
    _WaitAck=iVal;
}
void GTACheckFwcWarning::setFWCWarningColor(const QString &iColor)
{
    _WarningColor = iColor;
}

QString GTACheckFwcWarning::getFWCWarningColor() const
{
    return _WarningColor;
}
bool GTACheckFwcWarning::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
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
bool GTACheckFwcWarning::setDumpList(const QStringList& iDumpList)
{
    _dumpList.clear();
    _dumpList.append(iDumpList);
    return true;
}
QStringList GTACheckFwcWarning::getDumpList()const
{
    return _dumpList;

}
