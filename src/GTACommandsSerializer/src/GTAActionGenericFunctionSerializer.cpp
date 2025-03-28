#include "GTAActionGenericFunctionSerializer.h"
#include "GTAUtil.h"
#include "GTACommentSerializer.h"
#include "GTAActionGenericFunction.h"
#include "GTARequirementSerializer.h"

GTAActionGenericFunctionSerializer::GTAActionGenericFunctionSerializer() : GTACmdSerializerInterface()
{
    _pActionCmd = NULL;
}
void GTAActionGenericFunctionSerializer::setCommand(const GTACommandBase* ipCmd)
{
    _pActionCmd = dynamic_cast<GTAActionGenericFunction*>((GTACommandBase*) ipCmd);
}
bool GTAActionGenericFunctionSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oElement)//(QDomDocument &iDomDoc,QDomElement &oElement)
{
    bool rc = false;
    if(_pActionCmd != NULL)
    {
        QString actionName = _pActionCmd->getAction();

        if(actionName != ACT_GEN_FUNC)
            return rc;
        else
            rc = true;

        QString text  = _pActionCmd->getSCXMLText();
        QDomDocument domDoc;

        rc = domDoc.setContent(text.toLatin1());
        if (!rc)
         return rc;

        oElement  = iDomDoc.createElement(XNODE_ACTION);

        //QString complement = _pActionCmd->getComplement();
        //QString onFail =  _pActionCmd->getActionOnFail();
       
        //QString sTimeOut,sTimeOutUnit,sPrecision,sPrecisionUnit,confTime,confTimeUnit;
        //
        //_pActionCmd->getPrecision(sPrecision,sPrecisionUnit);
        //_pActionCmd->getTimeOut( sTimeOut,sTimeOutUnit);
        //_pActionCmd->getConfCheckTime(confTime,confTimeUnit);


        //
        //oElement.setAttribute(XNODE_NAME,actionName);
        //oElement.setAttribute(XNODE_COMPLEMENT,complement);
        //oElement.setAttribute(XNODE_ONFAIL,onFail);
        //oElement.setAttribute(XNODE_TIMEOUT,sTimeOut);
        //oElement.setAttribute(XNODE_PRECISION,sPrecision);
        //oElement.setAttribute(XNODE_ONFAIL,onFail);
        //oElement.setAttribute(XNODE_TIMEOUT_UNIT,sTimeOutUnit);
        //oElement.setAttribute(XNODE_PRECISION_UNIT,sPrecisionUnit);
        //oElement.setAttribute(XATTR_CONF_TIME,confTime);
        //oElement.setAttribute(XATTR_CONF_UNIT,confTimeUnit);
        //QString hiddenInSCXML = _pActionCmd->isIgnoredInSCXML() ? XNODE_TRUE : XNODE_FALSE;
        //oElement.setAttribute(XATTR_IGNORE_SCXML,hiddenInSCXML);
        setActionAttributes(_pActionCmd,oElement);
        
        QString functionName = _pActionCmd->getFunctionName();

        QDomElement nameElement = iDomDoc.createElement(XNODE_FUNCTION_NAME);
        QDomText nameText = iDomDoc.createTextNode(_pActionCmd->getFunctionName());
        nameElement.appendChild(nameText);
        oElement.appendChild(nameElement);

        QStringList sDumpList = _pActionCmd ->getDumpList();
        if (!sDumpList.isEmpty())
        {
            QDomElement doDumpList = iDomDoc.createElement(XNODE_DUMPLIST);
            oElement.appendChild(doDumpList);
            foreach(QString dumpParameter, sDumpList)
            {
                QDomElement doDumpParamNode = iDomDoc.createElement(XNODE_DUMP_PARAM);
                QDomText domParamValTxt = iDomDoc.createTextNode(dumpParameter);
                doDumpList.appendChild(doDumpParamNode);
                doDumpParamNode.appendChild(domParamValTxt);

            }

        }


        QDomElement scxmlInfoElem = domDoc.documentElement();
        QDomNode scxmlNodeToAppend= iDomDoc.importNode(scxmlInfoElem,true);
        oElement.appendChild(scxmlNodeToAppend);      


       
        //comment node
        GTACommentSerializer commentSerializer(_pActionCmd);
        QDomElement domComment;
        rc = commentSerializer.serialize(iDomDoc,domComment);
        if(rc && (!domComment.isNull()))
        {
            oElement.appendChild(domComment);
        }

        GTARequirementSerializer requirementSerializer(_pActionCmd);
        QDomElement domRequirement;
        rc = requirementSerializer.serialize(iDomDoc,domRequirement);
        if( rc && (!domRequirement.isNull()))
        {
                oElement.appendChild(domRequirement);
        }
       
        rc = true;
    }
    return rc;
}

bool GTAActionGenericFunctionSerializer::deserialize(const QDomElement &iElement, GTACommandBase *&opCommand)//(const QDomElement & ioElement)
{
    bool rc = false;
    if(!iElement.isNull())
    {
        /*QString actionName = iElement.attribute(XNODE_NAME);
        QString onFail = iElement.attribute(XNODE_ONFAIL);

        QString sTimeOut=iElement.attribute(XNODE_TIMEOUT);
        QString sTimeOutUnit=iElement.attribute(XNODE_TIMEOUT_UNIT);
        QString sPrecision=iElement.attribute(XNODE_PRECISION);
        QString sPrecisionUnit=iElement.attribute(XNODE_PRECISION_UNIT);
        QString confTime=iElement.attribute(XATTR_CONF_TIME);
        QString confTimeUnit=iElement.attribute(XATTR_CONF_UNIT);
        bool ignoreInSCXML = iElement.attribute(XATTR_IGNORE_SCXML).toUpper()=="TRUE"?true:false;*/

       

        QString sFunctionName,sSCXMLTxt;
        QStringList dumpList;



        QDomNodeList childList = iElement.childNodes();
        int childNodeSize = childList.count(); 
        for(int i = 0; i< childNodeSize; i++)
        {
            QDomNode domNode = childList.at(i);
            QString nodename = domNode.nodeName();
            if(domNode.isElement())
            {
                // QString nodename = childElem.nodeName();

                QDomElement childElem = domNode.toElement();
                QString textVal = childElem.text();
                QString snodename = childElem.nodeName();
                if (snodename == XNODE_DUMPLIST)
                {

                    QDomNodeList dumpParamNodes = domNode.childNodes();
                    int dumpNodeSize = dumpParamNodes.count();

                    for (int j =0 ;j<dumpNodeSize;j++)
                    {
                        QDomNode dumpNode = dumpParamNodes.at(j);
                        QString sDumpParameter = dumpNode.toElement().text();
                        if (!sDumpParameter.isEmpty())
                        {
                            dumpList.append(sDumpParameter);
                        }
                    }
                }
                else if (nodename == XNODE_FUNCTION_NAME)
                {
                    sFunctionName = childElem.text();
                }
                else if (nodename == "SCXML")
                {
                    QDomDocument doc;
                    QDomNode scxmlNode = doc.importNode(domNode,true);
                    doc.appendChild(scxmlNode);
                    sSCXMLTxt = doc.toString(3);
                }

            }
        }
        GTAActionGenericFunction * pGenFunCmd = new GTAActionGenericFunction();
        getActionAttributes(iElement,pGenFunCmd);
        /*pGenFunCmd->setAction(ACT_GEN_FUNC);

        pGenFunCmd->setActionOnFail(onFail);
        pGenFunCmd->setPrecision(sPrecision,sPrecisionUnit);
        pGenFunCmd->setConfCheckTime(confTime,confTimeUnit);
        pGenFunCmd->setTimeOut(sTimeOut,sTimeOutUnit);*/
        pGenFunCmd->setFunctionName(sFunctionName);
        pGenFunCmd->setSCXMLText(sSCXMLTxt);
        pGenFunCmd->setDumpList(dumpList);
        //pGenFunCmd->setIgnoreInSCXML(ignoreInSCXML);
       
        opCommand = pGenFunCmd;

        //comment node
        GTACommentSerializer commentSerializer(NULL);
        commentSerializer.deserialize(iElement,opCommand);

        //requiremrnt node
        GTARequirementSerializer requirementSerializer(NULL);
        requirementSerializer.deserialize(iElement,opCommand);

        rc = true;
    }
    return rc;

}

#include "GTASerilizerRegisterDef.h"
REGISTER_SERIALIZER(QString("%1").arg(ACT_GEN_FUNC),GTAActionGenericFunctionSerializer)
