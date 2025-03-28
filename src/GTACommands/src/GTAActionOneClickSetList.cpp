#include "GTAActionOneClickSet.h"
#include "GTAActionOneClickSetList.h"
#include "GTAEquationBase.h"

#pragma warning(push, 0)
#include <QMetaEnum>
#include <QLatin1String>
#pragma warning(pop)

GTAActionOneClickSetList::GTAActionOneClickSetList():GTAActionBase()
{
    
}
GTAActionOneClickSetList::GTAActionOneClickSetList(const GTAActionOneClickSetList& rhs): GTAActionBase(rhs.getAction(),rhs.getComplement())
{
    //from 

    QString sAction = rhs.getAction();
    this->setAction(sAction);
    //from GTAActionBase
    QString sTimeOut,sTimeOutUnit,sPrecision,sPrecisionUnit;

    rhs.getPrecision(sPrecision,sPrecisionUnit);
    this->setPrecision(sPrecision,sPrecisionUnit);

    rhs.getTimeOut( sTimeOut,sTimeOutUnit);
    this->setTimeOut( sTimeOut,sTimeOutUnit);

    this->setVmacForceType(rhs.getVmacForceType());
    this->setCommandType(rhs.getCommandType());
    this->setActionOnFail(rhs.getActionOnFail());
    
   
    setComment(rhs.getComment());

    //from GTAActionOneClickSetList

    int rhsListSize = rhs.getSetActionCount();
    for(int i=0;i<rhsListSize;i++)
    {
        GTAActionOneClickSet* pSetCmd=NULL;
       bool rc=rhs.getSetAction(i,pSetCmd);
        if (rc && NULL!=pSetCmd)
        {
            GTACommandBase* pCmd = pSetCmd->getClone();
            GTAActionOneClickSet* pSetAction = dynamic_cast<GTAActionOneClickSet*> (pCmd);
            rc = addSetAction(pSetAction);
        }
    }


}
GTACommandBase* GTAActionOneClickSetList::getClone() const
{
    return new GTAActionOneClickSetList(*this);
}
GTAActionOneClickSetList::GTAActionOneClickSetList(const QString &iAction, const QString & iComplement) : GTAActionBase(iAction,iComplement)
{
   setAction(iAction);// = iAction;
}
QString GTAActionOneClickSetList::getStatement() const
{
    QString oAction;
    foreach(GTAActionOneClickSet* pSetCmd, _setActionList)
    {
        if (NULL!=pSetCmd)
        {
            oAction.append(pSetCmd->getStatement() + "\n");
        }
    }
    return oAction;
}
QString GTAActionOneClickSetList::getSCXMLStateName() const
{
    QString oAction;

    GTAActionOneClickSet* pSetCmd = _setActionList.at(0);
    if (NULL!=pSetCmd)
    {
        oAction.replace("}","");
        oAction.append(pSetCmd->getStatement().trimmed());
    }
    
    QHash<QString,QString> hshOfArithSym = GTAUtil::getNomenclatureOfArithSymbols();
    oAction.replace(QString(ARITH_EQ), hshOfArithSym.value(ARITH_EQ));
    
    return oAction;
}
QList<GTACommandBase*>& GTAActionOneClickSetList::getChildren(void) const
{
    return *_lstChildren;
}
bool GTAActionOneClickSetList::insertChildren(GTACommandBase*, const int&)
{
    return false;
}
void  GTAActionOneClickSetList::setParent(GTACommandBase* iParent)
{
    _parent=iParent;
}
GTACommandBase* GTAActionOneClickSetList::getParent(void) const
{
    return _parent;
}
bool GTAActionOneClickSetList::addSetAction(GTAActionOneClickSet* ipSetActionCmd)
{
    bool rc = false;
    if(NULL!=ipSetActionCmd)
    {
        _setActionList.append(ipSetActionCmd);
        rc = true;
    }
    return rc;
}
bool GTAActionOneClickSetList::deleteSetAction(const int& iIndex)
{
    bool rc = false;
    int listSize = _setActionList.size();

    if (iIndex>=listSize || iIndex<0)
        rc = false;
    else  
    {
        GTAActionOneClickSet* pCmd = _setActionList.takeAt(iIndex);
        delete pCmd;
        pCmd = NULL;
        rc = true;
    }
    return rc;

}
bool GTAActionOneClickSetList::getSetAction(const int& iIndex,GTAActionOneClickSet*& opSetAction)const
{
    bool rc = true;
    int listSize = _setActionList.size();

    if (iIndex>=listSize || iIndex<0)
        rc = false;
    else  
        opSetAction = _setActionList.at(iIndex);
    return rc;
}
int GTAActionOneClickSetList::getSetActionCount()const
{
    return _setActionList.size();

}

QString GTAActionOneClickSetList::getVmacForceType() const
{
    return _VmacForceType;
}
void GTAActionOneClickSetList::setVmacForceType(const QString &iVmacForceType)
{
    _VmacForceType = iVmacForceType;
}
bool GTAActionOneClickSetList::canHaveChildren() const
{
    return false;//true;
}
QStringList GTAActionOneClickSetList::getVariableList() const
{
    QStringList lstOfVariables;
    for(int i=0;i<_setActionList.size();i++)
    {
        GTAActionOneClickSet* pSetCmd = _setActionList.at(i);
        if (pSetCmd && (! pSetCmd->getVariableList().isEmpty()))
        {
            lstOfVariables.append(pSetCmd->getVariableList());
        }
    }
    lstOfVariables.removeDuplicates();
    return lstOfVariables;
}
void GTAActionOneClickSetList::ReplaceTag(const QMap<QString,QString> & iTagValueMap)
{
    QMapIterator<QString,QString> iterTag(iTagValueMap);
    while (iterTag.hasNext())
    {
        iterTag.next();
        for(int i=0;i<_setActionList.size();i++)
        {
            GTAActionOneClickSet* pSetCmd = _setActionList.at(i);
            if (pSetCmd)
            {
                pSetCmd->ReplaceTag(iTagValueMap);
            }
        }
    }

}
void GTAActionOneClickSetList::stringReplace(const QRegExp& iStrToFind,const QString& iStringToReplace)
{
    for (int i=0;i<_setActionList.size();i++)
    {
        GTAActionOneClickSet* pSet = _setActionList.at(i);
        pSet->stringReplace(iStrToFind,iStringToReplace);
    }
    

}
bool GTAActionOneClickSetList::equipmentReplace(const QRegExp& iStrToFind, const QString& iStringToReplace)
{
    bool rc = false;
    for (int i = 0; i < _setActionList.size(); i++)
    {
        GTAActionOneClickSet* pSet = _setActionList.at(i);
        if (pSet->equipmentReplace(iStrToFind, iStringToReplace))
        {
            rc = true;
        }
    }
    return rc;
}
bool GTAActionOneClickSetList::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
{
    bool rc = false;
    for (int i=0;i<_setActionList.size();i++)
    {
        GTAActionOneClickSet* pSet = _setActionList.at(i);
        if (pSet->searchString(iRegExp,ibTextSearch))
        return true;
       
    }

    return rc;
}