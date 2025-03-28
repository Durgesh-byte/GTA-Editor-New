#include "GTACMDSCXMLSet.h"
#include "GTACMDSCXMLUtils.h"
#include "GTAEquationConst.h"
#include "GTAEquationBase.h"
#include "GTACMDSCXMLLog.h"

GTACMDSCXMLSet::GTACMDSCXMLSet()
{
	_EquationCmdSCXML = new GTACMDSCXMLEquationBase();
}
GTACMDSCXMLSet::GTACMDSCXMLSet(GTAActionSetList *&pCmdNode, const QString &iStateId, const QString &iTargetId)
{
	_SetCmd = pCmdNode;
	_StateId = iStateId;
	_TargetId = iTargetId;
	_EquationCmdSCXML = new GTACMDSCXMLEquationBase();
	_ForEachGrandParent = getForEachGrandParent(pCmdNode);
}

GTACMDSCXMLSet::~GTACMDSCXMLSet()
{
	if (NULL != _EquationCmdSCXML)
	{
		delete _EquationCmdSCXML;
		_EquationCmdSCXML = NULL;
	}
}

/**
 * @brief Retrieves the first grandparent of a given GTAActionSetList that meets specific conditions.
 *
 * This function navigates upwards from the given action set list, checking each grandparent's action
 * and complement. It returns the first grandparent for which the action equals `ACT_CONDITION` and
 * the complement equals `COM_CONDITION_FOR_EACH`. The search stops as soon as a matching grandparent
 * is found or when there are no more parents to check.
 *
 * @param setCmd A pointer to the starting GTAActionSetList object from which the grandparent search begins.
 *               It must not be nullptr.
 * @return GTAActionBase* A pointer to the grandparent GTAActionBase object that meets the conditions,
 *                        or nullptr if no such grandparent is found or if `setCmd` is nullptr.
 *
 * @note This function performs dynamic casts and checks for nullptr at each step to ensure safe navigation
 *       through the object hierarchy. It returns nullptr immediately if the input `setCmd` is nullptr or
 *       if any parent in the hierarchy is nullptr before meeting the conditions.
 */
GTAActionBase* GTACMDSCXMLSet::getForEachGrandParent(GTAActionSetList* setCmd) {
	if (setCmd == nullptr) {
		return nullptr;  // Initially check if the input is null
	}

	// Ensure current is valid and get its parent
	GTAActionBase* parent = dynamic_cast<GTAActionBase*> (setCmd->getParent());
	if (parent == nullptr) {
		return nullptr;  // No parent, so return null
	}

	// Continue with the grandparent check
	while (parent != nullptr) {
		GTAActionBase* grandParent = dynamic_cast<GTAActionBase*> (parent->getParent());
		if (grandParent == nullptr) {
			break;  // No further parents available
		}

		QString action = grandParent->getAction();
		QString complement = grandParent->getComplement();
		if (action == ACT_CONDITION && complement == COM_CONDITION_FOR_EACH) {
			return grandParent;  // Found the matching grandparent, return the pointer
		}

		// Move to the next level in the hierarchy
		parent = grandParent;
	}

	return nullptr;  // Return null if no matching grandparent is found
}

void GTACMDSCXMLSet::insertParamInfoList(const GTAICDParameter &iParamInfo)
{
	_EquationCmdSCXML->insertParamInfoList(iParamInfo);
	_ParamInfoList.append(iParamInfo);
	if (!iParamInfo.getName().isEmpty())
		_ParamNameInfoList.append(iParamInfo.getName());
}

QString GTACMDSCXMLSet::getSCXMLString() const
{
	return getSCXMLState().getSCXMLString(_isUSCXMLEn);
}

GTASCXMLState GTACMDSCXMLSet::getSCXMLState()const
{
	GTASCXMLState state;
	bool rc = getDebugModeStatus();
	GTASCXMLState megaState;
	QString preDebugStateId = QString("%1_predebug").arg(_StateId);
	QString debugStateId = QString("%1_debug").arg(_StateId);
	QString preDebugVar = QString("%1_preDebugVar").arg(preDebugStateId);
	QString finaltargetId = QString("%1_final").arg(_StateId);

	QString executableStateID, executableTargetID;
	if (rc)
	{
		executableStateID = QString("%1_exec").arg(_StateId);
		executableTargetID = QString("%1_debug").arg(_StateId);
	}
	else
	{
		executableStateID = _StateId;
		executableTargetID = _TargetId;
	}
	if (_SetCmd != NULL)
	{
		double dtimeout = _SetCmd->getTimeOutInMs();
		QString actOnFail = _SetCmd->getActionOnFail();

		QString stateId = QString("%1_SetCmd").arg(executableStateID);
		QString targetId = QString("%1_final").arg(executableStateID);

		GTASCXMLState SetState = getMultiSetSCXMLState(stateId, targetId, _ForEachGrandParent);

		state.insertStates(SetState);

		GTASCXMLTransition tx;
		tx.setTarget(executableTargetID);
		if (rc)
		{
			tx.setCondition(QString("(%1 == 'true')").arg(preDebugVar));

			GTASCXMLTransition tx1;
			tx1.setTarget(finaltargetId);
			tx1.setCondition(QString("(%1 != 'true')").arg(preDebugVar));
			state.insertTransitions(tx1);
		}
		state.insertTransitions(tx);
		state.setId(executableStateID);

		GTASCXMLOnEntry onEntry = state.getOnEntry();
		GTASCXMLOnExit onExit = state.getOnExit();

		if (dtimeout > 0)
		{
			QString timeoutEventName = QString("%1_timeout").arg(executableStateID);
			QString timeoutEventID = QString("%1_timeoutID").arg(executableStateID);
			GTASCXMLSend send;
			send.setId(timeoutEventID);
			send.setEvent(timeoutEventName);
			send.setDelay(QString::number(dtimeout) + "ms");
			onEntry.insertSend(send);
			state.setOnEntry(onEntry);

			//always cancel the timeout while exiting the state
			//it has served it's purpose
			GTASCXMLCancel canceltimeout;
			canceltimeout.setSendEventId(timeoutEventID);
			onExit.insertCancel(canceltimeout);

			GTASCXMLTransition timeoutTx;
			timeoutTx.setEvent(timeoutEventName);
			timeoutTx.setStatus(SCXML_STATUS_TIMEOUT);
			GTASCXMLLog timeoutLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::TIMEOUT_LOG, _SetCmd->getInstanceName(), "", "");
			if (rc)     //only for debug mode
			{
				timeoutTx.setCondition(QString("(%1 == 'true')").arg(preDebugVar));

				//additional tx
				GTASCXMLTransition Tx;
				if (actOnFail == "stop")
					Tx.setTarget(SCXML_MAIN_TP_FINAL);
				else
				{
					Tx.setTarget(finaltargetId);
				}
				Tx.setCondition(QString("(%1 != 'true')").arg(preDebugVar));
				Tx.insertLog(timeoutLog);
				state.insertTransitions(Tx);
			}

			if (actOnFail == "stop")
				timeoutTx.setTarget(SCXML_MAIN_TP_FINAL);
			else
			{
				timeoutTx.setTarget(executableTargetID);
			}

			timeoutTx.insertLog(timeoutLog);
			state.insertTransitions(timeoutTx);
		}

		//innsert logs
		GTACMDSCXMLLog log;
		log.setReportType(GTACMDSCXMLLog::REP_ACTION, GTACMDSCXMLLog::REP_ACTCHK);
		log.setCommandName(_CmdName);
		log.setCommandInstanceId(_SetCmd->getInstanceName());
		log.setCommandDescription(_SetCmd->getComment());
		log.setCommandStatement(_SetCmd->getStatement());
		QStringList requirements = _SetCmd->getRequirements();
		log.insertRequirements(requirements);
		QStringList varList;// = state.getVariableList();
		GTACMDSCXMLUtils::insertVariablesInLogReport(_ParamInfoList, varList, log);

		if (_GenNewLogFormat)
			onExit.insertExtraInfo(log.getLogElem());
		state.setOnExit(onExit);
		GTACMDSCXMLUtils::logAllParameters(_SetCmd->getInstanceName(), _ParamInfoList, state, SetState.getIsParamFromCSV());
	}

	if (rc)
	{
		megaState.setId(_StateId);

		GTASCXMLTransition tx;
		tx.setTarget(_TargetId);
		megaState.insertTransitions(tx);

		QStringList varList = state.getVariableList();
		varList.removeDuplicates();

		GTASCXMLState preDebugState = GTACMDSCXMLUtils::getPreDebugState(preDebugStateId, executableStateID, _PrevStateId, _SetCmd->getInstanceName());
		megaState.insertStates(preDebugState);
		megaState.insertStates(state);
		GTASCXMLState debugState = GTACMDSCXMLUtils::getDebugState(debugStateId, finaltargetId, varList, _ParamInfoList);
		megaState.insertStates(debugState);

		megaState.setInitId(preDebugStateId);
		return megaState;
	}
	else
	{
		return state;
	}
}

GTASCXMLState GTACMDSCXMLSet::getMultiSetSCXMLState(const QString &iStateId, const QString &iTargetId, GTAActionBase* forEachGrandParent/*, bool &oCICParamFound*/) const
{
	GTASCXMLState state;
	QString setParamStateId = QString("%1_SetParam").arg(iStateId);
	QString getParamStateId = QString("%1_GetParam").arg(iStateId);
	QString finalStateId = QString("%1_final").arg(iStateId);
	QStringList getListParam; 
	QStringList getListFunctionalStatus;
	GTASCXMLState setParamState;
	GTASCXMLState getParamState;

	setParamState.setId(setParamStateId);

	state.setId(iStateId);
	GTASCXMLFinal final;
	final.setId(finalStateId);
	state.setFinal(final);

	QList<GTAICDParameter> rhsParamList;
	QList<GTAICDParameter> lhsParamList;

	bool useVMACTimeout = false;

	QHash<QString, SingleSetCommandStructure*> toolSETGroup;
	QList <GTAActionSet*> setList;
	QHash<QString, QString> iSerialzationEquationMap;
	_SetCmd->getSetCommandlist(setList);
	GTAActionSet * setToRead = setList.at(0);
	QString paramFileName = GTACMDSCXMLUtils::getExportingFileName();
	QString param = setToRead->getParameter();
	QString functionalStatus = setToRead->getFunctionStatus();
	QString rightParam;
	bool isParamFromCSV = false;
	bool isParentForEach = false;
	bool isFunctionalStatusForEach = false;
	bool isRightParamForEach = false;
	bool isWriteBack = false;
	bool isEquationForeach = false;
	GTAActionForEach *pForEach = nullptr;
	GTAEquationConst *pEquation = dynamic_cast<GTAEquationConst *> (setToRead->getEquation());
	GTAEquationBase *pEquationBase = dynamic_cast<GTAEquationBase*> (setToRead->getEquation());
	if (pEquation != nullptr) {
		rightParam = pEquation->getConstant();
		if (rightParam.startsWith("line"))
		{
			isRightParamForEach = true;
			state.setIsParamFromCSV(true);
		}
	}
	if (param.startsWith("line")) {
		isParamFromCSV = true;
		state.setIsParamFromCSV(true);
	}
	if (pEquationBase != nullptr)
	{
		pEquationBase->getEquationArguments(iSerialzationEquationMap);
		for (auto argument: iSerialzationEquationMap.values())
			if (argument.contains("line."))
			{
				isEquationForeach = true;
				break;
			}
	}
	if (functionalStatus.startsWith("line"))
	{
		isFunctionalStatusForEach = true;
		state.setIsFunctionalStatusFromCSV(true);
	}
	if (isParamFromCSV || isRightParamForEach || isFunctionalStatusForEach)
	{
		int cmdType = _SetCmd->getParent()->getCommandType();
		if (cmdType == 0) {
			GTAActionBase* pActCmd = nullptr;
			pActCmd = dynamic_cast<GTAActionBase*> (_SetCmd->getParent());
			QString action = pActCmd == nullptr ? "" : pActCmd->getAction();
			QString complement = pActCmd == nullptr ? "" : pActCmd->getComplement();
			if (action == ACT_CONDITION && complement == COM_CONDITION_FOR_EACH) {
				isParentForEach = true;
				pForEach = dynamic_cast<GTAActionForEach*> (_SetCmd->getParent());
				isWriteBack = pForEach == nullptr ? false : pForEach->getOverWriteResults();
			}
			else {
				while (pForEach == nullptr)
				{
					GTACommandBase* pParent = pActCmd == nullptr ? nullptr : pActCmd->getParent();
					pActCmd = dynamic_cast<GTAActionBase*> (pParent);
					if (pActCmd == nullptr) {
						break;
					}
					QString parentAction = pActCmd->getAction();
					QString parentComplement = pActCmd->getComplement();
					if (parentAction == ACT_CONDITION && parentComplement == COM_CONDITION_FOR_EACH)
					{
						isParentForEach = true;
						pForEach = dynamic_cast<GTAActionForEach*> (pActCmd);
						isWriteBack = pForEach == nullptr ? false : pForEach->getOverWriteResults();
					}
				}
			}	
		}
		if (forEachGrandParent != nullptr && isParamFromCSV) 
		{
			isParentForEach = true;
			pForEach = dynamic_cast<GTAActionForEach*> (forEachGrandParent);
			isWriteBack = pForEach == nullptr ? false : pForEach->getOverWriteResults();
		}
	}

	if (isFunctionalStatusForEach && isParentForEach)
	{
		QString columnName = functionalStatus.split("__").last();
		int columnParam = 0;
		int lineCount = 0;
		QString line = QString();
		QString delim = pForEach->getDelimiterValue();
		QString csvPath;
		if (pForEach->getRelativePath().isEmpty())
		{
			csvPath = pForEach->getPath();
		}
		else
		{
			QString repoPath = GTAUtil::getRepositoryPath();
			csvPath = repoPath + pForEach->getRelativePath();
		}
		QFile fileObj(csvPath);
		bool rc1 = fileObj.open(QFile::ReadOnly | QFile::Text);
		if (rc1)
		{
			while (!fileObj.atEnd())
			{
				line = fileObj.readLine();
				if (line.endsWith("\n"))
				{
					line.remove("\n");
				}
				QStringList lineList = line.split(delim).replaceInStrings(QRegExp("^\\s+|\\s+$"), "");

				if (lineCount == 0)
				{
					columnParam = lineList.indexOf(columnName);
				}
				else
				{
					if (lineList.size() <= columnParam) {
						break;
					}
					if (columnParam >= 0)
						getListFunctionalStatus.append(lineList.at(columnParam));
				}
				lineCount++;
			}
		}
		fileObj.close();
	}

	if (isParamFromCSV && isParentForEach)
	{
		QString columnName = param.split("__").last();
		int columnParam = 0;
		int lineCount = 0;
		QString line = QString();
		QString delim = pForEach->getDelimiterValue();
		QString csvPath;
		if (pForEach->getRelativePath().isEmpty())
		{
			csvPath = pForEach->getPath();
		}
		else
		{
			QString repoPath = GTAUtil::getRepositoryPath();
			csvPath = repoPath + pForEach->getRelativePath();
		}
		QFile fileObj(csvPath);
		bool rc1 = fileObj.open(QFile::ReadOnly | QFile::Text);
		if (rc1)
		{
			while (!fileObj.atEnd())
			{
				line = fileObj.readLine();
				if (line.endsWith("\n"))
				{
					line.remove("\n");
				}
				QStringList lineList = line.split(delim).replaceInStrings(QRegExp("^\\s+|\\s+$"), "");

				if (lineCount == 0)
				{
					columnParam = lineList.indexOf(columnName);
				}
				else
				{
					if (lineList.size() <= columnParam) {
						break;
					}
					
					SingleSetCommandStructure* set = new SingleSetCommandStructure();
					GTAActionSet *pSetCmd = NULL;
					bool setExists = _SetCmd->getSetAction(0, pSetCmd);

					if (setExists && pSetCmd != NULL) {
						//creating ICD parameter for lhsVar
						GTAICDParameter lhsParam;
						QString lhsVar = columnParam>=0 ? lineList.at(columnParam) : QString();
						QString rhsVar;
						if (lhsVar.split('.').size() == 3) {
							lhsParam.setSignalName(lhsVar);
							lhsParam.setTempParamName(lhsVar);
							resolveParams(lhsParam);
							GTACMDSCXMLUtils::updateWithGenToolParam(lhsParam);

							if (lhsParam.getSignalType().isEmpty() && lhsParam.getSourceType() == "PMR" && lhsParam.getName().count(".") == 2 && lhsParam.getParameterType() == "VMAC")
								useVMACTimeout = true;
							set->lhsParam = lhsParam;

							//creating ICD parameter for lhsVar
							QString functionType, functionValue;
							GTAEquationBase* pEq = pSetCmd->getEquation();
							if (pEq != NULL)
							{
								_EquationCmdSCXML->getSetStructureFilled(pEq, set, rhsParamList, pForEach, lineCount);
							}
							resolveParams(set->rhsParam);
							GTACMDSCXMLUtils::updateWithGenToolParam(set->rhsParam);

							//identifying rhsVar for CONSTANTS
							set->isFSOnly = pSetCmd->getIsSetOnlyFSFixed() || pSetCmd->getIsSetOnlyFSVariable();

							if (getListFunctionalStatus.isEmpty())
							{
								QString functionStatus = pSetCmd->getFunctionStatus();
								if (functionStatus.trimmed().isEmpty())
									functionStatus = ACT_FSSSM_NOCHANGE;

								set->FSVal = functionStatus;
								QString toolname = lhsParam.getToolName();
								toolSETGroup.insertMulti(toolname, set);
							}
							else
							{
								if (getListFunctionalStatus[lineCount - 1] != NULL)
								{
									set->FSVal = getListFunctionalStatus[lineCount - 1];
									QString toolname = lhsParam.getToolName();
									toolSETGroup.insertMulti(toolname, set);
								}
							}
						}	
					}
				}
				lineCount++;
			}
		}
		fileObj.close();
	}
	else if(isRightParamForEach && isParentForEach) {
		QString columnName = rightParam.split("__").last();
		int columnParam = 0;
		int lineCount = 0;
		QString line = QString();
		QString delim = pForEach->getDelimiterValue();
		QString csvPath;
		if (pForEach->getRelativePath().isEmpty())
		{
			csvPath = pForEach->getPath();
		}
		else
		{
			QString repoPath = GTAUtil::getRepositoryPath();
			csvPath = repoPath + pForEach->getRelativePath();
		}
		QFile fileObj(csvPath);
		bool rc1 = fileObj.open(QFile::ReadOnly | QFile::Text);
		if (rc1)
		{
			while (!fileObj.atEnd())
			{
				line = fileObj.readLine();
				if (line.endsWith("\n"))
				{
					line.remove("\n");
				}
				QStringList lineList = line.split(delim).replaceInStrings(QRegExp("^\\s+|\\s+$"), "");

				if (lineCount == 0)
				{
					columnParam = lineList.indexOf(columnName);
				}
				else
				{
					if (lineList.size() <= columnParam) {
						break;
					}
					/*if (lineCount == 1) {
						//creating ICD parameter for rhsVar
						GTAICDParameter rhsParam;
						QString rhsVar = lineList.at(columnParam);
						if (rhsVar.split('.').size() == 3) {
							rhsParam.setSignalName(rhsVar);
							rhsParam.setTempParamName(rhsVar);
							//bool isLHSValid = 
							resolveParams(rhsParam);
							GTACMDSCXMLUtils::updateWithGenToolParam(rhsParam);
							if (rhsParam.getMedia() == "VMAC")
								useVMACTimeout = true;
							rhsParamList.append(rhsParam);
						}
					}*/
					if (columnParam >= 0 && lineList.at(columnParam).split('.').size() == 3) 
						getListParam.append(lineList.at(columnParam));
				}

				lineCount++;

			}
		}
		fileObj.close();
	}

	int totalSetCnt = _SetCmd->getSetActionCount();
	for (int i = 0; i < totalSetCnt; i++)
	{
		//pushing all information into a set structure
		SingleSetCommandStructure* set = new SingleSetCommandStructure();

		//reading individual SETS
		GTAActionSet *pSetCmd = NULL;
		_SetCmd->getSetAction(i, pSetCmd);

		//creating ICD parameter for lhsVar
		GTAICDParameter lhsParam;
		QString lhsVar = pSetCmd->getParameter();
		for (auto& infoParam : _ParamNameInfoList)
		{
			if (infoParam.toUpper() == lhsVar.toUpper())
				lhsVar = infoParam;
		}
		QString rhsVar;
		//lhsParam.setSignalName(lhsVar);
		//lhsParam.setTempParamName(lhsVar);
		if (_ParamNameInfoList.contains(lhsVar))
		{
			lhsParam.setSignalName(lhsVar);
			lhsParam.setTempParamName(lhsVar);
			resolveParams(lhsParam);
			GTACMDSCXMLUtils::updateWithGenToolParam(lhsParam);
			if (lhsParam.getSignalType().isEmpty() && lhsParam.getSourceType() == "PMR" && lhsParam.getName().count(".") == 2 && lhsParam.getParameterType() == "VMAC")
				useVMACTimeout = true;
		}
		else
			lhsParam.setTempParamName(lhsVar);
		//lhsParam.setName(lhsVar);
		//lhsParam.setValue(lhsVar);
		/*bool isLHSValid = */
		/*resolveParams(lhsParam);
		GTACMDSCXMLUtils::updateWithGenToolParam(lhsParam);*/
		//if (lhsParam.getMedia() == "VMAC")
		// Only for VMAC Parameters
		//if (lhsParam.getSignalType().isEmpty())
		/*if (lhsParam.getSignalType().isEmpty() && lhsParam.getSourceType() == "PMR" && lhsParam.getName().count(".") == 2 && lhsParam.getParameterType() == "VMAC")
			useVMACTimeout = true;*/
		//TODO here compare arr and cntr to make sure the correct ones are being used 
		QStringList templist = setParamState.getId().split('_');
		QString cntr;
		QString arr;
		int j = templist.size() - 1;
		while (j >= 0) {
			if (templist.at(j) == "ForEachStates") {
				for (int k = 0; k < j; k++) {
					if (k != 0) {
						cntr += "_" + templist.at(k);
						arr += "_" + templist.at(k);
					}
					else {
						cntr += templist.at(k);
						arr += templist.at(k);
					}

				}
				cntr += "_arr_cntr";
				arr += "_line_arr";
				j = -1;
			}
			j--;
		}
		QStringList nameToCheck = lhsParam.getTempParamName().split('[');
		if (nameToCheck.size() > 1){
			QStringList tempList = nameToCheck.at(1).split("__");
			if (tempList.size() == 2) {
				nameToCheck.replace(1, tempList.at(0));
				nameToCheck.append(tempList.at(1));
				if (nameToCheck.at(0) != arr) {
					lhsVar = arr + '[' + cntr + "]." + nameToCheck.at(2);
					lhsParam.setSignalName(lhsVar);
					lhsParam.setTempParamName(lhsVar);
				}
			}

		}
		set->lhsParam = lhsParam;

		//creating ICD parameter for lhsVar
		QString functionType, functionValue;
		GTAEquationBase *pEq = pSetCmd->getEquation();
		if (pEq != NULL)
		{
			_EquationCmdSCXML->getSetStructureFilled(pEq, set, rhsParamList);
		}
		QString setRhsParam = set->rhsParam.getName();
		for (auto& infoParam : _ParamNameInfoList)
		{
			if (infoParam.toUpper() == setRhsParam.toUpper())
				setRhsParam = infoParam;
		}
		if (_ParamNameInfoList.contains(setRhsParam))
		{
			set->rhsParam.setTempParamName(setRhsParam);
			resolveParams(set->rhsParam);
			GTACMDSCXMLUtils::updateWithGenToolParam(set->rhsParam);
		}
		/*resolveParams(set->rhsParam);
		GTACMDSCXMLUtils::updateWithGenToolParam(set->rhsParam);*/
		//if ("VMAC" == set->rhsParam.getMedia())
		// Only for VMAC Parameter
		/*if (set->rhsParam.getSignalType().isEmpty())
			useVMACTimeout = true;*/

		//identifying rhsVar for CONSTANTS
		set->isFSOnly = pSetCmd->getIsSetOnlyFSFixed() || pSetCmd->getIsSetOnlyFSVariable();

		//if (getListFunctionalStatus.isEmpty())
		//{
			QString functionStatus = pSetCmd->getFunctionStatus();
			QString fsValue = functionStatus;
			if (fsValue.trimmed().isEmpty())
				fsValue = ACT_FSSSM_NOCHANGE;

			set->FSVal = functionStatus;

			QString toolname = lhsParam.getToolName();
			toolSETGroup.insertMulti(toolname, set);
		//}
	}

	//create a get state if rhsparams require get
	if (!rhsParamList.isEmpty())
	{
		// if vmac delay is to be used, ensure that there is a vmac delay available.
		//if user hasn't mentioned a vmac delay i.e. _VmacHiddenDelay is empty, use 100ms delay by default
		//this is done to ensure that the transition has a valid delay to consume even when the user forgets
		getParamState = GTACMDSCXMLUtils::createGetParamInvokeState(rhsParamList, getParamStateId, setParamStateId, QString(), false, QString(), false, useVMACTimeout ? (_VmacHiddenDelay.isEmpty() ? "100ms" : _VmacHiddenDelay) : _HiddenDelay);
		double dtimeout = _SetCmd->getTimeOutInMs();
		QString actOnFail = _SetCmd->getActionOnFail();
		if (dtimeout > 0)
		{
			QString timeoutEventName = QString("%1_timeout").arg(_StateId);

			GTASCXMLTransition timeoutTx;
			timeoutTx.setEvent(timeoutEventName);
			timeoutTx.setStatus(SCXML_STATUS_TIMEOUT);
			if (actOnFail == "stop")
				timeoutTx.setTarget(SCXML_MAIN_TP_FINAL);
			else
				timeoutTx.setTarget(_TargetId);

			GTASCXMLLog timeoutLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::TIMEOUT_LOG, _SetCmd->getInstanceName(), "", "");
			timeoutTx.insertLog(timeoutLog);
			//getParamState.insertTransitions(timeoutTx);
		}
		if(!isRightParamForEach)
			state.insertStates(getParamState);
	}


	//making a list of return in invokes to create relevant transitions
	QStringList invokeReturns;
	//creating only 1 onentry and 1 datamodel
	GTASCXMLOnEntry onEntry;
	GTASCXMLDataModel dataModel;
	int toolcount = 0; bool invokesend = false;
	QStringList tools = toolSETGroup.keys();
	//removing duplicate keys just to iterate through unique keys so that there are no duplicate assigns/onEntries/invokes
	tools.removeDuplicates();
	for (const auto& tool : tools)
	{
		if (tool != "NA")
		{
			GTASCXMLData data;
			//assigning data structure for specific tool
			QString paramArray = QString("%1_SetParamList_%2").arg(setParamStateId, QString::number(toolcount));
			QString retArgName = QString("%1_%2_Return").arg(setParamStateId, QString::number(toolcount));
			data.setDataId(paramArray);
			data.setDataExpr("[]");
			dataModel.insertDataItem(data);
			data.setDataId(retArgName);
			data.setDataExpr("");
			dataModel.insertDataItem(data);

			QList<SingleSetCommandStructure*> structlist;
			structlist = toolSETGroup.values(tool);
			std::reverse(structlist.begin(), structlist.end());

			int arrCount = 0;
			for (auto* setStruct : structlist)
			{
				QString expr = ""; QString loc = "";
				GTASCXMLAssign assign;
				if (setStruct->isFSOnly)
				{
					QString functype = setStruct->lhsParam.getValueType();
					QString signaltype = setStruct->lhsParam.getSignalType();

					//opaque signal handling in new SCXML structure is neccessary as the XML-RPC bridge crashes
					expr = QString("{ 'Parameter': '%1', 'FunctionType':'%2' ,'Status':'%3','FunctionValue':{'Value':'NA'},%4}").arg(setStruct->lhsParam.getName(),
						setStruct->lhsParam.getValueType().contains("opaque", Qt::CaseInsensitive) ? (setStruct->lhsParam.getSignalType().contains("ARINC", Qt::CaseInsensitive) ? "Float" : setStruct->lhsParam.getValueType()) : setStruct->lhsParam.getValueType(),
						setStruct->FSVal, "'File' : '" + paramFileName + "'");
					loc = QString("%1[%2]").arg(paramArray, QString::number(arrCount));
					arrCount++;

					assign.setExpression(expr); assign.setLocation(loc);
					onEntry.insertAssign(assign);
				}
				else
				{

					//opaque signal handling in new SCXML structure is neccessary as the XML-RPC bridge crashes
					expr = QString("{ 'Parameter': '%1', 'FunctionType':'%2' ,'Status':'%3',%4,%5}").arg(setStruct->lhsParam.getName(),
						(setStruct->funcType == EQ_CONST) ? (setStruct->lhsParam.getValueType().contains("opaque", Qt::CaseInsensitive) ? (setStruct->lhsParam.getSignalType().contains("ARINC", Qt::CaseInsensitive) ? "Float" : setStruct->lhsParam.getValueType()) : setStruct->lhsParam.getValueType()) : setStruct->funcType.toUpper(),
						setStruct->FSVal,
						setStruct->funcValue, "'File' : '" + paramFileName + "'");
					loc = QString("%1[%2]").arg(paramArray, QString::number(arrCount));

					assign.setExpression(expr);
					assign.setLocation(loc);
					onEntry.insertAssign(assign);
					QList<GTASCXMLAssign> equationAssigns = _EquationCmdSCXML->getAssignList(paramArray, arrCount, setStruct);
					for (const auto& equationAssign : equationAssigns)
					{
						//if(!equationAssign.getExpression().startsWith("line"))
							onEntry.insertAssign(equationAssign);
					}
					arrCount++;
				}
			}
			QString invokeId = QString("%1_i%2").arg(setParamStateId, QString::number(toolcount));
			GTASCXMLInvoke invoke;
			bool rc = GTACMDSCXMLUtils::getCloneInvoke("set_param.scxml", invoke);

			if (rc)
			{
				//creating an assign to receive the return
				GTASCXMLAssign funcRet;
				funcRet.setExpression(QString("_event.data.%1").arg(retArgName));
				funcRet.setLocation(retArgName);

				//setting the invoke id and inserting assign
				invoke.setInvokeId(invokeId);
				invoke.insertAssign(funcRet);

				GTASCXMLAssign arrayClear;
				arrayClear.setExpression("[]");
				arrayClear.setLocation(paramArray);
				invoke.insertAssign(arrayClear);

				GTASCXMLXMLRPC func = invoke.getXMLRPCFunction();
				func.setToolId(tool);
				GTASCXMLXMLRPC::funcArg arg;
				func.getArgumentByName("ParameterList", arg);
				arg.value = paramArray;
				func.updateArgumentByName("ParameterList", arg);
				GTASCXMLXMLRPC::funcReturn ret;
				func.getReturnArgument(ret);
				ret.name = retArgName;

				func.updateReturnArgument(ret);

				invoke.setInvokeFunction(func);

				//DEV:the invoke delay should be handled onExit. But the scheduler cannot manage timeouts from outside a state
				QString hiddenDelay = useVMACTimeout ? _VmacHiddenDelay : _HiddenDelay;
				if (!hiddenDelay.trimmed().isEmpty() && !invokesend)
				{
					GTASCXMLSend invokeDelaySend;
					QString dealyEvent = QString("%1_dealyEvent").arg(iStateId);
					invokeDelaySend.setDelay(hiddenDelay);
					invokeDelaySend.setEvent(dealyEvent);
					invoke.insertSend(invokeDelaySend);
					invokesend = true;
				}
				setParamState.insertInvokes(invoke);
				invokeReturns.append(retArgName);
			}
			toolcount++;
		}
		else if((isWriteBack || !(!isRightParamForEach && isParamFromCSV)) && getListParam.isEmpty())
		{
			GTASCXMLAssign assign;
			QList<SingleSetCommandStructure*> structlist;
			structlist = toolSETGroup.values(tool);
			//std::reverse(structlist.begin(), structlist.end());

			for (auto* setStruct : structlist)
			{
				if (setStruct->rhsParam.getToolName() == "NA")
				{
					//assign.setExpression(setStruct->rhsParam.getTempParamName().isEmpty() ? "NA" : setStruct->rhsParam.getTempParamName());
					assign.setExpression(setStruct->rhsParam.getName().isEmpty() ? "NA" : setStruct->rhsParam.getName());
				}
				else
				{
					//assign.setExpression(QString("%1.Value").arg(setStruct->rhsParam.getTempParamName()));
					assign.setExpression(QString("%1.Value").arg(setStruct->rhsParam.getName()));
				}
				//assign.setLocation(setStruct->lhsParam.getTempParamName()); //BVU getName());
				assign.setLocation(setStruct->lhsParam.getTempParamName()); //BVU getName());
				if (!assign.getExpression().startsWith("line"))
					onEntry.insertAssign(assign);
				if (isWriteBack) break;
			}
		}
		//toolcount++;
	}

	//

	GTASCXMLTransition OKTx;
	GTASCXMLTransition KOTx;

	if (!invokeReturns.isEmpty())
	{
		QString OKtxCond; QString KOtxCond;
		for (int j = 0; j < invokeReturns.count(); j++)
		{
			QString condition = invokeReturns.at(j);
			if (j < invokeReturns.count() - 1)
			{
				OKtxCond = QString("%2 (%1.Status == 'OK') && ").arg(condition, OKtxCond);
				KOtxCond = QString("%2 (%1.Status != 'OK') && ").arg(condition, KOtxCond);
			}
			else
			{
				OKtxCond = QString("%2 (%1.Status == 'OK') ").arg(condition, OKtxCond);
				KOtxCond = QString("%2 (%1.Status != 'OK') ").arg(condition, KOtxCond);
			}
		}
		OKtxCond = QString("(%1)").arg(OKtxCond);
		KOtxCond = QString("(%1)").arg(KOtxCond);

		if (_HiddenDelay.trimmed().isEmpty())
		{
			OKTx.setEvent(QString("done.invoke._all"));
			KOTx.setEvent(QString("done.invoke._all"));
		}
		else
		{
			//DEV:HiddenDelay managed from invoke because the scheduler cannot handle send from onexit
			//GTASCXMLOnExit onExit;
			//GTASCXMLSend invokeDelaySend;
			QString dealyEvent = QString("%1_dealyEvent").arg(iStateId);
			//invokeDelaySend.setDelay(_HiddenDelay);
			//invokeDelaySend.setEvent(dealyEvent);
			//onExit.insertSend(invokeDelaySend);
			//setParamState.setOnExit(onExit);

			OKTx.setEvent(dealyEvent);
			KOTx.setEvent(dealyEvent);
		}

		OKTx.setStatus("OK");
		OKTx.setCondition(OKtxCond);
		KOTx.setStatus("KO");
		KOTx.setCondition(KOtxCond);

		OKTx.setTarget(finalStateId);
		KOTx.setTarget(finalStateId);

		GTASCXMLLog okLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG, _SetCmd->getInstanceName(), "true", "true");
		GTASCXMLLog koLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::FAIL_LOG, _SetCmd->getInstanceName(), "true", "true");

		OKTx.insertLog(okLog);
		KOTx.insertLog(koLog);

		setParamState.insertTransitions(OKTx);
		setParamState.insertTransitions(KOTx);
	}
	else
	{
		OKTx.setStatus("OK");
		OKTx.setTarget(finalStateId);
		GTASCXMLLog okLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG, _SetCmd->getInstanceName(), "true", "true");
		OKTx.insertLog(okLog);
		setParamState.insertTransitions(OKTx);
	}
	//

	setParamState.setDataModel(dataModel);
	setParamState.setOnEntry(onEntry);
	if (isParamFromCSV && setParamState.getInvokes().size() > 0) {
		//Here we modify datamodel, onentry and invoke to work as expected in a for each loop.
		GTASCXMLDataModel dm = setParamState.getDataModel();
		GTASCXMLData data = GTASCXMLData();
		data.setDataId(setParamState.getId() + "_" + "ForEachTable");
		data.setDataExpr("[]");
		dm.insertDataItem(data);
		setParamState.setDataModel(dm);
		GTASCXMLOnEntry onEntryTemp;
		GTASCXMLAssign assignTemp;
		QStringList templist = setParamState.getId().split('_');
		QString cntr;
		QString arr;
		int i = templist.size() - 1;
		while (i >= 0) {
			if (templist.at(i) == "ForEachStates") {
				for (int j = 0; j < i; j++) {
					if (j != 0) {
						cntr += "_" + templist.at(j);
						arr += "_" + templist.at(j);
					}
					else {
						cntr += templist.at(j);
						arr += templist.at(j);
					}

				}
				cntr += "_arr_cntr";
				arr += "_line_arr";
				i = -1;
			}
			i--;
		}
		foreach(GTASCXMLAssign assign, onEntry.getAssigns()) {
			if (!assign.getLocation().endsWith(".FunctionValue.Value")) {
				if (assign.getLocation().split("[").size() > 1 && assign.getLocation().split("[").at(1).split("]").at(0).contains("arr_cntr")) {
					assign.setLocation(setParamState.getDataModel().getDataItems().at(0).getId() + '[' + cntr + "].FunctionValue.Value");
					QString column = assign.getExpression().split('.').at(assign.getExpression().split('.').size() - 1);
					assign.setExpression(arr + '[' + cntr + "]." + column);
					assignTemp = assign;
					
				} else {
					onEntryTemp.insertAssign(assign);
				}
			} else if (isParamFromCSV && !isRightParamForEach) {
				onEntryTemp.insertAssign(assign);
			}
			

		}
		if(assignTemp.getLocation()!="")		
			onEntryTemp.insertAssign(assignTemp);

		assignTemp.setLocation(data.getId() + "[0]");
		assignTemp.setExpression(setParamState.getDataModel().getDataItems().at(0).getId() + '[' + cntr + ']');
		onEntryTemp.insertAssign(assignTemp);
		
		if (rightParam.split("__").size() == 2) {
			assignTemp.setLocation(assignTemp.getLocation() + ".FunctionValue.Value");
			assignTemp.setExpression(arr + '[' + cntr + "]." + rightParam.split("__").at(1));
			onEntryTemp.insertAssign(assignTemp);
		}
		setParamState.setOnEntry(onEntryTemp);

		QList<GTASCXMLInvoke> invokeTemp;
		invokeTemp = setParamState.getInvokes();

		setParamState.clearInvokes();
		GTASCXMLInvoke invoke = invokeTemp.at(0);
		GTASCXMLXMLRPC funcContainer = invoke.getXMLRPCFunction();
		GTASCXMLXMLRPC::funcArg func;

		GTASCXMLAssign assignTempInvoke;
		assignTempInvoke.setLocation(data.getId());
		assignTempInvoke.setExpression(data.getExpr());
		invoke.insertAssign(assignTempInvoke);
		funcContainer.getArgumentByName("ParameterList", func);
		func.value = data.getId();
		funcContainer.updateArgumentByName("ParameterList", func);
		invoke.setInvokeFunction(funcContainer);
		setParamState.insertInvokes(invoke);

	}
	else if (!getListParam.isEmpty())
	{
		GTAEquationConst * pEqConst = new GTAEquationConst;
		pEqConst->setConstant(getListParam.at(0));
		GTAEquationConst::ConstType local = GTAEquationConst::ConstType::LOCAL;
		pEqConst->setConstType(local);

		GTAEquationBase *pEq = dynamic_cast<GTAEquationBase*> (pEqConst);
		
		SingleSetCommandStructure *set = new SingleSetCommandStructure();
		if (pEq != NULL)
		{
			_EquationCmdSCXML->getSetStructureFilled(pEq, set, rhsParamList);
		}
		
		//if ("VMAC" == set->rhsParam.getMedia())
		// Only for VMAC Parameters
		/*if (set->rhsParam.getSignalType().isEmpty())
			useVMACTimeout = true;*/
			
		getParamState = GTACMDSCXMLUtils::createGetParamInvokeState(rhsParamList, getParamStateId, setParamStateId, QString(), false, QString(), false, useVMACTimeout ? (_VmacHiddenDelay.isEmpty() ? "100ms" : _VmacHiddenDelay) : _HiddenDelay);

		//getting substate to modify it

		GTASCXMLState subState;
		
		if (!getParamState.getStates().isEmpty())
		{
			subState = getParamState.getStates().at(0);
		}
		//TODO Maybe same correction like GTACMDSCXMLCheckValue ligne 683
		//Modifying datamodel to add a value container used in foreach
		GTASCXMLData subData = GTASCXMLData();
		subData.setDataId(getParamState.getId() + "_" + "forEachValue");
		subData.setDataExpr("");
		GTASCXMLDataModel subDm = subState.getDataModel();
		subDm.insertDataItem(subData);
		subState.setDataModel(subDm);

		//Modifying onEntry to assign a different value after each loop
		GTASCXMLOnEntry onEntryTemp;
		GTASCXMLAssign assignTemp;
		if (!subState.getOnEntry().getAssigns().isEmpty())
		{
			assignTemp = subState.getOnEntry().getAssigns().at(0);
		}
		QStringList templist = getParamState.getId().split('_');
		QString cntr;
		QString arr;
		int i = templist.size() - 1;
		while (i >= 0) {
			if (templist.at(i) == "ForEachStates") {
				for (int j = 0; j < i; j++) {
					if (j != 0) {
						cntr += "_" + templist.at(j);
						arr += "_" + templist.at(j);
					}
					else {
						cntr += templist.at(j);
						arr += templist.at(j);
					}

				}
				cntr += "_arr_cntr";
				arr += "_line_arr";
				i = -1;
			}
			i--;
		}
		QString tempString = assignTemp.getLocation();
		assignTemp.setLocation(getParamState.getId() + "_" + "forEachValue");

		if (!tempString.split('[').isEmpty())
		{
			assignTemp.setExpression(tempString.split('[').at(0) + '[' + cntr + ']');
			assignTemp.setExpression(arr + '[' + cntr + "]." + rightParam.split("__").at(1));
		}
		//if (subStates.size() > 1) {
		//}
		onEntryTemp.insertAssign(assignTemp);
		subState.setOnEntry(onEntryTemp);

		//Modifying invoke
		QList<GTASCXMLInvoke> invokeTemp;
		invokeTemp = subState.getInvokes();
		subState.clearInvokes();
		GTASCXMLInvoke invoke;
		if (!invokeTemp.isEmpty())
		{
			invoke = invokeTemp.at(0);
		}
		GTASCXMLXMLRPC funcContainer = invoke.getXMLRPCFunction();
		GTASCXMLXMLRPC::funcArg func;
		funcContainer.getArgumentByName("ParameterList", func);
		func.value = getParamState.getId() + "_" + "forEachValue";
		func.type = "array";
		funcContainer.updateArgumentByName("ParameterList", func);
		funcContainer.setFunctionName(GET_MULTI_PARAM_VALUE_SINGLE_SAMPLE);

		if (!invoke.getAssigns().isEmpty())
		{
			assignTemp = invoke.getAssigns().at(0);
		}
		tempString = assignTemp.getLocation();
		invoke.clearAssigns();
		invoke.insertAssign(assignTemp);

		assignTemp.setLocation(param);
		assignTemp.setExpression(tempString+".Value");
		invoke.insertAssign(assignTemp);

		assignTemp.setLocation(subData.getId());
		assignTemp.setExpression("");
		invoke.insertAssign(assignTemp);

		assignTemp.setLocation(tempString);
		assignTemp.setExpression("[]");
		invoke.insertAssign(assignTemp);


		invoke.setInvokeFunction(funcContainer);
		subState.insertInvokes(invoke);

		
		getParamState.clearStates();
		getParamState.insertStates(subState);
		state.insertStates(getParamState);
	}
	else if (isRightParamForEach && setParamState.getInvokes().size() == 0) {
		GTASCXMLOnEntry onEntryTemp;
		QStringList templist = setParamState.getId().split('_');
		QString cntr;
		QString arr;
		int i = templist.size() - 1;
		while (i >= 0) {
			if (templist.at(i) == "ForEachStates") {
				for (int j = 0; j < i; j++) {
					if (j != 0) {
						cntr += "_" + templist.at(j);
						arr += "_" + templist.at(j);
					}
					else {
						cntr += templist.at(j);
						arr += templist.at(j);
					}

				}
				cntr += "_arr_cntr";
				arr += "_line_arr";
				i = -1;
			}
			i--;
		}
		foreach(GTASCXMLAssign assign, onEntry.getAssigns()) {
			if (assign.getExpression().split('.').size() == 2) {
				assign.setExpression(arr + '[' + cntr + "]." + assign.getExpression().split('.').at(1));
				onEntryTemp.insertAssign(assign);
				setParamState.setOnEntry(onEntryTemp);
			}
		}
		if(onEntry.getAssigns().size() == 0 && rightParam.split("__").size() > 1)
		{
			GTASCXMLAssign assign;
			assign.setExpression(arr + '[' + cntr + "]." + rightParam.split("__").at(1));
			assign.setLocation(param);
			onEntryTemp.insertAssign(assign);
			setParamState.setOnEntry(onEntryTemp);
		}

	}
	else if (isParamFromCSV && isRightParamForEach)
	{
		if (onEntry.getAssigns().size() > 0) {
			GTASCXMLOnEntry onEntryTemp;
			onEntryTemp.insertAssign(onEntry.getAssigns().at(0));
			setParamState.setOnEntry(onEntryTemp);
		}
	} 
	else if (!isParamFromCSV && isRightParamForEach)
	{
		QList<GTASCXMLAssign> assignsToAdd;
		foreach(GTASCXMLAssign assign, onEntry.getAssigns())
		{
			QStringList templist = setParamState.getId().split('_');
			QString cntr;
			QString arr;
			int i = templist.size() - 1;
			while (i >= 0)
			{
				if (templist.at(i) == "ForEachStates")
				{
					for (int j = 0; j < i; j++)
					{
						if (j != 0)
						{
							cntr += "_" + templist.at(j);
							arr += "_" + templist.at(j);
						}
						else
						{
							cntr += templist.at(j);
							arr += templist.at(j);
						}

					}
					cntr += "_arr_cntr";
					arr += "_line_arr";
					i = -1;
				}
				i--;
			}
			GTASCXMLAssign tempAssign;
			tempAssign.setLocation(assign.getLocation().append(".FunctionValue.Value"));
			tempAssign.setExpression(arr + '[' + cntr + "]." + rightParam.split("__").last());
			assignsToAdd.append(tempAssign);
		}
		GTASCXMLOnEntry onEntryTemp = setParamState.getOnEntry();
		foreach(GTASCXMLAssign assign, assignsToAdd) 
		{
			onEntryTemp.insertAssign(assign);
		}
		setParamState.setOnEntry(onEntryTemp);
	}
	else 
	{
		QStringList idSplit = state.getId().split('_');
		if (onEntry.getAssigns().size() == 2 && onEntry.getAssigns().at(1).getExpression().contains("_arr_cntr") && onEntry.getAssigns().at(1).getExpression().contains("_line_arr")) {
			QStringList exprSplit = onEntry.getAssigns().at(1).getExpression().split('_');
			if (idSplit.at(0) != exprSplit.at(0) && idSplit.at(1) != exprSplit.at(1)) {
				GTASCXMLOnEntry onEntryTemp;
				QStringList templist = setParamState.getId().split('_');
				QString cntr;
				QString arr;
				int i = templist.size() - 1;
				while (i >= 0) 
				{
					if (templist.at(i) == "ForEachStates") 
					{
						for (int j = 0; j < i; j++) 
						{
							if (j != 0) 
							{
								cntr += "_" + templist.at(j);
								arr += "_" + templist.at(j);
							}
							else 
							{
								cntr += templist.at(j);
								arr += templist.at(j);
							}

						}
						cntr += "_arr_cntr";
						arr += "_line_arr";
						i = -1;
					}
					i--;
				}
					if (onEntry.getAssigns().at(1).getExpression().split('.').size() == 2)
					{
						onEntryTemp.insertAssign(onEntry.getAssigns().at(0));
						GTASCXMLAssign assign;
						assign.setLocation(onEntry.getAssigns().at(1).getLocation());
						assign.setExpression(arr + '[' + cntr + "]." + onEntry.getAssigns().at(1).getExpression().split('.').at(1));
						onEntryTemp.insertAssign(assign);
						setParamState.setOnEntry(onEntryTemp);
					}
			}
		}
	}

	GTASCXMLTransition tx;
	tx.setTarget(iTargetId);

	state.insertStates(setParamState);
	state.insertTransitions(tx);

	//clearing all SingleSetCommandStructure pointers from hash
	qDeleteAll(toolSETGroup);
	return state;
}
