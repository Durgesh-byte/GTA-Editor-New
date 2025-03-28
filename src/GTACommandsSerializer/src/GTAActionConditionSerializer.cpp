#include "GTAActionConditionSerializer.h"
#include "GTAUtil.h"
#include "GTACmdSerializer.h"
GTAActionConditionSerializer::GTAActionConditionSerializer(GTAActionCondition * ipActionCmd) : GTACmdSerializer()
{
    _pActionCmd = ipActionCmd;
}

bool GTAActionConditionSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oElement)//(QDomDocument &iDomDoc,QDomElement &oElement)
{
    bool rc = false;
    if(_pActionCmd != NULL)
    {
       
        QString param = _pActionCmd->getParameter();
        QString paramVal= _pActionCmd->getValue();
        oElement  = iDomDoc.createElement(XNODE_ACTION);
        /* 
        QString actionName = _pActionCmd->getAction();
        QString complmentName = _pActionCmd->getComplement();
        QString onFail = _pActionCmd->getActionOnFail();


        QString hiddenInSCXML = _pActionCmd->isIgnoredInSCXML() ? XNODE_TRUE : XNODE_FALSE;


        QString sTimeOut,sTimeOutUnit,sPrecision,sPrecisionUnit;
        _pActionCmd->getPrecision(sPrecision,sPrecisionUnit);
        _pActionCmd->getTimeOut( sTimeOut,sTimeOutUnit);


        oElement.setAttribute(XNODE_NAME,actionName);
        oElement.setAttribute(XNODE_COMPLEMENT,complmentName);
        oElement.setAttribute(XNODE_TIMEOUT,sTimeOut);
        oElement.setAttribute(XNODE_PRECISION,sPrecision);
		oElement.setAttribute(XNODE_ONFAIL,onFail);
		oElement.setAttribute(XNODE_TIMEOUT_UNIT,sTimeOutUnit);
		oElement.setAttribute(XNODE_PRECISION_UNIT,sPrecisionUnit);
        oElement.setAttribute(XATTR_IGNORE_SCXML,hiddenInSCXML);*/
        //oElement.setAttribute(XNODE_ORDER,QString::number(iOrder));
        setActionAttributes(_pActionCmd,oElement);

        QDomElement domParam = iDomDoc.createElement(XNODE_PARAMETER);
        QDomText domParamNameTxt = iDomDoc.createTextNode(param);
        domParam.appendChild(domParamNameTxt);
        oElement.appendChild(domParam);

        QDomElement domParamVal = iDomDoc.createElement(XNODE_VALUE);
        QDomText domParamValTxt = iDomDoc.createTextNode(paramVal);
        domParamVal.appendChild(domParamValTxt);
        oElement.appendChild(domParamVal);
        rc = true;
    }
    return rc;
}

bool GTAActionConditionSerializer::deserialize(const QDomElement &iElement, GTACommandBase *&opCommand)//(const QDomElement & ioElement)
{
    bool rc = false;
    if(!iElement.isNull())
    {   
        QString param;
        QString paramValue;
        
        QString actionName = iElement.attribute(XNODE_NAME);
        QString complementName = iElement.attribute(XNODE_COMPLEMENT);
        /*QString onFail = iElement.attribute(XNODE_ONFAIL);
		QString sTimeOut = iElement.attribute(XNODE_TIMEOUT);
		QString sTimeOutUnit = iElement.attribute(XNODE_TIMEOUT_UNIT);
		QString sPrecision = iElement.attribute(XNODE_PRECISION);
		QString sPrecisionUnit = iElement.attribute(XNODE_PRECISION_UNIT);
        bool ignoreInSCXML = iElement.attribute(XATTR_IGNORE_SCXML).toUpper()=="TRUE"?true:false;*/

        

        QDomNodeList childList = iElement.childNodes();
        for(int i = 0; i< childList.count(); i++)
        {
            QDomNode domNode = childList.at(i);
            if(domNode.isElement())
            {
                QDomElement childElem = domNode.toElement();
                if(childElem.nodeName() == XNODE_PARAMETER)
                    param == childElem.text();
                else if(childElem.nodeName() == XNODE_VALUE)
                    paramValue == childElem.text();

                if(! param.isEmpty() && (!paramValue.isEmpty()))
                    break;
            }
        }

        GTAActionCondition * pCondCmd = new GTAActionCondition(actionName,complementName);
		
        pCondCmd->setParameter(param);
        pCondCmd->setValue(paramValue);

        /*pCondCmd->setTimeOut(sTimeOut,sTimeOutUnit);
        pCondCmd->setPrecision(sPrecision,sPrecisionUnit);
        pCondCmd->setActionOnFail(onFail);
       
        pCondCmd->setIgnoreInSCXML(ignoreInSCXML);*/
        getActionAttributes(iElement,pCondCmd);
        opCommand = pCondCmd;
        rc = true;
    }
    return rc;
}
