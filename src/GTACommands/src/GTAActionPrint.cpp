#include "GTAActionPrint.h"
#include "GTAUtil.h"

GTAActionPrint::GTAActionPrint()
{
}

GTAActionPrint::GTAActionPrint(const GTAActionPrint& rhs):GTAActionBase(rhs)
{
    /*setAction(rhs.getAction());
    setComplement(rhs.getComplement());*/
    _PrintValues = rhs.getValues();
    _PrintType = rhs.getPrintType();
    setMessageType(rhs.getMessageType());
    /* setComment(rhs.getComment());*/
}
void GTAActionPrint::setValues(const QStringList & iParam)
{
    _PrintValues =iParam;
}

QStringList GTAActionPrint::getValues() const
{
    return _PrintValues;
}

//void GTAActionPrint::getTrimmedStatement(QString &val)
//{
//    QString uuid = getObjId();
//    if(val.contains("uuid"))
//    {
//        val.remove(QString("_uuid[%1]").arg(uuid));
//    }
//}


QString GTAActionPrint::getStatement() const
{
    QString sComplement = getComplement();
    if(sComplement == COM_PRINT_MSG)
        sComplement = "message to execution log";

    QStringList values = getValues();
    QStringList ValuesLst;

    for(int i = 0; i < values.count(); i++)
    {
       QString val = values.at(i);
       getTrimmedStatement(val);
       ValuesLst<<val;
    }

    return QString("%1 %2: %3").arg("print",sComplement,ValuesLst.join("\n"));
}
QString GTAActionPrint::getLTRAStatement()  const
{
    QString sComplement = getComplement();
    if(sComplement == COM_PRINT_MSG)
    {
        sComplement = "message to execution log";

        return QString("#c#%1#c#").arg(getValues().join("\n"));
    }
    else
    {
        QStringList items = getValues();
        QStringList newItems;
        if(!items.isEmpty())
        {

            return QString("#c#%1#c# %2 #b#%3#b#").arg("PRINT"," the following parameters:",items.join("\n"));
        }
		return QString();
    }
}
QList<GTACommandBase*>& GTAActionPrint::getChildren(void) const
{
    return *_lstChildren;
}
bool GTAActionPrint::insertChildren(GTACommandBase*, const int&)
{
    return false;
}

void  GTAActionPrint::setParent(GTACommandBase* iParent)
{
    _parent=iParent;
}
GTACommandBase* GTAActionPrint::getParent(void) const
{
    return _parent;
}
GTACommandBase* GTAActionPrint::getClone() const
{
    return  new GTAActionPrint(*this);
}
bool GTAActionPrint::canHaveChildren() const
{
    return false;//true;
}
QStringList GTAActionPrint::getVariableList() const
{
    QStringList lstOfVars;
    if (getPrintType() == PARAMETER )
        foreach(QString sPrintValue, _PrintValues)
        {
        if( GTACommandBase::isVariable(sPrintValue))
        {
            lstOfVars.append(sPrintValue);
        }
        //GTACommandBase::collectVariableNames(_PrintValues,lstOfVars);

    }

    return lstOfVars;

}

void GTAActionPrint:: ReplaceTag(const QMap<QString,QString>& iTagValueMap) 
{
    QMapIterator<QString,QString> iterTag(iTagValueMap);
    while (iterTag.hasNext())
    {
        iterTag.next();
        QString tag = QString("%1%2%3").arg(QChar(TAG_IDENTIFIER),iterTag.key(),QChar(TAG_IDENTIFIER));
        for(int i=0;i<_PrintValues.size();i++)
        {
            QString sParameter =  _PrintValues.at(i);
            sParameter.replace(tag,iterTag.value());
            _PrintValues.replace(i,sParameter);

        }

    }
}
void GTAActionPrint::setPrintType(PrintType iType)
{
    _PrintType = iType;
}

GTAActionPrint::PrintType GTAActionPrint::getPrintType() const
{
    return _PrintType;
}
void GTAActionPrint::stringReplace(const QRegExp& iStrToFind,const QString& iStringToReplace)
{

    _PrintValues.replaceInStrings(iStrToFind,iStringToReplace);


}
bool GTAActionPrint::equipmentReplace(const QRegExp& iStrToFind, const QString& iStringToReplace)
{
    QStringList temp = _PrintValues;
    bool rc = false;
    for (auto i = 0; i < temp.size(); ++i)
    {
        if (temp[i].count('.') == 2) // triplet detection
        {
            temp[i].chop(temp[i].size() - temp[i].indexOf('.'));
            if (temp[i].contains(iStrToFind))
            {
                rc = true;
                temp[i].replace(iStrToFind, iStringToReplace);
                _PrintValues[i].remove(0, _PrintValues[i].indexOf('.'));
                _PrintValues[i].prepend(temp[i]);
            }
        }
    }
    return rc;
}
bool GTAActionPrint::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
{
    bool rc = false;
    foreach(QString param,_PrintValues)
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
        rc=false;

    return rc;
}
QString GTAActionPrint::getSCXMLStateName()const
{
    QString sComplement = getComplement();
    QString sPrintVals;
    if(!_PrintValues.isEmpty())
        sPrintVals = _PrintValues.at(0);

    if(sComplement == COM_PRINT_MSG)
    {
        sComplement = "message to execution log";

        sPrintVals=sPrintVals.trimmed();
        QStringList words = sPrintVals.split(" ",QString::SkipEmptyParts);
        if (words.size()>=5)
            sPrintVals = QString("%1_%2_%3_%4_%5").arg(words.at(0),words.at(1),words.at(2),words.at(3),words.at(4));
    }
    

    return QString("%1_%2_%3").arg("print",sComplement,sPrintVals);

}
bool GTAActionPrint::hasChannelInControl()const
{

    if(getComplement() == COM_PRINT_PARAM)
        for(int i=0;i<_PrintValues.size();i++)
        {

        QString paramName = _PrintValues.at(i);
        if (GTACommandBase::hasChannelInCtrl(paramName))
        {
            return true;
        }

    }

    return false;
}
