#include "GTAActionPrintTimeSerializer.h"
#include "GTAUtil.h"
#include "GTACommentSerializer.h"
#include "GTAActionPrintTime.h"
#include "GTARequirementSerializer.h"

GTAActionPrintTimeSerializer::GTAActionPrintTimeSerializer() : GTACmdSerializerInterface()
{
    _pActionCmd = NULL;
}
void GTAActionPrintTimeSerializer::setCommand(const GTACommandBase* ipCmd)
{
    _pActionCmd = dynamic_cast<GTAActionPrintTime*>((GTACommandBase*)ipCmd);
}
bool GTAActionPrintTimeSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oElement)//(QDomDocument &iDomDoc,QDomElement &oElement)
{
    bool rc = false;
    if(_pActionCmd != NULL)
    {
        
        QString actionName = _pActionCmd->getAction();
         QString sComplement = _pActionCmd->getComplement();

        if(actionName != ACT_PRINT || sComplement!= COM_PRINT_TIME)
            return rc;
        else
            rc = true;

       
       


        QString sTimeType = _pActionCmd->getTimeType()==GTAActionPrintTime::UTC? ACT_PRINT_TIME_TYPE_NOW : ACT_PRINT_TIME_TYPE_NA;
        oElement  = iDomDoc.createElement(XNODE_ACTION);
       
        /*QString sOnFail =  _pActionCmd->getActionOnFail();
        QString sTimeOut,sTimeOutUnit,sPrecision,sPrecisionUnit;
        oElement.setAttribute(XNODE_NAME,actionName);
        oElement.setAttribute(XNODE_COMPLEMENT,sComplement);
        oElement.setAttribute(XNODE_ONFAIL,sOnFail);
        oElement.setAttribute(XNODE_PRECISION,sPrecision);
        oElement.setAttribute(XNODE_TIMEOUT,sTimeOut);
        oElement.setAttribute(XNODE_TIMEOUT_UNIT,sTimeOutUnit);
        oElement.setAttribute(XNODE_PRECISION_UNIT,sPrecisionUnit);

        QString hiddenInSCXML = _pActionCmd->isIgnoredInSCXML() ? XNODE_TRUE : XNODE_FALSE;
        oElement.setAttribute(XATTR_IGNORE_SCXML,hiddenInSCXML);*/
        setActionAttributes(_pActionCmd,oElement);




        QDomElement domParameter = iDomDoc.createElement(XNODE_PARAMETER);
        domParameter.setAttribute(XATTR_VAL,sTimeType);
        oElement.appendChild(domParameter);

        
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

bool GTAActionPrintTimeSerializer::deserialize(const QDomElement &iElement, GTACommandBase *&opCommand)//(const QDomElement & ioElement)
{
    bool rc = false;
    if(!iElement.isNull())
    {
        /*QString sActionName = iElement.attribute(XNODE_NAME);
        QString sOnFail = iElement.attribute(XNODE_ONFAIL);
        QString sComplement = iElement.attribute(XNODE_COMPLEMENT);
        QString sPrecision = iElement.attribute(XNODE_PRECISION);
        QString sPrecisionUnit = iElement.attribute(XNODE_PRECISION_UNIT);
        QString sTimeOut = iElement.attribute(XNODE_TIMEOUT);
        QString sTimeOutUnit = iElement.attribute(XNODE_TIMEOUT_UNIT);
        bool ignoreInSCXML = iElement.attribute(XATTR_IGNORE_SCXML).toUpper()=="TRUE"?true:false;
      */  
        
        GTAActionPrintTime* pPrintCmd = new GTAActionPrintTime();
        getActionAttributes(iElement,pPrintCmd);
		
        //pPrintCmd->setActionOnFail(sOnFail);
        //pPrintCmd->setTimeOut(sTimeOut,sTimeOutUnit);
        //pPrintCmd->setPrecision(sPrecision,sPrecisionUnit);
        //pPrintCmd->setIgnoreInSCXML(ignoreInSCXML);

        QDomNodeList childList = iElement.childNodes();
        QStringList sParameters;
        for(int i = 0; i < childList.count(); i++)
        {
            QDomElement childElem = childList.at(i).toElement();
            if(!childElem.isNull())
            {
                QString nodeName = childElem.nodeName();
                QString paramType = childElem.attribute(XNODE_NAME);
                QString paramVals = childElem.attribute(XNODE_VALUE);
                if(nodeName == XNODE_PARAMETER)
                {
                    if(paramVals.toUpper()!=ACT_PRINT_TIME_TYPE_NOW)
                        pPrintCmd->setTimeType(GTAActionPrintTime::Formatted);
                    else
                        pPrintCmd->setTimeType(GTAActionPrintTime::UTC);
                }
            }
        }

        opCommand = pPrintCmd;

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
REGISTER_SERIALIZER(QString("%1 %2").arg(ACT_PRINT,COM_PRINT_TIME),GTAActionPrintTimeSerializer)
