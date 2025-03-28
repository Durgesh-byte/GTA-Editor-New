#include "GTAActionSet.h"
#include "GTAActionForEach.h"
#include "GTAActionSetList.h"
#include "GTAEquationBase.h"

#pragma warning(push, 0)
#include <QFile>
#include <QMetaEnum>
#include <QLatin1String>
#pragma warning(pop)

GTAActionSetList::GTAActionSetList():GTAActionBase()
{
    setVmacForceType(SET_VMACFORCE_SOL);
}
GTAActionSetList::GTAActionSetList(const GTAActionSetList& rhs): GTAActionBase(rhs)
{

	/*GTAActionSetList *pSetList = dynamic_cast<GTAActionSetList *>(pActCmd);
            QList <GTAActionSet*>  _setActionList;
            pSetList->getSetCommandlist(_setActionList);
            CmdInfo sCmdInfo;
            sCmdInfo.subCmdType = GTACheckCompatibility::PARAMETER_TYPE;
            sCmdInfo.cmdType= actType;
            sCmdInfo.pCmd= ipCmd;
            sCmdInfo.rowIdx = iRowIdx;
*/
    //from 

    //QString sAction = rhs.getAction();
    //this->setAction(sAction);
    ////from GTAActionBase
    //QString sTimeOut,sTimeOutUnit,sPrecision,sPrecisionUnit;

    //rhs.getPrecision(sPrecision,sPrecisionUnit);
    //this->setPrecision(sPrecision,sPrecisionUnit);

    //rhs.getTimeOut( sTimeOut,sTimeOutUnit);
    //this->setTimeOut( sTimeOut,sTimeOutUnit);

    setVmacForceType(rhs.getVmacForceType());
   /* this->setCommandType(rhs.getCommandType());
    this->setActionOnFail(rhs.getActionOnFail());
    
   
    setComment(rhs.getComment());*/

    //from GTAActionSetList

    int rhsListSize = rhs.getSetActionCount();
    for(int i=0;i<rhsListSize;i++)
    {
        GTAActionSet* pSetCmd=NULL;
       bool rc=rhs.getSetAction(i,pSetCmd);
        if (rc && NULL!=pSetCmd)
        {
            GTACommandBase* pCmd = pSetCmd->getClone();
            GTAActionSet* pSetAction = dynamic_cast<GTAActionSet*> (pCmd);
            if (pSetAction->getObjId().trimmed().isEmpty())
                pSetAction->setObjId();
            pSetAction->setParent(rhs.getParent());
            rc = addSetAction(pSetAction);
        }
    }


}
GTACommandBase* GTAActionSetList::getClone() const
{
    auto cmd = new GTAActionSetList(*this);
    cmd->setLastEditedRow(_lastEditedRow);
    return cmd;
}
GTAActionSetList::GTAActionSetList(const QString &iAction, const QString & iComplement) : GTAActionBase(iAction,iComplement)
{
   setAction(iAction);// = iAction;
   setVmacForceType(SET_VMACFORCE_SOL);
}
QString GTAActionSetList::getStatement() const
{
    QString oAction;
    foreach(GTAActionSet* pSetCmd, _setActionList)
    {
        if (NULL!=pSetCmd)
        {
            oAction.append(pSetCmd->getStatement() + "\n");
        }
    }
    return oAction;
}

QString GTAActionSetList::getLTRAStatement() const
{
    QString oAction;
    int setcount = 0;
    foreach(GTAActionSet* pSetCmd, _setActionList)
    {
        if (NULL!=pSetCmd)
        {
            //don't append a new line in the last set command
            if (setcount < _setActionList.count())
                oAction.append(pSetCmd->getLTRAStatement() + "\n");
            else
                oAction.append(pSetCmd->getLTRAStatement());
			setcount++;
        }
    }
    return oAction;
}
QString GTAActionSetList::getSCXMLStateName() const
{
    QString oAction;

    GTAActionSet* pSetCmd = _setActionList.at(0);
    if (NULL!=pSetCmd)
    {
        oAction.replace("}","");
        oAction.append(pSetCmd->getStatement().trimmed());
    }
    
    QHash<QString,QString> hshOfArithSym = GTAUtil::getNomenclatureOfArithSymbols();
    oAction.replace(QString(ARITH_EQ), hshOfArithSym.value(ARITH_EQ));
    
    return oAction;
}
QList<GTACommandBase*>& GTAActionSetList::getChildren(void) const
{
    return *_lstChildren;
}
bool GTAActionSetList::insertChildren(GTACommandBase*, const int&)
{
    return false;
}
void  GTAActionSetList::setParent(GTACommandBase* iParent)
{
    _parent=iParent;
}
GTACommandBase* GTAActionSetList::getParent(void) const
{
    return _parent;
}
bool GTAActionSetList::addSetAction(GTAActionSet* ipSetActionCmd)
{
    bool rc = false;
    if(NULL!=ipSetActionCmd)
    {
        _setActionList.append(ipSetActionCmd);
        rc = true;
    }
    return rc;
}
bool GTAActionSetList::deleteSetAction(const int& iIndex)
{
    bool rc = false;
    int listSize = _setActionList.size();

    if (iIndex>=listSize || iIndex<0)
        rc = false;
    else  
    {
        GTAActionSet* pCmd = _setActionList.takeAt(iIndex);
        delete pCmd;
        pCmd = NULL;
        rc = true;
    }
    return rc;

}



bool GTAActionSetList::getSetAction(const int& iIndex,GTAActionSet*& opSetAction)const
{
    bool rc = true;

	int listSize = _setActionList.size();

	if (iIndex >= listSize || iIndex<0)
		rc = false;
	else
		opSetAction = _setActionList.at(iIndex);
	
    return rc;
}
int GTAActionSetList::getSetActionCount()const
{
    return _setActionList.size();

}

QString GTAActionSetList::getVmacForceType() const
{
    return _VmacForceType;
}
void GTAActionSetList::setVmacForceType(const QString &iVmacForceType)
{
    _VmacForceType = iVmacForceType;
}
bool GTAActionSetList::canHaveChildren() const
{
    return false;//true;
}
QStringList GTAActionSetList::getVariableList() const
{
    QStringList lstOfVariables;
    for(int i=0;i<_setActionList.size();i++)
    {
        GTAActionSet* pSetCmd = _setActionList.at(i);
        if (pSetCmd && (! pSetCmd->getVariableList().isEmpty()))
        {
            lstOfVariables.append(pSetCmd->getVariableList());
        }
    }
    lstOfVariables.removeDuplicates();
    return lstOfVariables;
}
void GTAActionSetList::ReplaceTag(const QMap<QString,QString> & iTagValueMap)
{
    QMapIterator<QString,QString> iterTag(iTagValueMap);
    while (iterTag.hasNext())
    {
        iterTag.next();
        for(int i=0;i<_setActionList.size();i++)
        {
            GTAActionSet* pSetCmd = _setActionList.at(i);
            if (pSetCmd)
            {
                pSetCmd->ReplaceTag(iTagValueMap);
            }
        }
    }

}
void GTAActionSetList::stringReplace(const QRegExp& iStrToFind,const QString& iStringToReplace)
{
    for (int i=0;i<_setActionList.size();i++)
    {
        GTAActionSet* pSet = _setActionList.at(i);
        pSet->stringReplace(iStrToFind,iStringToReplace);
    }
    

}
bool GTAActionSetList::equipmentReplace(const QRegExp& iStrToFind, const QString& iStringToReplace)
{
    bool rc = false;
    for (int i = 0; i < _setActionList.size(); i++)
    {
        GTAActionSet* pSet = _setActionList.at(i);
        if (pSet->equipmentReplace(iStrToFind, iStringToReplace))
        {
            rc = true;
        }
    }
    return rc;
}
bool GTAActionSetList::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
{
    bool rc = false;
    for (int i=0;i<_setActionList.size();i++)
    {
        GTAActionSet* pSet = _setActionList.at(i);
        if (pSet->searchString(iRegExp,ibTextSearch))
        return true;
       
    }

    return rc;
}
bool GTAActionSetList::hasChannelInControl() const
{
    
    for(int i=0;i<_setActionList.size();i++)
    {
        GTAActionSet* pSetCmd = _setActionList.at(i);
        if (pSetCmd)
        {
            if(pSetCmd->hasChannelInControl())
                return true;

        }
        
    }
   return false;
}
