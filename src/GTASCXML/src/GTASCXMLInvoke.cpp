#include "GTASCXMLInvoke.h"

#pragma warning(push, 0)
#include <QDebug>
#pragma warning(pop)

GTASCXMLInvoke::GTASCXMLInvoke()
{
}


void GTASCXMLInvoke::setInvokeId(const QString &iInvokeId)
{
    _invokeId = iInvokeId;
}

void GTASCXMLInvoke::setInvokeType(const QString &iInvokeType)
{
    _invokeType = iInvokeType;
}
void GTASCXMLInvoke::setInvokeFunction(const GTASCXMLXMLRPC &iXmlRpc)
{
    _func = iXmlRpc;
}

void GTASCXMLInvoke::setInvokeFunctionGeneric(const GTASCXMLGenericFunction &iGenFunc)
{
    _genFunc = iGenFunc;
}

void GTASCXMLInvoke::insertLog(const GTASCXMLLog &iLog)
{
    _logs<<iLog;
}

void GTASCXMLInvoke::insertAssign(const GTASCXMLAssign &iAssign)
{
    _assigns << iAssign;
}

void GTASCXMLInvoke::insertSend(const GTASCXMLSend &iSend)
{
    _sends <<iSend;
}

void GTASCXMLInvoke::setInvokeDelay(const QString &iTimeInMs)
{
    _invokeDelay = iTimeInMs;
}
QStringList GTASCXMLInvoke::getVariableList() const
{
    QStringList varList;
    foreach (GTASCXMLAssign assign, _assigns)
    {
        varList << assign.getVariableList();
    }
    return varList;
}
bool GTASCXMLInvoke::translateXMLToGenericStruct(QDomElement &iInvokeElem)
{
    bool rc =false;
    if(!iInvokeElem.isNull())
    {
        rc =true;
        QDomNodeList invokeChildren = iInvokeElem.childNodes();
        _invokeType = iInvokeElem.attribute(SCXML_INVOKE_TYPE);


        for(int i=0;i<invokeChildren.count();i++)
        {
            QDomNode childNode = invokeChildren.at(i);
            if(!childNode.isNull())
            {
                if(childNode.nodeName() == SCXML_INVOKE_CONTENT)
                {
                    QDomNodeList contentChildren = childNode.childNodes();
                    if(contentChildren.count()>0)
                    {
                        QDomNode genericFuncNode = contentChildren.at(0);
                        _genFunc.translateXMLToStruct(genericFuncNode.toElement());
                    }
                }
                else if(childNode.nodeName() == SCXML_INVOKE_FINALIZE)
                {
                    QDomNodeList finalizeChildren = childNode.childNodes();
                    for(int j=0;j<finalizeChildren.count();j++)
                    {
                        QDomNode finalChild = finalizeChildren.at(j);
                        if(finalChild.isNull())
                        {
                            if(finalChild.nodeName() == SCXML_ASSIGN)
                            {
                                GTASCXMLAssign assign;
                                assign.translateXMLToStruct(finalChild.toElement());
                                _assigns.append(assign);
                            }
                            else if(finalChild.nodeName() == SCXML_SEND)
                            {
                                GTASCXMLSend send;
                                send.translateXMLToStruct(finalChild.toElement());
                                _sends.append(send);
                            }
                            else if(finalChild.nodeName() == SCXML_LOG)
                            {
                                GTASCXMLLog log;
                                log.translateXMLToStruct(finalChild.toElement());
                                _logs.append(log);
                            }
                        }

                    }
                }
            }
        }
    }
    return rc;
}

bool GTASCXMLInvoke::translateXMLToStruct(const QDomElement &iInvokeElem)
{
    bool rc =false;
    if(!iInvokeElem.isNull())
    {
        rc =true;
        QDomNodeList invokeChildren = iInvokeElem.childNodes();
        _invokeType = iInvokeElem.attribute(SCXML_INVOKE_TYPE);


        for(int i=0;i<invokeChildren.count();i++)
        {
            QDomNode childNode = invokeChildren.at(i);
            if(!childNode.isNull())
            {
                if(childNode.nodeName() == SCXML_INVOKE_CONTENT)
                {
                    QDomNodeList contentChildren = childNode.childNodes();
                    if(contentChildren.count()>0)
                    {
                        QDomNode xmlrpcElem = contentChildren.at(0);

                        if((!xmlrpcElem.isNull())&& (xmlrpcElem.nodeName() == SCXML_XMLRPC))
                        {
                            _func.translateXMLToStruct(xmlrpcElem.toElement());
                        }
                    }
                }
                else if(childNode.nodeName() == SCXML_INVOKE_FINALIZE)
                {
                    QDomNodeList finalizeChildren = childNode.childNodes();
                    for(int j=0;j<finalizeChildren.count();j++)
                    {
                        QDomNode finalChild = finalizeChildren.at(j);
                        if(finalChild.isNull())
                        {
                            if(finalChild.nodeName() == SCXML_ASSIGN)
                            {
                                GTASCXMLAssign assign;
                                assign.translateXMLToStruct(finalChild.toElement());
                                _assigns.append(assign);
                            }
                            else if(finalChild.nodeName() == SCXML_SEND)
                            {
                                GTASCXMLSend send;
                                send.translateXMLToStruct(finalChild.toElement());
                                _sends.append(send);
                            }
                            else if(finalChild.nodeName() == SCXML_LOG)
                            {
                                GTASCXMLLog log;
                                log.translateXMLToStruct(finalChild.toElement());
                                _logs.append(log);
                            }
                        }

                    }
                }
            }
        }
    }
    return rc;
}
GTASCXMLXMLRPC GTASCXMLInvoke::getXMLRPCFunction()
{
    return _func;
}
GTASCXMLGenericFunction GTASCXMLInvoke::getGenericFunction()
{
    return _genFunc;
}
QDomElement GTASCXMLInvoke::getSCXML(QDomDocument &iDomDoc)  const
{
    QDomElement invokeElem = iDomDoc.createElement(SCXML_INVOKE);
    QDomElement contentElem = iDomDoc.createElement(SCXML_INVOKE_CONTENT);
    QDomElement finalizeElem = iDomDoc.createElement(SCXML_INVOKE_FINALIZE);

    invokeElem.setAttribute(SCXML_INVOKE_ID,_invokeId);
    if(!_invokeType.isEmpty())
        invokeElem.setAttribute(SCXML_INVOKE_TYPE,_invokeType);

    invokeElem.appendChild(contentElem);
    invokeElem.appendChild(finalizeElem);

    if(!_func.isNull())
        contentElem.appendChild(_func.getSCXML(iDomDoc));
    if(!_genFunc.isNull())
        contentElem.appendChild(_genFunc.getSCXML(iDomDoc));

    if(!_invokeDelay.isEmpty())
    {
        GTASCXMLAssign assign1;
        assign1.setExpression("Date.now()");
        assign1.setLocation("startTime");

        GTASCXMLAssign assign2;
        assign1.setExpression("Date.now()");
        assign1.setLocation("newTime");

        GTASCXMLLog log1;
        log1.setExpression(QString("while ((newTime - startTime) &lt; %1) {newTime=Date.now()}").arg(_invokeDelay));
        log1.setLabel("'Info");

        finalizeElem.appendChild(assign1.getSCXML(iDomDoc));
        finalizeElem.appendChild(assign2.getSCXML(iDomDoc));
        finalizeElem.appendChild(log1.getSCXML(iDomDoc));

    }

    for(int i=0;i<_assigns.count();i++)
    {
        finalizeElem.appendChild(_assigns.at(i).getSCXML(iDomDoc));
    }
    for(int i=0;i<_sends.count();i++)
    {
        finalizeElem.appendChild(_sends.at(i).getSCXML(iDomDoc));
    }
    for(int i=0;i<_logs.count();i++)
    {
        finalizeElem.appendChild(_logs.at(i).getSCXML(iDomDoc));
    }

    return invokeElem;
}
