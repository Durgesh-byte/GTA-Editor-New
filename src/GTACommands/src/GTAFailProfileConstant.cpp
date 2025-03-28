#include "GTAFailProfileConstant.h"
#include "GTAUtil.h"


GTAFailProfileConstant::GTAFailProfileConstant():GTAFailureProfileBase(GTAFailureProfileBase::CONST)
{
}
GTAFailProfileConstant::GTAFailProfileConstant(const GTAFailProfileConstant& rhs):GTAFailureProfileBase(GTAFailureProfileBase::CONST)
{    
    _Const=rhs.getConst();
   
}
GTAFailProfileConstant::~GTAFailProfileConstant()
{

}
void GTAFailProfileConstant::setConst(const QString &iVal)
{
    _Const=iVal;
}


QString GTAFailProfileConstant::getConst() const
{
    return _Const;
}


QString GTAFailProfileConstant::getStatement() const
{
    
    QString sVarVal1= QString("%1=%2").arg(XATTR_VAL,_Const);
    QString oEquation = QString("%1(%2)").arg(FEQ_CONST,sVarVal1);


    return oEquation;
}
GTAFailureProfileBase* GTAFailProfileConstant:: getClone()const
{
    return new GTAFailProfileConstant(*this);
}
void  GTAFailProfileConstant::getFunctionStatement(QString&, QString& osFunctionStatement) const
{
    
   //'type':'CONST','mode':2,'value':0
    osFunctionStatement = QString("'type':'CONST','mode':2,'value':%1").arg(_Const);
    //iSerialzationMap.insert()
}
void  GTAFailProfileConstant::getFailureProfileArguments(QHash<QString,QString>& iSerialzationMap)const
{
     iSerialzationMap.insert(XATTR_EQ_VAL,_Const);
    
}
void  GTAFailProfileConstant::setFailureProfileArguments(const QHash<QString,QString>& iSerialzationMap)
{
     if (iSerialzationMap.contains(XATTR_EQ_VAL))
          _Const=iSerialzationMap.value(XATTR_EQ_VAL);
    
  
}

QStringList GTAFailProfileConstant::getVariableList() const
{
    QStringList varList;

    if (GTACommandBase::isVariable(_Const))
        varList<<_Const;
   
    return varList;
}
void GTAFailProfileConstant:: ReplaceTag(const QMap<QString,QString>& iTagValueMap) 
{
    QMapIterator<QString,QString> iterTag(iTagValueMap);
    while (iterTag.hasNext())
    {
        iterTag.next();
        QString tag = QString("%1%2%3").arg(QChar(TAG_IDENTIFIER),iterTag.key(),QChar(TAG_IDENTIFIER));

        if (GTACommandBase::isVariable(_Const))
            _Const.replace(tag,iterTag.value());
        
      
    }
}