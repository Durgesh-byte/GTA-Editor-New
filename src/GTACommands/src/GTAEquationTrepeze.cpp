#include "GTAEquationTrepeze.h"

GTAEquationTrepeze::GTAEquationTrepeze():GTAEquationBase(GTAEquationBase::TREPEZE)
{
    _RampMapTable.clear();
    _LengthMapTable.clear();
    _LevelMapTable.clear();
}
GTAEquationTrepeze::~GTAEquationTrepeze()
{

}
GTAEquationTrepeze::GTAEquationTrepeze(const GTAEquationTrepeze& rhs):GTAEquationBase(GTAEquationBase::TREPEZE)
{
    int dLengthCnt = rhs.getLengthCount();
    for(int i = 1; i <= dLengthCnt; i++)
        _LengthMapTable[i] = rhs.getLength(i);

    int dLevelCnt = rhs.getLevelCount();
    for(int i = 1; i <= dLevelCnt; i++)
        _LevelMapTable[i] = rhs.getLevel(i);

    int dRampCnt = rhs.getRampCount();
    for(int i = 1; i <= dRampCnt; i++)
        _RampMapTable[i] = rhs.getRamp(i);

    this->setSolidState(rhs.getSolidState());
    this->setContinousState(rhs.getContinousState());
}

// GET Functions
int GTAEquationTrepeze::getRampCount() const
{
    return _RampMapTable.count();
}
int GTAEquationTrepeze::getLengthCount() const
{
    return _LengthMapTable.count();
}
int GTAEquationTrepeze::getLevelCount() const
{
    return _LevelMapTable.count();
}
QString GTAEquationTrepeze::getRamp(int Idx) const
{
    QString oValue;
    if(_RampMapTable.contains(Idx))
        oValue = _RampMapTable.value(Idx);

    return oValue;
}
QString GTAEquationTrepeze::getLevel(int Idx) const
{
    QString oValue;
    if(_LevelMapTable.contains(Idx))
        oValue = _LevelMapTable.value(Idx);

    return oValue;
}
QString GTAEquationTrepeze::getLength(int Idx) const
{
    QString oValue;
    if(_LengthMapTable.contains(Idx))
        oValue = _LengthMapTable.value(Idx);

    return oValue;
}

// INSERT Functions
void GTAEquationTrepeze::insertRamp(int index, const QString & iRamp )
{
    _RampMapTable.insert(index, iRamp);
}

void GTAEquationTrepeze::insertLevel(int index, const QString & iLevel )
{
    _LevelMapTable.insert(index, iLevel);
}

void GTAEquationTrepeze::insertLength(int index, const QString & iLen )
{
    _LengthMapTable.insert(index, iLen);
}

QString GTAEquationTrepeze::getStatement() const
{
    QString args = getArgumentStatement();
    if(!args.isEmpty())
    {
        // We must remove the last character "}" in order to concat Solid & Continuous labels
        args.resize(args.size() - 1);
    }
    return QString("%1%2,'Solid':'%3','Continuous':'%4'}").arg(EQ_TRAPEZE,args,getSolidState(),getContinousState()); //getArgumentStatement());
}

GTAEquationBase* GTAEquationTrepeze::getClone() const
{
    return new GTAEquationTrepeze(*this);
}

bool GTAEquationTrepeze::operator ==(GTAEquationBase*& pObj) const
{
    GTAEquationTrepeze *pRcvdObj = dynamic_cast<GTAEquationTrepeze *>(pObj);
    if (pRcvdObj != nullptr)
    {
        if((this->getSolidState() != pRcvdObj->getSolidState()) ||
                (this->getContinousState() != pRcvdObj->getContinousState()))
        {
            return false;
        }
        // compare level count, length count, ramp count of Trepeze widget
        if((this->getLevelCount() != pRcvdObj->getLevelCount()) ||
                (this->getLengthCount() != pRcvdObj->getLengthCount()) ||
                (this->getRampCount() != pRcvdObj->getRampCount()))
        {
            return false;
        }
        else
        {
            if((this->_LevelMapTable != pRcvdObj->_LevelMapTable) ||
                    (this->_LengthMapTable != pRcvdObj->_LengthMapTable) ||
                    (this->_RampMapTable != pRcvdObj->_RampMapTable))
            {
                return false;
            }
        }
        return true;
    }
	else
	{
		return false;
	}
}
void GTAEquationTrepeze::getFunctionStatement(QString& osFuncType, QString& osFunctionStatement) const
{
    osFuncType = QString(EQ_TRAPEZE).toUpper();
    osFunctionStatement = QString ("'FunctionValue':%1").arg(osFuncType, getArgumentStatement());
}

void GTAEquationTrepeze::getEquationArguments(QHash<QString,QString>& iSerializationMap)const
{
    int lenCount = getLengthCount();
    int levCount = getLevelCount();
    int ramCount = getRampCount();
    for(int i = 1; i <= lenCount; i++)
    {
        QString value = getLength(i);
        QString lengthParam = QString("%1%2").arg("Length", QString::number(i));
        iSerializationMap.insert(lengthParam, value);
    }
    for(int i = 1; i <= levCount; i++)
    {
        QString value = getLevel(i);
        QString levelParam = QString("%1%2").arg("Level", QString::number(i));
        iSerializationMap.insert(levelParam, value);
    }
    for(int i = 1; i <= ramCount; i++)
    {
        QString value = getRamp(i);
        QString rampParam = QString("%1%2").arg("Ramp", QString::number(i));
        iSerializationMap.insert(rampParam, value);
    }
}

void GTAEquationTrepeze::setEquationArguments(const QHash<QString,QString>& iSerializationMap)
{
    foreach(QString paramName , iSerializationMap.keys())
    {
        QString paramVal = iSerializationMap.value(paramName);

        if(iSerializationMap.contains(paramName))
        {
            bool ok = false;
            if(paramName.contains("Length"))
            {
                int lengthCnt = paramName.remove("Length").toInt(&ok, 10);
                if (ok)
                {
                    this->insertLength(lengthCnt, paramVal);
                }
            }
            else if(paramName.contains("Level"))
            {
                int levelCnt = paramName.remove("Level").toInt(&ok, 10);
                if (ok)
                {
                    this->insertLevel(levelCnt, paramVal);
                }
            }
            else if(paramName.contains("Ramp"))
            {
                int rampCnt = paramName.remove("Ramp").toInt(&ok, 10);
                if (ok)
                {
                    this->insertRamp(rampCnt, paramVal);
                }
            }
        }
    }
}

QString GTAEquationTrepeze::getArgumentStatement() const
{
    QStringList sEqItems;
    QString oEquation = "{";
    // For TRAPEZE Equation, there is always more Level than Length or Ramp fields
    for(int i = 1; i <= getLevelCount(); i++)
    {
        QString level = getLevel(i);
        QString length= getLength(i);
        QString ramp = getRamp(i);

        sEqItems.append(QString("'Level%1':'%2'").arg(QString::number(i),level));

        if(!length.isEmpty())
        {
            sEqItems.append(QString("'Length%1':'%2'").arg(QString::number(i),length));
        }
        if(!ramp.isEmpty())
        {
            sEqItems.append(QString("'Ramp%1':'%2'").arg(QString::number(i),ramp));
        }
    }

    oEquation += sEqItems.join(", ");

    // We need to close brackets of the expression
    oEquation += QString("}");
    return oEquation;
}

QStringList GTAEquationTrepeze::getVariableList() const
{
    QStringList varList;

    for(int i = 1; i <= getLevelCount(); i++)
    {
        QString level = getLevel(i);
        QString length = getLength(i);
        QString ramp = getRamp(i);
        
        if (GTACommandBase::isVariable(level))
            varList << level;
        if (GTACommandBase::isVariable(length))
            varList << length;
        if (GTACommandBase::isVariable(ramp))
            varList << ramp;
    }
    return varList;
}

void GTAEquationTrepeze::ReplaceTag(const QMap<QString,QString>& iTagValueMap)
{
    QMapIterator<QString,QString> iterTag(iTagValueMap);
    while (iterTag.hasNext())
    {
        iterTag.next();
        QString tag = QString("%1%2%3").arg(QChar(TAG_IDENTIFIER),iterTag.key(),QChar(TAG_IDENTIFIER));

        foreach(int key,_RampMapTable.keys())
        {
            QString sRamp = _RampMapTable.value(key);
            if (GTACommandBase::isVariable(sRamp))
                _RampMapTable[key] = sRamp.replace(tag, iterTag.value());
        }
        foreach(int key,_LengthMapTable.keys())
        {
            QString sLength = _LengthMapTable.value(key);
            if (GTACommandBase::isVariable(sLength))
                _LengthMapTable[key] = sLength.replace(tag, iterTag.value());
        }
        foreach(int key,_LevelMapTable.keys())
        {
            QString sLevel = _LevelMapTable.value(key);
            if (GTACommandBase::isVariable(sLevel))
                _LevelMapTable[key] = sLevel.replace(tag, iterTag.value());
        }
    }
}
