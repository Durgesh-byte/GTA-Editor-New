#include "AINGTAAVATARFileParser.h"
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
#include "AINGTAActionManual.h"
#include "AINGTALocalDbUtil.h"
#include "AINGTAInvalidCommand.h"
#include "AINGTAGenericToolCommand.h"
#include "AINGTAGenericFunction.h"
#include <QSqlQuery>
#include <QUuid>

AINGTAAVATARFileParser::AINGTAAVATARFileParser()
{
    _ErrorMessages.clear();
}
AINGTAAVATARFileParser::~AINGTAAVATARFileParser()
{
}
bool AINGTAAVATARFileParser::parseAndSaveFile(const QFileInfo &iAVATARFile)
{

    bool rc = true;
    QString filePath = iAVATARFile.absoluteFilePath();
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly))
    {
        QString msg  = QString("%1 Could not open %2").arg(ERR_PLUGIN,filePath);
        _ErrorMessages.append(msg);
        rc =false;
    }
    QString fileName = iAVATARFile.fileName();
    _CurrentDir = fileName.replace(".txt","").replace("  "," ").replace(" ","_").replace(".","_");

    _ElementDirPath = QString(QDir::cleanPath("%1/%2").arg(_ExportPath,_CurrentDir));


    QDir dir(_ElementDirPath);
    if(!dir.exists(_ElementDirPath))
        dir.mkdir(_ElementDirPath);

    AINGTAHeader* procHeader = new AINGTAHeader;
    QTextStream in(&file);

    QList<AINGTAElement *>pElementList;

    QList<AINGTACommandBase *> pCmdList;
    AINGTAElement::ElemType elemType = AINGTAElement::CUSTOM;
    QString callElemName;
    bool isCallEn = false;
    while(!in.atEnd()) {
        QString line = in.readLine();

        if(isCallEn ==false)
        {
            if(line.contains("="))
            {
                QStringList items = line.trimmed().split("=");
                if(items.count()>1)
                {
                    QString lhs = items.at(0);
                    QString rhs = items.at(1);
                    procHeader->addHeaderItem(lhs,true,rhs);
                }
            }
        }
        if(line.startsWith("{") && line.endsWith("}") )
        {
            isCallEn = true;
            if(!pCmdList.isEmpty())
            {
                QString newName = callElemName;
                newName.replace("-","_");
                newName.replace(".","_");
                newName.replace("  "," ");
                newName.replace(" ","_");
                AINGTAElement * pElement = new AINGTAElement(elemType,pCmdList);
                pElement->setName(newName);
                //QString completeFilePath = QString(QDir::cleanPath("%1/%2")).arg(_ElementDirPath,callElemName);
                //using the modified name for name of the procedure and functions
                //done to synchronize calls and procedure name in editor and actual
                QString completeFilePath = QString(QDir::cleanPath("%1/%2")).arg(_ElementDirPath,newName);
                pElement->setAbsoluteFilePath(completeFilePath);
                if(elemType == AINGTAElement::PROCEDURE)
                {
                    
                    procHeader->setName(_CurrentDir) ;
                    procHeader->setDescription(callElemName);
                    procHeader->insertDefaultHeaderNodes();
                    procHeader->editField("DESCRIPTION",callElemName);
                    pElement->setHeader(procHeader);
                    completeFilePath.append(".pro");
                    _ProcedureUUIDMap.insert(newName,pElement->getUuid());
                }
                else if(elemType == AINGTAElement::FUNCTION)
                {
                    AINGTAHeader* header = new AINGTAHeader;
                    header->setName(_CurrentDir) ;
                    header->setDescription(callElemName);
                    header->insertDefaultHeaderNodes();
                    header->editField("DESCRIPTION",callElemName);
                    pElement->setHeader(header);
                    completeFilePath.append(".fun");

                    //the avatar file is parsed line by line and thus making it difficult to check if
                    //there is a function present after the current line being parsed from the file
                    //to handle this, a map is updated when a call command is created if the function is
                    //not already found in the map. The same uuid must be assigned to the function when it is created.
                    QString uuid = pElement->getUuid();
                    if (_FunctionUUIDMap.keys().contains(newName))
                    {
                        uuid = _FunctionUUIDMap.value(newName);
                        pElement->setUuid(uuid);
                    }
                    else
                    {
                        _FunctionUUIDMap.insert(newName,uuid);
                    }
                }
                pElementList.append(pElement);
                emit saveImportElement(pElement,completeFilePath);
                pCmdList.clear();
            }
            callElemName = line.replace("{","").replace("}","");
            if(callElemName.startsWith("MACRO_"))
            {
                elemType = AINGTAElement::FUNCTION;
                callElemName.replace("MACRO_","");
            }
            else
            {
                elemType = AINGTAElement::PROCEDURE;
            }
        }
        else if(isCallEn)
        {
            AINGTACommandBase *pCmd = NULL;
            QString newLine = line;
            if(newLine.contains("//"))
            {
                newLine = newLine.split("//").first();
            }
            bool rc= makeCommandFromText(newLine.trimmed(),pCmd);
            if(pCmd != NULL)
            {
                pCmd->setComment(line);
                pCmdList.append(pCmd);
            }
        }
    }

    //creating a sequence for all procedures imported from avatar
    if(!pCmdList.isEmpty())
    {
        QString newName = callElemName;
        newName.replace("-","_");
        newName.replace(".","_");
        newName.replace("  "," ");
        newName.replace(" ","_");
        AINGTAElement * pElement = new AINGTAElement(elemType,pCmdList);
        pElement->setName(newName);
        //QString completeFilePath = QString(QDir::cleanPath("%1/%2")).arg(_ElementDirPath,callElemName);
        //using the modified name for name of the procedure and functions
        //done to synchronize calls and procedure name in editor and actual
        QString completeFilePath = QString(QDir::cleanPath("%1/%2")).arg(_ElementDirPath,newName);
        pElement->setAbsoluteFilePath(completeFilePath);
        if(elemType == AINGTAElement::PROCEDURE)
        {
            procHeader->setName(_CurrentDir) ;
            procHeader->setDescription(callElemName);
            procHeader->insertDefaultHeaderNodes();
            procHeader->editField("DESCRIPTION",callElemName);
            pElement->setHeader(procHeader);
            completeFilePath.append(".pro");
            _ProcedureUUIDMap.insert(newName,pElement->getUuid());
        }
        else if(elemType == AINGTAElement::FUNCTION)
        {
            AINGTAHeader* header = new AINGTAHeader;
            header->setName(_CurrentDir) ;
            header->setDescription(callElemName);
            header->insertDefaultHeaderNodes();
            header->editField("DESCRIPTION",callElemName);
            pElement->setHeader(header);
            completeFilePath.append(".fun");

            //the avatar file is parsed line by line and thus making it difficult to check if
            //there is a function present after the current line being parsed from the file
            //to handle this, a map is updated when a call command is created if the function is
            //not already found in the map. The same uuid must be assigned to the function when it is created.
            QString uuid = pElement->getUuid();
            if (_FunctionUUIDMap.keys().contains(newName))
            {
                uuid = _FunctionUUIDMap.value(newName);
                pElement->setUuid(uuid);
            }
            else
            {
                _FunctionUUIDMap.insert(newName,uuid);
            }
        }
        pElementList.append(pElement);
        emit saveImportElement(pElement,completeFilePath);
        pCmdList.clear();
    }
    if (!pElementList.isEmpty())
    {
        pCmdList.clear();
        for (int i=0;i<pElementList.count();i++)
        {
            if (pElementList.at(i)->getElementType() == AINGTAElement::PROCEDURE)
            {
                AINGTACommandBase *pCmd = NULL;
                QString newLine = QString("MACRO:%1").arg(pElementList.at(i)->getName());
                bool rc= makeCommandFromText(newLine.trimmed(),pCmd);
                if(pCmd != NULL)
                {
                    pCmd->setComment(newLine);
                    pCmdList.append(pCmd);
                }
            }
        }

        AINGTAElement * pElement = new AINGTAElement(AINGTAElement::SEQUENCE,pCmdList);
        AINGTAHeader* header = new AINGTAHeader;
        header->setName(_CurrentDir) ;
        header->setDescription(QString("Automatic Sequence generated for all procedures from AVATAR file"));
        header->insertDefaultHeaderNodes();
        header->editField("DESCRIPTION",QString("Automatic Sequence generated for all procedures from AVATAR file"));
        pElement->setHeader(header);

        pElement->setName(QString("main"));
        QString completeFilePath = QString(QDir::cleanPath("%1/%2.seq")).arg(_ElementDirPath,QString("main"));
        pElement->setAbsoluteFilePath(completeFilePath);
        emit saveImportElement(pElement,completeFilePath);
    }
    file.close();
    return rc;
}

QString AINGTAAVATARFileParser::getCommandType(const QString &iLine) const
{
    if(iLine.startsWith(AVATAR_CMD_ACK))
    {
        return AVATAR_CMD_ACK;
    }
    else if(iLine.startsWith(AVATAR_CMD_CALL))
    {
        return AVATAR_CMD_CALL;
    }
    else if(iLine.startsWith(AVATAR_CMD_MSG))
    {
        return AVATAR_CMD_MSG;
    }
    else if(iLine.startsWith(AVATAR_CMD_WAIT_FOR))
    {
        return AVATAR_CMD_WAIT_FOR;
    }
    else if(iLine.startsWith(AVATAR_CMD_CHK))
    {
        return AVATAR_CMD_CHK;
    }
    else if(iLine.startsWith(AVATAR_CMD_VER))
    {
        return AVATAR_CMD_VER;
    }
    else if(!iLine.contains(":"))
    {
        return AVATAR_CMD_SET;
    }
}

bool AINGTAAVATARFileParser::makeCommandFromText(const QString &iText, AINGTACommandBase *&oCmd)
{
    bool rc= false;

    QString cmdType = getCommandType(iText).trimmed();
    if(cmdType == AVATAR_CMD_ACK)
        rc= makeAckCommand(iText,oCmd);
    else if(cmdType == AVATAR_CMD_CALL)
        rc= makeCallCommand(iText,oCmd);
    else if(cmdType == AVATAR_CMD_CHK)
        rc= makeCheckCommand(iText,oCmd);
    else if(cmdType == AVATAR_CMD_MSG)
        rc= makeMsgCommand(iText,oCmd);
    else if(cmdType == AVATAR_CMD_SET)
        rc= makeSetCommand(iText,oCmd);
    else if(cmdType == AVATAR_CMD_VER)
        rc= makeVerCommand(iText,oCmd);
    else if(cmdType == AVATAR_CMD_WAIT_FOR)
        rc= makeWaitForCommand(iText,oCmd);
    else
    {
        QString reason = "Reason: Unknown Command or not handled by GTA.";
        rc = makeInvalidCommand(iText, reason, oCmd);
    }
    if(oCmd != NULL)
        oCmd->setObjId();
    return rc;
}
bool AINGTAAVATARFileParser::makeInvalidCommand(const QString &iText, const QString &iReason, AINGTACommandBase *&oCmd)
{
    bool rc;
    AINGTAInvalidCommand *pInvalid = new AINGTAInvalidCommand();
    QString message = QString("%1(%2)").arg(iText,iReason);
    _ErrorMessages.append(message);
    pInvalid->setMessage(message);
    oCmd = pInvalid;
    rc = true;
    return rc;
}
bool AINGTAAVATARFileParser::makeSetCommand(const QString &iText, AINGTACommandBase *&oCmd)
{
    bool rc =false;
    bool isSetTempEn = false;
    QString text = iText;
    if(text.contains("="))
    {
        if(text.contains("=/"))
        {
            text.replace("=/","=");
            isSetTempEn = true;
        }
        QStringList items = text.split("=");
        QString lhs = items.at(0).trimmed();
        QString rhs = items.at(1).trimmed();

        bool isSetOnlyFS = false;
        if(lhs.startsWith("SSM_"))
            isSetOnlyFS = true;

        AINGTAActionSet *pSetCmd = new AINGTAActionSet(ACT_SET_INSTANCE,QString(""));

        if (lhs.toLower().endsWith(".resistive",Qt::CaseInsensitive))
        {
            delete pSetCmd; pSetCmd = NULL;
            QString modifiedText = QString("%1Resistive::SET %2;;;%3").arg(AVATAR_CMD_ACK,lhs.remove(".resistive",Qt::CaseInsensitive),rhs);
            rc = makeAckCommand(modifiedText,oCmd);
            /*QString message = QString("Resistive command %1 = %2 was converted to Resistive External Tool").arg(lhs,rhs);
                                _ErrorMessages.append(message);*/
            return rc;
        }

        AINGTAICDParameter param;
        bool bok = resolveParameter(lhs,param);

        if (!bok)
        {
            delete pSetCmd; pSetCmd = NULL;
            QString text = isSetOnlyFS ? QString("SET SSM of %1 to %2").arg(lhs,rhs) : QString("SET value of %1 to %2").arg(lhs,rhs);
            rc = makeCheckCommand(text.prepend(AVATAR_CMD_CHK),oCmd);
            QString message = QString("Data %1 was not found in ICD, SET command was imported as manual action").arg(lhs);
            _ErrorMessages.append(message);
            return rc;
        }
        else
        {
            lhs = param.getName();
            pSetCmd->setParameter(lhs.trimmed());
            pSetCmd->setIsSetOnlyFS(isSetOnlyFS);
            rc = resolveParameterValue(rhs,isSetOnlyFS,param,rhs);



            if(rhs.startsWith("LIST"))
            {
                return false;
            }
            if(rhs.startsWith("RANGE"))
            {
                return false;
            }

            if(!isSetOnlyFS)
            {
                pSetCmd->setFunctionStatus("NO_CHANGE");
                AINGTAEquationConst *pConst = new AINGTAEquationConst();
                pConst->setConstant(rhs.trimmed());
                pConst->setSolidState("1");
                pConst->setContinousState("0");
                AINGTAEquationBase* pBaseEqn = pConst;
                pSetCmd->setEquation(pBaseEqn);
            }
            else
            {
                pSetCmd->setFunctionStatus(rhs);
            }
            AINGTAActionSetList *pSetList = new AINGTAActionSetList(ACT_SET,QString(""));
            pSetCmd->setAction(ACT_SET_INSTANCE);
            pSetList->addSetAction(pSetCmd);
            pSetList->setTimeOut("3","sec");
            pSetList->setActionOnFail("continue");
            pSetList->setAction(ACT_SET);
            oCmd = pSetList;
        }
    }
    return rc;
}
bool AINGTAAVATARFileParser::makeCheckCommand(const QString &iText, AINGTACommandBase *&oCmd)
{
    bool rc = false;
    if(iText.startsWith(AVATAR_CMD_CHK))
    {
        QString text = iText.trimmed();
        text.replace(AVATAR_CMD_CHK,"");
        AINGTAActionManual *pManAct = new AINGTAActionManual();
        pManAct->setAcknowledgement(true);
        pManAct->setMessage(text);
        pManAct->setAction(ACT_MANUAL);
        pManAct->setComplement("");
        oCmd = pManAct;
        rc = true;
    }
    return rc;
}
bool AINGTAAVATARFileParser::makeVerCommand(const QString &iText, AINGTACommandBase *&oCmd)
{
    bool rc =false;
    if(iText.startsWith(AVATAR_CMD_VER))
    {
        QString text = iText.trimmed();
        text.replace(AVATAR_CMD_VER,"");
        QString operVal;
        if(text.contains(ARITH_EQ))
        {
            operVal = ARITH_EQ;
        }
        else if(text.contains(ARITH_GT))
        {
            operVal = ARITH_GT;
        }
        else if(text.contains(ARITH_LT))
        {
            operVal = ARITH_LT;
        }
        else if(text.contains("<>"))
        {
            operVal = "<>";
        }
        QStringList items = text.split(operVal);
        if(items.count()>1)
        {
            QString lhs = items.at(0).trimmed();
            AINGTAICDParameter param;
            bool bok = resolveParameter(lhs,param);
            lhs = param.getName();
            QString rhsVals = items.at(1).trimmed();

            if (!bok)
            {
                QString text = QString("VERIFY the value of %1 %3 %2").arg(lhs,rhsVals,operVal);
                rc = makeCheckCommand(text.prepend(AVATAR_CMD_CHK),oCmd);
                QString message = QString("Data %1 was not found in ICD, VERIFY command was imported as manual action").arg(lhs);
                _ErrorMessages.append(message);
                return rc;
            }

            AINGTACheckValue *pChkValCmd = new AINGTACheckValue();
            pChkValCmd->setTimeOut("3","sec");
            pChkValCmd->setActionOnFail("continue");
            pChkValCmd->setWaitUntil(true);
            if((rhsVals.startsWith("[") && rhsVals.endsWith("]")) && (operVal == ARITH_EQ))
            {
                rhsVals.replace("[","").replace("]","");
                QStringList rangeCheck = rhsVals.split(";");
                pChkValCmd->insertParamenter(lhs,ARITH_GTEQ,rangeCheck.at(0),"",0,"value",true,false);
                pChkValCmd->insertParamenter(lhs,ARITH_LTEQ,rangeCheck.at(1),"",0,"value",true,false);
            }
            else if((rhsVals.startsWith("[") && rhsVals.endsWith("]")) && (operVal == "<>"))
            {
                rhsVals.replace("[","").replace("]","");
                QStringList rangeCheck = rhsVals.split(";");
                pChkValCmd->insertParamenter(lhs,ARITH_LTEQ,rangeCheck.at(0),"",0,"value",true,false);
                pChkValCmd->insertBinaryOperator("OR");
                pChkValCmd->insertParamenter(lhs,ARITH_GTEQ,rangeCheck.at(1),"",0,"value",true,false);
            }
            else
            {
                double precision = 0;

                if(rhsVals.contains("+/-"))
                {
                    QStringList precisionChk = rhsVals.split("+/-");
                    if(precisionChk.count()>1)
                    {
                        bool isDoubleOk = false;
                        QString strPrecision = precisionChk.at(1).trimmed();
                        precision = strPrecision.toDouble(&isDoubleOk);
                        if(!isDoubleOk)
                            precision = 0;
                        rhsVals = precisionChk.at(0).trimmed();
                    }
                }
                if(operVal == "<>")
                    operVal = ARITH_NOTEQ;

                if(!lhs.startsWith("SSM_"))
                {
                    if(rhsVals.toLower() == "false")
                        rhsVals="0";
                    else if(rhsVals.toLower() == "true")
                        rhsVals = "1";
                    if(rhsVals.contains("0x"))
                    {
                        bool isIntOk =false;
                        int val = rhsVals.toInt(&isIntOk,16);
                        rhsVals = QString::number(val);
                    }
                    resolveParameterValue(rhsVals,false,param,rhsVals);
                    pChkValCmd->insertParamenter(lhs,operVal,rhsVals,"",precision,"value",true,false);
                }
                else
                {
                    QString fsVal;
                    AINGTAICDParameter parameter;
                    bok = resolveParameter(lhs,parameter);
                    lhs = parameter.getName();
                    resolveParameterValue(rhsVals,true,parameter,fsVal);
                    if (!bok)
                    {
                        delete pChkValCmd; pChkValCmd = NULL;
                        QString text = QString("VERIFY the value of %1 %3 %2").arg(lhs,fsVal,operVal);
                        rc = makeCheckCommand(text.prepend(AVATAR_CMD_CHK),oCmd);
                        QString message = QString("Data %1 was not found in ICD, VERIFY command was imported as manual action").arg(lhs);
                        _ErrorMessages.append(message);
                        return rc;
                    }
                    pChkValCmd->insertParamenter(lhs,operVal,"",fsVal,precision,"value",false,true);
                }
            }
            if(pChkValCmd != NULL)
            {
                oCmd = pChkValCmd;
            }
            rc = true;
        }
    }
    return rc;
}
bool AINGTAAVATARFileParser::makeAckCommand(const QString &iText, AINGTACommandBase *&oCmd)
{
    bool rc = false;
    //convert this to external Tool Command
    if(iText.startsWith(AVATAR_CMD_ACK))
    {
        if(iText.contains("::"))
        {
            QString Text = iText;
            QStringList iTextList = Text.remove(AVATAR_CMD_ACK).split("::");

            //            Example shared by Cahon Giles
            //            ACK:MFD3_Video    ::  GET_IMG         000_VMS_Elec_SYSTEM-STATE-CHECK-ELEC_MFD3
            //            ACK:MFD1_IN       ::  PRESS_BUTTON    LSK12
            //            ACK:MFD3_IN       ::  PRESS_BUTTON    MSK3


            if (iTextList.count() == 2)
            {
                QString ToolName = "";
                QString ToolDisplayName = "";
                //function
                QString functionType = "";
                QString functionDisplay = "";

                //listing multiple arguments in a function if present
                QList<GenericFunctionArguments> argList;
                GenericFunctionArguments genArgs;

                QString ToolID = iTextList.at(0);
                QStringList FunctionArgs = iTextList.at(1).split(" ");
                QString toolID = ToolID;

                AINGTAGenericToolCommand *pGenTool = new AINGTAGenericToolCommand();
                AINGTAGenericFunction pGenFun;
                AINGTAGenericArgument pGenArg;
                //for setting StatusCode and Message in definition
                AINGTAGenericAttribute objSimpleRetType;
                AINGTAGenericAttribute objSimpleRetType1;
                AINGTAGenericParamData objParam;

                QList<AINGTAGenericArgument> Arguments;
                QList<AINGTAGenericAttribute> Attributes;
                QList <AINGTAGenericParamData> Definitions;

                if (FunctionArgs.count() == 2)
                {
                    QString FunctionName = FunctionArgs.at(0);
                    QString FunctionArg = FunctionArgs.at(1);

                    if (ToolID.contains("Video",Qt::CaseInsensitive))
                    {
                        ToolName = "AH MFD Plugins";
                        ToolDisplayName = "MFD Video";

                        if (ToolID.contains("MFD1") || ToolID.contains("MFD3"))
                        {
                            genArgs.HMILabel = "Device"; genArgs.name = "device"; genArgs.ID = "0"; genArgs.UserSelVal = "video0";
                            argList.append(genArgs);
                            genArgs.HMILabel = "ImageFileName"; genArgs.name = "action"; genArgs.ID = "1"; genArgs.UserSelVal = FunctionArg;
                            argList.append(genArgs);
                        }
                        else
                        {
                            genArgs.HMILabel = "Device"; genArgs.name = "device"; genArgs.ID = "0"; genArgs.UserSelVal = "video1";
                            argList.append(genArgs);
                            genArgs.HMILabel = "ImageFileName"; genArgs.name = "action"; genArgs.ID = "1"; genArgs.UserSelVal = FunctionArg;
                            argList.append(genArgs);
                        }
                        ToolID = "MFD_Video";

                        //genArgs.HMILabel = "VideoType"; genArgs.name = "text"; genArgs.ID = "2"; genArgs.UserSelVal = "DAZZLE_DVD_RECORDER";
                        //argList.append(genArgs);

                        functionType = "VideoPlugin";
                        functionDisplay = "Get Image";
                    }
                    //                    else if ((ToolID == "MFD1_IN") || (ToolID == "MFD2_IN"))
                    //                    {
                    //                        ToolName = "AH MFD robot Plugin";
                    //                        ToolDisplayName = "MFD Robot Tool";

                    //                        genArgs.HMILabel = "Button Name"; genArgs.name = "Button_Name"; genArgs.ID = "0"; genArgs.UserSelVal = FunctionArg;
                    //                        argList.append(genArgs);

                    //                        functionType = "PneumaticRobot";
                    //                        functionDisplay = QString("%1 Press Button").arg(toolID.replace("_IN",""));
                    //                    }
                    else if ((ToolID == "MFD3_IN") || (ToolID == "MFD4_IN") || (ToolID == "MFD1_IN") || (ToolID == "MFD2_IN"))
                    {
                        if (ToolID == "MFD1_IN")
                            ToolID = "MFD3_IN";
                        if (ToolID == "MFD2_IN")
                            ToolID = "MFD4_IN";
                        ToolName = "AH robotic ARM";
                        ToolDisplayName = "Robotic Arm Tool";
                        ToolID = "Robotic ARM";

                        functionType = "RoboticArm";

                        if (FunctionName != "grab_little_knob")
                        {
                            genArgs.HMILabel = "Equipment"; genArgs.name = "Equipment"; genArgs.ID = "0"; genArgs.UserSelVal = toolID;//genArgs.UserSelVal = toolID.toLower();
                            argList.append(genArgs);
                            genArgs.HMILabel = "Button Name"; genArgs.name = "Button_Name"; genArgs.ID = "1"; genArgs.UserSelVal = FunctionArg;
                            argList.append(genArgs);
                            functionDisplay = QString("%1 Press Button").arg(toolID.replace("_IN",""));
                        }
                        else
                        {
                            genArgs.HMILabel = "Button Name"; genArgs.name = "Button_Name"; genArgs.ID = "0"; genArgs.UserSelVal = FunctionArg;
                            argList.append(genArgs);
                            functionDisplay = QString("%1 Grab Button").arg(toolID.replace("_IN",""));
                        }
                    }
                    else if (toolID == "Resistive")
                    {
                        //The Resistive command in AVATAR is converted into External Tool Command.
                        //This call is made from SET command. Below is the example of how the SET command is sent to be created as an External Tool Command
                        //QString modifiedText = QString("%1Resistive::SET %2;;;%3").arg(AVATAR_CMD_ACK,lhs,rhs);
                        ToolName = "AH Resistive Plugin";
                        ToolDisplayName = "Resistive";

                        genArgs.HMILabel = "Parameter"; genArgs.name = "Parameter"; genArgs.ID = "0"; genArgs.UserSelVal = FunctionArg.split(";;;").first();
                        argList.append(genArgs);
                        genArgs.HMILabel = "Value"; genArgs.name = "Value"; genArgs.ID = "1"; genArgs.UserSelVal = FunctionArg.split(";;;").last();
                        argList.append(genArgs);

                        ToolID = "Resistive";

                        functionType = "ResistivePlugin";
                        functionDisplay = "SET resistive";
                    }
                    else
                    {
                        // new ACK command which has not been handled yet
                        ToolName = "";
                        ToolDisplayName = "";

                        functionType = "";
                        functionDisplay = "";

                        genArgs.HMILabel = ""; genArgs.name = ""; genArgs.ID = "0"; genArgs.UserSelVal = "";
                        argList.append(genArgs);
                    }

                    //Generic Attribute for function definition
                    QString attrOccurence = "1";
                    QString attrVal = "";
                    QStringList attrVals = attrVal.split(";",QString::SkipEmptyParts);
                    QString returnName = "ResultStruct";
                    QString returnType = "struct";
                    QString attrReturnCode = "true";
                    QString attrTrueCond = "OK";
                    QString attrFalseCond = "";
                    QString attrDesc = "Status code for ResultStruct parameter";
                    QString attrWaitUntil = "yes";
                    objSimpleRetType.setAttributeName(QString("StatusCode"));
                    objSimpleRetType.setAttributeType(QString("int"));
                    objSimpleRetType.setAttributeOccurence(attrOccurence);
                    objSimpleRetType.setAttributeValues(attrVals);
                    objSimpleRetType.setReturnCode(attrReturnCode);
                    objSimpleRetType.setTrueCondition(attrTrueCond);
                    objSimpleRetType.setFalseCondition(attrFalseCond);
                    objSimpleRetType.setAttributeDescription(attrDesc);
                    objSimpleRetType.setWaitUntil(attrWaitUntil);
                    objSimpleRetType1.setAttributeName(QString("Message"));
                    objSimpleRetType1.setAttributeType(QString("string"));
                    objSimpleRetType1.setAttributeOccurence(attrOccurence);
                    objSimpleRetType1.setAttributeValues(attrVals);
                    objSimpleRetType1.setReturnCode(QString(""));
                    objSimpleRetType1.setFalseCondition(attrFalseCond);
                    objSimpleRetType1.setAttributeDescription(QString("Message string"));
                    objSimpleRetType1.setWaitUntil(attrWaitUntil);
                    Attributes.append(objSimpleRetType);
                    Attributes.append(objSimpleRetType1);

                    //setting values to <data> in definition
                    objParam.setParamDataName(returnName);
                    objParam.setParamDataType(returnType);
                    objParam.setParamAttributeList(Attributes);
                    Definitions.append(objParam);

                    // generic arguments for function
                    for (int k=0;k<argList.count();k++)
                    {
                        QString argType = "string";
                        QString argValue = "";
                        QStringList argValuelst = argValue.split(";",QString::SkipEmptyParts);
                        QString val = argList.at(k).UserSelVal;
                        QString indexStr = "999";
                        int index = indexStr.toInt();
                        QString constVals = "";
                        QStringList argConstVals = constVals.split(";",QString::SkipEmptyParts);
                        QString defVals = "";
                        QStringList argDefVals = defVals.split(";",QString::SkipEmptyParts);
                        bool isArgConst = false;
                        QString mandatoryStr = "yes";
                        QString searchType = "";
                        QString searchPath = "";
                        QString condition = "";
                        QString userDbName = "";
                        bool isArgDisplayed =true;
                        pGenArg.setId(argList.at(k).ID.toInt());
                        pGenArg.setArgumentName(argList.at(k).name);
                        pGenArg.setArgumentType(argType);
                        pGenArg.setValues(argValuelst);
                        pGenArg.setUserSelectedValue(val);
                        pGenArg.setUserSelectedHashIndex(index);
                        pGenArg.setHMILabel(argList.at(k).HMILabel);
                        pGenArg.setConstantValue(argConstVals);
                        pGenArg.setDefaultValue(argDefVals);
                        pGenArg.setArgIsConst(isArgConst);
                        pGenArg.setMandatory(mandatoryStr);
                        pGenArg.setSearchType(searchType);
                        pGenArg.setPath(searchPath);
                        pGenArg.setCondition(condition);
                        pGenArg.setIsDisplayed(isArgDisplayed);
                        pGenArg.setUserDbName(userDbName);
                        Arguments.append(pGenArg);
                    }

                    // generic function
                    QString functionName = FunctionName;
                    QString functionToolID = ToolID;
                    bool isRetTypeSimple = false;
                    pGenFun.setIsReturnTypeSimple(isRetTypeSimple);
                    pGenFun.setFunctionName(functionName);
                    pGenFun.setToolType(functionType);
                    pGenFun.setToolID(functionToolID);
                    pGenFun.setDisplayName(functionDisplay);
                    pGenFun.setFunctionArguments(Arguments,true);
                    pGenFun.setAllArguments(Arguments,true);
                    pGenFun.setReturnParamName(returnName);
                    pGenFun.setReturnParamType(returnType);

                    pGenTool->insertFunction(pGenFun);
                    pGenTool->setAction(ACT_GEN_TOOL);
                    pGenTool->setToolName(ToolName);
                    pGenTool->setToolDisplayName(ToolDisplayName);
                    pGenTool->setDefinitions(Definitions);
                    if (ToolDisplayName == "Robotic Arm Tool")
                        pGenTool->setTimeOut(QString("10"),QString("sec"));
                    else
                        pGenTool->setTimeOut(QString("3"),QString("sec"));
                    pGenTool->setConfCheckTime(QString("0"),QString("sec"));
                    //                    pGenTool->setObjId();
                    pGenTool->setMismatch(false);
                    pGenTool->setActionOnFail("continue");
                }
                oCmd = pGenTool;
                rc = true;
            }
        }
        else
        {
            QString text = iText.trimmed();
            text.replace(AVATAR_CMD_ACK,"");
            AINGTAActionManual *pManAct = new AINGTAActionManual();
            pManAct->setAcknowledgement(true);
            pManAct->setMessage(text);
            pManAct->setAction(ACT_MANUAL);
            oCmd = pManAct;
            rc = true;
        }
    }
    return rc;
}
bool AINGTAAVATARFileParser::makeWaitForCommand(const QString &iText, AINGTACommandBase *&oCmd)
{
    bool rc = false;
    if(iText.startsWith(AVATAR_CMD_WAIT_FOR))
    {
        QString text = iText.trimmed();
        text.replace(AVATAR_CMD_WAIT_FOR,"");


        QString timeVal = text.replace(")","");
        QString timeValUnit = "ms";

        bool isTimeOk = false;
        double waitTime = timeVal.trimmed().toDouble(&isTimeOk);
        if(isTimeOk)
        {
            AINGTAActionWait *pWaitCmd = new AINGTAActionWait(ACT_WAIT, COM_WAIT_FOR,AINGTAActionWait::FOR);
            int mf = 1;
            if(timeValUnit == "ms")
                mf = 1000;
            QString waitTimeStr = QString::number(waitTime / mf);
            pWaitCmd->setCounter(waitTimeStr);
            pWaitCmd->setAction(ACT_WAIT);
            pWaitCmd->setComplement(COM_WAIT_FOR);
            oCmd = pWaitCmd;
            rc =true;
        }
    }
    return rc;
}
bool AINGTAAVATARFileParser::makeMsgCommand(const QString &iText, AINGTACommandBase *&oCmd)
{
    bool rc = false;
    if(iText.startsWith(AVATAR_CMD_MSG))
    {
        QString text = iText.trimmed();
        text.replace(AVATAR_CMD_MSG,"");
        AINGTAActionPrint *pPrintCmd = new AINGTAActionPrint();
        pPrintCmd->setValues(QStringList()<<text);
        pPrintCmd->setAction(ACT_PRINT);
        pPrintCmd->setComplement(COM_PRINT_MSG);
        oCmd = pPrintCmd;

        rc =true;
    }
    return rc;
}
bool AINGTAAVATARFileParser::makeCallCommand(const QString &iText, AINGTACommandBase *&oCmd)
{
    bool rc = false;
    if(iText.startsWith(AVATAR_CMD_CALL))
    {
        QString newName = iText.trimmed();
        newName.replace(AVATAR_CMD_CALL,"");
        newName.replace("-","_");
        newName.replace(".","_");
        newName.replace("  "," ");
        newName.replace(" ","_");
        AINGTAActionCall *pCallCmd = new AINGTAActionCall();
        QString callElemName = QString("%1/%2").arg(_CurrentDir,newName);
        pCallCmd->setElement(callElemName+".fun");
        pCallCmd->setAction(ACT_CALL);
        pCallCmd->setComplement(COM_CALL_FUNC);
        QString FuncUuid = _FunctionUUIDMap.value(newName);
        QString ProcUUID = _ProcedureUUIDMap.value(newName);
        if (FuncUuid.isEmpty() && ProcUUID.isEmpty())
        {
            FuncUuid = QUuid::createUuid().toString();
            _FunctionUUIDMap.insert(newName,FuncUuid);
        }
        pCallCmd->setRefrenceFileUUID(FuncUuid);
        if (!ProcUUID.isEmpty())
        {
            pCallCmd->setElement(callElemName+".pro");
            pCallCmd->setComplement(COM_CALL_PROC);
            pCallCmd->setRefrenceFileUUID(ProcUUID);
        }
        oCmd = pCallCmd;
        rc =true;
    }
    return rc;
}
void AINGTAAVATARFileParser::resolveFSValue(const QString &iFSVal, QString &oFSVal)
{
    if(iFSVal.toLower() == "fw")
        oFSVal = iFSVal.toUpper();
}
bool AINGTAAVATARFileParser::resolveParameter(const QString &iParam, AINGTAICDParameter &oResolvedParam)
{
    QString resolvedParam = iParam.trimmed();
    //failsafe to check query doesn't fail
    resolvedParam.replace("'","");

    AINGTAICDParameter param1;
    bool rc = getParameterFromDBNew(resolvedParam,param1);

    if (!rc)
    {
        if(iParam.toLower().endsWith(".resistive",Qt::CaseInsensitive))
        {
            resolvedParam.replace(".Resistive","",Qt::CaseInsensitive);
            /*oResolvedParam.replace(".resistive","");
			oResolvedParam.replace(".RESISTIVE","");*/
        }
        else if(iParam.toLower().endsWith("user",Qt::CaseInsensitive))
        {
            resolvedParam.replace(".USER","",Qt::CaseInsensitive);
            /*oResolvedParam.replace(".User","");
			oResolvedParam.replace(".user","");*/
        }
        //added additionally to identify appropriate signal name in ANALOG and DISCRETE
        else if(iParam.toLower().endsWith(".ANALOG",Qt::CaseInsensitive) || iParam.toLower().endsWith(".DISCRET",Qt::CaseInsensitive))
        {
            resolvedParam.replace(".ANALOG","",Qt::CaseInsensitive);
            resolvedParam.replace(".DISCRET","",Qt::CaseInsensitive);
        }

        //search in DB
        AINGTAICDParameter param;
        bool rc = getParameterFromDB(resolvedParam,param);
        if(rc)
            oResolvedParam = param;
        else
            oResolvedParam.setName(iParam);

        //rc = getParameterFromDB(oResolvedParam,param);
        //oResolvedParam = getParameterFromDB(resolvedParam,param) ? param : oResolvedParam.setName(iParam);
    }
    else
        oResolvedParam = param1;

    return rc;
}

bool AINGTAAVATARFileParser::getParameterFromDBNew(const QString &iParam, AINGTAICDParameter &oDBParam)
{
    bool rc = false;
    QString whereClause;
    QString oResolvedParam = iParam;

    if(iParam.toLower().endsWith(".resistive",Qt::CaseInsensitive))
    {
        oResolvedParam.replace(".resistive","",Qt::CaseInsensitive);
        if (oResolvedParam.split(".").count() == 2)
            whereClause = QString("NAME LIKE '%"+oResolvedParam.split(".").at(0)+"%' AND NAME LIKE '%"+oResolvedParam.split(".").at(1)+"%' AND FILE LIKE '%RESISTIVE%' AND SIGNALTYPE='ANALOGUE' AND EQUIPNAME='RESISTIVE'");
        else
            whereClause = QString("NAME LIKE '%"+oResolvedParam.split(".").at(0)+"%' AND FILE LIKE '%RESISTIVE%' AND SIGNALTYPE='ANALOGUE' AND EQUIPNAME='RESISTIVE'");
    }
    else if(iParam.toLower().endsWith("user",Qt::CaseInsensitive))
    {
        oResolvedParam.replace(".user","",Qt::CaseInsensitive);
        if (oResolvedParam.split(".").count() == 2)
            whereClause = QString("NAME LIKE '%"+oResolvedParam.split(".").at(0)+"%' AND NAME LIKE '%"+oResolvedParam.split(".").at(1)+"%' AND FILE LIKE '%MEMORY%' AND SIGNALTYPE='ANALOGUE' AND EQUIPNAME='MEMORY'");
        else
            whereClause = QString("NAME LIKE '%"+oResolvedParam.split(".").at(0)+"%' AND FILE LIKE '%MEMORY%' AND SIGNALTYPE='ANALOGUE' AND EQUIPNAME='MEMORY'");
    }
    //added additionally to identify appropriate signal name in ANALOG and DISCRETE
    else if(oResolvedParam.toLower().endsWith(".ANALOG",Qt::CaseInsensitive))
    {
        oResolvedParam.replace(".ANALOG","",Qt::CaseInsensitive);
        if (oResolvedParam.split(".").count() == 2)
            whereClause = QString("NAME LIKE '%"+oResolvedParam.split(".").at(0)+"%' AND NAME LIKE '%"+oResolvedParam.split(".").at(1)+"%' AND DIRECTION='OUTPUT' AND SIGNALTYPE='ANALOGUE'");
        else
            whereClause = QString("NAME LIKE '%"+oResolvedParam.split(".").at(0)+"%' AND DIRECTION='OUTPUT' AND SIGNALTYPE='ANALOGUE'");
    }
    else if(iParam.toLower().endsWith(".DISCRET",Qt::CaseInsensitive))
    {
        oResolvedParam.replace(".DISCRET","",Qt::CaseInsensitive);
        if (oResolvedParam.split(".").count() == 2)
            whereClause = QString("NAME LIKE '%"+oResolvedParam.split(".").at(0)+"%' AND NAME LIKE '%"+oResolvedParam.split(".").at(1)+"%' AND DIRECTION='OUTPUT' AND SIGNALTYPE='DISCRETE'");
        else
            whereClause = QString("NAME LIKE '%"+oResolvedParam.split(".").at(0)+"%' AND DIRECTION='OUTPUT' AND SIGNALTYPE='DISCRETE'");
    }
    else
    {
        QStringList items = iParam.split(".");

        if(items.count() == 2)
        {
            whereClause = QString("NAME LIKE '%"+items.at(0)+"%' AND NAME LIKE '%"+items.at(1)+"%' AND DIRECTION='OUTPUT'");
        }
        else  if(items.count() <= 1)
        {
            whereClause = QString("NAME LIKE '%"+iParam+"%' AND DIRECTION='OUTPUT'");
        }
    }

    QString SigQueryTempl = QString("SELECT * FROM %1 WHERE %2 LIMIT 1;").arg(PARAMETER_TABLE,whereClause);
    QList<AINGTAICDParameter> paramList;
    rc = AINGTALocalDbUtil::fillQuery(SigQueryTempl,paramList);
    if(!paramList.isEmpty())
    {
        oDBParam = paramList.first();
        rc= true;
    }
    return rc;
}

bool AINGTAAVATARFileParser::getParameterFromDB(const QString &iParam, AINGTAICDParameter &oDBParam)
{
    bool rc = false;
    QStringList items = iParam.split(".");
    QString whereClause;
    if(items.count() == 2)
    {
        whereClause = QString("NAME LIKE '%"+items.at(0)+"%' AND NAME LIKE '%"+items.at(1)+"%' AND DIRECTION='OUTPUT'");
    }
    else  if(items.count() <= 1)
    {
        whereClause = QString("NAME LIKE '%"+iParam+"%' AND DIRECTION='OUTPUT'");
    }

    QString SigQueryTempl = QString("SELECT * FROM %1 WHERE %2 LIMIT 1;").arg(PARAMETER_TABLE,whereClause);
    QList<AINGTAICDParameter> paramList;
    rc = AINGTALocalDbUtil::fillQuery(SigQueryTempl,paramList);
    if(!rc)
    {
        if(items.count() == 2)
        {
            whereClause = QString("NAME LIKE '%"+items.at(0)+"%' AND NAME LIKE '%"+items.at(1)+"%'");
        }
        else  if(items.count() <= 1)
        {
            whereClause = QString("NAME LIKE '%"+iParam+"%'");
        }
        QString SigQueryTempl = QString("SELECT * FROM %1 WHERE %2 LIMIT 1;").arg(PARAMETER_TABLE,whereClause);
        rc = AINGTALocalDbUtil::fillQuery(SigQueryTempl,paramList);
    }
    if(!paramList.isEmpty())
    {
        oDBParam = paramList.first();
        rc= true;
    }
    return rc;
}


bool AINGTAAVATARFileParser::resolveParameterValue(const QString &iVal, bool isFs, AINGTAICDParameter iParam, QString &oResolvedValue)
{

    bool rc= false;
    QList<AINGTAICDParameterValues> possibleVals;// = iParam.getPossibleValues();
    possibleVals = iParam.getPossibleValueList();
    if(isFs)
    {
        resolveFSValue(iVal,oResolvedValue);
        rc =true;
    }
    else
    {
        AINGTAICDParameterValues temp;
        temp.value = iVal.toLower();
        if(possibleVals.contains(temp) && (iParam.getSignalType().contains(BOOLEAN_TYPE)) || (iParam.getSignalType().contains(ENUMERATE_TYPE)))
        {

            int idx= possibleVals.indexOf(temp);
            if(idx>=0)
            {
                rc =true;
                AINGTAICDParameterValues val = possibleVals.at(idx);
                oResolvedValue = val.key;
            }
            else if((iVal.toLower() == "false") ||(iVal.toLower() == "true"))
            {
                rc =true;
                if(iVal.toLower() == "false")
                    oResolvedValue="0";
                else if(iVal.toLower() == "true")
                    oResolvedValue = "1";
            }
            else
                rc = false;
        }
        else
        {
            rc = true;
            if(iVal.toLower() == "false")
                oResolvedValue="0";
            else if(iVal.toLower() == "true")
                oResolvedValue = "1";
            else if(iVal.contains("0x"))
            {
                bool isIntOk =false;
                int val = iVal.toInt(&isIntOk,16);
                oResolvedValue = QString::number(val);
            }
        }
    }
    return rc;
}
