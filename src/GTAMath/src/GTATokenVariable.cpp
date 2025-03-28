
#include "GTATokenVariable.h"
GTATokenVariable::GTATokenVariable(const QString& iExpression):GTAIToken(GTAIToken::OPERATOR,iExpression)
{
   
}
GTATokenVariable::~GTATokenVariable()
{
}

bool GTATokenVariable::isValid()const
{
    return true;
}

GTAIToken* GTATokenVariable::getClone()const
{
    return new GTATokenVariable(_Exp);

}
bool GTATokenVariable::serializeToken(QDomElement& ioRootElement, QDomDocument& iDomDocument)const
{
    if (ioRootElement.isNull()||iDomDocument.isNull())
    {
        return false;
    }
    else
    {
        QDomElement element = iDomDocument.createElement("Variable");
        element.setAttribute("Representation",_Exp);
        ioRootElement.appendChild(element);
        return true;

        
    }

}