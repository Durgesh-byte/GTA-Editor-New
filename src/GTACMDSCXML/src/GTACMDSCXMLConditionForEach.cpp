#include "GTACMDSCXMLConditionForEach.h"
#include "GTASCXMLState.h"
#include "GTAUtil.h"
#include "GTACMDSCXMLLog.h"
#include "GTACMDSCXMLUtils.h"



GTACMDSCXMLConditionForEach::GTACMDSCXMLConditionForEach(GTAActionForEach *pCmdNode, const QString &iStateId, const QString &iTargetId):
        GTACMDSCXMLBase(pCmdNode,iStateId,iTargetId)
{
    _ForEachCmd = pCmdNode;
    _IncludeLoopTimeoutInSubStates = false;
}

GTACMDSCXMLConditionForEach::GTACMDSCXMLConditionForEach()
{

}

GTACMDSCXMLConditionForEach::~GTACMDSCXMLConditionForEach()
{

}
QString GTACMDSCXMLConditionForEach::getSCXMLString() const
{
    return getSCXMLState().getSCXMLString(_isUSCXMLEn);
}

GTASCXMLState GTACMDSCXMLConditionForEach::getSCXMLState()const
{
    // basestate which will consist compounded states (getParam, decision, content)
    GTASCXMLState ForEachState;
    if(_ForEachCmd != NULL)
    {
        ForEachState.setId(_StateId);

        //action on fail enabled/disabled
	QString actOnFail = _ForEachCmd->getActionOnFail();
	QString finalStateId = QString("%1_final").arg(_StateId);

        //timeout enabled/disabled
        //timeout disabled because loop needs to run till end of file

        //set a datamodel
        GTASCXMLData forData;
        GTASCXMLDataModel datamodel;
        forData.setDataExpr("[]");
        QString filename;
        
        if (_ForEachCmd->getRelativePath().isEmpty())
        {
            filename = _ForEachCmd->getPath().split("/").last();
        }
        else
        {
            filename = _ForEachCmd->getRelativePath().split("/").last();
        }
        filename.remove(".csv",Qt::CaseInsensitive);
        QString dataid = QString("%1_line_arr").arg(_StateId);
        forData.setDataId(dataid);

        datamodel.insertDataItem(forData);

        forData.setDataExpr("-1");
        dataid = QString("%1_arr_cntr").arg(_StateId);
        forData.setDataId(dataid);

        datamodel.insertDataItem(forData);
        ForEachState.setDataModel(datamodel);

        //get the column map for the existing for each
        //create an on entry
        QList<QHash<QString,QString> > columnMap;
        QStringList columnHeaders;
        _ForEachCmd->getColumnMap(columnMap);
        columnHeaders = _ForEachCmd->getHeaderColumns();

        GTASCXMLOnEntry OnEntry;
        GTASCXMLAssign assign;
        //OnEntry.insertAssign();
        QString arrayid = QString("%1_line_arr").arg(_StateId);

        //forming the JSON expression
        for (int i=0;i<columnMap.count();i++)
        {
            //Map(Colname,Value)
            QHash<QString,QString> line = columnMap.at(i);
            QString expr;
            QHash<QString,double> jsonDouble;
            foreach(QString key, line.keys())
            {
                bool ok = false;
                double value = line.value(key).toDouble(&ok);
                QString exprtemp;
                if (ok)
                {
                    jsonDouble.insert(QString("%1[%2].%3").arg(arrayid,QString::number(i),key),value);
                    exprtemp = QString("'%1':''").arg(key);
                }
                else
                {
					//checking if CSV contains " and replaces it with ' 
					if (line.value(key).contains("\"")) {
						QString temp= line.value(key);
						temp.replace("\"", "'");
						exprtemp = QString("'%1':'%2'").arg(key, temp);
					}
					else
					{
                        QString op=line.value(key);

                        if (QString::compare(op,"=") == 0)
                        {
                            op = "==";
                            exprtemp = QString("'%1':'%2'").arg(key, op);

                        }
                        else if (QString::compare(op,"NOT=") == 0)
                        {
                            op = "!=";
                            exprtemp = QString("'%1':'%2'").arg(key, op);
                        }
                        else
                        {
                            exprtemp = QString("'%1':'%2'").arg(key, op);
                        }
						
					}
                }
                expr.append(exprtemp);
                expr.append(",");
            }
            expr = expr.remove(expr.count()-1,1);
            expr = "{"+expr+"}";
            assign.setExpression(expr);
            assign.setLocation(QString("%1[%2]").arg(arrayid,QString::number(i)));            
            OnEntry.insertAssign(assign);

            //ensuring the values are send as values rather than string
            QHash<QString, double>::iterator j;
			QString ExpNotation = QString();
            for (j = jsonDouble.begin(); j != jsonDouble.end(); ++j)
            {
                GTASCXMLAssign assignspecific;
				ExpNotation = QString::number(j.value());
				// Double format has a limlit of 6 significatives number
				if(ExpNotation.contains("e+6"))
					assignspecific.setExpression(QString::number(j.value(),'f',0));
				else
					assignspecific.setExpression(QString::number(j.value(), 'g',10));
                assignspecific.setLocation(j.key());
                OnEntry.insertAssign(assignspecific);
            }
        }

				GTASCXMLLog log0 = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG, _ForEachCmd->getInstanceName(), "true", "true");
        QString log0Expr = log0.getExpression();
        log0Expr.replace("]#for each row in","]#_START_ for each row in");
        GTASCXMLLog startlog;startlog.setExpression(log0Expr);
        OnEntry.insertLog(startlog);

        ForEachState.setOnEntry(OnEntry);

        //decision state
        GTASCXMLState decisionState = getDecisionState();
        ForEachState.insertStates(decisionState);

        GTASCXMLState containerState;
        //inserting states within foreach according to relevant action/check
        for(int i=0;i<_Children.count();i++)
        {
            GTACMDSCXMLBase* pCmd = _Children.at(i);
            if(pCmd != NULL)
            {
                containerState = pCmd->getSCXMLState();
                //appending the states inside the loop
                //ForEachState.insertStates(containerState);
            }
        }
        //append to the onExit of a container if the user wants to write back into the table
        if (_ForEachCmd->getOverWriteResults())
        {
            GTASCXMLOnExit onExitContainer = containerState.getOnExit();
            foreach (QString header, columnHeaders)
            {
                QString lineVal = QString("%1[%2].%3").arg(arrayid,dataid,header);
                GTASCXMLLog detailLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::INFO_LOG,_ForEachCmd->getInstanceName(),QString("line.%1[index: '+%2+']").arg(header,dataid),lineVal);
                onExitContainer.insertLog(detailLog);
            }
            containerState.setOnExit(onExitContainer);
        }

        ForEachState.insertStates(containerState);

        //exit transition to next state
        GTASCXMLTransition tx;
        tx.setStatus("OK");
        tx.setTarget(_TargetId);

        ForEachState.insertTransitions(tx);


        //innsert logs
        GTACMDSCXMLLog log;
        log.setReportType(GTACMDSCXMLLog::REP_ACTION,GTACMDSCXMLLog::REP_ACTCHK);
        log.setCommandName(_CmdName);
        log.setCommandInstanceId(_ForEachCmd->getInstanceName());
        log.setCommandDescription(_ForEachCmd->getComment());
        log.setCommandStatement(_ForEachCmd->getStatement());
        QStringList requirements = _ForEachCmd->getRequirements();
        log.insertRequirements(requirements);
        QStringList varList;// = ForEachState.getVariableList();
        GTACMDSCXMLUtils::insertVariablesInLogReport(_ParamInfoList,varList,log);

        GTASCXMLOnExit onExit = ForEachState.getOnExit();

        //adding main log
		GTASCXMLLog log1 = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG, _ForEachCmd->getInstanceName(), "true", "true");
        QString log1Expr = log1.getExpression();
        log1Expr.replace("]#for each row in","]#_END_ for each row in");
        GTASCXMLLog endlog;
        endlog.setExpression(log1Expr);
        onExit.insertLog(log1);
        onExit.insertLog(endlog);

        if(_GenNewLogFormat)
            onExit.insertExtraInfo(log.getLogElem());
        ForEachState.setOnExit(onExit);
        GTACMDSCXMLUtils::logAllParameters(_ForEachCmd->getInstanceName(),_ParamInfoList,ForEachState);
    }

    return ForEachState;
}


GTASCXMLState GTACMDSCXMLConditionForEach::getDecisionState()const
{

    //timeout disabled because loop needs to run till end of file
    //double dTimeoutVal = _ForEachCmd->getTimeOutInMs();
    double dTimeoutVal = 0;
    QString actOnFail = _ForEachCmd->getActionOnFail();

    QString finalStateId = QString("%1_final").arg(_StateId);
    QString timeoutEventName = QString("%1_timeoutEvent").arg(_StateId);
    QString decisionStateId = QString("%1_Decision").arg(_StateId);
    QString ForEachStatesId = QString("%1_ForEachStates").arg(_StateId);

    GTASCXMLState decisionState;
    decisionState.setId(decisionStateId);

    GTASCXMLOnEntry onEntry;
    QString dummyEventName = QString("%1_dummy_event").arg(decisionStateId);
    GTASCXMLSend dummyEventSend;
    dummyEventSend.setDelay("50ms");
    dummyEventSend.setEvent(dummyEventName);
    onEntry.insertSend(dummyEventSend);

    QList<QHash<QString,QString> > columnMap;
    _ForEachCmd->getColumnMap(columnMap);

    QString filename;
    if (_ForEachCmd->getRelativePath().isEmpty())
    {
        filename = _ForEachCmd->getPath().split("/").last();
    }
    else
    {
        filename = _ForEachCmd->getRelativePath().split("/").last();
    }
    filename.remove(".csv",Qt::CaseInsensitive);

    QString condStmt;
    QString arrayName = QString("%1_line_arr").arg(_StateId);
    QString counterName = QString("%1_arr_cntr").arg(_StateId);

    GTASCXMLAssign assign;
    QString expression = QString("%1 + 1").arg(counterName);
    assign.setExpression(expression);
    assign.setLocation(counterName);
    onEntry.insertAssign(assign);

    //identify if the csv file has usable content. If no columns are available, make the command statement true to run for atleast one iteration
    condStmt = QString("%1 == %2").arg(counterName,QString::number(columnMap.count()-1));

    GTASCXMLUtils::validateExpression(condStmt);

    GTASCXMLTransition trueTx;
    GTASCXMLTransition falseTx;
    GTASCXMLTransition timeout;

    if(dTimeoutVal >0 )
    {
        timeout.setEvent(timeoutEventName);
        timeout.setStatus("TIMEOUT");

        if(actOnFail == "stop")
            timeout.setTarget(SCXML_FINAL_STATE_ID);
        else
            timeout.setTarget(finalStateId);

        GTASCXMLLog timeoutLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::TIMEOUT_LOG,_ForEachCmd->getInstanceName(),"Timeout","Timeout");
        timeout.insertLog(timeoutLog);
        decisionState.insertTransitions(timeout);
    }

    QString CondParam = QString("%1_CondVar").arg(_StateId);

    GTASCXMLData condData;
    condData.setDataExpr(condStmt);
    condData.setDataId(CondParam);

    GTASCXMLDataModel datamodel;
    datamodel.insertDataItem(condData);
    decisionState.setDataModel(datamodel);

    trueTx.setCondition(QString("%1 == true").arg(CondParam));
    falseTx.setCondition(QString("%1 == false").arg(CondParam));

    trueTx.setTarget(finalStateId);
    falseTx.setTarget(ForEachStatesId);

    GTASCXMLLog successLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG,_ForEachCmd->getInstanceName(),CondParam, "true");
    GTASCXMLLog failLog = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::FAIL_LOG,_ForEachCmd->getInstanceName(),CondParam,"false");

	QString expr = successLog.getExpression();
	//QString expr1 = failLog.getExpression();
	expr.replace(CondParam, QString("'+%1+'").arg(CondParam));
	//expr1.replace(CondParam, QString("'+%1+'").arg(CondParam));
	successLog.setExpression(expr);
	
	//failLog.setExpression(expr1);
	trueTx.insertLog(successLog);

    if(_RepeatLogs)
        falseTx.insertLog(failLog);
	
    trueTx.setEvent(dummyEventName);
    falseTx.setEvent(dummyEventName);

    decisionState.insertTransitions(trueTx);
    decisionState.insertTransitions(falseTx);

    decisionState.setOnEntry(onEntry);
	
    return decisionState;
}


