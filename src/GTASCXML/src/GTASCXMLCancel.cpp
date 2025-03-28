#include "GTASCXMLCancel.h"

GTASCXMLCancel::GTASCXMLCancel()
{
    _SendEventId  ="";
}
void GTASCXMLCancel::setSendEventId(const QString &iSendEventId)
{
    _SendEventId = iSendEventId;
}

QDomElement GTASCXMLCancel::getSCXML(QDomDocument &iDomDoc)  const
{
    QDomElement cancelElem = iDomDoc.createElement(SCXML_CANCEL);
    cancelElem.setAttribute(SCXML_CANCEL_SENDID,_SendEventId);
    return cancelElem;
}
