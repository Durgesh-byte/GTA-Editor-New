#include "GTACheckEcamDisplay.h"
#include "GTACheckBase.h"

GTACheckEcamDisplay::GTACheckEcamDisplay(): GTACheckBase(GTACheckBase::ECAM_DISPLAY)
{
	_DispType = "";
}
GTACheckEcamDisplay::~GTACheckEcamDisplay()
{

}
GTACheckEcamDisplay::GTACheckEcamDisplay(const QString & iType ) : GTACheckBase(GTACheckBase::ECAM_DISPLAY)
{
    setDisplayType(iType);
}
void GTACheckEcamDisplay::setDisplayType(const QString & iType)
{
    _DispType = iType;
}

void GTACheckEcamDisplay::setValue(const QString & iValue)
{
    _Value = iValue;
}

QString GTACheckEcamDisplay::getDisplayType() const
{
    return _DispType;
}

QString GTACheckEcamDisplay::getValue() const
{
    return _Value;
}

QString GTACheckEcamDisplay::getStatement() const
{
    QString oCheck = QString("check %1 %2 \"%3\"").arg("ECAM Display",getDisplayType(),getValue());
    return  oCheck ;
}
QList<GTACommandBase*>& GTACheckEcamDisplay::getChildren(void) const
{
	return *_lstChildren;
}
bool GTACheckEcamDisplay::insertChildren(GTACommandBase* pBase,const int& idPos)
{
    pBase;idPos;//for supressing warning.
	return false;
}

void  GTACheckEcamDisplay::setParent(GTACommandBase* iParent)
{
	_parent=iParent;
}
GTACommandBase* GTACheckEcamDisplay::getParent(void) const
{
	return _parent;
}
//bool GTACheckEcamDisplay::canHaveChildren() const
//{
//    return false;//true;
//}
GTACommandBase* GTACheckEcamDisplay::getClone() const
{
    return NULL;
}
bool GTACheckEcamDisplay::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
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
