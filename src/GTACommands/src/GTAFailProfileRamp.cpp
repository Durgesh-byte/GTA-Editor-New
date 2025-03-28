#include "GTAFailProfileRamp.h"
#include "GTAUtil.h"

GTAFailProfileRamp::GTAFailProfileRamp():GTAFailureProfileBase(GTAFailureProfileBase::RAMP)
{

}
GTAFailProfileRamp::GTAFailProfileRamp(const GTAFailProfileRamp& rhs):GTAFailureProfileBase(GTAFailureProfileBase::RAMP)
{
    _Stop=rhs.getStop();
    _Start=rhs.getStart();
    
   
}
GTAFailProfileRamp::~GTAFailProfileRamp()
{
       
}
void GTAFailProfileRamp::setStop(const QString &iVal)
{
 _Stop=iVal;
}

void GTAFailProfileRamp::setStart(const QString &iVal)
{
 _Start=iVal;
}


QString GTAFailProfileRamp::getStop() const
{
    return _Stop;
}

QString GTAFailProfileRamp::getStart() const
{
    return _Start;
}


QString GTAFailProfileRamp::getStatement() const
{
    
    QString sVarVal1= 		QString("%1=%2").arg(XATTR_EQ_STOP,_Stop);
    QString sVarVal2= 		QString("%1=%2").arg(XATTR_EQ_START,_Start);
  
    QString oEquation = QString("%1(%2,%3)").arg(EQ_RAMP,sVarVal1,sVarVal2);
    return oEquation;
}
 GTAFailureProfileBase* GTAFailProfileRamp:: getClone()const
{
	return new GTAFailProfileRamp(*this);
}
void  GTAFailProfileRamp::getFunctionStatement(QString&, QString& osFunctionStatement) const
{
    //osFuncType=QString(EQ_SQUARE).toUpper();
   
    //QString sVarVal1= QString("'%1':'%2'").arg(XATTR_EQ_STOP);
    //QString sVarVal2= QString("'%1':'%2'").arg(XATTR_EQ_START,_Start);
    //

    //osFunctionStatement = QString("%1{%2,%3}").arg("'FunctionValue':",sVarVal1,sVarVal2);

    osFunctionStatement=QString("'type':'RAMP','mode':2,'start':%1,'stop':%2").arg(_Start,_Stop);
}
void  GTAFailProfileRamp::getFailureProfileArguments(QHash<QString,QString>& iSerialzationMap)const
{

    iSerialzationMap.insert(XATTR_EQ_START,_Start);
    iSerialzationMap.insert(XATTR_EQ_STOP,_Stop);
}
void  GTAFailProfileRamp::setFailureProfileArguments(const QHash<QString,QString>& iSerialzationMap)
{

    if (iSerialzationMap.contains(XATTR_EQ_START))
        _Start=iSerialzationMap.value(XATTR_EQ_START);

     
    if (iSerialzationMap.contains(XATTR_EQ_STOP))
        _Stop=iSerialzationMap.value(XATTR_EQ_STOP);
    
}

QStringList GTAFailProfileRamp::getVariableList() const
{
    QStringList varList;

    if (GTACommandBase::isVariable(_Stop))
         varList<<_Stop;
    if (GTACommandBase::isVariable(_Start))
        varList<<_Start;
   
    return varList;
}
void GTAFailProfileRamp:: ReplaceTag(const QMap<QString,QString>& iTagValueMap) 
{
    QMapIterator<QString,QString> iterTag(iTagValueMap);
    while (iterTag.hasNext())
    {
        iterTag.next();
        QString tag = QString("%1%2%3").arg(QChar(TAG_IDENTIFIER),iterTag.key(),QChar(TAG_IDENTIFIER));
      
        if (GTACommandBase::isVariable(_Stop))
            _Stop.replace(tag,iterTag.value());
        if (GTACommandBase::isVariable(_Start))
            _Start.replace(tag,iterTag.value());
       
       

    }
}