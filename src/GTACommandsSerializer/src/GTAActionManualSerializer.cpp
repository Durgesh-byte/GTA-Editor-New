#include "GTAActionManualSerializer.h"
#include "GTAUtil.h"
#include "GTACommentSerializer.h"
#include "GTAActionManual.h"
#include "GTARequirementSerializer.h"

#pragma warning(push, 0)
#include <QMimeData>
#include <QTextCodec>
#include <QApplication>
#include <QTextEdit>
#pragma warning(pop)

GTAActionManualSerializer::GTAActionManualSerializer() : GTACmdSerializerInterface()
{
    _pActionCmd = NULL;
}
void GTAActionManualSerializer::setCommand(const GTACommandBase* ipCmd)
{
    _pActionCmd = dynamic_cast<GTAActionManual*>((GTACommandBase*)ipCmd);
}
bool GTAActionManualSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oElement)//(QDomDocument &iDomDoc,QDomElement &oElement)
{
    bool rc = false;
    if(_pActionCmd != NULL)
    {
        QString actionName = _pActionCmd->getAction();

        if(actionName != ACT_MANUAL)
            return rc;
        else
            rc = true;

        
        QString machine = _pActionCmd->getTargetMachine();
        QString message= _pActionCmd->getMessage();

        message.replace("\n","<NEW_LINE/>");

        QString htmlMessage = _pActionCmd->getHtmlMessage();

        QString acknowledgement = _pActionCmd->getAcknowledgetment() ? XNODE_TRUE : XNODE_FALSE;
        QString userFeedback = _pActionCmd->getUserFeedback() ? XNODE_TRUE : XNODE_FALSE;
        QString returnVal = _pActionCmd->getReturnType();
        QString statement = _pActionCmd->getStatement();
        QString isParamLocal = _pActionCmd->IsLocalParameter() ? XNODE_TRUE : XNODE_FALSE;
        QString paramName = _pActionCmd->getParameter();

        oElement  = iDomDoc.createElement(XNODE_ACTION);
        
        //QString complement = _pActionCmd->getComplement();
        //QString onFail =  _pActionCmd->getActionOnFail();
        //QString hiddenInSCXML = _pActionCmd->isIgnoredInSCXML() ? XNODE_TRUE : XNODE_FALSE;

        //oElement.setAttribute(XNODE_NAME,actionName);
        //oElement.setAttribute(XNODE_COMPLEMENT,complement);
        //oElement.setAttribute(XNODE_ONFAIL,onFail);
        //oElement.setAttribute(XATTR_IGNORE_SCXML,hiddenInSCXML);
        setActionAttributes(_pActionCmd,oElement);

        //        oElement.setAttribute(XNODE_ORDER,order);

        QDomElement domMachine = iDomDoc.createElement(XNODE_MACHINE);
        QDomText machineTxtNode = iDomDoc.createTextNode(machine);
        domMachine.appendChild(machineTxtNode);
        oElement.appendChild(domMachine);

        QDomElement domMsg = iDomDoc.createElement(XNODE_MESSAGE);
        QDomText msgTxtNode = iDomDoc.createTextNode(message);
        domMsg.appendChild(msgTxtNode);
        oElement.appendChild(domMsg);


        QDomElement domHtmlMsg = iDomDoc.createElement("HTML_MESSAGE");
        QDomText msgHtmlNode = iDomDoc.createTextNode(htmlMessage.toLatin1());
        domHtmlMsg.appendChild(msgHtmlNode);
        oElement.appendChild(domHtmlMsg);

        QDomElement domAckno = iDomDoc.createElement(XNODE_ACKNOWLEDGEMENT);
        domAckno.setAttribute(XNODE_VALUE,acknowledgement);
        //        QDomText acknoTxtNode = iDomDoc.createTextNode(acknowledgement);
        //        domAckno.appendChild(acknoTxtNode);
        oElement.appendChild(domAckno);

        QDomElement domFeedback = iDomDoc.createElement(XNODE_FEEDBACK);
        domFeedback.setAttribute(XNODE_VALUE,userFeedback);
        //        QDomText feedbackTxtNode = iDomDoc.createTextNode(userFeedback);
        //        domFeedback.appendChild(feedbackTxtNode);
        oElement.appendChild(domFeedback);

        QDomElement domRetVal = iDomDoc.createElement(XNODE_RETURNTYPE);
        QDomText retValTxtNode = iDomDoc.createTextNode(returnVal);
        domRetVal.appendChild(retValTxtNode);
        domFeedback.appendChild(domRetVal);

        QDomElement domParam = iDomDoc.createElement(XNODE_PARAMETER);
        domParam.setAttribute(XNODE_ISLOCAL,isParamLocal);
        QDomText paramTxtNode = iDomDoc.createTextNode(paramName);
        domParam.appendChild(paramTxtNode);
        domFeedback.appendChild(domParam);

        //        QDomElement domStatement = iDomDoc.createElement(XNODE_STATEMENT);
        //        QDomText statementTxtNode = iDomDoc.createTextNode(statement);
        //        domStatement.appendChild(statementTxtNode);
        //        oElement.appendChild(domStatement);

        //comment node
        GTACommentSerializer commentSerializer(_pActionCmd);
        QDomElement domComment;
        rc = commentSerializer.serialize(iDomDoc,domComment);
        if(rc && (!domComment.isNull()))
        {
            oElement.appendChild(domComment);
        }

        QString imageName = _pActionCmd->getImageName();
        QDomElement domImgElem = iDomDoc.createElement(XNODE_IMAGE);
        QDomText domImgText = iDomDoc.createTextNode(imageName);
        domImgElem.appendChild(domImgText);
        oElement.appendChild(domImgElem);

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

bool GTAActionManualSerializer::deserialize(const QDomElement &iElement, GTACommandBase *&opCommand)
{
    bool rc = false;
    if(!iElement.isNull())
    {
        QString actionName = iElement.attribute(XNODE_NAME);
        QString targetMachine;
        QString userMessage;
        QString htmlMessage;
        bool acknowledgement = false;
        bool userFeedback = false;
        QString param;
        bool isLocParam = false;
        QString retVal;
        QString image;

        //TODO: What if node is missing? Needs fail mechanism
        QDomNodeList childList = iElement.childNodes();
        for(int i = 0; i < childList.count(); i++)
        {
            QDomNode childNode = childList.at(i);
            if(childNode.isElement())
            {
                QDomElement childElem = childNode.toElement();
                QString nodeName = childElem.nodeName();
                QString nodeText = childElem.text();
                if(nodeName == XNODE_MACHINE)
                {
                    targetMachine = nodeText;
                }
                else if(nodeName == XNODE_MESSAGE)
                {
                    userMessage = nodeText;
                    userMessage.replace("<NEW_LINE/>","\n");
                }
                else if(nodeName == "HTML_MESSAGE")
                {
                    QString arr = childElem.firstChild().toText().data();
                    QTextCodec *codec = Qt::codecForHtml(arr.toLatin1());
                    QString str = codec->toUnicode(arr.toLatin1());
                    if (Qt::mightBeRichText(str)) 
                    {
                        htmlMessage = str;
                    } 
                    else
                    {
                        htmlMessage = nodeText;
                    }
                }
                else if(nodeName == XNODE_ACKNOWLEDGEMENT)
                {
                    acknowledgement = childElem.attribute(XNODE_VALUE).toUpper() == "TRUE" ? true : false;
                }
                else if(nodeName == XNODE_FEEDBACK)
                {
                    userFeedback = childElem.attribute(XNODE_VALUE).toUpper() == "TRUE" ? true : false;
                    QDomNodeList feedbackChildList = childElem.childNodes();
                    for(int j = 0; j < feedbackChildList.count(); j++)
                    {
                        QDomNode feedbackChildNode = feedbackChildList.at(j);
                        if(feedbackChildNode.isElement())
                        {
                            QDomElement feedbackChildElem = feedbackChildNode.toElement();
                            if(feedbackChildElem.nodeName() == XNODE_PARAMETER)
                            {
                                param = feedbackChildElem.text();
                                isLocParam = feedbackChildElem.attribute(XNODE_ISLOCAL).toUpper() == "TRUE" ? true: false;
                            }
                            else if(feedbackChildElem.nodeName() == XNODE_RETURNTYPE)
                            {
                                retVal = feedbackChildElem.text();
                            }
                        }
                    }
                }
                else if(nodeName == XNODE_IMAGE)
                {
                    image = childElem.text();
                }
            }
        }
        GTAActionManual * pManualCmd = new GTAActionManual(actionName);
        getActionAttributes(iElement,pManualCmd);

        pManualCmd->setTargetMachine(targetMachine);
        pManualCmd->setMessage(userMessage);
        pManualCmd->setAcknowledgement(acknowledgement);
        pManualCmd->setUserFeedback(userFeedback);
        pManualCmd->setParameter(param);
        pManualCmd->setParameterType(isLocParam);
        pManualCmd->setReturnType(retVal);
        pManualCmd->setImageName(image);
        pManualCmd->setHTMLMessage(htmlMessage);
        opCommand = pManualCmd;

        GTACommentSerializer commentSerializer(NULL);
        commentSerializer.deserialize(iElement,opCommand);

        GTARequirementSerializer requirementSerializer(NULL);
        requirementSerializer.deserialize(iElement,opCommand);

        rc = true;
    }
    return rc;

}

#include "GTASerilizerRegisterDef.h"
REGISTER_SERIALIZER(QString("%1").arg(ACT_MANUAL),GTAActionManualSerializer)
