#include "GTAEquationCrenels.h"

GTAEquationCrenels::GTAEquationCrenels() : GTAEquationBase(GTAEquationBase::CRENELS)
{
    _LengthMapTable.clear();
    _LevelMapTable.clear();
}
GTAEquationCrenels::~GTAEquationCrenels()
{

}
GTAEquationCrenels::GTAEquationCrenels(const GTAEquationCrenels& rhs):GTAEquationBase(GTAEquationBase::CRENELS)
{
    int dLengthCnt = rhs.getLengthCount();
    for(int i = 1; i <= dLengthCnt; i++)
        _LengthMapTable[i] = rhs.getLength(i);

    int dLevelCnt = rhs.getLevelCount();
    for(int i = 1; i <= dLevelCnt; i++)
        _LevelMapTable[i] = rhs.getLevel(i);

    this->setSolidState(rhs.getSolidState());
    this->setContinousState(rhs.getContinousState());
}

// GET Functions
int GTAEquationCrenels::getLengthCount() const
{
    return _LengthMapTable.count();
}
int GTAEquationCrenels::getLevelCount() const
{
    return _LevelMapTable.count();
}
QString GTAEquationCrenels::getLevel(int Idx) const
{
    QString oValue;
    if(_LevelMapTable.contains(Idx))
        oValue = _LevelMapTable.value(Idx);

    return oValue;
}
QString GTAEquationCrenels::getLength(int Idx) const
{
    QString oValue;
    if(_LengthMapTable.contains(Idx))
        oValue = _LengthMapTable.value(Idx);

    return oValue;
}

// INSERT Functions
void GTAEquationCrenels::insertLevel(int index, const QString & iLevel )
{
    _LevelMapTable.insert(index, iLevel);
}
void GTAEquationCrenels::insertLength(int index, const QString & iLen )
{
    _LengthMapTable.insert(index, iLen);
}

QString GTAEquationCrenels::getStatement() const
{
    QString args = getArgumentStatement();
    if(!args.isEmpty())
    {
        // We must remove the last character "}" in order to concat Solid & Continuous labels
        args.resize(args.size() - 1);
    }
    return QString("%1%2,'Solid':'%3','Continuous':'%4'}").arg(EQ_CRENELS, args,getSolidState(),getContinousState()); //getArgumentStatement());

}
GTAEquationBase* GTAEquationCrenels::getClone()const
{
    return new GTAEquationCrenels(*this);
}

bool GTAEquationCrenels::operator ==(GTAEquationBase*& pObj) const
{
    GTAEquationCrenels *pRcvdObj = dynamic_cast<GTAEquationCrenels *>(pObj);

    if (pRcvdObj != nullptr)
    {
        if((this->getSolidState() != pRcvdObj->getSolidState()) || (this->getContinousState() != pRcvdObj->getContinousState()))
            return false;

        // compare level count and length count of crenels widget
        if((this->getLevelCount() != pRcvdObj->getLevelCount()) || (this->getLengthCount() != pRcvdObj->getLengthCount()))
            return false;
        else
        {
            if((this->_LevelMapTable != pRcvdObj->_LevelMapTable) || (this->_LengthMapTable != pRcvdObj->_LengthMapTable))
                return false;
        }
        return true;
    }
    else
        return false;
}

void  GTAEquationCrenels:: getFunctionStatement(QString& osFuncType,QString& osFunctionStatement)const
{
    osFuncType = QString(EQ_CRENELS).toUpper();
    osFunctionStatement = QString("'FunctionValue':%1").arg(osFuncType, getArgumentStatement());
}

void GTAEquationCrenels::getEquationArguments(QHash<QString,QString>& iSerializationMap)const
{
    int lenCount = this->getLengthCount();
    int levCount = this->getLevelCount();
    for(int i = 1; i <= lenCount; i++)
    {
        QString value = getLength(i);
        QString lengthParam = QString("%1%2").arg("Length",QString::number(i));
        iSerializationMap.insert(lengthParam, value);
    }

    for(int i = 1; i <= levCount; i++)
    {
        QString value = getLevel(i);
        QString levelParam = QString("%1%2").arg("Level",QString::number(i));
        iSerializationMap.insert(levelParam, value);
    }
}

void GTAEquationCrenels::setEquationArguments(const QHash<QString,QString>& iSerializationMap)
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
        }
    }
}

QString GTAEquationCrenels::getArgumentStatement() const
{
    QStringList sEqItems;
    QString oEquation = "{";
    // For CRENELS Equation, there is always more Level than Length fields
    for(int i = 1; i <= getLevelCount(); i++)
    {
        QString level = getLevel(i);
        QString length = getLength(i);

        sEqItems.append(QString("'Level%1':'%2'").arg(QString::number(i), level));

        if(!length.isEmpty())
        {
            sEqItems.append(QString("'Length%1':'%2'").arg(QString::number(i), length));
        }
    }

    oEquation += sEqItems.join(", ");

    // We need to close brackets of the expression
    oEquation += QString("}");
    return oEquation;
}

QStringList GTAEquationCrenels::getVariableList() const
{
    QStringList varList;

    for(int i = 1; i <= getLevelCount(); i++)
    {
        QString level = getLevel(i);
        QString length = getLength(i);

        if (GTACommandBase::isVariable(level))
                varList << level;
        if (GTACommandBase::isVariable(length))
                varList << length;
    }
    return varList;
}

void GTAEquationCrenels::ReplaceTag(const QMap<QString,QString>& iTagValueMap)
{
    QMapIterator<QString,QString> iterTag(iTagValueMap);
    while (iterTag.hasNext())
    {
        iterTag.next();
        QString tag = QString("%1%2%3").arg(QChar(TAG_IDENTIFIER),iterTag.key(),QChar(TAG_IDENTIFIER));

        foreach(int key,_LengthMapTable.keys())
        {
            QString sLength = _LengthMapTable.value(key);
            if (GTACommandBase::isVariable(sLength))
                _LengthMapTable[key] = sLength.replace(tag, iterTag.value());
        }

        foreach(int key,_LevelMapTable.keys())
        {
            QString sLength = _LevelMapTable.value(key);
            if (GTACommandBase::isVariable(sLength))
                _LevelMapTable[key] = sLength.replace(tag, iterTag.value());
        }
    }
}
