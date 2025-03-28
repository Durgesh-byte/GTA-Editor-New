#include "GTAActionUnSubscribe.h"
#include "GTAUtil.h"
GTAActionUnSubscribe::GTAActionUnSubscribe():_isReleaseAll(false)
{
}

GTAActionUnSubscribe::GTAActionUnSubscribe(const QString &iAction, const QString & iComplement) : GTAActionBase(iAction,iComplement)
  ,_isReleaseAll(false)
{
}

GTAActionUnSubscribe::GTAActionUnSubscribe(const GTAActionUnSubscribe& rhs):GTAActionBase(rhs)
{
    _ParamList = rhs.getValues();
    _isReleaseAll = rhs.isReleaseAll();
}
void GTAActionUnSubscribe::setValues(const QStringList & iParamList)
{
    _ParamList =iParamList;
}
QStringList GTAActionUnSubscribe::getValues() const
{
    return _ParamList;
}
QString GTAActionUnSubscribe::getStatement() const
{
    QString sComplement = getComplement();
    if(_isReleaseAll)
    {
        return QString("Uninitialize all PIR parameters.\n");
    }
    else
        return QString("%1 %2: %3").arg("unInitialize",sComplement,getValues().join(", "));
}
QString GTAActionUnSubscribe::getLTRAStatement()  const
{
    QString sComplement = getComplement();
    QStringList items = getValues();
    if(!items.isEmpty())
    {
        return QString("#c#%1#c# %2 #b#%3#b#").arg("UnInitialize"," the following parameters: \n",items.join("\n"));
    }
    else
    {
        return QString("Uninitialize all PIR parameters.\n");
    }
}
QList<GTACommandBase*>& GTAActionUnSubscribe::getChildren(void) const
{
    return *_lstChildren;
}
bool GTAActionUnSubscribe::insertChildren(GTACommandBase*, const int&)
{
    return false;
}
void  GTAActionUnSubscribe::setParent(GTACommandBase* iParent)
{
    _parent=iParent;
}
GTACommandBase* GTAActionUnSubscribe::getParent(void) const
{
    return _parent;
}
GTACommandBase* GTAActionUnSubscribe::getClone() const
{
    return  new GTAActionUnSubscribe(*this);
}
bool GTAActionUnSubscribe::canHaveChildren() const
{
    return false;//true;
}
QStringList GTAActionUnSubscribe::getVariableList() const
{
    QStringList lstOfVars;
    foreach(QString sParam, _ParamList)
    {
        if( GTACommandBase::isVariable(sParam))
        {
            lstOfVars.append(sParam);
        }
    }

    return lstOfVars;

}
void GTAActionUnSubscribe:: ReplaceTag(const QMap<QString,QString>& iTagValueMap)
{
    QMapIterator<QString,QString> iterTag(iTagValueMap);
    while (iterTag.hasNext())
    {
        iterTag.next();
        QString tag = QString("%1%2%3").arg(QChar(TAG_IDENTIFIER),iterTag.key(),QChar(TAG_IDENTIFIER));
        for(int i=0;i<_ParamList.size();i++)
        {
            QString sParameter =  _ParamList.at(i);
            sParameter.replace(tag,iterTag.value());
            _ParamList.replace(i,sParameter);

        }

    }
}
void GTAActionUnSubscribe::stringReplace(const QRegExp& iStrToFind,const QString& iStringToReplace)
{

    _ParamList.replaceInStrings(iStrToFind,iStringToReplace);


}
bool GTAActionUnSubscribe::equipmentReplace(const QRegExp& iStrToFind, const QString& iStringToReplace)
{
    QStringList temp = _ParamList;
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
                _ParamList[i].remove(0, _ParamList[i].indexOf('.'));
                _ParamList[i].prepend(temp[i]);
            }
        }
    }
    return rc;
}
bool GTAActionUnSubscribe::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
{
    bool rc = false;
    foreach(QString param,_ParamList)
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
QString GTAActionUnSubscribe::getSCXMLStateName()const
{
    QString sComplement = getComplement();
    QString sPrintVals;
    if(!_ParamList.isEmpty())
        sPrintVals = _ParamList.at(0);

    return QString("%1_%2_%3").arg("unInitialize",sComplement,sPrintVals);

}
bool GTAActionUnSubscribe::hasChannelInControl()const
{
    for(int i=0;i<_ParamList.size();i++)
    {
        QString paramName = _ParamList.at(i);
        if (GTACommandBase::hasChannelInCtrl(paramName))
        {
            return true;
        }
    }
    return false;
}

void GTAActionUnSubscribe::setReleaseAll(bool iVal)
{
    _isReleaseAll = iVal;
}

bool GTAActionUnSubscribe::isReleaseAll()const
{
    return _isReleaseAll;
}
