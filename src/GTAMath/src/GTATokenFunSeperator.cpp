#include "GTATokenFunSeperator.h"

GTATokenFunSeperator::GTATokenFunSeperator(const QString& iExpression):GTAIToken(GTAIToken::CONSTANT,iExpression)
{
   
}
GTATokenFunSeperator::~GTATokenFunSeperator()
{
}
 bool GTATokenFunSeperator::isValid()   const
 {
     bool isNum = false;
     _Exp.toDouble(&isNum);
     return isNum;
 }

 GTAIToken* GTATokenFunSeperator::getClone()const
 {
     return new GTATokenFunSeperator(_Exp);

 }
 bool GTATokenFunSeperator::serializeToken(QDomElement& ioRootElement, QDomDocument& iDomDocument)const
 {
     if (ioRootElement.isNull()||iDomDocument.isNull())
     {
         return false;
     }
     else
     {
         QDomElement element = iDomDocument.createElement("FunctionSeperator");
         element.setAttribute("Representation",_Exp);
         ioRootElement.appendChild(element);
         return true;


     }

 }