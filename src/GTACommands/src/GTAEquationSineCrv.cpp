#include "GTAEquationSineCrv.h"
#include "GTAUtil.h"

GTAEquationSineCrv::GTAEquationSineCrv():GTAEquationBase(GTAEquationBase::SINECRV)
{
}
GTAEquationSineCrv::GTAEquationSineCrv(const GTAEquationSineCrv& rhs):GTAEquationBase(GTAEquationBase::SINECRV)
{
    _Gain = rhs.getGain();
    _TrigonometryOper = rhs.getTrignometryOperator();
    _Pulsation = rhs.getPulsation();
    _Phase = rhs.getPhase();
    _Offset = rhs.getOffset();
    setSolidState(rhs.getSolidState());
    setContinousState(rhs.getContinousState());
}
GTAEquationSineCrv::~GTAEquationSineCrv()
{

}

// SET Functions
void GTAEquationSineCrv::setGain(const QString &iVal)
{
    _Gain = iVal;
}
void GTAEquationSineCrv::setTrignometryOperator(const QString &iVal)
{
    _TrigonometryOper = iVal;
}
void GTAEquationSineCrv::setPulsation(const QString &iVal)
{
    _Pulsation = iVal;
}
void GTAEquationSineCrv::setPhase(const QString &iVal)
{
    _Phase = iVal;
}
void GTAEquationSineCrv::setOffset(const QString &iVal)
{
    _Offset =iVal;
}

// GET Functions
QString GTAEquationSineCrv::getGain() const
{
    return _Gain;
}
QString GTAEquationSineCrv::getTrignometryOperator() const
{
    return _TrigonometryOper;
}
QString GTAEquationSineCrv::getPulsation() const
{
    return _Pulsation;
}
QString GTAEquationSineCrv::getPhase() const
{
    return _Phase;
}
QString GTAEquationSineCrv::getOffset() const
{
    return _Offset;
}

QString GTAEquationSineCrv::getStatement() const
{
    QString sVarVal1 = QString("'%1':'%2'").arg(XATTR_EQ_PERIOD,  _Pulsation);
    QString sVarVal2 = QString("'%1':'%2'").arg(XATTR_EQ_OFFSET,  _Offset);
    QString sVarVal3 = QString("'%1':'%2'").arg(XATTR_EQ_AMP,     _Gain);
    QString sVarVal4 = QString("'%1':'%2'").arg(XATTR_EQ_PHASE,   _Phase);
    QString sVarVal5 = QString("'%1':'%2'").arg(XATTR_EQ_TRIG_OP, _TrigonometryOper);

    QString oEquation = QString("%1{%2,%3,%4,%5,%6,'Solid':'%7','Continuous':'%8'}").arg(EQ_SINECRV,sVarVal1,sVarVal2,sVarVal3,sVarVal4,sVarVal5,getSolidState(),getContinousState());
    return oEquation;
}
GTAEquationBase* GTAEquationSineCrv:: getClone()const
{
    return new GTAEquationSineCrv(*this);
}

bool GTAEquationSineCrv::operator ==(GTAEquationBase*& pObj) const
{
    GTAEquationSineCrv *pRcvdObj = dynamic_cast<GTAEquationSineCrv *>(pObj);
    if (pRcvdObj != nullptr)
        return ((this->getGain() == pRcvdObj->getGain()) &&
                (this->getTrignometryOperator() == pRcvdObj->getTrignometryOperator()) &&
                (this->getPulsation() == pRcvdObj->getPulsation()) &&
                (this->getPhase() == pRcvdObj->getPhase()) &&
                (this->getOffset() == pRcvdObj->getOffset()) &&
                (this->getSolidState() == pRcvdObj->getSolidState()) &&
                (this->getContinousState() == pRcvdObj->getContinousState()));
    else
        return false;
}

void  GTAEquationSineCrv::getFunctionStatement(QString& osFuncType,QString& osFunctionStatement)const
{
    osFuncType = QString(EQ_SINECRV).toUpper();

    QString sVarVal1 = QString("'%1':'%2'").arg(XATTR_EQ_PERIOD,  _Pulsation);
    QString sVarVal2 = QString("'%1':'%2'").arg(XATTR_EQ_OFFSET,  _Offset);
    QString sVarVal3 = QString("'%1':'%2'").arg(XATTR_EQ_AMP,     _Gain);
    QString sVarVal4 = QString("'%1':'%2'").arg(XATTR_EQ_PHASE,   _Phase);
    QString sVarVal5 = QString("'%1':'%2'").arg(XATTR_EQ_TRIG_OP, _TrigonometryOper);

    osFunctionStatement = QString("%1{%2,%3,%4,%5,%6,'Solid':'%7','Continuous':'%8'}").arg("'FunctionValue':",sVarVal1,sVarVal2,sVarVal3,sVarVal4,sVarVal5,getSolidState(),getContinousState());
}

void  GTAEquationSineCrv::getEquationArguments(QHash<QString,QString>& iSerializationMap)const
{
    iSerializationMap.insert(XATTR_EQ_AMP,    _Gain);
    iSerializationMap.insert(XATTR_EQ_OFFSET, _Offset);
    iSerializationMap.insert(XATTR_EQ_PHASE,  _Phase);
    iSerializationMap.insert(XATTR_EQ_PERIOD, _Pulsation);
    iSerializationMap.insert(XATTR_EQ_TRIG_OP,_TrigonometryOper);
}
void  GTAEquationSineCrv::setEquationArguments(const QHash<QString,QString>& iSerializationMap)
{
    if (iSerializationMap.contains(XATTR_EQ_AMP))
        _Gain = iSerializationMap.value(XATTR_EQ_AMP);

    if (iSerializationMap.contains(XATTR_EQ_OFFSET))
        _Offset = iSerializationMap.value(XATTR_EQ_OFFSET);

    if (iSerializationMap.contains(XATTR_EQ_PHASE))
        _Phase = iSerializationMap.value(XATTR_EQ_PHASE);

    if (iSerializationMap.contains(XATTR_EQ_PERIOD))
        _Pulsation = iSerializationMap.value(XATTR_EQ_PERIOD);

    if (iSerializationMap.contains(XATTR_EQ_TRIG_OP))
        _TrigonometryOper = iSerializationMap.value(XATTR_EQ_TRIG_OP);
}

QStringList GTAEquationSineCrv::getVariableList() const
{
    QStringList varList;

    if (GTACommandBase::isVariable(_Gain))
        varList << _Gain;
    if (GTACommandBase::isVariable(_TrigonometryOper))
        varList << _TrigonometryOper;
    if (GTACommandBase::isVariable(_Offset))
        varList << _Offset;
    if (GTACommandBase::isVariable(_Phase))
        varList << _Phase;
    if (GTACommandBase::isVariable(_Pulsation))
        varList << _Pulsation;

    return varList;
}

void GTAEquationSineCrv:: ReplaceTag(const QMap<QString,QString>& iTagValueMap)
{
    QMapIterator<QString,QString> iterTag(iTagValueMap);
    while (iterTag.hasNext())
    {
        iterTag.next();
        QString tag = QString("%1%2%3").arg(QChar(TAG_IDENTIFIER),iterTag.key(),QChar(TAG_IDENTIFIER));

        if (GTACommandBase::isVariable(_Gain))
            _Gain.replace(tag, iterTag.value());
        if (GTACommandBase::isVariable(_TrigonometryOper))
            _TrigonometryOper.replace(tag, iterTag.value());
        if (GTACommandBase::isVariable(_Offset))
            _Offset.replace(tag, iterTag.value());
        if (GTACommandBase::isVariable(_Phase))
            _Phase.replace(tag, iterTag.value());
        if (GTACommandBase::isVariable(_Pulsation))
            _Pulsation.replace(tag, iterTag.value());
    }
}
