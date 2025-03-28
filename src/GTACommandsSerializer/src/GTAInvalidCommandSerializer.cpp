#include "GTAInvalidCommandSerializer.h"
#include "GTAUtil.h"
#include "GTACommentSerializer.h"
#include "GTAInvalidCommand.h"
#include "GTARequirementSerializer.h"
GTAInvalidCommandSerializer::GTAInvalidCommandSerializer() : GTACmdSerializerInterface()
{
    _pActionCmd = NULL;
}
GTAInvalidCommandSerializer::~GTAInvalidCommandSerializer()
{
    _pActionCmd = NULL;
}
void GTAInvalidCommandSerializer::setCommand(const GTACommandBase* iPcmd)
{
    _pActionCmd = dynamic_cast<GTAInvalidCommand*>((GTACommandBase*) iPcmd);
}
bool GTAInvalidCommandSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oElement)
{
    bool rc = false;
    if(_pActionCmd != NULL )
    {
        rc = true;

        QString sMsg = _pActionCmd->getMessage();

        oElement  = iDomDoc.createElement(XNODE_ACTION);
        setActionAttributes(_pActionCmd,oElement);

        QDomElement domMsgElement = iDomDoc.createElement(XNODE_MESSAGE);
        QDomText msgTxt = iDomDoc.createTextNode(sMsg);
        domMsgElement.appendChild(msgTxt);
        oElement.appendChild(domMsgElement);
        

        rc = true;
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

    }
    return rc;
}

bool GTAInvalidCommandSerializer::deserialize(const QDomElement & iElement, GTACommandBase *& opCommand)
{
    bool rc = false;
    if(!iElement.isNull() && iElement.nodeName() == XNODE_ACTION)
    {

        QString actionName = iElement.attribute(XNODE_NAME);
        QString complement = iElement.attribute(XNODE_COMPLEMENT);

        if(actionName != ACT_INVALID)
            return rc;
        else
            rc = true;


        GTAInvalidCommand * pActionCmd = new GTAInvalidCommand();

        getActionAttributes(iElement,pActionCmd);
        opCommand = pActionCmd;


        QDomNodeList childList = iElement.childNodes();
        for(int i = 0; i < childList.count(); i++)
        {
            QDomNode childNode = childList.at(i);
            QDomElement childElem = childNode.toElement();
            QString sTagName = childElem.tagName();
            if (sTagName==XNODE_MESSAGE)
            {
                QString sMsg = childElem.text();
                pActionCmd->setMessage(sMsg);
                rc=true;
            }
        }

        //comment node
        GTACommentSerializer commentSerializer(NULL);
        commentSerializer.deserialize(iElement,opCommand);

        //requiremrnt node
        GTARequirementSerializer requirementSerializer(NULL);
        requirementSerializer.deserialize(iElement,opCommand);

    }
    return rc;
}

#include "GTASerilizerRegisterDef.h"
REGISTER_SERIALIZER(QString("%1").arg(ACT_INVALID),GTAInvalidCommandSerializer)
