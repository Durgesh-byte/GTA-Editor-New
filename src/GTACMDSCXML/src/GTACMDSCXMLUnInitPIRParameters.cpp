#include "GTACMDSCXMLUnInitPIRParameters.h"
#include "GTASCXMLState.h"
#include "GTAUtil.h"
#include "GTACMDSCXMLLog.h"
#include "GTACMDSCXMLUtils.h"

GTACMDSCXMLUnInitPIRParameters::GTACMDSCXMLUnInitPIRParameters(GTAActionUnSubscribe *pCmdNode, const QString &iStateId, const QString &iTargetId):
        GTACMDSCXMLBase(pCmdNode,iStateId,iTargetId)
{
    _UnSubCmd= pCmdNode;
}

GTACMDSCXMLUnInitPIRParameters::GTACMDSCXMLUnInitPIRParameters()
{

}

GTACMDSCXMLUnInitPIRParameters::~GTACMDSCXMLUnInitPIRParameters()
{

}
QString GTACMDSCXMLUnInitPIRParameters::getSCXMLString() const
{
    return getSCXMLState().getSCXMLString(_isUSCXMLEn);
}

GTASCXMLState GTACMDSCXMLUnInitPIRParameters::getSCXMLState()const
{
	GTASCXMLState state;
	QList<GTASCXMLState> multiStates;
	bool isforeach = false;
	if (_UnSubCmd != NULL)
	{
		QList<GTAICDParameter> paramList;
		QList<GTAICDParameter> paramListFinal;
		QList<QList<GTAICDParameter>>paramMultiList;
		//do not unsubscribe any variable apart from PIR parameters
		foreach(GTAICDParameter param, _ParamInfoList)
		{
			if (param.getMedia() == "PIR" || param.getSourceType() == "PIR" || param.getParameterType() == "FIB-IPR")
				paramList.append(param);
		}
		for (int i = 0; i < _UnSubCmd->getValues().size(); i++)
		{
			if (_UnSubCmd->getValues().at(i).startsWith("line")) {
				isforeach = true;
				GTACommandBase * cmdBase = _UnSubCmd->getParent();
				GTAActionForEach * actionForEach = dynamic_cast<GTAActionForEach*>(cmdBase);
				for (int j = 0; j < _UnSubCmd->getValues().size(); j++) {
					QString title = _UnSubCmd->getValues().at(j).split("__").at(1);
					//QStringList paramsInGTA = _CheckValueCmd->getListParameters();
					QStringList CSVContent = actionForEach->getCSVColumnByTitle(title);
					if (paramList.size() != CSVContent.size()) {
						QList<GTAICDParameter> paramListFromCSV;
						for (int k = 0; k < paramList.size(); k++) {
							if (CSVContent.contains(paramList.at(k).getName())) {
								paramListFromCSV.append(paramList.at(k));
							}
						}
						for (const auto& Param : CSVContent) {
							for (int k = 0; k < paramListFromCSV.size(); k++) {
								if (paramListFromCSV.at(i).getName() == Param) {
									paramListFinal.append(paramListFromCSV.at(k));
								}
							}
						}
						//TODO when multi Init & line PIR both subscribe the same parameters
						paramMultiList.append(paramListFinal);
						if (_UnSubCmd->getValues().size() > 1) {
							paramListFinal.clear();
						}
						//paramList = paramListFinal;
						paramListFromCSV.clear();
					}
				}

			}
		}

		if (paramMultiList.size() == 0) {
			state = getSCXMLForParameterUnSubs(paramList);
		}
		else if (paramMultiList.size() < 2) {
			QList<GTAICDParameter> tempParamList = paramMultiList.at(0);
			state = getSCXMLForParameterUnSubs(tempParamList);
		}
		else {
			QList<GTAICDParameter> tempParamList = paramMultiList.at(0);
			state = getSCXMLForParameterUnSubs(tempParamList);
			foreach(QList<GTAICDParameter> loopParamList, paramMultiList) {
				GTASCXMLState tempStates = getSCXMLForParameterUnSubs(loopParamList);
				multiStates.append(tempStates);
			}

		}

		QList<GTASCXMLState> subStateList = state.getStates();

		if (_UnSubCmd->getValues().size() > 0 && _UnSubCmd->getValues().at(0).startsWith("line"))
			state.clearStates();
		for (int j = 0; j < subStateList.size(); j++) {
			GTASCXMLState forEachState = subStateList.at(j);
			if (_UnSubCmd->getValues().size() > j && _UnSubCmd->getValues().at(j).startsWith("line")) {
				if (_StateId.split('_').at(_StateId.split('_').size() - 2) == "ForEachStates") {
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
					QList<GTASCXMLTransition> transitionList = subStateList.at(j).getTransitions();
					QString target;
					if (j == (_UnSubCmd->getValues().size() - 1)) {
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
					GTASCXMLDataModel dm = GTASCXMLDataModel();
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
					if (paramMultiList.size() < 2) 
					{
						if (paramMultiList.size() == 0)
							paramMultiList.append(paramList);
						foreach(GTAICDParameter param, paramMultiList.at(0)) {
							assignTemp.setLocation((data.getId() + "[%1]").arg(index));
							param.setSignalName(param.getName());
							param.setBusName(param.getName());
							param.setApplicationName(param.getName());
							assignTemp.setExpression(QString("{ 'Name':'%1','%2':{'Media':'PIR','%3': {'ParameterType':'%4'}}}").arg(param.getApplicationName()).arg(param.getBusName()).arg(param.getSignalName()).arg(param.getValueType()));
							onEntryTemp.insertAssign(assignTemp);
							index++;
						}
					}
					else 
					{
						foreach(GTAICDParameter param, paramMultiList.at(j)) {
							assignTemp.setLocation((data.getId() + "[%1]").arg(index));
							param.setSignalName(param.getName());
							param.setBusName(param.getName());
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
					funcContainer.getArgumentByName("", func);
					func.value = assignTemp.getLocation();
					funcContainer.updateArgumentByName("", func);
					invoke.setInvokeFunction(funcContainer);
					forEachState.insertInvokes(invoke);

					state.insertStates(forEachState);
				}
			}
		}
		GTACMDSCXMLUtils::logAllParameters(_UnSubCmd->getInstanceName(), paramList, state);
		/*if (isforeach)
		{
			GTACMDSCXMLUtils::logPirParameters(_UnSubCmd->getInstanceName(), _UnSubCmd->getValues(), state);
		}
		else
		{
			GTACMDSCXMLUtils::logAllParameters(_UnSubCmd->getInstanceName(), paramList, state);
		}*/
	}
	return state;
}


GTASCXMLState GTACMDSCXMLUnInitPIRParameters::getSCXMLForParameterUnSubs(QList<GTAICDParameter> &iParamList) const
{
    GTASCXMLState UnInitPIRParamState;

    UnInitPIRParamState = GTACMDSCXMLUtils::unSubscribeParameters(iParamList,_StateId,_TargetId,_UnSubCmd->getInstanceName());

    return UnInitPIRParamState;
}
