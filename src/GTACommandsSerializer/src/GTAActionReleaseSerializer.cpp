#include "GTAActionReleaseSerializer.h"
#include "GTAUtil.h"
#include "GTACommentSerializer.h"
#include "GTAActionRelease.h"
#include "GTARequirementSerializer.h"
GTAActionReleaseSerializer::GTAActionReleaseSerializer():GTACmdSerializerInterface()
{
    _pActionCmd = NULL;
}
void GTAActionReleaseSerializer::setCommand(const GTACommandBase* ipCmd)
{
    _pActionCmd = dynamic_cast<GTAActionRelease*>((GTACommandBase*)ipCmd);
}
bool GTAActionReleaseSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oElement)//(QDomDocument &iDomDoc,QDomElement &oElement)
{
    bool rc = false;
    if(_pActionCmd != NULL)
    {
        QString actionName = _pActionCmd->getAction();

        if(actionName != ACT_REL)
            return rc;
        else
            rc = true;
        oElement  = iDomDoc.createElement(XNODE_ACTION);

        //QString complement = _pActionCmd->getComplement();
        //QString onFail =  _pActionCmd->getActionOnFail();
        //QString precision, precisionType;
        //_pActionCmd->getPrecision(precision,precisionType);
        //QString timeOut, timeOutUnit;
        //_pActionCmd->getTimeOut(timeOut,timeOutUnit);

        //oElement.setAttribute(XNODE_NAME,actionName);
        //oElement.setAttribute(XNODE_COMPLEMENT,complement);
        //oElement.setAttribute(XNODE_ONFAIL,onFail);
        //oElement.setAttribute(XNODE_PRECISION,precision);
        //oElement.setAttribute(XNODE_PRECISION_UNIT,precisionType);
        //oElement.setAttribute(XNODE_TIMEOUT,timeOut);
        //oElement.setAttribute(XNODE_TIMEOUT_UNIT,timeOutUnit);

        //QString hiddenInSCXML = _pActionCmd->isIgnoredInSCXML() ? XNODE_TRUE : XNODE_FALSE;
        //oElement.setAttribute(XATTR_IGNORE_SCXML,hiddenInSCXML);
        setActionAttributes(_pActionCmd,oElement);


        QStringList paramList = _pActionCmd->getParameterList();
        foreach(QString parameter, paramList)
        {
            QDomElement ElemOnParam = iDomDoc.createElement(XNODE_PARAMETER);
            ElemOnParam.setAttribute(XNODE_NAME,parameter);
            oElement.appendChild(ElemOnParam);
        }

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

bool GTAActionReleaseSerializer::deserialize(const QDomElement &iElement, GTACommandBase *&opCommand)//(const QDomElement & ioElement)
{
    bool rc = false;
    if(!iElement.isNull())
    {
        QString actionName = iElement.attribute(XNODE_NAME);
        QString compliment = iElement.attribute(XNODE_COMPLEMENT);
        /*QString onFail = iElement.attribute(XNODE_ONFAIL);
        QString precision = iElement.attribute(XNODE_PRECISION);
        QString precisionType = iElement.attribute(XNODE_PRECISION_UNIT);
        QString timeOut = iElement.attribute(XNODE_TIMEOUT);
        QString timeOutUnit = iElement.attribute(XNODE_TIMEOUT_UNIT);
        bool ignoreInSCXML = iElement.attribute(XATTR_IGNORE_SCXML).toUpper()=="TRUE"?true:false;
        */

        GTAActionRelease * pCmd = new GTAActionRelease(actionName,compliment);
        getActionAttributes(iElement,pCmd);
       /* pCmd->setActionOnFail(onFail);
        pCmd->setPrecision(precision,precisionType);
        pCmd->setTimeOut(timeOut,timeOutUnit);
        pCmd->setIgnoreInSCXML(ignoreInSCXML);*/
        QStringList paramList;
        QDomNodeList childList = iElement.childNodes();
        for(int i = 0; i < childList.count(); i++)
        {
            QDomNode childNode = childList.at(i);
            if(childNode.isElement())
            {
                QDomElement ElemOnParam = childNode.toElement();
                if(ElemOnParam.nodeName() == XNODE_PARAMETER)
                    paramList.append(ElemOnParam.attribute(XNODE_NAME));
            }
        }
		pCmd->setParameterList(paramList);
        opCommand = pCmd;
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
REGISTER_SERIALIZER(QString("%1 %2").arg(ACT_REL,COM_REL_PAR),GTAActionReleaseSerializer)
