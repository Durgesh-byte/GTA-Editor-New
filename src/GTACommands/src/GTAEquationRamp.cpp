#include "GTAEquationRamp.h"
#include "GTAUtil.h"

GTAEquationRamp::GTAEquationRamp():GTAEquationBase(GTAEquationBase::RAMP)
{
    _isEndValueOffset = false;
}
GTAEquationRamp::GTAEquationRamp(const GTAEquationRamp& rhs):GTAEquationBase(GTAEquationBase::RAMP)
{
    _RiseTime = rhs.getRiseTime();
    _Start = rhs.getStart();
    _End = rhs.getEnd();
    this->setSolidState(rhs.getSolidState());
    this->setContinousState(rhs.getContinousState());
    _isEndValueOffset = rhs.isEndValueOffset();
}
GTAEquationRamp::~GTAEquationRamp()
{

}
void GTAEquationRamp::setRiseTime(const QString &iVal)
{
    _RiseTime = iVal;
}

void GTAEquationRamp::setStart(const QString &iVal)
{
    _Start =iVal;
}

void GTAEquationRamp::setEnd(const QString &iVal)
{
    _End = iVal;
}
QString GTAEquationRamp::getRiseTime() const
{
    return _RiseTime;
}

QString GTAEquationRamp::getStart() const
{
    return _Start;
}
QString GTAEquationRamp::getEnd() const
{
    return _End;
}


void GTAEquationRamp::setEndValueOffset(bool iVal)
{
    _isEndValueOffset = iVal;
}

bool GTAEquationRamp::isEndValueOffset()const
{
    return _isEndValueOffset;
}

QString GTAEquationRamp::getStatement() const
{
    QString arg = XATTR_EQ_END;
    QString value = _End;
    if (isEndValueOffset())
    {
        arg = "offset";
        value = _End.split("+").last();
    }
    QString oEquation=  QString("%1{'%2':'%3', '%4':'%5','%6':'%7','Solid':'%8','Continuous':'%9'}").arg(  EQ_RAMP,
                                                                XATTR_EQ_RISETIME,_RiseTime,
                                                                XATTR_EQ_START,_Start,
                                                                arg,value,getSolidState(),getContinousState());
    return oEquation;
}
GTAEquationBase* GTAEquationRamp:: getClone()const
{
    return new GTAEquationRamp(*this);
}
void  GTAEquationRamp::getFunctionStatement(QString& osFuncType,QString& osFunctionStatement)const
{
    osFuncType=QString(EQ_RAMP).toUpper();
     QString riseTime = QString::number(_RiseTime.toDouble()*1000);
     QString start = _Start;
     QString end = _End;
     if(_isEndValueOffset)
     {
         start = QString("");
         end = QString("");
     }
    osFunctionStatement=QString("%1{'%2':'%3', '%4':'%5','%6':'%7','Solid':'%8','Continuous':'%9'}").arg("'FunctionValue':",
                                                                            XATTR_EQ_RISETIME,riseTime,
                                                                            "StartValue",start,
                                                                            "EndValue",end,getSolidState(),getContinousState());
   
}
void  GTAEquationRamp::getEquationArguments(QHash<QString,QString>& iSerialzationMap)const
{
    iSerialzationMap.insert(XATTR_EQ_RISETIME,_RiseTime);
    iSerialzationMap.insert(XATTR_EQ_START,_Start);
    iSerialzationMap.insert(XATTR_EQ_END,_End);
    QString sIsOffset = _isEndValueOffset ? XNODE_TRUE : XNODE_FALSE;
    iSerialzationMap.insert(XATTR_EQ_IS_OFFSET,sIsOffset);
    
}
void  GTAEquationRamp::setEquationArguments(const QHash<QString,QString>& iSerialzationMap)
{
    if (iSerialzationMap.contains(XATTR_EQ_RISETIME))
        _RiseTime=iSerialzationMap.value(XATTR_EQ_RISETIME);
    
    if (iSerialzationMap.contains(XATTR_EQ_START))
        _Start=iSerialzationMap.value(XATTR_EQ_START);
    
    if (iSerialzationMap.contains(XATTR_EQ_END))
        _End=iSerialzationMap.value(XATTR_EQ_END);

    if(iSerialzationMap.contains(XATTR_EQ_IS_OFFSET))
    {
        QString value = iSerialzationMap.value(XATTR_EQ_IS_OFFSET);
        _isEndValueOffset = ((value == XNODE_TRUE) ? true : false );
    }
}

QStringList GTAEquationRamp::getVariableList() const
{
    QStringList varList;

    if (GTACommandBase::isVariable(_Start))
        varList<<_Start;
    if (GTACommandBase::isVariable(_RiseTime))
        varList<<_RiseTime;

    if(!_isEndValueOffset)
    {
        if (GTACommandBase::isVariable(_End))
            varList<<_End;
    }
    else
    {
        QString offset = _End.split("+").last();
        if (GTACommandBase::isVariable(offset.trimmed()))
            varList<<offset;
    }
    

    return varList;
}
void GTAEquationRamp:: ReplaceTag(const QMap<QString,QString>& iTagValueMap) 
{
    QMapIterator<QString,QString> iterTag(iTagValueMap);
    while (iterTag.hasNext())
    {
        iterTag.next();
        QString tag = QString("%1%2%3").arg(QChar(TAG_IDENTIFIER),iterTag.key(),QChar(TAG_IDENTIFIER));

        if (GTACommandBase::isVariable(_Start))
            _Start.replace(tag,iterTag.value());
        if (GTACommandBase::isVariable(_RiseTime))
            _RiseTime.replace(tag,iterTag.value());
        if (GTACommandBase::isVariable(_End))
            _End.replace(tag,iterTag.value());
       

    }
}

bool GTAEquationRamp::operator ==(GTAEquationBase*& pObj) const
{
    GTAEquationRamp *pRcvdObj = dynamic_cast<GTAEquationRamp *>(pObj);
    if (pRcvdObj != nullptr)
        return ((this->getRiseTime() == pRcvdObj->getRiseTime()) && (this->getStart() == pRcvdObj->getStart()) &&
                (this->getEnd() == pRcvdObj->getEnd()) && (this->isEndValueOffset() == pRcvdObj->isEndValueOffset()) &&
                (this->getSolidState() == pRcvdObj->getSolidState()) && 
				(this->getContinousState() == pRcvdObj->getContinousState()));
    else
        return false;
}
