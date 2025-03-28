#include "GTAActionOneClickPPCSerializer.h"
#include "GTAUtil.h"
#include "GTACommentSerializer.h"
#include "GTAActionOneClickPPC.h"
#include "GTARequirementSerializer.h"
GTAActionOneClickPPCSerializer::GTAActionOneClickPPCSerializer():GTACmdSerializerInterface()
{
    _pActionCmd = NULL;
}
void GTAActionOneClickPPCSerializer::setCommand(const GTACommandBase* ipCmd)
{
    _pActionCmd = dynamic_cast<GTAActionOneClickPPC*>((GTACommandBase*)ipCmd);
}
bool GTAActionOneClickPPCSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oElement)
{
    bool rc = false;
    if(_pActionCmd != NULL)
    {
        oElement  = iDomDoc.createElement(XNODE_ACTION);
       /* QString actionName = _pActionCmd->getAction();
        QString complmentName = _pActionCmd->getComplement();
        QString actionOnFail = _pActionCmd->getActionOnFail();
        
        oElement.setAttribute(XNODE_NAME,actionName);
        oElement.setAttribute(XNODE_COMPLEMENT,complmentName);
        oElement.setAttribute(XNODE_ONFAIL,actionOnFail);

        QString hiddenInSCXML = _pActionCmd->isIgnoredInSCXML() ? XNODE_TRUE : XNODE_FALSE;
        oElement.setAttribute(XATTR_IGNORE_SCXML,hiddenInSCXML);*/

        setActionAttributes(_pActionCmd,oElement);
        QDomElement ElemPath = iDomDoc.createElement(XNODE_PATH);
        ElemPath.setAttribute(XNODE_VALUE,_pActionCmd->getPPCPath());
        QString isStart  = _pActionCmd->getIsStart() == true ?XNODE_TRUE:XNODE_FALSE;
        ElemPath.setAttribute(XATTR_ONELCICK_SETENV_START_ENV,isStart);
        oElement.appendChild(ElemPath);

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


bool GTAActionOneClickPPCSerializer::deserialize(const QDomElement & iElement, GTACommandBase *& opCommand)
{
    bool rc = false;
    if(!iElement.isNull())
    {   QString actionName = iElement.attribute(XNODE_NAME);
        QString complementName = iElement.attribute(XNODE_COMPLEMENT);
        //QString sActionOnFail = iElement.attribute(XNODE_ONFAIL);
        GTAActionOneClickPPC * pOneClickPPCCmd = new GTAActionOneClickPPC(actionName,complementName);
       /* bool ignoreInSCXML = iElement.attribute(XATTR_IGNORE_SCXML).toUpper()=="TRUE"?true:false;
        pOneClickPPCCmd->setIgnoreInSCXML(ignoreInSCXML);*/

        getActionAttributes(iElement,pOneClickPPCCmd);


        QDomNodeList childList = iElement.childNodes();
        for(int i = 0; i < childList.count(); i++)
        {
            QDomNode childNode = childList.at(i);
            if(childNode.isElement())
            {
                QDomElement childElem = childNode.toElement();
                QString nodeName = childElem.nodeName();
                if(nodeName == XNODE_PATH)
                {
                    QDomNode PathNode = childElem.attributes().namedItem(XNODE_VALUE);
                    QDomNode isStartNode = childElem.attributes().namedItem(XATTR_ONELCICK_SETENV_START_ENV);
                    if(!PathNode.isNull())
                    {
                        QString val = PathNode.nodeValue();
                        pOneClickPPCCmd->setPPCPath(val);
                    }
                    if(!isStartNode .isNull())
                    {
                        bool isStart = isStartNode.nodeValue() == XNODE_TRUE?true:false;
                        pOneClickPPCCmd->setIsStart(isStart);
                    }
                }
            }
        }

        /*pOneClickPPCCmd->setActionOnFail(sActionOnFail);*/
        opCommand = pOneClickPPCCmd;
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
REGISTER_SERIALIZER(QString("%1 %2").arg(ACT_ONECLICK,COM_ONECLICK_PINPROG),GTAActionOneClickPPCSerializer)
