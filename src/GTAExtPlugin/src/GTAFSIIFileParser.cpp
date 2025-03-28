#include "AINGTAFSIIFileParser.h"
#include <qdom.h>
#include <QFile>
#include <QDebug>
#include "AINGTAExtPluginUtils.h"
#include <QDir>
#include "AINGTAElement.h"
#include "AINGTAActionCall.h"
#include "AINGTAHeader.h"
#include "AINGTAUtil.h"
#include "AINGTAActionPrint.h"
#include "AINGTAActionSet.h"
#include "AINGTAActionSetList.h"
#include "AINGTAEquationConst.h"
#include "AINGTAActionWait.h"
#include "AINGTACheckValue.h"
#include "AINGTAInvalidCommand.h"
#include "AINGTALocalDbUtil.h"
#include <QSqlQuery>

AINGTAFSIIFileParser::AINGTAFSIIFileParser()
{
    _ErrorMessages.clear();
}
AINGTAFSIIFileParser::~AINGTAFSIIFileParser()
{
}


QStringList AINGTAFSIIFileParser::getErrors()
{
    return _ErrorMessages;
}


bool AINGTAFSIIFileParser::parseAndSaveFile(const QFileInfo &iFSIIFile)
{

    bool rc = true;

    QDomDocument domDoc;
    _CurrentFunctionCntr = 0;
    _NameElemMap.clear();
    QString filePath = iFSIIFile.absoluteFilePath();
    QString fileName = iFSIIFile.fileName();
    fileName.replace(".fs2.xml","");
    _CurretFile = fileName;
    QFile xmlFile(filePath);
    if (!xmlFile.open(QIODevice::ReadOnly))
    {
        QString msg  = QString("%1 Could not open %2").arg(ERR_PLUGIN,filePath);
        _ErrorMessages.append(msg);
        return false;
    }
    if (!domDoc.setContent(&xmlFile))
    {
        xmlFile.close();
        QString msg  = QString("%1 Could not open %2").arg(ERR_PLUGIN,filePath);
        _ErrorMessages.append(msg);

        return false;
    }
    xmlFile.close();

    QDomElement docElem = domDoc.documentElement();

    QString rootElemName = docElem.nodeName();
    if(rootElemName == FSII_ELEM_ROOT)
    {

        QDomNodeList rootElemChildren = docElem.childNodes();
        for(int i=0;i<rootElemChildren.count();i++)
        {
            QDomElement childElem = rootElemChildren.at(i).toElement();
            QString childElemName = childElem.nodeName();
            if(childElemName == FSII_ELEM_TP)
            {
                createMainTestProcedure(childElem);
            }
            else if(childElemName == FSII_ELEM_FUNCTION)
            {
                createObjects(childElem);
            }
        }
    }
    return rc;
}

void AINGTAFSIIFileParser::createMainTestProcedure(QDomElement &iTPElem)
{
    QString procName = iTPElem.attribute(FSII_ATTR_NAME);
    QString _CurrentSUT = iTPElem.attribute(FSII_ATTR_SUT);

    QString dirName = _CurretFile;
    dirName.replace("  "," ");
    dirName.replace(" ","_");
    dirName.replace(".","_");
    dirName.replace("(","");
    dirName.replace(")","");

    _CurrentDir = dirName;
    _ElementDirPath = QString(QDir::cleanPath("%1/%2")).arg(_ExportPath,_CurrentDir);
    QDir dir(_ElementDirPath);

    if(!dir.exists(_ElementDirPath))
        dir.mkdir(_ElementDirPath);

    AINGTAHeader* header = new AINGTAHeader;
    header->setName(_CurrentDir) ;
    header->setDescription(procName);
    header->insertDefaultHeaderNodes();
    header->editField("DESCRIPTION",procName);

    QString TPName = "Main";
    _NameElemMap.insert(procName,TPName);

    QDomNodeList functionCalls = iTPElem.childNodes();
    QList<AINGTACommandBase *> cmdList;
    for(int i=0;i<functionCalls.count();i++)
    {
        QDomElement funcCall = functionCalls.at(i).toElement();
        if(funcCall.nodeName() == FSII_ELEM_CALL)
        {
            QString funcName = funcCall.attribute(FSII_ATTR_FUNC);
            QString isIgnored = funcCall.attribute(FSII_ATTR_DEACTIVATE);
            QString shortName = QString("Function_%1").arg(QString::number(i));
            _NameElemMap.insert(funcName,shortName);
            _CurrentFunctionCntr++;

            AINGTAActionCall *pCmd = NULL;

            QString callElemName = QString("%1/%2").arg(_CurrentDir,shortName);
            createCallCommand(callElemName,pCmd);
            if(pCmd != NULL)
            {
                if(!isIgnored.trimmed().isEmpty())
                {
                    if(isIgnored == "false")
                        pCmd->setIgnoreInSCXML(false);
                    else
                        pCmd->setIgnoreInSCXML(true);
                }

                cmdList.append(pCmd);
            }
        }
    }
    if(!cmdList.isEmpty())
    {
        AINGTAElement *tpElem = new AINGTAElement(AINGTAElement::PROCEDURE,cmdList);
        tpElem->setHeader(header);
        tpElem->setName(TPName);
        QString completeFilePAth = QString(QDir::cleanPath("%1/%2.pro")).arg(_ElementDirPath,TPName);
        tpElem->setAbsoluteFilePath(completeFilePAth);
        emit saveImportElement(tpElem,completeFilePAth);
    }
}

void AINGTAFSIIFileParser::createCallCommand(const QString &iCallName, AINGTAActionCall *&oCmd)
{
    AINGTAActionCall *pCallCmd = new AINGTAActionCall();
    pCallCmd->setElement(iCallName+".fun");
    pCallCmd->setAction(ACT_CALL);
    pCallCmd->setComplement(COM_CALL_FUNC);
    pCallCmd->setObjId();
    QString uuid = _FunctionUUIDMap.value(iCallName.split("/").last());
    if (uuid.isEmpty())
    {
        uuid = QUuid::createUuid().toString();
        _FunctionUUIDMap.insert(iCallName.split("/").last(),uuid);
    }
    pCallCmd->setRefrenceFileUUID(uuid);
    oCmd = pCallCmd;
}
void AINGTAFSIIFileParser::createObjects(QDomElement &iObjElem)
{
    QString funcName = iObjElem.attribute(FSII_ATTR_NAME);
    QString shortName;
    if(_NameElemMap.contains(funcName))
        shortName = _NameElemMap.value(funcName);
    else
    {
        shortName = QString("Function_%1").arg(QString::number(_CurrentFunctionCntr++));
        _NameElemMap.insert(funcName,shortName);
    }


    AINGTAHeader* header = new AINGTAHeader;
    header->setName(_CurrentDir) ;
    header->setDescription(funcName);
    header->insertDefaultHeaderNodes();
    header->editField("DESCRIPTION",funcName);

    QDomNodeList defList = iObjElem.elementsByTagName(FSII_ELEM_DEF);
    QDomNodeList funcBlockList = iObjElem.elementsByTagName(FSII_ELEM_FUNC_BLOCK);

    QDomElement funcDefElem;
    QDomElement funcBlkElem;
    if(defList.count()>0)
        funcDefElem = defList.at(0).toElement();

    if(funcBlockList.count()>0)
        funcBlkElem = funcBlockList.at(0).toElement();


    //Handle Definitions
    QDomNodeList signalList = funcDefElem.childNodes();
    QStringList fsSMMSignal;

    for(int i=0;i<signalList.count();i++)
    {
        QDomElement signalElem = signalList.at(i).toElement();
        if(signalElem.nodeName() == FSII_ELEM_SIGNAL)
        {
            QString sig = signalElem.attribute(FSII_ATTR_NAME);
            QString app = signalElem.attribute(FSII_ATTR_APP);

            //how to handle multiple apps
            if(app.contains(","))
                app = app.split(",").first();
            if(sig.endsWith(".FS") || sig.endsWith(".SSM")|| (sig.endsWith(".SDI")))
            {
                fsSMMSignal.append(QString("%1:%2").arg(app,sig));
            }
            else
            {
                resolveParams(sig,app);
            }
        }
    }

    for(int i=0;i<fsSMMSignal.count();i++)
    {
        QString info = fsSMMSignal.at(i);
        if(info.contains(":"))
        {
            QStringList items = info.split(":");
            resolveParams(items.at(1),items.at(0));
        }
    }

    //Handle function Block

    QList<AINGTACommandBase *> cmdList;

    QDomNodeList funcCommands = funcBlkElem.childNodes();

    if((funcCommands.count()<1))
    {
        AINGTAActionPrint *pPrintCmd = new AINGTAActionPrint(ACT_PRINT,COM_PRINT_MSG);
        pPrintCmd->setValues(QStringList()<<funcName);
        pPrintCmd->setActionOnFail(ACT_FAIL_CONTINUE);
        pPrintCmd->setIgnoreInSCXML(false);
        pPrintCmd->setObjId();
        cmdList.append(pPrintCmd);
    }
    for(int i=0;i<funcCommands.count();i++)
    {
        QDomElement funcCmd = funcCommands.at(i).toElement();
        QString isIgnoredAttr = funcCmd.attribute(FSII_ATTR_DEACTIVATE);
        QString stopOnFailAttr = funcCmd.attribute(FSII_ATTR_SOF);
        bool isIgnored = false;
        QString actionOnFail = "continue";

        if(!isIgnoredAttr.trimmed().isEmpty())
        {
            if(isIgnoredAttr == "false")
                isIgnored = false;
            else
                isIgnored = true;
        }
        if(stopOnFailAttr == "false")
            actionOnFail = "stop";
        else
            actionOnFail = "continue";

        QString cmdName = funcCmd.nodeName();
        if (cmdName == "")
        {
            _ErrorMessages.append("comment in Function Block. Skipped in GTA");
            continue;
        }
        if(cmdName == FSII_ELEM_CALL)
        {
            QString funcName = funcCmd.attribute(FSII_ATTR_FUNC);

            QString shortName;
            if(_NameElemMap.contains(funcName))
            {
                shortName = _NameElemMap.value(funcName);
            }
            else
            {
                shortName = QString("Function_%1").arg(QString::number(_CurrentFunctionCntr++));
                _NameElemMap.insert(funcName,shortName);
            }

            AINGTAActionCall *pCallCmd = NULL;
            QString callElemName = QString("%1/%2").arg(_CurrentDir,shortName);
            createCallCommand(callElemName,pCallCmd);
            pCallCmd->setIgnoreInSCXML(isIgnored);
            pCallCmd->setActionOnFail(actionOnFail);
            if(pCallCmd != NULL)
            {
                cmdList.append(pCallCmd);
            }
        }
        else if(cmdName == FSII_ELEM_LOG)
        {
            QString message = funcCmd.attribute(FSII_ATTR_TEXT);
            AINGTAActionPrint *pPrintCmd = new AINGTAActionPrint(ACT_PRINT,COM_PRINT_MSG);
            pPrintCmd->setValues(QStringList()<<message);
            pPrintCmd->setActionOnFail(actionOnFail);
            pPrintCmd->setIgnoreInSCXML(isIgnored);
            pPrintCmd->setObjId();
            cmdList.append(pPrintCmd);
        }
        else if(cmdName == FSII_ELEM_WAIT)
        {
            QString timeVal = funcCmd.attribute(FSII_ATTR_VALUE);
            QString timeValUnit = funcCmd.attribute(FSII_ATTR_UNIT);

            bool isTimeOk = false;
            double waitTime = timeVal.trimmed().toDouble(&isTimeOk);
            if(isTimeOk)
            {
                AINGTAActionWait *pWaitCmd = new AINGTAActionWait(ACT_WAIT, COM_WAIT_FOR,AINGTAActionWait::FOR);
                pWaitCmd->setActionOnFail(actionOnFail);
                pWaitCmd->setIgnoreInSCXML(isIgnored);
                int mf = 1;
                if(timeValUnit == FSII_ATTR_UNIT_MS)
                    mf = 1000;
                QString waitTimeStr = QString::number(waitTime / mf);
                pWaitCmd->setCounter(waitTimeStr);
                pWaitCmd->setObjId();
                if(pWaitCmd != NULL)
                {
                    cmdList.append(pWaitCmd);
                }
            }
        }
        else if(cmdName == FSII_ELEM_SET)
        {

            QDomNodeList children = funcCmd.childNodes();

            QString lhs;
            QString rhs;
            bool isSetOnlyFS = false;
            for(int j=0;j<children.count();j++)
            {
                QDomElement childElem = children.at(j).toElement();
                if(childElem.nodeName() == FSII_ELEM_WRITE)
                {
                    QDomNodeList signalParamList = childElem.childNodes();
                    for(int k=0;k<signalParamList.count();k++)
                    {
                        QDomElement sigParamElem = signalParamList.at(k).toElement();
                        if((sigParamElem.nodeName() == FSII_ELEM_SIGNAL)||(sigParamElem.nodeName() == FSII_ELEM_PARAMETER))
                        {
                            lhs = sigParamElem.attribute(FSII_ATTR_NAME);
                            break;
                        }
                        else if(sigParamElem.nodeName() == FSII_ELEM_CONST)
                        {
                            lhs = (sigParamElem.attribute(FSII_ATTR_VALUE) == "true")? "1":((sigParamElem.attribute(FSII_ATTR_VALUE) == "false")? "0":resolveOpaqueSignals(sigParamElem.attribute(FSII_ATTR_VALUE)));
                            break;
                        }
                    }
                }
                else if(childElem.nodeName() == FSII_ELEM_READ)
                {
                    QDomNodeList signalParamList = childElem.childNodes();
                    for(int k=0;k<signalParamList.count();k++)
                    {
                        QDomElement sigParamElem = signalParamList.at(k).toElement();
                        if((sigParamElem.nodeName() == FSII_ELEM_SIGNAL)||(sigParamElem.nodeName() == FSII_ELEM_PARAMETER))
                        {
                            rhs = sigParamElem.attribute(FSII_ATTR_NAME);
                            break;
                        }
                        else if(sigParamElem.nodeName() == FSII_ELEM_CONST)
                        {
                            rhs = (sigParamElem.attribute(FSII_ATTR_VALUE) == "true")? "1":((sigParamElem.attribute(FSII_ATTR_VALUE) == "false")? "0":resolveOpaqueSignals(sigParamElem.attribute(FSII_ATTR_VALUE)));
                            break;
                        }
                    }
                }
            }
            AINGTAICDParameter lhsParam;
            AINGTAICDParameter rhsParam;
            if(lhs.endsWith(".FS") || lhs.endsWith(".SSM"))
            {
                isSetOnlyFS = true;
            }
            if(_ResolvedParamList.contains(lhs))
            {
                lhsParam = _ResolvedParamList.value(lhs);
                lhs = lhsParam.getTempParamName();
            }

            if(_ResolvedParamList.contains(rhs))
            {
                rhsParam = _ResolvedParamList.value(rhs);
                rhs = rhsParam.getTempParamName();
            }

            AINGTAActionSet *pSetCmd = new AINGTAActionSet(ACT_SET_INSTANCE,QString(""));

            pSetCmd->setParameter(lhs.trimmed());
            pSetCmd->setIsSetOnlyFS(isSetOnlyFS);

            if(!isSetOnlyFS)
            {
                //pSetCmd->setFunctionStatus("NO_CHANGE");
                pSetCmd->setFunctionStatus("NO");
                AINGTAEquationConst *pConst = new AINGTAEquationConst();
                pConst->setConstant(rhs.trimmed());
                AINGTAEquationBase* pBaseEqn = pConst;
                pSetCmd->setEquation(pBaseEqn);
            }
            else
            {
                QString fsVal;
                resolveFSValue(lhsParam,rhs,fsVal);
                if (fsVal.isEmpty())
                {
                    fsVal = "NO";
                    QString comment = QString("%1 is considered as local param. FS is defaulted to NO").arg(lhs);
                    pSetCmd->setComment(comment);
                }
                pSetCmd->setFunctionStatus(fsVal);
            }
            AINGTAActionSetList *pSetList = new AINGTAActionSetList(ACT_SET,QString(""));
            pSetList->addSetAction(pSetCmd);
            pSetList->setTimeOut("3","sec");
            pSetList->setObjId();
            cmdList.append(pSetList);
        }
        else if(cmdName == FSII_ELEM_CHECK)
        {
            QDomNodeList conditionsLists = funcCmd.elementsByTagName(FSII_ELEM_CONDITIONS);
            //handle logic connector, by default it is "and"
            QDomElement conditionsElem;
            if(conditionsLists.count()>0)
            {
                conditionsElem = conditionsLists.at(0).toElement();
            }
            QDomNodeList conditions = conditionsElem.childNodes();

            AINGTACheckValue *pChkVal = NULL;
            QString lhs = "";
            QString rhs = "";
            for(int j=0;j<conditions.count();j++)
            {
                QDomElement condtionElem = conditions.at(j).toElement();
                QString oper = condtionElem.attribute(FSII_ATTR_OPERATOR);
                QDomNodeList leftElemList = condtionElem.elementsByTagName(FSII_ELEM_LEFT);
                QDomNodeList rightElemList = condtionElem.elementsByTagName(FSII_ELEM_RIGHT);
                if(leftElemList.count()>0)
                {
                    QDomElement leftElem = leftElemList.at(0).toElement();
                    QDomElement fsSignalElem = leftElem.firstChildElement();
                    if(!fsSignalElem.isNull() && ((fsSignalElem.nodeName() == FSII_ELEM_SIGNAL) || (fsSignalElem.nodeName() == FSII_ELEM_PARAMETER)))
                    {
                        lhs = fsSignalElem.attribute(FSII_ATTR_NAME);
                    }
                    else if(fsSignalElem.nodeName() == FSII_ELEM_CONST)
                    {
                        //lhs = fsSignalElem.attribute(FSII_ATTR_VALUE);
                        lhs = (fsSignalElem.attribute(FSII_ATTR_VALUE) == "true")? "1":((fsSignalElem.attribute(FSII_ATTR_VALUE) == "false")? "0":resolveOpaqueSignals(fsSignalElem.attribute(FSII_ATTR_VALUE)));
                    }
                }
                if(rightElemList.count()>0)
                {
                    QDomElement rightElem = rightElemList.at(0).toElement();
                    QDomElement fsSignalElem = rightElem.firstChildElement();
                    if(!fsSignalElem.isNull() && ((fsSignalElem.nodeName() == FSII_ELEM_SIGNAL) || (fsSignalElem.nodeName() == FSII_ELEM_PARAMETER)))
                    {
                        rhs = fsSignalElem.attribute(FSII_ATTR_NAME);
                    }
                    else if(fsSignalElem.nodeName() == FSII_ELEM_CONST)
                    {
                        //rhs = fsSignalElem.attribute(FSII_ATTR_VALUE);
                        rhs = (fsSignalElem.attribute(FSII_ATTR_VALUE) == "true")? "1":((fsSignalElem.attribute(FSII_ATTR_VALUE) == "false")? "0":resolveOpaqueSignals(fsSignalElem.attribute(FSII_ATTR_VALUE)));
                    }
                }

                if(!(lhs.isEmpty() || rhs.isEmpty()))
                {

                    bool isCheckOnlyFS =false;
                    bool isCheckOnlySDI = false;
                    AINGTAICDParameter lhsParam;
                    AINGTAICDParameter rhsParam;
                    if(lhs.endsWith(".FS") || lhs.endsWith(".SSM"))
                    {
                        isCheckOnlyFS = true;
                    }
                    if(lhs.endsWith(".SDI"))
                    {
                        isCheckOnlySDI = true;
                    }

                    if(_ResolvedParamList.contains(lhs))
                    {
                        lhsParam = _ResolvedParamList.value(lhs);
                        lhs = lhsParam.getTempParamName();
                    }

                    if(_ResolvedParamList.contains(rhs))
                    {
                        rhsParam = _ResolvedParamList.value(rhs);
                        rhs = rhsParam.getTempParamName();
                    }

                    if(pChkVal == NULL)
                    {
                        pChkVal = new AINGTACheckValue();
                    }

                    if(oper == "==")
                        oper = "=";



                    if(!isCheckOnlyFS && !isCheckOnlySDI)
                    {
                        if(rhs == "false")
                            rhs = "0";
                        else if(rhs == "true")
                            rhs = "1";

                        pChkVal->insertParamenter(lhs.trimmed(),oper,rhs.trimmed(),"NOT_LOST",0.0,"value",true,false);
                    }
                    else if(isCheckOnlyFS)
                    {
                        QString fsVal;
                        resolveFSValue(lhsParam,rhs,fsVal);
                        if (fsVal.isEmpty())
                        {
                            fsVal = "NO";
                            QString comment = QString("%1 is considered as local param. FS is defaulted to NO").arg(lhs);
                            pChkVal->setComment(comment);
                        }
                        pChkVal->insertParamenter(lhs.trimmed(),oper,"",fsVal,0.0,"value",false,true);
                    }
                    else if(isCheckOnlySDI)
                    {
                        bool isIntOk =false;
                        int sdiInt = rhs.toInt(&isIntOk);
                        if(isIntOk)
                        {
                            if(sdiInt == 0)
                                pChkVal->insertParamenter(lhs.trimmed(),oper,"","NOT_LOST",0.0,"value",false,false,true,false,false,"00",QString(""));
                            else if(sdiInt == 1)
                                pChkVal->insertParamenter(lhs.trimmed(),oper,"","NOT_LOST",0.0,"value",false,false,true,false,false,"01",QString(""));
                            else if(sdiInt == 2)
                                pChkVal->insertParamenter(lhs.trimmed(),oper,"","NOT_LOST",0.0,"value",false,false,true,false,false,"10",QString(""));
                            else if(sdiInt == 3)
                                pChkVal->insertParamenter(lhs.trimmed(),oper,"","NOT_LOST",0.0,"value",false,false,true,false,false,"11",QString(""));
                        }
                    }
                }
            }
            if(pChkVal != NULL)
            {
                pChkVal->setWaitUntil(true);
                pChkVal->setTimeOut("3","sec");
                pChkVal->setObjId();
                cmdList.append(pChkVal);

            }
        }
        else
        {
            AINGTACommandBase *pCmd = NULL;
            QString reason = "Reason: Unknown Command or not handled by GTA.";
            makeInvalidCommand(cmdName, reason, pCmd);
            cmdList.append(pCmd);
        }
    }
    if(!cmdList.isEmpty())
    {
        AINGTAElement *tpElem = new AINGTAElement(AINGTAElement::FUNCTION,cmdList);
        tpElem->setHeader(header);
        tpElem->setName(shortName);
        QString completeFilePAth = QString(QDir::cleanPath("%1/%2.fun")).arg(_ElementDirPath,shortName);
        tpElem->setAbsoluteFilePath(completeFilePAth);
        //the FSII file is parsed line by line and thus making it difficult to check if
        //there is a function present after the current line being parsed from the file
        //to handle this, a map is updated when a call command is created if the function is
        //not already found in the map. The same uuid must be assigned to the function when it is created.
        QString uuid = tpElem->getUuid();
        if (_FunctionUUIDMap.keys().contains(shortName))
        {
            uuid = _FunctionUUIDMap.value(shortName);
            tpElem->setUuid(uuid);
        }
        else
        {
            _FunctionUUIDMap.insert(shortName,uuid);
        }
        emit saveImportElement(tpElem,completeFilePAth);
    }
}
bool AINGTAFSIIFileParser::makeInvalidCommand(const QString &iText, const QString &iReason, AINGTACommandBase *&oCmd)
{
    bool rc;
    AINGTAInvalidCommand *pInvalid = new AINGTAInvalidCommand();
    QString message = QString("%1(%2)").arg(iText,iReason);
    _ErrorMessages.append(message);
    pInvalid->setMessage(message);
    //    pInvalid->setObjId();
    oCmd = pInvalid;
    rc = true;
    return rc;
}

void AINGTAFSIIFileParser::resolveParams(const QString &iSig, const QString &iAppName)
{
    QString app = iAppName;
    QString sig = iSig;
    QString messageName;
    QString signalName;
    QString fsName;
    QString busName;
    QString equipName;
    QString media;
    QString direction;
    QStringList items = sig.split(".");
    if(sig.endsWith(".FS") || sig.endsWith(".SSM")|| (sig.endsWith(".SDI")))
    {
        if(!_ResolvedParamList.contains(sig))
        {
            if(items.count()>4)
            {
                equipName = items.at(0);
                QString medDir = items.at(1);
                QStringList items1 = medDir.split("_");
                direction = items1.last();
                media = items1.first();
                busName = items.at(2);
                messageName = items.at(3);
                fsName = items.at(4);

                //failsafe to check query doesn't fail
                app.replace("'","");
                equipName.replace("'","");
                medDir.replace("'","");
                direction.replace("'","");
                media.replace("'","");
                busName.replace("'","");
                messageName.replace("'","");
                fsName.replace("'","");

                if(media == "A429")
                {
                    QString appQueryStr = "APPLICATION ='"+app+"'";
                    QString mediaQueryStr = "AND MEDIA ='"+media+"'";
                    QString busQueryStr = " AND BUSNAME ='"+busName+"'";
                    QString messageQueryStr = " AND MESSAGE ='"+messageName+"'";
                    QString equipmentQueryStr  = " AND EQUIPNAME ='"+equipName+"'";
                    QString whereClause = QString("%1 %2 %3 %4 %5").arg(appQueryStr,mediaQueryStr,busQueryStr,messageQueryStr,equipmentQueryStr);
                    QString fsSigQueryTempl = QString("SELECT * FROM %1 WHERE %2 LIMIT 1;").arg(PARAMETER_TABLE,whereClause);
                    QList<AINGTAICDParameter> paramList;
                    AINGTALocalDbUtil::fillQuery(fsSigQueryTempl,paramList);
                    if(!paramList.isEmpty())
                    {
                        AINGTAICDParameter param = paramList.first();
                        _ResolvedParamList.insert(sig,param);
                    }
                }
                else
                {
                    QString appQueryStr = "APPLICATION ='"+app+"'";
                    QString mediaQueryStr = "AND MEDIA ='"+media+"'";
                    QString busQueryStr = " AND BUSNAME ='"+busName+"'";
                    QString messageQueryStr = " AND MESSAGE ='"+messageName+"'";
                    QString fsNameQueryStr = " AND FS_NAME ='"+fsName+"'";
                    QString equipmentQueryStr  = " AND EQUIPNAME ='"+equipName+"'";
                    QString whereClause = QString("%1 %2 %3 %4 %5 %6").arg(appQueryStr,mediaQueryStr,busQueryStr,messageQueryStr,fsNameQueryStr,equipmentQueryStr);
                    QString fsSigQueryTempl = QString("SELECT * FROM %1 WHERE %2 LIMIT 1;").arg(PARAMETER_TABLE,whereClause);
                    QList<AINGTAICDParameter> paramList;
                    AINGTALocalDbUtil::fillQuery(fsSigQueryTempl,paramList);
                    if(!paramList.isEmpty())
                    {
                        AINGTAICDParameter param = paramList.first();
                        _ResolvedParamList.insert(sig,param);
                    }
                }

            }
        }
    }
    else
    {
        if(items.count() == 4)
        {
            equipName = items.at(0);
            QString medDir = items.at(1);
            QStringList items1 = medDir.split("_");
            direction = items1.last();
            media = items1.first();
            busName = items.at(2);
            signalName = items.at(3);

            //failsafe to check query doesn't fail
            app.replace("'","");
            equipName.replace("'","");
            medDir.replace("'","");
            direction.replace("'","");
            media.replace("'","");
            busName.replace("'","");
            signalName.replace("'","");

            QString triplet = QString("%1.%2.%3").arg(app,busName,signalName);
            QString fsSigQueryTempl = QString("SELECT * FROM %1 WHERE NAME='%2';").arg(PARAMETER_TABLE,triplet);
            QList<AINGTAICDParameter> paramList;
            AINGTALocalDbUtil::fillQuery(fsSigQueryTempl,paramList);
            if(!paramList.isEmpty())
            {
                AINGTAICDParameter param = paramList.first();
                _ResolvedParamList.insert(sig,param);
                QString fsSignal;
                if(media == "AFDX")
                {
                    fsSignal = QString("%1.%2.%3.%4.%5.FS").arg(equipName,medDir,busName,param.getMessageName(),param.getFSName());
                }
                else if(media == "A429")
                {
                    fsSignal = QString("%1.%2.%3.%4.%5.SSM").arg(equipName,medDir,busName,param.getMessageName(),param.getFSName());
                    QString sdiSignal= QString("%1.%2.%3.%4.%5.SDI").arg(equipName,medDir,busName,param.getMessageName(),param.getFSName());
                    if(!_ResolvedParamList.contains(sdiSignal))
                        _ResolvedParamList.insert(sdiSignal,param);
                }
                if(!_ResolvedParamList.contains(fsSignal))
                    _ResolvedParamList.insert(fsSignal,param);
            }
        }
        else if(items.count() == 3)
        {
            equipName = items.at(0);
            QString medDir = items.at(1);
            QStringList items1 = medDir.split("_");
            direction = items1.last();
            media = items1.first();
            signalName = items.at(2);

            //failsafe to check query doesn't fail
            app.replace("'","");
            equipName.replace("'","");
            medDir.replace("'","");
            direction.replace("'","");
            media.replace("'","");
            signalName.replace("'","");

            if(media.trimmed().toUpper() == "DISCRETE")
                media = "DIS";
            else if((media.trimmed().toUpper() == "ANALOGUE") ||(media.trimmed().toUpper() == "ANALOG"))
                media = "ANA";
            else
            {

            }
            QString appQueryStr = "APPLICATION ='"+app+"'";
            QString mediaQueryStr = "AND MEDIA ='"+media+"'";
            QString equipmentQueryStr  = " AND EQUIPNAME ='"+equipName+"'";
                        QString signalQueryStr = " AND NAME LIKE '%"+signalName+"'";
            QString whereClause = QString("%1 %2 %3 %4").arg(appQueryStr,mediaQueryStr,equipmentQueryStr,signalQueryStr);
            QString fsSigQueryTempl = QString("SELECT * FROM %1 WHERE %2 LIMIT 1;").arg(PARAMETER_TABLE,whereClause);

            QList<AINGTAICDParameter> paramList;
            AINGTALocalDbUtil::fillQuery(fsSigQueryTempl,paramList);
            if(!paramList.isEmpty())
            {
                AINGTAICDParameter param = paramList.first();
                _ResolvedParamList.insert(sig,param);
                QString fsSignal;
                if(media == "AFDX")
                {
                    fsSignal = QString("%1.%2.%3.%4.%5.FS").arg(equipName,medDir,busName,param.getMessageName(),param.getFSName());
                }
                else if(media == "A429")
                {
                    fsSignal = QString("%1.%2.%3.%4.%5.SSM").arg(equipName,medDir,busName,param.getMessageName(),param.getFSName());
                    QString sdiSignal= QString("%1.%2.%3.%4.%5.SDI").arg(equipName,medDir,busName,param.getMessageName(),param.getFSName());
                    if(!_ResolvedParamList.contains(sdiSignal))
                        _ResolvedParamList.insert(sdiSignal,param);
                }
                if(!_ResolvedParamList.contains(fsSignal))
                    _ResolvedParamList.insert(fsSignal,param);
            }
        }
    }

}

QString AINGTAFSIIFileParser::resolveOpaqueSignals(QString &ival)
{
    if (ival.startsWith("{") && ival.endsWith("}") && ival.contains(","))
    {
        ival.remove("{").remove("}");
        QStringList vals = ival.split(",");
        QString value;
        //an opaque value {0x7f,0xff,0xff,0x00} causes the bridge to crash but {0x55,0x55,0x55} works fine
        for (int i=0;i<(vals.count()<4? vals.count():3);i++)
        {
            QString tempval = vals.at(i);
            value = value.append(tempval.split("x").last());
        }
        return value;
    }
    else
        return ival;
}

void AINGTAFSIIFileParser::resolveFSValue(const AINGTAICDParameter &iParam, const QString &iUserFS, QString &oFS)
{
    if(iParam.getMedia() == "AFDX")
    {
        if(iUserFS == "0x00")
        {
            oFS =  QString("ND");
        }
        else if(iUserFS == "0x30")
        {
            oFS =  QString("NCD");
        }
        else if(iUserFS == "0x0C")
        {
            oFS =  QString("FT");
        }
        else if(iUserFS == "0x03")
        {
            oFS =  QString("NO");
        }
        else
        {
            QString msg  = QString("%1 %2 has invalid functional status as %3").arg(ERR_PLUGIN,iParam.getName(),iUserFS);
            _ErrorMessages.append(msg);
        }
    }
    else if(iParam.getMedia() == "A429")
    {
        if(iParam.getMediaType().toUpper() == "BNR")
        {
            /*if(iUserFS == "0x00")*/
            if(iUserFS == "0")
            {
                oFS =  QString("FW");
            }
            /*else if(iUserFS == "0x01")*/
            else if(iUserFS == "1")
            {

                oFS =  QString("NCD");
            }
            /*else if(iUserFS == "0x02")*/
            else if(iUserFS == "2")
            {
                oFS =  QString("FT");
            }
            /*else if(iUserFS == "0x03")*/
            else if(iUserFS == "3")
            {
                oFS =  QString("NO");
            }
            else
            {
                QString msg  = QString("%1 %2 has invalid functional status as %3").arg(ERR_PLUGIN,iParam.getName(),iUserFS);
                _ErrorMessages.append(msg);
            }
        }
        else if(iParam.getMediaType().toUpper() == "BCD")
        {
            /*if(iUserFS == "0x00")*/
            if(iUserFS == "0")
            {
                oFS =  QString("Plus");
            }
            /*else if(iUserFS == "0x01")*/
            else if(iUserFS == "1")
            {

                oFS =  QString("NCD");
            }
            /*else if(iUserFS == "0x02")*/
            else if(iUserFS == "2")
            {
                oFS =  QString("FT");
            }
            /*else if(iUserFS == "0x03")*/
            else if(iUserFS == "3")
            {
                oFS =  QString("Minus");
            }
            else
            {
                QString msg  = QString("%1 %2 has invalid functional status as %3").arg(ERR_PLUGIN,iParam.getName(),iUserFS);
                _ErrorMessages.append(msg);
            }
        }
        else if(iParam.getMediaType().toUpper() == "DISCRETE")
        {
            /*if(iUserFS == "0x00")*/
            if(iUserFS == "0")
            {
                oFS =  QString("NO");
            }
            /*else if(iUserFS == "0x01")*/
            else if(iUserFS == "1")
            {

                oFS =  QString("NCD");
            }
            /*else if(iUserFS == "0x02")*/
            else if(iUserFS == "2")
            {
                oFS =  QString("FT");
            }
            /*else if(iUserFS == "0x03")*/
            else if(iUserFS == "3")
            {
                oFS =  QString("FW");
            }
            else
            {
                QString msg  = QString("%1 %2 has invalid functional status as %3").arg(ERR_PLUGIN,iParam.getName(),iUserFS);
                _ErrorMessages.append(msg);
            }
        }
    }
}
