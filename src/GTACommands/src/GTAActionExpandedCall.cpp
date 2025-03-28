#include "GTAActionExpandedCall.h"
#include "GTAActionBase.h"
#include "GTAUtil.h"

GTAActionExpandedCall::GTAActionExpandedCall() : GTAActionCall()
{
    _IsBrokenLink=false;
    setAction(ACT_CALL_EXT);
}

GTAActionExpandedCall::GTAActionExpandedCall(const QString &iAction,
                                   const QString & iComplement,
                                                   const QString & iElement ): GTAActionCall(iAction,iComplement,iElement)
{
    _IsBrokenLink=false;
    setAction(ACT_CALL_EXT);
    setCommandType(GTACommandBase::ACTION);
    setElement(iElement);
	//_isParallelInSCXML=false;
}
GTAActionExpandedCall::GTAActionExpandedCall(GTAActionExpandedCall &iObj):GTAActionCall(iObj)
{
    //this->setComplement(iObj.getComplement());
    //this->setCommandType(iObj.getCommandType());
    //this->setComment(iObj.getComment());

    //this->setInstanceName(iObj.getInstanceName());
    //this->setActionOnFail(iObj.getActionOnFail());
    //QString precision, type;
    //iObj.getPrecision(precision,type);
    //QString timeOut,timeOutUnit;
    //iObj.getTimeOut(timeOut,timeOutUnit);
    //this->setPrecision(precision,type);
    //this->setTimeOut(timeOut,timeOutUnit);
    
    this->setElement(iObj.getElement());
    _referenceTimeoutStates = iObj.getReferenceTimeoutStates();
   
    QList<GTACommandBase*> lstChildrens = iObj.getChildren();
    for (int i=0;i<lstChildrens.size();++i)
    {
        GTACommandBase* pCurrentChild = lstChildrens.at(i);
        GTACommandBase* pClone = NULL;
        if (pCurrentChild!=NULL)
        {
            pClone=pCurrentChild->getClone();
            if (pClone)
                pClone->setParent(this);
        }
        this->insertChildren(pClone,i);
    }
    _IsBrokenLink=false;
    setRefrenceFileUUID(iObj.getRefrenceFileUUID());
//	_isParallelInSCXML = iObj.isParallelInSCXML();;
}
GTAActionExpandedCall::GTAActionExpandedCall(const GTAActionCall& iObj)
{
    setAction(ACT_CALL_EXT);
    setComplement(iObj.getComplement());
    setCommandType(iObj.getCommandType());
    setComment(iObj.getComment());
    setObjId(iObj.getObjId(),false);
    setIsParallelInSCXML(iObj.isParallelInSCXML());
    
    setInstanceName(iObj.getInstanceName());
    setActionOnFail(iObj.getActionOnFail());
    QString precision, type;
    iObj.getPrecision(precision,type);
    QString timeOut,timeOutUnit;
    iObj.getTimeOut(timeOut,timeOutUnit);
    setPrecision(precision,type);
    setTimeOut(timeOut,timeOutUnit);
    setElement(iObj.getElement());
    setIgnoreInSCXML(iObj.isIgnoredInSCXML());
   _referenceTimeoutStates= iObj.getReferenceTimeoutStates();
   setRefrenceFileUUID(iObj.getRefrenceFileUUID());
   setTagValue(iObj.getTagValue());
    

    QList<GTACommandBase*> lstChildrens = iObj.getChildren();
    for (int i=0;i<lstChildrens.size();++i)
    {
        GTACommandBase* pCurrentChild = lstChildrens.at(i);
        GTACommandBase* pClone = NULL;
        if (pCurrentChild!=NULL)
        {
            pClone=pCurrentChild->getClone();
            if (pClone)
                pClone->setParent(this);
        }
        this->insertChildren(pClone,i);
    }
    _IsBrokenLink=false;
//	_isParallelInSCXML = iObj.isParallelInSCXML();
}
GTAActionExpandedCall::~GTAActionExpandedCall()
{
    if (!_lstChildren->isEmpty())
    {
        for(int i=0;i<_lstChildren->size();++i)
        {
            GTACommandBase* pCommand = _lstChildren->at(i);
            if (NULL!=pCommand)
            {
                delete pCommand;
                pCommand=NULL;
            }
        }
        _lstChildren->clear();
    }
}

QList<GTACommandBase*>& GTAActionExpandedCall::getChildren(void) const
{
    return *_lstChildren;
}
bool GTAActionExpandedCall::insertChildren(GTACommandBase* pBase,const int& idPos)
{
    bool rc = false;
    int currSize=_lstChildren->size();
    if (idPos<=currSize && currSize>=0)
    {
        _lstChildren->insert(idPos,pBase);
        if (pBase!=NULL)
            pBase->setParent(this);
        rc = true;
    }
    return rc;
}
void  GTAActionExpandedCall::setParent(GTACommandBase* iParent)
{
    _parent=iParent;
}
GTACommandBase* GTAActionExpandedCall::getParent(void) const
{
    return _parent;
}
GTACommandBase * GTAActionExpandedCall::getClone() const
{
    return (new GTAActionExpandedCall(*this));
}
bool GTAActionExpandedCall:: hasReference()
{
    return true;
}
QColor GTAActionExpandedCall::getForegroundColor() const 
 {
     if(_IsBrokenLink)
         return QColor(255,0,0);
     else
         return QColor(0,0,0);
 }
bool GTAActionExpandedCall::canHaveChildren()const
{
    return true;
}
QStringList GTAActionExpandedCall::getVariableList() const
{
    return QStringList();

}

void GTAActionExpandedCall:: ReplaceTag(const QMap<QString,QString>& iTagValueMap) 
{
    iTagValueMap;//to suppress warning
    //do nothing
}

QList<GTACommandBase*> GTAActionExpandedCall::getExpandedList(void)
{
    QList<GTACommandBase*> oLstCmd;
    QList<GTACommandBase*> lstOfCommand = this->getChildren();
    for (int i=0;i<lstOfCommand.size();i++)
    {
        GTACommandBase* pCmd = lstOfCommand.at(i);
        GTAActionExpandedCall* pExpCall = dynamic_cast<GTAActionExpandedCall*> (pCmd);
        if (pExpCall)
        {
            QList<GTACommandBase*> lstOfCommandForCall = pExpCall->getExpandedList();
            oLstCmd.append(lstOfCommandForCall);
        }
        else
            oLstCmd.append(pCmd);
    }
    return oLstCmd;
}

QString GTAActionExpandedCall::getSCXMLStateName()const
{
    return QString("CALL_%1").arg(getElement());

}

QString GTAActionExpandedCall::getExecutionTime() const
{
    if((_lstChildren != NULL)&&(_lstChildren->count()>0))
    {
        GTACommandBase *pFirstCallCmd = NULL;
        for(int i=0;i<_lstChildren->count();i++)
        {
            pFirstCallCmd =  _lstChildren->at(i);
            if(pFirstCallCmd != NULL)
            {
                if(!pFirstCallCmd->isIgnoredInSCXML())
                    return pFirstCallCmd->getExecutionTime();
            }
        }
    }
    return "";
}
