#include "GTASCXMLGenericFunction.h"

GTASCXMLGenericFunction::GTASCXMLGenericFunction()
{
}

void GTASCXMLGenericFunction::setFunctionName(const QString &iFunctionName)
{
    _FunctionName = iFunctionName;
}

void GTASCXMLGenericFunction::setFunctionReturn(const QString &iReturnName)
{
    _ReturnName = iReturnName;
}

void GTASCXMLGenericFunction::insertFunctionAttributes(const QString &iName, const QString &iValue)
{
    funcAttributes attr;
    attr.name = iName;
    attr.value = iValue;
    _Attrs.append(attr);
}

QString GTASCXMLGenericFunction::getFunctionName() const
{
    return _FunctionName;
}

QString GTASCXMLGenericFunction::getFunctionReturnName() const
{
    return _ReturnName;
}
bool GTASCXMLGenericFunction::getFunctionAttributeByName(const QString &iAttrName, funcAttributes &oFuncAttrs)
{
    for(int i=0;i<_Attrs.count();i++)
    {
        funcAttributes attr = _Attrs.at(i);
        if(attr.name == iAttrName)
        {
            oFuncAttrs = attr;
            return true;
        }

    }
	return false;
}

bool GTASCXMLGenericFunction::updateAttributeByName(const QString &iArgName,const QString &iValue)
{
    for(int i=0;i<_Attrs.count();i++)
    {
        funcAttributes attr = _Attrs.at(i);
        if(attr.name == iArgName)
        {
            _Attrs[i].value = iValue;
            return true;
        }

    }
	return false;
}

bool GTASCXMLGenericFunction::translateXMLToStruct(const QDomElement &iFunctionElem)
{
    bool rc = false;
    QString nodeName = iFunctionElem.nodeName();
    if(!iFunctionElem.isNull())
    {
        rc =true;

        _FunctionName = iFunctionElem.nodeName();

        QDomNamedNodeMap attrs = iFunctionElem.attributes();
        for( int i = 0 ; i < attrs.length() ; ++i )
        {
            if(!(attrs.item(i).isNull()))
            {
                QDomNode debug = attrs.item(i);
                QDomAttr attr = debug.toAttr();
                if(!attr.isNull())
                {
                    funcAttributes funcAttr;
                    funcAttr.value = attr.value();
                    funcAttr.name =  attr.name();
                    _Attrs.append(funcAttr);
                }
            }
        }
        QDomNodeList funcChildList = iFunctionElem.childNodes();
        for(int i=0;i<funcChildList.count();i++)
        {
            QDomNode funcChildNode = funcChildList.at(i);
            if(!funcChildNode.isNull())
            {
                QDomElement funcChildNodeElem = funcChildNode.toElement();
                QString subNodeName = funcChildNodeElem.nodeName();
                if(subNodeName == "returnname")
                {
                    _ReturnName = funcChildNodeElem.attribute("name");

                }
            }
        }
    }
    return rc;
}

QDomElement GTASCXMLGenericFunction::getSCXML(QDomDocument & iDomDoc)  const
{
    QDomElement func = iDomDoc.createElement(_FunctionName);
    for(int i=0;i<_Attrs.count();i++)
    {
        func.setAttribute(_Attrs.at(i).name,_Attrs.at(i).value);
    }
    if(!_ReturnName.trimmed().isEmpty())
    {
        QDomElement returnElem = iDomDoc.createElement("returnvalue");
        returnElem.setAttribute("name",_ReturnName);
        func.appendChild(returnElem);
    }
    return func;
}
