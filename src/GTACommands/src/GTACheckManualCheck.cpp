#include "GTACheckManualCheck.h"
#include "GTACheckVisualDisplay.h"
#include "GTACheckBase.h"

GTACheckManualCheck::GTACheckManualCheck()  //: GTACheckBase(GTACheckBase::MANUAL_CHECK)
{
	setCheckType(GTACheckBase::MANUAL_CHECK);
}
QString GTACheckManualCheck::getStatement() const
{
    QString oCheck = QString("check that \"%1\"").arg(getValue());
    return oCheck;
}
GTACommandBase*GTACheckManualCheck::getClone() const
{
    return NULL;
}
bool GTACheckManualCheck::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
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
