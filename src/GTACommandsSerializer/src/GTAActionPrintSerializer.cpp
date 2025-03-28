#include "GTAActionPrintSerializer.h"
#include "GTAUtil.h"
#include "GTACommentSerializer.h"
#include "GTAActionPrint.h"
#include "GTARequirementSerializer.h"
GTAActionPrintSerializer::GTAActionPrintSerializer() : GTACmdSerializerInterface()
{
    _pActionCmd = NULL;
}
void GTAActionPrintSerializer::setCommand(const GTACommandBase* ipCmd)
{
    _pActionCmd = dynamic_cast<GTAActionPrint*>((GTACommandBase*)ipCmd);
}

bool GTAActionPrintSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oElement)//(QDomDocument &iDomDoc,QDomElement &oElement)
{
    bool rc = false;
    if(_pActionCmd != NULL)
    {
        
        QString actionName = _pActionCmd->getAction();

        if(actionName != ACT_PRINT)
            return rc;
        else
            rc = true;

         
        QString printTypeS;
        QStringList sParameters = _pActionCmd->getValues();
        QString messagTypeS = _pActionCmd->getMessageType();
        GTAActionPrint::PrintType printType = _pActionCmd->getPrintType();
        switch(printType)
        {
        case GTAActionPrint::PARAMETER :printTypeS = COM_PRINT_PARAM; break;
        case GTAActionPrint::MESSAGE :printTypeS = COM_PRINT_MSG; break;
        case GTAActionPrint::FWC_BUFFER :printTypeS = COM_PRINT_FWC_BUFF; break;
        case GTAActionPrint::BITE_BUFFER :printTypeS = COM_PRINT_BITE_BUFF; break;
        }

        oElement  = iDomDoc.createElement(XNODE_ACTION);

        //QString sComplement = _pActionCmd->getComplement();
        //QString sOnFail =  _pActionCmd->getActionOnFail();
        //QString sTimeOut,sTimeOutUnit,sPrecision,sPrecisionUnit;
        //_pActionCmd->getPrecision(sPrecision,sPrecisionUnit);
        //_pActionCmd->getTimeOut( sTimeOut,sTimeOutUnit);

      
        //oElement.setAttribute(XNODE_NAME,actionName);
        //oElement.setAttribute(XNODE_COMPLEMENT,sComplement);
        //oElement.setAttribute(XNODE_ONFAIL,sOnFail);
        //oElement.setAttribute(XNODE_PRECISION,sPrecision);
        //oElement.setAttribute(XNODE_TIMEOUT,sTimeOut);
        //oElement.setAttribute(XNODE_TIMEOUT_UNIT,sTimeOutUnit);
        //oElement.setAttribute(XNODE_PRECISION_UNIT,sPrecisionUnit);

        //QString hiddenInSCXML = _pActionCmd->isIgnoredInSCXML() ? XNODE_TRUE : XNODE_FALSE;
        //oElement.setAttribute(XATTR_IGNORE_SCXML,hiddenInSCXML);
        setActionAttributes(_pActionCmd,oElement);


        QString sParameter;
        if (!sParameters.isEmpty())
        { 
            if (sParameters.size()>1 )
                sParameter = sParameters.join(";");
            else
                sParameter = sParameters.at(0);
        }
       
        
        QDomElement domParameter = iDomDoc.createElement(XNODE_PARAMETER);
        domParameter.setAttribute(XATTR_NAME,printTypeS);
        domParameter.setAttribute(XATTR_VAL,sParameter);
        oElement.appendChild(domParameter);

        
        

        QDomElement domMsgType = iDomDoc.createElement(XNODE_PARAMETER);
        domMsgType.setAttribute(XNODE_NAME,XNODE_MESSAGETYPE);
        domMsgType.setAttribute(XNODE_VALUE,messagTypeS);
        oElement.appendChild(domMsgType);


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

bool GTAActionPrintSerializer::deserialize(const QDomElement &iElement, GTACommandBase *&opCommand)//(const QDomElement & ioElement)
{
    bool rc = false;
    if(!iElement.isNull())
    {
        QString sActionName = iElement.attribute(XNODE_NAME);
        QString sComplement = iElement.attribute(XNODE_COMPLEMENT);
        /*QString sOnFail = iElement.attribute(XNODE_ONFAIL);
        
        QString sPrecision = iElement.attribute(XNODE_PRECISION);
        QString sPrecisionUnit = iElement.attribute(XNODE_PRECISION_UNIT);
        QString sTimeOut = iElement.attribute(XNODE_TIMEOUT);
        QString sTimeOutUnit = iElement.attribute(XNODE_TIMEOUT_UNIT);
        bool ignoreInSCXML = iElement.attribute(XATTR_IGNORE_SCXML).toUpper()=="TRUE"?true:false;
        */
        
        GTAActionPrint* pPrintCmd = new GTAActionPrint(sActionName,sComplement);
		if(sComplement == COM_PRINT_PARAM)
               pPrintCmd->setPrintType(GTAActionPrint::PARAMETER);
		else if(sComplement == COM_PRINT_MSG)
               pPrintCmd->setPrintType(GTAActionPrint::MESSAGE);
		else if(sComplement == COM_PRINT_FWC_BUFF)
               pPrintCmd->setPrintType(GTAActionPrint::FWC_BUFFER);
		else if(sComplement == COM_PRINT_BITE_BUFF)
               pPrintCmd->setPrintType(GTAActionPrint::BITE_BUFFER);

        //pPrintCmd->setActionOnFail(sOnFail);
        //pPrintCmd->setTimeOut(sTimeOut,sTimeOutUnit);
        //pPrintCmd->setPrecision(sPrecision,sPrecisionUnit);
        //pPrintCmd->setIgnoreInSCXML(ignoreInSCXML);
        getActionAttributes(iElement,pPrintCmd);

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
                    if(paramType == XNODE_MESSAGETYPE)
                    {
                        pPrintCmd->setMessageType(paramVals);
                    }
                    else
                    {
                        sParameters= paramVals.split(";");
                        pPrintCmd->setValues(sParameters);
                    }
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
REGISTER_SERIALIZER_MULTI(QString("%1 %2").arg(ACT_PRINT,COM_PRINT_BITE_BUFF),GTAActionPrintSerializer,obj0)
REGISTER_SERIALIZER_MULTI(QString("%1 %2").arg(ACT_PRINT,COM_PRINT_MSG),GTAActionPrintSerializer,obj1)
REGISTER_SERIALIZER_MULTI(QString("%1 %2").arg(ACT_PRINT,COM_PRINT_PARAM),GTAActionPrintSerializer,obj2)
REGISTER_SERIALIZER_MULTI(QString("%1 %2").arg(ACT_PRINT,COM_PRINT_FWC_BUFF),GTAActionPrintSerializer,obj3)
