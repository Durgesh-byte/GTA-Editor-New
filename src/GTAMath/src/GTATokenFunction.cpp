
#include "GTATokenFunction.h"
#include "GTAMathExpression.h"

#pragma warning(push, 0)
#include "qdebug.h"
#pragma warning(pop)

GTATokenFunction::GTATokenFunction(const QString& iExpression,const int& iArguments):GTAIToken(GTAIToken::FUNCTION,iExpression)
{
   _argumentSize = iArguments;
   _FuncExpression = new GTAMathExpression(true);
   _isClosed=false;
}
GTATokenFunction::~GTATokenFunction()
{

    if(NULL!=_FuncExpression)
        delete _FuncExpression;
}

bool GTATokenFunction::pushToken(GTAIToken* iToken)
{

    bool pushStatus=false;
    if (_FuncExpression == NULL)
    {
        _FuncExpression = new GTAMathExpression(true);
    }

    if(_FuncExpression->isEmpty())
        if (iToken->isStartBracket()==false)
            return false;
    
    pushStatus = _FuncExpression->pushToken(iToken);
   /* if(pushStatus== true && iToken->isEndBracket())
        _isClosed = isBracesBalanced();*/
    return pushStatus;
}

bool GTATokenFunction::popExpression()
{
   _isClosed=false;
   if(_FuncExpression!=NULL)
   {
       if (_FuncExpression->isEmpty())
       {
           delete _FuncExpression;
           _FuncExpression=NULL;
           return false;
       }
       else
       {
           _FuncExpression->popToken();

       }
       return true;
   }
   return false;
    
}
bool GTATokenFunction::isValid() const
{
    if(_FuncExpression->paramInfo.contains("min")|| _FuncExpression->paramInfo.contains("max"))
        _FuncExpression->isMultipleCommaEnabled = true;
    else
        _FuncExpression->isMultipleCommaEnabled = false;

    if(_FuncExpression->paramInfo.contains("pow"))
        _FuncExpression->_isPowFunction = true;
    else
        _FuncExpression->_isPowFunction = false;

    return _FuncExpression->isValid();
}
QString GTATokenFunction::getExpression()
{
    QString expression;
    if (_FuncExpression!=NULL)
    {
        expression =_FuncExpression->getExpression();
        expression.prepend(_Exp);
    }
   
    return expression;
    

}
void  GTATokenFunction::ReplaceTag(const QMap<QString,QString>& iTagValueMap)
{
    _FuncExpression->ReplaceTag(iTagValueMap);
}

GTAIToken* GTATokenFunction::getLastItem()
{
    if(_FuncExpression == NULL || _FuncExpression->isEmpty())
        return this;
    else
        return _FuncExpression->getLastItem();
}
GTAIToken* GTATokenFunction::getClone()const
{
    if(_FuncExpression!=NULL)
    {
        QList<GTAIToken*> lstTokens = _FuncExpression->getTokenListClones();
        GTATokenFunction* pFunc = new GTATokenFunction(_Exp,_argumentSize);
        for (int i=0;i<lstTokens.size();i++)
        {
            GTAIToken* pToken = lstTokens.at(i);
            pFunc->pushToken(pToken);
        }
        pFunc->setClosed(true);
        return pFunc;
    }

    return NULL;
   
}
bool GTATokenFunction::serializeToken(QDomElement& ioRootElement, QDomDocument& iDomDocument)const
{
    if (ioRootElement.isNull()||iDomDocument.isNull())
    {
        return false;
    }
    else
    {
        QDomElement element = iDomDocument.createElement("Function");
        element.setAttribute("Representation",_Exp);
        element.setAttribute("ArgSize",QString::number(_argumentSize));
        _FuncExpression->serializeExpression(element,iDomDocument);
        ioRootElement.appendChild(element);
        return true;


    }

}
void GTATokenFunction::setExpression(GTAMathExpression* iExpression)
{
    if(_FuncExpression)
        {
            delete _FuncExpression;
            _FuncExpression=NULL;
        }
    _FuncExpression = iExpression;

}
 void GTATokenFunction::getVariableList(QStringList& oList)
 {
     if(_FuncExpression)
         _FuncExpression->getVariableList(oList);
 }
 QString GTATokenFunction::getSCXMLExpr() const
 {
     QString expression;
     if(_FuncExpression)
     {
        expression=_FuncExpression->getSCXMLExpr();
        expression.prepend(QString("Math.%1").arg(_Exp));
        if(expression.startsWith("Math.cbrt"))
        {
            expression.replace("Math.cbrt","Math.pow");
            expression = expression.mid(0,expression.lastIndexOf(")"));
            expression.append(",(1/3))");
        }
		if(expression.startsWith("Math.log10"))
		{
			expression.replace("Math.log10","Math.ln");
			expression.append("/Math.ln(10)");
		}
		
     }
     return expression;
 }
 void GTATokenFunction:: setFunctionBracesStack( QStack<GTAIToken*>& oStack)
 {
     if(_FuncExpression!=NULL && _FuncExpression->isEmpty()==false )
         _FuncExpression->setFunctionBracesStack(oStack);
 }
 void GTATokenFunction::getFlatExpression(QList<GTAIToken*>& oList)
 {
     _FuncExpression->getFlatExpression(oList);
 } 
 bool GTATokenFunction::isBracesBalanced()
 {
     return _FuncExpression->isBracesBalanced();
 }
//QString GTATokenFunction::getExpression()
//{
//    //QString expression;
//    /*for (int i=0;i<_expressionList.size();i++)
//    {
//
//    GTAMathExpression* pMExp = _expressionList.at(i);
//    if(pMExp)
//    {
//    if(i)
//    expression.append(",");
//
//    expression.append(QString(" %1").arg(pMExp->getExpression()));
//    }
//
//    }*/
//
//    QString expression =_FuncExpression->getExpression();
//    expression.prepend(_Exp);
//    return expression;
//
//
//}

//bool GTATokenFunction::isValid()   const
//{
//    //int noOfArgument = _expressionList.size();
//    //if(noOfArgument-2!=_argumentSize)
//    //    return false;
//    //else 
//    //{
//    //    for (int i=0;i<noOfArgument;i++)
//    //    {
//    //        GTAMathExpression* pExpr = _expressionList.at(i);
//    //        if (pExpr->isEmpty())
//    //        {
//    //            return false;//all expression should have at least one token
//    //        }
//    //    }
//    //}
//    //for (int i=1;i<_expressionList.size()-1;i++)
//    //{
//    //    GTAMathExpression* pExp = _expressionList.at(i);
//    //    if(!pExp->isValid())
//    //        return false;
//    //}
//
//    //return true;
//    return _FuncExpression->isValid();
//
//}

//bool GTATokenFunction::popExpression()
//{
//    /*int lastItemCnt = _expressionList.size()-1;
//    if(!lastItemCnt)
//    {
//
//    GTAMathExpression* pExp = _expressionList.at(lastItemCnt);
//    if (pExp->isEmpty())
//    {
//    _expressionList.takeAt(lastItemCnt);
//    delete pExp;
//    return false;
//    }
//    else
//    pExp->popToken();
//    return true;
//    }*/
//    _isClosed=false;
//    if (_FuncExpression->isEmpty())
//    {
//        delete _FuncExpression;
//        _FuncExpression=NULL;
//        return false;
//    }
//
//    _FuncExpression->popToken();
//    return true;
//
//}


//void GTATokenFunction::pushExpression(GTAMathExpression* iToken)
//{
//     _expressionList.push_back(iToken);
//}
//bool GTATokenFunction::pushToken(GTAIToken* iToken)
//{
//    
//    bool pushStatus=false;
//
//    if(_isClosed)
//        return pushStatus;
//
//    if (iToken->isEndBracket())
//    {
//        if(_FuncExpression->isValid())
//            pushStatus= false;
//        else
//        {
//            pushStatus = _FuncExpression->pushToken(iToken);
//            _isClosed=true;
//        }
//    }
//    else
//        pushStatus = _FuncExpression->pushToken(iToken);
//    return pushStatus;
//    //if(_expressionList.isEmpty())
//    //    _expressionList.append(new GTAMathExpression);
//
//    //if(iToken->isFunctionSeperator())
//    //{
//    //    _expressionList.append(new GTAMathExpression);
//    //    return true;
//    //}
//    //else  if(iToken->isStartBracket())
//    //{
//    //    GTAMathExpression* firstExp = _expressionList.at(0);
//    //    if(firstExp.isEmpty())
//    //        firstExp.pushToken(isStartBracket());
//    //    _expressionList.append(new GTAMathExpression);
//    //}
//    //else if (iToken->isEndBracket())
//    //{
//    //    /*for(int i=0;i<_expressionList.size();i++)
//    //    {
//    //        GTAMathExpression* pExp = _expressionList.at(i);
//    //        if(pExp && pExp->isValid()==true)
//    //           return false;
//    //    }*/
//
//    //    GTAMathExpression* lastExp = NULL;
//
//    //    lastExp = _expressionList.last();
//    //    if (lastExp->isValid())
//    //    {
//    //        lastExp = new GTAMathExpression;
//    //        lastExp->pushToken(iToken);
//    //        _expressionList.append();
//    //        return true;
//    //    }
//    //    else
//    //    
//    //   
//    //}
//    //else
//    //{
//    //    GTAMathExpression* pExpression = _expressionList.last();
//    //    if(pExpression)
//    //        pExpression->pushToken(iToken);
//
//    //}
//   
//}

//GTATokenFunction::~GTATokenFunction()
//{
//    /*for (int i=_expressionList.size()-1;i>=0;i--)
//    {
//    GTAMathExpression* pExp = _expressionList.takeAt(i);
//    if(pExp)
//    delete pExp;
//
//    }*/
//    if(NULL!=_FuncExpression)
//        delete _FuncExpression;
//}
