#include "AINGTALivePalleteCommands.h"
#include "AINGTAUtil.h"
#include "AINGTAActionSetList.h"
#include "AINGTAActionSet.h"
#include "AINGTAGenericToolCommand.h"
#include "AINGTAGenericFunction.h"
#include "AINGTAGenericArgument.h"
#include "AINGTAActionWait.h"
#include "AINGTACheckValue.h"
#include "AINGTAActionPrint.h"
#include "AINGTAActionMaths.h"
#include "AINGTAActionSet.h"
#include "AINGTAActionSetList.h"
#include "AINGTAEquationConst.h"
#include "AINGTAEquationRamp.h"
#include "AINGTAScxmlLogMessage.h"
#include "AINGTAActionManual.h"
#include "AINGTAActionPrintTime.h"
#include "AINGTAActionPrintTable.h"
#include <QTimer>
#include <QDebug>
#include <QDateTime>
#include <QMultiHash>

AINGTALivePalleteCommands::AINGTALivePalleteCommands(AINGTAElement* ipElem)
{
    _pElem = ipElem;
    //perform any kind of initialization or default assignment in init function
}

void AINGTALivePalleteCommands::init()
{
    _blockingResponse.clear();
    _liveTools.clear();
    _localParams.clear();
    _timedOut = false;
    _pTimer = new QTimer(this);
    _pTimer->setTimerType(Qt::PreciseTimer);
    _pTimer->setSingleShot(true);
    QObject::connect(_pTimer,&QTimer::timeout,this,&AINGTALivePalleteCommands::onTimeout);
}

AINGTALivePalleteCommands::~AINGTALivePalleteCommands()
{
    if (nullptr != _pTimer)
    {
        delete _pTimer;
        _pTimer = nullptr;
    }
    //do not delete the element
    //the element is linked from editor window. it is handled there
}

void AINGTALivePalleteCommands::populateLiveTools(QString itoolName,QString itoolIP, int itoolPort)
{
    if (!_liveTools.keys().contains(itoolName))
    {
        LiveParams param;
        param.toolName = itoolName;
        param.toolIP = itoolIP;
        param.port = itoolPort;
        _liveTools.insert(itoolName,param);
    }
}

void AINGTALivePalleteCommands::onTimeout()
{
    _timedOut = true;
    qInfo()<<"timeout hit";
}

void AINGTALivePalleteCommands::executeAndAnalyzeElement()
{
    _pElem->updateCommandInstanceName(QString());
    for (int i=0;i<_pElem->getAllChildrenCount();i++)
    {
        AINGTACommandBase* pCmd = nullptr;
        _pElem->getCommand(i,pCmd);
        _pTimer->stop();
        _timedOut = false;
        if (nullptr != pCmd && !pCmd->getReadOnlyState())
        {
            pCmd->setReadOnlyState(true);
            AINGTACommandBase::CommandType  cmdType = pCmd->getCommandType();
            if(cmdType == AINGTACommandBase::ACTION)
            {
                AINGTAActionBase *pActCmd = dynamic_cast<AINGTAActionBase *>(pCmd);
                QString act = pActCmd->getAction();
                QString complement = pActCmd->getComplement();

                if(act == ACT_PRINT)
                {
                    managePrintCommand(pCmd,complement);
                    pCmd->setExecutionResult("OK");
                }
                else if (act == ACT_WAIT)
                {
                    manageWaitCommand(pCmd,complement);
                    pCmd->setExecutionResult("OK");
                }
                else if (act == ACT_GEN_TOOL)
                {
                    manageExternalToolCommand(pCmd);
                }
                else if (act == ACT_MATHS)
                {
                    manageMathsCommands(pCmd);
                }
                else if (act == ACT_CALL_EXT)
                {
                }
                else if (act == ACT_SET)
                {
                    manageSetListCommands(pCmd);
                    if (!_timedOut || (_pTimer->remainingTime() > 0))
                    {
                        if (pCmd->getExecutionResult() != "KO")
                            pCmd->setExecutionResult("OK");
                    }
                    else
                    {
                        pCmd->setExecutionResult("KO");
                    }
                }
                else if(act == ACT_MANUAL)
                {
                    manageManualActCommands(pCmd);
                }
            }
            else if (cmdType == AINGTACommandBase::CHECK)
            {
                AINGTACheckBase *pCheckCmd = dynamic_cast<AINGTACheckBase *>(pCmd);

                AINGTACheckBase::CheckType chkType = pCheckCmd->getCheckType();
                if(chkType == AINGTACheckBase::VALUE)
                {
                    manageCheckValueCommand(pCmd);
                    if (!_timedOut || (_pTimer->remainingTime() > 0))
                    {
                        if (pCmd->getExecutionResult() != "KO")
                            pCmd->setExecutionResult("OK");
                    }
                    else
                    {
                        pCmd->setExecutionResult("KO");
                    }
                }

            }
        }
    }
    emit finished();
}

AINGTACommandBase* AINGTALivePalleteCommands::manageSetListfromGenerationWindow(QList<AINGTAICDParameter> &iParamList)
{
    AINGTAActionSetList *pSetList = new AINGTAActionSetList(ACT_SET,QString(""));
    foreach(AINGTAICDParameter parameter, iParamList)
    {
        AINGTAActionSet *pSetCmd = new AINGTAActionSet(ACT_SET_INSTANCE,QString(""));
        pSetCmd->setParameter(parameter.getName());
        pSetCmd->setFunctionStatus(parameter.getSSMValue());
        pSetCmd->setIsSetOnlyFS(false);
        pSetCmd->setIsSetOnlyValue(false);

        AINGTAEquationConst *pConst = new AINGTAEquationConst();
        pConst->setConstant(parameter.getValue());
        pConst->setSolidState("1");
        pConst->setContinousState("0");

        AINGTAEquationBase* pBaseEqn = pConst;
        pSetCmd->setEquation(pBaseEqn);
        pSetList->addSetAction(pSetCmd);
    }
    return pSetList;
}

void AINGTALivePalleteCommands::manageExternalToolCommand(AINGTACommandBase *&ipCmd)
{
    AINGTAGenericToolCommand* pExtCmd = dynamic_cast<AINGTAGenericToolCommand*>(ipCmd);
    if (pExtCmd != nullptr)
    {
        QString ip;int port = 0;
        QVariantList functionResults;
        QList<AINGTAGenericFunction> functions = pExtCmd->getFunctions();
        _pTimer->setInterval(static_cast<int>(pExtCmd->getTimeOutInMs()));
        _pTimer->start();
        for (auto function : functions)
        {
            QString toolID = function.getToolID();
            QString retType = function.getReturnParamType();
            QString retName = function.getReturnParamName();
            foreach (LiveParams tool, _liveTools)
            {
                if (tool.toolName == toolID)
                {
                    ip = tool.toolIP;
                    port = tool.port;
                    break;
                }
            }
            if (!ip.isEmpty() && (port > 0))
            {
                QString funcName = function.getFunctionName();
                QVariantList args;
                QList<AINGTAGenericArgument> arguments = function.getAllArguments();
                for (auto argument : arguments)
                {
                    QVariant arg = argument.getUserSelectedValue();
                    args<<QVariant(arg);
                }
                QVariant blockingResponse;
                if (pExtCmd->getTimeOutInMs() > 0)
                {
                    blockingResponse = emit sendBlockingExtToolRequest(ip,port,args,funcName,_pTimer);
                }
                else
                {
                    blockingResponse = emit sendBlockingExtToolRequest(ip,port,args,funcName,nullptr);
                }
                functionResults.append(blockingResponse);
            }
        }
        foreach (QVariant response,functionResults)
        {
            updateCmdResultBasedOnResponse(ipCmd,response);
        }


    }
}

void AINGTALivePalleteCommands::manageWaitCommand(AINGTACommandBase *&ipCmd, const QString iComplement)
{
    AINGTAActionWait* pWaitCmd = dynamic_cast<AINGTAActionWait*>(ipCmd);
    if (iComplement == COM_WAIT_FOR)
    {
        AINGTALivePalleteCommandsThread::sleep(pWaitCmd->getCounter().toInt());
    }
    else if (iComplement == COM_WAIT_UNTIL)
    {
        //to implement
    }
}

void AINGTALivePalleteCommands::manageCheckValueCommand(AINGTACommandBase *&ipCmd)
{
    AINGTACheckValue *pChkValCmd = dynamic_cast<AINGTACheckValue * >(ipCmd);
    bool waituntil = pChkValCmd->getWaitUntil();

    int timeout = static_cast<int>(pChkValCmd->getTimeOutInMs());
    QString conftime,conftimeunit;
    int conftimeout;
    pChkValCmd->getConfCheckTime(conftime,conftimeunit);
    if (conftimeunit == "ms")
        conftimeout = conftime.toInt() * 1000;
    else
        conftimeout = conftime.toInt();

    QList<AINGTAICDParameter> lhsList;
    QHash<QString,AINGTAICDParameter> lhsHash;
    QList<AINGTAICDParameter> rhsList;
    QHash<QString,AINGTAICDParameter> rhsHash;
    bool leftDone = false;

    for (int count=0;count < pChkValCmd->getCount(); count++)
    {
        AINGTAICDParameter param;
        QString parameter = pChkValCmd->getParameter(0);
        QString value = pChkValCmd->getValue(count);
        QString fs = pChkValCmd->getFunctionalStatus(count);
        QString cond = pChkValCmd->getCondition(count);
        QString sdi = pChkValCmd->getSDI(count);
        // bool isOnlyValue = pChkValCmd->getISOnlyCheckValue(count);
        double precision = pChkValCmd->getPrecisionValue(count);
        if (pChkValCmd->getPrecisionType(count).contains("%"))
        {
            precision /= 100;
        }
        QString binaryOperator = pChkValCmd->getBinaryOperator(count);
        if (binaryOperator == "OR")
            leftDone = true;

        param.setName(parameter);
        param.setValue(value);
        if (!fs.trimmed().isEmpty())
            param.setSSMValue(fs);
        else if (!sdi.trimmed().isEmpty())
            param.setSSMValue(sdi);

        if (!leftDone)
        {
            lhsList.append(param);
            lhsHash.insertMulti(cond,param);
        }
        else
        {
            rhsList.append(param);
            rhsHash.insertMulti(cond,param);
        }
    }

    _pTimer->setInterval(timeout);
    _pTimer->start();
    bool globalResult = getGlobalStatusofCheckLists(lhsHash,rhsHash);
    if (waituntil)
    {
        while (!globalResult)
        {
            globalResult = getGlobalStatusofCheckLists(lhsHash,rhsHash);
            qInfo()<<_timedOut;
            qInfo()<<_pTimer->isActive();
            qInfo()<<_pTimer->remainingTime();
            if (_timedOut || (_pTimer->remainingTime() == 0))
                break;
        }
    }
    if (globalResult)
        ipCmd->setExecutionResult("OK");
    else
        ipCmd->setExecutionResult("KO");
}

void AINGTALivePalleteCommands::managePrintCommand(AINGTACommandBase *&ipCmd, const QString iComplement)
{
    AINGTAActionPrint *pPrintCmd = dynamic_cast<AINGTAActionPrint *>(ipCmd);
    if(iComplement == COM_PRINT_MSG)
    {
        QStringList printMessage = pPrintCmd->getValues();

    }
    else if(iComplement == COM_PRINT_PARAM)
    {
        pPrintCmd->getValues();
        foreach (QString param, pPrintCmd->getValues())
        {
            AINGTAICDParameter paramSend;
            paramSend.setName(param);
            // bool status = (
			emit getParameterValue(paramSend);
            qInfo()<<paramSend.getValue();
        }
    }
    else if(iComplement == COM_PRINT_TABLE)
    {
        AINGTAActionPrintTable * pPrintTableCmd = dynamic_cast<AINGTAActionPrintTable *>(ipCmd);

        QStringList tableVarList = pPrintTableCmd->getValues();
        static QHash<QString, QMultiHash<QString,QStringList>>timeInstanceTables;
        QMultiHash<QString,QStringList>tableInstance;
        QStringList paramValueList;

        foreach(QString parameter, tableVarList)
        {
            QString paramValue, paramType, paramUnit;
            if(parameter.split(".").count() == 3)
            {
                AINGTAICDParameter sendParam;
                sendParam.setName(parameter);
                if(!emit getParameterValue(sendParam))
                {
                    ipCmd->setExecutionResult("KO");
                    return;
                }
                else
                {
                    paramType = (sendParam.getSignalType().isEmpty())?"N.A":sendParam.getSignalType();
                    paramUnit = (sendParam.getUnit().isEmpty())? "N.A":sendParam.getUnit();
                    paramValue = sendParam.getValue();
                }
            }
            else
            {
                getOrSetValueForLocalParams(parameter, paramValue);
                paramUnit = QString();
                paramType = QString();
            }
            paramValueList.append(paramType);
            paramValueList.append(paramUnit);
            paramValueList.append(paramValue);

            tableInstance.insertMulti(parameter,paramValueList);
            paramValueList.clear();
        }
        qDebug()<<pPrintTableCmd->getTableName();
        qDebug()<<pPrintTableCmd->getTitleName();
        timeInstanceTables.insert(QDateTime::currentDateTime().toString(), tableInstance);
    }
    else if(iComplement == COM_PRINT_TIME)
    {
        AINGTAActionPrintTime *pPrintTime = dynamic_cast<AINGTAActionPrintTime* >(ipCmd);
        AINGTAActionPrintTime::TimeType timeType = pPrintTime->getTimeType();
        QString timeExpression = QString();

        if(timeType == AINGTAActionPrintTime::Formatted)
            timeExpression = QDateTime::currentDateTime().toString();
        else
            timeExpression = QDateTime::currentDateTimeUtc().toString();
           // timeExpression = static_cast<int>(QDateTime::currentMSecsSinceEpoch());
    }
}

void AINGTALivePalleteCommands::manageMathsCommands(AINGTACommandBase*& ipCmd)
{
    AINGTAActionMaths *pMaths = dynamic_cast<AINGTAActionMaths * >(ipCmd);

    QString lhsParameter = pMaths->getFirstVariable();
    QStringList rhs = pMaths->getRHSVariableList();

    QString expr = pMaths->getSCXMLExpr().simplified().remove(' ');

    if(rhs.count() > 0)
    {
        //seggregate localParams & ICDParams
        //check if it is localParam  get it's value.
        //check for ICD parameter and get value

        foreach (QString param, rhs)
        {
            QString value;

            if(param.split(".").count() == 3)
            {
                AINGTAICDParameter paramSend;
                paramSend.setName(param);
                if(!emit getParameterValue(paramSend))
                {
                    ipCmd->setExecutionResult("KO");
                    return;
                }

                if(expr.contains(paramSend.getName()))
                    expr.replace(paramSend.getName(), paramSend.getValue());
            }
            else
            {
                getOrSetValueForLocalParams(param, value);

                if(expr.contains(param+"+"))
                    expr.replace(param+"+",value+"+");
                if(expr.contains(param+"-"))
                    expr.replace(param+"-",value+"-");
                if(expr.contains(param+"*"))
                    expr.replace(param+"*",value+"*");
                if(expr.contains(param+"/"))
                    expr.replace(param+"/",value+"/");
                if(expr.contains(param+")"))
                    expr.replace(param+")",value+")");
                if(expr.contains(param+","))
                    expr.replace(param+",",value+",");
            }
        }
    }

    //replace expression Math.ln(x) -> Math.log(x)
    if(expr.contains("Math.ln("))
        expr.replace("Math.ln(","Math.log(");

    //replace expression Math.log1p(x) -> Math.log(1+x)
    if(expr.contains("Math.log1p("))
        expr.replace("Math.log1p(", "Math.log(1+");

    //replace expression Math.log2(x) -> Math.log(x)/Math.log(2)
	if(expr.contains("Math.log2("))
    {
        QString expression;
        QString modExpr = expr;
        modExpr = modExpr.simplified().remove(' ');

        for(int i = 0, closingBracesIndex=0; i< modExpr.length(); i++)
        {
            int startIndex = modExpr.indexOf("Math.log2(");
            if(startIndex == -1)
            {
                for(int j =0; j<modExpr.length(); j++)
                    expression.append(modExpr.at(j));
                for (const auto modExp : modExpr)
                    expression.append(modExp);
                break;
            }

            closingBracesIndex = modExpr.indexOf(")", startIndex);
            modExpr.insert(++closingBracesIndex,"/Math.log(2)");

            for(int k =0; k < closingBracesIndex + 12; k++)
                expression.append(modExpr.at(k));
            modExpr.remove(0,closingBracesIndex+12);
        }
        if(expression.contains("Math.log2("))
            expression.replace("Math.log2(","Math.log(");

        expr = expression;
    }

    if(expr.contains("Math.sinh("))
        updateMathExpression(expr, "Math.sinh(");

    if(expr.contains("Math.cosh("))
        updateMathExpression(expr, "Math.cosh(");

    if(expr.contains("Math.tanh("))
        updateMathExpression(expr, "Math.tanh(");


    QScriptEngine myEngine;
    QString exprResult = myEngine.evaluate(expr).toString();
    if(myEngine.hasUncaughtException())
    {
        ipCmd->setExecutionResult("KO");
    }
    else
    {
        if(lhsParameter.split(".").count() == 3)
        {
            QList<AINGTAICDParameter> sendParams;
            AINGTAICDParameter sendParam;
            sendParam.setName(lhsParameter);
            sendParam.setSSMValue(ACT_FSSSM_NO);
            exprResult = QString("FunctionValue:{Value:%1}").arg(exprResult);
            sendParam.setValue(exprResult);
            sendParams.append(sendParam);

            _pTimer->setInterval(3000);
            _pTimer->start();
            QVariant blockingResponse = emit setParameterValue(sendParams,_pTimer);
            updateCmdResultBasedOnResponse(ipCmd,blockingResponse);
        }
        else
        {
            getOrSetValueForLocalParams(lhsParameter,exprResult,true);
            ipCmd->setExecutionResult("OK");
        }
    }
}

void AINGTALivePalleteCommands::manageSetListCommands(AINGTACommandBase*& ipCmd)
{
    AINGTAActionSetList *pSetList = dynamic_cast<AINGTAActionSetList *>(ipCmd);
    int timeout = static_cast<int>(pSetList->getTimeOutInMs());

    QList<AINGTAICDParameter> setParamList;
    for (int count=0;count<pSetList->getSetActionCount();count++)
    {
        AINGTAActionSet* pSet = nullptr;
        pSetList->getSetAction(count,pSet);
        if (nullptr != pSet)
        {
            AINGTAICDParameter setParam;
            QString lhs = pSet->getParameter();
            QString rhs = pSet->getValue();

            QString equationtype,equationvalue;
            pSet->getEquation()->getFunctionStatement(equationtype, equationvalue);
            equationvalue.replace("'","");

            //the next line is performed only to get the valuetype from ICD parameter
            setParam.setName(lhs);
            emit getParameterValue(setParam);

            if (equationtype.compare(EQ_CONST,Qt::CaseInsensitive) == 0)
            {
                //getValueFromBenchOrLocalParams(rhs);
                if (rhs.split(".").count() == 3)
                {
                    AINGTAICDParameter valueParam;
                    valueParam.setName(rhs);
                    if (emit getParameterValue(valueParam))
                    {
                        //value not available
                        pSetList->setExecutionResult("KO");
                        return;
                    }
                    rhs = valueParam.getValue();
                }
                else
                {
                    //local parameter				
                    getOrSetValueForLocalParams(rhs,rhs,false);

                }
                equationvalue = QString("FunctionValue:{Value:%1}").arg(rhs);
            }
            else if (equationtype.compare(EQ_RAMP,Qt::CaseInsensitive) == 0)
            {
                setParam.setValueType(equationtype);
                AINGTAEquationRamp *pRampEq = dynamic_cast<AINGTAEquationRamp*>(pSet->getEquation());
                if (nullptr != pRampEq)
                {
                    QString start = pRampEq->getStart();
                    QString rise = pRampEq->getRiseTime();
                    QString end = pRampEq->getEnd();
                    bool offset = pRampEq->isEndValueOffset();
                    bool isOK;double riseTime = rise.toDouble(&isOK);
                    if (isOK)
                        rise = QString::number(riseTime * 1000);


                    if (start.split(".").count() == 3)
                    {
                        AINGTAICDParameter valueParam;
                        valueParam.setName(start);
                        if (emit getParameterValue(valueParam))
                        {
                            //value not available
                            pSetList->setExecutionResult("KO");
                            return;
                        }
                        start = valueParam.getValue();
                    }
                    else
                    {
                        getOrSetValueForLocalParams(start,start,false);
                    }
                    if (offset)
                        end = QString::number(start.toInt() + end.toInt());

                    equationvalue=QString("%1{'%2':'%3', '%4':'%5','%6':'%7','Solid':'%8','Continuous':'%9'}").arg("'FunctionValue':",
                                                                                                                  XATTR_EQ_RISETIME,rise,
                                                                                                                  "StartValue",start,
                                                                                                                  "EndValue",end,
                                                                                                                  pRampEq->getSolidState(),
                                                                                                                  pRampEq->getContinousState());

                }
            }
            else
            {
                //assign other equation type
                setParam.setValueType(equationtype);
            }
            setParam.setValue(equationvalue);

            if (lhs.split(".").count() == 3)
            {
                setParam.setName(lhs);
                setParam.setSSMValue(pSet->getFunctionStatus());
                setParamList.append(setParam);
            }
            else
            {
                getOrSetValueForLocalParams(lhs,rhs,true);
            }
        }
    }

    _pTimer->setInterval(timeout);
    _pTimer->start();
    QVariant blockingResponse = (emit setParameterValue(setParamList,nullptr));
    updateCmdResultBasedOnResponse(ipCmd,blockingResponse);
}


bool AINGTALivePalleteCommands::valueComparator(QString ilhsVal,QString irhsVal,QString iCondition)
{
    bool rc = false;
    if (iCondition.compare(ARITH_EQ) == 0)
    {
        if (ilhsVal.compare(irhsVal,Qt::CaseSensitive) == 0)
            rc = true;
    }
    else if (iCondition.compare(ARITH_NOTEQ) == 0)
    {
        if (ilhsVal.compare(irhsVal,Qt::CaseSensitive) != 0)
            rc =true;
    }
    else if (iCondition.compare(ARITH_GT) == 0)
    {
        if (ilhsVal.toDouble() > irhsVal.toDouble())
            rc =true;
    }
    else if (iCondition.compare(ARITH_GTEQ) == 0)
    {
        if (ilhsVal.toDouble() >= irhsVal.toDouble())
            rc =true;
    }
    else if (iCondition.compare(ARITH_LT) == 0)
    {
        if (ilhsVal.toDouble() < irhsVal.toDouble())
            rc =true;
    }
    else if (iCondition.compare(ARITH_LTEQ) == 0)
    {
        if (ilhsVal.toDouble() <= irhsVal.toDouble())
            rc =true;
    }
    return rc;
}

bool AINGTALivePalleteCommands::getGlobalStatusofCheckLists(QHash<QString,AINGTAICDParameter> &iLhsList,QHash<QString,AINGTAICDParameter> &iRhsList)
{
    bool lhsStatus = true;
    bool rhsStatus = true;
    //parse and check lhs params first
    foreach (QString cond, iLhsList.keys())
    {
        AINGTAICDParameter param = iLhsList.value(cond);
        //managing local variable is still left

        //first check for the value we want to compare
        QString value = param.getValue();
        if (value.split(".").count() == 3)
        {
            AINGTAICDParameter valueParam;
            valueParam.setName(param.getValue());
            emit getParameterValue(valueParam);
            value = valueParam.getValue();
        }

        emit getParameterValue(param);

        if (!valueComparator(param.getValue(),value,cond))
        {
            lhsStatus = false;
            break;
        }
    }

    //parse and check rhs params first
    foreach (QString cond, iRhsList.keys())
    {
        AINGTAICDParameter param = iRhsList.value(cond);
        //managing local variable is still left

        //first check for the value we want to compare
        QString value = param.getValue();
        if (value.split(".").count() == 3)
        {
            AINGTAICDParameter valueParam;
            valueParam.setName(param.getValue());
            emit getParameterValue(valueParam);
            value = valueParam.getValue();
        }

        emit getParameterValue(param);
        if (!valueComparator(param.getValue(),value,cond))
        {
            rhsStatus = false;
            break;
        }
    }

    if (!iRhsList.isEmpty() && iLhsList.isEmpty())
        return (lhsStatus || rhsStatus);
    else if (iRhsList.isEmpty())
        return lhsStatus;
    else if (iLhsList.isEmpty())
        return rhsStatus;
    else
        return true;
}

void AINGTALivePalleteCommands::updateCmdResultBasedOnResponse(AINGTACommandBase *&ipCmd, QVariant iResponse)
{
    if (iResponse.canConvert<QVariantMap>())
    {
        QVariantMap ret = iResponse.toMap();
        if (ret.contains("Status") && ret.value("Status").canConvert<QString>())
        {
            if (ret.value("Status").toString() == "OK")
                ipCmd->setExecutionResult("OK");
            else if (ret.value("Status").toString() == "KO")
                ipCmd->setExecutionResult("KO");
            else
                ipCmd->setExecutionResult("NA");
        }
    }
    else if (iResponse.canConvert<QString>() && iResponse.toString().contains("EEE:"))
    {
        AINGTAScxmlLogMessage logMessage;
        logMessage.Identifier = ipCmd->getInstanceName();
        logMessage.Result = AINGTAScxmlLogMessage::KO;
        logMessage.ExpectedValue = "";
        logMessage.CurrentValue = iResponse.toString();
        logMessage.FunctionStatus = "";
        logMessage.Status = AINGTAScxmlLogMessage::Fail;
        logMessage.LOD = AINGTAScxmlLogMessage::Main;
        QList<AINGTAScxmlLogMessage> plist;
        const QList<AINGTAScxmlLogMessage> existingList = ipCmd->getLogMessageList();
        plist.append(existingList);
        plist.append(logMessage);

        ipCmd->setLogMessageList(plist);
        ipCmd->setExecutionResult("KO");
    }
	else if (iResponse.canConvert<QString>() && iResponse.toString().contains(BLOCKED_CALL))
	{
		//the call was timed out
		//response was not completed

		AINGTAScxmlLogMessage logMessage;
		logMessage.Identifier = ipCmd->getInstanceName();
		logMessage.Result = AINGTAScxmlLogMessage::KO;
		logMessage.ExpectedValue = "";
		logMessage.CurrentValue = iResponse.toString();
		logMessage.FunctionStatus = "";
		logMessage.Status = AINGTAScxmlLogMessage::TimeOut;
		logMessage.LOD = AINGTAScxmlLogMessage::Main;
		QList<AINGTAScxmlLogMessage> plist;
		const QList<AINGTAScxmlLogMessage> existingList = ipCmd->getLogMessageList();
		plist.append(existingList);
		plist.append(logMessage);

		ipCmd->setLogMessageList(plist);
		ipCmd->setExecutionResult("KO");
	}
	else if (ipCmd->getCommandType() == AINGTACommandBase::ACTION && (dynamic_cast<AINGTAActionBase *>(ipCmd)->getAction() == ACT_MANUAL || dynamic_cast<AINGTAActionBase *>(ipCmd)->getAction() == ACT_GEN_TOOL))
	{
		AINGTAActionManual* pManCmd = dynamic_cast<AINGTAActionManual*>(ipCmd);
		// AINGTAGenericToolCommand* pExtCmd = dynamic_cast<AINGTAGenericToolCommand*>(ipCmd);
		if (pManCmd)
		{
			if (!pManCmd->getUserFeedback())
			{

				AINGTAScxmlLogMessage logMessage1;
				logMessage1.CurrentValue = iResponse.toString();
				logMessage1.Identifier = pManCmd->getInstanceName();
				logMessage1.FunctionStatus = "NA";

				logMessage1.LOD = AINGTAScxmlLogMessage::Main;
				logMessage1.isCurrentValParam = false;


				if (iResponse.compare("false") == 0)
				{
					logMessage1.ExpectedValue = "false";
					logMessage1.Result = AINGTAScxmlLogMessage::KO;
					logMessage1.Status = AINGTAScxmlLogMessage::Fail;
					ipCmd->setExecutionResult("KO");
				}
				else
				{
					logMessage1.ExpectedValue = "true";
					logMessage1.Result = AINGTAScxmlLogMessage::OK;
					logMessage1.Status = AINGTAScxmlLogMessage::Success;
					ipCmd->setExecutionResult("OK");
				}
				QString log = logMessage1.getLogMessage();
				QList<AINGTAScxmlLogMessage> loglist;
				loglist << log;
				pManCmd->setLogMessageList(loglist);
			}
			else
			{
				QStringList varList;
				QList<AINGTAICDParameter> paramList;
				varList << pManCmd->getParameter();
				if (emit getICDParameterList(varList, paramList))
				{
					paramList[0].setValueType(pManCmd->getReturnType());
					paramList[0].setValue(iResponse.toString());
					QVariant blockingResponse = emit setParameterValue(paramList, nullptr);
				}
				else
				{
					//DEV COMMENT: handle local parameter
				}
				//DEV COMMENT: log for userfeedback
				ipCmd->setExecutionResult("OK");
			}
		}
		else
		{
			ipCmd->setExecutionResult("KO");
		}
	}
    else
    {
        //search for more practical return types and handle them
        ipCmd->setExecutionResult("KO");
    }
}

void AINGTALivePalleteCommands::manageManualActCommands(AINGTACommandBase *&ipCmd)
{
    static constexpr auto kDesiredToolName = "IC_0";
    AINGTAActionManual* pManCmd = dynamic_cast<AINGTAActionManual*>(ipCmd);
    if (pManCmd != nullptr)
    {
        if (_liveTools.contains(kDesiredToolName)) {
            auto tool = _liveTools.value(kDesiredToolName);
            QString message = pManCmd->getHtmlMessage();
            QVariantList args;
            QString funcName;
            if (pManCmd->getUserFeedback())
            {
                args << message;
                args << pManCmd->getReturnType();
                funcName = GET_PARAM_VALUE_SINGLE_SAMPLE_MANACT;
            }
            else
            {
                bool waitAck = pManCmd->getAcknowledgetment();
                funcName = DISPLAY_MESSAGE;
                args << message;
                args << waitAck;
                if (!waitAck)
                {
                    //non-blocking call
                    emit sendBlockingExtToolRequest(tool.toolIP, tool.port, args, funcName, nullptr);
                    pManCmd->setExecutionResult("OK");
                    return;
                }
            }
            QVariant blockingResponse = emit sendBlockingExtToolRequest(tool.toolIP, tool.port, args, funcName, nullptr);
            updateCmdResultBasedOnResponse(ipCmd, blockingResponse);
        }
    }
}

/*
converts Math.sinh(x) -> {Math.exp(x)-Math.exp(-x)}/2
         Math.cosh(x) -> {Math.exp(x)+Math.exp(-x)}/2
         Math.tanh(x) ->  Math.sinh(x)/Math.cosh(x)
*/
void AINGTALivePalleteCommands::updateMathExpression(QString &oExpression, QString mathCmd)
{
    QString modExpr = oExpression;
    QString updatedExpr, value;

    modExpr = modExpr.simplified().remove(' ');
    for(int i = 0, j=0; i<modExpr.length();i++)
    {
        if(!modExpr.contains(mathCmd))
        {
            for(int k = 0; k < modExpr.length(); k++)
                updatedExpr.append(modExpr.at(k));
            break;
        }
        bool valExtraction = false;
        int valIndex = modExpr.indexOf(mathCmd);
        for(j =valIndex; j < modExpr.indexOf(")",valIndex); j++)
        {
            if(modExpr.at(j) == "(")
            {
                valExtraction = true; ++j;
            }
            if(valExtraction)
                value.append(modExpr.at(j));
        }
        value.trimmed();
        for(int k = 0; k< valIndex; k++)
            updatedExpr.append(modExpr.at(k));

        QString insertString;
        if(mathCmd.compare("Math.sinh(") == 0)
            insertString = QString("(Math.exp(%1)-Math.exp(-%1))/2").arg(value);
        else if(mathCmd.compare("Math.cosh(") == 0)
            insertString = QString("(Math.exp(%1)+Math.exp(-%1))/2").arg(value);
        else if(mathCmd.compare("Math.tanh(") == 0)
            insertString = QString("(Math.exp(%1)-Math.exp(-%1))/(Math.exp(%1)+Math.exp(-%1))").arg(value);

        for(int k = 0; k<insertString.length(); k++)
            updatedExpr.append(insertString.at(k));

        modExpr.remove(0,++j);
        value.clear();
    }
    oExpression = updatedExpr;
}

void AINGTALivePalleteCommands::getOrSetValueForLocalParams(QString& iParam,QString& iValue,bool iSetTheValue)
{
    bool ok = false;
    iParam.toDouble(&ok);

    //simply assign the parameter as value if the param is a number or a string
    if (ok)
    {
        iValue = iParam;
        return;
    }
    else if (iParam.contains("'"))
    {
        iValue = iParam.replace("'","");
        return;
    }

	if (iValue.isEmpty() || iValue.isNull())
		iValue = "0";

    //insert new value to localparam if forced
    if (iSetTheValue)
        _localParams.insert(iParam,iValue);
    else
    {
        //get the local parameter value from the hash if available and is not to be forced
        if (_localParams.keys().contains(iParam))
            iValue = _localParams.value(iParam);
    }
}
