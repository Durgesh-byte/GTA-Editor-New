#include "AINGTAAutoTRFileParser.h"
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
#include <QSqlQuery>
#include "AINGTAActionTitle.h"
#include "AINGTAActionPrintTable.h"
#include "AINGTAInvalidCommand.h"

AINGTAAutoTRFileParser::AINGTAAutoTRFileParser()
{
    _ErrorMessages.clear();
}
AINGTAAutoTRFileParser::~AINGTAAutoTRFileParser()
{
}

bool AINGTAAutoTRFileParser::parseAndSaveFile(const QFileInfo &iAutoTRFile)
{
   
    bool rc = false;
	
    QString filePath = iAutoTRFile.absoluteFilePath();
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly))
    {
        QString msg  = QString("%1 Could not open %2").arg(ERR_PLUGIN,filePath);
        _ErrorMessages.append(msg);
        return false;
    }
    QString fileName = iAutoTRFile.fileName();
    _CurrentDir = fileName.replace(",","_").replace("&","").replace(".atr","").replace("-","_").replace("  "," ").replace(" ","_").replace(".","_").replace("__","_");

    _ElementDirPath = QString(QDir::cleanPath("%1/%2").arg(_ExportPath,_CurrentDir));

    QDir dir(_ElementDirPath);
    if(!dir.exists(_ElementDirPath))
        dir.mkdir(_ElementDirPath);


    QTextStream in(&file);

    QString startOfHeader = "false";
    QString startOfTest = "false";
    QString startOfIC = "false";

    int callCntr = 0;
    QString callElemName;
    QString actualFuncName;
    AINGTAHeader* header = new AINGTAHeader;
    header->setName(_CurrentDir) ;
    QList<AINGTACommandBase *> pCmdList;
    QList<AINGTACommandBase *> pCallCmdList;
    QString startOfCallCmd = "false";
    QString prevStartOfCallCmd = "false";
    QString headerDesc= "";
    QString hasDesc = "false";
    AINGTACommandBase *pCallCmd = NULL;
    while(!in.atEnd())
    {
        QString line = in.readLine().trimmed();

        if(line.trimmed().startsWith(AUTOTR_CMD_TEXT))
        {
            if((line.contains("<<<")) && (startOfCallCmd == "false"))
            {
                startOfCallCmd = "true";
                prevStartOfCallCmd ="false";
                QString text = line;
                text.replace("\"","");
                text.replace(AUTOTR_CMD_TEXT,"");
                text.replace("<<<","");
                text.replace("start function","");
                actualFuncName = text;

                callElemName = QString("Function_%1").arg(QString::number(callCntr++));

                AINGTACommandBase *pCmd = NULL;
                rc = makeCallCommand(callElemName,pCmd);
                if(pCmd != NULL)
                {
                    pCmd->setComment(line);
                }
                pCallCmd = pCmd;
            }
            else if(line.contains(">>>"))
            {
                startOfCallCmd = "false";
                prevStartOfCallCmd ="true";
                if(!pCallCmdList.isEmpty())
                {
                    AINGTAHeader* header1 = new AINGTAHeader;
                    header1->setName(callElemName) ;
                    header1->insertDefaultHeaderNodes();
                    header1->setDescription(actualFuncName);
                    header1->insertDefaultHeaderNodes();
                    header1->editField("DESCRIPTION",actualFuncName);

                    AINGTAElement *callElem = new AINGTAElement(AINGTAElement::FUNCTION,pCallCmdList);
                    callElem->setHeader(header1);
                    callElem->setName(callElemName);
                    QString completeFilePath = QDir::cleanPath(QString("%1/%2.fun").arg(_ElementDirPath,_CurrentDir));
                    callElem->setAbsoluteFilePath(completeFilePath);
                    //the AutoTR file is parsed line by line and thus making it difficult to check if
                    //there is a function present after the current line being parsed from the file
                    //to handle this, a map is updated when a call command is created if the function is
                    //not already found in the map. The same uuid must be assigned to the function when it is created.
                    QString uuid = callElem->getUuid();
                    if (_FunctionUUIDMap.keys().contains(callElemName))
                    {
                        uuid = _FunctionUUIDMap.value(callElemName);
                        callElem->setUuid(uuid);
                    }
                    else
                    {
                        _FunctionUUIDMap.insert(callElemName,uuid);
                    }
                    emit saveImportElement(callElem,completeFilePath);
                    callElemName = "";
                    actualFuncName = "";
                    pCallCmdList.clear();
                    if(pCallCmd != NULL)
                        pCmdList.append(pCallCmd);
                }
            }
        }

        if(startOfHeader == "true")
        {
            if(line.contains(":"))
            {
                QStringList items = line.split(":");
                QString fieldName = items.first().trimmed().toUpper();
                items.removeAt(0);
                QString fieldVal = items.join(":");
                if(fieldName.toUpper().contains("DESCRIPTION"))
                {
                    hasDesc = "true";
                    header->editField("DESCRIPTION",fieldVal);
                }
                header->addHeaderItem(fieldName,true,fieldVal);
            }
            else
            {
                if(!(line.contains(AUTOTR_TAG_BEGIN_TEXT)  || line.contains(AUTOTR_TAG_END_TEXT)))
                    headerDesc.append(line);
            }
            header->insertDefaultHeaderNodes();
        }
        if("true" == startOfIC)
        {
            AINGTACommandBase *pCmd = NULL;
            bool rc = makeCommandFromText(line,pCmd);
            if(pCmd != NULL)
            {
                pCmd->setComment(line);
                if(startOfCallCmd == "true")
                    pCallCmdList.append(pCmd);
                else
                    pCmdList.append(pCmd);
            }
        }
        if("true" == startOfTest)
        {
            AINGTACommandBase *pCmd = NULL;
            bool rc = makeCommandFromText(line,pCmd);
            if(pCmd != NULL)
            {
                pCmd->setComment(line);
                if(startOfCallCmd == "true")
                {
                    if(prevStartOfCallCmd == "false")
                    {
                        prevStartOfCallCmd = "true";
                        pCmdList.append(pCmd);
                    }
                    else
                        pCallCmdList.append(pCmd);
                }
                else
                    pCmdList.append(pCmd);
            }
        }


        if(line.startsWith(AUTOTR_TAG_BEGIN_TEXT))
            startOfHeader = "true";
        if(line.startsWith(AUTOTR_TAG_END_TEXT))
        {
            startOfHeader = "false";
            if(!headerDesc.isEmpty() && (hasDesc == "false"))
                header->editField("DESCRIPTION",headerDesc);
        }

        if(line.startsWith(AUTOTR_TAG_BEGIN_TEST))
        {
            AINGTACommandBase *pCmd = NULL;
            rc = makeTitleCommand("Start Of Test", pCmd);
            if(pCmd != NULL)
            {
                pCmd->setComment(line);
                if(startOfCallCmd == "true")
                    pCallCmdList.append(pCmd);
                else
                    pCmdList.append(pCmd);
            }
            startOfTest = "true";
        }
        if(line.startsWith(AUTOTR_TAG_END_TEST))
        {
            AINGTACommandBase *pCmd = NULL;
            rc = makeTitleCommand("End Of Test", pCmd);
            if(pCmd != NULL)
            {
                pCmd->setComment(line);
                if(startOfCallCmd == "true")
                    pCallCmdList.append(pCmd);
                else
                    pCmdList.append(pCmd);
            }
            startOfTest = "false";
        }
        if(line.startsWith(AUTOTR_TAG_BEGIN_IC))
        {
            AINGTACommandBase *pCmd = NULL;
            rc = makeTitleCommand("Start Of Initial Conditions", pCmd);
            if(pCmd != NULL)
            {
                pCmd->setComment(line);
                if(startOfCallCmd == "true")
                    pCallCmdList.append(pCmd);
                else
                    pCmdList.append(pCmd);
            }
            startOfIC = "true";
        }
        if(line.startsWith(AUTOTR_TAG_END_IC))
        {
            AINGTACommandBase *pCmd = NULL;
            rc = makeTitleCommand("End of Initial Conditions", pCmd);
            if(pCmd != NULL)
            {
                pCmd->setComment(line);
                if(startOfCallCmd == "true")
                    pCallCmdList.append(pCmd);
                else
                    pCmdList.append(pCmd);
            }
            startOfIC = "false";
        }
        if(line.startsWith(AUTOTR_TAG_LOG))
        {
            AINGTACommandBase *pCmd = NULL;
            rc = makeLogCommand(line,pCmd);
            if(pCmd != NULL)
            {
                pCmd->setComment(line);
                if(startOfCallCmd == "true")
                    pCallCmdList.append(pCmd);
                else
                    pCmdList.append(pCmd);
            }
        }
        else if(line.startsWith(AUTOTR_TAG_PLOT))
        {

        }
    }
    if(!pCmdList.isEmpty())
    {
        AINGTAElement *tpElem = new AINGTAElement(AINGTAElement::PROCEDURE,pCmdList);
        tpElem->setHeader(header);
        tpElem->setName(_CurrentDir);
        QString completeFilePath = QDir::cleanPath(QString("%1/%2.pro").arg(_ElementDirPath,_CurrentDir));
        tpElem->setAbsoluteFilePath(completeFilePath);
        emit saveImportElement(tpElem,completeFilePath);
        pCmdList.clear();
    }
    file.close();
    return rc;
}

QString AINGTAAutoTRFileParser::getCommandType(const QString &iLine) const
{
    QString text = iLine;

    if(text.startsWith("%"))
        text.replace("%","");
    if(text.trimmed().startsWith(AUTOTR_CMD_CHECK))
    {
        return AUTOTR_CMD_CHECK;
    }
    else if(text.trimmed().startsWith(AUTOTR_CMD_SET))
    {
        return AUTOTR_CMD_SET;
    }
    else if(text.trimmed().startsWith(AUTOTR_CMD_WAIT))
    {
        return AUTOTR_CMD_WAIT;
    }
    else if(text.trimmed().startsWith(AUTOTR_CMD_MATLAB_CMD))
    {
        return AUTOTR_CMD_MATLAB_CMD;
    }
    else if(text.trimmed().startsWith(AUTOTR_CMD_OBSERVE))
    {
        return AUTOTR_CMD_OBSERVE;
    }
    else if(text.trimmed().startsWith(AUTOTR_CMD_TEXT))
    {
        return AUTOTR_CMD_TEXT;
    }
    else
    {
        return AUTOTR_CMD_SET;
    }
}

bool AINGTAAutoTRFileParser::makeCommandFromText(const QString &iText, AINGTACommandBase *&oCmd)
{
    bool rc= false;
    QString text = iText.trimmed();
    bool isCmdIgnored = false;
    if(text.startsWith("%"))
    {
        int idx = text.indexOf("%");
        text = text.remove(idx,1);
        text = text.trimmed();
        isCmdIgnored = true;
    }
    QString cmdType = getCommandType(text).trimmed();
    if(cmdType == AUTOTR_CMD_CHECK)
    {
        rc = makeCheckCommand(text,oCmd);
    }
    else if(cmdType == AUTOTR_CMD_SET)
    {
        rc = makeSetCommand(text,oCmd);
    }
    else if(cmdType == AUTOTR_CMD_WAIT)
    {
        rc = makeWaitCommand(text,oCmd);
    }
    else if(cmdType == AUTOTR_CMD_MATLAB_CMD)
    {
        rc = makeMatlabCommand(text,oCmd);
    }
    else if(cmdType == AUTOTR_CMD_OBSERVE)
    {
        rc = makeObserveCommand(text,oCmd);
    }
    else if(cmdType ==  AUTOTR_CMD_TEXT)
    {
        rc = makeTextCommand(text,oCmd);
    }
    else
    {
        QString reason = "Reason: Unknown Command or not handled by GTA.";
        rc = makeInvalidCommand(iText, reason, oCmd);
    }
    if(oCmd != NULL)
    {
        oCmd->setIgnoreInSCXML(isCmdIgnored);
        oCmd->setObjId();
    }
    return rc;
}
bool AINGTAAutoTRFileParser::makeInvalidCommand(const QString &iText, const QString &iReason, AINGTACommandBase *&oCmd)
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
bool AINGTAAutoTRFileParser::makeLogCommand(const QString &iText, AINGTACommandBase *&oCmd)
{
    bool rc = false;
    QString text = iText;
    text.replace(AUTOTR_TAG_LOG,"");
    if(!text.contains(".lsf"))
    {
        QStringList params = text.trimmed().split(" ");
        AINGTAActionPrintTable *pPrintTable = new AINGTAActionPrintTable();
        pPrintTable->setTableName(QString("LogParameters"));
        pPrintTable->setTitleName(QString("LogParameters1"));
        pPrintTable->setValues(params);
        oCmd = pPrintTable;
    }
    return rc;
}

QString AINGTAAutoTRFileParser::getValueType(const QString &iVal)
{
    if(iVal.startsWith("[") && iVal.endsWith("]"))
    {
        if(iVal.contains(",") || iVal.contains(":"))
        {
            return AUTOTR_VAL_TYPE_RANGE;
        }
        else
        {
            return AUTOTR_VAL_TYPE_VECTOR;
        }
    }
    else
    {
        if((iVal.toLower() == "false") || (iVal.toLower() == "true"))
            return AUTOTR_VAL_TYPE_CONST;
        else
        {
            QRegExp enumsRx("([A-Za-z_]+)");
            QRegExp paramsRx("([A-Za-z0-9_]+)");
            QRegExp constRx("([0-9.]+)");//
            if(iVal.contains(enumsRx))
            {
                return AUTOTR_VAL_TYPE_ENUM;
            }
            else if(iVal.contains(paramsRx))
            {
                return AUTOTR_VAL_TYPE_PARAM;
            }
            else if(iVal.contains(constRx))
            {
                return AUTOTR_VAL_TYPE_CONST;
            }
        }
    }
    return AUTOTR_VAL_TYPE_UNKNOWN;
}
bool AINGTAAutoTRFileParser::makeSetCommand(const QString &iText, AINGTACommandBase *&oCmd)
{
    bool rc =false;
    QString text = iText;
    text.replace(AUTOTR_CMD_SET,"");
    if(text.contains(AUTOTR_OPERATOR_EQ))
    {
        QStringList items = text.split(AUTOTR_OPERATOR_EQ);
        QString lhs = items.at(0).trimmed();
        QString rhs = items.at(1).trimmed();

        AINGTAActionSet *pSetCmd = new AINGTAActionSet(ACT_SET_INSTANCE,QString(""));

        pSetCmd->setParameter(lhs.trimmed());
        pSetCmd->setIsSetOnlyFS(false);
        pSetCmd->setFunctionStatus("NO_CHANGE");



        QString valueType = getValueType(rhs);

        QString isSetOnlyFS = "false";
        if((lhs.endsWith("_FS")) || (lhs.endsWith("_SSM")))
            isSetOnlyFS = "true";
        if(isSetOnlyFS == "false")
        {
            if(valueType == AUTOTR_VAL_TYPE_CONST)
            {
                if(rhs.toLower() == "false")
                    rhs="0";
                else if(rhs.toLower() == "true")
                    rhs = "1";

                AINGTAEquationConst *pConst = new AINGTAEquationConst();
                pConst->setConstant(rhs.trimmed());
                AINGTAEquationBase* pBaseEqn = pConst;
                pSetCmd->setEquation(pBaseEqn);
            }
            else if(valueType == AUTOTR_VAL_TYPE_ENUM)
            {
                _EnumsUsed.append(rhs);
                AINGTAEquationConst *pConst = new AINGTAEquationConst();
                pConst->setConstant(rhs.trimmed());
                AINGTAEquationBase* pBaseEqn = pConst;
                pSetCmd->setEquation(pBaseEqn);
            }
            else if(valueType == AUTOTR_VAL_TYPE_PARAM)
            {
                AINGTAEquationConst *pConst = new AINGTAEquationConst();
                pConst->setConstant(rhs.trimmed());
                AINGTAEquationBase* pBaseEqn = pConst;
                pSetCmd->setEquation(pBaseEqn);
            }
            else
            {
                return false;
            }
        }
        else if(isSetOnlyFS == "true")
        {
            if(valueType == AUTOTR_VAL_TYPE_ENUM)
            {
                pSetCmd->setFunctionStatus(rhs.trimmed());
                pSetCmd->setIsSetOnlyFS(true);
            }
            else
            {
                return false;
            }
        }
        AINGTAActionSetList *pSetList = new AINGTAActionSetList(ACT_SET,QString(""));
        pSetCmd->setAction(ACT_SET_INSTANCE);
        pSetList->addSetAction(pSetCmd);
        pSetList->setTimeOut("3","sec");
        pSetList->setAction(ACT_SET);
        oCmd = pSetList;
    }
    return rc;
}
bool AINGTAAutoTRFileParser::makeMatlabCommand(const QString &iText, AINGTACommandBase *&oCmd)
{
    bool rc = false;
    QString text = iText.trimmed();
    text.replace(AUTOTR_CMD_MATLAB_CMD,"");

    AINGTAActionManual *pManAct = new AINGTAActionManual();
    pManAct->setAcknowledgement(true);
    pManAct->setMessage(QString("Perform the following MATLAB Operation:\n%1").arg(text));
    pManAct->setAction(ACT_MANUAL);
    pManAct->setComplement("");
    oCmd = pManAct;
    rc = true;
    return rc;
}
bool AINGTAAutoTRFileParser::makeCheckCommand(const QString &iText, AINGTACommandBase *&oCmd)
{
    bool rc =false;
    AINGTACheckValue *pChkValCmd = new AINGTACheckValue();
    QString text = iText.trimmed();
    text.replace(AUTOTR_CMD_CHECK,"");

    QString checkCmdText = text;
    QString actOnFail = "continue";
    QString confTime = "0";
    bool isConfTime= false;
    bool isTimeOut = false;
    QString timeOut = "3";
    QString timeoutUnit = "sec";
    QString conftimeUnit = "sec";
    bool isWaitUntilTrue = false;
    if(text.contains(","))
    {
        QStringList items = text.trimmed().split(",",QString::SkipEmptyParts);
        checkCmdText = items.at(0);
        text.replace(checkCmdText,"");
        if(text.contains(AUTOTR_ATTR_CHECK_FATAL))
        {
            actOnFail = "stop";
            text.replace(AUTOTR_ATTR_CHECK_FATAL,"");
        }
        if(text.contains(AUTOTR_ATTR_CHECK_FOR))
        {
            isConfTime = true;
            text = text.replace(AUTOTR_ATTR_CHECK_FOR,"").trimmed();
            confTime = text;
            confTime = confTime.replace(",","").trimmed();
            isWaitUntilTrue = true;
            isTimeOut = true;
            bool isDobuleOk  =false;
            timeOut = QString::number(confTime.toDouble(&isDobuleOk) + 3);
        }
        if(text.contains(AUTOTR_ATTR_CHECK_WITHIN))
        {
            isConfTime = false;
            confTime = "0";
            isTimeOut = true;
            text = text.replace(AUTOTR_ATTR_CHECK_WITHIN,"").trimmed();
            text = text.replace(",","").trimmed();
            timeOut = text;
            isWaitUntilTrue = true;
        }
        pChkValCmd->setTimeOut(timeOut,timeoutUnit);
        pChkValCmd->setConfCheckTime(confTime,conftimeUnit);
        pChkValCmd->setWaitUntil(isWaitUntilTrue);
    }
    else
        pChkValCmd->setWaitUntil(false);

    QString oper;
    if(checkCmdText.contains(AUTOTR_OPERATOR_EQ))
        oper= AUTOTR_OPERATOR_EQ;
    else if(checkCmdText.contains(AUTOTR_OPERATOR_GT))
        oper= AUTOTR_OPERATOR_GT;
    else if(checkCmdText.contains(AUTOTR_OPERATOR_LT))
        oper= AUTOTR_OPERATOR_LT;
    else if(checkCmdText.contains(AUTOTR_OPERATOR_GTEQ))
        oper= AUTOTR_OPERATOR_GTEQ;
    else if(checkCmdText.contains(AUTOTR_OPERATOR_LTEQ))
        oper= AUTOTR_OPERATOR_LTEQ;
    else if(checkCmdText.contains(AUTOTR_OPERATOR_NOTEQ))
        oper= AUTOTR_OPERATOR_NOTEQ;
    else
        oper = AUTOTR_OPERATOR_EQ;

    double dPrecision = 0.0;
    QString precisionType = "value";
    if(checkCmdText.contains(AUTOTR_ATTR_CHECK_TOL))
    {
        QStringList chkItems = checkCmdText.split(AUTOTR_ATTR_CHECK_TOL);
        checkCmdText = chkItems.at(0).trimmed();
        QString precision = chkItems.at(1);
        precision.replace("+-","");
        bool isDoubleOk = false;
        dPrecision = precision.toDouble(&isDoubleOk);
        if(!isDoubleOk)
            dPrecision = 0.0;
        
        if(precision.contains("%"))
        {
            precision.replace("%","");
            precisionType = "%";
        }
    }
    QStringList chkItems = checkCmdText.trimmed().split(oper,QString::SkipEmptyParts);
    if(chkItems.count()>1)
    {
        QString isSetOnlyFS = "false";
        QString lhs = chkItems.at(0).trimmed();
        QString rhs = chkItems.at(1).trimmed();
        QString rhsType = getValueType(rhs);
        if(lhs.endsWith("_FS") || lhs.endsWith("_SSM"))
            isSetOnlyFS = "true";
        if(isSetOnlyFS == "false")
        {
            if((rhsType == AUTOTR_VAL_TYPE_CONST) || (rhsType == AUTOTR_VAL_TYPE_PARAM))
            {
                pChkValCmd->insertParamenter(lhs,oper,rhs,ACT_FSSSM_NOT_LOST,dPrecision,precisionType,true,false);
            }
            else if(rhsType == AUTOTR_VAL_TYPE_ENUM)
            {
                _EnumsUsed.append(rhs);
                pChkValCmd->insertParamenter(lhs,oper,rhs,ACT_FSSSM_NOT_LOST,dPrecision,precisionType,true,false);
            }
            else if(rhsType == AUTOTR_VAL_TYPE_VECTOR)
            {
                rhs.replace("[","");
                rhs.replace("]","");
                rhs.replace(" ",",");
                rhs.replace("#,","#");
                rhs.replace(" ","");
                QStringList items = rhs.split(",",QString::SkipEmptyParts);
                for(int i=0;i<items.count();i++)
                {
                    QString val = items.at(i);
                    if(!val.startsWith("#"))
                        pChkValCmd->insertParamenter(lhs,oper,items.at(i),ACT_FSSSM_NOT_LOST,dPrecision,precisionType,true,false);
                }
            }
            else
                return false;
        }
        else if(isSetOnlyFS == "true")
        {
            if(rhsType == AUTOTR_VAL_TYPE_ENUM)
            {
                pChkValCmd->insertParamenter(lhs,oper,"",rhs.trimmed(),dPrecision,precisionType,false,true);
            }
            else
                return false;
        }
    }

    if(pChkValCmd != NULL)
    {
        pChkValCmd->setActionOnFail(actOnFail);
        oCmd = pChkValCmd;
    }
    rc = true;


    return rc;
}
bool AINGTAAutoTRFileParser::makeWaitCommand(const QString &iText, AINGTACommandBase *&oCmd)
{
    bool rc = false;
    QString text = iText;
    QString actionOnFail = "continue";
    double dPrecision = 0.0;
    QString precisionType = "value";
    text.replace(AUTOTR_CMD_WAIT,"");
    QString waitAttr = "";
    if(text.contains(AUTOTR_ATTR_WAIT_OR))
    {
        QStringList waitItems = text.trimmed().split(AUTOTR_ATTR_WAIT_OR,QString::SkipEmptyParts);
        text = QString(waitItems.at(0)).replace(",","").trimmed();
        waitAttr = waitItems.at(1).trimmed();
    }
    bool isTimeOk = false;
    double waitTime = text.trimmed().toDouble(&isTimeOk);

    if(isTimeOk)
    {
        if(waitAttr.isEmpty())
        {
            AINGTAActionWait *pWaitCmd = new AINGTAActionWait(ACT_WAIT, COM_WAIT_FOR,AINGTAActionWait::FOR);
            QString waitTimeStr = QString::number(waitTime);
            pWaitCmd->setCounter(waitTimeStr);
            pWaitCmd->setAction(ACT_WAIT);
            pWaitCmd->setComplement(COM_WAIT_FOR);
            oCmd = pWaitCmd;
            rc =true;
        }
        else
        {
            if(waitAttr.contains(AUTOTR_ATTR_CHECK_FATAL))
            {
                actionOnFail = "stop";
                waitAttr.replace(AUTOTR_ATTR_CHECK_FATAL,"");
                waitAttr.replace(",","");
            }
            if(waitAttr.contains(AUTOTR_ATTR_CHECK_TOL))
            {
                QStringList splitItems = waitAttr.split(AUTOTR_ATTR_CHECK_TOL,QString::SkipEmptyParts);
                waitAttr = splitItems.at(0).trimmed();
                QString precisionVal = splitItems.at(1).trimmed();
                precisionVal.replace("+-","");
                if(precisionVal.contains("%"))
                {
                    precisionType = "%";
                    precisionVal.replace("%","");
                }
                bool isDoubleOk = false;
                dPrecision = precisionVal.toDouble(&isDoubleOk);
                if(isDoubleOk)
                    dPrecision = 0.0;
            }
            QString oper;
            if(waitAttr.contains(AUTOTR_OPERATOR_EQ))
                oper = AUTOTR_OPERATOR_EQ;
            else if(waitAttr.contains(AUTOTR_OPERATOR_GT))
                oper = AUTOTR_OPERATOR_GT;
            else if(waitAttr.contains(AUTOTR_OPERATOR_LT))
                oper= AUTOTR_OPERATOR_LT;
            else if(waitAttr.contains(AUTOTR_OPERATOR_GTEQ))
                oper= AUTOTR_OPERATOR_GTEQ;
            else if(waitAttr.contains(AUTOTR_OPERATOR_LTEQ))
                oper= AUTOTR_OPERATOR_LTEQ;
            else if(waitAttr.contains(AUTOTR_OPERATOR_NOTEQ))
                oper= AUTOTR_OPERATOR_NOTEQ;
            else
                oper = AUTOTR_OPERATOR_EQ;


            QStringList lhsRhsVals = waitAttr.split(oper);
            QString lhs = lhsRhsVals.at(0).trimmed();
            QString rhsVal = lhsRhsVals.at(1).trimmed();
            QString rhs = rhsVal;
            QString rhsType = getValueType(rhsVal);
            if(rhsType == AUTOTR_VAL_TYPE_ENUM)
            {
                _EnumsUsed.append(rhsVal);
            }
            else if((rhsType == AUTOTR_VAL_TYPE_VECTOR) || (rhsType == AUTOTR_VAL_TYPE_UNKNOWN))
                return false;

            QString isSetOnlyFS= "false";
            if(lhs.endsWith("_FS") || lhs.endsWith("_SSM"))
                isSetOnlyFS = "true";

            AINGTAActionWait *pWaitCmd = new AINGTAActionWait(ACT_WAIT,COM_WAIT_UNTIL,AINGTAActionWait::UNTIL);
            pWaitCmd->setParameter(lhs);
            pWaitCmd->setCondition(oper);
            pWaitCmd->setPrecision(QString::number(dPrecision),precisionType);
            pWaitCmd->setActionOnFail(actionOnFail);
            pWaitCmd->setTimeOut(text,"sec");

            if(isSetOnlyFS == "false")
            {
                pWaitCmd->setValue(rhs);
                pWaitCmd->setIsValueOnly(true);
            }
            else if(isSetOnlyFS == "true")
            {
                if(rhsType == AUTOTR_VAL_TYPE_ENUM)
                {
                    pWaitCmd->setValue("");
                    pWaitCmd->setFunctionalStatus(rhs.trimmed());
                    pWaitCmd->setIsValueOnly(false);
                    pWaitCmd->setIsFsOnly(true);
                }
                else
                    return false;
            }
            oCmd = pWaitCmd;
            rc =true;
        }
    }
    return rc;
}
bool AINGTAAutoTRFileParser::makeTextCommand(const QString &iText, AINGTACommandBase *&oCmd)
{
    bool rc = false;
    QString text = iText.trimmed();
    text.replace(AUTOTR_CMD_TEXT,"");
    text = text.replace("\"","").trimmed();
    if(text.startsWith("<<<") || text.startsWith(">>>"))
    {
        text = text.replace("<<<","").trimmed();
        text = text.replace(">>>","").trimmed();
        rc = makeTitleCommand(text.trimmed(),oCmd);
    }
    else
    {
        AINGTAActionPrint *pPrintCmd = new AINGTAActionPrint();
        pPrintCmd->setValues(QStringList()<<text);
        pPrintCmd->setAction(ACT_PRINT);
        pPrintCmd->setComplement(COM_PRINT_MSG);
        oCmd = pPrintCmd;
        rc = true;
    }
    return rc;
}
bool AINGTAAutoTRFileParser::makeObserveCommand(const QString &iText, AINGTACommandBase *&oCmd)
{
    bool rc = false;
    QString text = iText;
    text.replace(AUTOTR_CMD_TEXT,"");
    AINGTAActionPrint *pPrintCmd = new AINGTAActionPrint();
    pPrintCmd->setValues(QStringList()<<text);
    pPrintCmd->setAction(ACT_PRINT);
    pPrintCmd->setComplement(COM_PRINT_MSG);
    oCmd = pPrintCmd;
    rc = true;
    return rc;
}
bool AINGTAAutoTRFileParser::makeTitleCommand(const QString &iTitleText, AINGTACommandBase *&oCmd)
{
    AINGTAActionTitle *pTitleCmd = new AINGTAActionTitle(ACT_TITLE);
    pTitleCmd->setTitle(iTitleText);
    oCmd = pTitleCmd;
    return true;
}
bool AINGTAAutoTRFileParser::makeCallCommand(const QString &iText, AINGTACommandBase *&oCmd)
{
    bool rc = false;
    QString newName = iText.trimmed();
    AINGTAActionCall *pCallCmd = new AINGTAActionCall();
    QString callElemName = QString("%1/%2").arg(_CurrentDir,newName);
    pCallCmd->setElement(callElemName+".fun");
    pCallCmd->setAction(ACT_CALL);
    pCallCmd->setComplement(COM_CALL_FUNC);
    QString uuid = _FunctionUUIDMap.value(newName);
    if (uuid.isEmpty())
    {
        uuid = QUuid::createUuid().toString();
        _FunctionUUIDMap.insert(newName,uuid);
    }
    pCallCmd->setRefrenceFileUUID(uuid);
    oCmd = pCallCmd;
    rc =true;
    return rc;
}
