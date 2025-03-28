#include "GTAActionWhileEnd.h"
#include "GTAUtil.h"
#include "GTAActionDoWhile.h"
GTAActionWhileEnd::GTAActionWhileEnd()
{
    setAction(ACT_CONDITION);
    setComplement(COM_CONDITION_ENDWHILE);
    _parent = NULL;
}
GTAActionWhileEnd::~GTAActionWhileEnd()
{

}
QString GTAActionWhileEnd::getStatement() const
{
   
        return getComplement();
}
QList<GTACommandBase*>& GTAActionWhileEnd::getChildren(void) const
{
    return *_lstChildren;
}
bool GTAActionWhileEnd::insertChildren(GTACommandBase*, const int&)
{
    return false;
}
void GTAActionWhileEnd::setParent(GTACommandBase* iParent)
{
    _parent=iParent;
}
GTACommandBase* GTAActionWhileEnd::getParent(void) const
{
    return _parent;
}
GTAActionWhileEnd::GTAActionWhileEnd(const GTAActionWhileEnd& rhs):GTAActionBase(rhs)
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
GTACommandBase* GTAActionWhileEnd::getClone() const
{
    GTACommandBase* pBase = new GTAActionWhileEnd(*this);
    return pBase;
}
bool GTAActionWhileEnd::canBeCopied()
{
    return false;
}
bool GTAActionWhileEnd::IsDeletable()
{
    return false;
}
 bool  GTAActionWhileEnd::IsUserEditable()
 {
    return false;
 } 
 bool  GTAActionWhileEnd::IsEndCommand() const
 {
     return true;
 }
 bool GTAActionWhileEnd::canHaveChildren() const
 {
     return false;//true;
 }
 QStringList GTAActionWhileEnd::getVariableList() const
 {
     return QStringList();

 }

 void GTAActionWhileEnd:: ReplaceTag(const QMap<QString,QString>& iTagValueMap) 
 {
     iTagValueMap;//to suppress warning
     //do nothing
 }
 bool GTAActionWhileEnd::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
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

 bool GTAActionWhileEnd::logCanExist()const
 {
     return false;
 }