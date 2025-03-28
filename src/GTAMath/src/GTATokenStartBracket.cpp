
#include "GTATokenStartBracket.h"
GTATokenStartBracket::GTATokenStartBracket(const QString& iExpression):GTAIToken(GTAIToken::STARTBRACKET,iExpression)
{
   
}
GTATokenStartBracket::~GTATokenStartBracket()
{
}
 bool GTATokenStartBracket::isValid()   const
 {
     if(_Exp.size() == 1)
     {
         if(_Exp == "(")
             return true;
     }
     return false;
 }

 GTAIToken* GTATokenStartBracket::getClone()const
 {
    return new GTATokenStartBracket(_Exp);
 }
 bool GTATokenStartBracket::serializeToken(QDomElement& ioRootElement, QDomDocument& iDomDocument)const
 {
     if (ioRootElement.isNull()||iDomDocument.isNull())
     {
         return false;
     }
     else
     {
         QDomElement element = iDomDocument.createElement("StartBracket");
         element.setAttribute("Representation",_Exp);
         ioRootElement.appendChild(element);
         return true;


     }

 }