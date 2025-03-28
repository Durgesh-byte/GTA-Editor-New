#include "GTASCXMLFinal.h"

GTASCXMLFinal::GTASCXMLFinal()
{
}


void GTASCXMLFinal::setId(const QString &iId)
{
    _id = iId;
}
QString GTASCXMLFinal::getId()const
{
    return _id;
}
void GTASCXMLFinal::setOnEntry(const GTASCXMLOnEntry &iOnEntry)
{
    _onEntry = iOnEntry;
}

void GTASCXMLFinal::setOnExit(const GTASCXMLOnExit &iOnExit)
{
    _onExit = iOnExit;
}
bool GTASCXMLFinal::translateXMLToStruct(const QDomElement &iFinalElem)
{
    bool rc= false;
    if(!iFinalElem.isNull() && (iFinalElem.nodeName() == SCXML_FINAL))
    {
        rc =true;
        _id = iFinalElem.attribute(SCXML_FINAL_ID);
    }
    return rc;
}
bool GTASCXMLFinal::isNull() const
{
	return _id.isEmpty();
}
QStringList GTASCXMLFinal::getVariableList() const
{
    QStringList varList;
    varList.append(_onEntry.getVariableList());
    varList.append(_onExit.getVariableList());
	return varList;
}

QDomElement GTASCXMLFinal::getSCXML(QDomDocument &iDomDoc)  const
{
    QDomElement finalElem = iDomDoc.createElement(SCXML_FINAL);

    finalElem.setAttribute(SCXML_FINAL_ID,_id);

    if (!_onEntry.isNull())
        finalElem.appendChild(_onEntry.getSCXML(iDomDoc));
    else if (!_onExit.isNull())
        finalElem.appendChild(_onExit.getSCXML(iDomDoc));

    return finalElem;
}
