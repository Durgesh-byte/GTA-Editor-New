#include "GTAIToken.h"
#include "GTAMathExpression.h"
#include "GTATokenFunction.h"
#include "GTAUtil.h"

#pragma warning(push, 0)
#include <QDebug>
#pragma warning(pop)

GTAMathExpression::GTAMathExpression(bool iFuncArg)
{
    _isFunctionArgument=iFuncArg;
}
GTAMathExpression::~GTAMathExpression()
{
    clearAll();
}
void GTAMathExpression::clearAll()
{
    
    for (int i=_TokenList.size()-1;i>=0;i--)
    {
        GTAIToken* pToken = _TokenList.takeAt(i);
        if(pToken)
            delete pToken;

    }
}
GTAMathExpression:: GTAMathExpression(GTAMathExpression* iRhs)
{
    if(NULL!=iRhs)
    {
        QList<GTAIToken*> pList = iRhs->getTokenListClones();
        _TokenList.clear();
        _TokenList.append(pList);
        _isFunctionArgument = iRhs->isFunctionExpression();
    }

}
//bool GTAMathExpression::pushToken(GTAIToken* iToken)
//{
//    bool status =true;
//    if(_TokenList.isEmpty())
//        _TokenList.append(iToken);
//    else
//    {
//        GTAIToken* pLastToken = _TokenList.at(_TokenList.size()-1);
//        if(NULL!=pLastToken)
//        {
//            if(pLastToken->isFunction())
//            {
//                GTATokenFunction* pFunction = dynamic_cast<GTATokenFunction*>(pLastToken);
//                if (pFunction && pFunction->isClosed()==false)
//                {
//                    status = pFunction->pushToken(iToken);
//                }
//                else 
//                {
//                    _TokenList.append(iToken);
//                }
//            }
//            else
//                _TokenList.append(iToken);
//        }
//    }
//
//    return status;
//}
bool GTAMathExpression::pushToken(GTAIToken* iToken)
{
    bool status =true;
    if(_TokenList.isEmpty())
        _TokenList.append(iToken);
    else 
    {
        GTAIToken* pLastToken = _TokenList.at(_TokenList.size()-1);
        if(NULL!=pLastToken)
        {
            if(pLastToken->isFunction())
            {
                GTATokenFunction* pFunction = dynamic_cast<GTATokenFunction*>(pLastToken);

                if (pFunction && iToken->isStartBracket())
                {
                    pFunction->pushToken(iToken);
                }
                else if (pFunction && pFunction->isClosed()==false)
                {
                    if(pFunction->isBracesBalanced())
                    { 
                        pFunction->setClosed(true); 
                        status =appendTokenToCurrentList(iToken);
                    }
                    else
                        status = pFunction->pushToken(iToken);
                }

                else 
                {
                    _TokenList.append(iToken);
                }
            }
            else
            {
                status =appendTokenToCurrentList(iToken);
            }
        }
    }

    return status;
}
bool GTAMathExpression::appendTokenToCurrentList(GTAIToken* iToken)
{
    if(iToken!=NULL)
    {
        if (iToken->isEndBracket())
        {
            if (isBracesBalanced()==false)
            {
                _TokenList.append(iToken);
            }
            else
                return false;
        }
        else
        {
            _TokenList.append(iToken);
        }
    }

    return true;
}
bool GTAMathExpression::isBracesBalanced()
{
    if(_TokenList.isEmpty()==false)
    {

        QList<GTAIToken*> flatList;
        this->getFlatExpression(flatList);

        QStack<GTAIToken*> braces;

        for (int i=0;i<flatList.size();i++)
        {
            GTAIToken* ptoken = flatList.at(i);
            if (ptoken->isStartBracket())
                braces.push(ptoken);

            if (ptoken->isEndBracket())
                if(braces.isEmpty()==false)
                    braces.pop();

        }

        if (braces.isEmpty()==false)
            return false;
    }

    return true;

}
QString GTAMathExpression::getSCXMLExpr()const
{
    QString expression;
    for (int i=0;i<_TokenList.size();i++)
    {
        GTAIToken* pToken = _TokenList.at(i);
        if(pToken->isFunction())
        {
            GTATokenFunction* pFun = dynamic_cast<GTATokenFunction*>(pToken);
            expression.append(QString("%1").arg(pFun->getSCXMLExpr()));
        }
        else
            expression.append(QString(" %1").arg(pToken->getExpression()));
    }
    //    if(expression.contains("cbrt("))
    //    {
    //        QString tempExpression = expression;
    //        tempExpression.replace("cbrt","pow");
    //        if(tempExpression.endsWith(")"))
    //        {
    //            tempExpression = tempExpression.mid(0,tempExpression.lastIndexOf(")"));
    //            tempExpression.append(",(1/3))");
    //        }
    //        return tempExpression;
    //    }
    //    else
    return expression.trimmed();

}
void GTAMathExpression::popToken()
{
    if(!_TokenList.isEmpty())
    {
        GTAIToken* pToken = _TokenList.at(_TokenList.size()-1);
        if(pToken)
        {
            if (pToken->isFunction())
            {
                if(!pToken->popExpression())
                {
                    _TokenList.takeAt(_TokenList.size()-1);
                    delete pToken;
                }

            }
            else
            {
                _TokenList.takeAt(_TokenList.size()-1);
                delete pToken;
            }
        }

    }
}

bool GTAMathExpression::isValid() const
{
	QList<GTAIToken*> lstOfBraces;
    int totalTokens=_TokenList.size();
    int operatorCnt =0;
    int varTypeCnt=0;
	int commaCnt = 0;
    static bool isRandom = false;
    bool flagStartBracket = false;
    bool flagEndBracket = false;

    for (int i=0;i<totalTokens;i++)
    {
        GTAIToken* pToken = _TokenList.at(i);
		
		if(_isFunctionArgument)
        {
            if(i==0)
            {
                if(pToken->isStartBracket())
                {
                    if(isRandom)
                    {
                        flagStartBracket = true;
                    }
                    continue;
                }
                else
                    return false;
            }

            if(i==totalTokens-1)
            {
                if(pToken->isEndBracket())
                {
                    if(isRandom)
                    {
                        flagEndBracket = true;
                    }
                    continue;
                }
                else
                    return false;
            }


        }       
        if (pToken)
        {
            if (pToken->isFunction())
            {
                QString sExp = pToken->getExpression();
                sExp.trimmed();
                GTATokenFunction* pFun = dynamic_cast<GTATokenFunction*>(pToken);
                if(sExp == MATH_FUN_RAND || sExp == MATH_FUNC_RAND_BRACES)
                {
                    if(isRandom)
                    {
                        isRandom = false;
                        return false;
                    }
                    isRandom = true;
                }

                pFun->_FuncExpression->paramInfo = sExp;
                if (pToken->isValid()==false)
                {
                    return false;
                }
                varTypeCnt++;
            }
            else if(pToken->isStartBracket())
            {
                if(isRandom)
                {
                    isRandom = false;
                    return false;
                }
                lstOfBraces.append(pToken);

            }
            else if(pToken->isEndBracket())
            {
                if(!lstOfBraces.isEmpty())
                {
                    if(lstOfBraces.last()->isStartBracket())
                        lstOfBraces.removeLast();
                    else if (lstOfBraces.isEmpty() || lstOfBraces.last()->isEndBracket())
                    {
                        return false;
                    }
                }
                else
                    return false;
            }
            else if (pToken->isOperator())
            {
                operatorCnt++;
            }
            else if (pToken->isConstant() || pToken->isVariable())
            {
                varTypeCnt++;
            }
            else if(pToken->isCommaSeparator())
            {
                commaCnt++;
                qDebug()<<commaCnt;
           }  
            else if(pToken->isFunctionSeperator())
            {

                if(_isFunctionArgument==false)
                    return false;
                else
                {
                    if (varTypeCnt!=operatorCnt+1)
                        return false;

                    if(!lstOfBraces.isEmpty())
                        return false;

                    varTypeCnt =0;
                    operatorCnt=0;
                    lstOfBraces.clear();
                }

            }

        }
    }
    if(_isPowFunction  && commaCnt > 1)
    {
        return false;
    }


    if(isRandom)
    {
        isRandom = false;
        if(0 == varTypeCnt && 0 == operatorCnt && lstOfBraces.isEmpty() && true == flagStartBracket && true == flagEndBracket)
        {
            return true;
        }
        else
            return false;
    }

    if(isMultipleCommaEnabled && commaCnt >= varTypeCnt)
        return false;

    if (! _isPowFunction && !isMultipleCommaEnabled && varTypeCnt!=operatorCnt+1)
        return false;


    if(!lstOfBraces.isEmpty())
        return false;


    return true;
}
QString GTAMathExpression::getExpression()
{
    QString expression;
    for (int i=0;i<_TokenList.size();i++)
    {
        GTAIToken* pToken = _TokenList.at(i);
        if(pToken)
            expression.append(QString(" %1").arg(pToken->getExpression()));
    }
    return expression.trimmed();

}
GTAIToken* GTAMathExpression::getLastItem()
{

    if(!_TokenList.isEmpty())
    {
        GTAIToken* pToken = _TokenList.at(_TokenList.size()-1);
        if (pToken!=NULL && pToken->isFunction())
        {
            return pToken->getLastItem();
        }
        else
            return pToken;
    }
    return NULL;
}
QList<GTAIToken*>     GTAMathExpression::getTokenListClones()
{

    QList<GTAIToken*> clonedTokens;
    for (int i=0;i<_TokenList.size();i++)
    {
        GTAIToken* pToken = _TokenList.at(i);
        if(pToken)
        {
            clonedTokens.append(pToken->getClone());
        }
    }

    return clonedTokens;

}
bool GTAMathExpression::serializeExpression(QDomElement& ioRootElement, QDomDocument& iDomDocument)
{
    if (ioRootElement.isNull() || iDomDocument.isNull())
    {
        return false;
    }
    else
    {
        QDomElement ExpressionElement = iDomDocument.createElement("Expression");
        ioRootElement.appendChild(ExpressionElement);
        for (int i=0;i<_TokenList.size();i++)
        {
            GTAIToken* pToken = _TokenList.at(i);
            if(pToken!=NULL)
                pToken->serializeToken(ExpressionElement,iDomDocument);
        }
        return true;

    }
}
void GTAMathExpression::getVariableList(QStringList& oList)
{
    for (int i=0;i<_TokenList.size();i++)
    {
        GTAIToken* pToken = _TokenList.at(i);
        if (pToken->isVariable())
        {
            oList.append(pToken->getExpression());
        }
        else if (pToken->isFunction())
        {
            GTATokenFunction* pFunction = dynamic_cast<GTATokenFunction*> (pToken);
            if (pFunction)
            {
                pFunction->getVariableList(oList);
            }
        }
    }
}
void GTAMathExpression::setFunctionBracesStack( QStack<GTAIToken*>& oStack)
{
    for (int i=0;i<_TokenList.size();i++)
    {
        GTAIToken* pToken = _TokenList.at(i);
        if (pToken->isStartBracket())
        {
            oStack.push(pToken);
        }
        else if (pToken->isFunction())
        {
            oStack.push(pToken);
            GTATokenFunction* pFunction = dynamic_cast<GTATokenFunction*>(pToken);

            if(pFunction)
                pFunction->setFunctionBracesStack(oStack);
        }
    }
}
void GTAMathExpression::getFlatExpression(QList<GTAIToken*>& oList)
{
    for (int i=0;i<_TokenList.size();i++)
    {
        GTAIToken* pToken = _TokenList.at(i);
        if (pToken->isFunction())
        {
            oList.append(pToken);
            GTATokenFunction* pFunction = dynamic_cast<GTATokenFunction*>(pToken);
            pFunction->getFlatExpression(oList);
        }
        else
            oList.append(pToken);
    }

}
void GTAMathExpression::updateGenToolRetrunParams(const QHash<QString,QString>& iGenToolReturn)
{
    for (int i=0;i<_TokenList.size();i++)
    {
        GTAIToken* pToken = _TokenList.at(i);
        if(pToken != NULL)
        {
            pToken->updateGenToolRetrunParams(iGenToolReturn);
        }
    }
}

void GTAMathExpression:: ReplaceTag(const QMap<QString,QString>& iTagValueMap)
{
    for (int i=0;i<_TokenList.size();i++)
    {
        GTAIToken* pToken = _TokenList.at(i);
        if(pToken != NULL)
        {
            pToken->ReplaceTag(iTagValueMap);
        }
    }
}
