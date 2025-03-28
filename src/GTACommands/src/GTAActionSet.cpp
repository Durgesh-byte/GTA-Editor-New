#include "GTAActionSet.h"
#include "GTAEquationBase.h"
#include "GTAEquationConst.h"

#pragma warning(push, 0)
#include <QMetaEnum>
#include <QLatin1String>
#pragma warning(pop)

GTAActionSet::GTAActionSet():GTAActionBase()
{
    _Parameter = QString();
	_pEquation = nullptr;
    _FunctionalStatus=ACT_FSSSM_NO;
    _SDIStatus = QString("00");
	_IsParamWithoutFS = false;
}

GTAActionSet::~GTAActionSet()
{
    if (_pEquation == nullptr)
    {
        delete _pEquation;
        _pEquation = nullptr;
    }
}

GTAActionSet::GTAActionSet(const GTAActionSet& rhs): GTAActionBase(rhs)
{
	_pEquation = nullptr;
    
    setIsSetOnlyFSFixed(rhs.getIsSetOnlyFSFixed());
    setIsSetOnlyFSVariable(rhs.getIsSetOnlyFSVariable());
    setIsSetOnlyValue(rhs.getIsSetOnlyValue());

    setParameter(rhs.getParameter());
    setFunctionStatus(rhs.getFunctionStatus());
    setIsSetOnlySDI(rhs.getIsSetOnlySDI());
    setSDIStatus(rhs.getSDIStatus());
	setIsParamWithoutFS(rhs.getIsParamWithoutFS());
    setParent(rhs.getParent());
    GTAEquationBase* pBase = rhs.getEquation();
    GTAEquationBase* pClone =NULL;
    if (pBase)
        pClone= pBase->getClone();

    this->setEquation(pClone);
}
GTACommandBase* GTAActionSet::getClone() const
{
    return new GTAActionSet(*this);
}
GTAActionSet::GTAActionSet(const QString &iAction, const QString & iComplement) : GTAActionBase(iAction,iComplement)
{
    _Parameter = QString();
    _pEquation = nullptr;
    _FunctionalStatus = ACT_FSSSM_NO;
    _IsSetOnlyFSFixed = false;
    _IsSetOnlyFSVariable = false;
    _SDIStatus = QString("00");
    _IsSetOnlySDI = false;
	_IsParamWithoutFS = false;
}
void GTAActionSet::setParameter(const QString & iParameter)
{
    _Parameter = iParameter;
}

void GTAActionSet::setEquation(GTAEquationBase *& ipEquation)
{
    if (_pEquation != nullptr)
    {
        delete _pEquation;
        _pEquation = nullptr;
    }
    _pEquation = ipEquation;
}

QString GTAActionSet::getParameter() const
{
    return _Parameter;
}
QString GTAActionSet::getValue() const
{
    QString value = _pEquation != NULL ? _pEquation->getStatement() : QString();
    return value;
}
GTAEquationBase * GTAActionSet::getEquation() const
{
    return _pEquation;
}

/* This function allow to create the output statement for the SET command
 * @info: The text can be different, it depends on the Functional Status and options
 * @return: none
*/
QString GTAActionSet::getStatement() const
{
    QString oAction;
	// Get PArameter and Value
    QString paramName = getParameter();
	QString Value = getValue();

    getTrimmedStatement(paramName);
	getTrimmedStatement(Value);

    if (_IsSetOnlyFSFixed || _IsSetOnlyFSVariable)
        oAction = QString("SET functional status of %1 to %2 ").arg(paramName,getFunctionStatus());
    else if(_IsSetOnlySDI)
        oAction = QString("SET SDI status of %1 to %2 ").arg(paramName,getSDIStatus());
	else if (_IsSetOnlyValue)
	{
		if(_IsParamWithoutFS)
			oAction = QString("SET value of %1 to %2 ").arg(paramName, Value);
		else
			oAction = QString("SET value of %1 (Function Status:%2) to %3 ").arg(paramName, getFunctionStatus(), Value);
	}
	else
        oAction = QString("SET value of %1 (Function Status:%2) to %3 ").arg(paramName,getFunctionStatus(),Value);
    return oAction;
}
QString GTAActionSet::getLTRAStatement()  const
{
    QString oAction ;
    QString parameter = getParameter();
    QString value = getValue();

    if (_IsSetOnlyFSFixed || _IsSetOnlyFSVariable)
    {
        oAction = QString("#c#SET#c# functional status of #b#%1#b# to #b#%2#b# ").arg(parameter,getFunctionStatus());
    }
    else if(_IsSetOnlySDI)
    {
        oAction = QString("#c#SET#c# SDI status of #b#%1#b# to #b#%2#b# ").arg(parameter,getSDIStatus());
    }
	else if (_IsSetOnlyValue)
	{
		if (_IsParamWithoutFS)
			oAction = QString("#c#SET#c# value of #b#%1#b# to #b#%2#b# ").arg(parameter, value);
		else
			oAction = QString("#c#SET#c# value of #b#%1#b# (Function Status:#b#%2#b#) to #b#%3#b# ").arg(parameter, getFunctionStatus(), value);
	}
    else
    {
        oAction = QString("#c#SET#c# value of #b#%1#b# (Function Status:#b#%2#b#) to #b#%3#b# ").arg(parameter,getFunctionStatus(),value);
    }
    return oAction;
}
QList<GTACommandBase*>& GTAActionSet::getChildren(void) const
{
    return *_lstChildren;
}
bool GTAActionSet::insertChildren(GTACommandBase*, const int&)
{
    return false;
}
void  GTAActionSet::setParent(GTACommandBase* iParent)
{
    _parent=iParent;
}
GTACommandBase* GTAActionSet::getParent(void) const
{
    return _parent;
}

bool GTAActionSet::canHaveChildren() const
{
    return false;//true;
}
QStringList GTAActionSet::getVariableList() const
{
    QStringList lstOfVars;
    if (GTACommandBase::isVariable(_Parameter))
    {
        lstOfVars.append(_Parameter);
    }
    if (_pEquation !=NULL )
    {
        QStringList lstEqnVars = _pEquation->getVariableList();
        lstOfVars.append(lstEqnVars);
    }
    return lstOfVars;
}
bool GTAActionSet::hasChannelInControl() const
{
    QStringList lstOfVars;
    if (GTACommandBase::hasChannelInCtrl(_Parameter))
    {
        return true;
    }
    if (_pEquation !=NULL )
    {
        QStringList lstEqnVars = _pEquation->getVariableList();
        foreach(QString variable, lstEqnVars)
        {
            if(GTACommandBase::hasChannelInCtrl(variable))
                return true;
        }
    }
    return false;
}
void GTAActionSet::ReplaceTag(const QMap<QString,QString> & iTagValueMap)
{
    QMapIterator<QString,QString> iterTag(iTagValueMap);
    while (iterTag.hasNext())
    {
        iterTag.next();
        QString tag = QString("%1%2%3").arg(QChar(TAG_IDENTIFIER),iterTag.key(),QChar(TAG_IDENTIFIER));
        _Parameter.replace(tag,iterTag.value());
        if (NULL!=_pEquation)
            _pEquation->ReplaceTag(iTagValueMap);
    }  
}

void GTAActionSet::setFunctionStatus(const QString& iFs)
{
    _FunctionalStatus = iFs;
}
QString GTAActionSet::getFunctionStatus()const
{
    return _FunctionalStatus;
}

void GTAActionSet::setSDIStatus(const QString& iSDIStatus)
{
    _SDIStatus = iSDIStatus;
}

QString GTAActionSet::getSDIStatus()const
{
    return _SDIStatus;
}

void GTAActionSet::stringReplace(const QRegExp& iStrToFind,const QString& iStringToReplace)
{
    _Parameter.replace(iStrToFind,iStringToReplace);
    if(_pEquation->getEquationType() == GTAEquationBase::CONST)
    {
        return _pEquation->stringReplace(iStrToFind,iStringToReplace);
    }
}
bool GTAActionSet::equipmentReplace(const QRegExp& iStrToFind, const QString& iStringToReplace)
{
    QString temp = _Parameter;
    bool rc = false;
    if (temp.count('.') == 2) // triplet detection
    {
        temp.chop(temp.size() - temp.indexOf('.'));
        if (temp.contains(iStrToFind))
        {
            rc = true;
            temp.replace(iStrToFind, iStringToReplace);
            _Parameter.remove(0, _Parameter.indexOf('.'));
            _Parameter.prepend(temp);
        }
    }
    return rc;
}
bool GTAActionSet::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
{
    bool rc = false;
    if(_Parameter.contains(iRegExp))
        rc=true;
    else if(_pEquation && _pEquation->getEquationType() == GTAEquationBase::CONST)
    {
        return _pEquation->searchString(iRegExp,ibTextSearch);
    }
    else if (ibTextSearch && this->getComment().contains(iRegExp))
        rc = true;
    else if (ibTextSearch && this->getStatement().contains(iRegExp))
        rc = true;
    else
        rc=false;
    return rc;
}


bool GTAActionSet::getIsSetOnlyFSFixed() const
{
    return _IsSetOnlyFSFixed;
}
void GTAActionSet::setIsSetOnlyFSFixed(const bool iIsSetOnlyFS)
{
    _IsSetOnlyFSFixed = iIsSetOnlyFS;
}

bool GTAActionSet::getIsSetOnlyFSVariable() const
{
    return _IsSetOnlyFSVariable;
}
void GTAActionSet::setIsSetOnlyFSVariable(const bool iIsSetOnlyFS)
{
    _IsSetOnlyFSVariable = iIsSetOnlyFS;
}

bool GTAActionSet::getIsSetOnlyValue() const
{
    return _IsSetOnlyValue;

}
void GTAActionSet::setIsSetOnlyValue(const bool IsSetOnlyValue)
{
    _IsSetOnlyValue = IsSetOnlyValue;
}

bool GTAActionSet::getIsSetOnlySDI() const
{
    return _IsSetOnlySDI;
}
void GTAActionSet::setIsSetOnlySDI(const bool iIsSetOnlySDI)
{
    _IsSetOnlySDI = iIsSetOnlySDI;
}
