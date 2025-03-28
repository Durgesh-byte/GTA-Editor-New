#include "GTAActionBase.h"
#include "GTAUtil.h"
#include "GTACheckBase.h"
#include "GTAUtil.h"

GTAActionBase::GTAActionBase() : GTACommandBase(GTACommandBase::ACTION)
{
    _Action = QString();
    _Complement = QString();
    _TimeOut= QString();
    _Precision= QString();
    _OnFail= QString();
}
GTAActionBase::~GTAActionBase()
{

}
GTAActionBase::GTAActionBase(const GTAActionBase& rhs):GTACommandBase(rhs)
{
    _Action=rhs.getAction();
    _Complement=rhs.getComplement();
    _OnFail=rhs.getActionOnFail();
    
    QString sQty, sUnit;
    rhs.getConfCheckTime( sQty, sUnit);
    _ConfTime=sQty;
    _ConfTimeUnit = sUnit;
    sQty.clear();sUnit.clear();

    rhs.getTimeOut(sQty,sUnit);
    _TimeOut=sQty;
    _TimeUnit = sUnit;
    sQty.clear();sUnit.clear();

    rhs.getPrecision(sQty,sUnit);
    _Precision=sQty;
    _PrecisionUnit = sUnit;

}
GTAActionBase::GTAActionBase(const QString &iAction,const QString &iComplement): GTACommandBase(GTACommandBase::ACTION)
{
    _Action = iAction;
    _Complement = iComplement;
    _TimeOut= QString();
    _Precision= QString();
    _OnFail= QString();
    _ConfTime = QString("0");
    _ConfTimeUnit = ACT_TIMEOUT_UNIT_SEC;
    
}

void GTAActionBase::setAction(const QString & iAction)
{
    _Action = iAction;
}
void GTAActionBase::setComplement(const QString & iComplement)
{
    _Complement = iComplement;
}

void GTAActionBase::setActionOnFail(const QString & iActionOnFail)
{
    _OnFail = iActionOnFail;
}
QString GTAActionBase::getAction() const
{
    return _Action;
}
QString GTAActionBase::getComplement() const
{
    return _Complement;
}


QString GTAActionBase::getActionOnFail() const
{
    return _OnFail;
}
QColor GTAActionBase::getForegroundColor() const
{
    return QColor(Qt::black);
}
/**
  * If the Command is not valid then after compatibility check
  * the background color of the Command changes.
  * it gets reset once the command becomes valid.
  */
QColor GTAActionBase::getBackgroundColor() const
{
    return QColor();

}
QFont GTAActionBase::getFont() const
{
    return QFont();
}
void GTAActionBase::setConfCheckTime(const QString & iTime,const QString& iUnit)
{
    _ConfTime = iTime;
    _ConfTimeUnit = iUnit;
}
void GTAActionBase::getConfCheckTime(QString & oTimeOut , QString& oTimeUnit) const
{
    oTimeOut= _ConfTime;
    oTimeUnit= _ConfTimeUnit;
}
void GTAActionBase::setTimeOut(const QString & iTimeOut,const QString& iTimeUnit)
{
    if(hasTimeOut())
    {
        _TimeOut = iTimeOut;
        _TimeUnit = iTimeUnit;
    }
}

void GTAActionBase::setPrecision(const QString & iPrecision , const QString& iPrecisionUnit)
{ 
    if(hasPrecision())
    {
        _Precision = iPrecision;
        _PrecisionUnit = iPrecisionUnit;
    }
}
void GTAActionBase::getTimeOut(QString & oTimeOut , QString& oTimeUnit) const
{
    oTimeOut= _TimeOut;
    oTimeUnit= _TimeUnit;
}
double GTAActionBase::getTimeOutInMs() const
{

    if ( this->hasTimeOut() && _TimeOut.isEmpty() == false)
    {
        bool isNUM;
        double timeOutVal = _TimeOut.toDouble(&isNUM);
        if(isNUM)
        {

            if(_TimeUnit==ACT_TIMEOUT_UNIT_SEC)
                timeOutVal = timeOutVal*1000;

            return timeOutVal;


        }

    }

    return 0.0;
}
void GTAActionBase::getPrecision(QString & oPrecision , QString& oPrecisionUnit) const
{

    oPrecision = _Precision;
    oPrecisionUnit = _PrecisionUnit;


}
// QStringList GTAActionBase::getVariableList() const
//{
//    return QStringList();
//
//}
//
// void GTAActionBase:: ReplaceTag(const QMap<QString,QString>& iTagValueMap) 
// {
//
// }

bool  GTAActionBase::isManuallyAdded()
{
    return _isManuallyAdded;
}
void  GTAActionBase::setIsManuallyAdded(bool val)
{
    _isManuallyAdded = val;
}


bool GTAActionBase::searchText(const QRegExp& iRegExp)
{
    bool rc;
    QString sTimeOut = QString("%1 %2").arg(_TimeOut,_TimeUnit);
    QString sPrecision = QString("%1 %2").arg(_Precision,_PrecisionUnit);

    if (sTimeOut.contains(iRegExp))
        rc = true;
    else if ( sPrecision.contains(iRegExp))
        rc = true;
    else if ( _OnFail.contains(iRegExp))
        rc = true;
    else if ( getCommandTypeForDisplay().contains(iRegExp))
        rc = true;
    else
        rc = false;

    return rc;

}
void  GTAActionBase::setPrecisonforAllChildren(const double& dOffset)
{

    QString sPrecision, sPrecionUnit;
    this->getPrecision(sPrecision, sPrecionUnit);
    bool isDouble = false;
    double precison = sPrecision.toDouble(&isDouble);
    if (isDouble)
    {
        precison  = precison +dOffset;
        if (this->hasChildren())
        {
            QList<GTACommandBase*>& lstOfChildrens = this->getChildren();
            int childrenSize = lstOfChildrens.size();
            if (childrenSize)
            {
                for (int i=0;i<childrenSize;i++)
                {
                    GTACommandBase* pCurCmd = lstOfChildrens.at(i);
                    GTAActionBase* pAction = dynamic_cast<GTAActionBase*> (pCurCmd);
                    if (pAction)
                    {


                        pAction->setPrecision(QString::number(precison),sPrecionUnit);
                        if(pAction->hasChildren())
                            pAction->setPrecisonforAllChildren(dOffset);
                    }
                }
            }
        }
    }
}
void  GTAActionBase::setTimeOutsforAllChildren(const double& dOffset)
{

    QString sTimeOut, sTimeOutUnit;
    this->getTimeOut(sTimeOut, sTimeOutUnit);
    bool isDouble = false;
    double timeOut = sTimeOut.toDouble(&isDouble);
    if (isDouble)
    {
        timeOut  = timeOut +dOffset;
        if (this->hasChildren())
        {
            QList<GTACommandBase*>& lstOfChildrens = this->getChildren();
            int childrenSize = lstOfChildrens.size();
            if (childrenSize)
            {
                for (int i=0;i<childrenSize;i++)
                {
                    GTACommandBase* pCurCmd = lstOfChildrens.at(i);
                    GTAActionBase* pAction = dynamic_cast<GTAActionBase*> (pCurCmd);
                    if (pAction)
                    {

                        if(pAction->hasTimeOut())
                        {
                            pAction->setTimeOut(QString::number(timeOut),sTimeOutUnit);
                            if(pAction->hasChildren())
                                pAction->setTimeOutsforAllChildren(dOffset);
                        }
                    }
                }
            }
        }
    }
}
void GTAActionBase::addTimeOut(const double& idTimeOut, const QString& iUnit)
{


    if(_TimeOut.isEmpty()==false &&  this->hasTimeOut())
    {
        double dCurrentTimeOut = _TimeOut.toDouble();
        if(_TimeUnit==ACT_TIMEOUT_UNIT_SEC)
            dCurrentTimeOut = dCurrentTimeOut*1000;

        double timeOutToAdd = idTimeOut;
        if (iUnit == ACT_TIMEOUT_UNIT_SEC)
            timeOutToAdd = timeOutToAdd*1000;

        double totalTimeOut =  dCurrentTimeOut + timeOutToAdd;

        if (_TimeUnit == ACT_TIMEOUT_UNIT_SEC)
            totalTimeOut = totalTimeOut/1000;

        setTimeOut(QString::number(totalTimeOut),_TimeUnit);

        if (this->hasChildren())
        {
            QList<GTACommandBase* >& chldLst =  this->getChildren();
            if (chldLst.isEmpty()==false)
            {
                for (int i=0;i<chldLst.size();i++)
                {
                    GTACommandBase* pCmd = chldLst.at(i);
                    GTAActionBase* pActBase = dynamic_cast<GTAActionBase*>(pCmd);
                    if (pActBase)
                        pActBase->addTimeOut(idTimeOut,iUnit);

                    GTACheckBase* pChk = dynamic_cast<GTACheckBase*>(pCmd);
                    if (pChk)
                        pChk->addTimeOut(idTimeOut,iUnit);
                    
                }
            }
        }
    }

}

/**
 * @brief This function set a value at the foreground text cell. 
 * 
 * @param iColor, color load with csv file. 
 */
void GTAActionBase::setForegroundColor(const QString& iColor){
    _Color = GTAUtil::getTextColor(iColor);
}
