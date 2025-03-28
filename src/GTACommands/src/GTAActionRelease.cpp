#include "GTAActionRelease.h"
#include "GTAUtil.h"

GTAActionRelease::GTAActionRelease()
{
    _Parameter.clear();
}
GTAActionRelease::GTAActionRelease(const GTAActionRelease &rhs):GTAActionBase(rhs)
{
    /*setAction(rhs.getAction());
    setComplement(rhs.getComplement());

    QString sTimeOut,sTimeOutUnit,sPrecision,sPrecisionUnit;

    rhs.getPrecision(sPrecision,sPrecisionUnit);
    setPrecision(sPrecision,sPrecisionUnit);
    rhs.getTimeOut( sTimeOut,sTimeOutUnit);
    setTimeOut( sTimeOut,sTimeOutUnit);
    setActionOnFail(rhs.getActionOnFail());
    setCommandType(rhs.getCommandType());*/
    setParent(rhs.getParent());
   /* setComment(rhs.getComment());
    setInstanceName(rhs.getInstanceName());*/

    setParameterList(rhs.getParameterList());
}

void GTAActionRelease::setParameterList(const QStringList & iParam)
{
    _Parameter =iParam;
}

QStringList GTAActionRelease::getParameterList() const
{
    return _Parameter;
}
QString GTAActionRelease::getStatement() const
{
    
    QStringList paramList = getParameterList();
    /*QString val;
    foreach(QString param, paramList)
    {
        if(val.isEmpty())
            val = param;
        else
            val += " " + param;
    }*/

    return QString("%1 %2 - %3").arg(getAction(),getComplement(),paramList.join("\n"));
}
QString GTAActionRelease::getSCXMLStateName()const
{

    QStringList paramList = getParameterList();
    return QString("%1_%2_%3").arg(getAction(),getComplement(),paramList.at(0));

}
QList<GTACommandBase*>& GTAActionRelease::getChildren(void) const
{
	return *_lstChildren;
}
bool GTAActionRelease::insertChildren(GTACommandBase*, const int&)
{
	return false;
}
void  GTAActionRelease::setParent(GTACommandBase* iParent)
{
	_parent=iParent;
}
GTACommandBase* GTAActionRelease::getParent(void) const
{
	return _parent;
}
bool GTAActionRelease::canHaveChildren() const
{
    return false;
}
QStringList GTAActionRelease::getVariableList() const
{
    QStringList lstOfVars;
    foreach(QString param,getParameterList())
    {
        if (GTACommandBase::isVariable(param))
        {
            lstOfVars.append(param);
        }
    }
    return lstOfVars;
}
bool GTAActionRelease::getSCXMLConditionStatement(const QStringList &iIcdParamList, QString &oCondtionStatement, const QHash<QString, QString> &, const QString &, bool) const
{
	bool rc = false;
	QStringList params = getParameterList();
	QStringList conditionItems;
	for(int i=0;i<params.count();i++)
	{
		QString param = params.at(i);
		if(iIcdParamList.contains(param))
		{
			QString cond = QString("(%1.Status == 'OK')").arg(param);
			conditionItems.append(cond);
		}
		rc =true;
	}
	oCondtionStatement = conditionItems.join(" && ");
	return rc;
}
void GTAActionRelease::ReplaceTag(const QMap<QString,QString>& iTagValueMap)
{
    QMapIterator<QString,QString> iterTag(iTagValueMap);
    while (iterTag.hasNext())
    {
        iterTag.next();
        QString tag = QString("%1%2%3").arg(QChar(TAG_IDENTIFIER),iterTag.key(),QChar(TAG_IDENTIFIER));
        QStringList updatedParamList;
        QStringList paramList = getParameterList();
        for(int i = 0 ; i < paramList.count(); i++)
        {
            QString param = paramList.at(i);
            param.replace(tag,iterTag.value());
            updatedParamList.append(param);
        }
        setParameterList(updatedParamList);
    }
}
GTACommandBase* GTAActionRelease::getClone() const
{
    GTACommandBase* pBase = new GTAActionRelease(*this);
    return pBase;
}
void GTAActionRelease::stringReplace(const QRegExp& iStrToFind,const QString& iStringToReplace)
{
   _Parameter.replaceInStrings(iStrToFind,iStringToReplace);

}
bool GTAActionRelease::equipmentReplace(const QRegExp& iStrToFind, const QString& iStringToReplace)
{
    QStringList temp = _Parameter;
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
                _Parameter[i].remove(0, _Parameter[i].indexOf('.'));
                _Parameter[i].prepend(temp[i]);
            }
        }
    }
    return rc;
}
bool GTAActionRelease::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
{
    bool rc = false;

    foreach(QString sParam,_Parameter)
    {
       if (sParam.contains(iRegExp))
        return true;
    }
    
    if (ibTextSearch && this->getComment().contains(iRegExp))
        rc = true;
    else if (ibTextSearch && this->getStatement().contains(iRegExp))
        rc = true;
    else
        rc=false;
    return rc;

   
}
bool GTAActionRelease::hasChannelInControl() const
{

    foreach(QString parameter, _Parameter)
    {
        if(GTACommandBase::hasChannelInCtrl(parameter))
            return true;
    }
   return false;
}
