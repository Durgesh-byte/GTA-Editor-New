
#include "GTATokenOperator.h"
GTATokenOperator::GTATokenOperator(const QString& iExpression):GTAIToken(GTAIToken::OPERATOR,iExpression)
{
   
}
GTATokenOperator::~GTATokenOperator()
{
}


bool GTATokenOperator::isValid()   const
{
    QString validOperators = QString("+-*/^");
    if(_Exp.size()==1)
        if (validOperators.contains(_Exp))
            return true;
        

    return false;
}

GTAIToken*  GTATokenOperator::getClone()const
{
    return new GTATokenOperator(_Exp);

}
bool GTATokenOperator::serializeToken(QDomElement& ioRootElement, QDomDocument& iDomDocument)const
{
    if (ioRootElement.isNull()||iDomDocument.isNull())
    {
        return false;
    }
    else
    {
        QDomElement element = iDomDocument.createElement("Operator");
        element.setAttribute("Representation",_Exp);
        ioRootElement.appendChild(element);
        return true;


    }

}