#include "GTACheckVisualDisplay.h"
#include "GTACheckBase.h"
GTACheckVisualDisplay::GTACheckVisualDisplay() : GTACheckBase(GTACheckBase::VISUAL_DISPLAY)
{
}
GTACheckVisualDisplay:: ~GTACheckVisualDisplay()
{

}

void GTACheckVisualDisplay::setValue(const QString & iVal)
{
    _Value =iVal;
}

QString GTACheckVisualDisplay::getValue() const
{
    return _Value;
}

QString GTACheckVisualDisplay::getStatement() const
{
    QString oCheck = QString("check %1 \"%2\"").arg("Visual Display",getValue());
    return oCheck;
}

QList<GTACommandBase*>& GTACheckVisualDisplay::getChildren(void) const
{
	return *_lstChildren;
}
bool GTACheckVisualDisplay::insertChildren(GTACommandBase* pBase,const int& idPos)
{
    pBase;idPos;//for supressing warning.
	return false;
}

void  GTACheckVisualDisplay::setParent(GTACommandBase* iParent)
{
	_parent=iParent;
}
GTACommandBase* GTACheckVisualDisplay::getParent(void) const
{
	return _parent;
}
GTACommandBase*GTACheckVisualDisplay::getClone() const
{
    return NULL;
}
bool GTACheckVisualDisplay::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
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
