
#include "GTAActionPrintTable.h"
#include "GTAUtil.h"

GTAActionPrintTable::GTAActionPrintTable():GTAActionBase()
{
  //  setAction(ACT_PRINT_TABLE);

	 setAction(ACT_PRINT);
    setComplement(COM_PRINT_TABLE);
}

GTAActionPrintTable::GTAActionPrintTable(const GTAActionPrintTable& rhs):GTAActionBase(rhs)
{
   /* setAction(rhs.getAction());
    setComplement(rhs.getComplement());*/
    _PrintValues = rhs.getValues();
   // setComment(rhs.getComment());
    setTableName(rhs.getTableName());
    setTitleName(rhs.getTitleName());
}
void GTAActionPrintTable::setValues(const QStringList & iParam)
{
    _PrintValues =iParam;
}

QStringList GTAActionPrintTable::getValues() const
{
    return _PrintValues;
}
QString GTAActionPrintTable::getStatement() const
{
    QString sComplement = getComplement();
    QStringList values = getValues();
    QStringList ValuesLst;

    for(int i = 0; i < values.count(); i++)
    {
       QString val = values.at(i);
       getTrimmedStatement(val);
       ValuesLst<<val;
    }
    return QString("%1[%2] %3:\n parameters:\n%4").arg("Print Table",_tableName,"",ValuesLst.join("\n"));
}
QString GTAActionPrintTable::getLTRAStatement() const
{
        return QString("#c#%1#c# [#b#%2#b#]").arg("PRINT TABLE",_tableName);//,getValues().join("\n"));
}
QList<GTACommandBase*>& GTAActionPrintTable::getChildren(void) const
{
	return *_lstChildren;
}
bool GTAActionPrintTable::insertChildren(GTACommandBase*, const int&)
{
	return false;
}

void  GTAActionPrintTable::setParent(GTACommandBase* iParent)
{
	_parent=iParent;
}
GTACommandBase* GTAActionPrintTable::getParent(void) const
{
	return _parent;
}
GTACommandBase* GTAActionPrintTable::getClone() const
{
    return  new GTAActionPrintTable(*this);
}
bool GTAActionPrintTable::canHaveChildren() const
{
    return false;//true;
}
QStringList GTAActionPrintTable::getVariableList() const
{
    QStringList lstOfVars;
    
    foreach(QString sPrintValue, _PrintValues)
    {
        if( GTACommandBase::isVariable(sPrintValue))
        {
            lstOfVars.append(sPrintValue);
        }
        

    }
   
    return lstOfVars;

}

void GTAActionPrintTable:: ReplaceTag(const QMap<QString,QString>& iTagValueMap) 
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


void GTAActionPrintTable::stringReplace(const QRegExp& iStrToFind,const QString& iStringToReplace)
{

    _PrintValues.replaceInStrings(iStrToFind,iStringToReplace);

        
}
bool GTAActionPrintTable::equipmentReplace(const QRegExp& iStrToFind, const QString& iStringToReplace)
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
bool GTAActionPrintTable::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
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
bool GTAActionPrintTable::hasChannelInControl()const
{
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
QString GTAActionPrintTable::getSCXMLStateName()const
{
    QString sComplement = getComplement();
    if(sComplement.isEmpty())
        sComplement="Table";
    QString sPrintVals;
    if(!_PrintValues.isEmpty())
        sPrintVals = _PrintValues.at(0);

   /* if(sComplement == COM_PRINT_MSG)
    {
        sComplement = "message to execution log";

        sPrintVals=sPrintVals.trimmed();
        QStringList words = sPrintVals.split(" ",QString::SkipEmptyParts);
        if (words.size()>=5)
            sPrintVals = QString("%1_%2_%3_%4_%5").arg(words.at(0),words.at(1),words.at(2),words.at(3),words.at(4));
    }*/
    

    return QString("%1_%2_%3").arg("print",sComplement,sPrintVals);

}

void GTAActionPrintTable::replaceUntagged()
{
    for(int i=_PrintValues.count() -1;i>=0;i--)
    {
        QString data = _PrintValues.at(i);
        if(data.contains(TAG_IDENTIFIER))
            _PrintValues.removeAt(i);
    }
    
}
