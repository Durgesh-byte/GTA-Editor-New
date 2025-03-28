#include "GTAFailProfileStopTrigger.h"
#include "GTAUtil.h"
GTAFailProfileStopTrigger::GTAFailProfileStopTrigger():GTAFailureProfileBase(GTAFailureProfileBase::STOPTRIGGER)
{
}
GTAFailProfileStopTrigger::GTAFailProfileStopTrigger(const GTAFailProfileStopTrigger& rhs):GTAFailureProfileBase(GTAFailureProfileBase::STOPTRIGGER)
{
	_timeOut=rhs.getTimeout();
}
GTAFailProfileStopTrigger::~GTAFailProfileStopTrigger()
{

}


 QString GTAFailProfileStopTrigger::getStatement() const
 {
     QString sVarVal1= QString("%1=%2").arg(XATTR_EQ_TIMEOUT,_timeOut);
    
     QString oEquation = QString("%1(%2)").arg(EQ_STOP_TRIGGER,sVarVal1);


     return oEquation;
 }
 GTAFailureProfileBase* GTAFailProfileStopTrigger:: getClone()const
 {
     return new GTAFailProfileStopTrigger(*this);
 }
 void  GTAFailProfileStopTrigger::getFunctionStatement(QString& osFuncType,QString& osFunctionStatement)const
 {

     osFuncType=QString(EQ_STOP_TRIGGER).toUpper();
     QString sVarVal1= QString("'%1':'%2'").arg(XATTR_EQ_START,_timeOut);
    


     osFunctionStatement = QString("%1{%2}").arg("'FunctionValue':",sVarVal1);
     osFunctionStatement=QString("'type':'STOPTRIGGER','mode':1,'timeout':%1").arg(_timeOut);
     //iSerialzationMap.insert()
 }
 void  GTAFailProfileStopTrigger::getFailureProfileArguments(QHash<QString,QString>& iSerialzationMap)const
 {
     iSerialzationMap.insert(XATTR_EQ_TIMEOUT,_timeOut);
   


 }
 void  GTAFailProfileStopTrigger::setFailureProfileArguments(const QHash<QString,QString>& iSerialzationMap)
 {

     if (iSerialzationMap.contains(XATTR_EQ_TIMEOUT))
         _timeOut=iSerialzationMap.value(XATTR_EQ_TIMEOUT);

 }

 QStringList GTAFailProfileStopTrigger::getVariableList() const
 {
     QStringList varList;

     if (GTACommandBase::isVariable(_timeOut))
         varList<<_timeOut;
    

     return varList;
 }
 void GTAFailProfileStopTrigger:: ReplaceTag(const QMap<QString,QString>& iTagValueMap) 
 {
     QMapIterator<QString,QString> iterTag(iTagValueMap);
     while (iterTag.hasNext())
     {
         iterTag.next();
         QString tag = QString("%1%2%3").arg(QChar(TAG_IDENTIFIER),iterTag.key(),QChar(TAG_IDENTIFIER));

         if (GTACommandBase::isVariable(_timeOut))
             _timeOut.replace(tag,iterTag.value());
         


     }
 }