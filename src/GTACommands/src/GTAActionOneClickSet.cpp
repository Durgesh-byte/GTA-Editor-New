#include "GTAActionOneClickSet.h"
#include "GTAEquationBase.h"
#include "GTAEquationConst.h"

#pragma warning(push, 0)
#include <QMetaEnum>
#include <QLatin1String>
#pragma warning(pop)

GTAActionOneClickSet::GTAActionOneClickSet():GTAActionBase()
{
    _Parameter = QString();
    _pEquation = NULL;
    _FunctionalStatus=ACT_FSSSM_NO;

}
GTAActionOneClickSet::GTAActionOneClickSet(const GTAActionOneClickSet& rhs): GTAActionBase(rhs)
{

    /*setAction(rhs.getAction());
    setComplement(rhs.getComplement());*/
	//from GTAActionBase
	/*QString sTimeOut,sTimeOutUnit,sPrecision,sPrecisionUnit;

	rhs.getPrecision(sPrecision,sPrecisionUnit);
	setPrecision(sPrecision,sPrecisionUnit);

	rhs.getTimeOut( sTimeOut,sTimeOutUnit);
	setTimeOut( sTimeOut,sTimeOutUnit);

	setCommandType(rhs.getCommandType());
	setActionOnFail(rhs.getActionOnFail());*/
    
    setIsSetOnlyFSFixed(rhs.getIsSetOnlyFSFixed());

	setParameter(rhs.getParameter());
	setFunctionStatus(rhs.getFunctionStatus());
    setActionOnFail(rhs.getActionOnFail());
    //copy  equation
    GTAEquationBase* pBase = rhs.getEquation();
    GTAEquationBase* pClone =NULL;
    if (pBase)
          pClone= pBase->getClone(); 
        
     this->setEquation(pClone);   

    /*setComment(rhs.getComment());*/
		
}
GTACommandBase* GTAActionOneClickSet::getClone() const
{
	return new GTAActionOneClickSet(*this);
}
GTAActionOneClickSet::GTAActionOneClickSet(const QString &iAction, const QString & iComplement) : GTAActionBase(iAction,iComplement)
{
    _Parameter = QString();
    _pEquation = NULL;
    _FunctionalStatus=ACT_FSSSM_NO;
    _IsSetOnlyFS=false;
}
void GTAActionOneClickSet::setParameter(const QString & iParameter)
{
    _Parameter = iParameter;
}

void GTAActionOneClickSet::setEquation(GTAEquationBase *& ipEquation)
{
    _pEquation = ipEquation;
}

QString GTAActionOneClickSet::getParameter() const
{
    return _Parameter;
}
QString GTAActionOneClickSet::getValue() const
{
    QString value = _pEquation != NULL ? _pEquation->getStatement() : QString();
    return value;
}
GTAEquationBase * GTAActionOneClickSet::getEquation() const
{
    return _pEquation;
}
QString GTAActionOneClickSet::getStatement() const
{
    QString oAction ;
    if (_IsSetOnlyFS)
        oAction = QString("ONE Click SET functional status of %1 to %2 ").arg(getParameter(),getFunctionStatus());
    else
        oAction = QString("ONE Click SET value of %1 (Function Status:%2) to %3 ").arg(getParameter(),getFunctionStatus(),getValue());
    return oAction;
}

QList<GTACommandBase*>& GTAActionOneClickSet::getChildren(void) const
{
	return *_lstChildren;
}
bool GTAActionOneClickSet::insertChildren(GTACommandBase*, const int&)
{
	return false;
}
void  GTAActionOneClickSet::setParent(GTACommandBase* iParent)
{
	_parent=iParent;
}
GTACommandBase* GTAActionOneClickSet::getParent(void) const
{
	return _parent;
}

bool GTAActionOneClickSet::canHaveChildren() const
{
    return false;//true;
}
QStringList GTAActionOneClickSet::getVariableList() const
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
     /*if (NULL!=_pEquation)
     {
         if(_pEquation->getEquationType()==GTAEquationBase::CONST)
         {
             GTAEquationConst* pEqnConst = dynamic_cast<GTAEquationConst*>(_pEquation);
            
             if (NULL!=pEqnConst)
             {
                 QString eqnVal = pEqnConst->getConstant();
                 if (GTACommandBase::isVariable(eqnVal))
                     lstOfVars.append(eqnVal);
             }
             
             
         }
     }*/
    //GTACommandBase::collectVariableNames(_Parameter,lstOfVars);
    return lstOfVars;

}
void GTAActionOneClickSet::ReplaceTag(const QMap<QString,QString> & iTagValueMap)
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

void GTAActionOneClickSet::setFunctionStatus(const QString& iFs)
{
    _FunctionalStatus = iFs;
}
QString GTAActionOneClickSet::getFunctionStatus()const
{
    return _FunctionalStatus;
}
void GTAActionOneClickSet::stringReplace(const QRegExp& iStrToFind,const QString& iStringToReplace)
{
    _Parameter.replace(iStrToFind,iStringToReplace);

}
bool GTAActionOneClickSet::equipmentReplace(const QRegExp& iStrToFind, const QString& iStringToReplace)
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
bool GTAActionOneClickSet::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
{
    bool rc = false;
    if(_Parameter.contains(iRegExp))
        rc=true;
    else if (ibTextSearch && this->getComment().contains(iRegExp))
        rc = true;
    else if (ibTextSearch && this->getStatement().contains(iRegExp))
        rc = true;
    else
        rc=false;
    return rc;
}


bool GTAActionOneClickSet::getIsSetOnlyFSFixed() const
{
return _IsSetOnlyFS;

}
void GTAActionOneClickSet::setIsSetOnlyFSFixed(const bool iIsSetOnlyFS)
{

    _IsSetOnlyFS = iIsSetOnlyFS;
}
