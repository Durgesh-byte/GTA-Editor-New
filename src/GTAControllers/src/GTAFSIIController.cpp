//#include "GTAFSIIController.h"
//#include "GTAFSIIParser.h"
//#include "GTAFSIIUtil.h"
//#include "GTAHeader.h"
//#include <QSqlQuery>
//#include <QVariant>
//#include "GTASystemServices.h"
//#include <QSqlResult>
//#include <QVariant>
//#include <QDir>

//GTAFSIIController* GTAFSIIController::_pFSIIController = 0;

//GTAFSIIController::GTAFSIIController()
//{
    
//}

//GTAFSIIController* GTAFSIIController::getFSIIController()
//{
//    if(_pFSIIController != NULL)
//        return _pFSIIController;
//    else
//    {
//        _pFSIIController = new GTAFSIIController();
//        return _pFSIIController;
//    }
//}

//bool GTAFSIIController::parseFiles(QSqlDatabase &iParamDB, const QString &iImportPath, const QString &iErrorPath)
//{
//    bool rc = false;
//    _ParameterDatabase = iParamDB;

//    GTAFSIIParser fs2Parser;
//    rc = fs2Parser.parseFiles(iImportPath,iErrorPath);
//    if(rc)
//    {
//        QList<GTAFSIIFile> fs2DataList = fs2Parser.getFSIIData();
//        _FSParamAndSignalType = fs2Parser.getFSParameters();
//        //processFSParameters();
//        convertToGTA(fs2DataList);
//    }
//    return rc;
//}

//bool GTAFSIIController::parseFile(QSqlDatabase &iParamDB, const QFileInfo &iFSIIFile, const QString &iErrorPath)
//{
//    bool rc = false;
//    _ParameterDatabase = iParamDB;

//    GTAFSIIParser fs2Parser;
//    fs2Parser.setLogPath(iErrorPath);
//    rc = fs2Parser.parseFile(iFSIIFile);
//    if(rc)
//    {
//        QList<GTAFSIIFile> fs2DataList = fs2Parser.getFSIIData();
//        _FSParamAndSignalType = fs2Parser.getFSParameters();
//        convertToGTA(fs2DataList);
//    }
//    return rc;
//}
//void GTAFSIIController::processFSParameters()
//{
//    QList<QString> ParamList = _FSParamAndSignalType.keys();

//    for(int i = 0; i < ParamList.count(); i++)
//    {
//        QString param = ParamList.at(i);
//        GTAFSIISignal objSignal = _FSParamAndSignalType.value(param);

//        QString application = objSignal.getApplication();
//        QString media = objSignal.getMedia();
//        QString busName = objSignal.getBusName();
//        QString message = objSignal.getMessage();
//        QString fsName = objSignal.getFSName();
//        QString equipment = objSignal.getEquipment();

//        QString appQueryStr = "APPLICATION ='"+application+"'";
//        QString mediaQueryStr = "AND MEDIA ='"+media+"'";
//        QString busQueryStr = " AND BUSNAME ='"+busName+"'";
//        QString messageQueryStr = " AND MESSAGE ='"+message+"'";
//        QString fsNameQueryStr = " AND FS_NAME ='"+fsName+"'";
//        QString equipmentQueryStr  = " AND EQUIPNAME ='"+equipment+"'";

//        QString Query = QString("SELECT * FROM %1 WHERE ").arg(PARAMETER_TABLE);

//        QString SelectQuery = QString("%1%2%3%4%5%6%7;").arg(Query,mediaQueryStr,messageQueryStr,fsNameQueryStr,appQueryStr,busQueryStr,equipmentQueryStr);


//        QSqlQuery getParamQ(_ParameterDatabase);

//        if(getParamQ.exec(SelectQuery))
//        {
//            if(getParamQ.isActive() && getParamQ.isSelect())
//            {
//                if(getParamQ.first())
//                {
//                    QString SignalName = getParamQ.value(16).toString();
//                    QString fsParamName = QString("%1.%2.%3").arg(application,busName,SignalName);
//                    _ParameterMap.insert(param,fsParamName);
//                }
//            }
//            else
//            {
//                _ParameterMap.insert(param,PARAM_NOT_FOUND);
//                _ErrorFile.open(QFile::Append|QIODevice::Text);
//                QString err = QString("%1%2\n").arg(param,PARAM_NOT_FOUND);
//                _ErrorFile.write(err.toUtf8());
//                _ErrorFile.close();
//                //log error for
//            }
//        }
//        else
//        {
//            _ParameterMap.insert(param,PARAM_NOT_FOUND);
//            _ErrorFile.open(QFile::Append|QIODevice::Text);
//            QString err = QString("%1%2\n").arg(param,PARAM_NOT_FOUND);
//            _ErrorFile.write(err.toUtf8());
//            _ErrorFile.close();
//            //   qDebug() << getParamQ.lastError();
//        }

//    }
//}


////iCallCmdRelPath - This parameter will contain the name of the folder,
////in which the procedures and functions will be saved after import
//GTACommandBase* GTAFSIIController::createCallCommand(GTAFSIIFunctionCall* pFunctionCall, const QString &iCallCmdRelPath)
//{
//    if(pFunctionCall)
//    {
//        GTACommandBase* pCmd;
//        GTAActionCall *pCall = new GTAActionCall;
//        pCall->setAction(ACT_CALL);
//        pCall->setComplement(COM_CALL_FUNC);

//        QString stopOnFail = pFunctionCall->isStopOnFail();
//        pCall->setActionOnFail(stopOnFail);
//        bool isCmdIgnored = pFunctionCall->isCommandDeactivated();
//        pCall->setIgnoreInSCXML(isCmdIgnored);
//        QString parameter = pFunctionCall->getFunctionName();
//        QString ext = QString("fun");
//        QString callName;
//        if(iCallCmdRelPath.isEmpty())
//            callName = QString("%1.%2").arg(parameter,ext);
//        else
//            callName = QDir::cleanPath(QString("%1%2%1%3.%4").arg(QDir::separator(),iCallCmdRelPath,parameter,ext));
//        pCall->setElement(callName);
//        pCmd = pCall;
//        return pCmd;
//    }
//}

//void GTAFSIIController::createProcedure(GTAFSIIFile &iFS2Data)
//{
//    QString externalXmlPath = iFS2Data.getExternalXmlFilePath();
//    QString XmlFileName;
//    QString FSIIFolder;
//    QFileInfo file(externalXmlPath);
//    if(file.exists())
//    {
//        XmlFileName = file.fileName();
//        XmlFileName.remove(".xml");
//        FSIIFolder = QString("FSII-%1").arg(XmlFileName);
//    }
//    QString name = iFS2Data.getName();
//    QString desc = iFS2Data.getDescription();
//    GTAFSIITestProcedure testProc = iFS2Data.getTestProcedure();
//    QStringList ICD_Files = iFS2Data.getICDDatabaseList(); // where to store ICD params???
//    GTASystemServices *pSysServices = GTASystemServices::getSystemServices();
//    QStringList ICD_DB = pSysServices->getICDFiles();
//    for(int i = 0; i < ICD_Files.count(); i++)
//    {
//        QString icd_file = ICD_Files.at(i);
//        if(!ICD_DB.contains(icd_file))
//        {
//            //log an error
//            _ErrorFile.open(QFile::Append|QIODevice::Text);
//            QString error = QString("ICD files not found in the database\n");
//            _ErrorFile.write(error.toUtf8());
//            _ErrorFile.close();
//        }

//    }

//    QList<GTAFSIIFunctionCall> funcCallList = testProc.getTestProcedureFunctionCalls();

//    QList <GTACommandBase *> cmdList;
//    for(int i = 0; i < funcCallList.count(); i++)
//    {
//        GTAFSIIFunctionCall funcCallObj = funcCallList.at(i);
//        GTACommandBase *pCmd = createCallCommand(&funcCallObj,FSIIFolder);
//        cmdList.append(pCmd);

//    }

//    GTAElement* TestProcElem = new GTAElement(GTAElement::PROCEDURE,cmdList);
//    TestProcElem->setName(name);
//    TestProcElem->setFSIIRefFile(XmlFileName);
//    GTAHeader* header = new GTAHeader;
//    header->setName(name) ;
//    header->setDescription(desc);
//    header->insertDefaultHeaderNodes();
//    header->editField("DESCRIPTION",desc);
//    TestProcElem->setHeader(header);
//    _TestProcElemMap.insert(XmlFileName,TestProcElem);
//    _ExternalXmlPathMap.insert(XmlFileName,externalXmlPath);

//}

//GTACommandBase* GTAFSIIController::createSetCommand(GTAFSIISet *pFS2Set)
//{
//    GTAActionSetList *pSetList;
//    // bool isCmdValid = true;
//    if(pFS2Set)
//    {
//        pSetList = new GTAActionSetList(ACT_SET,QString(""));
//        pSetList->setTimeOut("3",ACT_TIMEOUT_UNIT_SEC);


//        GTAActionSet* pSet;
//        pSet= new GTAActionSet(ACT_SET_INSTANCE,QString(""));
//        QString paramName = pFS2Set->getParamName();
//        QString paramVal = pFS2Set->getParamValue();

//        bool isFSOnly = pFS2Set->isFSOnly();
//        bool isCmdIgnored = pFS2Set->isCommandDeactivated();
//        QString actionOnFail = pFS2Set->isStopOnFail();

//        pSetList->setActionOnFail((actionOnFail == "true")?"stop":"continue");

//        pSet->setActionOnFail((actionOnFail == "true")?"stop":"continue");
//        pSet->setIgnoreInSCXML(isCmdIgnored);
//        pSet->setIsSetOnlyFS(isFSOnly);
//        if(isFSOnly)
//        {


//            pSet->setIsSetOnlyFS(true);
//            pSet->setFunctionStatus(paramVal);

//            //            if(! _ParameterMap.isEmpty())
//            //            {
//            //                QHash<QString,QString>::const_iterator itr = _ParameterMap.find(paramName);
//            //                if(itr != _ParameterMap.end() && itr.key() == paramName)
//            //                {
//            //                    paramName = itr.value();
//            //                }
//            //                QString FSStatus = processFunctionalStatus(paramName,paramVal);
//            //                pSet->setFunctionStatus(FSStatus);
//            //            }
//            //            else
//            //            {

//            //                // log error : parameter not found in database
//            //                _ErrorFile.open(QFile::Append|QIODevice::Text);
//            //                QString error = QString("%1%2\n").arg(paramName,PARAM_NOT_FOUND);
//            //                _ErrorFile.write(error.toUtf8());
//            //                _ErrorFile.close();
//            //            }

//        }
//        else
//        {
//            GTAEquationConst *pEqn = new GTAEquationConst;
//            pEqn->setConstant(paramVal);
//            GTAEquationBase* pBaseEqn = pEqn;
//            pSet->setEquation(pBaseEqn);
//            pSet->setFunctionStatus(QString("NO_CHANGE"));
//        }

//        pSet->setParameter(paramName);
//        pSetList->addSetAction(pSet);

//        return pSetList;


//    }

//}

//QString GTAFSIIController::processFunctionalStatus(const QString &iParamName, const QString &iParamVal)
//{

//    QHash<QString,GTAFSIISignal>::const_iterator itr = _FSParamAndSignalType.find(iParamName);
//    if(itr != _FSParamAndSignalType.end() && itr.key() == iParamName)
//    {
//        GTAFSIISignal objSignal = itr.value();
//        QString media = objSignal.getMedia();

//        if(media == "AFDX")
//        {
//            if(iParamVal == "0x00")
//            {
//                return QString("ND");
//            }
//            else if(iParamVal == "0x01")
//            {

//                return QString("NCD");
//            }
//            else if(iParamVal == "0x02")
//            {
//                return QString("FT");
//            }
//            else if(iParamVal == "0x03")
//            {
//                return QString("NO");
//            }

//        }
//        else if(media == "A429")
//        {
//            return QString("NO");           //Need to work on logic for ARINC
//        }

//    }
//    else
//        return QString("NO");
//}

//GTACommandBase* GTAFSIIController::createWaitForCommand(GTAFSIIWait *pFS2Wait)
//{
//    if(pFS2Wait)
//    {
//        GTAActionWait *pWaitFor = new GTAActionWait(ACT_WAIT,COM_WAIT_FOR,GTAActionWait::FOR);
//        QString unit = pFS2Wait->getUnit();
//        QString counterVal = pFS2Wait->getCounterValue();
//        QString actionOnFail = pFS2Wait->isStopOnFail();
//        bool isIgnored = pFS2Wait->isCommandDeactivated();

//        pWaitFor->setIgnoreInSCXML(isIgnored);
//        pWaitFor->setCounter(counterVal);
//        return pWaitFor;
//    }
//}

//GTACommandBase* GTAFSIIController::createWaitUntilCommand(GTAFSIIWaitWhile *pFS2WaitWhile)
//{
//    if(pFS2WaitWhile)
//    {
//        GTAActionWait *pWaitUntil = new GTAActionWait(ACT_WAIT,COM_WAIT_UNTIL,GTAActionWait::UNTIL);
//        QString actionOnFail = pFS2WaitWhile->isStopOnFail();
//        bool isCmdIgnored = pFS2WaitWhile->isCommandDeactivated();

//        QString timeout = pFS2WaitWhile->getTimeOut();
//        QString unit = pFS2WaitWhile->getUnit();
//        QString errorOnTimeOut = pFS2WaitWhile->getErrorOnTimeOut(); // What to do for this errorOnTimeout
//        GTAFSIICondition condition = pFS2WaitWhile->getCondition();
//        QString parameter = condition.getLHS();
//        QString value = condition.getRHS();
//        QString condOp = condition.getOperator();

//        pWaitUntil->setActionOnFail(actionOnFail);
//        pWaitUntil->setIgnoreInSCXML(isCmdIgnored);
//        pWaitUntil->setTimeOut(timeout,unit);
//        pWaitUntil->setParameter(parameter);
//        pWaitUntil->setValue(value);
//        pWaitUntil->setCondition(condOp);
//        return pWaitUntil;
//    }
//}

//GTACommandBase* GTAFSIIController::createPrintMessageCommand(GTAFSIILogEntry* pFS2LogEntry)
//{
//    if(pFS2LogEntry)
//    {
//        GTAActionPrint *pPrintMessage = new GTAActionPrint(ACT_PRINT,COM_PRINT_MSG);
//        QString actionOnFail = pFS2LogEntry->isStopOnFail();
//        bool isCmdIgnored = pFS2LogEntry->isCommandDeactivated();
//        QString message = pFS2LogEntry->getMessage();

//        pPrintMessage->setIgnoreInSCXML(isCmdIgnored);
//        pPrintMessage->setMessageType(message);
//        QStringList msg;
//        msg.append(message);
//        pPrintMessage->setValues(msg);
//        pPrintMessage->setPrintType(GTAActionPrint::MESSAGE);
//        return pPrintMessage;
//    }
//}

//GTACommandBase* GTAFSIIController::createConditionWhileCommand(GTAFSIIRunXTimes* pRunXTimes, const QString iCallCmdRelPath)
//{
//    if(pRunXTimes)
//    {

//        GTAActionWhile *pWhile = new GTAActionWhile(ACT_CONDITION,COM_CONDITION_WHILE);

//        QString actionOnFail = pRunXTimes->isStopOnFail();
//        bool isCmdIgnored = pRunXTimes->isCommandDeactivated();
//        pWhile->setIgnoreInSCXML(isCmdIgnored);
//        pWhile->setActionOnFail(actionOnFail);

//        QString paramerter = QString("CounterValue");
//        int numOfIterations = pRunXTimes->getNumberOfIterations();
//        pWhile->setParameter(paramerter);
//        pWhile->setValue(QString::number(numOfIterations));
//        pWhile->setCondition(QString("!=")); //ask ikshvaku

//        QList<GTAFSIICommandBase*> lstFuncBlock = pRunXTimes->getFunctionalBlockCommand();
//        QList<GTACommandBase*> lstChildren;
//        convertToGTACommands(lstFuncBlock,lstChildren,iCallCmdRelPath);
//        for(int i = 0; i < lstChildren.count();i++)
//        {
//            GTACommandBase *pCmd = lstChildren.at(i);
//            pWhile->insertChildren(pCmd,i);
//        }
//        return pWhile;
//    }

//}

//GTACommandBase* GTAFSIIController::createConditionWhileCommand(GTAFSIILoop* pLoop,const QString iCallCmdRelPath)
//{
//    if(pLoop)
//    {
//        GTAActionWhile *pWhile = new GTAActionWhile(ACT_CONDITION,COM_CONDITION_WHILE);

//        QString actionOnFail = pLoop->isStopOnFail();
//        bool isCmdIgnored = pLoop->isCommandDeactivated();
//        pWhile->setIgnoreInSCXML(isCmdIgnored);
//        pWhile->setActionOnFail(actionOnFail);

//        GTAFSIICondition condition = pLoop->getCondiiton();
//        QString parameter = condition.getLHS();
//        QString value = condition.getRHS();
//        QString condOp = condition.getOperator();
//        pWhile->setParameter(parameter);
//        pWhile->setValue(value);
//        pWhile->setCondition(condOp);


//        QList<GTAFSIICommandBase*> lstFuncBlock = pLoop->getFunctionBlockCommands();
//        QList<GTACommandBase*> lstChildren;
//        convertToGTACommands(lstFuncBlock,lstChildren,iCallCmdRelPath);
//        for(int i = 0; i < lstChildren.count();i++)
//        {
//            GTACommandBase *pCmd = lstChildren.at(i);
//            pWhile->insertChildren(pCmd,i);
//        }

//        return pWhile;
//    }
//}

//GTACommandBase* GTAFSIIController::createManualActionCommand(GTAFSIIPause* pPause)
//{
//    if(pPause)
//    {
//        GTAActionManual *pManual = new GTAActionManual(ACT_MANUAL);

//        QString actionOnFail = pPause->isStopOnFail();
//        bool isCmdIgnored = pPause->isCommandDeactivated();

//        pManual->setIgnoreInSCXML(isCmdIgnored);
//        pManual->setActionOnFail(actionOnFail);

//        QString message = pPause->getMessage();
//        pManual->setMessage(message);
//        pManual->setAcknowledgement(true); //ask .. pause does not have any param. hence here true is passed
//        return pManual;
//    }
//}

//GTACommandBase* GTAFSIIController::createIfCommand(GTAFSIIIf* pFS2If)
//{
//    if(pFS2If)
//    {
//        GTAActionIF *pIF = new GTAActionIF(ACT_CONDITION,COM_CONDITION_IF);

//        QString actionOnFail = pFS2If->isStopOnFail();
//        bool isCmdIgnored = pFS2If->isCommandDeactivated();
//        pIF->setActionOnFail(actionOnFail);
//        pIF->setIgnoreInSCXML(isCmdIgnored);

//        //ask how to create since FSII If has multiple conditions and GTA if has only one
//        //proposed solution
//        return pIF;
//    }
//}

//GTACommandBase* GTAFSIIController::createCheckValueCommand(GTAFSIICheck* pFS2Check)
//{
//    if(pFS2Check)
//    {
//        GTACheckValue *pCheck = new GTACheckValue();
//        QString actionOnFail = pFS2Check->isStopOnFail();
//        bool isCmdIgnored = pFS2Check->isCommandDeactivated();

//        pCheck->setIgnoreInSCXML(isCmdIgnored);
//        pCheck->setActionOnFail((actionOnFail == "true")?"stop":"continue");

//        QList<GTAFSIICondition> conditionList = pFS2Check->getConditionList();
//        QString logicalConnector = pFS2Check->getLogicalConnector();

//        pCheck->setConfCheckTime("0",ACT_TIMEOUT_UNIT_SEC);
//        pCheck->setTimeOut("3",ACT_TIMEOUT_UNIT_SEC);
//        pCheck->setWaitUntil(true);

//        for(int i = 0; i < conditionList.count(); i++)
//        {
//            GTAFSIICondition condObj = conditionList.at(i);
//            QString parameter = condObj.getLHS();
//            QString value = condObj.getRHS();
//            QString condOp = condObj.getOperator();
//            bool isFs = condObj.getIsSetOnlyFS();

//            double precision = 0.0;
//            QString precisionType = QString("value");
//            QString functionalStatus;
//            //            if(isFs && !_ParameterMap.isEmpty())
//            //            {
//            //                QHash<QString,QString>::const_iterator itr = _ParameterMap.find(parameter);
//            //                if(itr != _ParameterMap.end() && itr.key() == parameter)
//            //                    parameter = itr.value();
//            //                functionalStatus = processFunctionalStatus(parameter,value);

//            //            }
//            //            if(functionalStatus.isEmpty())
//            //                functionalStatus = QString("NO");


//            if(isFs)
//            {
//                pCheck->insertParamenter(parameter,condOp,"",value,precision,precisionType,!isFs,isFs);
//            }
//            else
//                pCheck->insertParamenter(parameter,condOp,value,"NO",precision,precisionType,!isFs,isFs);
//            pCheck->insertBinaryOperator(logicalConnector);
//            pCheck->setTimeOut("3","sec");
//            return pCheck;
//        }
//    }
//}

//void GTAFSIIController::convertToGTACommands(QList<GTAFSIICommandBase*> &iFS2CmdList,QList<GTACommandBase*> &oCmdList,const QString iCallCmdRelPath)
//{

//    GTACommandBase* pCmd = NULL;
//    for(int i = 0; i < iFS2CmdList.count(); i++)
//    {
//        GTAFSIICommandBase* pFS2Cmd = iFS2CmdList.at(i);
//        if(pFS2Cmd)
//        {
//            QString commandName = pFS2Cmd->getCommandName();
//            bool isCommandValid = pFS2Cmd->IsCommandValid();
//            if(isCommandValid)
//            {
//                if(commandName == CMD_TYPE_SET)
//                {
//                    GTAFSIISet *pFS2Set = dynamic_cast<GTAFSIISet*>(pFS2Cmd);
//                    pCmd = createSetCommand(pFS2Set);
//                }
//                else if (commandName == CMD_TYPE_WAIT)
//                {
//                    GTAFSIIWait *pFS2Wait = dynamic_cast<GTAFSIIWait*>(pFS2Cmd);
//                    pCmd = createWaitForCommand(pFS2Wait);
//                }
//                else if(commandName == CMD_TYPE_LOG_ENTRY)
//                {
//                    GTAFSIILogEntry *pFS2LogEntry = dynamic_cast<GTAFSIILogEntry*>(pFS2Cmd);
//                    pCmd = createPrintMessageCommand(pFS2LogEntry);
//                }
//                else if(commandName == CMD_TYPE_RUN_X_TIMES)
//                {
//                    GTAFSIIRunXTimes* pRunXTimes = dynamic_cast<GTAFSIIRunXTimes*>(pFS2Cmd);
//                    pCmd = createConditionWhileCommand(pRunXTimes,iCallCmdRelPath);
//                }
//                else if(commandName == CMD_TYPE_CHECK)
//                {
//                    GTAFSIICheck *pCheck = dynamic_cast<GTAFSIICheck*>(pFS2Cmd);
//                    pCmd = createCheckValueCommand(pCheck);
//                }
//                else if(commandName == CMD_TYPE_WAIT_WHILE)
//                {
//                    GTAFSIIWaitWhile *pFS2WaitWhile = dynamic_cast<GTAFSIIWaitWhile*>(pFS2Cmd);
//                    pCmd = createWaitUntilCommand(pFS2WaitWhile);
//                }
//                else if (commandName == CMD_TYPE_IF)
//                {
//                    //            GTAFSIIIf *pIf = dynamic_cast<GTAFSIIIf*> (pFS2Cmd);
//                    //            pCmd = createIfCommand(pIf);
//                    //log a warning
//                    _ErrorFile.open(QFile::Append|QIODevice::Text);
//                    QString error = QString("Conversion for If_else command not yet implemented. Ignoring If command\n");
//                    _ErrorFile.write(error.toUtf8());
//                    _ErrorFile.close();

//                }
//                else if (commandName == CMD_TYPE_DECREASE || commandName == CMD_TYPE_INCREASE)
//                {
//                    GTAFSIIIncreaseDecrease *pCommand = dynamic_cast<GTAFSIIIncreaseDecrease*>(pFS2Cmd);
//                    pCmd = createSetCommand(pCommand);
//                }
//                else if (commandName == CMD_TYPE_PAUSE)
//                {
//                    GTAFSIIPause* pPause = dynamic_cast<GTAFSIIPause*>(pFS2Cmd);
//                    pCmd = createManualActionCommand(pPause);
//                }
//                else if (commandName == CMD_TYPE_FUNC_CALL)
//                {
//                    GTAFSIIFunctionCall* pFuncCall = dynamic_cast<GTAFSIIFunctionCall*>(pFS2Cmd);
//                    pCmd = createCallCommand(pFuncCall,iCallCmdRelPath);
//                }
//                else if (commandName == CMD_TYPE_LOOP)
//                {
//                    GTAFSIILoop* pLoop = dynamic_cast<GTAFSIILoop*>(pFS2Cmd);
//                    pCmd = createConditionWhileCommand(pLoop,iCallCmdRelPath);
//                }
//            }
//            else
//            {
//                GTAInvalidCommand *pInvalidCmd = new GTAInvalidCommand;
//                pCmd = pInvalidCmd;
//            }
//            if(pCmd)
//            {
//                oCmdList.append(pCmd);
//            }
//        }
//    }
//}

//void GTAFSIIController::createFunction(GTAFSIIFile &iFS2Data)
//{
//    QString fileName;
//    QString FSIIFolder;
//    QString filePath = iFS2Data.getExternalXmlFilePath();
//    QFileInfo file(filePath);
//    if(file.exists())
//    {
//        fileName = file.fileName();
//        fileName.remove(".xml");
//        FSIIFolder = QString("FSII-%1").arg(fileName);
//    }

//    QList<GTAFSIIFunction> funcList = iFS2Data.getFunctionList();
//    for(int i = 0; i < funcList.count(); i++)
//    {
//        GTAElement *functionElem;
//        GTAHeader *header;
//        GTAFSIIFunction funcObj = funcList.at(i);
//        QString name = funcObj.getName();
//        QString desc = funcObj.getDescription();
//        QList<GTAFSIICommandBase*> fs2CmdList = funcObj.getCommands();
//        QList<GTACommandBase*> cmdList;
//        if(fs2CmdList.isEmpty())
//        {
//            GTACommandBase *pCmdBase = NULL;
//            GTAActionPrint *pPrintMessage = new GTAActionPrint(ACT_PRINT,COM_PRINT_MSG);
//            QString actionOnFail("continue");
//            bool isCmdIgnored = false;
//            QString message = QString("Print <%1>").arg(desc);

//            pPrintMessage->setActionOnFail(actionOnFail);
//            pPrintMessage->setIgnoreInSCXML(isCmdIgnored);
//            pPrintMessage->setMessageType(message);
//            QStringList msg;
//            msg.append(message);
//            pPrintMessage->setValues(msg);
//            pPrintMessage->setPrintType(GTAActionPrint::MESSAGE);
//            pCmdBase = pPrintMessage;
//            cmdList.append(pCmdBase);
//        }
//        else
//        {
//            convertToGTACommands(fs2CmdList,cmdList,FSIIFolder);
//        }


//        functionElem = new GTAElement(GTAElement::FUNCTION,cmdList);

//        functionElem->setName(name);
//        header = new GTAHeader;
//        header->setName(name);
//        header->setDescription(desc);
//        header->insertDefaultHeaderNodes();
//        header->editField("DESCRIPTION",desc);
//        functionElem->setHeader(header);
//        _FunctionElemMap.insertMulti(fileName,functionElem);
//    }
//}

//void GTAFSIIController::convertToGTA(QList<GTAFSIIFile> fs2DataList)
//{
//    for(int i = 0; i < fs2DataList.count(); i++)
//    {
//        GTAFSIIFile fs2Data = fs2DataList.at(i);
//        createFunction(fs2Data);
//        createProcedure(fs2Data);
//    }
//}

//QHash<QString,QString> GTAFSIIController::getExternalXmlPathMap()const
//{
//    return _ExternalXmlPathMap;
//}

//QHash<QString, GTAElement*> GTAFSIIController::getTestProcElementMap()
//{
//    return _TestProcElemMap;
//}

//QHash<QString, GTAElement*> GTAFSIIController::getFunctionElementMap()
//{
//    return _FunctionElemMap;
//}

