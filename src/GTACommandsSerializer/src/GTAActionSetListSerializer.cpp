#include "GTAActionSetListSerializer.h"
#include "GTAActionSetList.h"
#include "GTAUtil.h"
#include "GTAEquationSerializer.h"
#include "GTACommentSerializer.h"
#include "GTAActionSetSerializer.h"
#include "GTAActionSet.h"
#include "GTARequirementSerializer.h"

#pragma warning(push, 0)
#include <QDebug>
#pragma warning(pop)

GTAActionSetListSerializer::GTAActionSetListSerializer():GTACmdSerializerInterface()
{
    _pActionCmd = NULL;
}
GTAActionSetListSerializer::~GTAActionSetListSerializer()
{
    _pActionCmd = NULL;
}
void GTAActionSetListSerializer::setCommand(const GTACommandBase* iPcmd)
{
    _pActionCmd =dynamic_cast<GTAActionSetList*>((GTACommandBase*) iPcmd);
}
bool GTAActionSetListSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oElement)
{
    bool rc = false;
    if(_pActionCmd != NULL )
    {
        QList<GTAActionSet*> setActionList;
        _pActionCmd->getSetCommandlist(setActionList);
        if (setActionList.size())
        {

             oElement  = iDomDoc.createElement(XNODE_ACTION);
                         
             setActionAttributes(_pActionCmd,oElement);

             QString vmacForceType = _pActionCmd->getVmacForceType(); 
             oElement.setAttribute(XNODE_VMACFORCETYPE,vmacForceType);

             QString lastEditedRow = QString::number(_pActionCmd->getLastEditedRow());
             oElement.setAttribute(XNODE_LAST_EDITED_ROW, lastEditedRow);


            int id=-1;
            foreach(GTAActionSet* pSetCmd, setActionList)
            {
                id++;
                if (pSetCmd!=NULL)
                {
                   
                    QDomElement pSetElement;// = new QDomElement;// = iDomDoc.createElement(XNODE_ACTION);;
                    GTAActionSetSerializer setSerialiser(pSetCmd);
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

bool GTAActionSetListSerializer::deserialize(const QDomElement & iElement, GTACommandBase *& opCommand)
{
	bool rc = false;
     if(!iElement.isNull() && iElement.nodeName() == XNODE_ACTION)
     {
         QString actionName = iElement.attribute(XNODE_NAME);

         if(actionName != ACT_SET)
             return rc;
         else
             rc = true;

         QString complement = iElement.attribute(XNODE_COMPLEMENT);
         QString onFail = iElement.attribute(XNODE_ONFAIL);
        
         GTAActionSetList * pActionCmd = new GTAActionSetList(actionName,complement);

		 
         getActionAttributes(iElement,pActionCmd);

         int lastEditedRow = iElement.hasAttribute(XNODE_LAST_EDITED_ROW) ?  iElement.attribute(XNODE_LAST_EDITED_ROW).toInt() : 0;
         pActionCmd->setLastEditedRow(lastEditedRow);
         
        
         QString sVmacForceType = iElement.attribute(XNODE_VMACFORCETYPE);
         pActionCmd->setVmacForceType(sVmacForceType);
         
        
        
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
                     GTAActionSetSerializer setSerializer(NULL);
                     setSerializer.deserialize(childElem,pCmd);
                     GTAActionSet* pSetAction = dynamic_cast<GTAActionSet* > (pCmd);
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
REGISTER_SERIALIZER(QString("%1").arg(ACT_SET),GTAActionSetListSerializer)
