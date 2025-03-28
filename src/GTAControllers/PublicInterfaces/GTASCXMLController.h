#ifndef GTASCXMLCONTROLLER_H
#define GTASCXMLCONTROLLER_H

#include "GTAElement.h"
#include "GTASCXMLUtils.h"
#include "GTASCXMLSCXML.h"
#include "GTACommandList.h"
#include "GTAICDParameter.h"
#include "GTAAppController.h"

#pragma warning(push, 0)
#include <QList>
#pragma warning(pop)

//TODO: This is a warning against assigning Rvalues to references, which is done in default arguments for many methods in this file,
//      This will be fixed in the future, as this refactor will take some time (there are a lot od default arguments which will be broken by removing default argument from some of the refs
#pragma warning (disable : 4239)
class GTASCXLMParamInfo
{
    QString _appName;
    QHash<QString,QString> busMediaInfo;
    bool isInitNeeded;

public:
    void insertParamInfo(const QString &iAppName, const QString &iBusName, const QString &iMedia, bool iIsInitNeeded)
    {
        _appName = iAppName;
        isInitNeeded = iIsInitNeeded;
        if(!busMediaInfo.contains(iBusName))
        {
            busMediaInfo.insert(iBusName,iMedia);
        }
    }
    bool operator ==(const GTASCXLMParamInfo & iObj) const
    {
        return (this->_appName == iObj._appName);
    }
    QString getId(){return _appName;}
    bool getIsInitNeeded(){return isInitNeeded;}
    QString getJsonExpr() const
    {
        QStringList buses = busMediaInfo.keys();
        QStringList content;
        content.append(QString("'Name':'%1'").arg(_appName));
        for(int i=0;i<buses.count();i++)
        {
            QString bus = buses.at(i);
            QString media = busMediaInfo.value(bus);
            content.append(QString("'%1':{'Media':'%2'}").arg(bus,media));
        }

        return QString("{%1}").arg(content.join(","));
    }
};

class GTASCXMLController
{



    GTAElement *_pElement;
    bool _isPirInit;
    QList<GTAElement *> _IntermediateElemList;
    GTASCXMLSCXML _Scxml;
    QString _LastError;
    QString _elementName;
    QString _ScxmlTemplatePath;
    int StateIdCntr;
    QStringList _subscribeParamList;
    QHash<QString, GTAICDParameter> _icdParamList;
    QList<QString> cmdParameterList;
    QHash<QString, QString> _stateIdMap;
    QHash<QString,QString> _genToolReturnMap;
    QHash<QString,QString> _genToolParams;
    bool _isGenericSCXMLCofig;
    QStringList ExternalSubscribeParams;
    GTAAppController *_pAppCtrl;
    enum paramType {PARAM_ICD=1,PARAM_LOCAL=2, PARAM_NUM=3,PARAM_STRING=4,PARAM_UNKNOWN=5};
    enum condType {COND_SUCCESS=1,COND_FAIL=2, COND_TIMEOUT=3,COND_NONE=4,COND_UNKNOWN=5};
public:
    GTASCXMLController(const QString &iTemplatePath);

    bool exportToScxml(const GTAElement * ipElement,const QString &iFilePath,bool iHasUnsubscibeAtStart,bool iHasUnsubscibeAtEnd);
    QString getLastError(){return _LastError;}

private:

    void setElementName(GTAElement* pElement,QString iSaveFilePath);

    void GetParamList(QStringList &iVariableList);
    void GroupElementByTitleComplete(const GTAElement *pElem, QStringList &oVariableList, GTAElement *oGroupedElem);
    void createCommandGroupedByTitle(GTACommandBase*& ipCmd, GTAActionParentTitle *&ipTitleCmd,QStringList &oVariableList);

    bool getStateChart(const GTAElement *pElem,GTASCXMLState &ioState);
    bool getStateChart(const GTACommandBase *pCmd,GTASCXMLState &ioState);
    void createStateNodeForCommand(GTACommandBase *pCmd,GTASCXMLState &ioState, const QString &iStateId, const QString &iTargetId);
    void setStateIDName(const QString& iStateID, const GTACommandBase* pCmd);


    void createSCXMLPrintMessage(const GTAActionPrint *iPrintCmd,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QStringList &iExtTimeouts);
    void createSCXMLPrintParameter(const GTAActionPrint *iPrintCmd,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QStringList &iExtTimeouts);
    void createSCXMLPrintTable(const GTAActionPrintTable	 *iPrintTableCmd,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QStringList &iExtTimeouts);
    void createSCXMLPrintTime(const GTAActionPrintTime *pPrintTime, GTASCXMLState &iState,const QString &iStateId, const QString &iTargetId,const QStringList &iExtTimeouts);
    void createSCXMLReleaseParam(const GTAActionRelease *pReleaseParam, GTASCXMLState &iState,const QString &iStateId, const QString &iTargetId,const QStringList &iExtTimeouts);
    void createSCXMLTitle(const GTAActionParentTitle *iTitleCmd,GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QStringList &iExtTimeouts);
    void createSCXMLWaitFor(const GTAActionWait *iWaitCmd,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QStringList &iExtTimeouts);
    void createSCXMLWaitUntil(const GTAActionWait *iWaitCmd,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QList<GTASCXMLTransition> &iExtTransitions= QList<GTASCXMLTransition>(), const QString &iConfEventName = QString());
    void createSCXMLCall(const GTAActionExpandedCall *pCallCmd, GTASCXMLState &iState,const QString &iStateId, const QString &iTargetId,const QStringList &iExtTimeouts);
    void createSCXMLConditionIf(const GTAActionIF *pIfCmd, GTASCXMLState &iState,const QString &iStateId, const QString &iTargetId,const QStringList &iExtTimeouts);
    void createSCXMLCheckAudioAlarm(const GTACheckAudioAlarm *iAudioAlarm, GTASCXMLState &iState,const QString &iStateId, const QString &iTargetId,const QStringList &iExtTimeouts, const QString &iOperator=QString());
    void createSCXMLConditionWhile(const GTAActionWhile *iWhileCmd, GTASCXMLState &iState,const QString &iStateId, const QString &iTargetId,const QStringList &iExtTimeouts);
    void createSCXMLConditionDoWhile(const GTAActionDoWhile *iDoWhileCmd, GTASCXMLState &iState,const QString &iStateId, const QString &iTargetId,const QStringList &iExtTimeouts);
    void createSCXMLSet(const GTAActionSet *iSetCmd, GTASCXMLState &iState,const QString &iStateId, const QString &iTargetId,const QStringList &iExtTimeouts, const QString &iInvokeDealy);
    void createSCXMLSetList(const GTAActionSetList *iSetCmdList, GTASCXMLState &iState,const QString &iStateId, const QString &iTargetId,const QStringList &iExtTimeouts);
    void createSCXMLManualAction(const GTAActionManual *iManActCmd,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QStringList &iExtTimeouts);
    void createSCXMLGenericFunction(const GTAActionGenericFunction *iGenFunc,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QStringList &iExtTimeouts);
    void createSCXMLMaths(const GTAActionMaths *iMaths,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QStringList &iExtTimeouts);
    void createSCXMLCheckFWCWarning(const GTACheckFwcWarning *iChkFwcWarning,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QStringList &iExtTimeouts);
    void createSCXMLCheckBITEMessage(const GTACheckBiteMessage *iChkBiteMessage,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QStringList &iExtTimeouts);
    void createSCXMLCheckValue(const GTACheckValue *iChkValue,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QList<GTASCXMLTransition> &iExtTransitions, const QString &iConfEventName);
    void createSCXMLOneClickPPC(const GTAActionOneClickPPC *iPpcCmd,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QStringList &iExtTimeouts);
    void createSCXMLOneClickEnv(const GTAOneClick *iOneClickEnvCmd,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QStringList &iExtTimeouts);
    void createSCXMLOneClickStartEnv(const GTAOneClick *iOneClickEnvCmd,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QStringList &iExtTimeouts);
    void createSCXMLOneClickStopAlto(const GTAOneClick *iOneClickEnvCmd,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QStringList &iExtTimeouts);
    void createSCXMLOneClickStopTestConfig(const GTAOneClick *iOneClickEnvCmd,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QStringList &iExtTimeouts);
    void createSCXMLOneClickApplication(const GTAOneClickLaunchApplication *iOneClickAppCmd,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QStringList &iExtTimeouts);
    void createSCXMLOneClickLaunchApp(const GTAOneClickLaunchApplication *iOneClickAppCmd,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QStringList &iExtTimeouts);
    void createSCXMLParallelCall(const GTAActionCallProcedures *pParallelCallCmd,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QStringList &iExtTimeouts);
    void createSCXMLGenericTool(const GTAGenericToolCommand *pGenToolCmd,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QStringList &iExtTimeouts);
    void createSCXMLSetGeneric(const GTAActionSet *iSetCmd, GTASCXMLState &iState,const QString &iStateId, const QString &iTargetId,const QStringList &iExtTimeouts);
    void createSCXMLSubscribe(const GTAActionSubscribe *iSubscribeCmd,GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QStringList &iExtTimeouts);
    void createSCXMLUnSubscribe(const GTAActionUnSubscribe *iSubscribeCmd,GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QStringList &iExtTimeouts);

    void createSCXMLDataModel(GTASCXMLSCXML &iSCXMLNode);
    QString getParameterExpression(const QString &iParamId) const;

    bool getStateForParamGeneric(GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId,const QString &iParam);
    bool getStateForParam(GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId,const QString &iParam, const QString &iCmdName, bool iAddHiddenWaitTime = true);
    bool getInitStateForParameter(GTASCXMLState &ioState,const  GTAICDParameter &iParam,const  QString &iStateId, const QString &iTargetId, const QString &iFinalId, const GTACommandBase *pCmd = NULL);
    bool getUnInitStateForParameter(GTASCXMLState &ioState,const  GTAICDParameter &iParam,const  QString &iStateId, const QString &iTargetId, const QString &iFinalId, const GTACommandBase *pCmd = NULL);
    bool getSubscribeInvokeForParam(GTASCXMLInvoke &ioInvoke);
    bool getInvokeForParam(GTASCXMLInvoke &ioInvoke, const QString &iParam, const QString &iInvokId, const QString &iCmdName);
    bool getInvokeForParamGeneric(GTASCXMLInvoke &ioInvoke, const QString &iInvokId);
    bool getCloneInvoke(const QString & iTemplateFileName, GTASCXMLInvoke &iInvoke);
    bool openScxmlTemplateFile(const QString & iTemplateFileName, QDomDocument & oDomDoc);
    bool getInvokeDealy(const QString iCmdName, QString &oDelay);
    bool getWaitState(const GTACommandBase *pCmd,const QString &iDelay,const QString iTarget,const QString iStateId, GTASCXMLState &ioWaitState);
    void getTimeoutTranstion(const GTACommandBase *pCmd,const QString &iEventName, const QString &iTargetId, GTASCXMLTransition &oTimeoutTx, const QString &iTimeoutCond = QString());

    void getInitStateForApplications(GTASCXMLState &oState, const QString &iStateId, const QString &iTarget);
    void getUnInitState(GTASCXMLState &oState, const QString &iStateId, const QString &iTarget);

    void getStateForDumpListParameters(const GTACommandBase *pCmd,GTASCXMLState &iState, const QString iStateId, const QString iTargetId);

    void insertSuccessTrasition(const GTACommandBase *pCmd,const QStringList &iIcdParamList,const QString &iTargetId,const QString &iEventName, GTASCXMLState &ioState, QString &iExtCond = QString(), QString &iTimeOutCond = QString(), bool iAddLog = true);
    void insertFailTrasition(const GTACommandBase *pCmd,const QStringList &iIcdParamList,const QString &iTargetId,const QString &iEventName, GTASCXMLState &ioState, QString &iExtCond= QString(), bool iForceSuccess = false, QString &iTimeOutCond = QString(), bool iAddLog = true);
    void insertTimeOutTrasitionWoEvent(const GTACommandBase *pCmd, const QString &iTargetId, const QString &iEventName, GTASCXMLState &ioState, QString &iTimeoutCond);
    void insertTimeoutTransition(const GTACommandBase *pCmd,const QString &iEventName, const QString &iTargetId, GTASCXMLState &ioState);
    void insertTimeoutInState(const GTACommandBase *pCmd,const QStringList &iIcdParamList, GTASCXMLState &ioState);
    void getConditionStatement(const GTACommandBase *pCmd,const QStringList &iIcdParamList, QString &oCond, const GTASCXMLController::condType &iCondType);
    bool getIsActionOnFailForCmd(const GTACommandBase *pCmd);
    void logAllParametersOnExit(const GTACommandBase *pCmd, const QStringList &iParamList, GTASCXMLState &ioState);
    GTASCXMLController::paramType getParamType(const QString &iParam);
    void updateWithGenToolParam(QString &ioParam);
};

#endif // GTASCXMLCONTROLLER_H
