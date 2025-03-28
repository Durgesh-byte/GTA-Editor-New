#include "GTAActionTitleSerializer.h"
#include "GTAUtil.h"
#include "GTACommentSerializer.h"
#include "GTAActionTitle.h"
#include "GTARequirementSerializer.h"
GTAActionTitleSerializer::GTAActionTitleSerializer() : GTACmdSerializerInterface()
{
    _pActionCmd = NULL;
}
GTAActionTitleSerializer::~GTAActionTitleSerializer()
{
    _pActionCmd = NULL;
}
void GTAActionTitleSerializer::setCommand(const GTACommandBase* pCmd)
{
    _pActionCmd = dynamic_cast<GTAActionTitle*>((GTACommandBase*) pCmd);
}
bool GTAActionTitleSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oElement)
{
    bool rc = false;
    if(_pActionCmd != NULL )
    {
        rc = true;
        oElement  = iDomDoc.createElement(XNODE_ACTION);

        setActionAttributes(_pActionCmd,oElement);



        QString title = _pActionCmd->getTitle();
        //Parameter Node
        QDomElement domTitleElem = iDomDoc.createElement(XNODE_TITLE);
        oElement.appendChild(domTitleElem);
        QDomText domTitleText = iDomDoc.createTextNode(title);
        domTitleElem.appendChild(domTitleText);

        

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
    }
    return rc;
}

bool GTAActionTitleSerializer::deserialize(const QDomElement & iElement, GTACommandBase *& opCommand)
{
    bool rc = false;
    if(!iElement.isNull() && iElement.nodeName() == XNODE_ACTION)
    {

        QString actionName = iElement.attribute(XNODE_NAME);
        QString complement = iElement.attribute(XNODE_COMPLEMENT);

        if(actionName != ACT_TITLE)
            return rc;
        else
            rc = true;



        
        
        GTAActionTitle * pActionCmd = new GTAActionTitle(actionName);
        getActionAttributes(iElement,pActionCmd);
        
        opCommand = pActionCmd;

        QDomNodeList childList = iElement.childNodes();
        for(int i = 0; i < childList.count(); i++)
        {
            QDomNode childNode = childList.at(i);
            if(childNode.isElement())
            {
                QDomElement childElem = childNode.toElement();
                QString nodeName = childElem.nodeName();
                QString valueAttr = childElem.text();

                if(nodeName == XNODE_TITLE)
                {
                    pActionCmd->setTitle(valueAttr);
                }
                else if(nodeName == XNODE_COMMENT)
                {
                    valueAttr = valueAttr.replace("<NEW_LINE/>","\n");
                    valueAttr = valueAttr.remove("\"");
                    pActionCmd->setComment(valueAttr);
                }
                else if(nodeName == XNODE_IMAGE)
                {
                    pActionCmd->setImageName(valueAttr);
                }

            }
        }
        //requiremrnt node
        GTARequirementSerializer requirementSerializer(NULL);
        requirementSerializer.deserialize(iElement,opCommand);
    }
    return rc;
}

#include "GTASerilizerRegisterDef.h"
REGISTER_SERIALIZER(QString("%1").arg(ACT_TITLE),GTAActionTitleSerializer)
