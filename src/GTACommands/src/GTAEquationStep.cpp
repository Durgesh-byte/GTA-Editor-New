#include "GTAEquationStep.h"
#include "GTAUtil.h"

GTAEquationStep::GTAEquationStep():GTAEquationBase(GTAEquationBase::STEP)
{
}
GTAEquationStep::GTAEquationStep(const GTAEquationStep& rhs):GTAEquationBase(GTAEquationBase::STEP)
{
    _Period = rhs.getPeriod();
    _startValue = rhs.getStartValue();
    _endValue = rhs.getEndValue();
    this->setSolidState(rhs.getSolidState());
    this->setContinousState(rhs.getContinousState());
	 
}
GTAEquationStep::~GTAEquationStep()
{

}

// SET Functions
void GTAEquationStep::setPeriod(const QString &iVal)
{
    _Period = iVal;
}
void GTAEquationStep::setStartValue(const QString &iVal)
{
    _startValue = iVal;
}
void GTAEquationStep::setEndValue(const QString &iVal)
{
    _endValue = iVal;
}

// GET Functions
QString GTAEquationStep::getPeriod() const
{
    return _Period;
}
QString GTAEquationStep::getStartValue() const
{
    return _startValue;
}
QString GTAEquationStep::getEndValue() const
{
    return _endValue;
}

QString GTAEquationStep::getStatement() const
{
    QString sVarVal1 = QString("'%1':'%2'").arg(XATTR_EQ_PERIOD,     _Period);
    QString sVarVal2 = QString("'%1':'%2'").arg(XATTR_EQ_STARTVALUE, _startValue);
    QString sVarVal3 = QString("'%1':'%2'").arg(XATTR_EQ_ENDVALUE,   _endValue);

    QString oEquation = QString("%1{%2,%3,%4,'Solid':'%5','Continuous':'%6'}").arg(EQ_STEP,sVarVal1,sVarVal2,sVarVal3,getSolidState(),getContinousState());
    return oEquation;
}

GTAEquationBase* GTAEquationStep::getClone()const
{
    return new GTAEquationStep(*this);
}

bool GTAEquationStep::operator ==(GTAEquationBase*& pObj) const
{
    GTAEquationStep *pRcvdObj = dynamic_cast<GTAEquationStep *>(pObj);
    if (pRcvdObj != nullptr)
        return ((this->getPeriod() == pRcvdObj->getPeriod()) &&
                (this->getStartValue() == pRcvdObj->getStartValue()) &&
                (this->getEndValue() == pRcvdObj->getEndValue()) &&
                (this->getSolidState() == pRcvdObj->getSolidState()) &&
                (this->getContinousState() == pRcvdObj->getContinousState()));
    else
        return false;
}

void  GTAEquationStep::getFunctionStatement(QString& osFuncType,QString& osFunctionStatement)const
{
    osFuncType = QString(EQ_STEP).toUpper();

    QString sVarVal1 = QString("'%1':'%2'").arg(XATTR_EQ_PERIOD,     _Period);
    QString sVarVal2 = QString("'%1':'%2'").arg(XATTR_EQ_STARTVALUE, _startValue);
    QString sVarVal3 = QString("'%1':'%2'").arg(XATTR_EQ_ENDVALUE,   _endValue);

    osFunctionStatement = QString("%1{%2,%3,%4,'Solid':'%5','Continuous':'%6'}").arg("'FunctionValue':",sVarVal1,sVarVal2,sVarVal3,getSolidState(),getContinousState());
}

void GTAEquationStep::getEquationArguments(QHash<QString,QString>& iSerializationMap) const
{
    iSerializationMap.insert(QString(XATTR_EQ_PERIOD), _Period);
    iSerializationMap.insert(QString(XATTR_EQ_STARTVALUE), _startValue);
    iSerializationMap.insert(QString(XATTR_EQ_ENDVALUE), _endValue);
}

void GTAEquationStep::setEquationArguments(const QHash<QString,QString>& iSerializationMap)
 {
     if (iSerializationMap.contains(XATTR_EQ_PERIOD))
        _Period = iSerializationMap.value(XATTR_EQ_PERIOD);

     if (iSerializationMap.contains(XATTR_EQ_STARTVALUE))
        _startValue = iSerializationMap.value(XATTR_EQ_STARTVALUE);

     if (iSerializationMap.contains(XATTR_EQ_ENDVALUE))
        _endValue = iSerializationMap.value(XATTR_EQ_ENDVALUE);
 }

QStringList GTAEquationStep::getVariableList() const
{
    QStringList varList;

    if (GTACommandBase::isVariable(_Period))
        varList << _Period;
    if (GTACommandBase::isVariable(_startValue))
        varList << _startValue;
    if (GTACommandBase::isVariable(_endValue))
        varList << _endValue;

    return varList;
}

void GTAEquationStep:: ReplaceTag(const QMap<QString,QString>& iTagValueMap)
{
    QMapIterator<QString,QString> iterTag(iTagValueMap);
    while (iterTag.hasNext())
    {
        iterTag.next();
        QString tag = QString("%1%2%3").arg(QChar(TAG_IDENTIFIER),iterTag.key(),QChar(TAG_IDENTIFIER));

        if (GTACommandBase::isVariable(_Period))
            _Period.replace(tag, iterTag.value());
        if (GTACommandBase::isVariable(_startValue))
            _startValue.replace(tag, iterTag.value());
        if (GTACommandBase::isVariable(_endValue))
            _endValue.replace(tag, iterTag.value());
    }
}
