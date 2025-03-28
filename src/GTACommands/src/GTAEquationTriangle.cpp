#include "GTAEquationTriangle.h"
#include "GTAUtil.h"


GTAEquationTriangle::GTAEquationTriangle():GTAEquationBase(GTAEquationBase::TRIANGLE)
{
}
GTAEquationTriangle::GTAEquationTriangle(const GTAEquationTriangle& rhs):GTAEquationBase(GTAEquationBase::TRIANGLE)
{
    _Period = rhs.getPeriod();
    _minValue = rhs.getMinValue();
    _maxValue = rhs.getMaxValue();
    _startValue = rhs.getStartValue();
    _direction = rhs.getDirection();
    this->setSolidState(rhs.getSolidState());
    this->setContinousState(rhs.getContinousState());
}
GTAEquationTriangle::~GTAEquationTriangle()
{

}

// SET Functions
void GTAEquationTriangle::setPeriod(const QString &iVal)
{
    _Period = iVal;
}
void GTAEquationTriangle::setMaxValue(const QString &iVal)
{
    _maxValue = iVal;
}
void GTAEquationTriangle::setMinValue(const QString &iVal)
{
    _minValue = iVal;
}
void GTAEquationTriangle::setStartValue(const QString &iVal)
{
    _startValue = iVal;
}
void GTAEquationTriangle::setDirection(const QString &iVal)
{
    _direction = iVal;
}

// GET Functions
QString GTAEquationTriangle::getPeriod() const
{
    return _Period;
}
QString GTAEquationTriangle::getMinValue() const
{
    return _minValue;
}
QString GTAEquationTriangle::getMaxValue() const
{
    return _maxValue;
}
QString GTAEquationTriangle::getStartValue() const
{
    return _startValue;
}
QString GTAEquationTriangle::getDirection() const
{
    return _direction;
}

QString GTAEquationTriangle::getStatement() const
{
    QString sVarVal1 = QString("'%1':'%2'").arg(XATTR_EQ_PERIOD,     _Period);
    QString sVarVal2 = QString("'%1':'%2'").arg(XATTR_EQ_MAXVALUE,   _maxValue);
    QString sVarVal3 = QString("'%1':'%2'").arg(XATTR_EQ_MINVALUE,   _minValue);
    QString sVarVal4 = QString("'%1':'%2'").arg(XATTR_EQ_STARTVALUE, _startValue);
    QString sVarVal5 = QString("'%1':'%2'").arg(XATTR_EQ_DIRECTION,  _direction);

    QString oEquation = QString("%1{%2,%3,%4,%5,%6,'Solid':'%7','Continuous':'%8'}").arg(EQ_TRIANGLE,sVarVal1,sVarVal2,sVarVal3,sVarVal4,sVarVal5,getSolidState(),getContinousState());
    return oEquation;
}
GTAEquationBase* GTAEquationTriangle::getClone()const
{
    return new GTAEquationTriangle(*this);
}

bool GTAEquationTriangle::operator ==(GTAEquationBase*& pObj) const
{
    GTAEquationTriangle*pRcvdObj = dynamic_cast<GTAEquationTriangle*>(pObj);
    if (pRcvdObj != nullptr)
        return ((this->getPeriod() == pRcvdObj->getPeriod()) &&
                (this->getMinValue() == pRcvdObj->getMinValue()) &&
                (this->getMaxValue() == pRcvdObj->getMaxValue()) &&
                (this->getStartValue() == pRcvdObj->getStartValue()) &&
                (this->getDirection() == pRcvdObj->getDirection()) &&
                (this->getSolidState() == pRcvdObj->getSolidState()) &&
                (this->getContinousState() == pRcvdObj->getContinousState()));
    else
        return false;
}
void  GTAEquationTriangle::getFunctionStatement(QString& osFuncType,QString& osFunctionStatement)const
{
    osFuncType = QString(EQ_TRIANGLE).toUpper();

    QString sVarVal1 = QString("'%1':'%2'").arg(XATTR_EQ_PERIOD,     _Period);
    QString sVarVal2 = QString("'%1':'%2'").arg(XATTR_EQ_MAXVALUE,   _maxValue);
    QString sVarVal3 = QString("'%1':'%2'").arg(XATTR_EQ_MINVALUE,   _minValue);
    QString sVarVal4 = QString("'%1':'%2'").arg(XATTR_EQ_STARTVALUE, _startValue);
    QString sVarVal5 = QString("'%1':'%2'").arg(XATTR_EQ_DIRECTION,  _direction);

    osFunctionStatement = QString("%1{%2,%3,%4,%5,%6,'Solid':'%7','Continuous':'%8'}").arg("'FunctionValue':",sVarVal1,sVarVal2,sVarVal3,sVarVal4,sVarVal5,getSolidState(),getContinousState());
}

void  GTAEquationTriangle::getEquationArguments(QHash<QString,QString>& iSerializationMap)const
{
    iSerializationMap.insert(QString(XATTR_EQ_PERIOD),     _Period);
    iSerializationMap.insert(QString(XATTR_EQ_MAXVALUE),   _maxValue);
    iSerializationMap.insert(QString(XATTR_EQ_MINVALUE),   _minValue);
    iSerializationMap.insert(QString(XATTR_EQ_STARTVALUE), _startValue);
    iSerializationMap.insert(QString(XATTR_EQ_DIRECTION),  _direction);
}
void  GTAEquationTriangle::setEquationArguments(const QHash<QString,QString>& iSerializationMap)
{
    if (iSerializationMap.contains(QString(XATTR_EQ_PERIOD)))
        _Period = iSerializationMap.value(QString(XATTR_EQ_PERIOD));

    if (iSerializationMap.contains(QString(XATTR_EQ_MAXVALUE)))
        _maxValue = iSerializationMap.value(QString(XATTR_EQ_MAXVALUE));

    if (iSerializationMap.contains(QString(XATTR_EQ_MINVALUE)))
        _minValue = iSerializationMap.value(QString(XATTR_EQ_MINVALUE));

    if (iSerializationMap.contains(QString(XATTR_EQ_STARTVALUE)))
        _startValue = iSerializationMap.value(QString(XATTR_EQ_STARTVALUE));

    if (iSerializationMap.contains(QString(XATTR_EQ_DIRECTION)))
        _direction = iSerializationMap.value(QString(XATTR_EQ_DIRECTION));
}

QStringList GTAEquationTriangle::getVariableList() const
{
    QStringList varList;

    if (GTACommandBase::isVariable(_Period))
        varList << _Period;
    if (GTACommandBase::isVariable(_maxValue))
        varList << _maxValue;
    if (GTACommandBase::isVariable(_minValue))
        varList << _minValue;
    if (GTACommandBase::isVariable(_startValue))
        varList << _startValue;
    if (GTACommandBase::isVariable(_direction))
        varList << _direction;

    return varList;
}
void GTAEquationTriangle:: ReplaceTag(const QMap<QString,QString>& iTagValueMap)
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
        if (GTACommandBase::isVariable(_startValue))
            _startValue.replace(tag, iterTag.value());
        if (GTACommandBase::isVariable(_direction))
            _direction.replace(tag, iterTag.value());
    }
}
