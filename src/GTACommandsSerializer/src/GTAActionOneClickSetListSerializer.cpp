#include "GTAActionOneClickSetListSerializer.h"
#include "GTAUtil.h"
#include "GTAEquationSerializer.h"
#include "GTACommentSerializer.h"
#include "GTAActionOneClickSetSerializer.h"
#include "GTAActionOneClickSet.h"
#include "GTAActionOneClickSetList.h"
#include "GTARequirementSerializer.h"

#pragma warning(push, 0)
#include <QDebug>
#pragma warning(pop)

GTAActionOneClickSetListSerializer::GTAActionOneClickSetListSerializer():GTACmdSerializerInterface()
{
    _pActionCmd = NULL;
}
void GTAActionOneClickSetListSerializer::setCommand(const GTACommandBase* ipCmd)
{
    _pActionCmd = dynamic_cast<GTAActionOneClickSetList*>((GTACommandBase*)ipCmd);
}
bool GTAActionOneClickSetListSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oElement)
{
    bool rc = false;
    if(_pActionCmd != NULL )
    {
        QList<GTAActionOneClickSet*> setActionList;
        _pActionCmd->getSetCommandlist(setActionList);
        if (setActionList.size())
        {

             oElement  = iDomDoc.createElement(XNODE_ACTION);
             //QString actionName = _pActionCmd->getAction();
             //QString complement = _pActionCmd->getComplement();
             //QString onFail = _pActionCmd->getActionOnFail();
               

             //QString sTimeOut,sTimeOutUnit,sPrecision,sPrecisionUnit;
             //_pActionCmd->getPrecision(sPrecision,sPrecisionUnit);
             //_pActionCmd->getTimeOut( sTimeOut,sTimeOutUnit);

             ////oElement  = iDomDoc.createElement(XNODE_ACTION);
             //oElement.setAttribute(XNODE_NAME,actionName);
             //oElement.setAttribute(XNODE_COMPLEMENT,complement);
             //oElement.setAttribute(XNODE_TIMEOUT,sTimeOut);
             //oElement.setAttribute(XNODE_ONFAIL,onFail);
             //oElement.setAttribute(XNODE_PRECISION,sPrecision);
             //oElement.setAttribute(XNODE_TIMEOUT_UNIT,sTimeOutUnit);
             //oElement.setAttribute(XNODE_PRECISION_UNIT,sPrecisionUnit);
             setActionAttributes(_pActionCmd,oElement);
             QString vmacForceType = _pActionCmd->getVmacForceType(); 
             oElement.setAttribute(XNODE_VMACFORCETYPE,vmacForceType);

             QString hiddenInSCXML = _pActionCmd->isIgnoredInSCXML() ? XNODE_TRUE : XNODE_FALSE;
             oElement.setAttribute(XATTR_IGNORE_SCXML,hiddenInSCXML);




            int id=-1;
            foreach(GTAActionOneClickSet* pSetCmd, setActionList)
            {
                id++;
                if (pSetCmd!=NULL)
                {
                   
                    QDomElement pSetElement;// = new QDomElement;// = iDomDoc.createElement(XNODE_ACTION);;
                    GTAActionOneClickSetSerializer setSerialiser;
                    setSerialiser.setCommand((GTACommandBase*)pSetCmd);
                    rc =setSerialiser.serialize(iDomDoc,pSetElement);
                    if (!pSetElement.isNull() && rc==true)
                    {
                        oElement.appendChild(pSetElement);
                        pSetElement.setAttribute(XNODE_ID,QString::number(id));
                    }

                }
            }
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

bool GTAActionOneClickSetListSerializer::deserialize(const QDomElement & iElement, GTACommandBase *& opCommand)
{
	bool rc = false;
     if(!iElement.isNull() && iElement.nodeName() == XNODE_ACTION)
     {
         QString actionName = iElement.attribute(XNODE_NAME);

         if(actionName != ACT_ONECLICK_SET)
             return rc;
         else
             rc = true;

         QString complement = iElement.attribute(XNODE_COMPLEMENT);
         QString onFail = iElement.attribute(XNODE_ONFAIL);
        
         GTAActionOneClickSetList * pActionCmd = new GTAActionOneClickSetList(actionName,complement);

		 /*QString sTimeOut = iElement.attribute(XNODE_TIMEOUT);
		 QString sTimeOutUnit = iElement.attribute(XNODE_TIMEOUT_UNIT);
		
	
		 pActionCmd->setTimeOut(sTimeOut,sTimeOutUnit);*/

         QString sVmacForceType = iElement.attribute(XNODE_VMACFORCETYPE);
         pActionCmd->setVmacForceType(sVmacForceType);

         //QString sOnFail = iElement.attribute(XNODE_ONFAIL);
         //pActionCmd->setActionOnFail(sOnFail);

         //bool ignoreInSCXML = iElement.attribute(XATTR_IGNORE_SCXML).toUpper()=="TRUE"?true:false;
         //pActionCmd->setIgnoreInSCXML(ignoreInSCXML);

         // QString sPrecision = iElement.attribute(XNODE_PRECISION);
         // QString sPrecisionUnit = iElement.attribute(XNODE_PRECISION_UNIT);
		 // pActionCmd->setPrecision(sPrecision,sPrecisionUnit);
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
                  if(nodeName == XNODE_SET_INSTANCE)
                 {
                     GTACommandBase* pCmd=NULL;
                     GTAActionOneClickSetSerializer setSerializer;
                     setSerializer.deserialize(childElem,pCmd);
                     GTAActionOneClickSet* pSetAction = dynamic_cast<GTAActionOneClickSet* > (pCmd);
                     if(NULL!=pSetAction)
                         pActionCmd->addSetAction(pSetAction);
                    // pActionCmd->setParameter(nameAttrVal);
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
REGISTER_SERIALIZER(QString("%1").arg(ACT_ONECLICK_SET),GTAActionOneClickSetListSerializer)
