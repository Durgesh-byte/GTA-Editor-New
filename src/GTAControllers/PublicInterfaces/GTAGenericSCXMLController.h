#ifndef GTAGENERICSCXMLCONTROLLER_H
#define GTAGENERICSCXMLCONTROLLER_H


#include "GTAAppController.h"
#include "GTASCXMLSCXML.h"
#include "GTASCXMLState.h"
#include "GTACommandBase.h"
#include "GTACommandList.h"
#include "GTAICDParameter.h"

class GTAControllers_SHARED_EXPORT GTAGenericSCXMLController
{
private:
    GTAAppController *_pAppCtrl;
    GTAElement *_pElement;
    QString _ScxmlTemplatePath;
    bool _isGenericSCXMLCofig;
    bool _EnGenerateInvokeForLocalVar;
    QString _LastError;
    QString _elementName;
    QHash<QString, GTAICDParameter> _dbParamList;
    QHash<QString,QString> _genToolReturnMap;
    QStringList cmdParameterList;

    QList<GTAHeader *> _headerLists;
public:
    GTAGenericSCXMLController(const QString &iTemplatePath, bool iGenInvokeForLocal);
    bool exportToScxml(const GTAElement * ipElement,const QString &iFilePath);
    QString getLastError();

private:
    enum ParamType {PARAM_DB=1,PARAM_LOCAL=2, PARAM_NUM=3,PARAM_STRING=4,PARAM_UNKNOWN=5};
    ParamType getParamType(const QString &iParam);

    void GetParamList( QStringList &iVariableList);

    void setElementName(GTAElement* pElement,QString iSaveFilePath);
    void GroupElementByTitleComplete(const GTAElement *pElem, QStringList &oVariableList, GTAElement *oGroupedElem);
    void createCommandGroupedByTitle(GTACommandBase*& ipCmd, GTAActionParentTitle *&ipTitleCmd,QStringList &oVariableList);

    bool getStateChart(const GTAElement *pElem,GTASCXMLState &ioState, const bool isCallCmd = false, QString tpFinalStateId= QString());
    bool getStateChart(const GTACommandBase *pCmd,GTASCXMLState &ioState);
    void createStateNodeForCommand(GTACommandBase *pCmd,GTASCXMLState &ioState, const QString &iStateId, const QString &iTargetId);

    bool openScxmlTemplateFile(const QString & iTemplateFileName, QDomDocument & oDomDoc);
    bool getCloneInvoke(const QString & iTemplateFileName, GTASCXMLInvoke &iInvoke);
    void insertInvokeForParam(const QStringList &iParamList, GTASCXMLState &oState, QStringList &oEventList, int id = -1,bool isFSOnly = false, bool isValueOnly=false);
    bool getInvokeForParamGeneric(GTASCXMLInvoke &ioInvoke, const QString &iInvokId);
    bool getInvokeForSetParamGeneric(GTASCXMLInvoke &ioInvoke, const QString &iInvokId);
    void insertInvokeForSetParam(const QString &iParam, const QString &iVal, const QString &iFS, const bool &iSetOnlyFS,GTASCXMLState &oState, QStringList &oEventList, bool iSetOnlyValue = false, int id=-1);

    void createSCXMLDataModel(GTASCXMLSCXML &iSCXMLNode);

    void createGenericSCXMLTitle(const GTAActionParentTitle *iTitleCmd,GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId);
    void createGenericSCXMLPrintMessage(const GTAActionPrint *iPrintCmd,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId);
    void createGenericSCXMLPrintTime(const GTAActionPrintTime *iPrintCmd,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId);
    void createGenericSCXMLWaitFor(const GTAActionWait *iWaitCmd,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId);
    void createGenericSCXMLPrintParameter(const GTAActionPrint *iPrintCmd,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId);
    void createGenericSCXMLPrintTable(const GTAActionPrintTable *iPrintCmd,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId);

    void createGenericSCXMLConditionIf(const GTAActionIF *iIfCmd, GTASCXMLState &iState,const QString &iStateId, const QString &iTargetId);
    void createGenericSCXMLConditionWhile(const GTAActionWhile *iWhileCmd, GTASCXMLState &iState,const QString &iStateId, const QString &iTargetId);
    void createGenericSCXMLConditionDoWhile(const GTAActionDoWhile *iDoWhileCmd, GTASCXMLState &iState,const QString &iStateId, const QString &iTargetId);

    void createGenericSCXMLWaitUntil(const GTAActionWait *iWaitUntil, GTASCXMLState &iState,const QString &iStateId, const QString &iTargetId);
    void createGenericSCXMLSet(const GTAActionSetList *iSetListCmd, GTASCXMLState &iState,const QString &iStateId, const QString &iTargetId);
    void createGenericSCXMLCheckValue(const GTACheckValue *iCheckValueCmd, GTASCXMLState &iState,const QString &iStateId, const QString &iTargetId);
    void createGenericSCXMLCall(const GTAActionExpandedCall *pCallCmd, GTASCXMLState &iState,const QString &iStateId, const QString &iTargetId);
    void crateGenericSCXMLMaths(const GTAActionMaths * iMathsCmd, GTASCXMLState &iState,const QString &iStateId, const QString &iTargetId);

    void GenerateHeaderMetaData(const QString &iExportPath, const QString &iFileName);
    //    void validateExpression(QString &oExpression);
};

#endif // GTAGENERICSCXMLCONTROLLER_H

