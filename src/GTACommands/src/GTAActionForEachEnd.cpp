#include "GTAActionForEachEnd.h"
#include "GTAUtil.h"
#include "GTAActionDoWhile.h"
GTAActionForEachEnd::GTAActionForEachEnd()
{
    setAction(ACT_CONDITION);
    setComplement(COM_CONDITION_FOR_EACH_END);
    _parent = NULL;
}
GTAActionForEachEnd::~GTAActionForEachEnd()
{

}
QString GTAActionForEachEnd::getStatement() const
{
   
        return getComplement();
}
QString GTAActionForEachEnd::getLTRAStatement() const
{
    QString oAction = "";
    oAction = QString("#c#%1#c#").arg(getComplement().toUpper());
    return oAction;
}
QList<GTACommandBase*>& GTAActionForEachEnd::getChildren(void) const
{
    return *_lstChildren;
}
bool GTAActionForEachEnd::insertChildren(GTACommandBase*, const int&)
{
    return false;
}
void GTAActionForEachEnd::setParent(GTACommandBase* iParent)
{
    _parent=iParent;
}
GTACommandBase* GTAActionForEachEnd::getParent(void) const
{
    return _parent;
}
GTAActionForEachEnd::GTAActionForEachEnd(const GTAActionForEachEnd& rhs):GTAActionBase(rhs)
{
    ////from GTACommandBase
    //this->setCommandType(rhs.getCommandType());
    ////this->setParent(NULL);//This should be set based on paste

    ////from GTAActionBase
    //this->setAction(rhs.getAction());
    //this->setComplement(rhs.getComplement());
    //QString sTimeOut,sTimeOutUnit,sPrecision,sPrecisionUnit;

    //rhs.getPrecision(sPrecision,sPrecisionUnit);
    //setPrecision(sPrecision,sPrecisionUnit);

    //rhs.getTimeOut( sTimeOut,sTimeOutUnit);
    //setTimeOut( sTimeOut,sTimeOutUnit);
    //this->setActionOnFail(rhs.getActionOnFail());
    //this->setComment(rhs.getComment());
}
GTACommandBase* GTAActionForEachEnd::getClone() const
{
    GTACommandBase* pBase = new GTAActionForEachEnd(*this);
    return pBase;
}
bool GTAActionForEachEnd::canBeCopied()
{
    return false;
}
bool GTAActionForEachEnd::IsDeletable()
{
    return false;
}
 bool  GTAActionForEachEnd::IsUserEditable()
 {
    return false;
 } 
 bool  GTAActionForEachEnd::IsEndCommand() const
 {
     return true;
 }
 bool GTAActionForEachEnd::canHaveChildren() const
 {
     return false;//true;
 }
 QStringList GTAActionForEachEnd::getVariableList() const
 {
     return QStringList();

 }

 void GTAActionForEachEnd:: ReplaceTag(const QMap<QString,QString>& iTagValueMap) 
 {
     iTagValueMap;//to suppress warning
     //do nothing
 }
 bool GTAActionForEachEnd::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
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

 bool GTAActionForEachEnd::logCanExist()const
 {
     return false;
 }