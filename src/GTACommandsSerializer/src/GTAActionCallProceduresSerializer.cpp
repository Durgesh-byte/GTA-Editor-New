#include "GTAActionCallProceduresSerializer.h"
#include "GTAUtil.h"
#include "GTACommentSerializer.h"
#include "GTAActionCallProcedures.h"
#include "GTARequirementSerializer.h"

GTAActionCallProceduresSerializer::GTAActionCallProceduresSerializer() : GTACmdSerializerInterface()
{
    _pActionCmd = NULL;
}
GTAActionCallProceduresSerializer:: ~GTAActionCallProceduresSerializer()
{
    
}
void GTAActionCallProceduresSerializer::setCommand(const GTACommandBase* pCmd)
{
_pActionCmd = dynamic_cast<GTAActionCallProcedures*>((GTACommandBase*)pCmd);
}
bool GTAActionCallProceduresSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oElement)//(QDomDocument &iDomDoc,QDomElement &oElement)
{
    bool rc = false;
    if(_pActionCmd != NULL)
    {
        
        QString actionName = _pActionCmd->getAction();

        if(actionName != ACT_CALL_PROCS)
            return rc;
        else
            rc = true;

               QString printTypeS;
        QList<GTACallProcItem> callItemList = _pActionCmd->getCallProcedures();
       

        oElement  = iDomDoc.createElement(XNODE_ACTION);
        /* 
        QString sComplement = _pActionCmd->getComplement();
        QString sOnFail =  _pActionCmd->getActionOnFail();
        QString sTimeOut,sTimeOutUnit,sPrecision,sPrecisionUnit;
        _pActionCmd->getPrecision(sPrecision,sPrecisionUnit);
        _pActionCmd->getTimeOut( sTimeOut,sTimeOutUnit);
        QString hiddenInSCXML = _pActionCmd->isIgnoredInSCXML() ? XNODE_TRUE : XNODE_FALSE;

        oElement.setAttribute(XNODE_NAME,actionName);
        oElement.setAttribute(XNODE_COMPLEMENT,sComplement);
        oElement.setAttribute(XNODE_ONFAIL,sOnFail);
        oElement.setAttribute(XNODE_PRECISION,sPrecision);
        oElement.setAttribute(XNODE_TIMEOUT,sTimeOut);
        oElement.setAttribute(XNODE_TIMEOUT_UNIT,sTimeOutUnit);
        oElement.setAttribute(XNODE_PRECISION_UNIT,sPrecisionUnit);
        oElement.setAttribute(XATTR_IGNORE_SCXML,hiddenInSCXML);*/
        setActionAttributes(_pActionCmd,oElement);

        bool executePrallely = _pActionCmd->getPrallelExecution();
        oElement.setAttribute(XATTR_PARALLEL,executePrallely?"TRUE":"FALSE");


        QString sParameter;
        if (!callItemList.isEmpty())
        { 
            for (auto callItem : callItemList)
            {
                QString sType = COM_CALL_PROC;
                if (callItem._type == GTACallProcItem::FUNCTION)
                    sType=COM_CALL_FUNC;
                QDomElement domProcedure = iDomDoc.createElement(XNODE_PROCEDURE);
                domProcedure.setAttribute(XATTR_NAME,callItem._elementName);
                domProcedure.setAttribute(XATTR_TYPE,sType);
                domProcedure.setAttribute(XATTR_REF_UUID,callItem._UUID);
                QMap<QString,QString> iTagValuePair = callItem._tagValue;
                QStringList tagValueList;
                for (auto tag : iTagValuePair.keys())
                {
                    tagValueList.append(QString("%1:%2").arg(tag,iTagValuePair.value(tag)));
                }

                domProcedure.setAttribute(XATTR_TAGVALUE, tagValueList.join(";"));
                oElement.appendChild(domProcedure);
                
            }
        }

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

bool GTAActionCallProceduresSerializer::deserialize(const QDomElement &iElement, GTACommandBase *&opCommand)//(const QDomElement & ioElement)
{
    bool rc = false;
    if(!iElement.isNull())
    {
      /*  QString sActionName = iElement.attribute(XNODE_NAME);
        QString sOnFail = iElement.attribute(XNODE_ONFAIL);
        QString sComplement = iElement.attribute(XNODE_COMPLEMENT);
        QString sPrecision = iElement.attribute(XNODE_PRECISION);
        QString sPrecisionUnit = iElement.attribute(XNODE_PRECISION_UNIT);
        QString sTimeOut = iElement.attribute(XNODE_TIMEOUT);
        QString sTimeOutUnit = iElement.attribute(XNODE_TIMEOUT_UNIT);
        bool ignoreInSCXML = iElement.attribute(XATTR_IGNORE_SCXML).toUpper()=="TRUE"?true:false;*/


        bool executePrallely = iElement.attribute(XATTR_PARALLEL)=="TRUE"?true:false;

        GTAActionCallProcedures* pCallProcs = new GTAActionCallProcedures();
        getActionAttributes(iElement,pCallProcs);
       
        pCallProcs->setParallelExecution(executePrallely);
        /*pCallProcs->setIgnoreInSCXML(ignoreInSCXML);
        pCallProcs->setActionOnFail(sOnFail);
        pCallProcs->setTimeOut(sTimeOut,sTimeOutUnit);
        pCallProcs->setPrecision(sPrecision,sPrecisionUnit);*/
        QDomNodeList childList = iElement.childNodes();
        QStringList sParameters;
        for(int i = 0; i < childList.count(); i++)
        {
            QDomElement childElem = childList.at(i).toElement();
            if(!childElem.isNull())
            {
                QString nodeName = childElem.nodeName();
                
                if(nodeName == XNODE_PROCEDURE)
                {
                    
                   /* pCallProcs->setParallelExecution(executePrallely);
                    sParameters= paramVals.split(";");
                    pCallProcs->setCallProcedures(sParameters);*/

                   
                    QString sProcName = childElem.attribute(XATTR_NAME);
                    QString sProcType = childElem.attribute(XATTR_TYPE);
                    QString sUUID = childElem.attribute(XATTR_REF_UUID);
                    QString sTagValueStr = childElem.attribute(XATTR_TAGVALUE);
                    GTACallProcItem::call_type callType = GTACallProcItem::PROCEDURE;
                    if(sProcType == COM_CALL_FUNC)
                        callType=GTACallProcItem::FUNCTION;

                    QMap <QString,QString> tagValueHash;
//                    if(callType==GTACallProcItem::FUNCTION)
//                    {
                        QStringList sTagValuePairList = sTagValueStr.split(";");


                        foreach(QString sPair, sTagValuePairList)
                        {
                            QStringList keyValue = sPair.split(":");
                            if (keyValue.size()==2)
                            {
                                tagValueHash.insert(keyValue.at(0),keyValue.at(1));
                            }
                        }
//                    }

                   
                    pCallProcs->appendCallProcedure(callType,sProcName,tagValueHash,sUUID);

                }
            }
        }

        opCommand = pCallProcs;

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
REGISTER_SERIALIZER(ACT_CALL_PROCS,GTAActionCallProceduresSerializer)
