#include "GTAFailProfileSine.h"
#include "GTAUtil.h"


GTAFailProfileSine::GTAFailProfileSine():GTAFailureProfileBase(GTAFailureProfileBase::SINUS)
{
}
GTAFailProfileSine::GTAFailProfileSine(const GTAFailProfileSine& rhs):GTAFailureProfileBase(GTAFailureProfileBase::SINUS)
{
    _Period=rhs.getPeriod();
    _Amplitude=rhs.getAmplitude();
    _Offset=rhs.getOffset();
    _Phase=rhs.getPhase();

}
GTAFailProfileSine::~GTAFailProfileSine()
{

}
void GTAFailProfileSine::setPeriod(const QString &iVal)
{
    _Period=iVal;
}
void GTAFailProfileSine::setAmplitude(const QString &iVal)
{
    _Amplitude=iVal;
}
void GTAFailProfileSine::setOffset(const QString &iVal)
{
    _Offset=iVal;
}
void GTAFailProfileSine::setPhase(const QString &iVal)
{
    _Phase=iVal;
}

QString GTAFailProfileSine::getPeriod() const
{
    return _Period;
}
QString GTAFailProfileSine::getAmplitude() const
{
    return _Amplitude;
}
QString GTAFailProfileSine::getOffset() const
{
    return _Offset;
}
QString GTAFailProfileSine::getPhase() const
{
    return _Phase;
} 

QString GTAFailProfileSine::getStatement() const
{
    QString oEquation=QString("%1(%2='%3',%4='%5',%6='%7',%8='%9')").arg(EQ_SINUS,
                                                                        XATTR_EQ_AMP,_Amplitude,
                                                                        XATTR_EQ_PERIOD,_Period,
                                                                        XATTR_EQ_OFFSET,_Offset,
                                                                        XATTR_EQ_PHASE,_Phase);
    return oEquation;
}
GTAFailureProfileBase* GTAFailProfileSine:: getClone()const
{
    return new GTAFailProfileSine(*this);
}
void  GTAFailProfileSine::getFunctionStatement(QString&, QString& osFunctionStatement) const
{
    //osFuncType=QString(EQ_SINUS).toUpper();
    //osFunctionStatement = QString("%1{'%2':'%3','%4':'%5','%6':'%7','%8':'%9'}").arg(   "'FunctionValue':",
    //                                                                                    XATTR_EQ_AMP,_Amplitude,
    //                                                                                    XATTR_EQ_PERIOD,_Period,
    //                                                                                    XATTR_EQ_OFFSET,_Offset,
    //                                                                                    XATTR_EQ_PHASE,_Phase);

    osFunctionStatement=QString("'type':'SINE','mode':2,'amplitude':%1,'period':%2,'offset':%3,'phase':%4").arg(_Amplitude,_Period,_Offset,_Phase);
    
}
void  GTAFailProfileSine::getFailureProfileArguments(QHash<QString,QString>& iSerialzationMap)const
{
    iSerialzationMap.insert(XATTR_EQ_AMP,_Amplitude);
    iSerialzationMap.insert(XATTR_EQ_OFFSET,_Offset);
    iSerialzationMap.insert(XATTR_EQ_PHASE,_Phase);
    iSerialzationMap.insert(XATTR_EQ_PERIOD,_Period);
}
void  GTAFailProfileSine::setFailureProfileArguments(const QHash<QString,QString>& iSerialzationMap)
{
    if (iSerialzationMap.contains(XATTR_EQ_AMP))
        _Amplitude=iSerialzationMap.value(XATTR_EQ_AMP);

    if (iSerialzationMap.contains(XATTR_EQ_OFFSET))
        _Offset=iSerialzationMap.value(XATTR_EQ_OFFSET);

    if (iSerialzationMap.contains(XATTR_EQ_PHASE))
        _Phase=iSerialzationMap.value(XATTR_EQ_PHASE);

    if (iSerialzationMap.contains(XATTR_EQ_PERIOD))
        _Period=iSerialzationMap.value(XATTR_EQ_PERIOD);
}