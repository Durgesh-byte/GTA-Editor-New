#include "GTAActionElseSerializer.h"
#include "GTAUtil.h"
#include "GTACommentSerializer.h"
#include "GTAActionElse.h"
#include "GTARequirementSerializer.h"

GTAActionElseSerializer::GTAActionElseSerializer()
{
    _pActionCmd = NULL;
}

GTAActionElseSerializer::~GTAActionElseSerializer()
{
	_pActionCmd = NULL;
}

void GTAActionElseSerializer::setCommand(const GTACommandBase* pCmd)
{
	_pActionCmd = dynamic_cast<GTAActionElse*>((GTACommandBase*) pCmd);
}

bool GTAActionElseSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oElement)
{
    bool rc = false;
    if(_pActionCmd != NULL)
    {

        oElement  = iDomDoc.createElement(XNODE_ACTION);
        setActionAttributes(_pActionCmd,oElement);

        //QString actionName = _pActionCmd->getAction();
        //QString complmentName = _pActionCmd->getComplement();
        //oElement.setAttribute(XNODE_NAME,actionName);
        //oElement.setAttribute(XNODE_COMPLEMENT,complmentName);
        //QString hiddenInSCXML = _pActionCmd->isIgnoredInSCXML() ? XNODE_TRUE : XNODE_FALSE;
        //oElement.setAttribute(XATTR_IGNORE_SCXML,hiddenInSCXML);

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


bool GTAActionElseSerializer::deserialize(const QDomElement & iElement, GTACommandBase *& opCommand)
{
    bool rc = false;
    if(!iElement.isNull())
    {   QString actionName = iElement.attribute(XNODE_NAME);
        QString complementName = iElement.attribute(XNODE_COMPLEMENT);
        /*bool ignoreInSCXML = iElement.attribute(XATTR_IGNORE_SCXML).toUpper()=="TRUE"?true:false;*/

        
        GTAActionElse * pCondCmd = new GTAActionElse(actionName,complementName);
        opCommand = pCondCmd;
       /* opCommand->setIgnoreInSCXML(ignoreInSCXML);*/
        getActionAttributes(iElement,pCondCmd);

        QDomNodeList childList = iElement.childNodes();
        for(int i = 0; i < childList.count(); i++)
        {
            QDomNode childNode = childList.at(i);
            if(childNode.isElement())
            {
                QDomElement childElem = childNode.toElement();
                QString nodeName = childElem.nodeName();
                QString nameAttr = childElem.attribute(XNODE_NAME);
                QString valueAttr = childElem.attribute(XNODE_VALUE);
                if(nodeName == XNODE_PARAMETER)
                {
                    if(nameAttr == XNODE_COMMENT)
                    {
                        pCondCmd->setComment(valueAttr);
                    }
                }
            }
        }

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
REGISTER_SERIALIZER(QString("%1 %2").arg(ACT_CONDITION,COM_CONDITION_ELSE),GTAActionElseSerializer)
