#include "GTAFailProfileExp.h"
#include "GTAUtil.h"


GTAFailProfileExp::GTAFailProfileExp():GTAFailureProfileBase(GTAFailureProfileBase::EXPONENT)
{
}
GTAFailProfileExp::GTAFailProfileExp(const GTAFailProfileExp& rhs):GTAFailureProfileBase(GTAFailureProfileBase::EXPONENT)
{
    _start=rhs.getStart();
    _stop=rhs.getStop();
    _tau=rhs.getTau();
    

}
GTAFailProfileExp::~GTAFailProfileExp()
{

}
void GTAFailProfileExp::setStart(const QString &iVal)
{
    _start=iVal;
}
void GTAFailProfileExp::setStop(const QString &iVal)
{
    _stop=iVal;
}
void GTAFailProfileExp::setTau(const QString &iVal)
{
    _tau=iVal;
}


QString GTAFailProfileExp::getStart() const
{
    return _start;
}
QString GTAFailProfileExp::getStop() const
{
    return _stop;
}
QString GTAFailProfileExp::getTau() const
{
    return _tau;
}

QString GTAFailProfileExp::getStatement() const
{
    QString sVarVal1= QString("%1=%2").arg(XATTR_EQ_START,_start);
    QString sVarVal2= QString("%1=%2").arg(XATTR_EQ_STOP,_stop);
    QString sVarVal3= QString("%1=%2").arg(XATTR_EQ_TAU,_tau); 
    QString oEquation = QString("%1(%2,%3,%4)").arg(EQ_EXP,sVarVal1,sVarVal2,sVarVal3);


    return oEquation;
}
GTAFailureProfileBase* GTAFailProfileExp:: getClone()const
{
    return new GTAFailProfileExp(*this);
}
void  GTAFailProfileExp::getFunctionStatement(QString&, QString& osFunctionStatement) const
{
    
    //osFuncType=QString(EQ_EXP).toUpper();
    //QString sVarVal1= QString("'%1':'%2'").arg(XATTR_EQ_START,_start);
    //QString sVarVal2= QString("'%1':'%2'").arg(XATTR_EQ_STOP,_stop);
    //QString sVarVal3= QString("'%1':'%2'").arg(XATTR_EQ_TAU,_tau);

    osFunctionStatement = QString("'type':'EXPONENT','mode':2,'start':%1,'stop':%2,'tau':%3").arg(_start,_stop,_tau);
    //iSerialzationMap.insert()
}
void  GTAFailProfileExp::getFailureProfileArguments(QHash<QString,QString>& iSerialzationMap)const
{
     iSerialzationMap.insert(XATTR_EQ_START,_start);
     iSerialzationMap.insert(XATTR_EQ_STOP,_stop);
     iSerialzationMap.insert(XATTR_EQ_TAU,_tau);


}
void  GTAFailProfileExp::setFailureProfileArguments(const QHash<QString,QString>& iSerialzationMap)
{
    
     if (iSerialzationMap.contains(XATTR_EQ_STOP))
         _stop=iSerialzationMap.value(XATTR_EQ_STOP);

    

     if (iSerialzationMap.contains(XATTR_EQ_TAU))
         _tau=iSerialzationMap.value(XATTR_EQ_TAU);

     if (iSerialzationMap.contains(XATTR_EQ_START))
         _start=iSerialzationMap.value(XATTR_EQ_START);

}
//QString GTAFailProfileExp::getRampModeType(void)const
//{
//    QString sRampMode;
//    switch (_rampMode)
//    {
//    case GTAFailProfileExp::INVERTED:
//        sRampMode="Inverted";
//        break;
//    case GTAFailProfileExp::RAMPUP:
//        sRampMode="RampUp";
//        break;
//    }
//    return sRampMode;
//}

QStringList GTAFailProfileExp::getVariableList() const
{
    QStringList varList;

    if (GTACommandBase::isVariable(_start))
        varList<<_start;
    if (GTACommandBase::isVariable(_stop))
        varList<<_stop;
    if (GTACommandBase::isVariable(_tau))
        varList<<_tau;

   

    return varList;
}
void GTAFailProfileExp:: ReplaceTag(const QMap<QString,QString>& iTagValueMap) 
{
    QMapIterator<QString,QString> iterTag(iTagValueMap);
    while (iterTag.hasNext())
    {
        iterTag.next();
        QString tag = QString("%1%2%3").arg(QChar(TAG_IDENTIFIER),iterTag.key(),QChar(TAG_IDENTIFIER));

        if (GTACommandBase::isVariable(_start))
            _start.replace(tag,iterTag.value());
        if (GTACommandBase::isVariable(_stop))
            _stop.replace(tag,iterTag.value());
        if (GTACommandBase::isVariable(_tau))
            _tau.replace(tag,iterTag.value());
        
       

    }
}