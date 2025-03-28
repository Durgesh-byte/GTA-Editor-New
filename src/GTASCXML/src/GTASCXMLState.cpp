#include "GTASCXMLState.h"
#include "GTAScxmlLogMessage.h"

GTASCXMLState::GTASCXMLState()
{
    _type = STATE;
}


void GTASCXMLState::setId(const QString &iId)
{
    _id = iId;
}
void GTASCXMLState::setIsParamFromCSV(const bool isParamFromCSV) {
	_isParamFromCSV = isParamFromCSV;
}
bool GTASCXMLState::getIsParamFromCSV() const {
	return _isParamFromCSV;
}
void GTASCXMLState::setIsFunctionalStatusFromCSV(const bool isFunctionalStatusFromCSV) {
    _isFunctionalStatusFromCSV = isFunctionalStatusFromCSV;
}
bool GTASCXMLState::getIsFunctionalStatusFromCSV() const {
    return _isFunctionalStatusFromCSV;
}
QString GTASCXMLState::getId() const
{
    return _id;
}
QString GTASCXMLState::getInitId() const
{
    return _initial;
}
QList<GTASCXMLState> GTASCXMLState::getStates() const
{
    return _states;
}
GTASCXMLOnEntry GTASCXMLState::getOnEntry() const
{
    return _onEntry;
}
GTASCXMLOnExit GTASCXMLState::getOnExit()
{
    return _onExit;
}
QList<GTASCXMLInvoke> GTASCXMLState::getInvokes()
{
    return _invokes;
}
QList<GTASCXMLTransition> GTASCXMLState::getTransitions() const
{
    return _transitions;
}

void GTASCXMLState::setInitId(const QString &iInitId)
{
    _initial = iInitId;
}

void GTASCXMLState::setType(const  STATE_TYPE &iType)
{
    _type = iType;
}

void GTASCXMLState::setOnEntry(const GTASCXMLOnEntry &iOnEntry)
{
    _onEntry = iOnEntry;
}

void GTASCXMLState::insertOnEntry(const GTASCXMLOnEntry &iOnEntry)
{
	foreach(GTASCXMLLog log, iOnEntry.getLogs()) {
		_onEntry.insertLog(log);
	}

	foreach(GTASCXMLAssign assign, iOnEntry.getAssigns()) {
		_onEntry.insertAssign(assign);
	}

	foreach(GTASCXMLSend send, iOnEntry.getSends()) {
		_onEntry.insertSend(send);
	}

	foreach(GTASCXMLCancel cancel, iOnEntry.getCancels()) {
		_onEntry.insertCancel(cancel);
	}

	foreach(QDomElement extraInfo, iOnEntry.getExtraInfo()) {
		_onEntry.insertExtraInfo(extraInfo);
	}

}

void GTASCXMLState::setOnExit(const GTASCXMLOnExit &iOnExit)
{
    _onExit = iOnExit;
}
void GTASCXMLState::setFinal(const GTASCXMLFinal &iFinal)
{
    _final = iFinal;
}
void GTASCXMLState::insertTransitions(const GTASCXMLTransition &iTransitions, bool iSubStates)
{
    if(iSubStates)
    {
        if(_invokes.count()>0)
            _transitions <<iTransitions;
    }
    else
        _transitions <<iTransitions;

    if(iSubStates)
    {
        for(int i=0;i<_states.count();i++)
        {
            _states[i].insertTransitions(iTransitions,true);
        }
    }
}
void GTASCXMLState::insertTransitionAtIndex(const GTASCXMLTransition &iTransitions, int index)
{
    _transitions.insert(index,iTransitions);
}

void GTASCXMLState::removeTransition(const GTASCXMLTransition &iTransitions, bool iSubStates)
{
    if(iSubStates)
    {
        for(int i=0;i<_states.count();i++)
        {
            _states[i].removeTransition(iTransitions,true);
        }
    }
    else
    {
        for (int j=0;j<_transitions.count();j++)
        {
            GTASCXMLTransition transition = _transitions.at(j);
            if (transition.getEvent() == iTransitions.getEvent() && transition.getTarget() == iTransitions.getTarget())
                _transitions.removeAt(j);
        }
    }
}

GTASCXMLDataModel GTASCXMLState::getDataModel() const
{
    return _datamodel;
}
void GTASCXMLState::setDataModel(const GTASCXMLDataModel &iDataModel)
{
    _datamodel = iDataModel;
}
void GTASCXMLState::insertVarList(const QStringList &iVarList)
{
    _varList.append(iVarList);
}

QStringList GTASCXMLState::getVariableList() const
{
    QStringList varList;
    varList.append(_onEntry.getVariableList());
    varList.append(_onExit.getVariableList());
    varList.append(_final.getVariableList());

    for(int i=0;i<_states.count();i++)
    {
        varList.append(_states.at(i).getVariableList());
    }

    for(int i=0;i<_transitions.count();i++)
    {
        varList.append(_transitions.at(i).getVariableList());
    }
    for(int i=0;i<_invokes.count();i++)
    {
        varList.append(_invokes.at(i).getVariableList());
    }
    return varList;
}

bool GTASCXMLState::isNull()
{
    if(_isNested)
    {
        return _states.isEmpty();
    }
    else
    {
        return _transitions.isEmpty();
    }
}
void GTASCXMLState::insertStates(const GTASCXMLState &iState)
{
    _states<< iState;
    if(_initial.isEmpty())
    {
        setInitId(iState.getId());
    }
    _isNested = true;
}
void GTASCXMLState::insertInvokes(const GTASCXMLInvoke &iInvoke)
{
    _invokes<<iInvoke;
}
void GTASCXMLState::clearInvokes()
{
	_invokes.clear();
}
void GTASCXMLState::clearStates()
{
	_states.clear();
}
bool GTASCXMLState::translateXMLToStruct(const QDomElement &iStateElem)
{
    bool rc = false;
    if(!iStateElem.isNull())
    {
        if(iStateElem.nodeName() == SCXML_STATE_STATE)
        {
            rc = true;
            _type = STATE;

            _id = iStateElem.attribute(SCXML_STATE_ID);
            _initial = iStateElem.attribute(SCXML_STATE_INITID);

            if(!_initial.trimmed().isEmpty())
                _isNested = true;
            QDomNodeList childNodes = iStateElem.childNodes();

            if(!childNodes.isEmpty())
            {
                for(int i=0;i<childNodes.count();i++)
                {
                    QDomNode stateChildElem = childNodes.at(i);
                    if(!stateChildElem.isNull())
                    {
                        if(stateChildElem.nodeName() == SCXML_FINAL)
                        {
                            _final.translateXMLToStruct(stateChildElem.toElement());
                        }
                        else if(stateChildElem.nodeName() == SCXML_ONEXIT)
                        {
                            _onExit.translateXMLToStruct(stateChildElem.toElement());
                        }
                        else if(stateChildElem.nodeName() == SCXML_ONENTRY)
                        {
                            _onEntry.translateXMLToStruct(stateChildElem.toElement());
                        }
                        else if(stateChildElem.nodeName() == SCXML_TRANSITION)
                        {
                            GTASCXMLTransition tx;
                            tx.translateXMLToStruct(stateChildElem.toElement());
                            _transitions<<tx;
                        }
                        else if(stateChildElem.nodeName() == SCXML_INVOKE)
                        {
                            GTASCXMLInvoke invoke;
                            invoke.translateXMLToStruct(stateChildElem.toElement());
                            _invokes<<invoke;
                        }
                        else if((stateChildElem.nodeName() == SCXML_STATE_STATE) || (stateChildElem.nodeName() == SCXML_STATE_PARALLEL))
                        {
                            GTASCXMLState state;
                            state.translateXMLToStruct(stateChildElem.toElement());
                            _states<<state;
                        }
                        else
                        {

                        }
                    }

                }
            }
            else
                rc = false;

        }
        else if(iStateElem.nodeName() == SCXML_STATE_PARALLEL)
        {
            _type = PARALLEL;
        }
        else
        {
            rc = false;
        }
    }

    return rc;
}
void GTASCXMLState::embedLog(const bool &iEmbedLog, const QString &iInstanceName)
{
    _embedLog = iEmbedLog;
    _InstanceName = iInstanceName;

    for(int i=0;i<_states.count();i++)
    {
        _states[i].embedLog(_embedLog,_InstanceName);
    }

    if(_embedLog)
        getLogs();
}
void GTASCXMLState::getLogs()
{
    QStringList varList = _onEntry.getVariableList();
    varList.append(_onExit.getVariableList());
    varList.append(_final.getVariableList());
    for(int i=0;i<_invokes.count();i++)
    {
        varList.append(_invokes.at(i).getVariableList());
    }
    for(int i=0;i<_transitions.count();i++)
    {
        varList.append(_transitions.at(i).getVariableList());
    }
    varList.append(_varList);
    varList.removeDuplicates();
    QStringList doneParams;
    for(int i=0;i<varList.count();i++)
    {
        QString var = varList.at(i);

        if(var.trimmed().isEmpty())
            continue;

        if(var.contains("_invokeCntr.value"))
            continue;

        if(var.endsWith(".time_stamp"))
            continue;

        if(!var.endsWith(".value"))
            continue;

        if(doneParams.contains(var))
            continue;



        doneParams.append(var);

        GTAScxmlLogMessage logMessage1;
        bool isNum = false;
        bool isString = false;
        var.toDouble(&isNum);

        if(var.contains("\"") || var.contains("\'"))
            isString = true;

        if(!isString)
        {
            if(isNum)
            {
                logMessage1.isCurrentValParam = false;
            }
            else
            {
                logMessage1.isCurrentValParam = true;
            }
        }
        logMessage1.CurrentValue = var;
        logMessage1.ExpectedValue = var;
        logMessage1.Identifier = _InstanceName;
        logMessage1.FunctionStatus = "NA";
        logMessage1.Result = GTAScxmlLogMessage::OK;
        logMessage1.Status = GTAScxmlLogMessage::Success;
        logMessage1.LOD = GTAScxmlLogMessage::Detail;
        QString log1 = logMessage1.getLogMessage(_embedLog);
        GTASCXMLLog log;
        log.setLabel("'info'");
        log.setExpression(QString("%1").arg(log1));
        _onExit.insertLog(log);
    }
}

QDomElement GTASCXMLState::getSCXML(QDomDocument &iDomDoc, bool iEnUSCXML)  const
{
    if(_type == PARALLEL)
    {
        QDomElement  stateElem = iDomDoc.createElement(SCXML_STATE_PARALLEL);
        stateElem.setAttribute(SCXML_STATE_ID,_id);
        if((_isNested) && (_states.count()>0))
        {
            for(int i=0;i<_states.count();i++)
            {
                stateElem.appendChild(_states.at(i).getSCXML(iDomDoc,iEnUSCXML));
            }
        }
        for(int i=0;i<_transitions.count();i++)
        {
            GTASCXMLTransition tx = _transitions.at(i);
            if(iEnUSCXML && _isNested && tx.getEvent().trimmed().isEmpty())
                tx.setEvent(QString("done.state.%1").arg(_id));

            stateElem.appendChild(tx.getSCXML(iDomDoc));
        }
        return stateElem;
    }
    else
    {
        QDomElement stateElem = iDomDoc.createElement(SCXML_STATE_STATE);
        stateElem.setAttribute(SCXML_STATE_ID,_id);
        if(!_datamodel.isNULL())
        {
            stateElem.appendChild(_datamodel.getSCXML(iDomDoc));
        }
        if((_isNested)  && (_states.count()>0))
        {
            stateElem.setAttribute(SCXML_STATE_INITID,_initial);
        }
        stateElem.appendChild(_onEntry.getSCXML(iDomDoc));

        if((_isNested) && (_states.count()>0))
        {
            for(int i=0;i<_states.count();i++)
            {
                GTASCXMLState tempState = _states.at(i);
                if(!tempState.getId().trimmed().isEmpty())
                    stateElem.appendChild(tempState.getSCXML(iDomDoc));
            }
        }
        else
        {
            for(int i=0;i<_invokes.count();i++)
            {
                GTASCXMLInvoke invoke = _invokes.at(i);

                QString id=invoke.getInvokeId();
                QStringList idSplit = id.split("_",QString::SkipEmptyParts);
                idSplit.removeLast();
                id =QString("%1_i%2").arg(idSplit.join("_"),QString::number(i));
                invoke.setInvokeId(id);
                stateElem.appendChild(invoke.getSCXML(iDomDoc));
            }
        }

        for(int i=0;i<_transitions.count();i++)
        {
            stateElem.appendChild(_transitions.at(i).getSCXML(iDomDoc));
        }
        stateElem.appendChild(_onExit.getSCXML(iDomDoc));

        if((_isNested) && (_states.count()>0))
        {
            if(_final.getId().isEmpty())
            {
                QString newFinalId = QString("%1_final").arg(_id);
                GTASCXMLFinal ff;
                ff.setId(newFinalId);
                stateElem.appendChild(ff.getSCXML(iDomDoc));
            }
            else
                stateElem.appendChild(_final.getSCXML(iDomDoc));
        }


        return stateElem;
    }
}

QString GTASCXMLState::getSCXMLString(bool iEnUSCXML) const
{
    QDomDocument doc;
    if(_states.count()>0)
    {
        QString statesStr = "";
        for(int i=0;i<_states.count();i++)
        {
            GTASCXMLState tempState = _states.at(i);
            if(!tempState.getId().trimmed().isEmpty())
                statesStr.append(tempState.getSCXMLString(iEnUSCXML));
        }
        QDomElement elem= getCurrentStateSCXML(doc,iEnUSCXML);
        doc.appendChild(elem);
        QString tempStateChart = doc.toString();
        tempStateChart.replace("<NESTED_STATE_CONTENT/>",statesStr);
        return tempStateChart;
    }
    else
    {
        QDomElement elem= getSCXML(doc,iEnUSCXML);
        doc.appendChild(elem);
        return doc.toString();
    }
}
QDomElement GTASCXMLState::getCurrentStateSCXML(QDomDocument &iDomDoc, bool iEnUSCXML) const
{
    if(_type == PARALLEL)
    {
        QDomElement  stateElem = iDomDoc.createElement(SCXML_STATE_PARALLEL);
        stateElem.setAttribute(SCXML_STATE_ID,_id);
        if((_isNested) && (_states.count()>0))
        {
            QDomElement  nestedElem = iDomDoc.createElement("NESTED_STATE_CONTENT");
            stateElem.appendChild(nestedElem);
        }
        for(int i=0;i<_transitions.count();i++)
        {
            GTASCXMLTransition tx = _transitions.at(i);
            if(iEnUSCXML && _isNested && tx.getEvent().trimmed().isEmpty())
                tx.setEvent(QString("done.state.%1").arg(_id));

            stateElem.appendChild(tx.getSCXML(iDomDoc));
        }
        return stateElem;
    }
    else
    {
        QDomElement stateElem = iDomDoc.createElement(SCXML_STATE_STATE);
        stateElem.setAttribute(SCXML_STATE_ID,_id);
        if(!_datamodel.isNULL())
        {
            stateElem.appendChild(_datamodel.getSCXML(iDomDoc));
        }
        if((_isNested)  && (_states.count()>0))
        {
            stateElem.setAttribute(SCXML_STATE_INITID,_initial);
        }
        stateElem.appendChild(_onEntry.getSCXML(iDomDoc));

        if((_isNested) && (_states.count()>0))
        {
            QDomElement  nestedElem = iDomDoc.createElement("NESTED_STATE_CONTENT");
            stateElem.appendChild(nestedElem);
        }
        else
        {
            for(int i=0;i<_invokes.count();i++)
            {
                GTASCXMLInvoke invoke = _invokes.at(i);

                QString id=invoke.getInvokeId();
                QStringList idSplit = id.split("_",QString::SkipEmptyParts);
                idSplit.removeLast();
                id =QString("%1_i%2").arg(idSplit.join("_"),QString::number(i));
                invoke.setInvokeId(id);
                stateElem.appendChild(invoke.getSCXML(iDomDoc));
            }
        }
        for(int i=0;i<_transitions.count();i++)
        {

            GTASCXMLTransition tx = _transitions.at(i);
			if(iEnUSCXML && _isNested && tx.getEvent().trimmed().isEmpty())
				tx.setEvent(QString("done.state.%1").arg(_id));

			stateElem.appendChild(tx.getSCXML(iDomDoc));
        }
        stateElem.appendChild(_onExit.getSCXML(iDomDoc));

        if((_isNested) && (_states.count()>0))
        {
            if(_final.getId().isEmpty())
            {
                QString newFinalId = QString("%1_final").arg(_id);
                GTASCXMLFinal ff;
                ff.setId(newFinalId);
                stateElem.appendChild(ff.getSCXML(iDomDoc));
            }
            else
                stateElem.appendChild(_final.getSCXML(iDomDoc));
        }

        return stateElem;
    }
}

void GTASCXMLState::clearTransitions(){
	_transitions.clear();
}
