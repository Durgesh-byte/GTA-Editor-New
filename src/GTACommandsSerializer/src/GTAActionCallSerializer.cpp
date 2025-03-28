#include "GTAActionCallSerializer.h"
#include "GTAUtil.h"
#include "GTACommentSerializer.h"
#include "GTAActionCall.h"
#include "GTARequirementSerializer.h"

GTAActionCallSerializer::GTAActionCallSerializer()
{
    _pActionCmd = NULL;
}
void GTAActionCallSerializer::setCommand(const GTACommandBase *pCmd)
{
    _pActionCmd = dynamic_cast<GTAActionCall*> ( (GTACommandBase *) pCmd);
}
bool GTAActionCallSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oElement)
{
    bool rc = false;
    if(_pActionCmd != NULL )
    {


        oElement  = iDomDoc.createElement(XNODE_ACTION);
        setActionAttributes(_pActionCmd,oElement);

        //Parameter Node
        QString elementName = _pActionCmd->getElement();
        QDomElement domElement = iDomDoc.createElement(XNODE_ELEMENT);
        oElement.appendChild(domElement);
        domElement.setAttribute(XNODE_NAME, elementName);

        //monitor Name
        QString MonitorName = _pActionCmd->getMonitorName();
        domElement.setAttribute(XNODE_MON_NAME, MonitorName);

        //reference
        QString refFileUUID = _pActionCmd->getRefrenceFileUUID();
        domElement.setAttribute("REF_FILE_UUID",refFileUUID);

        QString isContinuousLoop = (_pActionCmd->isContinuousLoop())?XNODE_TRUE:XNODE_FALSE;
        domElement.setAttribute("IS_CONT_LOOP",isContinuousLoop);
        if(_pActionCmd->isContinuousLoop())
        {
            QString continuousLoopTime = _pActionCmd->getContinuousLoopTime();
            QString continuousLoopTimeUnit = _pActionCmd->getContinuousLoopTimeUnit();
            domElement.setAttribute("CONT_TIME_LOOP",continuousLoopTime);
            domElement.setAttribute("CONT_TIME_LOOP_UNIT",continuousLoopTimeUnit);
        }


        //VARIABLE VALUE pairs

        QMap<QString, QString> tagValuePairs = _pActionCmd->getTagValue();
        QMapIterator<QString, QString> iterTag(tagValuePairs);
        while (iterTag.hasNext()) 
        {
            iterTag.next();
            QDomElement domParam = iDomDoc.createElement(XNODE_PARAMETER);
            domParam.setAttribute(XNODE_NAME,iterTag.key());
            domParam.setAttribute(XNODE_VALUE,iterTag.value());
            domElement.appendChild(domParam);
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

bool GTAActionCallSerializer::deserialize(const QDomElement & iElement, GTACommandBase *& opCommand)
{
    bool rc = false;
    if(!iElement.isNull() && iElement.nodeName() == XNODE_ACTION)
    {
        QString actionName = iElement.attribute(XNODE_NAME);
        QString complement = iElement.attribute(XNODE_COMPLEMENT);

        QString elementName;
        QString monName;
        QString refFileUUID;
        bool bIsContinuousLoop = false;
        QString continuousLoopTime = "";
        QString continuousLoopTimeUnit = "";
        QMap<QString, QString> tagValuePairs;
        QDomNodeList childList = iElement.childNodes();
        //TODO: What if fail is missing? Needs fail mechanism
        for(int i = 0; i < childList.count(); i++)
        {
            QDomNode childNode = childList.at(i);
            if(childNode.isElement())
            {
                QDomElement childElem = childNode.toElement();
                QString nameAttr = childElem.attribute(XNODE_NAME);
                QString  monNameAttr = childElem.attribute(XNODE_MON_NAME);
                refFileUUID = childElem.attribute("REF_FILE_UUID");

                QString isContinuousLoop = childElem.attribute("IS_CONT_LOOP");
                bIsContinuousLoop = (isContinuousLoop == XNODE_TRUE)?true:false;
                if(bIsContinuousLoop)
                {
                    continuousLoopTime = childElem.attribute("CONT_TIME_LOOP");
                    continuousLoopTimeUnit = childElem.attribute("CONT_TIME_LOOP_UNIT");
                }


                QString nodeName = childElem.nodeName();
                //monitor Name
                if(nodeName == XNODE_ELEMENT )
                {
                    monName = monNameAttr;
                    elementName = nameAttr;
                    QDomNodeList paramNodeLst = childElem.childNodes();
                    for(int j = 0; j < paramNodeLst.count(); j++)
                    {
                        QDomNode paramNode = paramNodeLst.at(j);
                        if (!paramNode.isNull())
                        {
                            QDomElement paramElement = paramNode.toElement();
                            QString sVar = paramElement.attribute(XNODE_NAME);
                            QString sVal = paramElement.attribute(XNODE_VALUE);
                            if (!sVar.isEmpty() && !sVal.isEmpty())
                            {
                                tagValuePairs[sVar]=sVal;
                            }
                        }
                    }


                    break;
                }
            }
        }
        if(actionName == ACT_CALL && ! complement.isEmpty() && ! elementName.isEmpty())
        {
            rc = true;
            GTAActionCall * pCmd = new GTAActionCall(actionName,complement,elementName);
            pCmd->setTagValue(tagValuePairs);
            pCmd->setMonitorName(monName);
            pCmd->setIsContinuousLoop(bIsContinuousLoop);
            pCmd->setContinuousLoopTime(continuousLoopTime);
            pCmd->setContinuousLoopTimeUnit(continuousLoopTimeUnit);
            pCmd->setRefrenceFileUUID(refFileUUID);
            getActionAttributes(iElement,pCmd);

            opCommand = pCmd;

            //comment node
            GTACommentSerializer commentSerializer(NULL);
            commentSerializer.deserialize(iElement,opCommand);


            //requiremrnt node
            GTARequirementSerializer requirementSerializer(NULL);
            requirementSerializer.deserialize(iElement,opCommand);
        }

    }
    return rc;
}
#include "GTASerilizerRegisterDef.h"
REGISTER_SERIALIZER(ACT_CALL,GTAActionCallSerializer)
