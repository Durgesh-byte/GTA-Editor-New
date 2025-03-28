#include "AINGTAFSIIParser.h"
#include <QString>
#include <QDir>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNodeList>
#include "AINGTAFSIIFunctionCall.h"
#include "AINGTAFSIIUtil.h"
#include "AINGTAFSIICommandBase.h"
#include "AINGTAFSIICommandList.h"
#include "AINGTALocalDbUtil.h"
#include "AINGTAUtil.h"
#include "AINGTAICDParameter.h"

int AINGTAFSIIParser::_Counter = 0;

AINGTAFSIIParser::AINGTAFSIIParser()
{

}
/*  Parametrised constructor for AINGTAFSIIParser
 *  input :-
 *  iFSIIScriptFile : flat script folder complete path
 */
//AINGTAFSIIParser::AINGTAFSIIParser(const QString &iFSIIScriptPath, const QString &iErrorPath)
//{

//}
bool AINGTAFSIIParser::setLogPath(const QString &iErrorPath)
{
    bool frc = false;
    _LogFile.setFileName(iErrorPath);
    if(!_LogFile.exists())
        frc = _LogFile.open(QFile::WriteOnly | QIODevice::Text);
    else
        frc = _LogFile.open(QFile::Append |QIODevice::Text);
    return frc;
}

bool AINGTAFSIIParser::parseFiles(const QString &iFSIIScriptPath, const QString &iErrorPath)
{
    bool frc = false;
    int validFileCount = 0;
    frc = setLogPath(iErrorPath);
    if(frc)
    {
        QDir dir(iFSIIScriptPath);
        if(dir.exists())
        {
            _FileList = dir.entryInfoList(QStringList("*.xml"),QDir::Files|QDir::NoSymLinks);

            if(!_FileList.isEmpty())
            {

                for(int i = 0; i < _FileList.count(); i++)
                {
                    bool retVal = parseFile(_FileList.at(i));
                    if(retVal)
                        validFileCount++;
                }
                if(validFileCount > 0)
                {
                    frc = true;
                }
                else
                {
                    frc = false;
                    QString error = QString("No Flat Script files in the directory\n");
                    _LogFile.write(error.toUtf8());
                }
            }
            else
            {
                //error no files in directory
                frc = false;
                QString error = QString("No Flat Script files in the directory\n");
                _LogFile.write(error.toUtf8());
            }
        }
        else
        {
            //log for invalid directory
            frc = false;
            QString error = QString("Invalid directory\n");
            _LogFile.write(error.toUtf8());

        }
    }
    _LogFile.close();
    return frc;
}
bool AINGTAFSIIParser::parseFile(const QFileInfo &iFSIIFileInfo)
{
    QString fileName = iFSIIFileInfo.fileName();
    bool retVal = parseFlatScriptFile(iFSIIFileInfo);
    if(retVal)
    {
        QString error = QString("%1 : parsed successfully\n").arg(fileName);
        _LogFile.write(error.toUtf8());
    }
    else
    {
        QString error = QString("%1 : error in parsing\n").arg(fileName);
        _LogFile.write(error.toUtf8());
    }
    return retVal;
}

bool AINGTAFSIIParser::parseFlatScriptFile(const QFileInfo &iFile)
{
    _Counter = 0;
    QString configFileName = iFile.absoluteFilePath();

    QFile xmlFile(configFileName);
    bool rc = xmlFile.open(QFile::ReadOnly | QFile::Text);
    QDomDocument * pDomDoc = NULL;
    if (rc)
    {
        pDomDoc = new QDomDocument();
        if(pDomDoc != NULL)
        {
            if(pDomDoc->setContent(&xmlFile))
            {

                QDomElement rootElem = pDomDoc->documentElement();

                QString name = rootElem.nodeName();
                if(name != "fs:flatscriptII")
                    return false;

                if(rootElem.hasChildNodes())
                {
                    _CurrentObjFile.clear();
                    QStringList ICDDatabase;
                    AINGTAFSIITestProcedure testProc;
                    _CurrentObjFile.setExternalXmlFilePath(configFileName);

                    QDomNodeList lstAllNodes = rootElem.childNodes();
                    for(int i = 0;i < lstAllNodes.count(); i++)
                    {
                        QDomNode fs2Node = lstAllNodes.at(i);
                        if(fs2Node.nodeName() == "fs:sources")
                        {
                            QDomNodeList lstSources = fs2Node.childNodes();
                            for(int j = 0; j < lstSources.count(); j++)
                            {
                                QDomNode nodeSource = lstSources.at(j);
                                if(nodeSource.nodeName() == "fs:icd")
                                {
                                    QString parameterDatabase = nodeSource.attributes().namedItem("name").nodeValue();
                                    ICDDatabase.append(parameterDatabase);
                                }
                            }
                            _CurrentObjFile.setICDDatabaseList(ICDDatabase);
                        }
                        else if(fs2Node.nodeName() == "fs:test_procedure")
                        {
                            QString name = fs2Node.attributes().namedItem("name").nodeValue();
                            QList<AINGTAFSIIFunctionCall> lstTestProcCalls;
                            createTestProcedure(fs2Node,lstTestProcCalls,_CurrentObjFile);
                            testProc.setTestProcedureFunctionCalls(lstTestProcCalls);
                            QString shortName = QString("Main");
                            _CurrentObjFile.setName(shortName);
                            _CurrentObjFile.setDescription(name);
                            _CurrentObjFile.setTestProcedure(testProc);
                        }
                        else if(fs2Node.nodeName() == "fs:function")
                        {
                            QString name = fs2Node.attributes().namedItem("name").nodeValue();
                            AINGTAFSIIFunction objFunction;
                            QString shortName;
                            if(_CurrentObjFile.isPresentInFuncNameMap(name))
                            {
                                shortName = _CurrentObjFile.getValueFromFuncNameMap(name);
                            }
                            else
                            {
                                shortName = getShortName(name);
                                _CurrentObjFile.insertIntoFuncNameMap(name,shortName);
                            }

                            objFunction.setName(shortName);
                            objFunction.setDescription(name);
                            createFunction(fs2Node,objFunction);
                            _CurrentObjFile.addFunction(objFunction);
                        }
                    }
                    _FS2Data.append(_CurrentObjFile);
                }
            }
            else
            {
                delete pDomDoc;
                pDomDoc = NULL;

            }
        }
    }
}

QString AINGTAFSIIParser::getShortName(const QString &iLongName)
{
    _Counter++;
    //    QString longName = iLongName;
    //    QStringList splitName = longName.split(" ");
    //    QString shortName;
    //    for(int i = 0; i < splitName.count(); i++)
    //    {
    //        QString t = splitName.at(i);
    //        if(!t.isEmpty())
    //        {
    //            if(t.size() > 3)
    //            {
    //                t.truncate(3);
    //                if(i != 0)
    //                    t.prepend("_");
    //                shortName.append(t);
    //            }
    //        }
    //    }

    QString shortName = QString("Function_%1").arg(_Counter);

    return shortName;
}

void AINGTAFSIIParser::createTestProcedure(QDomNode &funcNode, QList<AINGTAFSIIFunctionCall> &oFunctionCalls, AINGTAFSIIFile &iObjFile)
{

    QDomNodeList lstFunctionCall = funcNode.childNodes();

    for(int j = 0; j < lstFunctionCall.count(); j++)
    {
        QDomNode functionCallNode = lstFunctionCall.at(j);
        if(functionCallNode.nodeName() == "fs:function_call")
        {
            QString stopOnFail = functionCallNode.attributes().namedItem("stop_on_fail").nodeValue();
            QString functionName = functionCallNode.attributes().namedItem("function").nodeValue();
            QString log = functionCallNode.attributes().namedItem("log").nodeValue();
            QString modName;
            if(iObjFile.isPresentInFuncNameMap(functionName))
            {
                modName = iObjFile.getValueFromFuncNameMap(functionName);
            }
            else
            {
                modName = getShortName(functionName);
                iObjFile.insertIntoFuncNameMap(functionName,modName);
            }

            AINGTAFSIIFunctionCall objFunctionCall;
            objFunctionCall.setFunctionName(modName);
            objFunctionCall.setStopOnFail(stopOnFail);
            objFunctionCall.setLog(log);
            oFunctionCalls.append(objFunctionCall);

        }
    }
}




void AINGTAFSIIParser::createFunction(QDomNode &funcNode,AINGTAFSIIFunction &oFunction)
{

    if(funcNode.hasChildNodes())
    {
        QDomNodeList lstFuncNode = funcNode.childNodes();
        for(int i = 0; i < lstFuncNode.count(); i++)
        {
            QDomNode node = lstFuncNode.at(i);

            if(node.nodeName() == "fs:definitions")
            {
                if(node.hasChildNodes())
                {
                    QList<AINGTAFSIISignal> lstSignal;
                    QDomNodeList definitionNodeList = node.childNodes();
                    for(int j = 0; j < definitionNodeList.count(); j++)
                    {
                        QDomNode signalNode = definitionNodeList.at(j);
                        if(signalNode.nodeName() == "fs:signal")
                        {
                            AINGTAFSIISignal objSignal;
                            QString name = signalNode.attributes().namedItem("name").nodeValue();

                            QString record = signalNode.attributes().namedItem("record").nodeValue();
                            QString application = signalNode.attributes().namedItem("application").nodeValue();

                            objSignal.setRecord(record);
                            objSignal.setApplication(application);

                            processSignal(name,objSignal);
                            objSignal.setName(name);


                            lstSignal.append(objSignal);
                        }
                    }
                    oFunction.setSignals(lstSignal);

                }
            }
            if(node.nodeName() == FSII_FUNCTION_BLOCK)
            {
                if(node.hasChildNodes())
                {
                    QDomNodeList cmdNodeList = node.childNodes();
                    for(int j = 0; j < cmdNodeList.count(); j++)
                    {
                        QDomNode cmdNode = cmdNodeList.at(j);
                        AINGTAFSIICommandBase *pCmd = createCommand(cmdNode);
                        if(pCmd != NULL)
                            oFunction.addCommand(pCmd);
                    }

                }
            }
        }
    }
}


void AINGTAFSIIParser::processSignal(QString &ioName, AINGTAFSIISignal &oSignal)
{

    bool IsFS = false;
    if(ioName.endsWith(".FS"))
    {
        //ioName.remove(".FS");
        oSignal.setIsSignalFS(true);
        IsFS = true;
    }

    if(ioName.endsWith(".SSM"))
    {
        // ioName.remove(".SSM");
        oSignal.setIsSignalFS(true);
        IsFS = true;
    }

    QString signalName;
    QString media;
    QString direction;
    QString appName = oSignal.getApplication();
    QString busName;
    QString msgName;
    QString equipment;
    QString fsName;
    QString gtaName;

    if(appName.contains(","))
    {
        QStringList temp1 = appName.split(",",QString::SkipEmptyParts);
        appName = temp1.first();
    }
    QStringList paramName = ioName.split(".",QString::SkipEmptyParts);
    if(paramName.count() >= 3 && paramName.count() <= 6)
    {

        equipment = paramName.at(0);
        QStringList mediaDirInfo = QString(paramName.at(1)).split("_");
        media = mediaDirInfo.at(0);
        direction = mediaDirInfo.at(1);
        QString whereClause;
        if(media.toLower().contains("discrete") || media.toLower().contains("analog"))
        {
            QString firstThreeChar = media;
            firstThreeChar.remove(3,firstThreeChar.count());
            signalName = paramName.at(2);
            whereClause = "MEDIA LIKE '%"+media+"%' AND EQUIPNAME LIKE '%"+equipment+"%' AND APPLICATION LIKE '%"+appName+"%' AND SIGNAL_NAME LIKE '%"+signalName+"%'";
        }
        else
        {
            if(ioName.endsWith(".SSM") || ioName.endsWith(".FS"))
            {
                busName = paramName.at(2);
                msgName = paramName.at(3);
                fsName = paramName.at(4);
                whereClause = "MEDIA LIKE '%"+media+"%' AND EQUIPNAME LIKE '%"+equipment+"%' AND APPLICATION LIKE '%"+appName+"%' AND FS_NAME LIKE '%"+fsName+"%' AND MESSAGE LIKE '%"+msgName+"%' AND BUSNAME LIKE '%"+busName+"%' ";
            }
            else
            {
                busName = paramName.at(2);
                signalName = paramName.at(3);
                whereClause = "MEDIA LIKE '%"+media+"%' AND EQUIPNAME LIKE '%"+equipment+"%' AND APPLICATION LIKE '%"+appName+"%' AND SIGNAL_NAME LIKE '%"+signalName+"%' AND BUSNAME LIKE '%"+busName+"%' ";
            }
        }

        QString Query1 = QString("SELECT * FROM %1 WHERE %2").arg(PARAMETER_TABLE,whereClause);
        QList<AINGTAICDParameter> params;
        AINGTALocalDbUtil::fillQuery(Query1,params);
        AINGTAICDParameter param;
        QString mediaType;
        if(!params.isEmpty())
        {
            param = params.at(0);
            gtaName = param.getName();
            mediaType = param.getMediaType();
        }


        oSignal.setBusName(busName);
        oSignal.setDirection(direction);
        oSignal.setEquipment(equipment);
        oSignal.setFSName(fsName);
        oSignal.setMedia(media);
        oSignal.setMessage(msgName);
        oSignal.setGTAName(gtaName);
        oSignal.setMediaType(mediaType);

        if(oSignal.IsSignalFS())
        {
            if(!_FSParameters.contains(gtaName))
            {
                _FSParameters.insert(gtaName,oSignal);
            }
        }
        if(! _CRDCToGTAParamMap.contains(ioName))
        {
            _CRDCToGTAParamMap.insert(ioName,gtaName);
        }
    }
}

QString AINGTAFSIIParser::getGTAParamName(const QString &iName)
{
    QString GTAName("");

    if(! _CRDCToGTAParamMap.isEmpty())
    {
        QHash<QString,QString>::const_iterator itr = _CRDCToGTAParamMap.find(iName);
        if(itr != _CRDCToGTAParamMap.end() && itr.key() == iName)
        {
            GTAName = itr.value();
        }
    }
    return GTAName;

}

AINGTAFSIICommandBase* AINGTAFSIIParser::createCommand(QDomNode &cmdNode)
{
    AINGTAFSIICommandBase *pCmd = NULL;
    if(cmdNode.nodeName() == FSII_CMD_SET)
    {
        AINGTAFSIISet *pSet = new AINGTAFSIISet(CMD_TYPE_SET);
        QDomNamedNodeMap cmdNodeAttr = cmdNode.attributes();
        QString stopOnFail = cmdNodeAttr.namedItem(FSII_TAG_STOP_ON_FAIL).nodeValue();
        QString log = cmdNodeAttr.namedItem(FSII_TAG_LOG).nodeValue();
        QString deactivate = cmdNodeAttr.namedItem(FSII_TAG_DEACTIVATE).nodeValue();
        pSet->setStopOnFail(stopOnFail);
        pSet->setLog(log);
        pSet->setDeactivate(deactivate);
        QString paramValue;
        bool isFSSet = false;
        QString paramName;
        QString gtaName;
        QDomNodeList setCmdNodeList = cmdNode.childNodes();
        for(int i = 0; i < setCmdNodeList.count(); i++)
        {
            QDomNode setCmdNode = setCmdNodeList.at(i);

            if(setCmdNode.nodeName() == FSII_WRITE)
            {
                QDomNode writeNode = setCmdNode;
                QDomNode paramterNode = writeNode.childNodes().at(0);
                if(paramterNode.nodeName() == FSII_SIGNAL)
                {
                    paramName = paramterNode.attributes().namedItem("name").nodeValue();
                    if((paramName.endsWith(".FS")) || (paramName.endsWith(".SSM")))
                    {
                        isFSSet = true;
                        pSet->setFSOnly(true);
                    }
                    else
                    {
                        isFSSet = false;
                        pSet->setValueOnly(true);
                    }
                    gtaName = getGTAParamName(paramName);
                    if(!gtaName.isEmpty())
                        pSet->setParamName(gtaName);
                    else
                    {
                        //log error
                        _LogFile.open(QFile::Append|QIODevice::Text);
                        QString error = QString("Parameter (%1) not defined in the defintion block\n").arg(paramName);
                        _LogFile.write(error.toUtf8());
                        _LogFile.close();
                        pSet->setIsCommandValid(false);
                    }

                }
            }
            else
            {
                QDomNode readNode = setCmdNode;
                QDomNode parameterValueNode = readNode.childNodes().at(0);
                paramValue = parameterValueNode.attributes().namedItem("value").nodeValue();
            }
        }

        if(isFSSet)
        {
            QString fsVal = getValidityValue(paramName,gtaName,paramValue);
            pSet->setParamValue(fsVal);
        }
        else
        {
            pSet->setParamValue(paramValue);
        }

        pCmd = pSet;

    }
    else if(cmdNode.nodeName() == FSII_CMD_CHECK)
    {
        AINGTAFSIICheck *pCheck = new AINGTAFSIICheck(CMD_TYPE_CHECK);

        QDomNamedNodeMap cmdNodeAttr = cmdNode.attributes();
        QString stopOnFail = cmdNodeAttr.namedItem(FSII_TAG_STOP_ON_FAIL).nodeValue();
        QString log = cmdNodeAttr.namedItem(FSII_TAG_LOG).nodeValue();
        QString deactivate = cmdNodeAttr.namedItem(FSII_TAG_DEACTIVATE).nodeValue();
        pCheck->setStopOnFail(stopOnFail);
        pCheck->setLog(log);
        pCheck->setDeactivate(deactivate);

        QDomNodeList checkNodeList = cmdNode.childNodes();
        for(int i = 0; i < checkNodeList.count(); i++)
        {
            QDomNode checkNode = checkNodeList.at(i);
            if(checkNode.nodeName() == FSII_CONDITIONS)
            {
                QDomNamedNodeMap conditionsNodeAttr = checkNode.attributes();
                QString logicalConnector = conditionsNodeAttr.namedItem(FSII_TAG_LOGICAL_CONNECTOR).nodeValue();
                pCheck->setLogicalConnector(logicalConnector);

                QDomNodeList conditionNodeList = checkNode.childNodes();

                for(int j = 0; j < conditionNodeList.count(); j++)
                {
                    AINGTAFSIICondition objCondition;
                    QDomNode conditionNode = conditionNodeList.at(j);
                    if(conditionNode.nodeName() == FSII_CONDITION)
                    {
                        bool isFSCheck = false;
                        QString gtaName;

                        QDomNamedNodeMap conditionNodeAttr = conditionNode.attributes();
                        QString logicalOperator = conditionNodeAttr.namedItem("operator").nodeValue();
                        objCondition.setOperator(logicalOperator);

                        QDomNodeList conditionParamList = conditionNode.childNodes();
                        QDomNode conditionLeftNode = conditionParamList.at(0);
                        QDomNode parameterNode = conditionLeftNode.childNodes().at(0);
                        QString parameterName = parameterNode.attributes().namedItem("name").nodeValue();
                        objCondition.setLHS(parameterName);
                        if(parameterNode.nodeName() == FSII_SIGNAL)
                        {

                            if((parameterName.endsWith(".FS"))||(parameterName.endsWith(".SSM")))
                            {
                                isFSCheck = true;
                                objCondition.setIsSetOnlyFS(true);
                                objCondition.setIsSetOnlyValue(false);
                            }
                            else
                            {
                                isFSCheck = false;
                                objCondition.setIsSetOnlyFS(false);
                                objCondition.setIsSetOnlyValue(true);
                            }
                            gtaName = getGTAParamName(parameterName);
                            if(!gtaName.isEmpty())
                                objCondition.setLHS(gtaName);
                            else
                            {
                                //log error
                                _LogFile.open(QFile::Append|QIODevice::Text);
                                QString error = QString("Parameter not defined in the defintion block\n");
                                _LogFile.write(error.toUtf8());
                                _LogFile.close();
                                pCheck->setIsCommandValid(false);
                            }
                        }

                        QDomNode conditionRightNode = conditionParamList.at(1); // Check if the order could be changed then this wont work
                        QDomNode paramterValNode = conditionRightNode.childNodes().at(0);
                        QString paramterVal = paramterValNode.attributes().namedItem("value").nodeValue();

                        if(isFSCheck)
                        {
                            QString fsVal = getValidityValue(parameterName,gtaName,paramterVal);
                            objCondition.setRHS(fsVal);
                        }
                        else
                        {
                            objCondition.setRHS(paramterVal);
                        }



                        pCheck->addCondition(objCondition);

                    }
                }
            }
        }
        pCmd = pCheck;
    }
    else if(cmdNode.nodeName() == FSII_CMD_WAIT)
    {
        AINGTAFSIIWait *pWait = new AINGTAFSIIWait(CMD_TYPE_WAIT);
        QDomNamedNodeMap waitNodeAttr = cmdNode.attributes();
        QString stopOnFail = waitNodeAttr.namedItem(FSII_TAG_STOP_ON_FAIL).nodeValue();
        QString log = waitNodeAttr.namedItem(FSII_TAG_LOG).nodeValue();
        QString deactivate = waitNodeAttr.namedItem(FSII_TAG_DEACTIVATE).nodeValue();

        QString value = waitNodeAttr.namedItem("value").nodeValue();
        QString unit = waitNodeAttr.namedItem("unit").nodeValue();

        pWait->setStopOnFail(stopOnFail);
        pWait->setLog(log);
        pWait->setDeactivate(deactivate);

        pWait->setCounterValue(value);
        pWait->setUnit(unit);
        if(unit == "ms")
        {
            bool isConvOk = false;
            unsigned int intWaitVal = value.toUInt(&isConvOk);
            if(isConvOk)
            {
                intWaitVal = intWaitVal/1000;
                value = QString::number(intWaitVal);
            }
            else
            {
                value = "3";
            }
        }
        pCmd = pWait;
    }
    else if(cmdNode.nodeName() == FSII_CMD_LOG_ENTRY)
    {
        AINGTAFSIILogEntry *pLogEntry = new AINGTAFSIILogEntry(CMD_TYPE_LOG_ENTRY);
        QDomNamedNodeMap logEntryNodeAttr = cmdNode.attributes();
        QString stopOnFail = logEntryNodeAttr.namedItem(FSII_TAG_STOP_ON_FAIL).nodeValue();
        QString log = logEntryNodeAttr.namedItem(FSII_TAG_LOG).nodeValue();
        QString deactivate = logEntryNodeAttr.namedItem(FSII_TAG_DEACTIVATE).nodeValue();

        QString text = logEntryNodeAttr.namedItem("text").nodeValue();

        pLogEntry->setStopOnFail(stopOnFail);
        pLogEntry->setLog(log);
        pLogEntry->setDeactivate(deactivate);
        pLogEntry->setMessage(text);

        pCmd = pLogEntry;
    }
    else if(cmdNode.nodeName() == FSII_CMD_FUNC_CALL)
    {
        AINGTAFSIIFunctionCall *pCall = new AINGTAFSIIFunctionCall(CMD_TYPE_FUNC_CALL);

        QDomNamedNodeMap funcCallNodeAttr = cmdNode.attributes();
        QString stopOnFail = funcCallNodeAttr.namedItem(FSII_TAG_STOP_ON_FAIL).nodeValue();
        QString log = funcCallNodeAttr.namedItem(FSII_TAG_LOG).nodeValue();
        QString deactivate = funcCallNodeAttr.namedItem(FSII_TAG_DEACTIVATE).nodeValue();
        QString funcName = funcCallNodeAttr.namedItem("function").nodeValue();
        QString shortName = funcName;
        pCall->setStopOnFail(stopOnFail);
        pCall->setLog(log);
        pCall->setDeactivate(deactivate);

        if(_CurrentObjFile.isPresentInFuncNameMap(funcName))
        {
            shortName = _CurrentObjFile.getValueFromFuncNameMap(funcName);
        }
        else
        {
            shortName = getShortName(funcName);
            _CurrentObjFile.insertIntoFuncNameMap(funcName,shortName);
        }

        pCall->setFunctionName(shortName);

        pCmd = pCall;

    }
    else if(cmdNode.nodeName() == FSII_CMD_INCREASE)
    {
        AINGTAFSIIIncreaseDecrease *pIncrease = new AINGTAFSIIIncreaseDecrease(CMD_TYPE_INCREASE);

        QDomNamedNodeMap increaseNodeAttr = cmdNode.attributes();
        QString stopOnFail = increaseNodeAttr.namedItem(FSII_TAG_STOP_ON_FAIL).nodeValue();
        QString log = increaseNodeAttr.namedItem(FSII_TAG_LOG).nodeValue();
        QString deactivate = increaseNodeAttr.namedItem(FSII_TAG_DEACTIVATE).nodeValue();

        pIncrease->setStopOnFail(stopOnFail);
        pIncrease->setLog(log);
        pIncrease->setDeactivate(deactivate);

        QDomNode writeNode = cmdNode.childNodes().at(0);
        QDomNode paramterNode = writeNode.childNodes().at(0);
        QString paramName = paramterNode.namedItem("name").nodeValue();
        pIncrease->setParamName(paramName);


        QDomNode byNode = cmdNode.childNodes().at(1);
        QDomNode valueNode = byNode.childNodes().at(0);
        QString paramVal = valueNode.namedItem("value").nodeValue();
        pIncrease->setParamValue(paramVal);

        pCmd = pIncrease;

    }
    else if(cmdNode.nodeName() == FSII_CMD_DECREASE)
    {
        AINGTAFSIIIncreaseDecrease *pDecrease = new AINGTAFSIIIncreaseDecrease(CMD_TYPE_DECREASE);
        QDomNamedNodeMap decreaseNodeAttr = cmdNode.attributes();
        QString stopOnFail = decreaseNodeAttr.namedItem(FSII_TAG_STOP_ON_FAIL).nodeValue();
        QString log = decreaseNodeAttr.namedItem(FSII_TAG_LOG).nodeValue();
        QString deactivate = decreaseNodeAttr.namedItem(FSII_TAG_DEACTIVATE).nodeValue();

        pDecrease->setStopOnFail(stopOnFail);
        pDecrease->setLog(log);
        pDecrease->setDeactivate(deactivate);

        QDomNode writeNode = cmdNode.childNodes().at(0);
        QDomNode paramterNode = writeNode.childNodes().at(0);
        QString paramName = paramterNode.namedItem("name").nodeValue();
        pDecrease->setParamName(paramName);

        QDomNode byNode = cmdNode.childNodes().at(1);
        QDomNode valueNode = byNode.childNodes().at(0);
        QString paramVal = valueNode.namedItem("value").nodeValue();
        pDecrease->setParamValue(paramVal);

        pCmd = pDecrease;
    }
    else if(cmdNode.nodeName() == FSII_CMD_WAIT_WHILE)
    {
        AINGTAFSIIWaitWhile *pWaitWhile = new AINGTAFSIIWaitWhile(CMD_TYPE_WAIT_WHILE);
        QDomNamedNodeMap waitWhileNodeAttr = cmdNode.attributes();
        QString stopOnFail = waitWhileNodeAttr.namedItem(FSII_TAG_STOP_ON_FAIL).nodeValue();
        QString log = waitWhileNodeAttr.namedItem(FSII_TAG_LOG).nodeValue();
        QString deactivate = waitWhileNodeAttr.namedItem(FSII_TAG_DEACTIVATE).nodeValue();
        QString timeout = waitWhileNodeAttr.namedItem("timeout").nodeValue();
        QString unit = waitWhileNodeAttr.namedItem("unit").nodeValue();
        QString errorOnTimeOut = waitWhileNodeAttr.namedItem("error on timeout").nodeValue();

        pWaitWhile->setStopOnFail(stopOnFail);
        pWaitWhile->setLog(log);
        pWaitWhile->setDeactivate(deactivate);
        pWaitWhile->setTimeOut(timeout);
        pWaitWhile->setUnit(unit);
        pWaitWhile->setErrorOnTimeOut(errorOnTimeOut);

        QDomNodeList waitCmdNodeList = cmdNode.childNodes();
        for(int i = 0; i < waitCmdNodeList.count(); i++)
        {
            QDomNode waitCmdNode = waitCmdNodeList.at(i);
            if(waitCmdNode.nodeName() == FSII_CONDITIONS)
            {
                AINGTAFSIICondition objCondition;
                QDomNode conditionNode = waitCmdNode.childNodes().at(0);
                if(conditionNode.nodeName() == FSII_CONDITION)
                {
                    QString logicalOperator = conditionNode.namedItem(FSII_TAG_OPERATOR).nodeValue();
                    objCondition.setOperator(logicalOperator);

                    QDomNode leftNode = conditionNode.childNodes().at(0);
                    QDomNode paramNode = leftNode.childNodes().at(0);
                    QString paramName = paramNode.attributes().namedItem("name").nodeValue();
                    objCondition.setLHS(paramName);

                    if(paramNode.nodeName() == FSII_SIGNAL)
                    {
                        QString name = getGTAParamName(paramName);
                        if(!name.isEmpty())
                            objCondition.setLHS(name);
                        else
                        {
                            //log error
                            _LogFile.open(QFile::Append|QIODevice::Text);
                            QString error = QString("Parameter not defined in the defintion block\n");
                            _LogFile.write(error.toUtf8());
                            _LogFile.close();
                            pWaitWhile->setIsCommandValid(false);
                        }

                    }

                    QDomNode rightNode = conditionNode.childNodes().at(1);
                    QDomNode paramValNode = rightNode.childNodes().at(0);
                    QString paramVal = paramValNode.attributes().namedItem("value").nodeValue();
                    objCondition.setRHS(paramVal);

                    pWaitWhile->setCondition(objCondition);
                }
            }
        }
        pCmd = pWaitWhile;
    }
    else if(cmdNode.nodeName() == FSII_CMD_IF_ELSE)
    {
        AINGTAFSIIIf *pIf = new AINGTAFSIIIf(CMD_TYPE_IF);

        QDomNamedNodeMap cmdNodeAttr = cmdNode.attributes();
        QString stopOnFail = cmdNodeAttr.namedItem(FSII_TAG_STOP_ON_FAIL).nodeValue();
        QString log = cmdNodeAttr.namedItem(FSII_TAG_LOG).nodeValue();
        QString deactivate = cmdNodeAttr.namedItem(FSII_TAG_DEACTIVATE).nodeValue();
        pIf->setStopOnFail(stopOnFail);
        pIf->setLog(log);
        pIf->setDeactivate(deactivate);

        QDomNodeList ifCmdNodeList = cmdNode.childNodes();
        for(int i = 0; i < ifCmdNodeList.count(); i++)
        {
            QDomNode ifCmdNode = ifCmdNodeList.at(i);
            if(ifCmdNode.nodeName() == FSII_CONDITIONS)
            {
                QString logicalOp = ifCmdNode.attributes().namedItem(FSII_TAG_LOGICAL_CONNECTOR).nodeValue();
                pIf->setLogicalOperator(logicalOp);
                QDomNodeList conditionNodeList = ifCmdNode.childNodes();
                for(int j= 0; j < conditionNodeList.count(); j++)
                {
                    AINGTAFSIICondition objCondition;
                    QDomNode conditionNode = conditionNodeList.at(j);
                    QString condOp = conditionNode.attributes().namedItem(FSII_TAG_OPERATOR).nodeValue();
                    objCondition.setOperator(condOp);
                    QDomNode leftNode = conditionNode.childNodes().at(0);
                    QDomNode paramNode = leftNode.childNodes().at(0);
                    QString paramName = paramNode.attributes().namedItem("name").nodeValue();
                    objCondition.setLHS(paramName);

                    if(paramNode.nodeName() == FSII_SIGNAL)
                    {
                        if(! _CRDCToGTAParamMap.isEmpty())
                        {
                            QString name = getGTAParamName(paramName);
                            if(!name.isEmpty())
                                objCondition.setLHS(name);
                            else
                            {
                                //log error
                                //will complete if be invalid if one of the condition has unmatched signal?
                            }
                        }
                    }
                    QDomNode rightNode = conditionNode.childNodes().at(1);
                    QDomNode paramValNode = rightNode.childNodes().at(0);
                    QString paramaVal = paramValNode.attributes().namedItem("value").nodeValue();
                    objCondition.setRHS(paramaVal);

                    pIf->addCondition(objCondition);
                }
            }
            else if(ifCmdNode.nodeName() == FSII_IS_TRUE)
            {
                QDomNodeList trueCondList = ifCmdNode.childNodes();
                for(int j = 0; j < trueCondList.count(); j++)
                {
                    QDomNode trueCondNode = trueCondList.at(j);
                    AINGTAFSIICommandBase *pIfCmd = createCommand(trueCondNode);
                    if(pIfCmd)
                    {
                        pIf->addCommandOnTrue(pIfCmd);
                    }

                }
            }
            else if(ifCmdNode.nodeName() == FSII_IS_FALSE)
            {
                QDomNodeList falseCondList = ifCmdNode.childNodes();
                for(int j = 0; j < falseCondList.count(); j++)
                {
                    QDomNode falseCondNode = falseCondList.at(j);
                    AINGTAFSIICommandBase *pIfCmd = createCommand(falseCondNode);
                    if(pIfCmd)
                    {
                        pIf->addComamndOnFalse(pIfCmd);
                    }
                }
            }
        }

        pCmd = pIf;
    }
    else if(cmdNode.nodeName() == FSII_CMD_LOOP)
    {

        AINGTAFSIILoop *pLoop = new AINGTAFSIILoop(CMD_TYPE_LOOP);

        QDomNamedNodeMap cmdNodeAttr = cmdNode.attributes();
        QString stopOnFail = cmdNodeAttr.namedItem(FSII_TAG_STOP_ON_FAIL).nodeValue();
        QString log = cmdNodeAttr.namedItem(FSII_TAG_LOG).nodeValue();
        QString deactivate = cmdNodeAttr.namedItem(FSII_TAG_DEACTIVATE).nodeValue();
        pLoop->setStopOnFail(stopOnFail);
        pLoop->setLog(log);
        pLoop->setDeactivate(deactivate);

        QDomNodeList loopCmdNodeList = cmdNode.childNodes();
        for(int i = 0; i < loopCmdNodeList.count(); i++)
        {
            QDomNode loopCmdNode = loopCmdNodeList.at(i);
            if(loopCmdNode.nodeName() == FSII_CONDITIONS)
            {
                AINGTAFSIICondition objCondition;
                QDomNode conditionNode = loopCmdNode.childNodes().at(0);
                QString condOp = conditionNode.attributes().namedItem(FSII_TAG_OPERATOR).nodeValue();
                objCondition.setOperator(condOp);

                QDomNode leftNode = conditionNode.childNodes().at(0);
                QDomNode paramNameNode = leftNode.childNodes().at(0);
                QString paramName = paramNameNode.attributes().namedItem("name").nodeValue();
                objCondition.setLHS(paramName);

                if(paramNameNode.nodeName() == FSII_SIGNAL)
                {
                    if(! _CRDCToGTAParamMap.isEmpty())
                    {
                        QString name = getGTAParamName(paramName);
                        if(!name.isEmpty())
                            objCondition.setLHS(name);
                        else
                        {
                            //log error
                            _LogFile.open(QFile::Append|QIODevice::Text);
                            QString error = QString("Parameter not defined in the defintion block\n");
                            _LogFile.write(error.toUtf8());
                            _LogFile.close();
                            pLoop->setIsCommandValid(false);
                        }
                    }
                }


                QDomNode rightNode = conditionNode.childNodes().at(1);
                QDomNode paramValNode = rightNode.childNodes().at(1);
                QString paramVal = paramValNode.attributes().namedItem("value").nodeValue();
                objCondition.setRHS(paramVal);

                pLoop->setCondition(objCondition);

            }
            else if(loopCmdNode.nodeName() == FSII_FUNCTION_BLOCK)
            {
                QDomNodeList funcBlockNodeList = loopCmdNode.childNodes();
                for(int j = 0; j < funcBlockNodeList.count(); j++)
                {
                    QDomNode funcBlockNode = funcBlockNodeList.at(j);
                    AINGTAFSIICommandBase *pCmdBase = createCommand(funcBlockNode);
                    pLoop->addFunctionBlockCommand(pCmdBase);
                }
            }
        }
        pCmd = pLoop;
    }
    else if(cmdNode.nodeName() == FSII_CMD_RUN_X_TIMES)
    {
        AINGTAFSIIRunXTimes *pRunXTimes = new AINGTAFSIIRunXTimes(CMD_TYPE_RUN_X_TIMES);

        QDomNamedNodeMap cmdNodeAttr = cmdNode.attributes();
        QString stopOnFail = cmdNodeAttr.namedItem(FSII_TAG_STOP_ON_FAIL).nodeValue();
        QString log = cmdNodeAttr.namedItem(FSII_TAG_LOG).nodeValue();
        QString deactivate = cmdNodeAttr.namedItem(FSII_TAG_DEACTIVATE).nodeValue();
        QString numOfIterations = cmdNodeAttr.namedItem(FSII_TAG_NUM_ITERATIONS).nodeValue();
        pRunXTimes->setStopOnFail(stopOnFail);
        pRunXTimes->setLog(log);
        pRunXTimes->setDeactivate(deactivate);
        pRunXTimes->setNumberOfIterations(numOfIterations);

        QDomNode funcBlockNode = cmdNode.childNodes().at(0);
        if(funcBlockNode.nodeName() == FSII_FUNCTION_BLOCK)
        {
            QDomNodeList commandList = funcBlockNode.childNodes();
            for(int i = 0; i < commandList.count(); i++)
            {
                QDomNode commandNode = commandList.at(i);
                AINGTAFSIICommandBase *pCmdBase = createCommand(commandNode);
                pRunXTimes->addFunctionBlockCommand(pCmdBase);
            }

        }
        pCmd = pRunXTimes;
    }
    else if(cmdNode.nodeName() == FSII_CMD_PAUSE)
    {
        AINGTAFSIIPause *pPause = new AINGTAFSIIPause(CMD_TYPE_PAUSE);

        QDomNamedNodeMap cmdNodeAttr = cmdNode.attributes();
        QString stopOnFail = cmdNodeAttr.namedItem(FSII_TAG_STOP_ON_FAIL).nodeValue();
        QString log = cmdNodeAttr.namedItem(FSII_TAG_LOG).nodeValue();
        QString deactivate = cmdNodeAttr.namedItem(FSII_TAG_DEACTIVATE).nodeValue();
        QString message = cmdNodeAttr.namedItem(FSII_TAG_MESSAGE).nodeValue();
        pPause->setStopOnFail(stopOnFail);
        pPause->setLog(log);
        pPause->setDeactivate(deactivate);
        pPause->setMessage(message);

        pCmd = pPause;
    }
    else
    {
        QString cmdName = cmdNode.nodeName();
        _LogFile.open(QFile::Append|QIODevice::Text);
        QString err = QString("%1%2").arg(cmdName, QString(" - has not been parsed\n"));
        _LogFile.write(err.toUtf8());
        _LogFile.close();
    }

    return pCmd;
}

QList<AINGTAFSIIFile> AINGTAFSIIParser::getFSIIData()
{
    return _FS2Data;
}

QHash<QString, AINGTAFSIISignal> AINGTAFSIIParser::getFSParameters()
{
    return _FSParameters;
}

AINGTAFSIIFile AINGTAFSIIParser::getFileFromName(QString &iName)
{
    AINGTAFSIIFile file;
    for(int i=0;i<_FS2Data.count();i++)
    {
        if(_FS2Data.at(i).getDescription() == iName)
        {
            return _FS2Data.at(i);
        }
    }
    return file;
}
QString AINGTAFSIIParser::getValidityValue(const QString &iName,const QString &iGTAName, const QString &iFSVal)
{
    QStringList items = iName.split(".");
    if(items.count()>2)
    {
        QString mediaDir = items.at(1);
        QString media = mediaDir.split("_").at(0);
        if(media == "AFDX")
        {
            if(iFSVal == "0x03")
            {
                return "NO";
            }
            else if(iFSVal == "0x30")
            {
                return "NCD";
            }
            else if(iFSVal.toLower() == "0x0c")
            {
                return "FT";
            }
            else if(iFSVal == "0x00")
            {
                return "ND";
            }
            else
                return "NO";
        }
        else if(media == "A429")
        {
            AINGTAFSIISignal signal =  _FSParameters.value(iGTAName);
            QString mediaType = signal.getMediaType();
            if(mediaType == "BNR")
            {
                if(iFSVal == "0x03")
                {
                    return "NO";
                }
                else if(iFSVal == "0x01")
                {
                    return "NCD";
                }
                else if(iFSVal == "0x02")
                {
                    return "FT";
                }
                else if(iFSVal == "0x00")
                {
                    return "FW";
                }
                else
                    return "NO";
            }
            else if(mediaType == "BCD")
            {
                if(iFSVal == "0x03")
                {
                    return "MINUS";
                }
                else if(iFSVal == "0x01")
                {
                    return "NCD";
                }
                else if(iFSVal == "0x02")
                {
                    return "FT";
                }
                else if(iFSVal == "0x00")
                {
                    return "PLUS";
                }
                else
                    return "NO";
            }
            else if(mediaType == "Discrete")
            {
                if(iFSVal == "0x03")
                {
                    return "FW";
                }
                else if(iFSVal == "0x01")
                {
                    return "NCD";
                }
                else if(iFSVal == "0x02")
                {
                    return "NO";
                }
                else if(iFSVal == "0x00")
                {
                    return "NO";
                }
                else
                    return "NO";
            }
            else
                return "NO";
        }
    }
    return "NO";
}
