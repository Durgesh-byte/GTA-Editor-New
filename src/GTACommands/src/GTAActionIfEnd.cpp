#include "GTAActionIfEnd.h"
#include "GTAUtil.h"
GTAActionIfEnd::GTAActionIfEnd()
{
    setAction(ACT_CONDITION);
    setComplement(COM_CONDITION_ENDIF);
    _parent = NULL;
}
GTAActionIfEnd::~GTAActionIfEnd()
{

}
QString GTAActionIfEnd::getStatement() const
{
    return "END IF";
}
QList<GTACommandBase*>& GTAActionIfEnd::getChildren(void) const
{
    return *_lstChildren;
}
bool GTAActionIfEnd::insertChildren(GTACommandBase* pBase,const int& idPos)
{
    pBase;idPos;//for supressing warning.
    return false;
}
void GTAActionIfEnd::setParent(GTACommandBase* iParent)
{
    _parent=iParent;
}
GTACommandBase* GTAActionIfEnd::getParent(void) const
{
    return _parent;
}
GTAActionIfEnd::GTAActionIfEnd(const GTAActionIfEnd& rhs):GTAActionBase(rhs)
{
    //from GTACommandBase
   /* this->setCommandType(rhs.getCommandType());*/
    //this->setParent(NULL);//This should be set based on paste

    //from GTAActionBase
  /*  this->setAction(rhs.getAction());
    this->setComplement(rhs.getComplement());
	QString sTimeOut,sTimeOutUnit,sPrecision,sPrecisionUnit;

	rhs.getPrecision(sPrecision,sPrecisionUnit);
	setPrecision(sPrecision,sPrecisionUnit);

	rhs.getTimeOut( sTimeOut,sTimeOutUnit);
	setTimeOut( sTimeOut,sTimeOutUnit);
    this->setActionOnFail(rhs.getActionOnFail());
    this->setComment(rhs.getComment());*/
}
GTACommandBase* GTAActionIfEnd::getClone() const
{
    GTACommandBase* pBase = new GTAActionIfEnd(*this);
    return pBase;
}
bool GTAActionIfEnd::canBeCopied()
{
    return false;
}
bool GTAActionIfEnd::IsDeletable()
{
    return false;
}
bool GTAActionIfEnd:: IsUserEditable()
{
    return false;

}
bool GTAActionIfEnd:: IsEndCommand()const
{
    return true;
}
bool GTAActionIfEnd::canHaveChildren() const
{
    return false;//true;
}
QStringList GTAActionIfEnd::getVariableList() const
{
    return QStringList();

}

void GTAActionIfEnd:: ReplaceTag(const QMap<QString,QString>& iTagValueMap) 
{
    iTagValueMap;//to suppress warning
    //do nothing
}
bool GTAActionIfEnd::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
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

bool GTAActionIfEnd::logCanExist()const
{
    return false;
}