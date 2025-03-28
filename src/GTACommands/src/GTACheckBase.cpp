#include "GTACheckBase.h"
#include "GTAUtil.h"
#include "GTAActionBase.h"
GTACheckBase::GTACheckBase(CheckType iType): GTACommandBase(GTACommandBase::CHECK)
{
    _Check = iType;
    _TimeOut = "";
    _Precision="";
    _OnFail="";
    _ConfTime = QString("0");
    _ConfTimeUnit = ACT_TIMEOUT_UNIT_SEC;
}
GTACheckBase::GTACheckBase(const GTACheckBase& rhs):GTACommandBase(rhs)
{
    QString sQty,sQtyUnit;
   
    rhs.getTimeOut(sQty,sQtyUnit);
    _TimeOut=sQty;
    _TimeUnit=sQtyUnit;
    sQty.clear();sQtyUnit.clear();
    
    
    rhs.getPrecision(sQty,sQtyUnit);
    _Precision=sQty;
    _PrecisionUnit=sQtyUnit;
    sQty.clear();sQtyUnit.clear();
    
    rhs.getConfCheckTime(sQty,sQtyUnit);
    _ConfTime=sQty;
    _ConfTimeUnit= sQtyUnit;
    
    _OnFail=rhs.getActionOnFail();
    _Check=rhs.getCheckType();
}
GTACheckBase::~GTACheckBase()
{

}

GTACheckBase::CheckType GTACheckBase::getCheckType() const
{
    return _Check;
}

void GTACheckBase::setTimeOut(const QString & iTimeOut,const QString& iTimeUnit)
{
    _TimeOut = iTimeOut;
	_TimeUnit = iTimeUnit;
}
void GTACheckBase::setConfCheckTime(const QString & iTime,const QString& iUnit)
{
    _ConfTime = iTime;
    _ConfTimeUnit = iUnit;
}
void GTACheckBase::getTimeOut(QString & oTimeOut , QString& oTimeUnit) const
{
    oTimeOut= _TimeOut;
    oTimeUnit= _TimeUnit;
}
void GTACheckBase::setPrecision(const QString & iPrecision , const QString& iPrecisionUnit)
{
    _Precision = iPrecision;
	_PrecisionUnit = iPrecisionUnit;
}

void GTACheckBase::setActionOnFail(const QString & iActionOnFail)
{
    _OnFail = iActionOnFail;
}


void GTACheckBase::getConfCheckTime( QString & oTime, QString& oUnit) const
{
    oTime=_ConfTime ;
    oUnit =_ConfTimeUnit ;
}
void GTACheckBase::getPrecision(QString & oPrecision , QString& oPrecisionUnit) const
{
   oPrecision = _Precision;
   oPrecisionUnit = _PrecisionUnit;

}
QString GTACheckBase::getActionOnFail() const
{
    return _OnFail;
}
QColor GTACheckBase::getForegroundColor() const
{
    return QColor(Qt::darkGreen);
}
/**
  * If the Command is not valid then after compatibility check
  * the background color of the Command changes.
  * it gets reset once the command becomes valid.
  */
QColor GTACheckBase::getBackgroundColor() const
{
    return QColor();//QColor(255,255,255);
  
}
QFont GTACheckBase::getFont() const
{
    return QFont();
}
bool GTACheckBase::canHaveChildren() const
{
    return false;//true;
}

QStringList GTACheckBase::getVariableList() const
{
    return QStringList();
}
void GTACheckBase:: ReplaceTag(const QMap<QString,QString>& iTagValueMap) 
{
    iTagValueMap;//to suppress warning

}

//CHK_VALUE "Value"
//#define CHK_FWC_WARN "FWC Warning"
//#define CHK_FWC_PROC "FWC Procedure"
//#define CHK_FWC_DISP "FWC Display"
//#define CHK_BITE_MSG "BITE Message"
//#define CHK_ECAM_DISP "ECAM Display"
//#define CHK_AUDIO_ALRM "Audio Alarm"
//#define CHK_VISU_DISP "Visual Display"
//#define CHK_MANUAL_CHECK "Manual Check"
QString GTACheckBase::getCheckName()const
{
    QString checkType;
    switch (_Check)
    {
        case VALUE:
            checkType=CHK_VALUE;
            break;
        case FWC_WARNING:
             checkType=CHK_FWC_WARN;
            break;
        case FWC_PROCEDURE:
             checkType=CHK_FWC_PROC;
            break;
        case FWC_DISPLAY:
             checkType=CHK_FWC_DISP;
            break;
        case BITE_MESSAGE:
             checkType=CHK_BITE_MSG;
            break;
        case ECAM_DISPLAY:
             checkType=CHK_ECAM_DISP;
            break;
        case AUDIO_ALARM:
             checkType=CHK_AUDIO_ALRM;
            break;
        case VISUAL_DISPLAY:
             checkType=CHK_VISU_DISP;
            break;
        case MANUAL_CHECK:
             checkType=CHK_MANUAL_CHECK;
            break;
        case REFRESH:
            checkType=CHK_REFRESH;
            break;
        default:
            checkType=QString();
    }

    return checkType;
}
bool GTACheckBase::searchText(const QRegExp& iRegExp)
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
void GTACheckBase::addTimeOut(const double& idTimeOut, const QString& iUnit)
{


    if(_TimeOut.isEmpty()==false )
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

double GTACheckBase::getTimeOutInMs() const
{

    if ( _TimeOut.isEmpty() == false)
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
