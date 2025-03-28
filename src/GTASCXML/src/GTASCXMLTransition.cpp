#include "GTASCXMLTransition.h"

GTASCXMLTransition::GTASCXMLTransition()
{
}


void GTASCXMLTransition::setCondition(const QString &iCond)
{
    _cond = iCond;
    GTASCXMLUtils::validateExpression(_cond);
}

void GTASCXMLTransition::setEvent(const QString &iEvent)
{
    _event = iEvent;
}

void GTASCXMLTransition::setTarget(const QString &iTarget)
{
    _target = iTarget;
}

void GTASCXMLTransition::insertLog(const GTASCXMLLog &iLog)
{
    _logs <<iLog;
}
void GTASCXMLTransition::setDefaultEventCancel(bool iDefEventCancel)
{
    _hasDefEventCancel = iDefEventCancel;
}

QStringList GTASCXMLTransition::getVariableList() const
{
    QStringList varList;
    foreach (GTASCXMLAssign assign, _assigns)
    {
        varList << assign.getVariableList();
    }
    return varList;
}
void GTASCXMLTransition::insertAssign(const GTASCXMLAssign &iAssign)
{
    _assigns << iAssign;
}

void GTASCXMLTransition::insertSend(const GTASCXMLSend & iSend)
{
    _sends << iSend;
}

void GTASCXMLTransition::setStatus(const QString &iStatus)
{
    _status = iStatus;
}
bool GTASCXMLTransition::translateXMLToStruct(const QDomElement &iTransitionElem)
{
    bool rc =false;
    if(iTransitionElem.nodeName() == SCXML_TRANSITION)
    {
        rc = true;
        _status = iTransitionElem.attribute(SCXML_TRANSITION_STATUS);
        _cond = iTransitionElem.attribute(SCXML_TRANSITION_COND);
        _event = iTransitionElem.attribute(SCXML_TRANSITION_EVENT);
        _target = iTransitionElem.attribute(SCXML_TRANSITION_TARGET);
        QDomNodeList txChildren = iTransitionElem.childNodes();
        for(int i=0;i<txChildren.count();i++)
        {
            QDomNode txChild = txChildren.at(i);
            if(!txChild.isNull())
            {
                if(txChild.nodeName() == SCXML_ASSIGN)
                {
                    GTASCXMLAssign assign;
                    assign.translateXMLToStruct(txChild.toElement());
                    _assigns.append(assign);
                }
                else if(txChild.nodeName() == SCXML_SEND)
                {
                    GTASCXMLSend send;
                    send.translateXMLToStruct(txChild.toElement());
                    _sends.append(send);
                }
                else if(txChild.nodeName() == SCXML_LOG)
                {
                    GTASCXMLLog log;
                    log.translateXMLToStruct(txChild.toElement());
                    _logs.append(log);
                }
            }

        }
    }
    return rc;
}
void GTASCXMLTransition::insertCancel(const GTASCXMLCancel &iCancel)
{
    _cancels <<iCancel;
}
QDomElement GTASCXMLTransition::getSCXML(QDomDocument &iDomDoc) const
{
    QDomElement txElem = iDomDoc.createElement(SCXML_TRANSITION);

    if(!_cond.isEmpty())
        txElem.setAttribute(SCXML_TRANSITION_COND,_cond);
    if(!_event.isEmpty())
        txElem.setAttribute(SCXML_TRANSITION_EVENT,_event);
    if(!_status.isEmpty())
        txElem.setAttribute(SCXML_TRANSITION_STATUS,_status);
    if(!_target.isEmpty())
        txElem.setAttribute(SCXML_TRANSITION_TARGET,_target);

    for(int i=0;i<_assigns.count();i++)
    {
        txElem.appendChild(_assigns.at(i).getSCXML(iDomDoc));
    }
    for(int i=0;i<_sends.count();i++)
    {
        txElem.appendChild(_sends.at(i).getSCXML(iDomDoc));
    }
    for(int i=0;i<_logs.count();i++)
    {
        txElem.appendChild(_logs.at(i).getSCXML(iDomDoc));
    }
    for(int i=0;i<_cancels.count();i++)
    {
        txElem.appendChild(_cancels.at(i).getSCXML(iDomDoc));
    }
    if(_hasDefEventCancel)
    {
        if((!_event.contains("done.invoke") && !_event.trimmed().isEmpty()))
        {
            GTASCXMLCancel cancelEvent;
            cancelEvent.setSendEventId(_event);
            //txElem.appendChild(cancelEvent.getSCXML(iDomDoc));
        }
    }
    return txElem;

}
