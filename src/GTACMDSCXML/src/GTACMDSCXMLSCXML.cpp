#include "GTACMDSCXMLSCXML.h"
#include "GTASCXMLSCXML.h"
#include "GTACMDSCXMLLog.h"
#include "GTACMDSCXMLUtils.h"
#include <QDebug>

GTACMDSCXMLSCXML::GTACMDSCXMLSCXML()
{
    _TimeoutVal = "1200000ms";
}

/**
 * @brief Returns the complete string corresponding to the state chart of the element
 * @param isEnUSCXML
 * @return string of the SCXML
 */
QString GTACMDSCXMLSCXML::getSCXMLString(bool isEnUSCXML) const
{
    return getSCXMLState().getSCXMLString(isEnUSCXML);
}

void GTACMDSCXMLSCXML::setCommandList(const QList<GTACMDSCXMLBase*>& iCmdList)
{
    _CmdList = iCmdList;
}
void GTACMDSCXMLSCXML::setDebugMode(bool iEnDebugMode)
{
    _isDebugModeEn = iEnDebugMode;
}

void GTACMDSCXMLSCXML::setStandaloneMode(bool iEnStandaloneMode)
{
    _isStandAloneModeEn = iEnStandaloneMode;
}

void GTACMDSCXMLSCXML::setUnSubStart(bool iInitUnSubStart)
{
    _isInitUnSubscribeStart = iInitUnSubStart;
}

void GTACMDSCXMLSCXML::setUnSubEnd(bool iInitUnSubEnd)
{
    _isInitUnSubscribeEnd = iInitUnSubEnd;
}

void GTACMDSCXMLSCXML::setSubUnsubTimeout(QString iTimeoutVal)
{
    _TimeoutVal = iTimeoutVal;
}

/**
 * @brief: This method sets the unsubscribeFileParamOnly value which will be used to create scxml
 * @param: unsubscribeFileParamOnly : if true then only File params will be unsubscribed
 */
void GTACMDSCXMLSCXML::setUnsubscribeFileParamOnly(const bool& unsubscribeFileParamOnly)
{
    _unsubscribeFileParamOnly = unsubscribeFileParamOnly;
}

/**
 * \brief Formulates the SCXML object for state chart command container
 * \return SCXML object
 */
GTASCXMLSCXML GTACMDSCXMLSCXML::getSCXMLState() const
{
    GTASCXMLSCXML scxml;

    GTASCXMLState mainState;
    QString mainStateId = "TestProcedure";
    mainState.setId(mainStateId);

    GTASCXMLTransition tx;
    tx.setTarget(SCXML_FINAL_STATE_ID);

    QList<GTAICDParameter> paramList;
    if (!_CmdList.isEmpty())
    {
        for (auto& pCmdSCXML : _CmdList)
        {
            if (pCmdSCXML != nullptr)
            {
                pCmdSCXML->setDebugModeStatus(_isDebugModeEn);
                pCmdSCXML->getParamInfoList(paramList, true);
                mainState.insertStates(pCmdSCXML->getSCXMLState());
            }
        }
    }

    GTASCXMLDataModel dataModel;
    for (int i = 0; i < paramList.count(); i++)
    {
        GTAICDParameter param = paramList.at(i);
        GTASCXMLData dataId;
        QString resolvedParam = param.getTempParamName();
        QString resolvedToolType = param.getToolType();
        if (!resolvedParam.contains("$") && (resolvedToolType != INTERNAL_PARAM)/*!(resolvedParam.contains("_line_arr[") && resolvedParam.contains("_arr_cntr]"))*/)
        {
            //GTACMDSCXMLUtils::updateWithGenToolParam(resolvedParam);
            QStringList Bool_List;
            Bool_List << "true" << "false";
            bool testDouble = false;
            bool testInt = false;
            resolvedParam.toDouble(&testDouble);
            resolvedParam.toInt(&testInt, 10);
            if (!(resolvedParam.startsWith("'") && resolvedParam.endsWith("'")) && !(testDouble) && !(testInt) && !Bool_List.contains(resolvedParam))
            {
                dataId.setDataId(resolvedParam);
                if (param.getToolName() != "NA")
                {
                    //dataId.setDataExpr(QString("{'ParameterType':'%1'}").arg(param.getValueType()));
                    //dataModel.insertDataItem(dataId);
                }
                else
                {
                    dataId.setDataExpr("");
                    dataModel.insertDataItem(dataId);
                }
            }
        }
    }

    mainState.setDataModel(dataModel);

    //    if(!_isDebugModeEn)
    //    {
    if (!_isStandAloneModeEn)
    {
        GTASCXMLState InitState;
        QString initStateId = "InitStateId";
        InitState.setId(initStateId);

        QString subStateId = "SubscribeApps";
        QString unSubStateId = "UnSubscribeAll";

        GTASCXMLState unSubState = GTACMDSCXMLUtils::createUnSubscribeAll(paramList, unSubStateId, subStateId);
        GTASCXMLState subState = GTACMDSCXMLUtils::createSubscribeForApps(paramList, subStateId, QString("%1_final").arg(initStateId));


        GTASCXMLOnEntry onEntryUnSub = unSubState.getOnEntry();
        GTASCXMLOnEntry onEntrySub = subState.getOnEntry();
        GTASCXMLOnExit onExitUnSub = unSubState.getOnExit();
        GTASCXMLOnExit onExitSub = subState.getOnExit();

        QString unSubEventName = QString("%1_TimeoutEvent").arg(unSubState.getId());
        QString unSubEventID = QString("%1_TimeoutEventID").arg(unSubState.getId());
        QString subEventName = QString("%1_TimeoutEvent").arg(subState.getId());
        QString subEventID = QString("%1_TimeoutEventID").arg(subState.getId());
        GTASCXMLSend timeoutSend;
        GTASCXMLCancel timeoutCancel;

        timeoutSend.setEvent(unSubEventName);
        timeoutSend.setId(unSubEventID);
        timeoutSend.setDelay(_TimeoutVal);
        onEntryUnSub.insertSend(timeoutSend);
        timeoutCancel.setSendEventId(subEventID);
        onExitUnSub.insertCancel(timeoutCancel);

        timeoutSend.setEvent(subEventName);
        timeoutSend.setId(subEventID);
        onEntrySub.insertSend(timeoutSend);
        timeoutCancel.setSendEventId(unSubEventID);
        onExitSub.insertCancel(timeoutCancel);

        unSubState.setOnEntry(onEntryUnSub);
        unSubState.setOnExit(onExitUnSub);
        subState.setOnEntry(onEntrySub);
        subState.setOnExit(onExitSub);


        GTASCXMLTransition subTimeoutTx;
        subTimeoutTx.setTarget(QString("%1_final").arg(initStateId));
        subTimeoutTx.setEvent(subEventName);
        subTimeoutTx.setStatus("Timeout");

        subState.insertTransitions(subTimeoutTx, true);

        GTASCXMLTransition unSubTimeoutTx;
        unSubTimeoutTx.setTarget(subStateId);
        unSubTimeoutTx.setEvent(unSubEventName);
        unSubTimeoutTx.setStatus("Timeout");

        //unSubState.insertTransitions(unSubTimeoutTx,true);
        unSubState.insertTransitions(unSubTimeoutTx);



        if (_isInitUnSubscribeStart)
            InitState.insertStates(unSubState);

        InitState.insertStates(subState);

            GTASCXMLTransition subTx;
            subTx.setTarget(mainStateId);
            InitState.insertTransitions(subTx);
            scxml.insertState(InitState);
            scxml.setToolVersion(QString(TOOL_VERSION_STR));

        if (_isInitUnSubscribeEnd)
        {
            QString scxmlUnSubEndStateId = "UnSubscribeAllAtEnd";

                //inform the server when the procedure is ending
                if(_isDebugModeEn)
                {
                    GTAICDParameter param;
                    param.setSignalName("GTA_DEBUG");
                    param.setTempParamName("GTA_DEBUG");
                    param.setToolName("GTA_DEBUG");
                    paramList.append(param);
                }

				GTASCXMLState unSubStateEnd = GTACMDSCXMLUtils::createUnSubscribeAll(paramList, scxmlUnSubEndStateId, SCXML_FINAL_STATE_ID, _unsubscribeFileParamOnly);

                GTASCXMLSend subTimeoutSend;
                GTASCXMLOnEntry unSub = unSubStateEnd.getOnEntry();
                subTimeoutSend.setEvent(scxmlUnSubEndStateId);
                subTimeoutSend.setDelay(_TimeoutVal);
                unSub.insertSend(subTimeoutSend);
                unSubStateEnd.setOnEntry(unSub);
                subTx.setTarget(scxmlUnSubEndStateId);
                mainState.insertTransitions(subTx);
                scxml.insertState(mainState);
                scxml.insertState(unSubStateEnd);
            }
            else
            {
                subTx.setTarget(SCXML_FINAL_STATE_ID);
                mainState.insertTransitions(subTx);
                scxml.insertState(mainState);
            }
        }
        else
        {
            mainState.insertTransitions(tx);
            scxml.insertState(mainState);
        }
//    }
    return scxml;
}