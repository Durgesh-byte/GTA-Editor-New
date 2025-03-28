#include "GTAEquationSinus.h"
#include "GTAUtil.h"


GTAEquationSinus::GTAEquationSinus():GTAEquationBase(GTAEquationBase::SINUS)
{
}
GTAEquationSinus::GTAEquationSinus(const GTAEquationSinus& rhs):GTAEquationBase(GTAEquationBase::SINUS)
{
	_isMinMax = rhs.getIsMinMax();
	_isPeriod = rhs.getIsPeriod();
	_isStartDir = rhs.getIsStartDir();
    _period = rhs.getPeriod();
	_frequency = rhs.getFrequency();
    _maxValue = rhs.getMaxValue();
    _minValue = rhs.getMinValue();
	_amplitude = rhs.getAmplitude();
	_offset = rhs.getOffset();
	_startValue = rhs.getStartValue();
	_direction = rhs.getDirection();
	_phase = rhs.getPhase();
    
    this->setSolidState(rhs.getSolidState());
    this->setContinousState(rhs.getContinousState());
}
GTAEquationSinus::~GTAEquationSinus()
{

}

// SET Functions
void GTAEquationSinus::setPeriod(const QString &iVal)
{
	_isPeriod = true;
    _period = iVal;
}
void GTAEquationSinus::setMaxValue(const QString &iVal)
{
	_isMinMax = true;
    _maxValue = iVal;
}
void GTAEquationSinus::setMinValue(const QString &iVal)
{
	_isMinMax = true;
    _minValue = iVal;
}
void GTAEquationSinus::setStartValue(const QString &iVal)
{
	_isStartDir = true;
    _startValue = iVal;
}
void GTAEquationSinus::setDirection(const QString &iVal)
{
	_isStartDir = true;
    _direction = iVal;
}
void GTAEquationSinus::setFrequency(const QString &iVal)
{
	_isPeriod = false;
	_frequency = iVal;
}
void GTAEquationSinus::setAmplitude(const QString &iVal) 
{
	_isMinMax = false;
	_amplitude = iVal;
}
void GTAEquationSinus::setOffset(const QString &iVal) 
{
	_isMinMax = false;
	_offset = iVal;
}
void GTAEquationSinus::setPhase(const QString &iVal) 
{
	_isStartDir = false;
	_phase = iVal;
}
void GTAEquationSinus::setIsDegrees(const bool isDegrees)
{
	_isDegrees = isDegrees;
}
// GET Functions
QString GTAEquationSinus::getPeriod() const
{
    return _period;
}
QString GTAEquationSinus::getMaxValue() const
{
    return _maxValue;
}
QString GTAEquationSinus::getMinValue() const
{
    return _minValue;
}
QString GTAEquationSinus::getStartValue() const
{
    return _startValue;
}
QString GTAEquationSinus::getDirection() const
{
    return _direction;
}
QString GTAEquationSinus::getFrequency() const 
{
	return _frequency;
}
QString GTAEquationSinus::getAmplitude() const
{
	return _amplitude;
}
QString GTAEquationSinus::getOffset() const
{
	return _offset;
}
QString GTAEquationSinus::getPhase() const
{
	return _phase;
}
bool GTAEquationSinus::getIsMinMax() const
{
	return _isMinMax;
}
bool GTAEquationSinus::getIsPeriod() const
{
	return _isPeriod;
}
bool GTAEquationSinus::getIsStartDir() const
{
	return _isStartDir;
}
bool GTAEquationSinus::getIsDegrees() const
{
	return _isDegrees;
}
QString GTAEquationSinus::getStatement() const
{
    QString sVarVal1 = QString("'%1':'%2'").arg(XATTR_EQ_PERIOD,     _period);
    QString sVarVal2 = QString("'%1':'%2'").arg(XATTR_EQ_MAXVALUE,   _maxValue);
    QString sVarVal3 = QString("'%1':'%2'").arg(XATTR_EQ_MINVALUE,   _minValue);
    QString sVarVal4 = QString("'%1':'%2'").arg(XATTR_EQ_STARTVALUE, _startValue);
    QString sVarVal5 = QString("'%1':'%2'").arg(XATTR_EQ_DIRECTION,  _direction);

    QString oEquation = QString("%1{%2,%3,%4,%5,%6,'Solid':'%7','Continuous':'%8'}").arg(EQ_SINUS,sVarVal1,sVarVal2,sVarVal3,sVarVal4,sVarVal5,getSolidState(),getContinousState());
    return oEquation;
}
GTAEquationBase* GTAEquationSinus:: getClone()const
{
    return new GTAEquationSinus(*this);
}

bool GTAEquationSinus::operator ==(GTAEquationBase*& pObj) const
{
    GTAEquationSinus *pRcvdObj = dynamic_cast<GTAEquationSinus *>(pObj);
    if (pRcvdObj != nullptr)
        return ((this->getPeriod() == pRcvdObj->getPeriod()) &&
                (this->getStartValue() == pRcvdObj->getStartValue()) &&
                (this->getMinValue() == pRcvdObj->getMinValue()) &&
                (this->getMaxValue() == pRcvdObj->getMaxValue()) &&
                (this->getDirection() == pRcvdObj->getDirection()) &&
                (this->getSolidState() == pRcvdObj->getSolidState()) &&
                (this->getContinousState() == pRcvdObj->getContinousState()));
    else
        return false;
}
void  GTAEquationSinus::getFunctionStatement(QString& osFuncType,QString& osFunctionStatement)const
{

    osFuncType = QString(EQ_SINUS).toUpper();
    QString sVarVal1 = QString("'%1':'%2'").arg(XATTR_EQ_PERIOD,     _period);
    QString sVarVal2 = QString("'%1':'%2'").arg(XATTR_EQ_MAXVALUE,   _maxValue);
    QString sVarVal3 = QString("'%1':'%2'").arg(XATTR_EQ_MINVALUE,   _minValue);
    QString sVarVal4 = QString("'%1':'%2'").arg(XATTR_EQ_STARTVALUE, _startValue);
    QString sVarVal5 = QString("'%1':'%2'").arg(XATTR_EQ_DIRECTION,  _direction);

    osFunctionStatement = QString("%1{%2,%3,%4,%5,%6,'Solid':'%7','Continuous':'%8'}").arg("'FunctionValue':",sVarVal1,sVarVal2,sVarVal3,sVarVal4,sVarVal5,getSolidState(),getContinousState());
}

void  GTAEquationSinus::getEquationArguments(QHash<QString,QString>& iSerialzationMap)const
{
	QString boolMode;
	if (_isMinMax)
		boolMode.append('1');
	else
		boolMode.append('0');
	if (_isPeriod)
		boolMode.append('1');
	else
		boolMode.append('0');
	if (_isStartDir)
		boolMode.append('1');
	else
		boolMode.append('0');
	if (_isDegrees)
		boolMode.append('1');
	else
		boolMode.append('0');
	iSerialzationMap.insert(XATTR_EQ_BOOLMODE, boolMode);
	if(_isPeriod)
		iSerialzationMap.insert(XATTR_EQ_PERIOD, _period);
	else
		iSerialzationMap.insert(XATTR_EQ_FREQUENCY, _frequency);

	if (_isMinMax) 
	{
		iSerialzationMap.insert(XATTR_EQ_MAXVALUE, _maxValue);
		iSerialzationMap.insert(XATTR_EQ_MINVALUE, _minValue);
	}
	else
	{
		iSerialzationMap.insert(XATTR_EQ_AMP, _amplitude);
		iSerialzationMap.insert(XATTR_EQ_OFFSET, _offset);
	}

	if (_isStartDir) 
	{
		iSerialzationMap.insert(XATTR_EQ_STARTVALUE, _startValue);
		iSerialzationMap.insert(XATTR_EQ_DIRECTION, _direction);
	}
	else
		iSerialzationMap.insert(XATTR_EQ_PHASE, _phase);
    
}
void GTAEquationSinus::setEquationArguments(const QHash<QString,QString>& iSerializationMap)
{
	if (iSerializationMap.contains(XATTR_EQ_BOOLMODE))
	{
		QString boolMode = iSerializationMap.value(XATTR_EQ_BOOLMODE);
		if (boolMode.length() == 4) 
		{
			_isMinMax = (boolMode.at(0) == '1');
			_isPeriod = (boolMode.at(1) == '1');
			_isStartDir = (boolMode.at(2) == '1');
			_isDegrees = (boolMode.at(3) == '1');
		}
	}
		
	
	if (iSerializationMap.contains(XATTR_EQ_DIRECTION))
		_direction = iSerializationMap.value(XATTR_EQ_DIRECTION);

    if (iSerializationMap.contains(XATTR_EQ_PERIOD))
        _period = iSerializationMap.value(XATTR_EQ_PERIOD);

    if (iSerializationMap.contains(XATTR_EQ_MAXVALUE))
        _maxValue = iSerializationMap.value(XATTR_EQ_MAXVALUE);

    if (iSerializationMap.contains(XATTR_EQ_MINVALUE))
        _minValue = iSerializationMap.value(XATTR_EQ_MINVALUE);

    if (iSerializationMap.contains(XATTR_EQ_STARTVALUE))
        _startValue = iSerializationMap.value(XATTR_EQ_STARTVALUE);

	if (iSerializationMap.contains(XATTR_EQ_FREQUENCY))
		_frequency = iSerializationMap.value(XATTR_EQ_FREQUENCY);

	if (iSerializationMap.contains(XATTR_EQ_AMP))
		_amplitude = iSerializationMap.value(XATTR_EQ_AMP);
	
	if (iSerializationMap.contains(XATTR_EQ_OFFSET))
		_offset = iSerializationMap.value(XATTR_EQ_OFFSET);

	if (iSerializationMap.contains(XATTR_EQ_PHASE))
		_phase = iSerializationMap.value(XATTR_EQ_PHASE);

	if (!_isMinMax)
		ampOffToMinMax();
	else
		minMaxToAmpOff();
	if (!_isPeriod)
		periodToFreq();
	else
		freqToPeriod();
	if (!_isStartDir)
		phaseToStartDir();
	else
		startDirToPhase();
}

QStringList GTAEquationSinus::getVariableList() const
{
    QStringList varList;

    if (GTACommandBase::isVariable(_period))
        varList << _period;
    if (GTACommandBase::isVariable(_maxValue))
        varList << _maxValue;
    if (GTACommandBase::isVariable(_minValue))
        varList << _minValue;
    if (GTACommandBase::isVariable(_startValue))
        varList << _startValue;

    return varList;
}

void GTAEquationSinus::phaseToStartDir()
{
	double phase = _phase.toDouble();
	if (_isDegrees)
	{
		phase = qDegreesToRadians(phase);
	}
	double startValue = (qSin(phase)*_amplitude.toDouble()) + _offset.toDouble();
	_startValue = QString::number(startValue);
	if ((-M_PI_2 <= phase) && (phase < M_PI_2))
	{
		_direction = "Rising Edge";
	}
	else
	{
		_direction = "Falling Edge";
	}
}
void GTAEquationSinus::startDirToPhase()
{
	double newPhase = qAsin((_startValue.toDouble() - _offset.toDouble()) / _amplitude.toDouble());
	if (_direction == "Falling Edge")
		newPhase = M_PI - newPhase;
	if (newPhase > M_PI)
	{
		newPhase = newPhase - 2 * M_PI;
	}
	else if (newPhase <= -M_PI)
	{
		newPhase = newPhase + 2 * M_PI;
	}
	if (_isDegrees) {
		newPhase = qRadiansToDegrees(newPhase);
	}
	_phase=QString::number(newPhase);
}
void GTAEquationSinus::ampOffToMinMax() 
{
	double newMaxValue = _amplitude.toDouble() + _offset.toDouble();
	double newMinValue = _offset.toDouble() - _amplitude.toDouble();
	_maxValue = QString::number(newMaxValue);
	_minValue = QString::number(newMinValue);
}
void GTAEquationSinus::minMaxToAmpOff()
{
	double newAmp = (_maxValue.toDouble() + _minValue.toDouble()) / 2;
	double newOff = (_maxValue.toDouble() - _minValue.toDouble()) / 2;
	_amplitude = QString::number(newAmp);
	_offset = QString::number(newOff);
}
void GTAEquationSinus::periodToFreq()
{
	double period = 1 / _frequency.toDouble();
	_period = QString::number(period);
}
void GTAEquationSinus::freqToPeriod()
{
	double freq = 1 / _period.toDouble();
	_frequency = QString::number(freq);
}
void GTAEquationSinus:: ReplaceTag(const QMap<QString,QString>& iTagValueMap)
{
    QMapIterator<QString,QString> iterTag(iTagValueMap);
    while (iterTag.hasNext())
    {
        iterTag.next();
        QString tag = QString("%1%2%3").arg(QChar(TAG_IDENTIFIER),iterTag.key(),QChar(TAG_IDENTIFIER));

        if (GTACommandBase::isVariable(_period))
            _period.replace(tag, iterTag.value());
        if (GTACommandBase::isVariable(_maxValue))
            _maxValue.replace(tag, iterTag.value());
        if (GTACommandBase::isVariable(_minValue))
            _minValue.replace(tag, iterTag.value());
        if (GTACommandBase::isVariable(_startValue))
            _startValue.replace(tag, iterTag.value());
    }
}
