#include "GTAActionElse.h"
#include "GTAUtil.h"
GTAActionElse::GTAActionElse(const QString & iActionName, const QString & iComplmentName) : GTAActionBase(iActionName,iComplmentName) 
{

}
GTAActionElse::GTAActionElse (const GTAActionElse& rhs):GTAActionBase(rhs)
{


	//This should be set based on paste, paste command should reparent it
    this->setParent(rhs.getParent());

    QList<GTACommandBase*> lstChildrens = rhs.getChildren();
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

QString GTAActionElse::getGlobalResultStatus()
{
    QString data = "NA";

    QList<GTACommandBase *> childrens = getChildren();

    for(int i=0;i<childrens.count();i++)
    {
        GTACommandBase *pCmd = childrens.at(i);
        QString data1 = pCmd->getGlobalResultStatus();
        if(!data1.trimmed().isEmpty())
        {
            if(data1 == "KO")
            {
                data = data1;
                break;
            }
            else if(data1 != "NA")
            {
                data = data1;
            }
        }
    }
    return data;
}


GTAActionElse::~GTAActionElse()
{


    if (_lstChildren!=NULL && (!_lstChildren->isEmpty()))
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

QString GTAActionElse::getStatement() const
{
    return QString(getComplement()).toUpper();
}

QList<GTACommandBase*>& GTAActionElse::getChildren(void) const
{
    return *_lstChildren;
}
bool GTAActionElse::insertChildren(GTACommandBase* pBase,const int& idPos)
{
    bool rc = false;
    int currSize=_lstChildren->size();
    if (idPos<=currSize && currSize>=0)
    {
        _lstChildren->insert(idPos,pBase);
        if (pBase!=NULL)
        {
            pBase->setParent(this);
            if(this->isIgnoredInSCXML() == true)
            {
                pBase->setIgnoreInSCXML(this->isIgnoredInSCXML());
            }
            if(this->getReadOnlyState() == true)
            {
                pBase->setReadOnlyState(this->getReadOnlyState());
            }
        }
        rc = true;
    }
    return rc;
}

void  GTAActionElse::setParent(GTACommandBase* iParent)
{
    _parent=iParent;
}
GTACommandBase* GTAActionElse::getParent(void) const
{
    return _parent;
}
GTACommandBase* GTAActionElse::getClone() const
{
    return new GTAActionElse(*this);

}

bool GTAActionElse::canBeCopied()
{
    return false;
}
bool GTAActionElse::IsDeletable()
{
    return true;
}


bool GTAActionElse:: hasChildren()
{
     return !(_lstChildren->isEmpty());
}
bool GTAActionElse::canHaveChildren() const
{
    return true;
}
QStringList GTAActionElse::getVariableList() const
{
    QStringList lstOfVars;
//    for(int i=0;i<_lstChildren->size();++i)
//    {
//        GTACommandBase* pCommand = _lstChildren->at(i);
//        if (NULL!=pCommand)
//        {
//            QStringList lsofVarsForChild = pCommand->getVariableList();
//            if (!lsofVarsForChild.isEmpty())
//                lstOfVars.append(lsofVarsForChild);
//        }
//    }
    return lstOfVars;

}

void GTAActionElse:: ReplaceTag(const QMap<QString,QString>& iTagValueMap) 
{
    iTagValueMap;//to suppress warning
//    for(int i=0;i<_lstChildren->size();++i)
//    {
//        GTACommandBase* pCommand = _lstChildren->at(i);
//        if (NULL!=pCommand)
//            pCommand->ReplaceTag(iTagValueMap);
//    }
}
bool GTAActionElse::expandReferences()
{
    int currChildrenSize = _lstChildren->size();
    for (int i=currChildrenSize-1;i>=0;i--)
    {
        GTACommandBase* pCurrentChild = _lstChildren->at(i);
        if (pCurrentChild->hasReference())
        {
            _lstChildren->removeAt(i);
            QList<GTACommandBase*>& referenceChildrens = pCurrentChild->getChildren();
            for (int j=0;j<referenceChildrens.size();j++)
            {
                _lstChildren->insert(i+j,referenceChildrens.at(j)->getClone());
            }

        }
    }
    currChildrenSize = _lstChildren->size();
    for (int i=currChildrenSize-1;i>=0;i--)
    { 
        GTACommandBase* pCurrentChild = _lstChildren->at(i);
        pCurrentChild->expandReferences();
    }
    return true;
}
bool GTAActionElse::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
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
