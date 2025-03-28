#include "GTAEquationPulse.h"
#include "GTAUtil.h"


GTAEquationPulse::GTAEquationPulse():GTAEquationBase(GTAEquationBase::SQUARE)
{
}
GTAEquationPulse::GTAEquationPulse(const GTAEquationPulse& rhs):GTAEquationBase(GTAEquationBase::SQUARE)
{
    _Period=rhs.getPeriod();
    _maxValue=rhs.getMaxValue();
    _minValue=rhs.getMinValue();
    _direction=rhs.getDirection();
    _dutyCycle=rhs.getDutyCycle();
    this->setSolidState(rhs.getSolidState());
    this->setContinousState(rhs.getContinousState());
}
GTAEquationPulse::~GTAEquationPulse()
{
       
}

// SET Functions
void GTAEquationPulse::setPeriod(const QString &iVal)
{
    _Period = iVal;
}
void GTAEquationPulse::setMaxValue(const QString &iVal)
{
    _maxValue = iVal;
}
void GTAEquationPulse::setMinValue(const QString &iVal)
{
    _minValue = iVal;
}
void GTAEquationPulse::setDirection(const QString &iVal)
{
    _direction = iVal;
}
void GTAEquationPulse::setDutyCycle(const QString &iVal)
{
    _dutyCycle = iVal;
}

// GET Functions
QString GTAEquationPulse::getPeriod() const
{
    return _Period;
}
QString GTAEquationPulse::getMinValue() const
{
    return _minValue;
}
QString GTAEquationPulse::getMaxValue() const
{
    return _maxValue;
}
QString GTAEquationPulse::getDirection() const
{
    return _direction;
}
QString GTAEquationPulse::getDutyCycle() const
{
    return _dutyCycle;
}

QString GTAEquationPulse::getStatement() const
{
    QString sVarVal1 = QString("'%1':'%2'").arg(XATTR_EQ_PERIOD,     _Period);
    QString sVarVal2 = QString("'%1':'%2'").arg(XATTR_EQ_MAXVALUE,   _maxValue);
    QString sVarVal3 = QString("'%1':'%2'").arg(XATTR_EQ_MINVALUE,   _minValue);
    QString sVarVal4 = QString("'%1':'%2'").arg(XATTR_EQ_DUTYCYCLE,  _dutyCycle);
    QString sVarVal5 = QString("'%1':'%2'").arg(XATTR_EQ_DIRECTION,  _direction);

    QString oEquation = QString("%1{%2,%3,%4,%5,%6,'Solid':'%7','Continuous':'%8'}").arg(EQ_SQUARE,sVarVal1,sVarVal2,sVarVal3,sVarVal4,sVarVal5,getSolidState(),getContinousState());
    return oEquation;
}
 GTAEquationBase* GTAEquationPulse:: getClone()const
{
     return new GTAEquationPulse(*this);
 }

 bool GTAEquationPulse::operator ==(GTAEquationBase*& pObj) const
 {
     GTAEquationPulse *pRcvdObj = dynamic_cast<GTAEquationPulse *>(pObj);
     if (pRcvdObj != nullptr)
         return ((this->getPeriod() == pRcvdObj->getPeriod()) &&
                 (this->getMinValue() == pRcvdObj->getMinValue()) &&
                 (this->getMaxValue() == pRcvdObj->getMaxValue()) &&
                 (this->getDirection() == pRcvdObj->getDirection()) &&
                 (this->getDutyCycle() == pRcvdObj->getDutyCycle()) &&
                 (this->getSolidState() == pRcvdObj->getSolidState()) &&
                 (this->getContinousState() == pRcvdObj->getContinousState()));
     else
         return false;

 }
void  GTAEquationPulse::getFunctionStatement(QString& osFuncType,QString& osFunctionStatement)const
{
    osFuncType = QString(EQ_SQUARE).toUpper();

    QString sVarVal1 = QString("'%1':'%2'").arg(XATTR_EQ_PERIOD,     _Period);
    QString sVarVal2 = QString("'%1':'%2'").arg(XATTR_EQ_MAXVALUE,   _maxValue);
    QString sVarVal3 = QString("'%1':'%2'").arg(XATTR_EQ_MINVALUE,   _minValue);
    QString sVarVal4 = QString("'%1':'%2'").arg(XATTR_EQ_DUTYCYCLE,  _dutyCycle);
    QString sVarVal5 = QString("'%1':'%2'").arg(XATTR_EQ_DIRECTION,  _direction);

    osFunctionStatement = QString("%1{%2,%3,%4,%5,%6,'Solid':'%7','Continuous':'%8'}").arg("'FunctionValue':",sVarVal1,sVarVal2,sVarVal3,sVarVal4,sVarVal5,getSolidState(),getContinousState());
}

void  GTAEquationPulse::getEquationArguments(QHash<QString,QString>& iSerializationMap)const
{
    iSerializationMap.insert(XATTR_EQ_PERIOD,    _Period);
    iSerializationMap.insert(XATTR_EQ_MAXVALUE,  _maxValue);
    iSerializationMap.insert(XATTR_EQ_MINVALUE,  _minValue);
    iSerializationMap.insert(XATTR_EQ_DUTYCYCLE, _dutyCycle);
    iSerializationMap.insert(XATTR_EQ_DIRECTION, _direction);
}
void  GTAEquationPulse::setEquationArguments(const QHash<QString,QString>& iSerializationMap)
{
    if (iSerializationMap.contains(XATTR_EQ_MAXVALUE))
        _maxValue = iSerializationMap.value(XATTR_EQ_MAXVALUE);
    
    if (iSerializationMap.contains(XATTR_EQ_MINVALUE))
        _minValue = iSerializationMap.value(XATTR_EQ_MINVALUE);

    if (iSerializationMap.contains(XATTR_EQ_DIRECTION))
        _direction = iSerializationMap.value(XATTR_EQ_DIRECTION);

    if (iSerializationMap.contains(XATTR_EQ_DUTYCYCLE))
        _dutyCycle = iSerializationMap.value(XATTR_EQ_DUTYCYCLE);

    if (iSerializationMap.contains(XATTR_EQ_PERIOD))
        _Period = iSerializationMap.value(XATTR_EQ_PERIOD);
}

QStringList GTAEquationPulse::getVariableList() const
{
    QStringList varList;

    if (GTACommandBase::isVariable(_Period))
         varList << _Period;
    if (GTACommandBase::isVariable(_maxValue))
        varList << _maxValue;
    if (GTACommandBase::isVariable(_minValue))
        varList << _minValue;
    if (GTACommandBase::isVariable(_dutyCycle))
        varList << _dutyCycle;
    
    return varList;
}
void GTAEquationPulse:: ReplaceTag(const QMap<QString,QString>& iTagValueMap) 
{
    QMapIterator<QString,QString> iterTag(iTagValueMap);
    while (iterTag.hasNext())
    {
        iterTag.next();
        QString tag = QString("%1%2%3").arg(QChar(TAG_IDENTIFIER),iterTag.key(),QChar(TAG_IDENTIFIER));
      
        if (GTACommandBase::isVariable(_Period))
            _Period.replace(tag, iterTag.value());
        if (GTACommandBase::isVariable(_maxValue))
           _maxValue.replace(tag, iterTag.value());
        if (GTACommandBase::isVariable(_minValue))
            _minValue.replace(tag, iterTag.value());
        if (GTACommandBase::isVariable(_dutyCycle))
            _dutyCycle.replace(tag, iterTag.value());
        if (GTACommandBase::isVariable(_direction))
            _direction.replace(tag, iterTag.value());
    }
}
