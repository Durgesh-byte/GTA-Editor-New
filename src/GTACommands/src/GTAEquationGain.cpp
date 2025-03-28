#include "GTAEquationGain.h"
#include "GTAUtil.h"

GTAEquationGain::GTAEquationGain():GTAEquationBase(GTAEquationBase::GAIN)
{
}
GTAEquationGain::GTAEquationGain(const GTAEquationGain& rhs):GTAEquationBase(GTAEquationBase::GAIN)
{
    _GainValue=rhs.getGain();
    this->setSolidState(rhs.getSolidState());
    this->setContinousState(rhs.getContinousState());
}
GTAEquationGain::~GTAEquationGain()
{

}
void GTAEquationGain::setGain(const QString & iGain)
{
    _GainValue  = iGain;
}

QString GTAEquationGain::getGain() const
{
    return _GainValue;
}
QString GTAEquationGain::getStatement() const
{
    return  QString("%4{'gain':'%1','Solid':'%2','Continuous':'%3'}").arg(getGain(),getSolidState(),getContinousState(),EQ_GAIN);
}
GTAEquationBase* GTAEquationGain::getClone()const
{
    GTAEquationBase* pClone  = new GTAEquationGain(*this);
    return pClone;
}

bool GTAEquationGain::operator ==(GTAEquationBase*& pObj) const
{
    GTAEquationGain *pRcvdObj = dynamic_cast<GTAEquationGain *>(pObj);
    if (pRcvdObj != nullptr)
        return ((this->getGain() == pRcvdObj->getGain()) &&
                (this->getSolidState() == pRcvdObj->getSolidState()) && 
				(this->getContinousState() == pRcvdObj->getContinousState()));
    else
        return false;
}
void  GTAEquationGain::getFunctionStatement(QString& osFuncType,QString& osFunctionStatement)const
{
    osFuncType=QString(EQ_GAIN);
    osFunctionStatement=QString("'FunctionValue':{'gain':'%1','Solid':'%2','Continuous':'%3'}").arg(_GainValue,getSolidState(),getContinousState());
}
void  GTAEquationGain::getEquationArguments(QHash<QString,QString>& iSerialzationMap)const
{
    iSerialzationMap.insert(EQ_GAIN,_GainValue);
}
void  GTAEquationGain::setEquationArguments(const QHash<QString,QString>& iSerialzationMap)
{
    _GainValue=iSerialzationMap.value(EQ_GAIN);
}
QStringList GTAEquationGain::getVariableList() const
{
    QStringList varList;
    if (GTACommandBase::isVariable(_GainValue))
    {
        varList<<_GainValue;
    }
    return varList;
}
void GTAEquationGain:: ReplaceTag(const QMap<QString,QString>& iTagValueMap)
{
    QMapIterator<QString,QString> iterTag(iTagValueMap);
    if (GTACommandBase::isVariable(_GainValue))
    {
        while (iterTag.hasNext())
        {
            iterTag.next();
            QString tag = QString("%1%2%3").arg(QChar(TAG_IDENTIFIER),iterTag.key(),QChar(TAG_IDENTIFIER));
            _GainValue.replace(tag,iterTag.value());

        }
    }
}
