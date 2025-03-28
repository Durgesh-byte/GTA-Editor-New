#include "GTATokenConst.h"

GTATokenConst::GTATokenConst(const QString& iExpression):GTAIToken(GTAIToken::CONSTANT,iExpression)
{
   
}
GTATokenConst::~GTATokenConst()
{
}
 bool GTATokenConst::isValid()   const
 {
     bool isNum = false;
     _Exp.toDouble(&isNum);
     return isNum;
 }
GTAIToken* GTATokenConst::getClone()const
{
   GTAIToken* pTokenClone =  new GTATokenConst(_Exp);
   return pTokenClone;
}
bool GTATokenConst::serializeToken(QDomElement& ioRootElement, QDomDocument& iDomDocument)const
{
    if (ioRootElement.isNull()||iDomDocument.isNull())
    {
        return false;
    }
    else
    {
        QDomElement element = iDomDocument.createElement("Constant");
        element.setAttribute("Representation",_Exp);
        ioRootElement.appendChild(element);
        return true;


    }

}