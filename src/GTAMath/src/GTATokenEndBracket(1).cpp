
#include "GTATokenEndBracket.h"
GTATokenEndBracket::GTATokenEndBracket(const QString& iExpression):GTAIToken(GTAIToken::ENDBRACKET,iExpression)
{
   
}
GTATokenEndBracket::~GTATokenEndBracket()
{
}
 bool GTATokenEndBracket::isValid()   const
 {
     if(_Exp.size() == 1)
     {
         if(_Exp == ")")
             return true;
     }
     return false;
}
GTAIToken* GTATokenEndBracket::getClone()const
{

    GTAIToken* ptokenClone = new GTATokenEndBracket(_Exp);
    return ptokenClone;
}
bool GTATokenEndBracket::serializeToken(QDomElement& ioRootElement, QDomDocument& iDomDocument)const
{
    if (ioRootElement.isNull()||iDomDocument.isNull())
    {
        return false;
    }
    else
    {
        QDomElement element = iDomDocument.createElement("EndBracket");
        element.setAttribute("Representation",_Exp);
        ioRootElement.appendChild(element);
        return true;


    }

}