#include "GTAActionPrintTableSerializer.h"
#include "GTAUtil.h"
#include "GTACommentSerializer.h"
#include "GTAActionPrintTable.h"
#include "GTARequirementSerializer.h"
GTAActionPrintTableSerializer::GTAActionPrintTableSerializer() : GTACmdSerializerInterface()
{
    _pActionCmd = NULL;
}
void GTAActionPrintTableSerializer::setCommand(const GTACommandBase* ipCmd)
{
    _pActionCmd = dynamic_cast<GTAActionPrintTable*>((GTACommandBase*)ipCmd);
}
bool GTAActionPrintTableSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oElement)//(QDomDocument &iDomDoc,QDomElement &oElement)
{
    bool rc = false;
    if(_pActionCmd != NULL)
    {
        
        QString actionName = _pActionCmd->getAction();
		QString sComplementName = _pActionCmd->getComplement();

		//if(actionName != ACT_PRINT_TABLE)
        if(actionName != ACT_PRINT && sComplementName != COM_PRINT_TABLE)
            return rc;
        else
            rc = true;
        oElement  = iDomDoc.createElement(XNODE_ACTION);

        //QString sComplement = _pActionCmd->getComplement();
        //QString sOnFail =  _pActionCmd->getActionOnFail();
        //QString sTimeOut,sTimeOutUnit,sPrecision,sPrecisionUnit;

        //_pActionCmd->getPrecision(sPrecision,sPrecisionUnit);
        //_pActionCmd->getTimeOut( sTimeOut,sTimeOutUnit);
        //
        //
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
        QString sTableName = _pActionCmd->getTableName();
        QString sTitleName = _pActionCmd->getTitleName();
        oElement.setAttribute(XATTR_TABLE_NAME, sTableName);
        oElement.setAttribute(XATTR_TITLE, sTitleName);






        QStringList sParameters = _pActionCmd->getValues();

        QString sParameter;
        if (!sParameters.isEmpty())
        { 
            if (sParameters.size()>1 )
                sParameter = sParameters.join(";");
            else
                sParameter = sParameters.at(0);
        }
       
        
        QDomElement domParameter = iDomDoc.createElement(XNODE_PARAMETER);
        domParameter.setAttribute(XATTR_VAL,sParameter);
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

bool GTAActionPrintTableSerializer::deserialize(const QDomElement &iElement, GTACommandBase *&opCommand)//(const QDomElement & ioElement)
{
    bool rc = false;
    if(!iElement.isNull())
    {
        QString sTableName = iElement.attribute(XATTR_TABLE_NAME); 
        QString sTitleName = iElement.attribute(XATTR_TITLE);
        /*bool ignoreInSCXML = iElement.attribute(XATTR_IGNORE_SCXML).toUpper()=="TRUE"?true:false;
        QString sActionName = iElement.attribute(XNODE_NAME);
        QString sOnFail = iElement.attribute(XNODE_ONFAIL);
        QString sPrecision = iElement.attribute(XNODE_PRECISION);
        QString sPrecisionUnit = iElement.attribute(XNODE_PRECISION_UNIT);
        QString sTimeOut = iElement.attribute(XNODE_TIMEOUT);
        QString sTimeOutUnit = iElement.attribute(XNODE_TIMEOUT_UNIT);*/

        
        GTAActionPrintTable* pPrintTableCmd = new GTAActionPrintTable();
        getActionAttributes(iElement,pPrintTableCmd);
		

        //pPrintTableCmd->setActionOnFail(sOnFail);
        //pPrintTableCmd->setTimeOut(sTimeOut,sTimeOutUnit);
        //pPrintTableCmd->setPrecision(sPrecision,sPrecisionUnit);
        //pPrintTableCmd->setIgnoreInSCXML(ignoreInSCXML);

        pPrintTableCmd->setTitleName(sTitleName);
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
                    
                    {
                        sParameters= paramVals.split(";");
                        pPrintTableCmd->setValues(sParameters);
                    }
                }
            }
        }

        pPrintTableCmd->setTableName(sTableName);
        opCommand = pPrintTableCmd;

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
//REGISTER_SERIALIZER(QString("%1").arg(ACT_PRINT_TABLE) , GTAActionPrintTableSerializer)
REGISTER_SERIALIZER(QString("%1 %2").arg(ACT_PRINT,COM_PRINT_TABLE) , GTAActionPrintTableSerializer)
