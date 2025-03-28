#include "GTACheckBiteMessage.h"
#include "GTACheckBase.h"
GTACheckBiteMessage::GTACheckBiteMessage() : GTACheckBase(GTACheckBase::BITE_MESSAGE)
{
}
GTACheckBiteMessage::GTACheckBiteMessage(const GTACheckBiteMessage& rhs): GTACheckBase(rhs)
{
	/*QString sTimeOut, sTimeUnit, sPrecision, sPrecisionUnit;
	rhs.getTimeOut(sTimeOut,sTimeUnit);
	rhs.getPrecision(sPrecision,sPrecisionUnit);

	setTimeOut(sTimeOut,sTimeUnit);
	setPrecision(sPrecision,sPrecisionUnit);
	setActionOnFail(rhs.getActionOnFail());*/


	setMessage(rhs.getMessage());
	setCondition(rhs.getCondition());
	setOnReport(rhs.getOnReport());
    setBuffDepth(rhs.getBuffDepth());
    setWaitStatus(rhs.getWaitStatus());
    setDumpList(rhs.getDumpList());
    // this->setComment(rhs.getComment());
}
GTACheckBiteMessage::~GTACheckBiteMessage()
{

}
void GTACheckBiteMessage::setMessage(const QString & iVal)
{
    _Message = iVal;
}

void GTACheckBiteMessage::setCondition(const QString & iVal)
{
    _Condition = iVal;
}

void GTACheckBiteMessage::setOnReport(const QString & iVal)
{
    _OnReport = iVal;
}

QString GTACheckBiteMessage::getMessage() const
{
    return _Message;
}
QString GTACheckBiteMessage::getCondition() const
{
    return _Condition;
}
QString GTACheckBiteMessage::getOnReport() const
{
    return _OnReport;
}

QString GTACheckBiteMessage::getStatement() const
{
    QString oCheck = QString("check %1 \"%2\" is %3 displayed on %4").arg("BITE Message",getMessage(),getCondition(),getOnReport());
    return oCheck;
}

QList<GTACommandBase*>& GTACheckBiteMessage::getChildren(void) const
{
	return *_lstChildren;
}
bool GTACheckBiteMessage::insertChildren(GTACommandBase*, const int&)
{
	return false;
}

void  GTACheckBiteMessage::setParent(GTACommandBase* iParent)
{
	_parent=iParent;
}
GTACommandBase* GTACheckBiteMessage::getParent(void) const
{
	return _parent;
}
GTACommandBase* GTACheckBiteMessage::getClone() const
{
	return(new GTACheckBiteMessage(*this));
}
void GTACheckBiteMessage::setWaitStatus(const bool & iVal)
{
    _waitForAck = iVal;

}
void GTACheckBiteMessage::setBuffDepth(const QString & iVal)
{
    _bufferDepth = iVal;
}
bool GTACheckBiteMessage::getWaitStatus() const
{
    return _waitForAck;
}
QString GTACheckBiteMessage::getBuffDepth() const
{
    return _bufferDepth;
}
//bool GTACheckBiteMessage::canHaveChildren() const
//{
//    return false;//true;
//}
bool GTACheckBiteMessage::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
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
bool GTACheckBiteMessage::setDumpList(const QStringList& iDumpList)
{
    _dumpList.clear();
    _dumpList.append(iDumpList);
    return true;
}
QStringList GTACheckBiteMessage::getDumpList()const
{
    return _dumpList;

}
QString GTACheckBiteMessage::getSCXMLStateName()const
{
 return getStatement();
}
