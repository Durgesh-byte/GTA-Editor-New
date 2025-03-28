#include "GTAActionParentTitle.h"
#include "GTAUtil.h"
#include "GTAActionExpandedCall.h"
#include "GTAActionFTAKinematicMultiOutput.h"
#include "GTACommandBase.h"
#include "GTAActionCallProcedures.h"

GTAActionParentTitle::GTAActionParentTitle(GTAActionTitle* pTitle,const QString &) : GTAActionTitle(ACT_TITLE)
{
    if(pTitle)
    {
        _Title = pTitle->getTitle();
        _Comment = pTitle->getComment();
        setInstanceName(pTitle->getInstanceName());
    }
    _ImageName = QString();
}
GTAActionParentTitle::GTAActionParentTitle(const GTAActionParentTitle& iObjTitle): GTAActionTitle(ACT_TITLE)
{
    
    this->setAction(iObjTitle.getAction());
    this->setComplement(iObjTitle.getComplement());
    this->setActionOnFail(iObjTitle.getActionOnFail());

    QString sTimeOut,sTimeOutUnit,sPrecision,sPrecisionUnit;

    iObjTitle.getPrecision(sPrecision,sPrecisionUnit);
    this->setPrecision(sPrecision,sPrecisionUnit);

    iObjTitle.getTimeOut( sTimeOut,sTimeOutUnit);
    this->setTimeOut( sTimeOut,sTimeOutUnit);
    this->setCommandType(this->getCommandType());

    this->setTitle(iObjTitle.getTitle());
    this->setComment(iObjTitle.getComment());
    this->setImageName(iObjTitle.getImageName());
    this->setInstanceName(iObjTitle.getInstanceName());


    QList<GTACommandBase*> lstChildrens = iObjTitle.getChildren();
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
}

GTAActionParentTitle::~GTAActionParentTitle()
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

QList<GTACommandBase*>& GTAActionParentTitle::getChildren(void) const
{
    return *_lstChildren;
}

bool GTAActionParentTitle::insertChildren(GTACommandBase* pBase,const int& idPos)
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

int GTAActionParentTitle::getAllChildrenCount(void)
{
    

    return   GTACommandBase::getAllChildrenCount();;
}


GTACommandBase* GTAActionParentTitle::getClone()const
{
    return (new GTAActionParentTitle(*this));
}

bool GTAActionParentTitle::canHaveChildren() const
{
    return true;
}
QString GTAActionParentTitle::getSCXMLStateName()const
{
    return _Title;
}
QString GTAActionParentTitle::getGlobalResultStatus()
{
    //    QString data = "NA";
    //    QString previousStatus= "NA";
    //    if(_lstChildren != NULL && (!_lstChildren->isEmpty()))
    //    {
    //        for(int i =0;i<_lstChildren->count();i++)
    //        {
    //            GTACommandBase * pCmd = _lstChildren->at(i);
    //            if(pCmd != NULL)
    //            {
    //                if(pCmd->hasReference())
    //                {
    //                    GTAActionExpandedCall *pCall = static_cast<GTAActionExpandedCall*>(pCmd);
    //                    if(pCall != NULL)
    //                        data = pCall->getGlobalResultStatus();
    //                }
    //                else if(pCmd->createsCommadsFromTemplate())
    //                {
    //                    GTAActionFTAKinematicMultiOutput *pKinematicCmd = static_cast<GTAActionFTAKinematicMultiOutput *>(pCmd);
    //                    if(pKinematicCmd!= NULL)
    //                        data = pKinematicCmd->getGlobalResultStatus();
    //                }
    //                else if(pCmd->isTitle())
    //                {
    //                    GTAActionParentTitle *pTitle = static_cast<GTAActionParentTitle*>(pCmd);
    //                    data = pTitle->getGlobalResultStatus();
    //                }
    //                else
    //                {
    //                    QList<GTAScxmlLogMessage> msgList = pCmd->getLogMessageList();
    //                    foreach(GTAScxmlLogMessage objLog, msgList)
    //                    {
    //                        if(objLog.LOD == GTAScxmlLogMessage::Main)
    //                        {
    //                            data =  objLog.ResultTypeToString(objLog.Result);
    //                        }
    //                    }
    //                }
    //            }
    //            if(data == "KO")
    //            {
    //                return data;
    //            }
    //            if(previousStatus == "NA" && data == "OK")
    //                previousStatus = "OK";

    //            if(previousStatus == "OK" && data == "NA")
    //                data = "OK";

    //        }

    //    }

    QString result = "NA";
    QString data = "";
    if((_lstChildren != NULL) && (!_lstChildren->isEmpty()))
    {
        for(int i=0;i<_lstChildren->count();i++)
        {
            GTACommandBase *pTitleChildCmd =  _lstChildren->at(i);
            GTACommandBase *pChildCmd = dynamic_cast<GTACommandBase *>(pTitleChildCmd);
            GTAActionCallProcedures *pCallproc = dynamic_cast<GTAActionCallProcedures *>(pTitleChildCmd);
            if(pChildCmd != NULL)
            {
                if(pChildCmd->isTitle())
                {
                    GTAActionParentTitle *pTitle = dynamic_cast<GTAActionParentTitle*>(pChildCmd);
                    if(pTitle != NULL)
                    {
                        data = pTitle->getGlobalResultStatus();
                    }
                }
                else if(pChildCmd->hasReference())
                {
                    GTAActionExpandedCall *pCallCmd = dynamic_cast<GTAActionExpandedCall*>(pChildCmd);
                    if(pCallCmd != NULL)
                    {
                        data = pCallCmd->getGlobalResultStatus();
                    }
                }
                else if(pChildCmd->canHaveChildren())
                {
                    data = pChildCmd->getGlobalResultStatus();
                }
                else if (pCallproc != NULL)
                {
                    data = pChildCmd->getGlobalResultStatus();
                }
                else
                {
                    data = pChildCmd->getExecutionResult();
                }
                if(data == "KO")
                    break;
                else
                {
                    if((data == "OK")|| (data=="KO"))
                        result = data;
                }
            }
        }
    }
    if((data != "NA") && (data != ""))
        result = data;
    return result;
}


/**
 * This function provides gets the execution epoch time for the first non ignored child 
 * @return: epoch time in double
*/
double GTAActionParentTitle::getExecutionEpochTime()const
{
    double epochTime = 0.0;
    if((_lstChildren != NULL)&&(_lstChildren->count()>0))
    {
        GTACommandBase *pFirstTitleCmd = NULL;
        for(int i=0;i<_lstChildren->count();i++)
        {
            pFirstTitleCmd =  _lstChildren->at(i);
            if(pFirstTitleCmd != NULL)
            {
                if(!pFirstTitleCmd->isIgnoredInSCXML())
                {
                    epochTime = pFirstTitleCmd->getExecutionEpochTime();
                    return epochTime;
                }
            }
        }
    }
    return epochTime;
}

QString GTAActionParentTitle::getExecutionTime() const
{
    if((_lstChildren != NULL)&&(_lstChildren->count()>0))
    {
        GTACommandBase *pFirstTitleCmd = NULL;
        for(int i=0;i<_lstChildren->count();i++)
        {
            pFirstTitleCmd =  _lstChildren->at(i);
            if(pFirstTitleCmd != NULL)
            {
                if(!pFirstTitleCmd->isIgnoredInSCXML())
                    return pFirstTitleCmd->getExecutionTime();
            }
        }
    }
    return "";
}
void GTAActionParentTitle::getUningnoredChildren(QList<GTACommandBase *> &unIgnoredChildren) const
{
    if(!_lstChildren->isEmpty())
    {
        for(int i =0;i<_lstChildren->count();i++)
        {
            GTACommandBase * pCmd = _lstChildren->at(i);
            if(!pCmd->isIgnoredInSCXML())
                unIgnoredChildren.append(pCmd);
        }
    }
}

void GTAActionParentTitle::setCollapsed(bool iCollapsed)
{
    _IsCollapsed = iCollapsed;

}

bool GTAActionParentTitle::getCollapsed()
{
    return _IsCollapsed;
}


//bool GTAActionParentTitle::canHaveInstanceName()
//{
//    return false;
//}
