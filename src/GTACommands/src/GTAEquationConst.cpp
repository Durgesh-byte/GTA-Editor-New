#include "GTAEquationConst.h"


void GTAEquationConst::setConstant(const QString & iConst)
{
    _ConstValue = iConst;
}
GTAEquationConst::~GTAEquationConst()
{
    _ConstValue="";
}
QString GTAEquationConst::getConstant() const
{
    return _ConstValue;
}
QString GTAEquationConst::getStatement() const
{
    return getConstant();
}
GTAEquationConst::GTAEquationConst(const GTAEquationConst& rhs):GTAEquationBase(GTAEquationBase::CONST)
{
    _ConstValue = rhs.getConstant();
    setSolidState(rhs.getSolidState());
    setContinousState(rhs.getContinousState());
}

GTAEquationBase* GTAEquationConst:: getClone()const
{
    GTAEquationBase* pClone  = new GTAEquationConst(*this);
    return pClone;
}

bool GTAEquationConst::operator ==(GTAEquationBase*& pObj) const
{
    GTAEquationConst *pRcvdObj = dynamic_cast<GTAEquationConst *>(pObj);
    if (pRcvdObj != nullptr)
        return ((this->getConstant() == pRcvdObj->getConstant()) && (this->getConstType() == pRcvdObj->getConstType()));
    else
        return false;
}


void  GTAEquationConst::getFunctionStatement(QString& osFuncType,QString& osFunctionStatement)const
{
    osFuncType=QString(EQ_CONST).toUpper();
    QString constVal = _ConstValue;

    if(_ConstValue.startsWith("\'"))
        constVal= constVal.replace("\'","");
    if(_ConstValue.endsWith("\'"))
        constVal= constVal.replace("\'","");
    if(_ConstType == ICD)
    {
        osFunctionStatement=QString("'FunctionValue':{'Value':''}");//.arg(constVal);
    }
    else if(_ConstType == LOCAL)
    {
        osFunctionStatement=QString("'FunctionValue':{'Value':''}");//.arg(constVal);
    }
    else
        osFunctionStatement=QString("'FunctionValue':{'Value':''}");//.arg(constVal);
}
void  GTAEquationConst::getEquationArguments(QHash<QString,QString>& iSerialzationMap)const
{
    iSerialzationMap.insert(EQ_CONST,_ConstValue);

}
void  GTAEquationConst::setEquationArguments(const QHash<QString,QString>& iSerialzationMap)
{
    if (iSerialzationMap.contains(EQ_CONST))
    {
        _ConstValue=iSerialzationMap.value(EQ_CONST);
    }
}
QStringList GTAEquationConst::getVariableList() const
{
    QStringList varList;
    if (GTACommandBase::isVariable(_ConstValue) && !_ConstValue.contains(ACT_CONST_EQN_TIME_TYPE_UTC,Qt::CaseSensitive) && !_ConstValue.contains(ACT_CONST_EQN_TIME_TYPE_FORMATTED,Qt::CaseSensitive))
    {
        varList<<_ConstValue;
    }
    return varList;
}
void GTAEquationConst:: ReplaceTag(const QMap<QString,QString>& iTagValueMap)
{
    QMapIterator<QString,QString> iterTag(iTagValueMap);
    while (iterTag.hasNext())
    {
        iterTag.next();
        QString tag = QString("%1%2%3").arg(QChar(TAG_IDENTIFIER),iterTag.key(),QChar(TAG_IDENTIFIER));
        _ConstValue.replace(tag,iterTag.value());

    }
}
void GTAEquationConst::setConstType(ConstType iConstType)
{
    _ConstType = iConstType;
}

GTAEquationConst::ConstType GTAEquationConst::getConstType() const
{
    return _ConstType;
}
void GTAEquationConst::stringReplace(const QRegExp& iStrToFind,const QString& iStringToReplace)
{
    _ConstValue.replace(iStrToFind,iStringToReplace);

}
bool GTAEquationConst::equipmentReplace(const QRegExp& iStrToFind, const QString& iStringToReplace)
{
    QString temp = _ConstValue;
    bool rc = false;
    if (temp.count('.') == 2) // triplet detection
    {
        temp.chop(temp.size() - temp.indexOf('.'));
        if (temp.contains(iStrToFind))
        {
            rc = true;
            temp.replace(iStrToFind, iStringToReplace);
            _ConstValue.remove(0, _ConstValue.indexOf('.'));
            _ConstValue.prepend(temp);
        }
    }
    return rc;
}
bool GTAEquationConst::searchString(const QRegExp& iRegExp, bool) const
{
    bool rc = false;
    if(_ConstValue.contains(iRegExp))
        rc=true;
    else
        rc=false;
    return rc;
}
