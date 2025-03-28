#include "GTAActionCallProcedures.h"
#include "GTAUtil.h"


GTACallProcItem::GTACallProcItem(call_type iItemType, const QString& iElemName,const QMap<QString,QString>& itagValue,const QString &iUUID)
{
    _type=iItemType;
    _elementName=iElemName;
    _tagValue=itagValue;
    _UUID = iUUID;
    
}
GTACallProcItem::GTACallProcItem(const GTACallProcItem& iRhs)
{
    _type           = iRhs._type;
    _elementName    = iRhs._elementName;
    _tagValue       = iRhs._tagValue;
    _UUID           = iRhs._UUID;


}

GTAActionCallProcedures::GTAActionCallProcedures()
{ 
    setAction(ACT_CALL_PROCS);
}

GTAActionCallProcedures::GTAActionCallProcedures(const GTAActionCallProcedures& rhs):GTAActionBase(rhs)
{
    setCallProcedures(rhs.getCallProcedures());
    setParallelExecution(rhs.getPrallelExecution());
}
void GTAActionCallProcedures::setCallProcedures(const QList<GTACallProcItem>& icallProcedures)
{
    _callProcedures=icallProcedures;
}

QList<GTACallProcItem> GTAActionCallProcedures::getCallProcedures() const
{
    return _callProcedures;
}
QString GTAActionCallProcedures::getStatement() const
{
    QString displayStr = QString("%1: \t").arg(ACT_CALL_PROCS);
    for (auto callItem : _callProcedures )
    {
        if(!callItem._elementName.isEmpty())
        {
            QStringList args;
            QString callItemDisplay;
            for (auto key : callItem._tagValue.keys())
            {
                if(!key.isEmpty())
                {
                    QString sVal =  callItem._tagValue.value(key);
                    if(!sVal.isEmpty())
                    {
                        args.append(key+" : "+sVal);
                    }
                }
            }
            if (!args.isEmpty())
                callItemDisplay = QString("%1(%2)").arg(callItem._elementName,args.join(", "));
            else
                callItemDisplay = QString("%1").arg(callItem._elementName);

            displayStr.append(callItemDisplay+"\t");
        }
    }
    return displayStr;
}

QString GTAActionCallProcedures::getLTRAStatement()  const
{
    QString displayStr = QString("#c#%1:#c# \n").arg(ACT_CALL_PROCS);
    for (auto callItem :_callProcedures )
    {
        if(!callItem._elementName.isEmpty())
        {
            QStringList args;
            QString callItemDisplay;
            for (auto key : callItem._tagValue.keys())
            {
                if(!key.isEmpty())
                {
                    QString sVal =  callItem._tagValue.value(key);
                    if(!sVal.isEmpty())
                    {
                        args.append(key+" : "+sVal);
                    }
                }
            }
            if (!args.isEmpty())
                callItemDisplay = QString("#c#CALL#c# - #b#%1#b# (%2)").arg(callItem._elementName,args.join(", "));
            else
                callItemDisplay = QString("#c#CALL#c# - #b#%1#b#").arg(callItem._elementName);

            displayStr.append(callItemDisplay+"\t\n");
        }
    }
    return displayStr;
}

QList<GTACommandBase*>& GTAActionCallProcedures::getChildren(void) const
{
    return *_lstChildren;
}
bool GTAActionCallProcedures::insertChildren(GTACommandBase* pBase,const int& idPos)
{
//    return false;
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

void  GTAActionCallProcedures::setParent(GTACommandBase* iParent)
{
    _parent=iParent;
}
GTACommandBase* GTAActionCallProcedures::getParent(void) const
{
    return _parent;
}
GTACommandBase* GTAActionCallProcedures::getClone() const
{
    return  new GTAActionCallProcedures(*this);
}
bool GTAActionCallProcedures::canHaveChildren() const
{
    return false;//true;
}
QStringList GTAActionCallProcedures::getVariableList() const
{
    return _lstOfVars;
}

void GTAActionCallProcedures::updateVariableList(QStringList& iVarList)
{
    _lstOfVars = iVarList;
}

void GTAActionCallProcedures:: ReplaceTag(const QMap<QString,QString>&) 
{
    /*QMapIterator<QString,QString> iterTag(iTagValueMap);
    while (iterTag.hasNext())
    {
       iterTag.next();
       QString tag = QString("%1%2%3").arg(QChar(TAG_IDENTIFIER),iterTag.key(),QChar(TAG_IDENTIFIER));
       for(int i=0;i<_callProcedures.size();i++)
       {
          QString sParameter =  _callProcedures.at(i);
          sParameter.replace(tag,iterTag.value());
          _callProcedures.replace(i,sParameter);

       }
       
    }*/
}

void GTAActionCallProcedures::stringReplace(const QRegExp&, const QString&)
{

    //_callProcedures.replaceInStrings(iStrToFind,iStringToReplace);


}
bool GTAActionCallProcedures::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
{
    bool rc = false;
    foreach(GTACallProcItem callItem,_callProcedures)
    {
        rc = callItem._elementName.contains(iRegExp);
        if (rc)
            return rc;
    }

    if (ibTextSearch && this->getComment().contains(iRegExp))
        rc = true;
    else if (ibTextSearch && this->getStatement().contains(iRegExp))
        rc = true;
    else
        rc=false;

    return rc;
}
QString GTAActionCallProcedures::getSCXMLStateName()const
{
    QStringList procNames = getProcedureNames();
    if (_callProcedures.size()<=2)
    {
        return QString("%1_%2").arg(ACT_CALL_PROCS,procNames.join("_"));
    }
    else
        return QString("%1_%2_%3").arg(ACT_CALL_PROCS,procNames.at(0),procNames.at(1));

}
bool GTAActionCallProcedures::getCallProcedure(const int& index, GTACallProcItem::call_type& oType, QString& oElemName, QMap<QString,QString>&  oTtagValue,QString &oUUID)const
{
    bool rc = false;
    if (index < _callProcedures.size())
    {
        rc = true;
        oElemName = _callProcedures.at(index)._elementName;
        oType = _callProcedures.at(index)._type;
        oTtagValue = _callProcedures.at(index)._tagValue;
        oUUID = _callProcedures.at(index)._UUID;
    }

    return rc;

}
void GTAActionCallProcedures::appendCallProcedure(const GTACallProcItem::call_type& iType, const QString& iElemName,const QMap<QString,QString>&  iTtagValue,const QString &iUUID)
{
    GTACallProcItem item(iType,iElemName,iTtagValue,iUUID);

    _callProcedures.append(item);

}
QStringList GTAActionCallProcedures::getProcedureNames()const
{

    QStringList lstOfProcedures;
    foreach(GTACallProcItem callItem,_callProcedures )
    {
        if(!callItem._elementName.isEmpty())
        {
            lstOfProcedures.append(callItem._elementName);

        }
    }
    return lstOfProcedures;
}

QString GTAActionCallProcedures::getGlobalResultStatus()
{
    QString result = "NA";
    QList <GTACommandBase*> lstChildrens = getChildren();
    if (!lstChildrens.isEmpty())
    {
        GTACommandBase* pCmd = NULL;
        for(int childIdx=0;childIdx<lstChildrens.count();childIdx++)
        {
            pCmd = lstChildrens.at(childIdx);
            if (pCmd != NULL)
            {
                result = pCmd->getGlobalResultStatus();
            }
            if ((result == "KO") || (result == "NA"))
                break;
        }
    }
    return result;
}
