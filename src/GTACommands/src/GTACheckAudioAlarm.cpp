#include "GTACheckAudioAlarm.h"
#include "GTACheckBase.h"
GTACheckAudioAlarm::GTACheckAudioAlarm() : GTACheckBase(GTACheckBase::AUDIO_ALARM)
{

}
GTACheckAudioAlarm::GTACheckAudioAlarm(const GTACheckAudioAlarm& rhs): GTACheckBase(rhs)
{
    //QString sTimeOut, sTimeUnit, sPrecision, sPrecisionUnit;
    //rhs.getTimeOut(sTimeOut,sTimeUnit);
    //rhs.getPrecision(sPrecision,sPrecisionUnit);

    //setTimeOut(sTimeOut,sTimeUnit);


    //setPrecision(sPrecision,sPrecisionUnit);
    //setActionOnFail(rhs.getActionOnFail());
    
    setValue(rhs.getAlarmName());
    setTime(rhs.getTime());
    setDumpList(rhs.getDumpList());
    /*this->setComment(rhs.getComment());*/
}
GTACheckAudioAlarm::~GTACheckAudioAlarm()
{

}

void GTACheckAudioAlarm::setValue(const QString & iVal)
{
    _Value = iVal;
}

void GTACheckAudioAlarm::setTime(const QString & iVal)
{
    _Time = iVal;
}


QString GTACheckAudioAlarm::getAlarmName() const
{
    return _Value;
}

QString GTACheckAudioAlarm::getTime() const
{
    return _Time;
}

QString GTACheckAudioAlarm::getStatement() const
{
    QString oCheck = QString("check %1 of type %2 is heard after %4s").arg("Audio Alarm",getAlarmName(),getTime());
    return  oCheck ;
}
QString GTACheckAudioAlarm::getSCXMLStateName()const
{

    
    return getStatement();

}
QList<GTACommandBase*>& GTACheckAudioAlarm::getChildren(void) const
{
	return *_lstChildren;
}
bool GTACheckAudioAlarm::insertChildren(GTACommandBase*, const int&)
{
	return false;
}

void  GTACheckAudioAlarm::setParent(GTACommandBase* iParent)
{
	_parent=iParent;
}
GTACommandBase* GTACheckAudioAlarm::getParent(void) const
{
	return _parent;
}
GTACommandBase* GTACheckAudioAlarm::getClone() const
{
    return new GTACheckAudioAlarm(*this);
}
bool GTACheckAudioAlarm::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
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
bool GTACheckAudioAlarm::setDumpList(const QStringList& iDumpList)
{
    _dumpList.clear();
    _dumpList.append(iDumpList);
    return true;
}
QStringList GTACheckAudioAlarm::getDumpList()const
{
    return _dumpList;

}
