#include "GTAActionParentTitleEnd.h"
#include "GTAUtil.h"
#include "GTAActionDoWhile.h"
GTAActionParentTitleEnd::GTAActionParentTitleEnd()
{
    setAction(ACT_TITLE_END);
    _parent = NULL;
}
GTAActionParentTitleEnd::~GTAActionParentTitleEnd()
{

}
QString GTAActionParentTitleEnd::getStatement() const
{
   
        return getComplement();
}
QList<GTACommandBase*>& GTAActionParentTitleEnd::getChildren(void) const
{
    return *_lstChildren;
}
bool GTAActionParentTitleEnd::insertChildren(GTACommandBase*, const int&)
{
    return false;
}
void GTAActionParentTitleEnd::setParent(GTACommandBase* iParent)
{
    _parent=iParent;
}
GTACommandBase* GTAActionParentTitleEnd::getParent(void) const
{
    return _parent;
}
GTAActionParentTitleEnd::GTAActionParentTitleEnd(const GTAActionParentTitleEnd& rhs):GTAActionBase(rhs)
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
GTACommandBase* GTAActionParentTitleEnd::getClone() const
{
    GTACommandBase* pBase = new GTAActionParentTitleEnd(*this);
    return pBase;
}
bool GTAActionParentTitleEnd::canBeCopied()
{
    return false;
}
bool GTAActionParentTitleEnd::IsDeletable()
{
    return false;
}
 bool  GTAActionParentTitleEnd::IsUserEditable()
 {
    return false;
 } 
 bool  GTAActionParentTitleEnd::IsEndCommand() const
 {
     return true;
 }
 bool GTAActionParentTitleEnd::canHaveChildren() const
 {
     return false;//true;
 }
 QStringList GTAActionParentTitleEnd::getVariableList() const
 {
     return QStringList();

 }

 void GTAActionParentTitleEnd:: ReplaceTag(const QMap<QString,QString>& iTagValueMap) 
 {
     iTagValueMap;//to suppress warning
     //do nothing
 }
 bool GTAActionParentTitleEnd::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
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

 bool GTAActionParentTitleEnd::logCanExist()const
 {
     return false;
 }