#include "GTASCXMLSend.h"

GTASCXMLSend::GTASCXMLSend()
{
}
void GTASCXMLSend::setId(const QString &iId)
{
    _id = iId;
}

void GTASCXMLSend::setDelay(const QString &iDelay)
{
	bool isNumberConvOk = false;
	QString delayVal = iDelay;
	QString unit = "ms";
	if(delayVal.trimmed().endsWith("ms"))
		unit = "ms";
	else 
		unit = "s";
	
	delayVal.replace(unit,"");

	double delay = delayVal.toDouble(&isNumberConvOk);
	if(isNumberConvOk)
	{
		_delay = QString("%1%2").arg(QString::number(delay,'g',10),unit);
	}
	else
	{
		// For arguments
		if(iDelay.startsWith("@") && iDelay.endsWith("@"))
		{
			_delay = iDelay;
		}
		else
		{
			_delayexpr = iDelay;
		}
	}
}

void GTASCXMLSend::setEvent(const QString &iEvent)
{
    _event = iEvent;
}

bool GTASCXMLSend::translateXMLToStruct(const QDomElement &iSendElem)
{
    bool rc =false;
    if((!iSendElem.isNull())&&(iSendElem.nodeName() == SCXML_SEND))
    {
        rc = true;
        _id = iSendElem.attribute(SCXML_SEND_ID);
        _event = iSendElem.attribute(SCXML_SEND_EVENT);
        _delay = iSendElem.attribute(SCXML_SEND_DELAY);
		_delayexpr = iSendElem.attribute(SCXML_SEND_DELAYEXPR);
    }
    return rc;
}

QStringList GTASCXMLSend::getVariableList() const
{
    return QStringList();
}

QDomElement GTASCXMLSend::getSCXML(QDomDocument &iDomDoc)  const
{
    QDomElement sendElem = iDomDoc.createElement(SCXML_SEND);

    if(!_id.isEmpty())
        sendElem.setAttribute(SCXML_SEND_ID,_id);
    if(!_event.isEmpty())
        sendElem.setAttribute(SCXML_SEND_EVENT,_event);
    if(!_delay.isEmpty())
        sendElem.setAttribute(SCXML_SEND_DELAY,_delay);
	if (!_delayexpr.isEmpty())
		sendElem.setAttribute(SCXML_SEND_DELAYEXPR, _delayexpr);

    return sendElem;
}
