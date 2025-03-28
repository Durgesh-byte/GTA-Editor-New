#include "GTAActionOneClickTitleSerializer.h"
#include "GTAUtil.h"
#include "GTACommentSerializer.h"
#include "GTAActionOneClickTitle.h"
#include "GTARequirementSerializer.h"
GTAActionOneClickTitleSerializer::GTAActionOneClickTitleSerializer() : GTACmdSerializerInterface()
{
    _pActionCmd = NULL;
}
void GTAActionOneClickTitleSerializer::setCommand(const GTACommandBase* ipCmd)
{
    _pActionCmd = dynamic_cast<GTAActionOneClickTitle*>((GTACommandBase*)ipCmd);
}
bool GTAActionOneClickTitleSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oElement)
{
    bool rc = false;
    if(_pActionCmd != NULL )
    {
        rc = true;
        oElement  = iDomDoc.createElement(XNODE_ACTION);
        //QString actionName = _pActionCmd->getAction();
        //QString complement = _pActionCmd->getComplement();

        //QString sTimeOut,sTimeOutUnit,sPrecision,sPrecisionUnit;
        //_pActionCmd->getPrecision(sPrecision,sPrecisionUnit);
        //_pActionCmd->getTimeOut( sTimeOut,sTimeOutUnit);
        //QString onFail = _pActionCmd->getActionOnFail();

        
        //oElement.setAttribute(XNODE_NAME,actionName);
        //oElement.setAttribute(XNODE_COMPLEMENT,complement);
        //oElement.setAttribute(XNODE_TIMEOUT,sTimeOut);
        //oElement.setAttribute(XNODE_ONFAIL,onFail);
        //oElement.setAttribute(XNODE_PRECISION,sPrecision);
        //oElement.setAttribute(XNODE_TIMEOUT_UNIT,sTimeOutUnit);
        //oElement.setAttribute(XNODE_PRECISION_UNIT,sPrecisionUnit);

        //QString hiddenInSCXML = _pActionCmd->isIgnoredInSCXML() ? XNODE_TRUE : XNODE_FALSE;
        //oElement.setAttribute(XATTR_IGNORE_SCXML,hiddenInSCXML);
        setActionAttributes(_pActionCmd,oElement);



        QString title = _pActionCmd->getTitle();
        //Parameter Node
        QDomElement domTitleElem = iDomDoc.createElement(XNODE_TITLE);
        oElement.appendChild(domTitleElem);
        QDomText domTitleText = iDomDoc.createTextNode(title);
        domTitleElem.appendChild(domTitleText);

        //        QDomElement domCommentElem = iDomDoc.createElement(XNODE_PARAMETER);
        //        oElement.appendChild(domCommentElem);
        //        domCommentElem.setAttribute(XNODE_NAME,XNODE_COMMENT);
        //        domCommentElem.setAttribute(XNODE_VALUE,comment);

        //        QDomElement domImgElem = iDomDoc.createElement(XNODE_PARAMETER);
        //        oElement.appendChild(domImgElem);
        //        domImgElem.setAttribute(XNODE_NAME,XNODE_IMAGE);
        //        domImgElem.setAttribute(XNODE_VALUE,image);

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

bool GTAActionOneClickTitleSerializer::deserialize(const QDomElement & iElement, GTACommandBase *& opCommand)
{
    bool rc = false;
    if(!iElement.isNull() && iElement.nodeName() == XNODE_ACTION)
    {

        QString actionName = iElement.attribute(XNODE_NAME);
        QString complement = iElement.attribute(XNODE_COMPLEMENT);

        if(actionName != ACT_ONECLICK || complement!=COM_ONECLICK_TITLE)
            return rc;
        else
            rc = true;

        /*QString onFail = iElement.attribute(XNODE_ONFAIL);
        QString timeOut = iElement.attribute(XNODE_TIMEOUT);
        QString precision = iElement.attribute(XNODE_PRECISION);
        QString sTimeOutUnit = iElement.attribute(XNODE_TIMEOUT_UNIT);
        QString sPrecisionUnit = iElement.attribute(XNODE_PRECISION_UNIT);
        bool ignoreInSCXML = iElement.attribute(XATTR_IGNORE_SCXML).toUpper()=="TRUE"?true:false;
      */  
        GTAActionOneClickTitle * pActionCmd = new GTAActionOneClickTitle(actionName,complement);
        /*pActionCmd->setComplement(complement);
        pActionCmd->setActionOnFail(onFail);
        pActionCmd->setTimeOut(timeOut,sTimeOutUnit);
        pActionCmd->setPrecision(precision,sPrecisionUnit);
        pActionCmd->setIgnoreInSCXML(ignoreInSCXML);
        pActionCmd->setIgnoreInSCXML(ignoreInSCXML);*/
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
                    valueAttr = valueAttr.remove("\"");
                    pActionCmd->setComment(valueAttr);
                }
                else if(nodeName == XNODE_IMAGE)
                {
                    pActionCmd->setImageName(valueAttr);
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
REGISTER_SERIALIZER(QString("%1 %2").arg(ACT_ONECLICK,COM_ONECLICK_TITLE),GTAActionOneClickTitleSerializer)
