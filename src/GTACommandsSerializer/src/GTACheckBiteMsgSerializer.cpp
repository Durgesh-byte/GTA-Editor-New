#include "GTACheckBiteMsgSerializer.h"
#include "GTAUtil.h"
#include "GTACommands.h"
#include "GTACheckBase.h"
#include "GTACommentSerializer.h"
#include "GTACheckBiteMessage.h"
#include "GTARequirementSerializer.h"

GTACheckBiteMsgSerializer::GTACheckBiteMsgSerializer() : GTACmdSerializerInterface()
{
    _pBiteMsgCmd = NULL;
}

GTACheckBiteMsgSerializer::~GTACheckBiteMsgSerializer()
{
    _pBiteMsgCmd = NULL;
}

void GTACheckBiteMsgSerializer::setCommand(const GTACommandBase* iPCmd)
{
    _pBiteMsgCmd = dynamic_cast<GTACheckBiteMessage*>((GTACommandBase*) iPCmd);
}

bool GTACheckBiteMsgSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oElement)//(QDomDocument &iDomDoc,QDomElement &oElement)
{
    bool rc = false;
    if(_pBiteMsgCmd != NULL)
    {

        QString sTimeOut , sTimeUnit;
        _pBiteMsgCmd->getTimeOut(sTimeOut,sTimeUnit) ;

        QString sPrecision,sPrecisionUnit;
        _pBiteMsgCmd->getPrecision(sPrecision,sPrecisionUnit) ;
        QString sActionOnFail =_pBiteMsgCmd->getActionOnFail() ;



        QString byteMessg = _pBiteMsgCmd->getMessage() ;
        QString byteMsgCondition = _pBiteMsgCmd->getCondition() ;
        QString byteMsgOnReport = _pBiteMsgCmd->getOnReport() ;
        QString sWaitForAcStatus = "FALSE";
        QString sWaitForBuffDepth = _pBiteMsgCmd->getBuffDepth();

        bool sWaitforAck = _pBiteMsgCmd->getWaitStatus();
        if (sWaitforAck)
            sWaitForAcStatus = "TRUE";
        
        QStringList sDumpList = _pBiteMsgCmd ->getDumpList();

        oElement  = iDomDoc.createElement(XNODE_CHECK);
        oElement.setAttribute(XNODE_NAME,CHK_BITE_MSG);

        setActionAttributes(_pBiteMsgCmd,oElement);



        QDomElement domMsgElement = iDomDoc.createElement(XNODE_PARAMETER);
        domMsgElement.setAttribute(XNODE_NAME,"MESSAGE");
        domMsgElement.setAttribute(XNODE_CONDITION,byteMsgCondition);
        domMsgElement.setAttribute(XNODE_VALUE,byteMessg);
        oElement.appendChild(domMsgElement);

        QDomElement domMsgRprtElement = iDomDoc.createElement(XNODE_REPORTON);
        QDomText msgTxtRprt = iDomDoc.createTextNode(byteMsgOnReport);
        domMsgRprtElement.appendChild(msgTxtRprt);
        oElement.appendChild(domMsgRprtElement);

        QDomElement domWaitForActElem = iDomDoc.createElement(XNODE_ACKNOWLEDGEMENT);
        domWaitForActElem.setAttribute(XNODE_VALUE,sWaitForAcStatus);
        oElement.appendChild(domWaitForActElem);


        QDomElement domWaitBufferDepthElem = iDomDoc.createElement(XNODE_WAIT_BUFFER_DEPTH);
        QDomText buffDepthTxtN= iDomDoc.createTextNode(sWaitForBuffDepth);
        domWaitBufferDepthElem.appendChild(buffDepthTxtN);
        oElement.appendChild(domWaitBufferDepthElem);

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

        //comment node
        GTACommentSerializer commentSerializer(_pBiteMsgCmd);
        QDomElement domComment;
        rc = commentSerializer.serialize(iDomDoc,domComment);
        if(rc && (!domComment.isNull()))
        {
            oElement.appendChild(domComment);
        }

        GTARequirementSerializer requirementSerializer(_pBiteMsgCmd);
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

bool GTACheckBiteMsgSerializer::deserialize(const QDomElement &iElement, GTACommandBase *&opCommand)//(const QDomElement & ioElement)
{
    bool rc = false;
    if(!iElement.isNull())
    {
        QString actionName = iElement.attribute(XNODE_CHECK);

        QStringList dumpList;


        GTACheckBiteMessage* pChkBtMsg = new GTACheckBiteMessage;
        getActionAttributes(iElement,pChkBtMsg);


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
                pChkBtMsg->setMessage(domValAttr.value());

                QDomAttr domCondtionAttr = childElem.attributeNode(XNODE_CONDITION);
                pChkBtMsg->setCondition(domCondtionAttr.value());
            }
            else if (sTagName==XNODE_REPORTON)
            {
                QString sMsg = childElem.text();
                pChkBtMsg->setOnReport(sMsg);
            }
            else if (sTagName == XNODE_ACKNOWLEDGEMENT)
            {
                QString sMsg = childElem.attribute(XNODE_VALUE);
                if (sMsg.toUpper()=="TRUE")
                    pChkBtMsg->setWaitStatus(true);
                else
                    pChkBtMsg->setWaitStatus(false);
            }
            else if (sTagName==XNODE_WAIT_BUFFER_DEPTH)
            {
                QString sMsg = childElem.text();
                pChkBtMsg->setBuffDepth(sMsg);
            }else if (sTagName == XNODE_DUMPLIST)
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
                    pChkBtMsg->setDumpList(dumpList);
            }
            

        }
	
	

        opCommand=pChkBtMsg;

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
REGISTER_SERIALIZER(QString("%1").arg(CHK_BITE_MSG),GTACheckBiteMsgSerializer)

