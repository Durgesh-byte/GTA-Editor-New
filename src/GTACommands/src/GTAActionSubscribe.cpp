#include "GTAActionSubscribe.h"
#include "GTAUtil.h"

GTAActionSubscribe::GTAActionSubscribe()
{
}

GTAActionSubscribe::GTAActionSubscribe(const GTAActionSubscribe& rhs):GTAActionBase(rhs)
{
    _ParamList = rhs.getValues();
}
void GTAActionSubscribe::setValues(const QStringList & iParamList)
{
    _ParamList =iParamList;
}
QStringList GTAActionSubscribe::getValues() const
{
    return _ParamList;
}
QString GTAActionSubscribe::getStatement() const
{
    QString sComplement = getComplement();
    return QString("%1 %2: %3").arg("initialize",sComplement,getValues().join(", "));
}
QString GTAActionSubscribe::getLTRAStatement()  const
{
    QString sComplement = getComplement();
    QStringList items = getValues();
    if(!items.isEmpty())
    {
        return QString("#c#%1#c# %2 #b#%3#b#").arg("Initialize"," the following parameters: \n",items.join("\n"));
    }
	return QString();
}
QList<GTACommandBase*>& GTAActionSubscribe::getChildren(void) const
{
    return *_lstChildren;
}
bool GTAActionSubscribe::insertChildren(GTACommandBase*, const int&)
{
    return false;
}
void  GTAActionSubscribe::setParent(GTACommandBase* iParent)
{
    _parent=iParent;
}
GTACommandBase* GTAActionSubscribe::getParent(void) const
{
    return _parent;
}
GTACommandBase* GTAActionSubscribe::getClone() const
{
    return  new GTAActionSubscribe(*this);
}
bool GTAActionSubscribe::canHaveChildren() const
{
    return false;
}
QStringList GTAActionSubscribe::getVariableList() const
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
void GTAActionSubscribe:: ReplaceTag(const QMap<QString,QString>& iTagValueMap)
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
void GTAActionSubscribe::stringReplace(const QRegExp& iStrToFind,const QString& iStringToReplace)
{
    _ParamList.replaceInStrings(iStrToFind,iStringToReplace);
}
bool GTAActionSubscribe::equipmentReplace(const QRegExp& iStrToFind, const QString& iStringToReplace)
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
bool GTAActionSubscribe::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
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
QString GTAActionSubscribe::getSCXMLStateName()const
{
    QString sComplement = getComplement();
    QString sPrintVals;
    if(!_ParamList.isEmpty())
        sPrintVals = _ParamList.at(0);

    return QString("%1_%2_%3").arg("initialize",sComplement,sPrintVals);
}
bool GTAActionSubscribe::hasChannelInControl()const
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
