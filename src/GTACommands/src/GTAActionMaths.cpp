#include "GTAActionMaths.h"
#include "GTAUtil.h"
#include "GTAMathExpression.h"

GTAActionMaths::GTAActionMaths()
{
    setAction(ACT_MATHS);
    _pExpression=NULL;
}

GTAActionMaths::GTAActionMaths(const GTAActionMaths& rhs):GTAActionBase(rhs)
{
    /* setAction(rhs.getAction());
    setComplement(rhs.getComplement());
    setComment(rhs.getComment());*/
    setFirstVariable(rhs.getFirstVariable());
    setExpressionTxt(rhs.getExpressionTxt());

    GTAMathExpression* pExp = rhs.getExpression();
    if(NULL!=pExp)
        _pExpression=new GTAMathExpression(pExp);
}
GTAActionMaths::~GTAActionMaths()
{
    delete _pExpression;

}
void GTAActionMaths::setExpression(GTAMathExpression* iExp)
{
    _pExpression = iExp;

}
GTAMathExpression* GTAActionMaths::getExpression()const
{
    return _pExpression;
}

void GTAActionMaths::setFirstVariable(const QString & ipVar)
{
    _var1=ipVar;
}

QString GTAActionMaths::getFirstVariable()const
{
    return _var1;
}


QString GTAActionMaths::getStatement() const
{

    QString var1 = _var1;
    getTrimmedStatement(var1);
    if (_pExpression!=NULL)
    {
        return QString("Maths : %1 = %2").arg(var1,_pExpression->getExpression());
    }
    return QString();
}

QList<GTACommandBase*>& GTAActionMaths::getChildren(void) const
{
    return *_lstChildren;
}
bool GTAActionMaths::insertChildren(GTACommandBase*, const int&)
{
    return false;
}

void  GTAActionMaths::setParent(GTACommandBase* iParent)
{
    _parent=iParent;
}
GTACommandBase* GTAActionMaths::getParent(void) const
{
    return _parent;
}
GTACommandBase* GTAActionMaths::getClone() const
{
    return  new GTAActionMaths(*this);
}
bool GTAActionMaths::canHaveChildren() const
{
    return false;//true;
}
QStringList GTAActionMaths::getVariableList() const
{
    QStringList lstOfVars;
    if( GTACommandBase::isVariable(_var1))
        lstOfVars.append(_var1);
    if(_pExpression!= NULL)
        _pExpression->getVariableList(lstOfVars);
    return lstOfVars;
}
QStringList GTAActionMaths::getRHSVariableList() const
{
    QStringList lstOfVars;
    _pExpression->getVariableList(lstOfVars);
    return lstOfVars;
}

void GTAActionMaths:: ReplaceTag(const QMap<QString,QString>& iTagValueMap) 
{
    //iTagValueMap;//to suppress warning
    QStringList keys = iTagValueMap.keys();
    for(int i=0;i<keys.count();i++)
    {
        QString token = keys.at(i);
        QString value = iTagValueMap.value(token);
        token = TAG_IDENTIFIER+token+TAG_IDENTIFIER;
        if(_var1.contains(token))
        {
            _var1.replace(token,value);
        }
    }
    if(_pExpression != NULL)
        _pExpression->ReplaceTag(iTagValueMap);
}

void GTAActionMaths::updateGenToolRetrunParams(const QHash<QString,QString>& iGenToolReturn)
{
    //iTagValueMap;//to suppress warning
    if(_var1.contains("$"))
    {
        QStringList iTokens = _var1.split("$");
        QStringList keys = iGenToolReturn.keys();
        for(int i=0;i<keys.count();i++)
        {
            QString key = keys.at(i);

            QStringList tokens = key.split("$");

            if(tokens.at(0) == iTokens.at(0))
            {
                if(iTokens.at(1).contains(tokens.at(1)))
                {
                    _var1 = iGenToolReturn.value(key);
                    break;
                }
            }


        }
    }
    if(_pExpression != NULL)
        _pExpression->updateGenToolRetrunParams(iGenToolReturn);
}

bool GTAActionMaths::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
{
    bool rc = false;


    if (_var1.contains(iRegExp))
        return true;

    if (ibTextSearch && this->getComment().contains(iRegExp))
        rc = true;
    else if (ibTextSearch && this->getStatement().contains(iRegExp))
        rc = true;
    else
        rc=false;

    return rc;
}
QString GTAActionMaths::getSCXMLStateName()const
{
    return getStatement();

}
void GTAActionMaths::stringReplace(const QRegExp& iStrToFind,const QString& iStringToReplace)
{

    _var1.replace(iStrToFind,iStringToReplace);
}

QString GTAActionMaths::getSCXMLExpr() const
{
    if(_pExpression != NULL)
    {
        //return processExpression(_pExpression->getSCXMLExpr());
        return _pExpression->getSCXMLExpr();
    }
    return QString();
}
QString GTAActionMaths::processExpression(const QString &ioExpression) const
{
    QString expression = ioExpression;
    QStringList tokens;
    bool tokenFound = false;
    QString tokenItem = "";
    int openBraceCnt = 0;
    int tokenFoundInd = -1;
    while(expression.contains("Math.cbrt"))
    {
        tokens = ((expression.replace("(","$($")).replace(")","$)$")).split("$");
        for(int i=0;i<tokens.count();i++)
        {
            QString token = tokens.at(i);
            if(token.contains("Math.cbrt"))
            {
                tokenFound = true;
                tokenItem = token;
                tokenFoundInd = i;
            }

            if((token.contains("(")) && (tokenFound))
            {
                openBraceCnt++;
            }
            if((token.contains(")")) && (tokenFound))
            {
                openBraceCnt--;
            }
            if(openBraceCnt == 0)
            {
                if(tokenItem == "Math.cbrt")
                {
                    if(token.contains(")"))
                    {
                        token = token.replace(")",",(1/3))");
                        tokens[i] = token;
                        tokens[tokenFoundInd].replace("cbrt","pow");
                    }
                }
            }
        }
        expression = tokens.join("");
    }
    return expression;
}

void GTAActionMaths::setExpressionTxt(const QString& iExpression)
{
    _Expression=iExpression;
}
QString GTAActionMaths::getExpressionTxt()const
{
    return _Expression;
}
bool GTAActionMaths::hasChannelInControl()const
{
    if (GTACommandBase::hasChannelInCtrl(_var1))
    {
        return true;
    }
    foreach (QString rhsVar,getRHSVariableList())
    {
        if (GTACommandBase::hasChannelInCtrl(rhsVar))
        {
            return true;
        }
    }
    return false;
}
