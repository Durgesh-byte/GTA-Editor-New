#include "GTAActionSetSerializer.h"
#include "GTAUtil.h"
#include "GTAEquationSerializer.h"
#include "GTACommentSerializer.h"
#include "GTARequirementSerializer.h"
GTAActionSetSerializer::GTAActionSetSerializer(GTAActionSet *pActionCmd)
{
    _pActionCmd = pActionCmd;
}
bool GTAActionSetSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oElement)
{
    bool rc = false;
    if(_pActionCmd != NULL )
    {
        QString actionName = _pActionCmd->getAction();
        QString complement = _pActionCmd->getComplement();
        QString paramName = _pActionCmd->getParameter();
        QString onFail = _pActionCmd->getActionOnFail();
        QString sFunctionStatus = _pActionCmd->getFunctionStatus();
        QString sSDIStatus = _pActionCmd->getSDIStatus();
        GTAEquationBase * pEquation = _pActionCmd->getEquation();

        QString sTimeOut,sTimeOutUnit,sPrecision,sPrecisionUnit;
        _pActionCmd->getPrecision(sPrecision,sPrecisionUnit);
        _pActionCmd->getTimeOut( sTimeOut,sTimeOutUnit);

        oElement  = iDomDoc.createElement(XNODE_SET_INSTANCE);
        oElement.setAttribute(XNODE_NAME,actionName);
        oElement.setAttribute(XNODE_ONFAIL,onFail);

        QString hiddenInSCXML = _pActionCmd->isIgnoredInSCXML() ? XNODE_TRUE : XNODE_FALSE;
        oElement.setAttribute(XATTR_IGNORE_SCXML,hiddenInSCXML);

        QString uuid = _pActionCmd->getObjId();
        oElement.setAttribute(XNODE_UUID,uuid);

        //Parameter Node
        QDomElement domParam = iDomDoc.createElement(XNODE_PARAMETER);
        oElement.appendChild(domParam);
        domParam.setAttribute(XNODE_NAME, paramName);
        domParam.setAttribute(XNODE_VALUE,XNODE_EQUATION);
        domParam.setAttribute(XNODE_FS,sFunctionStatus);
        domParam.setAttribute(XNODE_SDI,sSDIStatus);

        QString isSetOnlyFSFixed = _pActionCmd->getIsSetOnlyFSFixed() ? "TRUE" : "FALSE";
        domParam.setAttribute(XATTR_ISONLYFS_SET_FIXED,isSetOnlyFSFixed);

        QString isSetOnlyFSVariable = _pActionCmd->getIsSetOnlyFSVariable() ? "TRUE" : "FALSE";
        domParam.setAttribute(XATTR_ISONLYFS_SET_VARIABLE, isSetOnlyFSVariable);

        QString isSetOnlySDI = _pActionCmd->getIsSetOnlySDI() ? "TRUE" : "FALSE";
        domParam.setAttribute(XATTR_ISONLYSDI_SET,isSetOnlySDI);

		QString isSetOnlyValue = _pActionCmd->getIsSetOnlyValue() ? "TRUE" : "FALSE";
		domParam.setAttribute(XATTR_ISONLYVALUE_SET, isSetOnlyValue);
		
		QString isParamWithoutFS = _pActionCmd->getIsParamWithoutFS() ? "TRUE" : "FALSE";
        domParam.setAttribute(XATTR_ISPARAMWITHOUTFS, isParamWithoutFS);

        GTAEquationSerializer equationSerializer(pEquation);
        QDomElement domEquation;
        rc = equationSerializer.serialize(iDomDoc,domEquation);
        if(rc && (! domEquation.isNull()))
            oElement.appendChild(domEquation);

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

bool GTAActionSetSerializer::deserialize(const QDomElement & iElement, GTACommandBase *& opCommand)
{
    bool rc = false;
    if(!iElement.isNull() )
    {
        QString actionName = iElement.attribute(XNODE_NAME);
        QString onFail = iElement.attribute(XNODE_ONFAIL);
        QString uuid = iElement.attribute(XNODE_UUID);

        if(actionName != ACT_SET_INSTANCE)
            return rc;
        else
            rc = true;

        GTAActionSet * pActionCmd = new GTAActionSet(actionName,"");
        getActionAttributes(iElement,pActionCmd);
        opCommand = pActionCmd;
        if (pActionCmd->getObjId() != uuid && !uuid.isEmpty() )
            pActionCmd->setObjId(uuid,false);
        else
            pActionCmd->setObjId();

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

                    QString sSDIStatus = childElem.attribute(XNODE_SDI);
                    if (!sSDIStatus.isEmpty())
                        pActionCmd->setSDIStatus(sSDIStatus);

                    QString sIsSetOnlyFSFixed = childElem.attribute(XATTR_ISONLYFS_SET_FIXED);
                    if(sIsSetOnlyFSFixed == XNODE_TRUE)
                    {
                        pActionCmd->setIsSetOnlyFSFixed(true);
                    }
                    else
                    {
                        pActionCmd->setIsSetOnlyFSFixed(false);
                    }

                    QString sIsSetOnlyFSVariable = childElem.attribute(XATTR_ISONLYFS_SET_VARIABLE);
                    if (sIsSetOnlyFSVariable == XNODE_TRUE)
                    {
                        pActionCmd->setIsSetOnlyFSVariable(true);
                    }
                    else
                    {
                        pActionCmd->setIsSetOnlyFSVariable(false);
                    }

                    QString sIsSetOnlySDI = childElem.attribute(XATTR_ISONLYSDI_SET);
                    if(sIsSetOnlySDI == XNODE_TRUE)
                        pActionCmd->setIsSetOnlySDI(true);
                    else
                        pActionCmd->setIsSetOnlySDI(false);

                    QString sIsSetOnlyValue = childElem.attribute(XATTR_ISONLYVALUE_SET);
                    if(sIsSetOnlyValue == XNODE_TRUE)
                        pActionCmd->setIsSetOnlyValue(true);
                    else
                        pActionCmd->setIsSetOnlyValue(false);

					QString sIsParamWithoutFS = childElem.attribute(XATTR_ISPARAMWITHOUTFS);
					if (sIsParamWithoutFS == XNODE_TRUE)
						pActionCmd->setIsParamWithoutFS(true);
					else
						pActionCmd->setIsParamWithoutFS(false);
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
