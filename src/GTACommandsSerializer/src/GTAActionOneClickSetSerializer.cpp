#include "GTAActionOneClickSetSerializer.h"
#include "GTAUtil.h"
#include "GTAEquationSerializer.h"
#include "GTACommentSerializer.h"
#include "GTAActionOneClickSet.h"
#include "GTARequirementSerializer.h"
GTAActionOneClickSetSerializer::GTAActionOneClickSetSerializer():GTACmdSerializerInterface()
{
    _pActionCmd = NULL;
}
void GTAActionOneClickSetSerializer::setCommand(const GTACommandBase* ipCmd)
{
    _pActionCmd = dynamic_cast<GTAActionOneClickSet*>((GTACommandBase*)ipCmd);

}
bool GTAActionOneClickSetSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oElement)
{
    bool rc = false;
    if(_pActionCmd != NULL )
    {
        QString actionName = _pActionCmd->getAction();
        QString complement = _pActionCmd->getComplement();
        QString paramName = _pActionCmd->getParameter();
        QString onFail = _pActionCmd->getActionOnFail();
        QString sFunctionStatus = _pActionCmd->getFunctionStatus();

        GTAEquationBase * pEquation = _pActionCmd->getEquation();


        QString sTimeOut,sTimeOutUnit,sPrecision,sPrecisionUnit;
        _pActionCmd->getPrecision(sPrecision,sPrecisionUnit);
        _pActionCmd->getTimeOut( sTimeOut,sTimeOutUnit);

        oElement  = iDomDoc.createElement(XNODE_SET_INSTANCE);
        oElement.setAttribute(XNODE_NAME,actionName);
        oElement.setAttribute(XNODE_ONFAIL,onFail);
        
        /*oElement.setAttribute(XNODE_COMPLEMENT,complement);
        oElement.setAttribute(XNODE_TIMEOUT,sTimeOut);
        oElement.setAttribute(XNODE_ONFAIL,onFail);
        oElement.setAttribute(XNODE_PRECISION,sPrecision);
  oElement.setAttribute(XNODE_TIMEOUT_UNIT,sTimeOutUnit);
  oElement.setAttribute(XNODE_PRECISION_UNIT,sPrecisionUnit);*/

        //Parameter Node
        QDomElement domParam = iDomDoc.createElement(XNODE_PARAMETER);
        oElement.appendChild(domParam);
        domParam.setAttribute(XNODE_NAME, paramName);
        domParam.setAttribute(XNODE_VALUE,XNODE_EQUATION);
        domParam.setAttribute(XNODE_FS,sFunctionStatus);
        QString isSetOnlyFS = _pActionCmd->getIsSetOnlyFSFixed() ? "TRUE" : "FALSE";
        domParam.setAttribute(XATTR_ISONLYFS_SET_FIXED,isSetOnlyFS);

        QString hiddenInSCXML = _pActionCmd->isIgnoredInSCXML() ? XNODE_TRUE : XNODE_FALSE;
        oElement.setAttribute(XATTR_IGNORE_SCXML,hiddenInSCXML);



        GTAEquationSerializer equationSerializer(pEquation);
        QDomElement domEquation;
        rc = equationSerializer.serialize(iDomDoc,domEquation);
        if(rc && (! domEquation.isNull()))
            oElement.appendChild(domEquation);

        /*  QDomElement domVmaceForce = iDomDoc.createElement(XNODE_PARAMETER);
        oElement.appendChild(domVmaceForce);
        domVmaceForce.setAttribute(XNODE_NAME,XNODE_VMACFORCETYPE);
        domVmaceForce.setAttribute(XNODE_VALUE,vmacForceType);*/

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

bool GTAActionOneClickSetSerializer::deserialize(const QDomElement & iElement, GTACommandBase *& opCommand)
{
    bool rc = false;
    if(!iElement.isNull() )
    {
        QString actionName = iElement.attribute(XNODE_NAME);
        /*QString onFail = iElement.attribute(XNODE_ONFAIL);
         bool ignoreInSCXML = iElement.attribute(XATTR_IGNORE_SCXML).toUpper()=="TRUE"?true:false;
        */ 

        if(actionName != ACT_ONECLICK_SET_INSTANCE)
            return rc;
        else
            rc = true;

        /*QString complement = iElement.attribute(XNODE_COMPLEMENT);
         QString onFail = iElement.attribute(XNODE_ONFAIL);*/
        
        GTAActionOneClickSet * pActionCmd = new GTAActionOneClickSet(actionName,"");
        /* pActionCmd->setActionOnFail(onFail);
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
                QString nameAttrVal = childElem.attribute(XNODE_NAME);


                if(nodeName == XNODE_EQUATION)
                {
                    GTAEquationSerializer serializer(NULL);
                    GTACommandBase * pEquation = NULL;
                    rc = serializer.deserialize(childElem,pEquation);
                    if(rc && pEquation != NULL )
                    {
                        GTAEquationBase * pEq = dynamic_cast<GTAEquationBase*>(pEquation);
                        pActionCmd->setEquation(pEq);

                    }
                }
                else if(nodeName == XNODE_PARAMETER )
                {
                    QString sParameter = childElem.attribute(XNODE_NAME);
                    pActionCmd->setParameter(sParameter);
                    QString sfunctionStatus = childElem.attribute(XNODE_FS);
                    if (!sfunctionStatus.isEmpty())
                        pActionCmd->setFunctionStatus(sfunctionStatus);
                    QString sIsSetOnlyFS = childElem.attribute(XATTR_ISONLYFS_SET_FIXED);
                    if(sIsSetOnlyFS == XNODE_TRUE)
                        pActionCmd->setIsSetOnlyFSFixed(true);
                    else
                        pActionCmd->setIsSetOnlyFSFixed(false);
                }

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
REGISTER_SERIALIZER(QString("%1").arg(ACT_ONECLICK_SET_INSTANCE),GTAActionOneClickSetSerializer)
