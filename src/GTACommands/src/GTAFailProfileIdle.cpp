#include "GTAFailProfileIdle.h"
#include "GTAUtil.h"
GTAFailProfileIdle::GTAFailProfileIdle() : GTAFailureProfileBase(GTAFailureProfileBase::IDLE)
{
}
GTAFailProfileIdle::GTAFailProfileIdle(const GTAFailProfileIdle&) : GTAFailureProfileBase(GTAFailureProfileBase::IDLE)
{
	
}
GTAFailProfileIdle::~GTAFailProfileIdle()
{

}
 GTAFailureProfileBase* GTAFailProfileIdle::getClone()const
{
	GTAFailureProfileBase* pClone  = new GTAFailProfileIdle(*this);
	return pClone;
}
 void  GTAFailProfileIdle::getFunctionStatement(QString&, QString& osFunctionStatement) const
 {
     
     osFunctionStatement=QString("'type':'IDLE','mode':2");
 }
 void  GTAFailProfileIdle::getFailureProfileArguments(QHash<QString,QString>&) const
 {
   
 }
 void  GTAFailProfileIdle::setFailureProfileArguments(const QHash<QString,QString>&)
 {
    
 }
 QStringList GTAFailProfileIdle::getVariableList() const 
 {
     QStringList varList;
    
     return varList;
 }
 void GTAFailProfileIdle::ReplaceTag(const QMap<QString,QString>&) 
 {
 }
 QString GTAFailProfileIdle::getStatement() const
 {
     return  QString("PROFILE(IDLE)");
 }