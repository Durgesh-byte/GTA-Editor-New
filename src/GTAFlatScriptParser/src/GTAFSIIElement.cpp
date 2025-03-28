#include "AINGTAFSIIElement.h"

AINGTAFSIIElement::AINGTAFSIIElement()
{

}

void AINGTAFSIIElement::setElementName(const QString &iName)
{
    _Name = iName;
}

QString AINGTAFSIIElement::getElementName()
{
    return _Name;
}
