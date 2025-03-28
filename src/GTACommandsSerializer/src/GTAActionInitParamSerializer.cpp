#include "GTAActionInitParamSerializer.h"
#include "GTAUtil.h"
#include "GTACommentSerializer.h"
#include "GTAActionSubscribe.h"
#include "GTARequirementSerializer.h"
GTAActionInitParamSerializer::GTAActionInitParamSerializer():GTACmdSerializerInterface()
{
    _pActionCmd = NULL;
}
void GTAActionInitParamSerializer::setCommand(const GTACommandBase* ipCmd)
{
    _pActionCmd = dynamic_cast<GTAActionSubscribe*>((GTACommandBase*)ipCmd);

}

bool GTAActionInitParamSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oElement)//(QDomDocument &iDomDoc,QDomElement &oElement)
{
    bool rc = false;
    if(_pActionCmd != NULL)
    {
        
        QString actionName = _pActionCmd->getAction();
        QString complementName = _pActionCmd->getComplement();

        if(actionName != ACT_SUBSCRIBE)
            return rc;
        else
            rc = true;


        if(complementName == COM_SUBSCRIBE_PARAM)
        {
            QStringList sParameters = _pActionCmd->getValues();
            oElement  = iDomDoc.createElement(XNODE_ACTION);

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
            domParameter.setAttribute(XATTR_VAL,sParameter);
            oElement.appendChild(domParameter);

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

bool GTAActionInitParamSerializer::deserialize(const QDomElement &iElement, GTACommandBase *&opCommand)//(const QDomElement & ioElement)
{
    bool rc = false;
    if(!iElement.isNull())
    {
        QString sActionName = iElement.attribute(XNODE_NAME);
        QString sComplement = iElement.attribute(XNODE_COMPLEMENT);



        GTAActionSubscribe* pSubscribeCmd = new GTAActionSubscribe(sActionName,sComplement);
        getActionAttributes(iElement,pSubscribeCmd);

        if(sComplement == COM_SUBSCRIBE_PARAM)
        {
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
                        sParameters= paramVals.split(";");
                        pSubscribeCmd->setValues(sParameters);
                    }
                }
            }
        }
        opCommand = pSubscribeCmd;

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
REGISTER_SERIALIZER(QString("%1 %2").arg(ACT_SUBSCRIBE,COM_SUBSCRIBE_PARAM),GTAActionInitParamSerializer)
