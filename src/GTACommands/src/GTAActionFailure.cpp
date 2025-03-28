
#include "GTAActionFailure.h"
#include "GTAUtil.h"

GTAActionFailure::GTAActionFailure():GTAActionBase()
{
    setAction(ACT_FAILURE);
}

GTAActionFailure::GTAActionFailure(const GTAActionFailure& rhs):GTAActionBase(rhs)
{

    setFailureName(rhs.getFailureName());

    setFailureType(rhs.getFailureType());

    QList <QPair<QString,GTAFailureConfig* >> lstParamCinfigVal = rhs.getParamValConfig();
    for (int i=0;i<lstParamCinfigVal.size();i++)
    {
        QPair<QString,GTAFailureConfig* > pairItem = lstParamCinfigVal.at(i);
        if (pairItem.second!=NULL)
        {
             
            _lstParamConfigVal.append( qMakePair(pairItem.first,new GTAFailureConfig(*pairItem.second) ));

        }
    }


}
void GTAActionFailure::setParamValConfig(QList <QPair<QString,GTAFailureConfig* >> ilstParamCinfigVal)
{
    _lstParamConfigVal=ilstParamCinfigVal;
}


QString GTAActionFailure::getStatement() const
{
    //QString sComplement = getComplement();
    //return QString("%1[%2] %3:\n parameters:\n%4").arg("Failure:",_failureName,sComplement,getValues().join("\n"));
    QString sComplement;

    switch (_FailureType)
    {
    case START:
        sComplement = "START";
        break;
    case STOP:
        sComplement = "STOP";
        break;

    case PAUSE:
        sComplement = "PAUSE";
        break;

    case RESUME:
        sComplement = "RESUME";
        break;

    default: 
        break;

    };

    return QString("Failure %1[%2]").arg(_failureName,sComplement);

}

QList<GTACommandBase*>& GTAActionFailure::getChildren(void) const
{
	return *_lstChildren;
}
bool GTAActionFailure::insertChildren(GTACommandBase*, const int&)
{
	return false;
}

void  GTAActionFailure::setParent(GTACommandBase* iParent)
{
	_parent=iParent;
}
GTACommandBase* GTAActionFailure::getParent(void) const
{
	return _parent;
}
GTACommandBase* GTAActionFailure::getClone() const
{
    return  new GTAActionFailure(*this);
}
bool GTAActionFailure::canHaveChildren() const
{
    return false;//true;
}
QStringList GTAActionFailure::getVariableList() const
{
     
    
    return  QStringList();//<<_ParameterName;

}

void GTAActionFailure:: ReplaceTag(const QMap<QString,QString>&) 
{
   /* QMapIterator<QString,QString> iterTag(iTagValueMap);
    while (iterTag.hasNext())
    {
       iterTag.next();
       QString tag = QString("%1%2%3").arg(QChar(TAG_IDENTIFIER),iterTag.key(),QChar(TAG_IDENTIFIER));

        _ParameterName.replace(tag,iterTag.value());
         

      
       
    }*/
}


void GTAActionFailure::stringReplace(const QRegExp&, const QString&)
{

    //_ParameterName.replace(iStrToFind,iStringToReplace);

        
}
bool GTAActionFailure::searchString(const QRegExp&, bool) const
{
    bool rc = false;
   /* foreach(QString param,_PrintValues)
    {
        rc = param.contains(iRegExp);
        if (rc)
            return rc;
    }

    if (ibTextSearch && this->getComment().contains(iRegExp))
        rc = true;
    else if (ibTextSearch && this->getStatement().contains(iRegExp))
        rc = true;
    else
        rc=false;*/
        
    return rc;
}
QString GTAActionFailure::getSCXMLStateName()const
{
    /*QString sComplement = getComplement();
    if(sComplement.isEmpty())
        sComplement="Table";*/
   /* QString sPrintVals;
    if(!_failureName.isEmpty())
        sPrintVals = _PrintValues.at(0);*/

   /* if(sComplement == COM_PRINT_MSG)
    {
        sComplement = "message to execution log";

        sPrintVals=sPrintVals.trimmed();
        QStringList words = sPrintVals.split(" ",QString::SkipEmptyParts);
        if (words.size()>=5)
            sPrintVals = QString("%1_%2_%3_%4_%5").arg(words.at(0),words.at(1),words.at(2),words.at(3),words.at(4));
    }*/
    

    

    QString sComplement = getFailureTypeInString();

    return QString("%1_%2_%3").arg("Failure",sComplement,_failureName);

}

void GTAActionFailure::replaceUntagged()
{
   /* for(int i=_PrintValues.count() -1;i>=0;i--)
    {
        QString data = _PrintValues.at(i);
        if(data.contains(TAG_IDENTIFIER))
            _PrintValues.removeAt(i);
    }
    */
}

QString GTAActionFailure::getFailureTypeInString  ()const
{
    QString sComplement;
    switch (_FailureType)
    {
    case START:
        sComplement = "START";
        break;
    case STOP:
        sComplement = "STOP";
        break;

    case PAUSE:
        sComplement = "PAUSE";
        break;

    case RESUME:
        sComplement = "RESUME";
        break;

    default: 
        break;

    };
    return sComplement;
}