#include "GTAFailProfileRampSlope.h"
#include "GTAUtil.h"

GTAFailProfileRampSlope::GTAFailProfileRampSlope():GTAFailureProfileBase(GTAFailureProfileBase::RAMPSLOPE)
{

}
GTAFailProfileRampSlope::GTAFailProfileRampSlope(const GTAFailProfileRampSlope& rhs):GTAFailureProfileBase(GTAFailureProfileBase::RAMPSLOPE)
{
    _Offset=rhs.getOffset();
    _Slope=rhs.getSlope();
    
   
}
GTAFailProfileRampSlope::~GTAFailProfileRampSlope()
{
       
}
void GTAFailProfileRampSlope::setOffset(const QString &iVal)
{
 _Offset=iVal;
}

void GTAFailProfileRampSlope::setSlope(const QString &iVal)
{
 _Slope=iVal;
}


QString GTAFailProfileRampSlope::getOffset() const
{
    return _Offset;
}

QString GTAFailProfileRampSlope::getSlope() const
{
    return _Slope;
}


QString GTAFailProfileRampSlope::getStatement() const
{
    
    QString sVarVal1= 		QString("%1=%2").arg(XATTR_EQ_OFFSET,_Offset);
    QString sVarVal2= 		QString("%1=%2").arg(XATTR_EQ_SLOPE,_Slope);
  
    QString oEquation = QString("%1(%2,%3)").arg(FEQ_RAMPSLOPE,sVarVal1,sVarVal2);
    return oEquation;
}
 GTAFailureProfileBase* GTAFailProfileRampSlope:: getClone()const
{
	return new GTAFailProfileRampSlope(*this);
}
void  GTAFailProfileRampSlope::getFunctionStatement(QString&, QString& osFunctionStatement) const
{
    //osFuncType=QString(EQ_SQUARE).toUpper();
   
    //QString sVarVal1= QString("'%1':'%2'").arg(XATTR_EQ_OFFSET,_Offset);
    //QString sVarVal2= QString("'%1':'%2'").arg(XATTR_EQ_SLOPE,_Slope);
    //

    //osFunctionStatement = QString("%1{%2,%3}").arg("'FunctionValue':",sVarVal1,sVarVal2);
    osFunctionStatement=QString("'type':'RAMP','mode':2,'offset':%1,'slope':%2").arg(_Offset,_Slope);
}
void  GTAFailProfileRampSlope::getFailureProfileArguments(QHash<QString,QString>& iSerialzationMap)const
{

    iSerialzationMap.insert(XATTR_EQ_SLOPE,_Slope);
    iSerialzationMap.insert(XATTR_EQ_OFFSET,_Offset);
}
void  GTAFailProfileRampSlope::setFailureProfileArguments(const QHash<QString,QString>& iSerialzationMap)
{

    if (iSerialzationMap.contains(XATTR_EQ_SLOPE))
        _Slope=iSerialzationMap.value(XATTR_EQ_SLOPE);

     
    if (iSerialzationMap.contains(XATTR_EQ_OFFSET))
        _Offset=iSerialzationMap.value(XATTR_EQ_OFFSET);
    
}

QStringList GTAFailProfileRampSlope::getVariableList() const
{
    QStringList varList;

    if (GTACommandBase::isVariable(_Offset))
         varList<<_Offset;
    if (GTACommandBase::isVariable(_Slope))
        varList<<_Slope;
   
    return varList;
}
void GTAFailProfileRampSlope:: ReplaceTag(const QMap<QString,QString>& iTagValueMap) 
{
    QMapIterator<QString,QString> iterTag(iTagValueMap);
    while (iterTag.hasNext())
    {
        iterTag.next();
        QString tag = QString("%1%2%3").arg(QChar(TAG_IDENTIFIER),iterTag.key(),QChar(TAG_IDENTIFIER));
      
        if (GTACommandBase::isVariable(_Offset))
            _Offset.replace(tag,iterTag.value());
        if (GTACommandBase::isVariable(_Slope))
            _Slope.replace(tag,iterTag.value());
       
       

    }
}