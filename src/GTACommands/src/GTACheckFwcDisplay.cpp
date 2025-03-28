#include "GTACheckFwcDisplay.h"
#include "GTACheckBase.h"
#include  "GTACheckFwcWarning.h"
GTACheckFwcDisplay::GTACheckFwcDisplay() 
{
	setCheckType(GTACheckBase::FWC_DISPLAY);
}
QString GTACheckFwcDisplay::getStatement() const
{
    QString oCheck = QString("check %1 \"%2\" is %3 displayed on %4").arg("FWC Display",getMessage(),getCondition(),getOnDisplay());
    return oCheck;
}


GTACommandBase* GTACheckFwcDisplay::getClone() const
{
	return(new GTACheckFwcDisplay(*this));
}
GTACheckFwcDisplay::GTACheckFwcDisplay(const GTACheckFwcDisplay& rhs) 
{
	QString sTimeOut, sTimeUnit, sPrecision, sPrecisionUnit;
	rhs.getTimeOut(sTimeOut,sTimeUnit);
	rhs.getPrecision(sPrecision,sPrecisionUnit);

	setTimeOut(sTimeOut,sTimeUnit);
	setPrecision(sPrecision,sPrecisionUnit);
	setActionOnFail(rhs.getActionOnFail());


	setMessage	(rhs.getMessage());
	setCondition(rhs.getCondition());
	setOnDisplay(rhs.getOnDisplay());
	setCheckType(GTACheckBase::FWC_DISPLAY);
     this->setComment(rhs.getComment());

}
//bool GTACheckFwcDisplay::canHaveChildren() const
//{
//    return false;//true;
//}
bool GTACheckFwcDisplay::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
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
