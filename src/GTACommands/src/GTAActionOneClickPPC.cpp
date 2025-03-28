#include "GTAActionOneClickPPC.h"
#include "GTAUtil.h"

GTAActionOneClickPPC::GTAActionOneClickPPC(const QString &iAction, const QString &iComplement)
    : GTAActionBase(iAction,iComplement)
{

}
GTAActionOneClickPPC::~GTAActionOneClickPPC()
{

}
GTAActionOneClickPPC::GTAActionOneClickPPC(const GTAActionOneClickPPC& iCmd):GTAActionBase(iCmd)
{
   /* QString precisionVal, precisionType;
    iCmd.getPrecision(precisionVal,precisionType);
    QString timeOut, unit;
    iCmd.getTimeOut(timeOut,unit);*/

    this->setPPCPath(iCmd.getPPCPath());
    this->setAction(iCmd.getAction());
   /* this->setComplement(iCmd.getComplement());
    this->setCommandType(iCmd.getCommandType());
    this->setActionOnFail(iCmd.getActionOnFail());
    this->setPrecision(precisionVal,precisionType);
    this->setTimeOut(timeOut,unit);
    this->setComment(iCmd.getComment());*/
    this->setIsStart(iCmd.getIsStart());
    setDumpList(iCmd.getDumpList());
}
QString GTAActionOneClickPPC::getStatement() const
{
    QString oAction;
    QString ppcPath = getPPCPath();
    if(!_IsStart)
    {
        ppcPath = "Stop PPC";
    }
    oAction = QString("%1 %2 %3").arg(getAction(),getComplement(),ppcPath);
    return oAction;
}

QList<GTACommandBase*>& GTAActionOneClickPPC::getChildren(void) const
{
    return *_lstChildren;
}
bool GTAActionOneClickPPC::insertChildren(GTACommandBase*, const int&)
{
    return false;
}

void GTAActionOneClickPPC ::setParent(GTACommandBase* iParent)
{
    _parent=iParent;
}
GTACommandBase* GTAActionOneClickPPC::getParent(void) const
{
    return _parent;
}

GTACommandBase* GTAActionOneClickPPC::getClone() const
{
    return  (new GTAActionOneClickPPC(*this));
}
bool GTAActionOneClickPPC::canHaveChildren() const
{
    return false;//true;
}

QStringList GTAActionOneClickPPC::getVariableList() const
{
    return QStringList();
}

void GTAActionOneClickPPC:: ReplaceTag(const QMap<QString,QString>& iTagValueMap)
{
    iTagValueMap;//to suppress warning
}
void GTAActionOneClickPPC::stringReplace(const QRegExp&, const QString&)
{

}
bool GTAActionOneClickPPC::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
{
    bool rc = false;

    if (ibTextSearch && this->getComment().contains(iRegExp))
        rc = true;
    else if (ibTextSearch && this->getStatement().contains(iRegExp))
        rc = true;
    else
        rc=false;

    return rc;
}
bool GTAActionOneClickPPC::setDumpList(const QStringList&)
{

    return true;
}
QStringList GTAActionOneClickPPC::getDumpList()const
{
    return QStringList();
}

QString GTAActionOneClickPPC::getPPCPath() const
{
    return _PPCPath;
}

void GTAActionOneClickPPC::setPPCPath(const QString &iPath)
{
    _PPCPath = iPath;
}
QString GTAActionOneClickPPC::getSCXMLStateName()const
{
    return getStatement();

}