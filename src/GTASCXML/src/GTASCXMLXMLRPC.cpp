#include "GTASCXMLXMLRPC.h"

GTASCXMLXMLRPC::GTASCXMLXMLRPC()
{
}
void GTASCXMLXMLRPC::setNodeType(const QString &iNodeType)
{
    _NodeType = iNodeType;
}

void GTASCXMLXMLRPC::setToolId(const QString &iToolId)
{
    _ToolId = iToolId;
}

void GTASCXMLXMLRPC::setToolType(const QString &iToolType)
{
    _ToolType = iToolType;
}

void GTASCXMLXMLRPC::setFunctionName(const QString &iFuncName)
{
    _FunctionName = iFuncName;
}

void GTASCXMLXMLRPC::insertFunctionArgument(const QString &iArgType,const QString &iArgName,const QString &iArgValue)
{
    funcArg arg;
    arg.name = iArgName;
    arg.type = iArgType;
    arg.value = iArgValue;
    _Args<<arg;
}

void GTASCXMLXMLRPC::insertFunctionReturn(const QString &iName, const QString &iType)
{
    funcReturn ret;
    ret.name = iName;
    ret.type = iType;
    _Returns << ret;
}

bool GTASCXMLXMLRPC::translateXMLToStruct(const QDomElement &iXmlRpcElem)
{
    bool rc = false;
    QString nodeName = iXmlRpcElem.nodeName();
    if(!iXmlRpcElem.isNull())
    {
        if(nodeName == SCXML_XMLRPC)
        {
            rc =true;
            _NodeType = nodeName;
            _ToolId = iXmlRpcElem.attribute(SCXML_XMLRPC_TOOL_ID);
            _ToolType = iXmlRpcElem.attribute(SCXML_XMLRPC_TOOL_TYPE);

            QDomNodeList childNodes = iXmlRpcElem.childNodes();
            if(childNodes.count()>0)
            {
                QDomNode funcNode = childNodes.at(0);
                if(!funcNode.isNull())
                {
                    if(funcNode.nodeName() == SCXML_XMLRPC_FUNCTION)
                    {
                        QDomElement funcElem = funcNode.toElement();
                        _FunctionName = funcElem.attribute(SCXML_XMLRPC_FUNC_NAME);
                        QDomNodeList funcChildList = funcElem.childNodes();
                        for(int i=0;i<funcChildList.count();i++)
                        {
                            QDomNode funcChildNode = funcChildList.at(i);
                            if(!funcChildNode.isNull())
                            {
                                QDomElement funcChildNodeElem = funcChildNode.toElement();
                                QString subNodeName = funcChildNodeElem.nodeName();
                                if(subNodeName == SCXML_XMLRPC_FUNC_ARG)
                                {
                                    funcArg arg;
                                    arg.name = funcChildNodeElem.attribute(SCXML_XMLRPC_FUNC_ARG_NAME);
                                    arg.type = funcChildNodeElem.attribute(SCXML_XMLRPC_FUNC_ARG_TYPE);
                                    arg.value = funcChildNodeElem.attribute(SCXML_XMLRPC_FUNC_ARG_VALUE);
                                    _Args<<arg;
                                }
                                else if(subNodeName == SCXML_XMLRPC_FUNC_RET)
                                {
                                    funcReturn ret;

                                    ret.name = funcChildNodeElem.attribute(SCXML_XMLRPC_FUNC_RET_NAME);

                                    _Returns<<ret;
                                }
                                else
                                {

                                }
                            }
                        }
                    }
                    else
                        rc =false;
                }
            }
            else
                rc =false;
        }
    }
    return rc;
}
bool GTASCXMLXMLRPC::getArgumentByName(const QString &iArgName,funcArg &oFuncArg)
{

    for(int i=0;i<_Args.count();i++)
    {
        funcArg arg = _Args.at(i);
        if(arg.name == iArgName)
        {
            oFuncArg = arg;
            return true;
        }
    }
    return false;
}
bool GTASCXMLXMLRPC::updateArgumentByName(const QString &iArgName,funcArg &iFuncArg)
{
    funcArg arg;
    for(int i=0;i<_Args.count();i++)
    {
        arg = _Args.at(i);
        if(arg.name == iArgName)
        {
            _Args[i].name = iFuncArg.name;
            _Args[i].type = iFuncArg.type;
            _Args[i].value = iFuncArg.value;
            return true;
        }
    }

    return false;
}
bool GTASCXMLXMLRPC::getFunctionArgument(funcArg &oRet)
{
    if(!_Args.isEmpty())
    {
        oRet = _Args.first();
        return true;
    }
    return false;
}

bool GTASCXMLXMLRPC::getReturnArgument(funcReturn &oRet)
{
    if(!_Returns.isEmpty())
    {
        oRet = _Returns.first();
        return true;
    }
    return false;
}

bool GTASCXMLXMLRPC::updateReturnArgument(funcReturn &iRet)
{
    if(!_Returns.isEmpty())
    {
        _Returns[0].name = iRet.name;
        _Returns[0].type = iRet.type;
        _Returns[0].value = iRet.value;

        return true;
    }
    return false;
}
QDomElement GTASCXMLXMLRPC::getSCXML(QDomDocument & iDomDoc)  const
{
    QDomElement xmlElem = iDomDoc.createElement(SCXML_XMLRPC);
    if(!_ToolId.isEmpty())
        xmlElem.setAttribute(SCXML_XMLRPC_TOOL_ID,_ToolId);
    if(!_ToolType.isEmpty())
        xmlElem.setAttribute(SCXML_XMLRPC_TOOL_TYPE,_ToolType);

    QDomElement funcElem = iDomDoc.createElement(SCXML_XMLRPC_FUNCTION);
    funcElem.setAttribute(SCXML_XMLRPC_FUNC_NAME,_FunctionName);

    for(int i=0;i<_Args.count();i++)
    {
        funcArg arg = _Args.at(i);
        QDomElement argElem = iDomDoc.createElement(SCXML_XMLRPC_FUNC_ARG);
        if(!arg.name.isEmpty())
            argElem.setAttribute(SCXML_XMLRPC_FUNC_ARG_NAME,arg.name);
        if(!arg.type.isEmpty())
            argElem.setAttribute(SCXML_XMLRPC_FUNC_ARG_TYPE,arg.type);
		//asked by Olivier to send value as blank if mandatory is forced as NO
        /*if(!arg.value.isEmpty())
        {*/
			//scxml argument in external tool now needs to be handled by user himself
           /* if(arg.type == "string")
                argElem.setAttribute(SCXML_XMLRPC_FUNC_ARG_VALUE,QString("'%1'").arg(arg.value));
            else*/
                argElem.setAttribute(SCXML_XMLRPC_FUNC_ARG_VALUE,QString("%1").arg(arg.value));
        //}
        funcElem.appendChild(argElem);
    }
    for(int i=0;i<_Returns.count();i++)
    {
        funcReturn ret = _Returns.at(i);
        QDomElement retElem = iDomDoc.createElement(SCXML_XMLRPC_FUNC_RET);
        retElem.setAttribute(SCXML_XMLRPC_FUNC_RET_NAME,ret.name);
        funcElem.appendChild(retElem);
    }
    xmlElem.appendChild(funcElem);

    return xmlElem;
}
