#include "GTAActionCondition.h"
#include "GTAUtil.h"


GTAActionCondition::GTAActionCondition()
{
	
}
GTAActionCondition::~GTAActionCondition()
{

}

void GTAActionCondition::setParameter(const QString & iParam)
{
    _Param =iParam;
}

void GTAActionCondition::setValue(const QString & iParam)
{
    _ParamValue = iParam;
}

void GTAActionCondition::setCondition(const QString & iOperator)
{
    _Operator = iOperator;
}


QString GTAActionCondition::getParameter() const
{
    return _Param;
}
QString GTAActionCondition::getValue()const
{
    return _ParamValue;
}
QString GTAActionCondition::getCondition() const
{
    return _Operator;
}

QString GTAActionCondition::getStatement() const
{
    QString oAction;
    QString paramName = getParameter();
    getTrimmedStatement(paramName);
    oAction = QString("%1 %2 %3 %4").arg(getComplement(),paramName,getCondition(),getValue());
    return oAction;
}

QList<GTACommandBase*>& GTAActionCondition::getChildren(void) const
{
    return *_lstChildren;
}
bool GTAActionCondition::insertChildren(GTACommandBase* pBase,const int& idPos)
{
    bool rc = false;
    int currSize=_lstChildren->size();
    if (idPos<=currSize && currSize>=0)
    {
        _lstChildren->insert(idPos,pBase);
        rc = true;
    }
    return rc;
}
QString GTAActionCondition::getGlobalResultStatus()
{
	QString data = "OK";

	QList<GTACommandBase *> childrens = getChildren();

	for(int i=0;i<childrens.count();i++)
	{
		GTACommandBase *pCmd = childrens.at(i);
		data = pCmd->getGlobalResultStatus();
		if(data == "KO")
			break;
	}
	return data;
}
void  GTAActionCondition::setParent(GTACommandBase* iParent)
{
    _parent=iParent;
}
GTACommandBase* GTAActionCondition::getParent(void) const
{
    return _parent;
}
bool GTAActionCondition::canHaveChildren()const
{
    return false;//true
}
QStringList GTAActionCondition::getVariableList() const
{
    return QStringList();

}
QString GTAActionCondition:: getLTRAStatement()  const
{
    QString parameter = getParameter();
    QString val = getValue();
    QString val1 = "";
    QString val2 = "";
//    if(parameter.contains("."))
//    {
//        QStringList items = parameter.trimmed().split(".");
//        if(!items.isEmpty())
//            val1 =items.last();
//    }
//    if(val.contains("."))
//    {
//        QStringList items = val.trimmed().split(".");
//        if(!items.isEmpty())
//            val2 =items.last();
//    }
    QString oAction;
    oAction = QString("#c#%1#c# #b#%2#b# %3 #b#%4#b#").arg(getComplement().toUpper(),parameter,getCondition(),val);
    return oAction;
}
void GTAActionCondition:: ReplaceTag(const QMap<QString,QString>& iTagValueMap) 
{
    iTagValueMap;//to suppress warning
    //do nothing
}
GTACommandBase*GTAActionCondition::getClone() const
{
    return NULL;
}
void GTAActionCondition::replaceUntagged()
{
    int currChildrenSize = _lstChildren->size();
    for (int i=currChildrenSize-2;i>=0;i--)
    {
        GTACommandBase* pCurrentChild = _lstChildren->at(i);

        if (pCurrentChild!=NULL)
        {
            //if (pCurrentChild->canHaveChildren())
            //{
            pCurrentChild->replaceUntagged();
            //}

            QStringList varList = pCurrentChild->getVariableList();
            bool contains = false;
            foreach(QString varName,varList)
            {
                if(varName.contains(TAG_IDENTIFIER))
                {
                    contains = true;
                }
            }
            if(contains )
            {
                delete pCurrentChild ;
                _lstChildren->removeAt(i);
            }
        }
    }
}
