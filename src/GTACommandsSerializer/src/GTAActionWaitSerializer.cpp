#include "GTAActionWaitSerializer.h"
#include "GTAUtil.h"
#include "GTACommentSerializer.h"
#include "GTAActionWait.h"
#include "GTARequirementSerializer.h"
GTAActionWaitSerializer::GTAActionWaitSerializer():GTACmdSerializerInterface()
{
    _pActionCmd = NULL;
}
GTAActionWaitSerializer::~GTAActionWaitSerializer()
{
    _pActionCmd = NULL;
}
void GTAActionWaitSerializer::setCommand(const GTACommandBase* iPCmd)
{
    _pActionCmd = dynamic_cast<GTAActionWait*>((GTACommandBase*) iPCmd);
}
bool GTAActionWaitSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oElement)
{
    bool rc = false;
    if(_pActionCmd != NULL)
    {
        QString actionName = _pActionCmd->getAction();
        

        if(actionName != ACT_WAIT)
            return rc;
        else
            rc = true;

        oElement  = iDomDoc.createElement(XNODE_ACTION);

        QString complement = _pActionCmd->getComplement();
        QString param = _pActionCmd->getParameter();
        QString paramVal = _pActionCmd->getValue();
        QString condition = _pActionCmd->getCondition();
        QString counter = _pActionCmd->getCounter();
        QStringList sDumpList = _pActionCmd->getDumpList();
        QString onFail =  _pActionCmd->getActionOnFail();
        QString functionalStatus = _pActionCmd->getFunctionalStatus();
        QString isFSOnly = (_pActionCmd->getIsFsOnly() == true)? XNODE_TRUE:XNODE_FALSE;
        QString isValueOnly = (_pActionCmd->getIsValueOnly() == true)? XNODE_TRUE:XNODE_FALSE;
        QString chkSDIOnly = (_pActionCmd->getIsSDIOnly() == true)?XNODE_TRUE:XNODE_FALSE;
        QString chkParityOnly = (_pActionCmd->getIsParityOnly() == true)?XNODE_TRUE:XNODE_FALSE;
        QString chkRefreshOnly = (_pActionCmd->getIsRefreshRateOnly() == true)?XNODE_TRUE:XNODE_FALSE;
        QString isLoopSampling = (_pActionCmd->getIsLoopSampling() == true)?XNODE_TRUE:XNODE_FALSE;
		// These values are written only when Loop Sampling is enabled
        QString loopSamplingValue = (_pActionCmd->getIsLoopSampling() == true) ? _pActionCmd->getLoopSampling():QString();
        QString loopSamplingUnit = (_pActionCmd->getIsLoopSampling() == true) ? _pActionCmd->getUnitLoopSampling(): QString();
        QString sSDI = _pActionCmd->getSDI();
        QString sParity = _pActionCmd->getParity();
        setActionAttributes(_pActionCmd,oElement);

        QDomElement paramElem  = iDomDoc.createElement(XNODE_PARAMETER);
        paramElem.setAttribute(XNODE_NAME,param);
        paramElem.setAttribute(XNODE_VALUE,paramVal);
        paramElem.setAttribute(XNODE_CONDITION,condition);
        paramElem.setAttribute(XNODE_CHK_FS_ONLY,isFSOnly);
        paramElem.setAttribute(XNODE_CHK_VALUE_ONLY,isValueOnly);
        paramElem.setAttribute(XNODE_FUNC_STATUS,functionalStatus);
        paramElem.setAttribute(XNODE_CHK_SDI_ONLY,chkSDIOnly);
        paramElem.setAttribute(XNODE_CHK_PARITY_ONLY,chkParityOnly);
        paramElem.setAttribute(XNODE_CHK_REFRESH_RATE_ONLY,chkRefreshOnly);
        paramElem.setAttribute(XNODE_SDI_STATUS,sSDI);
        paramElem.setAttribute(XNODE_PARITY_STATUS,sParity);
        paramElem.setAttribute(XNODE_LOOP_SAMPLING_STATUS, isLoopSampling);
        paramElem.setAttribute(XNODE_LOOP_SAMPLING_VALUE, loopSamplingValue);
        paramElem.setAttribute(XNODE_LOOP_SAMPLING_UNIT, loopSamplingUnit);
        oElement.appendChild(paramElem);

        QDomElement counterElem = iDomDoc.createElement(XNODE_PARAMETER);
        counterElem.setAttribute(XNODE_NAME,"LOOP_COUNTER");
        counterElem.setAttribute(XNODE_VALUE,counter);
        oElement.appendChild(counterElem);

        if (complement == COM_WAIT_UNTIL)
        {
            if (onFail == ACT_FAIL_STOP)
            {
                if (!sDumpList.isEmpty())
                {
                    QDomElement doDumpList = iDomDoc.createElement(XNODE_DUMPLIST);
                    oElement.appendChild(doDumpList);
                    foreach(QString dumpParameter, sDumpList)
                    {
                        QDomElement doDumpParamNode = iDomDoc.createElement(XNODE_DUMP_PARAM);
                        QDomText domParamValTxt = iDomDoc.createTextNode(dumpParameter);
                        doDumpList.appendChild(doDumpParamNode);
                        doDumpParamNode.appendChild(domParamValTxt);

                    }
                }
            }
        }
        //Comment
        GTACommentSerializer commentSerializer(_pActionCmd);
        QDomElement commentElem;
        if( commentSerializer.serialize(iDomDoc,commentElem) && ! commentElem.isNull())
            oElement.appendChild(commentElem);

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

bool GTAActionWaitSerializer::deserialize(const QDomElement & iElement, GTACommandBase *& opCommand)
{
    bool rc = false;
    if(!iElement.isNull())
    {

        QString actionName = iElement.attribute(XNODE_NAME);
        QString complement = iElement.attribute(XNODE_COMPLEMENT);

        if(actionName == ACT_WAIT)
            rc = true;
        else return false;

        GTAActionWait::WaitType type = GTAActionWait::UNTIL;
        if(complement == COM_WAIT_FOR)
            type = GTAActionWait::FOR;
        else if(complement == COM_WAIT_UNTIL)
            type = GTAActionWait::UNTIL;

        GTAActionWait * pCmd = new GTAActionWait(actionName,complement,type);
        getActionAttributes(iElement,pCmd);

        QDomNodeList childList = iElement.childNodes();
        for(int i = 0; i < childList.count(); i++)
        {
            QDomNode childNode = childList.at(i);
            if(childNode.isElement())
            {
                QDomElement childElem = childNode.toElement();
                QString nameAttr = childElem.attribute(XNODE_NAME);
                QString nodeName = childElem.nodeName();

                if(nodeName == XNODE_PARAMETER && nameAttr == "LOOP_COUNTER")
                {
                    QString loopCounter = childElem.attribute(XNODE_VALUE);
                    pCmd->setCounter(loopCounter);
                }
                else if(nodeName == XNODE_PARAMETER)
                {
                    QString paramName =nameAttr;
                    QString paramVal = childElem.attribute(XNODE_VALUE);
                    QString condition = childElem.attribute(XNODE_CONDITION);
                    QString functionalStatus = childElem.attribute(XNODE_FUNC_STATUS);
                    bool isFSOnly = (childElem.attribute(XNODE_CHK_FS_ONLY) == XNODE_TRUE)? true:false;
                    bool isValOnly = (childElem.attribute(XNODE_CHK_VALUE_ONLY) == XNODE_TRUE) ? true:false;

                    bool chkSDIOnly = (childElem.attribute(XNODE_CHK_SDI_ONLY) == XNODE_TRUE)?true:false;
                    bool chkParityOnly = (childElem.attribute(XNODE_CHK_PARITY_ONLY) == XNODE_TRUE)?true:false;
                    bool chkRefreshRateOnly = (childElem.attribute(XNODE_CHK_REFRESH_RATE_ONLY) == XNODE_TRUE)?true:false;
                    QString sSDI = childElem.attribute(XNODE_SDI_STATUS);
                    QString sParity = childElem.attribute(XNODE_PARITY_STATUS);
                    bool isLoopSampling = (childElem.attribute(XNODE_LOOP_SAMPLING_STATUS) == XNODE_TRUE) ? true:false;
                    QString sLoopSampling = childElem.attribute(XNODE_LOOP_SAMPLING_VALUE);
                    QString sLoopSamplingUnit = childElem.attribute(XNODE_LOOP_SAMPLING_UNIT);

                    pCmd->setParameter(paramName);
                    pCmd->setValue(paramVal);
                    pCmd->setCondition(condition);
                    pCmd->setIsFsOnly(isFSOnly);
                    pCmd->setIsValueOnly(isValOnly);
                    pCmd->setFunctionalStatus(functionalStatus);
                    pCmd->setIsParityOnly(chkParityOnly);
                    pCmd->setIsSDIOnly(chkSDIOnly);
                    pCmd->setIsRefreshRateOnly(chkRefreshRateOnly);
                    pCmd->setSDI(sSDI);
                    pCmd->setParity(sParity);
                    pCmd->setIsLoopSampling(isLoopSampling);
                    pCmd->setLoopSampling(sLoopSampling);
                    pCmd->setUnitLoopSampling(sLoopSamplingUnit);
                }
                else
                {
                    if (type == GTAActionWait::UNTIL)
                    {
                        QStringList dumpList;
                        if (nodeName == XNODE_DUMPLIST)
                        {
                            QDomNodeList dumpParamNodes = childNode.childNodes();
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
                        if (!dumpList.isEmpty())
                        {
                            pCmd->setDumpList(dumpList);
                        }
                    }
                }
            }
        }
        opCommand = pCmd;

        GTACommentSerializer commentSerializer(NULL);
        commentSerializer.deserialize(iElement,opCommand);


        //requiremrnt node
        GTARequirementSerializer requirementSerializer(NULL);
        requirementSerializer.deserialize(iElement,opCommand);

    }
    return rc;
}
#include "GTASerilizerRegisterDef.h"
REGISTER_SERIALIZER_MULTI(QString("%1 %2").arg(ACT_WAIT,COM_WAIT_FOR),GTAActionWaitSerializer,obj1)
REGISTER_SERIALIZER_MULTI(QString("%1 %2").arg(ACT_WAIT,COM_WAIT_UNTIL),GTAActionWaitSerializer,obj2)
