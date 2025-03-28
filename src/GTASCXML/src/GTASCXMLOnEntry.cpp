#include "GTASCXMLOnEntry.h"

GTASCXMLOnEntry::GTASCXMLOnEntry()
{
}
void GTASCXMLOnEntry::insertLog(const GTASCXMLLog &iLog)
{
    _logs <<iLog;
}

QList<GTASCXMLLog> GTASCXMLOnEntry::getLogs() const
{
	return _logs;
}

void GTASCXMLOnEntry::insertAssign(const GTASCXMLAssign &iAssign)
{
    _assigns << iAssign;
}

void GTASCXMLOnEntry::insertSend(const GTASCXMLSend & iSend)
{
    _sends << iSend;
}

QList<GTASCXMLSend> GTASCXMLOnEntry::getSends() const 
{
	return _sends;
}

bool GTASCXMLOnEntry::isNull() const
{
    if(_logs.isEmpty() && _assigns.isEmpty() && _sends.isEmpty())
        return true;
    else
        return false;
}
void GTASCXMLOnEntry::insertCancel(const GTASCXMLCancel &iCancel)
{
    _cancels << iCancel;
}

QList<GTASCXMLCancel> GTASCXMLOnEntry::getCancels() const 
{
	return _cancels; 
}

void GTASCXMLOnEntry::insertExtraInfo(QDomElement &iExtraInfo)
{
    _extraInfoList<<iExtraInfo;
}

QList<QDomElement> GTASCXMLOnEntry::getExtraInfo() const 
{
	return _extraInfoList;
}

QStringList GTASCXMLOnEntry::getVariableList() const
{
    QStringList varList;
    foreach (GTASCXMLAssign assign, _assigns)
    {
        varList << assign.getVariableList();
    }
    return varList;
}

bool GTASCXMLOnEntry::translateXMLToStruct(const QDomElement &iOnEntryElem)
{
    bool rc = false;
    if(!iOnEntryElem.isNull())
    {
        QDomNodeList childNodes = iOnEntryElem.childNodes();
        if(iOnEntryElem.nodeName() == SCXML_ONENTRY)
        {
            rc = true;
            for(int i=0;i<childNodes.count();i++)
            {
                QDomNode childElem = childNodes.at(i);
                if(!childElem.isNull())
                {
                    if(childElem.nodeName() == SCXML_ASSIGN)
                    {
                        GTASCXMLAssign assign;
                        assign.translateXMLToStruct(childElem.toElement());
                        _assigns<<assign;
                    }
                    else if(childElem.nodeName() == SCXML_LOG)
                    {
                        GTASCXMLLog log;
                        log.translateXMLToStruct(childElem.toElement());
                        _logs<<log;
                    }
                    else if(childElem.nodeName() == SCXML_SEND)
                    {
                        GTASCXMLSend send;
                        send.translateXMLToStruct(childElem.toElement());
                        _sends<<send;
                    }
                    else
                    {

                    }
                }

            }
        }
    }
    return rc;
}

QList<GTASCXMLAssign> GTASCXMLOnEntry::getAssigns() const
{
    return _assigns;
}

QDomElement GTASCXMLOnEntry::getSCXML(QDomDocument &iDomDoc)  const
{
    QDomElement onEntryElem = iDomDoc.createElement(SCXML_ONENTRY);

    for(int i=0;i<_assigns.count();i++)
    {
        onEntryElem.appendChild(_assigns.at(i).getSCXML(iDomDoc));
    }
    for(int i=0;i<_sends.count();i++)
    {
        onEntryElem.appendChild(_sends.at(i).getSCXML(iDomDoc));
    }
    for(int i=0;i<_logs.count();i++)
    {
        onEntryElem.appendChild(_logs.at(i).getSCXML(iDomDoc));
    }
    for(int i=0;i<_cancels.count();i++)
    {
        onEntryElem.appendChild(_cancels.at(i).getSCXML(iDomDoc));
    }
    for(int i=0;i<_extraInfoList.count();i++)
    {
        onEntryElem.appendChild(_extraInfoList.at(i));
    }

    return onEntryElem;
}
