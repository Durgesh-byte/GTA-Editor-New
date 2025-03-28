#include "GTAActionOneClickParentTitle.h"
#include "GTAUtil.h"
GTAActionOneClickParentTitle::GTAActionOneClickParentTitle(GTAActionTitle* pTitle, const QString &, const QString& iComplement) : GTAActionTitle(ACT_ONECLICK)
{
    if(pTitle)
        _Title = pTitle->getTitle();
    _Comment = QString();
    _ImageName = QString();
    setComplement(iComplement);
}
GTAActionOneClickParentTitle::GTAActionOneClickParentTitle(const GTAActionOneClickParentTitle& iObjTitle): GTAActionTitle(ACT_ONECLICK)
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
    
}

GTAActionOneClickParentTitle::~GTAActionOneClickParentTitle()
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

QList<GTACommandBase*>& GTAActionOneClickParentTitle::getChildren(void) const
{
    return *_lstChildren;
}

bool GTAActionOneClickParentTitle::insertChildren(GTACommandBase* pBase,const int& idPos)
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

int GTAActionOneClickParentTitle::getAllChildrenCount(void)
{
    return   GTACommandBase::getAllChildrenCount();
}


GTACommandBase* GTAActionOneClickParentTitle::getClone()const
{
    return (new GTAActionOneClickParentTitle(*this));
}

bool GTAActionOneClickParentTitle::canHaveChildren() const
{
    return true;
}
QString GTAActionOneClickParentTitle::getSCXMLStateName()const
{
    return _Title;
}