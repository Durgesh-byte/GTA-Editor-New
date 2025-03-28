#include "GTAFailProfileSawTooth.h"
#include "GTAUtil.h"


GTAFailProfileSawTooth::GTAFailProfileSawTooth():GTAFailureProfileBase(GTAFailureProfileBase::SAWTOOTH)
{
}
GTAFailProfileSawTooth::GTAFailProfileSawTooth(const GTAFailProfileSawTooth& rhs):GTAFailureProfileBase(GTAFailureProfileBase::SAWTOOTH)
{
    _Period=rhs.getPeriod();
    _Phase=rhs.getPhase();
    _Offset=rhs.getOffset();

}
GTAFailProfileSawTooth::~GTAFailProfileSawTooth()
{

}
void GTAFailProfileSawTooth::setPeriod(const QString &iVal)
{
    _Period=iVal;
}

void GTAFailProfileSawTooth::setOffset(const QString &iVal)
{
    _Offset=iVal;
}
void GTAFailProfileSawTooth::setPhase(const QString &iVal)
{
    _Phase=iVal;
}


QString GTAFailProfileSawTooth::getPeriod() const
{
    return _Period;
}

QString GTAFailProfileSawTooth::getOffset() const
{
    return _Offset;
}
QString GTAFailProfileSawTooth::getPhase() const
{
    return _Phase;
} 
QString GTAFailProfileSawTooth::getStatement() const
{
    
    QString sVarVal1= QString("%1=%2").arg(XATTR_EQ_PHASE,_Phase);
    QString sVarVal2= QString("%1=%2").arg(XATTR_EQ_OFFSET,_Offset);
    QString sVarVal3= QString("%1=%2").arg(XATTR_EQ_PERIOD,_Period);
    QString oEquation = QString("%1(%2,%3,%4)").arg(EQ_SAWTOOTH,sVarVal1,sVarVal2,sVarVal3);


    return oEquation;
}
GTAFailureProfileBase* GTAFailProfileSawTooth:: getClone()const
{
    return new GTAFailProfileSawTooth(*this);
}
void  GTAFailProfileSawTooth::getFunctionStatement(QString&, QString& osFunctionStatement) const
{
    
    //osFuncType=QString(EQ_SAWTOOTH).toUpper();
    //QString sVarVal1= QString("'%1':'%2'").arg(XATTR_EQ_PHASE,_Phase);
    //QString sVarVal2= QString("'%1':'%2'").arg(XATTR_EQ_OFFSET,_Offset);
    //QString sVarVal3= QString("'%1':'%2'").arg(XATTR_EQ_PERIOD,_Period);


    //osFunctionStatement = QString("%1{%2,%3,%4}").arg("'FunctionValue':",sVarVal1,sVarVal2,sVarVal3);
    osFunctionStatement=QString("'type':'SAWTOOTH','mode':2,'phase':%1,'offset':%2,'period':%3").arg(_Phase,_Offset,_Period);
    //iSerialzationMap.insert()
}
void  GTAFailProfileSawTooth::getFailureProfileArguments(QHash<QString,QString>& iSerialzationMap)const
{
     iSerialzationMap.insert(XATTR_EQ_PHASE,_Phase);
     iSerialzationMap.insert(XATTR_EQ_OFFSET,_Offset);
     iSerialzationMap.insert(XATTR_EQ_PERIOD,_Period);

}
void  GTAFailProfileSawTooth::setFailureProfileArguments(const QHash<QString,QString>& iSerialzationMap)
{
     if (iSerialzationMap.contains(XATTR_EQ_PHASE))
          _Phase=iSerialzationMap.value(XATTR_EQ_PHASE);
    

     if (iSerialzationMap.contains(XATTR_EQ_OFFSET))
         _Offset=iSerialzationMap.value(XATTR_EQ_OFFSET);

     if (iSerialzationMap.contains(XATTR_EQ_PERIOD))
         _Period=iSerialzationMap.value(XATTR_EQ_PERIOD);

}

QStringList GTAFailProfileSawTooth::getVariableList() const
{
    QStringList varList;

    if (GTACommandBase::isVariable(_Period))
        varList<<_Period;
    if (GTACommandBase::isVariable(_Phase))
        varList<<_Phase;
    if (GTACommandBase::isVariable(_Offset))
        varList<<_Offset;

    return varList;
}
void GTAFailProfileSawTooth:: ReplaceTag(const QMap<QString,QString>& iTagValueMap) 
{
    QMapIterator<QString,QString> iterTag(iTagValueMap);
    while (iterTag.hasNext())
    {
        iterTag.next();
        QString tag = QString("%1%2%3").arg(QChar(TAG_IDENTIFIER),iterTag.key(),QChar(TAG_IDENTIFIER));

        if (GTACommandBase::isVariable(_Period))
            _Period.replace(tag,iterTag.value());
        
        if (GTACommandBase::isVariable(_Phase))
            _Phase.replace(tag,iterTag.value());

        if (GTACommandBase::isVariable(_Offset))
            _Offset.replace(tag,iterTag.value());

    }
}