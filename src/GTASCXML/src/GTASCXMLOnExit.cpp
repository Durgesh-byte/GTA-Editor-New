#include "GTASCXMLOnExit.h"

GTASCXMLOnExit::GTASCXMLOnExit()
{
}
void GTASCXMLOnExit::insertLog(const GTASCXMLLog &iLog)
{
    _logs <<iLog;
}
void GTASCXMLOnExit::insertAssign(const GTASCXMLAssign &iAssign)
{
    _assigns << iAssign;
}

void GTASCXMLOnExit::insertSend(const GTASCXMLSend & iSend)
{
    _sends << iSend;
}
bool GTASCXMLOnExit::isNull() const
{
    if(_logs.isEmpty() && _assigns.isEmpty() && _sends.isEmpty())
        return true;
    else
        return false;
}

void GTASCXMLOnExit::insertExtraInfo(const QDomElement &iExtraInfo)
{
    _extraInfoList<<iExtraInfo;
}
QStringList GTASCXMLOnExit::getVariableList() const
{
    QStringList varList;
    foreach (GTASCXMLAssign assign, _assigns)
    {
        varList << assign.getVariableList();
    }
    return varList;
}
bool GTASCXMLOnExit::translateXMLToStruct(const QDomElement &iOnExitElem)
{
    bool rc = false;
    if(!iOnExitElem.isNull())
    {
        QDomNodeList childNodes = iOnExitElem.childNodes();
        if(iOnExitElem.nodeName() == SCXML_ONEXIT)
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
void GTASCXMLOnExit::insertCancel(const GTASCXMLCancel &iCancel)
{
    _cancels <<iCancel;
}
QDomElement GTASCXMLOnExit::getSCXML(QDomDocument &iDomDoc)  const
{
    QDomElement onExitElem = iDomDoc.createElement(SCXML_ONEXIT);


    for(int i=0;i<_assigns.count();i++)
    {
        onExitElem.appendChild(_assigns.at(i).getSCXML(iDomDoc));
    }
    for(int i=0;i<_sends.count();i++)
    {
        onExitElem.appendChild(_sends.at(i).getSCXML(iDomDoc));
    }
    for(int i=0;i<_logs.count();i++)
    {
        onExitElem.appendChild(_logs.at(i).getSCXML(iDomDoc));
    }
    for(int i=0;i<_cancels.count();i++)
    {
        onExitElem.appendChild(_cancels.at(i).getSCXML(iDomDoc));
    }
    for(int i=0;i<_extraInfoList.count();i++)
    {
        onExitElem.appendChild(_extraInfoList.at(i));
    }
    return onExitElem;
}
