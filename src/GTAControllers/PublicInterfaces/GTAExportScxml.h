#ifndef GTAEXPORTSCXML_H
#define GTAEXPORTSCXML_H
#include "GTAElement.h"
#include "GTAActionManual.h"
#include "GTAControllers.h"
#include "GTAActionIF.h"
#include "GTAGenericToolCommand.h"
#include "GTAActionIRT.h"
#include "GTAOneClick.h"
#include "GTACheckValue.h"
#include "GTAActionSet.h"
#include "GTAActionSetList.h"
#include "GTAActionPrint.h"
#include "GTAActionWait.h"
#include "GTAActionWhile.h"
#include "GTAActionForEach.h"
#include "GTACheckFwcWarning.h"
#include "GTACheckBiteMessage.h"
#include "GTAScxmlLogMessage.h"
#include "GTAActionRelease.h"
#include "GTAICDParameter.h"
#include "GTACheckAudioAlarm.h"
#include "GTAActionOneClickPPC.h"
#include "GTAOneClickLaunchApplication.h"
#include "GTAActionRoboArm.h"
#include "GTAActionPrintTable.h"
#include "GTAActionParentTitle.h"
#include "GTAActionMaths.h"
#include "GTAActionMath.h"
#include "GTAActionOneClickSetList.h"
#include "GTAActionDoWhile.h"
#include "GTAActionGenericFunction.h"
#include "GTAActionOneClickParentTitle.h"
#include "GTAActionExpandedCall.h"
#include "GTAActionPrintTime.h"
#include "GTAActionFunctionReturn.h"
#include "GTAActionCallProcedures.h"
#include "GTAInvalidCommand.h"
#include "GTAActionFailure.h"
#include "GTAActionSubscribe.h"
#include "GTAActionUnSubscribe.h"

#pragma warning(push, 0)
#include <QDomElement>
#include <QString>
#include <QHash>
#include <QStringList>
#pragma warning(pop)

//TODO: This is a warning against assigning Rvalues to references, which is done in default arguments for many methods in this file,
//      This will be fixed in the future, as this refactor will take some time (there are a lot od default arguments which will be broken by removing default argument from some of the refs
#pragma warning (disable : 4239)

class GTACmdNode;
class GTAControllers_SHARED_EXPORT GTAExportScxml
{
public:
	enum VariableType { String, Number, Boolean, EmptyString, Unknown };

	GTAExportScxml(const QString& iTemplatePath);
	~GTAExportScxml();

	/**
	  * This function return the last error occured during the scxml export
	  */
	inline QString getLastError() const { return _LastError; }

	/** This command export the GTA data model to scxml
	  * ipElement: Element to be exported to scxml
	  * iFilePath : Full file path of scxml
	  * return true if file is created successfully
	  */
	bool exportToScxml(const GTAElement* ipElement, const QString& iFilePath, bool iHasUnsubscibeAtStart, bool iHasUnsubscibeAtEnd);

	/**
	  * This function return the scxml template path of each commands
	  */
	inline QString getTemplatePath() const { return _ScxmlTemplatePath; }

	static VariableType IsTypeOf(const QString& iVariable);
private:

	QHash<QString, QString> _StateIdNameMap;
	struct GTAScxmlDataModel
	{
		QString ID;
		QString Expr;
		bool IsApplication;
		bool IsPIR;
		bool isGenerateSubscribe;
		GTAScxmlDataModel() { ID = ""; Expr = ""; IsApplication = false; IsPIR = false; }
		/*GTAScxmlDataModel(const GTAScxmlDataModel& rhs)
		{
			ID=rhs.ID;
			Expr=rhs.Expr;
			IsApplication = rhs.IsApplication;
			IsPIR=rhs.IsPIR;
			_Actor = rhs.getActorName();
			QStringList mediaList = rhs.getMediaList();
			foreach(QString sMedia,mediaList)
			{
				QStringList busList = rhs.getBusNameList(sMedia);
				if(!busList.isEmpty())
				_MediaBusMap[sMedia]=busList;
			}
		}*/
		void insert(const QString& iMedia, const QString& iBusName, const QString& iActor)
		{
			QStringList busList;
			if (_MediaBusMap.contains(iMedia))
				busList = _MediaBusMap.value(iMedia);

			if (!busList.contains(iBusName))
				busList.append(iBusName);

			_MediaBusMap.insert(iMedia, busList);
			_Actor = iActor;
		}

		QStringList getMediaList()const
		{
			return _MediaBusMap.keys();
		}
		QStringList getBusNameList(const QString& iMedia)const
		{
			return _MediaBusMap.value(iMedia);
		}
		QString getActorName()const
		{
			return _Actor;
		}
		bool operator ==(const GTAScxmlDataModel& iObj) const
		{
			return (this->ID == iObj.ID && this->IsApplication == iObj.IsApplication);
		}
		/* GTAScxmlDataModel& operator =(const GTAScxmlDataModel & rhs)
		{
			ID=rhs.ID;
			Expr=rhs.Expr;
			IsApplication = rhs.IsApplication;
			IsPIR=rhs.IsPIR;
			_Actor = rhs.getActorName();
			QStringList mediaList = rhs.getMediaList();
			foreach(QString sMedia,mediaList)
			{
				QStringList busList = rhs.getBusNameList(sMedia);
				if(!busList.isEmpty())
					_MediaBusMap[sMedia]=busList;
			}
			return *this;
		}*/
	private:
		QHash<QString, QStringList> _MediaBusMap;
		QString _Actor;
	};

	struct GTASXLog
	{
		GTASXLog(QString iLabel = QString(), QString iExpr = QString()) { Label = iLabel; Expr = iExpr; }
		QString Label;
		QString Expr;
	};
	struct GTASXSend
	{
		GTASXSend(QString iEvent = QString(), QString iDelay = QString()) { Event = iEvent; Delay = iDelay; }
		QString Event;
		QString Delay;
	};
	struct GTASXTransition
	{
		GTASXTransition() { Target = QString(); Event = QString(); Condition = QString(); }
		QString Target;
		QString Event;
		QString Condition;
		QString status;
	};
	struct GTASXAssign
	{
		GTASXAssign(QString iLocation = QString(), QString iExpr = QString()) { Location = iLocation; Expr = iExpr; }
		QString Location;
		QString Expr;
	};
	struct GTASXFuncArgument
	{
		GTASXFuncArgument() { Name = QString(); Value = QString(); }
		QString Name;
		QString Value;
	};
	struct GTASXFuncReturnVal
	{
		GTASXFuncReturnVal() { Name = QString(); }
		QString Name;
	};

private:
	QString _ScxmlTemplatePath;
	QString _LastError;
	QStringList _VariableList;
	QHash<QString, GTAICDParameter> _ParamList;
	bool _IsPirParamManagement;
	int _Counter;
	QHash<QString, GTAScxmlDataModel> _DataModelList;
	QHash<QString, GTAScxmlDataModel> _DataModelListForOneClick;
	bool _hasChannelInControl;

	QHash<QString, QString> _hshStateIDName;
	QHash<QString, QString> _genToolReturnMap;
	QHash<QString, QString> _genToolParams;
	QList<QString> _addedParams;
	QList<GTAElement*> _IntermediateElemList;
	QList<GTACommandBase*> _oneClickCmdList;
	QList<GTACommandBase*> _FailureCommandList;
	QList<GTACommandBase*> _oneClickStopCmdList;
	QString _elementName;
	GTAElement* _pElement;
	GTAElement* _pTitleBasedElement;

	//    QList<GTACommandBase*> _oneClickLaunchAppCmdList;
	//    QList<GTACommandBase*> _oneClickPPCCmdList;
	void setElementName(GTAElement* pElement, QString iSaveFilePath);
	inline void initializeStateNumber() { _Counter = 0; }
	inline QString getNextStateNumber() { return QString("s%1").arg(_Counter++); }
	inline QString getCurrentStatNumber() { return QString("s%1").arg(_Counter); }
	/**
	  * This function create relationship between commands from the provide set of command
	  * & assign unique ID called state id
	  * The relationship is used to nevigate up & down as well as parent-child relationship between commands
	  * Data structure is as follows
	  * Root
	  *  |(sibling)
	  * NextNode -(Parent-Child)- ChildNode
	  *  |(sibling)                 |(sibling)
	  *  |                        NextNode
	  * NextNode - ChildNode
	  */
	void MakeState(const QList<GTACommandBase*>& iCmdList, GTACmdNode*& ipRootNode, GTACmdNode*& ipParant, bool IsCondition = true);

	/**
 * This function scan the ipNode to get the last node, the last node is retrieve from sibling relationship, parent-child relationship is ignored
 * @ipNode: The node to be scan for last node
 * @opLastNode: the last node to be retrieved from ipNode
 * return true if found , otherwise false.
 */
	bool getLastNode(GTACmdNode* ipNode, GTACmdNode*& opLastNode);
	/**
	  * This function create final scxml using the root command node
	  * ipRootNode: The root commmand node , it is first command of editor
	  * iFilePath: Path where the scxml file is created
	  * Return true, if the scxml successfully created otherwise false;
	  */
	bool MakeStateChart(const GTACmdNode* ipRootNode, const QString& iFilePath, bool iHasUnsubscibeAtStart, bool iHasUnsubscibeAtEnd);

	/**
	  * This function assemble the scxml stats for each command
	  * ipRootNode: The command node for which the stat to be created
	  * iDomDoc: state xml is create in the context of this document
	  * iMainStatElem : The Root element where the state is inserted
	  * iLogStatElem : The log state where the log of each command is inserted
	  * return true if scxml of the command is created, otherwise false
	  */
	bool MakeScxml(const GTACmdNode* ipRootNode, QDomDocument& iDomDoc, QDomElement& iMainStatElem, QDomElement& iLogStatElem);

	/**
	  * This function retrieve the xml document from provide xml file path
	  * iTemplateFileName: xml file
	  * oDomDoc: xml document of the provide file
	  * return true if the xml document is found
	  */
	bool openScxmlTemplateFile(const QString& iTemplateFileName, QDomDocument& oDomDoc);

	/**
	  * This function open the xml file and import all the node in provided xml document context
	  * iDocInContext: xml document context where the xml file nodes are imported
	  * oRootElem : The root element in iDocInContext context
	  * return true if successfully xml file imported in iDocInContext document context otherwise false;
	  */
	bool getCloneRootElement(const QString& iTemplateFileName, QDomDocument& iDocInContext, QDomElement& oRootElem);

	bool getConfigurationTimeNode(QDomDocument& iDocInContext, QDomElement& oElem, QString iStateID, const QString& confTime, const QString& iTargetID, QString& oConfTimeEventName);
	bool getTimeOutNode(QDomDocument& iDocInContext, QDomElement& oRootElem, QString iStateID, const QString& confTime, const QString& iTargetID);
	/**
	* This function return the evaluated target ID,
	* if ipSourceNode is not the last node then next ipSourceNode->NextNode command node stat id is target id for the state
	* Otherwise following cases have different target ID
	* Case-1: if ipSourceNode is last node and have parent which is a while action, then while node state id => target id
	* Case-2: if ipSourceNode is last node and have parent which is not a while action, then parent's next node stat id => target id
	* ipSourceNode: The command node which target ID to be evaluated
	* return target ID of the provide node
	*/
	QString getTargetId(const GTACmdNode* ipSourceNode);

	/**
	  * This function create the scxml state for a manual action
	  * pCmd: Manual action command
	  * iDomDoc: state xml is create in the context of this document
	  * iRootElem: Element where the created state is inserted
	  * iStateId: State id of the command;
	  * iTargetId: The next ID where the stat should go after execution
	  * return true if scxml of the command is created, otherwise false
	  */
	bool createActionManualState(const GTAActionManual* pCmd, QDomDocument& iDomDoc, QDomElement& iRootElem, const QString& iStateId, const QString& iTargetId, const QMap<QString, double>&);
	/**
	  * This function create the scxml state for a Maths action
	  * pCmd: Maths action command
	  * iDomDoc: state xml is create in the context of this document
	  * iRootElem: Element where the created state is inserted
	  * iStateId: State id of the command;
	  * iTargetId: The next ID where the stat should go after execution
	  * return true if scxml of the command is created, otherwise false
	  */
	bool createActionMathsState(const GTAActionMaths* pCmd, QDomDocument& iDomDoc, QDomElement& iRootElem, const QString& iStateId, const QString& iTargetId);
	bool createActionMathState(const GTAActionMath* pCmd, QDomDocument& iDomDoc, QDomElement& iRootElem, const QString& iStateId, const QString& iTargetId);

	/**
	  * This function create the scxml state for a "if" action
	  * pCmd: if/else command
	  * iDomDoc: state xml is create in the context of this document
	  * iRootElem: Element where the created state is inserted
	  * iStateId: State id of the command;
	  * iTrueStateId: The next ID if the condition is true
	  * iFalseStateId: The next ID if the condition is false
	  * iStatOfSibling: The ID of Sibling command node below if
	  * return true if scxml of the command is created, otherwise false
	  */
	bool createActionIfState(const GTAActionIF* pCmd, QDomDocument& iDomDoc, QDomElement& iRootElem,
		const QString& iStatId, const QString& iTrueStateId, const QString iFalseStateId,
		const QString& iStatOfSibling, QDomElement& oStateNode);

	/**
	  * This function create the scxml state for a while action
	  * pCmd: For Each command
	  * iDomDoc: state xml is create in the context of this document
	  * iRootElem: Element where the created state is inserted
	  * iStateId: State id of the command;
	  * iTrueStateId: The next ID if the condition is true
	  * iFalseStateId: The next ID if the condition is false
	  * iStatOfSibling: The ID of Sibling command node below while
	  * return true if scxml of the command is created, otherwise false
	  */
	bool createActionForEachState(GTAActionForEach* pCmd, QDomDocument& iDomDoc, QDomElement& iRootElem,
		QString& ioStatId, const QString& iTrueStateId, const QString iFalseStateId,
		const QString& iStatOfSibling, QDomElement& oElem);

	/**
	  * This function create the scxml state for a while action
	  * pCmd: While command
	  * iDomDoc: state xml is create in the context of this document
	  * iRootElem: Element where the created state is inserted
	  * iStateId: State id of the command;
	  * iTrueStateId: The next ID if the condition is true
	  * iFalseStateId: The next ID if the condition is false
	  * iStatOfSibling: The ID of Sibling command node below while
	  * return true if scxml of the command is created, otherwise false
	  */
	bool createActionWhileState(GTAActionWhile* pCmd, QDomDocument& iDomDoc, QDomElement& iRootElem,
		QString& ioStatId, const QString& iTrueStateId, const QString iFalseStateId,
		const QString& iStatOfSibling, QDomElement& oElem);

	/**
	* This function create the scxml state for a do while action
	* pCmd: do While command
	* iDomDoc: state xml is create in the context of this document
	* iRootElem: Element where the created state is inserted
	* iStateId: State id of the command;
	* iTrueStateId: The next ID if the condition is true
	* iFalseStateId: The next ID if the condition is false
	* iStatOfSibling: The ID of Sibling command node below while
	* return true if scxml of the command is created, otherwise false
	*/

	bool createActionDoWhileState(GTAActionDoWhile* pCmd, QDomDocument& iDomDoc,
		QDomElement& iRootElem, QString& ioStatId,
		const QString& iTrueStateId, const QString iFalseStateId,
		const QString& iStatOfSibling, QDomElement& oElem);
	/**
	  * This function create the scxml state for a IRT action
	  * pCmd: IRT command
	  * iDomDoc: state xml is create in the context of this document
	  * iRootElem: Element where the created state is inserted
	  * iLogStatElem : The log state where the log of the command is inserted
	  * iStateId: State id of the command;
	  * iTargetId: The next ID where the stat should go after execution
	  * return true if scxml of the command is created, otherwise false
	  */
	bool createActionIRTState(const GTAActionIRT* pCmd, QDomDocument& iDomDoc, QDomElement& iRootElem, QDomElement& iLogStatElem, const QString& iStateId, const QString& iTargetId);

	/**
	  * create stat for OneClick command, return true if succeded otherwise false
	  */

	bool createActionOneClickSetEnvState(const GTAOneClick* pCmd, QDomDocument& iDomDoc, QDomElement& iRootElem, QDomElement& iLogStatElem, const QString& iStateId, const QString& iTargetId, QDomElement& oRootElement);
	/**
	  * create stat for OneClick PPC command, return true if succeded otherwise false
	  */

	bool createActionOneClickPPCState(const GTAActionOneClickPPC* pCmd, QDomDocument& iDomDoc, QDomElement& iRootElem, QDomElement& iLogStatElem, const QString& iStateId, const QString& iTargetId, QDomElement& oRootElement);

	/**
	  * create stat for OneClick Launch Application command, return true if succeded otherwise false
	  */
	bool createActionOneClickLauncAppState(const GTAOneClickLaunchApplication* pCmd, QDomDocument& iDomDoc, QDomElement& iRootElem, QDomElement& iLogStatElem, const QString& iStateId, const QString& iTargetId, QDomElement& oRootElement);

	/**
	  * This function create the scxml state for a IRT Log stat
	  * pCmd: IRT command
	  * iDomDoc: state xml is create in the context of this document
	  * iLogState: Element where the created state is inserted
	  * iStateId: State id of the command;
	  * iExecutionFlag: Execution enum value. Executed,Logged
	  * iTemplFileName: scxml template file path of IRT log
	  * iCmdInstanceName : Instance of command for which log is created
	  * return true if scxml of the command is created, otherwise false
	  */
	bool createActionIRTLogState(QDomDocument& iDomDoc, QDomElement& iLogState, const QString& iStateId,
		const QString& iExecutionFlag, const QString& iTemplFileName, const QString& iCmdInstanceName, const QDomElement& iFunDBArgNode);

	/**
	  * This function create the scxml state for a Set action
	  * pCmd: Set command
	  * iDomDoc: state xml is create in the context of this document
	  * iRootElem: Element where the created state is inserted
	  * iStateId: State id of the command;
	  * iTargetId: The next ID where the stat should go after execution
	  * return true if scxml of the command is created, otherwise false
	  */
	bool createActionSetState(const GTAActionSetList* pCmd, QDomDocument& iDomDoc, QDomElement& iRootElem, const QString& iStateId, const QString& iTargetId);

	/**
	  * This function create the scxml state for a wait for/until action
	  * pWaitCmd: wait for/until command
	  * iDomDoc: state xml is create in the context of this document
	  * iRootElem: Element where the created state is inserted
	  * iStateId: State id of the command;
	  * iTargetId: The next ID where the stat should go after execution
	  * return true if scxml of the command is created, otherwise false
	  */
	bool createActionWaitState(const GTAActionWait* pWaitCmd, QDomDocument& iDomDoc, QDomElement& iRootElem, const QString& iStateId, const QString& iTargetId);
	bool createActionWaitForState(const GTAActionWait* pWaitCmd, QDomDocument& iDomDoc, QDomElement& iRootElem, const QString& iStateId, const QString& iTargetId);

	/**
	* This function create the scxml state for a Title action
	* pTitleCmd: title command
	* iDomDoc: state xml is create in the context of this document
	* iRootElem: Element where the created state is inserted
	* iStateId: State id of the command;
	* iTargetId: The next ID where the stat should go after execution
	* oRootElem: retuns the title state dom element.
	* return true if scxml of the command is created, otherwise false
	*/
	bool createActionTitleState(const GTAActionParentTitle* pTitleCmd, QDomDocument& iDomDoc, QDomElement& iRootElem, const QString& iStateId, const QString& iTargetId, QDomElement& oRootElem);
	bool createActionCallState(const GTAActionCall* pExtCall, QDomDocument& iDomDoc, QDomElement& iRootElem, const QString& iStateId, const QString& iTargetId, QDomElement& oRootElem, bool isDummy = false);
	bool createActionCallProcsState(const GTAActionCallProcedures* procCall, QDomDocument& iDomDoc, QDomElement& iRootElem, const QString& iStateId, const QString& iTargetId, QDomElement& oRootElem);

	bool createActionPrintTimeState(const GTAActionPrintTime* pPrintCmd, QDomDocument& iDomDoc, QDomElement& iRootElem, const QString& iStateId, const QString& iTargetId);
	bool createCheckFWCWrng(const GTACheckFwcWarning* pFwcWarnCmd, QDomDocument& iDomDoc, QDomElement& iRootElem, const QString& iStateId, const QString& iTargetId);
	bool createCheckBiteMessage(const GTACheckBiteMessage* pBiteMsgCmd, QDomDocument& iDomDoc, QDomElement& iRootElem, const QString& iStateId, const QString& iTargetId);
	bool createCheckAudioAlarm(const GTACheckAudioAlarm* pAudioAlarmCmd, QDomDocument& iDomDoc, QDomElement& iRootElem, const QString& iStateId, const QString& iTargetId, const QString& iFalseStateId = QString(), const QString& iOperator = QString());
	bool createActionInvlaidState(const GTAInvalidCommand* pCmd, QDomDocument& iDomDoc, QDomElement& iRootElem, const QString& iStateId, const QString& iTargetId);
	bool createActionSubscribeState(const GTAActionSubscribe* pCmd, QDomDocument& iDomDoc, QDomElement& iRootElem, const QString& iStateId, const QString& iTargetId);

	bool createActionUnSubscribeState(const GTAActionUnSubscribe* pCmd, QDomDocument& iDomDoc, QDomElement& iRootElem, const QString& iStateId, const QString& iTargetId);
	/**
	  * This function makes a mathmatical condtional equation
	  * iParam: variable to be compared
	  * iVal: value to be compare with iParam
	  * iCond: condition on which comparision to be taken place
	  * oTrueExpress: the equation if the condition is met
	  * oFalseExpress: the equation if the condition does not met
	  * iStateId: State id of the command;
	  * iTargetId: The next ID where the stat should go after execution
	  * isStringVal: the iParam or iVal is bool type, e.g. value is of type "true", "false"
	  */

	void getExpresion(QString& iParam, const QString& iVal, const QString& iCond, QString& oTrueExpress, QString& oFalseExpress, bool isStringVal = false, const double iPrecision = 0.0, const QString& iPrecisionUnit = ACT_PRECISION_UNIT_VALUE);

	/**
	  * This function create the scxml state for check value command
	  * pChk : Command for which the state to be created
	  * iDomDoc: state xml is create in the context of this document
	  * iRootElem: Element where the created state is inserted
	  * iStateId: State id of the command;
	  * iTargetId: The next ID where the stat should go after execution
	  * return true if scxml of the command is created, otherwise false
	  */
	bool createCheckValueState(const GTACheckValue* pChk, QDomDocument& iDomDoc, QDomElement& iRootElem, const QString& iStateId, const QString& iTargetId,
		const QString& iFailStateID = QString(), bool externalTimeOut = false, const QString& externalTimeOutEventName = QString(),
		QDomElement& oStateElem = QDomElement(), const QString& iConfTimeInMs = QString(), const QString& iconfigStateId = QString(),
		const QString& iTimeOutDelay = QString(), const QString iTimeoutName = QString());

	void dumpQNodeToDebug(const QDomNode& iNode, const QString& iString = QString())const;
	void dumpElemenToDebug(const QDomElement& iNode, const QString& iString = QString())const;

	/**
	  * This function instert the application data in data model object
	  * The data model object used finally to create scxml data model
	  */
	void insertApplicationDataModel(const QString& iAppName, const QString& iMedia, const QString& iBusName, const QString& iActor, bool isInsertInSubscribe = true);
	void insertApplicationDataModelInitEnv(const QString& iAppName, const QString& iMedia, const QString& iBusName, const QString& iActor);

	void updateSubscribeState(QDomDocument& iDomDoc, QDomElement& iSubscribeElement, QDomElement& iDataModeElement, QDomElement& iUnSubscribeElement, const QHash<QString, GTAScxmlDataModel>& iDMList);

	bool insertValidAppDataModel(QString& iParameter, QString& oTypeOfParam = QString(PARAM_TYPE_LOCAL), bool isGenerateSubscribe = true);

	/**
	  * This function create the scxml state for print value command, it can be either parameter or message print
	  * @pPrintCmd : Command for which the state to be created
	  * @iDomDoc: state xml is create in the context of this document
	  * @iRootElem: Element where the created state is inserted
	  * @iStateId: State id of the command;
	  * @iTargetId: The next ID where the stat should go after execution
	  * return true if scxml of the command is created, otherwise false
	  */
	bool createActionPrintState(const GTAActionPrint* pPrintCmd, QDomDocument& iDomDoc, QDomElement& iRootElem, const QString& iStateId, const QString& iTargetId);

	/**
	* This function create the scxml state for print value command, it can be either parameter or message print
	* @pPrintCmd : Command for which the state to be created
	* @iDomDoc: state xml is create in the context of this document
	* @iRootElem: Element where the created state is inserted
	* @iStateId: State id of the command;
	* @iTargetId: The next ID where the stat should go after execution
	* return true if scxml of the command is created, otherwise false
	*/
	bool createActionPrintTableState(const GTAActionPrintTable* pPrintCmd, QDomDocument& iDomDoc, QDomElement& iRootElem, const QString& iStateId, const QString& iTargetId);

	/**
	  * This function create the scxml state for Robotic ARM value command
	  * @pRoboArmCmd : Command for which the state to be created
	  * @iDomDoc: state xml is create in the context of this document
	  * @iRootElem: Element where the created state is inserted
	  * @iStateId: State id of the command;
	  * @iTargetId: The next ID where the stat should go after execution
	  * return true if scxml of the command is created, otherwise false
	  */
	bool createActionRoboArmState(const GTAActionRoboArm* pRoboArmCmd, QDomDocument& iDomDoc, QDomElement& iRootElem, const QString& iStateId, const QString& iTargetId);

	/**
	  * This function create the scxml state for print value of FWC buffer command
	  * @pPrintCmd : Command for which the state to be created
	  * @iDomDoc: state xml is create in the context of this document
	  * @iRootElem: Element where the created state is inserted
	  * @iStateId: State id of the command;
	  * @iTargetId: The next ID where the stat should go after execution
	  * return true if scxml of the command is created, otherwise false
	  */
	bool createActionPrintFwcBufferState(const GTAActionPrint* pPrintCmd, QDomDocument& iDomDoc, QDomElement& iRootElem, const QString& iStateId, const QString& iTargetId);

	/**
	  * This function create the scxml state for print value of BITE buffer command
	  * @pPrintCmd : Command for which the state to be created
	  * @iDomDoc: state xml is create in the context of this document
	  * @iRootElem: Element where the created state is inserted
	  * @iStateId: State id of the command;
	  * @iTargetId: The next ID where the stat should go after execution
	  * return true if scxml of the command is created, otherwise false
	  */
	bool createActionPrintBiteBufferState(const GTAActionPrint* pPrintCmd, QDomDocument& iDomDoc, QDomElement& iRootElem, const QString& iStateId, const QString& iTargetId);

	bool createActionGenericToolSCXMLState(GTAGenericToolCommand* pGenericToolCmd, QDomDocument& iDomDoc, QDomElement& iRootElem, const QString& iStateId, const QString& iTargetId);

	/**
	  * This function create the scxml state for release command
	  * @pReleaseCmd : Command for which the state to be created
	  * @iDomDoc: state xml is create in the context of this document
	  * @iRootElem: Element where the created state is inserted
	  * @iStateId: State id of the command;
	  * @iTargetId: The next ID where the stat should go after execution
	  * return true if scxml of the command is created, otherwise false
	  */
	bool createActionReleaseState(const GTAActionRelease* pReleaseCmd, QDomDocument& iDomDoc, QDomElement& iRootElem, const QString& iStateId, const QString& iTargetId);
	bool createActionFailureState(const GTAActionFailure* pFailurecmd, QDomDocument& iDomDoc, QDomElement& iRootElem, const QString& iStateId, const QString& iTargetId);
	bool createActionOneClickTitleState(const GTAActionOneClickParentTitle* pTitleCmd, QDomDocument& iDomDoc, QDomElement& iRootElem, const QString& iStateId, const QString& iTargetId, QDomElement& oRootElem);
	//common functions for writing scxml nodes.

	bool createGenericFunctionState(const GTAActionGenericFunction* pPrintCmd, QDomDocument& iDomDoc, QDomElement& iRootElem, const QString& iStateId, const QString& iTargetId);
	bool createActionReturnFunctionState(const GTAActionFunctionReturn* pFuncRetCmd, QDomDocument& iDomDoc, QDomElement& iRootElem, const QString& iStateId, const QString& iTargetId);
	//bool getStateChildElements(QDomDocument& iContextDoc, const QDomElement & iStateElem, QMap<QString,QDomElement>& stateChildMap);
	bool setOnEntryAssign(QDomElement& iOnEntryElem, const QString& iLocation, const QString& iExpression);
	bool setOnEntry(QDomElement& iOnEntryElem,
		const QString& isAssignLocation, const QString& isAssignExpression,
		const QString& isLogLabel, const QString& isLogExpression,
		const QString& isSendEvent, const QString& isSendDelay);
	bool setOnExitAssign(QDomElement& iOnExitElem, const QString& iLocation, const QString& iExpression);
	bool setInvokeFunction(QDomElement& iInvokeElem, const QString& iInvokeID, const QString& iArgumentValue, const QString& iReturnVal);
	bool setInvokeFinalize(QDomElement& iInvokeElem, const QString& iAssignLocation, const QString& iAssignExpr, const QString& iLogLabel, const QString& iLogExpr);
	bool setTranstion(QDomElement& iTransitionElem, const QString& iTarget, const QString& iEvent, const QString& iCond);

	void getScxmlElementPathMap(const QDomElement& iRootElem, const QString& iXPath, QMap<QString, QDomElement>& oPathMap);
	void setStateAttr(QDomElement& iElemOfState, const QString stateID);
	bool setOnEntryOrExit(QDomElement& iElemOnEntryOrExit, const GTASXAssign& iObjAssign,
		const GTASXLog& iObjLog, const GTASXSend& iObjSend);
	bool setTransitionAttr(QDomElement& iElemOnTransit, const GTASXTransition& iObjTransition);

	/**
	  * This function set the invoke node of state, arguments are set based on the name attribue of argument node match
	  * iElemOnInvoke: Element of invoke which need to be updated by attributes
	  * invokeId: invoke ID of the invoke node, it should be unique within the state
	  * iArgList: list of argument, argument value is set where the argument name is matches
	  * iObjRetVal: Atrributes of return value node of function node
	  * iObjAssign: Atrributes of assign node in finalize node
	  * iObjAssign: Atrributes of log node in finalize node
	  * return true if successfully invoke node is set otherwise false
	  */
	bool setInvokeAttr(QDomElement& iElemOnInvoke, const QString invokeId, const QList<GTASXFuncArgument>& iArgList,
		const GTASXFuncReturnVal& iObjRetVal, const GTASXAssign& iObjAssign,
		const GTASXLog& iObjLog, bool appendBraces = false);

	/**
	  * This function return the list of valid parameters (ICD/PMR/PIR)
	  * Here valid parameters are the variable present in ICD/PMR/PIR files
	  * constant type which are of type number & local which are of strings
	  * iVarList: list of variables used in commands, it can be any data type
	  */
	QStringList getValidParametersToInvoke(const QStringList& iVarList);

	/**
	  * This function create invoke node for each parameter in iParamList
	  * ioStatElem : The stat element where the invoke nodes are inserted
	  * iRefInvokeElem: Each node is clon of this node, it shall be removed using ioStatElem::removeChild from the stat after calling this function
	  * iParamList: list of valid parameters, valid parameter can be retrieved from getValidParametersToInvoke(), this function do not validate the parameters
	  * iBaseInvokeID: The base invoke id is incremented by number of parameter
	  * iFuncArgParam: the name of parameter in function argument of invoke node
	  * iLogLabel : label of log in finalize node
	  * iLogMsg : log message in finalize node
	  * iAppenBraces:
	  * i
	  */
	void createInvokeNodes(QDomDocument& iDomDoc, QDomElement& ioStatElem, QDomElement& iRefInvokeElem, const QStringList& iParamList,
		const QString& iBaseInvokeID, const QString& iFuncArgParam, const QString& iLogLabel = "Info",
		const QString& iLogMsg = "", bool iApendBraces = false, const QString& iActionName = "", const QString& iComplementName = "", QString& oTxEventName = QString());

	void createMultiInvokeNode(QDomDocument& iDomDoc, QDomElement& ioStatElem, QDomElement& iRefInvokeElem, const QStringList& iParamList,
		const QString& iBaseInvokeID, QString& oTxEventName, const QString& iActionName = "", const QString& iComplementName = "", const QString& iStateId = QString());

	//void createMultiInvokeNode(QDomDocument& iDomDoc, QDomElement& ioStatElem, QDomElement& iRefInvokeElem, const QStringList& iParamList,
	//    const QString& iBaseInvokeID, const QString& iFuncArgParam, const QString& iLogLabel = "Info",
	//    const QString& iLogMsg = "", bool iApendBraces = false, const QString& iActionName = "", const QString& iComplementName = "", QString& oTxEventName = QString(), const QString& iStateId = QString());

	void createMultiInvokeNodeForVIP_VMAC(QDomDocument& iDomDoc, QDomElement& ioStatElem, QDomElement& iRefInvokeElem, const QStringList& iParamList,
		const QString& iBaseInvokeID, const QString& iFuncArgParam, const QString& iLogLabel = "Info",
		const QString& iLogMsg = "", bool iApendBraces = false, const QString& iActionName = "", const QString& iComplementName = "", QString& oTxEventName = QString(), const QString& iStateId = QString());

	/**
	* This function create states for invoke for each parameter in iParamList
	* ioStatElem : The stat element where the invoke nodes are inserted
	* iRefInvokeElem: Each node is clon of this node, it shall be removed using ioStatElem::removeChild from the stat after calling this function
	* iParamList: list of valid parameters, valid parameter can be retrieved from getValidParametersToInvoke(), this function do not validate the parameters
	* iBaseInvokeID: The base invoke id is incremented by number of parameter
	* iFuncArgParam: the name of parameter in function argument of invoke node
	* iLogLabel : label of log in finalize node
	* iLogMsg : log message in finalize node
	* iAppenBraces:
	* i
	*/
	void createInvokeStateNodes(QDomDocument& iDomDoc, QDomElement& ioStatElem, QDomElement& iRefInvokeElem,
		const QStringList& iParamList, const QString& iBaseInvokeID,
		const QString& iFuncArgParam, const QString& iLogLabel, const QString& iLogMsg, bool appendBraces = false, const QString& iActionName = "", const QString& iComplementName = "", const QString iStateId = "", QString& oTxEventName = QString());

	/**
	  * This function create followings for action of fail of a command
	  * Create OnEntry/Assign nodes if does not exist and provide time out event
	  * Always create Transition of state , if action of fail is stop then transition happened to be final otherwise next state
	  * iElemOnState: State element where time out to be implemented
	  * iStateId: State id of the command;
	  * iTargetId: The next ID where the stat should go after execution
	  * iActionOnFail: value of action on fail of the command
	  * iTimeOutInMS: Time out value of the command, it should be in mili second, see the usage of getTimeOutInMiliSecond
	  * IsDelayCheckCond: This flag add event in conditional transition to delay the condition checking
	  */
	void createActionOnFail(QDomDocument& iDomDoc, QDomElement& iElemOnState, const QString& iStateId,
		const QString& iTargetId, const QString& iActionOnFail, const QString& iTimeOutInMS,
		const QString& iLogMessage, bool IsDelayCheckCond = false, const QString& iDelayEventName = QString(), bool bCreateLog = true, bool isLoop = false, QString& oTimeoutEventFlag = QString());

	void createActionOnFail(QDomDocument& iDomDoc, QDomElement& iElemOnState, const QString& iStateId,
		const QString& iTargetId, const QString& iActionOnFail, const QStringList& iTimeOutsInMS,
		const QString& iLogMessage, bool IsDelayCheckCond = false, const QString& iDelayEventName = QString(), bool bCreateLog = true, bool isLoop = false, QString& oTimeoutEventFlag = QString());

	/**
	  * Note: Preferably use createActionOnFail()
	  * This function create followings for action of fail of a command,
	  * Create OnEntry/Assign nodes if does not exist and provide time out event
	  * Always create Transition of state , if action of fail is stop then transition happened to be final otherwise next state
	  * iElemOnState: State element where time out to be implemented
	  * iDomDoc: state xml is create in the context of this document
	  * iRootElem: Element where the created transition & onEntry node is inserted
	  * iSend: Attributes of send node of onenty node
	  * iTransit: Attributes of transit node, Transition node for action on fail is always created
	  * iTransitAssign: Attributes of assign node in transition node
	  * iTransitLog: Attributes of log node in transition node
	  * bCreateLog: If Log node is not required (true by default)
	  */
	void setActionOnFailAttr(QDomDocument& iDomDoc, QDomElement& iElemOnState,
		const GTASXSend& iSend, const GTASXTransition& iTransit,
		const GTASXAssign& iTransitAssign, const GTASXLog& iTransitLog, bool bCreateLog = true, bool isLoop = false, QString& oTimeoutEventFlag = QString(), const QString iDealyEventName = QString());

	/**
	  * This function create the additional state e.g. success or fail state associated with the parent state
	  * iDomDoc : xml document in context the state to be created
	  * iRootElem : Root element where the created state to be inserted
	  * iAssign: Assign node to be set
	  * iTransit: Transition node to be set
	  * iLog : log node to set
	  * iStateID: state id of the create state
	  */
	void createTransitionChildren(QDomDocument& iDomDoc, QDomElement& iElemOnTransit,
		const GTASXAssign& iAssign,
		const GTASXLog& iLog, QString& iCondtion = QString(), QString& iCondLoc = QString());

	/**
	  * This convert the time out value to mili second format e.g. 3 sec is converted to 3000.00ms
	  */
	QString getTimeOutInMiliSecond(GTACommandBase* pCmd);

	QString getConfTimeInMiliSecond(GTACommandBase* pCmd);
	//Functin to get XmlPath for nodes

	QString getStateXPath();
	QString getOnEntryXPath();
	QString getOnEntryLogXPath();
	QString getOnEntrySendXPath();
	QString getOnEntryAssignXPath();
	QString getOnExitXPath();
	QString getOnExitLogXPath();

	QString getOnExitAssignXPath();
	QString getStateTransistionXPath();
	QString getStateInvokeXPath();
	QString getInvokeFinalizeXPath();
	QString getInvokeFunctionXPath();
	void insertFinalState(QDomDocument& iDomDoc, QDomElement& iStateElem, const QString& iId);

	/**
	  * The function open the scxml template file
	  * @iFileName : template file name with extension e.g. condition_if.scxml
	  * @oContents : retrieve the template contents
	  * return true if template open successfully otherwise false
	  */
	bool openFile(const QString& iFileName, QByteArray& oContents);

	/**
	  * The function replaces the tags which are used in template contents
	  * @iTmplContents: template contents
	  * @iTagToReplace: it is key-value pair where key is tag and value is tag value
	  * return new contents replaced by tag value
	  */
	QByteArray replaceTag(const QByteArray& iTmplContents, const QMap<QString, QString>& iTagToReplace);

	/**
	  * This function return the xml element created from the contents of scxml template file
	  * @iXmlContents: scxml template contents
	  * @iDocInContext: xml document in context where the element is created
	  * return the dom element if successfully contents are created to xml otherwise null element is return
	  */
	QDomElement getElement(QByteArray& iXmlContents, QDomDocument& iDocInContext);

	/**
	  * This function create wait for buffer depth stat element
	  * @iDomDoc: document in context where the stat is created
	  * @iStatID: stat ID of this stat
	  * @iTargetID: id of target stat
	  * @iEvent : event on which the transition to next stat is take place
	  * @iDelay : number of second the stat wait for
	  * return the xml element of this stat
	  */
	QDomElement createBufferDepthState(QDomDocument& iDomDoc, const QString& iStatID, const QString& iTargetID, const QString iEvent, const QString& iDelay);

	/**
	  * This funcation insert a log node in OnExit Element
	  * iElemOnExit : Where the log element node to insterted
	  * iVarList : List of variable to be logged
	  * iStatId: Id of stat for which the logging is being done
	  * iCmdInstanceName: instance name of command
   * iDomDoc : Document in context
	  */
	void insertVariableOnExit(QDomElement& iElemOnExit, const QStringList& iVarList, const QString& iStatId, const QString iCmdInstanceName, QDomDocument& iDomDoc);

	/**
	*This funciton preprocesses XMLRpc contents of IRT command (wherever string is encountered in type attribute,
	*                                                            value attribute is replaced with single quotes)
	*/
	void processIRTXmlRpc(QDomDocument& iXMLRPCDomDoc);

	void setStateIDName(const QString& iStaeID, const GTACommandBase* pCmd);
	void createLoopTimeOutTranstions(QDomElement& iStateElem, QDomDocument& iDomDoc, const QString& iStateId, const GTACommandBase* pParent);
	void createCallTimeOutTranstions(QDomDocument& iDomDoc, QDomElement& iStateElem, GTACommandBase* ipCmd, const QString& sFailStateID, GTAScxmlLogMessage logForCallTimeout = GTAScxmlLogMessage());
	void insertChannelInControlState(QDomDocument& iDomDoc, QDomElement& oStateElem);
	void getChannelInControlCondition(const QString& iParam, const QString& iVal, const QString& iCond, QString& oTrueExpress, QString& oFalseExpress, bool isStringVal = false, const double iPrecision = 0.0, const QString& iPrecisionUnit = ACT_PRECISION_UNIT_VALUE, QStringList& oVarList = QStringList(), bool isFunctionStatus = false);
	void getNodeTimeouts(GTACmdNode* pCmdNode, QMap<QString, double>& timeOuts);
	void generateFailureDataModel(QDomDocument& iDomDoc, QDomElement& iDataModel);
	void setInvokeDelay(QDomDocument& iDomDoc, QDomNode& iInvokeNode, const QString& iActionName, const QString& iComplementName, QString& oTxEventName, const int mulFact = 1);
	bool isAParameter(QString iParameter);
	GTAICDParameter getParamObj(QString iParameter);
	QString getActualParameter(const QString& iParam);
	bool getStateElemenetForPIR(QDomDocument& iDomDoc, QDomElement& elementOnState, const QStringList& iPirParameterList, QDomElement& oStateOnSubscribe);
	bool getSubscribeStateForInvoke(QDomDocument& iDomDoc, const QString& iPirParameter, const QString& invokeStateID, QDomElement& oStateOnSubscribe, QString& oRetParam);
	bool getUnSubscribeStateForInvoke(QDomDocument& iDomDoc, const QString& iPirParameter, QDomElement& oStateOnSubscribe, const QString& invokeStateID, QString& oRetParam);

	bool getPIRStateForInvokes(QDomDocument& iDomDoc, const QString& iStateID, const QDomElement& iRefInvokeElem,
		const QString& iFuncArgParam, const QString& iLogLabel, const QString& iLogMsg, bool appendBraces,
		const QStringList& iPirParameterList, QDomElement& oStateOnInvokeStates, const QString& iActionName, const QString& iComplementName, QString& oTxName);

	void updateWithGenToolParam(QString& ioParam);
};
#pragma warning (default : 4239)
#endif // GTAEXPORTSCXML_H
