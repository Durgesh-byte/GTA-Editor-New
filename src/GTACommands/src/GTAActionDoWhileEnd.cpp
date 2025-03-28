#include "GTAActionDoWhileEnd.h"
#include "GTAUtil.h"
#include "GTAActionDoWhile.h"
GTAActionDoWhileEnd::GTAActionDoWhileEnd()
{
    setAction(ACT_CONDITION);
    setComplement(COM_CONDITION_DO_WHILE_END);
    _parent = NULL;
}
GTAActionDoWhileEnd::~GTAActionDoWhileEnd()
{

}
QString GTAActionDoWhileEnd::getStatement() const
{
    GTAActionDoWhile* pParentWhile = dynamic_cast<GTAActionDoWhile*> (this->getParent());
    if (pParentWhile)
    {
        return pParentWhile->getStatementForEnd();
    }
    else
        return getComplement();
}
QString GTAActionDoWhileEnd::getLTRAStatement() const
{
    GTAActionDoWhile* pParentWhile = dynamic_cast<GTAActionDoWhile*> (this->getParent());
    if (pParentWhile)
    {
        QString param = pParentWhile->getParameter();
        QString value = pParentWhile->getValue();
        QString cond = pParentWhile->getCondition();
        return QString("#c#%1#c#(#b#%2#b# %3 #b#%4#b#)").arg("WHILE" ,param,cond,value);
    }
    else
        return getComplement();
}
QList<GTACommandBase*>& GTAActionDoWhileEnd::getChildren(void) const
{
    return *_lstChildren;
}
bool GTAActionDoWhileEnd::insertChildren(GTACommandBase* pBase,const int& idPos)
{
     pBase;idPos;//for supressing warning.
    return false;
}
void GTAActionDoWhileEnd::setParent(GTACommandBase* iParent)
{
    _parent=iParent;
}
GTACommandBase* GTAActionDoWhileEnd::getParent(void) const
{
    return _parent;
}
GTAActionDoWhileEnd::GTAActionDoWhileEnd(const GTAActionDoWhileEnd& rhs):GTAActionBase(rhs)
{
    
}
GTACommandBase* GTAActionDoWhileEnd::getClone() const
{
    GTACommandBase* pBase = new GTAActionDoWhileEnd(*this);
    return pBase;
}
bool GTAActionDoWhileEnd::canBeCopied()
{
    return false;
}
bool GTAActionDoWhileEnd::IsDeletable()
{
    return false;
}
 bool  GTAActionDoWhileEnd::IsUserEditable()
 {
    return false;
 }
 bool  GTAActionDoWhileEnd::IsEndCommand() const
 {
     return true;
 }
 bool GTAActionDoWhileEnd::canHaveChildren() const
 {
     return false;//true;
 }
 QStringList GTAActionDoWhileEnd::getVariableList() const
 {
     return QStringList();

 }

 void GTAActionDoWhileEnd:: ReplaceTag(const QMap<QString,QString>& iTagValueMap) 
 {
     iTagValueMap;//to suppress warning
     //do nothing
 }
 bool GTAActionDoWhileEnd::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
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

 bool GTAActionDoWhileEnd::logCanExist()const
 {
     return false;
 }
