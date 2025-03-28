#include "GTAActionFailureSerializer.h"
#include "GTAUtil.h"
#include "GTACommentSerializer.h"
#include "GTAFailureProfileSerializer.h"
#include "GTAFailureConfigSerializer.h"
#include "GTAActionFailure.h"
#include "GTARequirementSerializer.h"
GTAActionFailureSerializer::GTAActionFailureSerializer() : GTACmdSerializerInterface()
{
    _pActionCmd = NULL;
}
void GTAActionFailureSerializer::setCommand(const GTACommandBase* ipCmd)
{
    _pActionCmd = dynamic_cast<GTAActionFailure*>((GTACommandBase*)ipCmd);
}
bool GTAActionFailureSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oElement)//(QDomDocument &iDomDoc,QDomElement &oElement)
{
    bool rc = false;
    if(_pActionCmd != NULL)
    {
        
        QString actionName = _pActionCmd->getAction();

        if(actionName != ACT_FAILURE)
            return rc;
        else
            rc = true;
        oElement  = iDomDoc.createElement(XNODE_ACTION);

        setActionAttributes(_pActionCmd,oElement);
        QString sFailureName = _pActionCmd->getFailureName();
        oElement.setAttribute(XATTR_FAIL_NAME, sFailureName);

        GTAActionFailure::Failure_Type failType = _pActionCmd->getFailureType();
        QString sFailType = "START";

        switch (failType)
        {
        case GTAActionFailure::START:
                sFailType="START";
                break;
        case GTAActionFailure::STOP:
            sFailType="STOP";
            break;
        case GTAActionFailure::PAUSE:
            sFailType="PAUSE";
            break;
        case GTAActionFailure::RESUME:
            sFailType="RESUME";
            break;
        }

        oElement.setAttribute(XATTR_FAIL_TYPE, sFailType);
        QList <QPair<QString,GTAFailureConfig* >> lstOfConfigs = _pActionCmd->getParamValConfig();
        //oElement.setAttribute(XATTR_TITLE, sTitleName);

        if (lstOfConfigs.size()!=0)
        {
            QDomElement configurationElem = iDomDoc.createElement(XNODE_CONFIGS);

            oElement.appendChild(configurationElem);
            for (int i=0;i<lstOfConfigs.size();i++)
            {
                QPair<QString,GTAFailureConfig* > pairOfParamConfig = lstOfConfigs.at(i);
                GTAFailureConfig* pFailureConfig = pairOfParamConfig.second;
                QString sParameterName = pairOfParamConfig.first;

                QDomElement configNode;
                GTAFailureConfigSerializer configSerializer(pFailureConfig);
                configSerializer.serialize(iDomDoc,configNode);
                if (configNode.isNull()==false)
                {
                    configurationElem.appendChild(configNode);
                    configNode.setAttribute(XNODE_PARAMETER,sParameterName);
                }
                
                
                
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

bool GTAActionFailureSerializer::deserialize(const QDomElement &iElement, GTACommandBase *&opCommand)//(const QDomElement & ioElement)
{
    bool rc = false;
    if(!iElement.isNull())
    {
       


        QString sFailureName = iElement.attribute(XATTR_FAIL_NAME);
        QString sFailtype = iElement.attribute(XATTR_FAIL_TYPE).toUpper();
        
      
        
        
        GTAActionFailure* pFailureCommand = new GTAActionFailure();
        GTAActionFailure::Failure_Type failType = GTAActionFailure::START;
      

        if (sFailtype=="START")
        {
            failType = GTAActionFailure::START;
        }
        else if (sFailtype=="STOP")
        {
            failType = GTAActionFailure::STOP;
        }
        else if (sFailtype=="PAUSE")
        {
            failType = GTAActionFailure::PAUSE;
        }
        else if (sFailtype=="RESUME")
        {
            failType = GTAActionFailure::RESUME;
        }
		
        pFailureCommand->setFailureName(sFailureName);
        pFailureCommand->setFailureType(failType);
        getActionAttributes(iElement,pFailureCommand);
       

        GTAFailureConfig* pConfig = NULL;
        QList <QPair<QString,GTAFailureConfig* >> lstParamConfigVal;
        QDomNodeList confiNodes = iElement.elementsByTagName(XNODE_CONFIGS);
        if (confiNodes.isEmpty()==false)
        {
            QDomNode configNode = confiNodes.at(0);
            QDomNodeList configProfileNodes = configNode.toElement().elementsByTagName(XNODE_CONFIG);

            for (int i=0;i<configProfileNodes.size();i++)
            {

                QDomElement configProfileNode = configProfileNodes.at(i).toElement();
                QString sParameterName = configProfileNode.attribute(XNODE_PARAMETER);

                GTAFailureConfigSerializer dSerializer;
                dSerializer.deserialize(configProfileNode,pConfig);
                if (NULL!=pConfig)
                {
                   lstParamConfigVal.append(qMakePair(sParameterName,pConfig));
                }
            }
           
        }

        if (false==lstParamConfigVal.isEmpty())
        {
            pFailureCommand->setParamValConfig(lstParamConfigVal);
        }
       

       /* QDomNodeList childList = iElement.childNodes();
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
                        pFailureCommand->setValues(sParameters);
                    }
                }
            }
        }

        pFailureCommand->setTableName(sFailureName);*/
        opCommand = pFailureCommand;

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
REGISTER_SERIALIZER(QString("%1").arg(ACT_FAILURE),GTAActionFailureSerializer)
