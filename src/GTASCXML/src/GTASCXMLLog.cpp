#include "GTASCXMLLog.h"

GTASCXMLLog::GTASCXMLLog()
{
}
void GTASCXMLLog::setExpression(const QString &iExpr)
{
    _expr = iExpr;
}

void GTASCXMLLog::setLabel(const QString &iLabel)
{
    _label = iLabel;
}
QString GTASCXMLLog::getExpression() const
{
    return _expr;
}
QString GTASCXMLLog::getLabel() const
{
    return _label;
}

bool GTASCXMLLog::translateXMLToStruct(const QDomElement &iLogElem)
{
    bool rc =false;
    if(!iLogElem.isNull())
    {
        if(iLogElem.nodeName() == SCXML_LOG)
        {
            rc = true;
            _expr = iLogElem.attribute(SCXML_LOG_EXPR);
            _label = iLogElem.attribute(SCXML_LOG_LABEL);
        }
    }

    return rc;
}
QStringList GTASCXMLLog::getVariableList() const
{
  return QStringList();
}
QDomElement GTASCXMLLog::getSCXML(QDomDocument &iDomDoc)  const
{
    QDomElement logElem =iDomDoc.createElement(SCXML_LOG);
    logElem.setAttribute(SCXML_LOG_EXPR,_expr);
    logElem.setAttribute(SCXML_LOG_LABEL,_label);

    return logElem;
}
