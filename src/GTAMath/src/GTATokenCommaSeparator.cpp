#include "GTATokenCommaSeparator.h"

GTATokenCommaSeparator ::GTATokenCommaSeparator(const QString& iExpression):GTAIToken(GTAIToken::COMMASEPARATOR, iExpression)
{
}

GTATokenCommaSeparator::~GTATokenCommaSeparator()
{
}

bool GTATokenCommaSeparator::isValid() const
{
    if(_Exp.size() == 1)
    {
        if(_Exp == ",")
            return true;
    }
    return false;
}

GTAIToken* GTATokenCommaSeparator::getClone() const
{
    return new GTATokenCommaSeparator(_Exp);
}

bool GTATokenCommaSeparator::serializeToken(QDomElement &ioRootElement, QDomDocument &iDomDocument) const
{
    if(ioRootElement.isNull() || iDomDocument.isNull())
        return false;
    else
    {
        QDomElement element = iDomDocument.createElement("CommaSeparator");
        element.setAttribute("Representation",_Exp);
        ioRootElement.appendChild(element);
        return true;
    }
}
