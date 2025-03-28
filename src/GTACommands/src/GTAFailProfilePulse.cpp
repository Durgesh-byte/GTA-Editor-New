#include "GTAFailProfilePulse.h"
#include "GTAUtil.h"

GTAFailProfilePulse::GTAFailProfilePulse():GTAFailureProfileBase(GTAFailureProfileBase::PULSE)
{
    _dutyCycle="0.5";
}
GTAFailProfilePulse::GTAFailProfilePulse(const GTAFailProfilePulse& rhs):GTAFailureProfileBase(GTAFailureProfileBase::PULSE)
{
    _Period=rhs.getPeriod();
    _Offset=rhs.getOffset();
    _Phase=rhs.getPhase();
    _dutyCycle=rhs.getDutyCycle();

}
GTAFailProfilePulse::~GTAFailProfilePulse()
{
       
}
void GTAFailProfilePulse::setPeriod(const QString &iVal)
{
 _Period=iVal;
}

void GTAFailProfilePulse::setOffset(const QString &iVal)
{
 _Offset=iVal;
}
void GTAFailProfilePulse::setPhase(const QString &iVal)
{
 _Phase=iVal;
}
void GTAFailProfilePulse::setDutyCycle(const QString &iVal)
{
    _dutyCycle=iVal;;
}

QString GTAFailProfilePulse::getPeriod() const
{
    return _Period;
}

QString GTAFailProfilePulse::getOffset() const
{
    return _Offset;
}
QString GTAFailProfilePulse::getPhase() const
{
    return _Phase;
} 
QString GTAFailProfilePulse::getDutyCycle() const
{
    return _dutyCycle;
}
QString GTAFailProfilePulse::getStatement() const
{
    QString sVarVal1= 		QString("%1=%2").arg(XATTR_EQ_MAXVALUE,_dutyCycle);
    QString sVarVal2= 		QString("%1=%2").arg(XATTR_EQ_MINVALUE,_Period);
    QString sVarVal3= 		QString("%1=%2").arg(XATTR_EQ_OFFSET,_Offset);
    QString sVarVal4= 		QString("%1=%2").arg(XATTR_EQ_PHASE,_Phase);


    QString oEquation = QString("%1(%2,%3,%4,%5)").arg(EQ_SQUARE,sVarVal1,sVarVal2,sVarVal3,sVarVal4);
    return oEquation;
}
 GTAFailureProfileBase* GTAFailProfilePulse:: getClone()const
{
	return new GTAFailProfilePulse(*this);
}
void  GTAFailProfilePulse::getFunctionStatement(QString&, QString& osFunctionStatement) const
{
    //osFuncType=QString(EQ_SQUARE).toUpper();
    //QString sVarVal1= QString("'%1':'%2'").arg(XATTR_EQ_MAXVALUE,_dutyCycle);
    //QString sVarVal2= QString("'%1':'%2'").arg(XATTR_EQ_MINVALUE,_Period);
    //QString sVarVal3= QString("'%1':'%2'").arg(XATTR_EQ_OFFSET,_Offset);
    //QString sVarVal4= QString("'%1':'%2'").arg(XATTR_EQ_PHASE,_Phase);

    //osFunctionStatement = QString("%1{%2,%3,%4,%5}").arg("'FunctionValue':",sVarVal1,sVarVal2,sVarVal3,sVarVal4);
    osFunctionStatement=QString("'type':'PULSE','mode':2,'dutycycle':%1,'period':%2,'offset':%3,'phase':%4").arg(_dutyCycle,_Period,_Offset,_Phase);
}
void  GTAFailProfilePulse::getFailureProfileArguments(QHash<QString,QString>& iSerialzationMap)const
{

    iSerialzationMap.insert(XATTR_EQ_OFFSET,_Offset);
    iSerialzationMap.insert(XATTR_EQ_PHASE,_Phase);
    iSerialzationMap.insert(XATTR_EQ_DUTYCYCLE,_dutyCycle);
    iSerialzationMap.insert(XATTR_EQ_PERIOD,_Period);
}
void  GTAFailProfilePulse::setFailureProfileArguments(const QHash<QString,QString>& iSerialzationMap)
{

    if (iSerialzationMap.contains(XATTR_EQ_OFFSET))
        _Offset=iSerialzationMap.value(XATTR_EQ_OFFSET);

    if (iSerialzationMap.contains(XATTR_EQ_PHASE))
        _Phase=iSerialzationMap.value(XATTR_EQ_PHASE);

    if (iSerialzationMap.contains(XATTR_EQ_DUTYCYCLE))
        _dutyCycle=iSerialzationMap.value(XATTR_EQ_DUTYCYCLE);

    if (iSerialzationMap.contains(XATTR_EQ_PERIOD))
        _Period=iSerialzationMap.value(XATTR_EQ_PERIOD);
    
}

QStringList GTAFailProfilePulse::getVariableList() const
{
    QStringList varList;

    if (GTACommandBase::isVariable(_Period))
         varList<<_Period;
    if (GTACommandBase::isVariable(_Offset))
        varList<<_Offset;
    if (GTACommandBase::isVariable(_Phase))
        varList<<_Phase;
    if (GTACommandBase::isVariable(_dutyCycle))
        varList<<_dutyCycle;
    
    return varList;
}
void GTAFailProfilePulse:: ReplaceTag(const QMap<QString,QString>& iTagValueMap) 
{
    QMapIterator<QString,QString> iterTag(iTagValueMap);
    while (iterTag.hasNext())
    {
        iterTag.next();
        QString tag = QString("%1%2%3").arg(QChar(TAG_IDENTIFIER),iterTag.key(),QChar(TAG_IDENTIFIER));
      
        if (GTACommandBase::isVariable(_Period))
            _Period.replace(tag,iterTag.value());
        if (GTACommandBase::isVariable(_Offset))
            _Offset.replace(tag,iterTag.value());
        if (GTACommandBase::isVariable(_Phase))
           _Phase.replace(tag,iterTag.value());
        if (GTACommandBase::isVariable(_dutyCycle))
            _dutyCycle.replace(tag,iterTag.value());

    }
}