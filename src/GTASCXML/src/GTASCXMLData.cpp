#include "GTASCXMLData.h"
#include "GTASCXMLUtils.h"

GTASCXMLData::GTASCXMLData()
{
}
void GTASCXMLData::setDataId(const QString &iId)
{
    _id = iId;
}

void GTASCXMLData::setDataExpr(const QString &iExpr)
{
    _expr = iExpr;
}

QDomElement GTASCXMLData::getSCXML(QDomDocument &iDomDoc)  const
{
    if(!_id.trimmed().isEmpty())
    {
        QDomElement dataElem = iDomDoc.createElement(SCXML_DATA_MODEL_DATA);
        dataElem.setAttribute(SCXML_DATA_MODEL_DATA_ID,_id);
        dataElem.setAttribute(SCXML_DATA_MODEL_DATA_EXPR,_expr);
        return dataElem;
    }
    else
    {
        QDomElement dataElemNull;
        return dataElemNull;
    }
}
