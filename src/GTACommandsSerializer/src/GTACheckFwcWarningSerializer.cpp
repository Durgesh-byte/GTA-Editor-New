#include "GTACheckFwcWarningSerializer.h"
#include "GTAUtil.h"
#include "GTACommands.h"
#include "GTACheckBase.h"
#include "GTACommentSerializer.h"
#include "GTARequirementSerializer.h"
#include "GTACheckFwcWarning.h"
GTACheckFwcWarningSerializer::GTACheckFwcWarningSerializer() : GTACmdSerializerInterface()
{
    _pFWCWarning = NULL;
}
GTACheckFwcWarningSerializer::~GTACheckFwcWarningSerializer()
{
    _pFWCWarning = NULL;
}

void GTACheckFwcWarningSerializer::setCommand(const GTACommandBase* iPCmd)
{
    _pFWCWarning = dynamic_cast<GTACheckFwcWarning*>((GTACommandBase*) iPCmd);
}
bool GTACheckFwcWarningSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oElement)//(QDomDocument &iDomDoc,QDomElement &oElement)
{
    bool rc = false;
    if(_pFWCWarning != NULL)
    {

        QString sTimeOut , sTimeUnit;
        _pFWCWarning->getTimeOut(sTimeOut,sTimeUnit) ;

        QString sPrecision,sPrecisionUnit,ConfTime,ConfUnit;
        _pFWCWarning->getPrecision(sPrecision,sPrecisionUnit) ;
        _pFWCWarning->getConfCheckTime(ConfTime,ConfUnit);
        QString sActionOnFail =_pFWCWarning->getActionOnFail() ;
        QStringList sDumpList = _pFWCWarning ->getDumpList();



        QString sfwcMessg = _pFWCWarning->getMessage() ;
        QString sfwcMsgCondition = _pFWCWarning->getCondition() ;
        QString sfwcMsgOnReport = _pFWCWarning->getOnDisplay() ;
        QString sWaitForAcStatus = "FALSE";
        QString sHeader = _pFWCWarning->getHeader();
        QString sWaitForBuffDepth = _pFWCWarning->getWaitForBuffer();
        QString sColor = _pFWCWarning->getFWCWarningColor();

        bool sWaitforAck = _pFWCWarning->getWaitAck();


        if (sWaitforAck)
            sWaitForAcStatus = "TRUE";

        oElement  = iDomDoc.createElement(XNODE_CHECK);
        oElement.setAttribute(XNODE_NAME,CHK_FWC_WARN);

        setActionAttributes(_pFWCWarning,oElement);



        QDomElement domMsgElement = iDomDoc.createElement(XNODE_PARAMETER);
        domMsgElement.setAttribute(XNODE_NAME,"MESSAGE");
        domMsgElement.setAttribute(XNODE_CONDITION,sfwcMsgCondition);
        domMsgElement.setAttribute(XNODE_VALUE,sfwcMessg);
        oElement.appendChild(domMsgElement);

        QDomElement domMsgRprtElement = iDomDoc.createElement(XNODE_DISPLAYON);
        QDomText msgTxtRprt = iDomDoc.createTextNode(sfwcMsgOnReport);
        domMsgRprtElement.appendChild(msgTxtRprt);
        oElement.appendChild(domMsgRprtElement);

        QDomElement domWaitForActElem = iDomDoc.createElement(XNODE_ACKNOWLEDGEMENT);
        domWaitForActElem.setAttribute(XNODE_VALUE,sWaitForAcStatus);
        oElement.appendChild(domWaitForActElem);


        QDomElement domWaitBufferDepthElem = iDomDoc.createElement(XNODE_WAIT_BUFFER_DEPTH);
        QDomText buffDepthTxtN= iDomDoc.createTextNode(sWaitForBuffDepth);
        domWaitBufferDepthElem.appendChild(buffDepthTxtN);
        oElement.appendChild(domWaitBufferDepthElem);


        QDomElement domFwcHdrElem = iDomDoc.createElement(XNODE_FWC_WARN_HEADER);
        QDomText buffHdrTxtN= iDomDoc.createTextNode(sHeader);
        domFwcHdrElem.appendChild(buffHdrTxtN);
        oElement.appendChild(domFwcHdrElem);


        QDomElement domFwcColorElem = iDomDoc.createElement(XNODE_FWC_WARN_COLOR);
        QDomText buffColorTxtN= iDomDoc.createTextNode(sColor);
        domFwcColorElem.appendChild(buffColorTxtN);
        oElement.appendChild(domFwcColorElem);

        if (sActionOnFail == ACT_FAIL_STOP && !sDumpList.isEmpty())
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

        //comment node
        GTACommentSerializer commentSerializer(_pFWCWarning);
        QDomElement domComment;
        rc = commentSerializer.serialize(iDomDoc,domComment);
        if(rc && (!domComment.isNull()))
        {
            oElement.appendChild(domComment);
        }

        GTARequirementSerializer requirementSerializer(_pFWCWarning);
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

bool GTACheckFwcWarningSerializer::deserialize(const QDomElement &iElement, GTACommandBase *&opCommand)//(const QDomElement & ioElement)
{
    bool rc = false;
    if(!iElement.isNull())
    {
        QString actionName = iElement.attribute(XNODE_CHECK);
        QString sOnFail = iElement.attribute(XNODE_ONFAIL);
        QString sPrecisoin = iElement.attribute(XNODE_PRECISION);
        QString sTimeOut = iElement.attribute(XNODE_TIMEOUT);
        QString sTimeOutUnit= iElement.attribute(XNODE_TIMEOUT_UNIT);;
        QString sPrecisoinUnit= iElement.attribute(XNODE_PRECISION_UNIT);
        QStringList dumpList ;
        QString ConfTime = iElement.attribute(XATTR_CONF_TIME);
        QString ConfUnit = iElement.attribute(XATTR_CONF_UNIT);
        // bool ignoreInSCXML = iElement.attribute(XATTR_IGNORE_SCXML).toUpper()=="TRUE"?true:false;
        

        GTACheckFwcWarning* pFWCWarning = new GTACheckFwcWarning;
        
        getActionAttributes(iElement,pFWCWarning);

        QDomNodeList childList = iElement.childNodes();
        for(int i = 0; i < childList.count(); i++)
        {
            QDomNode childNode = childList.at(i);
            QDomElement childElem = childNode.toElement();
            QString sTagName = childElem.tagName();

            if (sTagName==XNODE_PARAMETER)
            {
                QString sMsg = childElem.text();
                QDomAttr domValAttr = childElem.attributeNode(XNODE_VALUE);
                pFWCWarning->setMessage(domValAttr.value());

                QDomAttr domCondtionAttr = childElem.attributeNode(XNODE_CONDITION);
                pFWCWarning->setCondition(domCondtionAttr.value());
            }
            else if (sTagName==XNODE_DISPLAYON)
            {
                QString sMsg = childElem.text();
                pFWCWarning->setOnDisplay(sMsg);
            }
            else if (sTagName == XNODE_ACKNOWLEDGEMENT)
            {
                QString sMsg = childElem.attribute(XNODE_VALUE);
                if (sMsg.toUpper()=="TRUE")
                    pFWCWarning->setWaitAck(true);
                else
                    pFWCWarning->setWaitAck(false);
            }
            else if (sTagName==XNODE_WAIT_BUFFER_DEPTH)
            {
                QString sMsg = childElem.text();
                pFWCWarning->setWaitForBuffer(sMsg);
            }
            else if (sTagName==XNODE_FWC_WARN_HEADER)
            {
                QString sMsg = childElem.text();
                pFWCWarning->setHeader(sMsg);
            }
            else if (sTagName==XNODE_FWC_WARN_COLOR)
            {
                QString sColor = childElem.text();
                pFWCWarning->setFWCWarningColor(sColor);
            }
            else if (sTagName == XNODE_DUMPLIST && sOnFail == ACT_FAIL_STOP)
            {

                QDomNodeList dumpParamNodes = childNode.childNodes();
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

                if (!dumpList.isEmpty())
                    pFWCWarning->setDumpList(dumpList);
            }
        }

        opCommand=pFWCWarning;

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
REGISTER_SERIALIZER(QString("%1").arg(CHK_FWC_WARN),GTACheckFwcWarningSerializer)

