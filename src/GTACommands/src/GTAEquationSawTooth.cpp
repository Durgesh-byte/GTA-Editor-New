#include "GTAEquationSawTooth.h"
#include "GTAUtil.h"


GTAEquationSawTooth::GTAEquationSawTooth():GTAEquationBase(GTAEquationBase::SAWTOOTH)
{
}
GTAEquationSawTooth::GTAEquationSawTooth(const GTAEquationSawTooth& rhs):GTAEquationBase(GTAEquationBase::SAWTOOTH)
{
    _Period=rhs.getPeriod();
    _maxValue=rhs.getMaxValue();
    _minValue=rhs.getMinValue();
    _startValue=rhs.getStartValue();
    _rampMode=rhs.getRampMode();
    this->setSolidState(rhs.getSolidState());
    this->setContinousState(rhs.getContinousState());
}
GTAEquationSawTooth::~GTAEquationSawTooth()
{

}

// SET Function
void GTAEquationSawTooth::setPeriod(const QString &iVal)
{
    _Period=iVal;
}
void GTAEquationSawTooth::setMaxValue(const QString &iVal)
{
    _maxValue=iVal;
}
void GTAEquationSawTooth::setMinValue(const QString &iVal)
{
    _minValue=iVal;
}
void GTAEquationSawTooth::setStartValue(const QString &iVal)
{
    _startValue=iVal;
}
void GTAEquationSawTooth::setRampMode(const QString &iVal)
{
    _rampMode=iVal;;
}

// GET Functions
QString GTAEquationSawTooth::getPeriod() const
{
    return _Period;
}
QString GTAEquationSawTooth::getMaxValue() const
{
    return _maxValue;
}
QString GTAEquationSawTooth::getMinValue() const
{
    return _minValue;
}
QString GTAEquationSawTooth::getStartValue() const
{
    return _startValue;
}
QString GTAEquationSawTooth::getRampMode() const
{
    return _rampMode;
}

QString GTAEquationSawTooth::getStatement() const
{
    QString sVarVal1 = QString("'%1':'%2'").arg(XATTR_EQ_PERIOD,     _Period);
    QString sVarVal2 = QString("'%1':'%2'").arg(XATTR_EQ_MAXVALUE,   _maxValue);
    QString sVarVal3 = QString("'%1':'%2'").arg(XATTR_EQ_MINVALUE,   _minValue);
    QString sVarVal4 = QString("'%1':'%2'").arg(XATTR_EQ_STARTVALUE, _startValue);
    QString sVarVal5 = QString("'%1':'%2'").arg(XATTR_EQ_RAMPMODE,   _rampMode);
    
    QString oEquation = QString("%1{%2,%3,%4,%5,%6,'Solid':'%7','Continuous':'%8'}").arg(EQ_SAWTOOTH,sVarVal1,sVarVal2,sVarVal3,sVarVal4,sVarVal5,getSolidState(),getContinousState());
    return oEquation;
}
GTAEquationBase* GTAEquationSawTooth:: getClone()const
{
    return new GTAEquationSawTooth(*this);
}

bool GTAEquationSawTooth::operator ==(GTAEquationBase*& pObj) const
{
    GTAEquationSawTooth *pRcvdObj = dynamic_cast<GTAEquationSawTooth *>(pObj);
    if (pRcvdObj != nullptr)
        return ((this->getPeriod() == pRcvdObj->getPeriod()) &&
                (this->getStartValue() == pRcvdObj->getStartValue()) &&
                (this->getMinValue() == pRcvdObj->getMinValue()) &&
                (this->getMaxValue() == pRcvdObj->getMaxValue()) &&
                (this->getRampMode() == pRcvdObj->getRampMode()) &&
                (this->getSolidState() == pRcvdObj->getSolidState()) && 
				(this->getContinousState() == pRcvdObj->getContinousState()));
    else
        return false;
}
void  GTAEquationSawTooth::getFunctionStatement(QString& osFuncType, QString& osFunctionStatement)const
{
    osFuncType= QString(EQ_SAWTOOTH).toUpper();
    
    QString sVarVal1 = QString("'%1':'%2'").arg(XATTR_EQ_PERIOD,     _Period);
    QString sVarVal2 = QString("'%1':'%2'").arg(XATTR_EQ_MAXVALUE,   _maxValue);
    QString sVarVal3 = QString("'%1':'%2'").arg(XATTR_EQ_MINVALUE,   _minValue);
    QString sVarVal4 = QString("'%1':'%2'").arg(XATTR_EQ_STARTVALUE, _startValue);
    QString sVarVal5 = QString("'%1':'%2'").arg(XATTR_EQ_RAMPMODE,   _rampMode);

    osFunctionStatement = QString("%1{%2,%3,%4,%5,%6,'Solid':'%7','Continuous':'%8'}").arg("'FunctionValue':",sVarVal1,sVarVal2,sVarVal3,sVarVal4,sVarVal5,getSolidState(),getContinousState());
}
void  GTAEquationSawTooth::getEquationArguments(QHash<QString,QString>& iSerialzationMap)const
{
     iSerialzationMap.insert(XATTR_EQ_PERIOD,     _Period);
     iSerialzationMap.insert(XATTR_EQ_MAXVALUE,   _maxValue);
     iSerialzationMap.insert(XATTR_EQ_MINVALUE,   _minValue);
     iSerialzationMap.insert(XATTR_EQ_STARTVALUE, _startValue);
     iSerialzationMap.insert(XATTR_EQ_RAMPMODE,   _rampMode);
}
void  GTAEquationSawTooth::setEquationArguments(const QHash<QString,QString>& iSerialzationMap)
{
     if (iSerialzationMap.contains(XATTR_EQ_RAMPMODE))
          _rampMode = iSerialzationMap.value(XATTR_EQ_RAMPMODE);

     if (iSerialzationMap.contains(XATTR_EQ_PERIOD))
         _Period = iSerialzationMap.value(XATTR_EQ_PERIOD);

     if (iSerialzationMap.contains(XATTR_EQ_MAXVALUE))
         _maxValue = iSerialzationMap.value(XATTR_EQ_MAXVALUE);

     if (iSerialzationMap.contains(XATTR_EQ_MINVALUE))
         _minValue = iSerialzationMap.value(XATTR_EQ_MINVALUE);

     if (iSerialzationMap.contains(XATTR_EQ_STARTVALUE))
         _startValue = iSerialzationMap.value(XATTR_EQ_STARTVALUE);
}

QStringList GTAEquationSawTooth::getVariableList() const
{
    QStringList varList;

    if (GTACommandBase::isVariable(_Period))
        varList<<_Period;
    if (GTACommandBase::isVariable(_maxValue))
        varList<<_maxValue;
    if (GTACommandBase::isVariable(_minValue))
        varList<<_minValue;
    if (GTACommandBase::isVariable(_startValue))
        varList<<_startValue;

    return varList;
}

void GTAEquationSawTooth:: ReplaceTag(const QMap<QString,QString>& iTagValueMap) 
{
    QMapIterator<QString,QString> iterTag(iTagValueMap);
    while (iterTag.hasNext())
    {
        iterTag.next();
        QString tag = QString("%1%2%3").arg(QChar(TAG_IDENTIFIER),iterTag.key(),QChar(TAG_IDENTIFIER));

        if (GTACommandBase::isVariable(_Period))
            _Period.replace(tag,iterTag.value());
        if (GTACommandBase::isVariable(_maxValue))
            _maxValue.replace(tag,iterTag.value());
        if (GTACommandBase::isVariable(_minValue))
            _minValue.replace(tag,iterTag.value());
        if (GTACommandBase::isVariable(_startValue))
            _startValue.replace(tag,iterTag.value());
    }
}
