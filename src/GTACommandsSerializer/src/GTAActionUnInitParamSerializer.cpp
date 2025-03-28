#include "GTAActionUnInitParamSerializer.h"
#include "GTAUtil.h"
#include "GTACommentSerializer.h"
#include "GTAActionUnSubscribe.h"
#include "GTARequirementSerializer.h"

GTAActionUnInitParamSerializer::GTAActionUnInitParamSerializer():GTACmdSerializerInterface()
{
    _pActionCmd = NULL;
}
void GTAActionUnInitParamSerializer::setCommand(const GTACommandBase* ipCmd)
{
    _pActionCmd = dynamic_cast<GTAActionUnSubscribe*>((GTACommandBase*)ipCmd);

}

bool GTAActionUnInitParamSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oElement)//(QDomDocument &iDomDoc,QDomElement &oElement)
{
    bool rc = false;
    if(_pActionCmd != NULL)
    {

        QString actionName = _pActionCmd->getAction();
        QString complementName = _pActionCmd->getComplement();

        if(actionName != ACT_UNSUBSCRIBE)
            return rc;
        else
            rc = true;


        if(complementName == COM_UNSUBSCRIBE_PARAM)
        {

            bool isReleaseAll = _pActionCmd->isReleaseAll();
            oElement  = iDomDoc.createElement(XNODE_ACTION);
            setActionAttributes(_pActionCmd,oElement);
            if(isReleaseAll)
            {
                QDomElement domReleaseAll = iDomDoc.createElement("ALL_PARAMS");
                domReleaseAll.setAttribute(XATTR_VAL,XNODE_TRUE);
                oElement.appendChild(domReleaseAll);
            }
            else
            {
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

bool GTAActionUnInitParamSerializer::deserialize(const QDomElement &iElement, GTACommandBase *&opCommand)//(const QDomElement & ioElement)
{
    bool rc = false;
    if(!iElement.isNull())
    {
        QString sActionName = iElement.attribute(XNODE_NAME);
        QString sComplement = iElement.attribute(XNODE_COMPLEMENT);



        GTAActionUnSubscribe* pUnSubscribeCmd = new GTAActionUnSubscribe(sActionName,sComplement);
        getActionAttributes(iElement,pUnSubscribeCmd);

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
                    QString paramVals = childElem.attribute(XNODE_VALUE);
                    if(nodeName == XNODE_PARAMETER)
                    {
                        sParameters= paramVals.split(";");
                        pUnSubscribeCmd->setValues(sParameters);
                    }
                    else if(nodeName == "ALL_PARAMS")
                    {
                        pUnSubscribeCmd->setReleaseAll(true);
                        break;
                    }
                }
            }
        }
        opCommand = pUnSubscribeCmd;

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
REGISTER_SERIALIZER(QString("%1 %2").arg(ACT_UNSUBSCRIBE,COM_UNSUBSCRIBE_PARAM),GTAActionUnInitParamSerializer)
