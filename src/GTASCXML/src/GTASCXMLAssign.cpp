#include "GTASCXMLAssign.h"

GTASCXMLAssign::GTASCXMLAssign()
{
}

void GTASCXMLAssign::setExpression(const QString &iExpr)
{
    _expr = iExpr;
}

void GTASCXMLAssign::setLocation(const QString &iLocation)
{
    _loc = iLocation;
}
bool GTASCXMLAssign::translateXMLToStruct(const QDomElement &iAssignElem)
{
    bool rc = false;
    if(!iAssignElem.isNull() && (iAssignElem.nodeName() == SCXML_ASSIGN))
    {
        rc = true;
        _expr  = iAssignElem.attribute(SCXML_ASSIGN_EXPR);
        _loc  = iAssignElem.attribute(SCXML_ASSIGN_LOCATION);
    }
    return rc;
}
QStringList GTASCXMLAssign::getVariableList() const
{
    QRegExp rx("([A-Za-z0-9._]+)");
    QString location = _loc;

    bool isNum = false;
    location.toDouble(&isNum);
    bool isString = (location.contains("\"") || location.contains("\'"));

    if((!isNum) && (!isString))
    {
        if(location.contains("["))
            location = (location.split("[")).at(0);

        if(location.contains(rx))
            return QStringList()<<location;
        else
            return QStringList();
    }
    else
        return QStringList();

}

QString GTASCXMLAssign::getExpression() const
{
    return _expr;
}
QString GTASCXMLAssign::getLocation() const
{
    return _loc;
}

QDomElement GTASCXMLAssign::getSCXML(QDomDocument &iDomDoc)  const
{
    QDomElement assignElem = iDomDoc.createElement(SCXML_ASSIGN);
    QString expression = _expr;

    if((expression.startsWith("\"")) && (expression.endsWith("\"")))
        expression = expression.replace("\"","'");

    assignElem.setAttribute(SCXML_ASSIGN_EXPR,expression);
    assignElem.setAttribute(SCXML_ASSIGN_LOCATION,_loc);
    return assignElem;
}
