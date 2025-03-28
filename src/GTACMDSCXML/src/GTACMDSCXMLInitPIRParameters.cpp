#include "GTACMDSCXMLInitPIRParameters.h"
#include "GTASCXMLState.h"
#include "GTAUtil.h"
#include "GTACMDSCXMLLog.h"
#include "GTACMDSCXMLUtils.h"

GTACMDSCXMLInitPIRParameters::GTACMDSCXMLInitPIRParameters(GTAActionSubscribe *pCmdNode, const QString &iStateId, const QString &iTargetId) :
	GTACMDSCXMLBase(pCmdNode, iStateId, iTargetId)
{
	_SubCmd = pCmdNode;
}

GTACMDSCXMLInitPIRParameters::GTACMDSCXMLInitPIRParameters()
{

}

GTACMDSCXMLInitPIRParameters::~GTACMDSCXMLInitPIRParameters()
{

}
QString GTACMDSCXMLInitPIRParameters::getSCXMLString() const
{
	return getSCXMLState().getSCXMLString(_isUSCXMLEn);
}


GTASCXMLState GTACMDSCXMLInitPIRParameters::getSCXMLState()const
{
	GTASCXMLState state;
	QList<GTASCXMLState> multiStates;
	if (_SubCmd != NULL)
	{
		QList<GTAICDParameter> paramList;
		QList<GTAICDParameter> paramListFinal;
		QList<QList<GTAICDParameter>>paramMultiList;
		//do not subscribe any variable apart from PIR parameters
		foreach(GTAICDParameter param, _ParamInfoList)
		{
			if (param.getMedia() == "PIR" || param.getSourceType() == "PIR" || param.getParameterType() == "FIB-IPR")
			{
				paramList.append(param);
			}
		}

		if (_SubCmd->getValues().at(0).startsWith("line")) {
			GTACommandBase * cmdBase = _SubCmd->getParent();
			GTAActionForEach * actionForEach = dynamic_cast<GTAActionForEach*>(cmdBase);
			for (int j = 0; j < _SubCmd->getValues().size(); j++) {
				QString title = _SubCmd->getValues().at(j).split("__").at(1);
				//QStringList paramsInGTA = _CheckValueCmd->getListParameters();
				QStringList CSVContent = actionForEach->getCSVColumnByTitle(title);
				QList<GTAICDParameter> paramListFromCSV;
				for (int i = 0; i < paramList.size(); i++) {
					if (CSVContent.contains(paramList.at(i).getName())) {
						paramListFromCSV.append(paramList.at(i));
					}
				}
				paramListFinal = paramListFromCSV;
				/*for (int i = 0; i < paramListFromCSV.size(); i++)
				{
					int index = CSVContent.indexOf(paramListFinal.at(i).getName());
					if (i != index)
						paramListFinal.move(index, i);
				}*/
				//TODO when multi Init & line PIR both subscribe the same parameters
				paramMultiList.append(paramListFinal);
				paramList = paramListFinal;
				paramListFromCSV.clear();

			}
		}
		if (paramMultiList.size() == 0) {
			state = getSCXMLForParameterSubs(paramList);
		}
		else if (paramMultiList.size() < 2) {
			QList<GTAICDParameter> tempParamList = paramMultiList.at(0);
			state = getSCXMLForParameterSubs(tempParamList);
		}
		else {
			QList<GTAICDParameter> tempParamList = paramMultiList.at(0);
			state = getSCXMLForParameterSubs(tempParamList);
			foreach(QList<GTAICDParameter> loopParamList, paramMultiList) {
				GTASCXMLState tempStates = getSCXMLForParameterSubs(loopParamList);
				multiStates.append(tempStates);
			}

		}
		QList<GTASCXMLState> subStateList = state.getStates();

		if (_SubCmd->getValues().size() > 0 && _SubCmd->getValues().at(0).startsWith("line"))
			state.clearStates();
		for (int j = 0; j < _SubCmd->getValues().size(); j++) {
			if (!_SubCmd->getValues().at(0).startsWith("line"))
				break;
			GTASCXMLState forEachState;
			if (j == 0) {
				forEachState = subStateList.at(j);
			}
			else {
				forEachState = multiStates.at(j).getStates().at(j);
			}
			if (_SubCmd->getValues().size() > 0 && _SubCmd->getValues().at(j).startsWith("line")) {
				//Getting a copy of current states before clearing them 

				QStringList templist = state.getId().split('_');
				QString cntr;
				int i = templist.size() - 1;
				while (i >= 0) {
					if (templist.at(i) == "ForEachStates") {
						for (int k = 0; k < i; k++) {
							if (k != 0) {
								cntr += "_" + templist.at(k);
							}
							else {
								cntr += templist.at(k);
							}

						}
						cntr += "_arr_cntr";
						i = -1;
					}
					i--;
				}

				//Editing transitions to comply with foreach
				QList<GTASCXMLTransition> transitionList;
				if (j == 0) {
					transitionList = subStateList.at(j).getTransitions();
				}
				else {
					transitionList = multiStates.at(j).getTransitions();
				}

				QString target;
				if (j == (_SubCmd->getValues().size() - 1)) {
					target = subStateList.at(subStateList.size() - 1).getTransitions().at(0).getTarget();
				}
				else {
					target = subStateList.at(j).getTransitions().at(0).getTarget();
				}
				forEachState.clearTransitions();
				foreach(GTASCXMLTransition transition, transitionList) {
					transition.setTarget(target);
					forEachState.insertTransitions(transition, false);
				}


				//Creating new datamodel
				GTASCXMLDataModel dm = GTASCXMLDataModel();;
				GTASCXMLData data = GTASCXMLData();
				data.setDataId(subStateList.at(j).getId() + "_" + "InputTable");
				data.setDataExpr("[]");
				dm.insertDataItem(data);
				data.setDataId(subStateList.at(j).getId() + "_" + "ForEachTable");
				data.setDataExpr("[]");
				dm.insertDataItem(data);
				forEachState.setDataModel(dm);

				//Creating new onEntry
				GTASCXMLOnEntry onEntryTemp;
				GTASCXMLAssign assignTemp;
				int index = 0;
				if (paramMultiList.size() < 2) {
					if (paramMultiList.size() == 0)
						paramMultiList.append(paramList);
					foreach(GTAICDParameter param, paramMultiList.at(0)) {
						assignTemp.setLocation((data.getId() + "[%1]").arg(index));
						param.setBusName(param.getName());
						param.setSignalName(param.getName());
						param.setApplicationName(param.getName());
						assignTemp.setExpression(QString("{ 'Name':'%1','%2':{'Media':'PIR','%3': {'ParameterType':'%4'}}}").arg(param.getApplicationName()).arg(param.getBusName()).arg(param.getSignalName()).arg(param.getValueType()));
						onEntryTemp.insertAssign(assignTemp);
						index++;
					}
				}
				else {
					foreach(GTAICDParameter param, paramMultiList.at(j)) {
						assignTemp.setLocation((data.getId() + "[%1]").arg(index));
						param.setBusName(param.getName());
						param.setSignalName(param.getName());
						param.setApplicationName(param.getName());
						assignTemp.setExpression(QString("{ 'Name':'%1','%2':{'Media':'PIR','%3': {'ParameterType':'%4'}}}").arg(param.getApplicationName()).arg(param.getBusName()).arg(param.getSignalName()).arg(param.getValueType()));
						onEntryTemp.insertAssign(assignTemp);
						index++;
					}  
				}
				assignTemp.setLocation(subStateList.at(j).getId() + "_" + "InputTable" + "[0]");
				assignTemp.setExpression(QString(data.getId() + "[%1]").arg(cntr));
				onEntryTemp.insertAssign(assignTemp);
				forEachState.setOnEntry(onEntryTemp);

				//Editing invoke 
				QList<GTASCXMLInvoke> invokeTemp = forEachState.getInvokes();
				forEachState.clearInvokes();
				GTASCXMLInvoke invoke = invokeTemp.at(0);
				GTASCXMLXMLRPC funcContainer = invoke.getXMLRPCFunction();
				GTASCXMLXMLRPC::funcArg func;
				funcContainer.getArgumentByName("ParameterList", func);
				func.value = assignTemp.getLocation();
				funcContainer.updateArgumentByName("ParameterList", func);
				invoke.setInvokeFunction(funcContainer);
				forEachState.insertInvokes(invoke);

				state.insertStates(forEachState);
			}
		}
		//innsert logs Modification that removed too much log information.
		/*GTASCXMLOnExit onExit = state.getOnExit();
		if (_GenNewLogFormat) 
		{
			GTACMDSCXMLLog log;
			log.setReportType(GTACMDSCXMLLog::REP_ACTION, GTACMDSCXMLLog::REP_ACTCHK);
			log.setCommandName(_CmdName);
			log.setCommandInstanceId(_SubCmd->getInstanceName());
			log.setCommandDescription(_SubCmd->getComment());
			log.setCommandStatement(_SubCmd->getStatement());
			QStringList requirements = _SubCmd->getRequirements();
			log.insertRequirements(requirements);
			QStringList varList;// = state.getVariableList();
			GTACMDSCXMLUtils::insertVariablesInLogReport(paramMultiList.at(0), varList, log);
			onExit.insertExtraInfo(log.getLogElem());
		}
		state.setOnExit(onExit);
		if (isforeach)
		{
			GTACMDSCXMLUtils::logPirParameters(_SubCmd->getInstanceName(), _SubCmd->getValues(), state);
		}
		else
		{
			GTACMDSCXMLUtils::logAllParameters(_SubCmd->getInstanceName(), paramList, state);
		}*/
		//innsert logs
		GTACMDSCXMLLog log;
		log.setReportType(GTACMDSCXMLLog::REP_ACTION, GTACMDSCXMLLog::REP_ACTCHK);
		log.setCommandName(_CmdName);
		log.setCommandInstanceId(_SubCmd->getInstanceName());
		log.setCommandDescription(_SubCmd->getComment());
		log.setCommandStatement(_SubCmd->getStatement());
		QStringList requirements = _SubCmd->getRequirements();
		log.insertRequirements(requirements);
		QStringList varList;// = state.getVariableList();
		GTACMDSCXMLUtils::insertVariablesInLogReport(paramList, varList, log);
		GTASCXMLOnExit onExit = state.getOnExit();
		if (_GenNewLogFormat)
			onExit.insertExtraInfo(log.getLogElem());
		state.setOnExit(onExit);
		GTACMDSCXMLUtils::logAllParameters(_SubCmd->getInstanceName(), paramList, state);

	}
	return state;
}


GTASCXMLState GTACMDSCXMLInitPIRParameters::getSCXMLForParameterSubs(QList<GTAICDParameter> &iParamList) const
{
	GTASCXMLState InitPIRParamState;

	InitPIRParamState = GTACMDSCXMLUtils::subscribeParameters(iParamList, _StateId, _TargetId, _SubCmd->getInstanceName());

	return InitPIRParamState;
}
