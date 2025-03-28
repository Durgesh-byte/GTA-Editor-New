#include "GTAActionDoWhileSerializer.h"
#include "GTAUtil.h"
#include "GTACommentSerializer.h"
#include "GTAActionDoWhile.h"
#include "GTARequirementSerializer.h"

GTAActionDoWhileSerializer::GTAActionDoWhileSerializer():GTACmdSerializerInterface()
{
    _pActionCmd = NULL;
}
GTAActionDoWhileSerializer::~GTAActionDoWhileSerializer()
{
    _pActionCmd = NULL;
}
void GTAActionDoWhileSerializer::setCommand(const GTACommandBase* iPCmd)
{
    _pActionCmd = dynamic_cast<GTAActionDoWhile*>((GTACommandBase*) iPCmd);
}
bool GTAActionDoWhileSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oElement)
{
    bool rc = false;
    if(_pActionCmd != NULL)
    {
        QString param = _pActionCmd->getParameter();
        QString paramVal= _pActionCmd->getValue();
        QString condition = _pActionCmd->getCondition();
        QStringList sDumpList = _pActionCmd ->getDumpList();
        QString chkAudioAlarm = (_pActionCmd->getChkAudioAlarm()==true)?XNODE_TRUE:XNODE_FALSE;
        QString chkFSOnly = (_pActionCmd->getIsFsOnly() == true)?XNODE_TRUE:XNODE_FALSE;
        QString chkValueOnly = (_pActionCmd->getIsValueOnly() == true)?XNODE_TRUE:XNODE_FALSE;
        QString chkSDIOnly = (_pActionCmd->getIsSDIOnly() == true)?XNODE_TRUE:XNODE_FALSE;
        QString chkParityOnly = (_pActionCmd->getIsParityOnly() == true)?XNODE_TRUE:XNODE_FALSE;
        QString chkRefreshOnly = (_pActionCmd->getIsRefreshRateOnly() == true)?XNODE_TRUE:XNODE_FALSE;

        oElement  = iDomDoc.createElement(XNODE_ACTION);
        oElement.setAttribute(XNODE_CHK_AUDIO_ALARM,chkAudioAlarm);
        oElement.setAttribute(XNODE_CHK_FS_ONLY,chkFSOnly);
        oElement.setAttribute(XNODE_CHK_VALUE_ONLY,chkValueOnly);
        oElement.setAttribute(XNODE_CHK_SDI_ONLY,chkSDIOnly);
        oElement.setAttribute(XNODE_CHK_PARITY_ONLY,chkParityOnly);
        oElement.setAttribute(XNODE_CHK_REFRESH_RATE_ONLY,chkRefreshOnly);

        setActionAttributes(_pActionCmd,oElement);

        QDomElement domParam = iDomDoc.createElement(XNODE_PARAMETER);
        QDomText domParamNameTxt = iDomDoc.createTextNode(param);
        domParam.appendChild(domParamNameTxt);
        oElement.appendChild(domParam);

        QDomElement domCondtion = iDomDoc.createElement(XNODE_OPERATOR);
        QDomText domCondText = iDomDoc.createTextNode(condition);
        domCondtion.appendChild(domCondText);
        oElement.appendChild(domCondtion);

        QDomElement domParamVal = iDomDoc.createElement(XNODE_VALUE);
        QDomText domParamValTxt = iDomDoc.createTextNode(paramVal);
        domParamVal.appendChild(domParamValTxt);
        oElement.appendChild(domParamVal);

        if (chkAudioAlarm==XNODE_TRUE)
        {
            QString heardAfter = _pActionCmd->getHeardAfter();
            QDomElement domhearAfterVal = iDomDoc.createElement(XNODE_HEARDAFTER);
            QDomText domHeardAfterValTxt = iDomDoc.createTextNode(heardAfter);
            domhearAfterVal.appendChild(domHeardAfterValTxt);
            oElement.appendChild(domhearAfterVal);
        }

        QString functionalStatus = _pActionCmd->getFunctionalStatus();
        QDomElement domFunctionalStatus = iDomDoc.createElement(XNODE_FUNC_STATUS);
        QDomText domFuncStatusTxt = iDomDoc.createTextNode(functionalStatus);
        domFunctionalStatus.appendChild(domFuncStatusTxt);
        oElement.appendChild(domFunctionalStatus);

        QString sSdi = _pActionCmd->getSDI();
        QDomElement domSDIStatus = iDomDoc.createElement(XNODE_SDI_STATUS);
        QDomText domSDIStatusTxt = iDomDoc.createTextNode(sSdi);
        domSDIStatus.appendChild(domSDIStatusTxt);
        oElement.appendChild(domSDIStatus);

        QString sParity = _pActionCmd->getParity();
        QDomElement domParityStatus = iDomDoc.createElement(XNODE_PARITY_STATUS);
        QDomText domParityStatusTxt = iDomDoc.createTextNode(sParity);
        domParityStatus.appendChild(domParityStatusTxt);
        oElement.appendChild(domParityStatus);

        if (!sDumpList.isEmpty())
        {
            QDomElement doDumpList = iDomDoc.createElement(XNODE_DUMPLIST);
            oElement.appendChild(doDumpList);
            for (auto dumpParameter : sDumpList)
            {
                QDomElement doDumpParamNode = iDomDoc.createElement(XNODE_DUMP_PARAM);
                doDumpList.appendChild(doDumpParamNode);
                doDumpParamNode.appendChild(iDomDoc.createTextNode(dumpParameter));
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

        rc = true;
    }
    return rc;
}

bool GTAActionDoWhileSerializer::deserialize(const QDomElement & iElement, GTACommandBase *& opCommand)
{
    bool rc = false;
    if(!iElement.isNull())
    {  


        QString actionName = iElement.attribute(XNODE_NAME);
        QString complementName = iElement.attribute(XNODE_COMPLEMENT);


        bool chkAudioAlarm = (iElement.attribute(XNODE_CHK_AUDIO_ALARM) == XNODE_TRUE)?true:false;
        bool chkFSOnly = (iElement.attribute(XNODE_CHK_FS_ONLY) == XNODE_TRUE)?true:false;
        bool chkValueOnly = (iElement.attribute(XNODE_CHK_VALUE_ONLY) == XNODE_TRUE)?true:false;
        bool chkSDIOnly = (iElement.attribute(XNODE_CHK_SDI_ONLY) == XNODE_TRUE)?true:false;
        bool chkParityOnly = (iElement.attribute(XNODE_CHK_PARITY_ONLY) == XNODE_TRUE)?true:false;
        bool chkRefreshRateOnly = (iElement.attribute(XNODE_CHK_REFRESH_RATE_ONLY) == XNODE_TRUE)?true:false;


        QStringList dumpList;
        QString isHeardAfter;
        QString param;
        QString paramValue;
        QString condition;
        QString funcStatus;
        QString sSDI;
        QString sParity;
        QDomNodeList childList = iElement.childNodes();
        for(int i = 0; i< childList.count(); i++)
        {
            QDomNode domNode = childList.at(i);
            if(domNode.isElement())
            {

                QDomElement childElem = domNode.toElement();
                QString snodename = childElem.nodeName();
                QString textVal = childElem.text();
                if(snodename == XNODE_PARAMETER)
                    param = textVal;
                else if(snodename == XNODE_VALUE)
                    paramValue = textVal;
                else if(snodename == XNODE_OPERATOR)
                    condition = textVal;
                else if (snodename == XNODE_DUMPLIST)
                {

                    QDomNodeList dumpParamNodes = domNode.childNodes();
                    int dumpNodeSize = dumpParamNodes.count();

                    for (int j =0 ;j<dumpNodeSize;j++)
                    {
                        QDomNode dumpNode = dumpParamNodes.at(j);
                        QString sDumpParameter = dumpNode.toElement().text();
                        if (!sDumpParameter.isEmpty())
                        {
                            dumpList.append(sDumpParameter);
                        }
                    }
                }
                else if (snodename == XNODE_HEARDAFTER)
                {

                    isHeardAfter = textVal;
                }
                else if(snodename == XNODE_FUNC_STATUS)
                {
                    funcStatus = textVal;
                }
                else if(snodename == XNODE_SDI_STATUS)
                {
                    sSDI = textVal;
                }
                else if(snodename == XNODE_PARITY_STATUS)
                {
                    sParity = textVal;
                }
                else
                    break;
            }
        }

        GTAActionDoWhile * pCondCmd = new GTAActionDoWhile(actionName,complementName);

        getActionAttributes(iElement,pCondCmd);
        pCondCmd->setParameter(param);
        pCondCmd->setValue(paramValue);
        pCondCmd->setChkAudioAlarm(chkAudioAlarm);
        pCondCmd->setIsFsOnly(chkFSOnly);
        pCondCmd->setIsValueOnly(chkValueOnly);
        pCondCmd->setCondition(condition);
        pCondCmd->setFunctionalStatus(funcStatus);
        pCondCmd->setIsSDIOnly(chkSDIOnly);
        pCondCmd->setSDI(sSDI);
        pCondCmd->setIsParityOnly(chkParityOnly);
        pCondCmd->setParity(sParity);
        pCondCmd->setIsRefreshRateOnly(chkRefreshRateOnly);
        
        if (chkAudioAlarm)
            pCondCmd->setHeardAfter(isHeardAfter);

        if (!dumpList.isEmpty())
            pCondCmd->setDumpList(dumpList);
        opCommand = pCondCmd;

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
REGISTER_SERIALIZER(QString("%1 %2").arg(ACT_CONDITION,COM_CONDITION_DO_WHILE),GTAActionDoWhileSerializer)
