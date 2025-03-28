#include "GTAActionMath.h"
#include "GTAUtil.h"

GTAActionMath::GTAActionMath()
{
    setAction(ACT_MATHS);
}

GTAActionMath::GTAActionMath(const GTAActionMath& rhs):GTAActionBase(rhs)
{
    /* setAction(rhs.getAction());
    setComplement(rhs.getComplement());
    setComment(rhs.getComment());*/
    setFirstVariable(rhs.getFirstVariable());
    setSecondVariable(rhs.getSecondVariable());
    setLocation(rhs.getLocation());
    setOperator(rhs.getOperator());
}
void GTAActionMath::setFirstVariable(const QString & ipVar)
{
    _var1=ipVar;
}
void GTAActionMath::setSecondVariable(const QString & ipVar)
{
    _var2=ipVar;
}
void GTAActionMath::setLocation(const QString & iLocation)
{
    _Location = iLocation;
}
void GTAActionMath::setOperator(const QString & iOperator)
{   
    _operator=iOperator;

}

QString GTAActionMath::getFirstVariable()const
{
    return _var1;
}
QString GTAActionMath::getSecondVariable()const
{
    return _var2;
}
QString GTAActionMath::getLocation()const
{
    return _Location;
}
QString GTAActionMath::getOperator()const
{
    return _operator;
}

QString GTAActionMath::getStatement() const
{
    QString sComplement= getComplement() ;

    QString var1 = _var1;
    getTrimmedStatement(var1);
    QString var2 = _var2;
    getTrimmedStatement(var2);

    if(sComplement== COM_MATHS_BINARY)
    {
        return  QString("Math Operation: [%1=%2 %3 %4]").arg(_Location,var1,_operator,var2);
    }
    else if (sComplement == COM_MATHS_POW)
    {
        return  QString("Math Operation: [%1=%2(%3,%4)]").arg(_Location,sComplement,var1,var2);
    }
    else if (sComplement == COM_MATHS_ABS)
    {
        return  QString("Math Operation: [%1=%2(%3)]").arg(_Location,sComplement,var1);
    }
    else
    {
        return  QString("Math Operation: [%1(%2)]").arg(sComplement,var1);
    }
}

QList<GTACommandBase*>& GTAActionMath::getChildren(void) const
{
    return *_lstChildren;
}
bool GTAActionMath::insertChildren(GTACommandBase*, const int&)
{
    return false;
}

void  GTAActionMath::setParent(GTACommandBase* iParent)
{
    _parent=iParent;
}
GTACommandBase* GTAActionMath::getParent(void) const
{
    return _parent;
}
GTACommandBase* GTAActionMath::getClone() const
{
    return  new GTAActionMath(*this);
}
bool GTAActionMath::canHaveChildren() const
{
    return false;//true;
}
QStringList GTAActionMath::getVariableList() const
{
    QStringList lstOfVars;

    if( GTACommandBase::isVariable(_var1))
        lstOfVars.append(_var1);
    if( GTACommandBase::isVariable(_var2))
        lstOfVars.append(_var2);

    return lstOfVars;

}

void GTAActionMath:: ReplaceTag(const QMap<QString,QString>& iTagValueMap) 
{
    QMapIterator<QString,QString> iterTag(iTagValueMap);
    QString var1 = _var1;
    QString var2 = _var2;
    QString location = _Location;
    while (iterTag.hasNext())
    {
        iterTag.next();
        QString tag = QString("%1%2%3").arg(QChar(TAG_IDENTIFIER),iterTag.key(),QChar(TAG_IDENTIFIER));

        var1.replace(tag,iterTag.value());
        var2.replace(tag,iterTag.value());
        location.replace(tag,iterTag.value());
    }
    _var1= var1;
    _var2= var2;
    _Location = location;
}

bool GTAActionMath::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
{
    bool rc = false;


    if (_var1.contains(iRegExp))
        return true;
    if (_var2.contains(iRegExp))
        return true;

    if (ibTextSearch && this->getComment().contains(iRegExp))
        rc = true;
    else if (ibTextSearch && this->getStatement().contains(iRegExp))
        rc = true;
    else
        rc=false;

    return rc;
}
QString GTAActionMath::getSCXMLStateName()const
{
    return getStatement();

}
void GTAActionMath::stringReplace(const QRegExp& iStrToFind,const QString& iStringToReplace)
{

    _var1.replace(iStrToFind,iStringToReplace);
    _var2.replace(iStrToFind,iStringToReplace);
    _Location.replace(iStrToFind,iStringToReplace);


}
