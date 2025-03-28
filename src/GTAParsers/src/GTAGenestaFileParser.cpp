#include "AINGTAGenestaFileParser.h"
#include "AINGTAUtil.h"
#include "AINGTAActionTitle.h"
#include"AINGTAActionBase.h"
#include"AINGTACheckBase.h"
#include"AINGTAHeader.h"
#include <QFile>
#include <QDir>
#include <QStringList>

#include "AINGTAGenestaActSetDeserializer.h"
#include "AINGTAGenestaActIFDeserializer.h"
#include "AINGTAGenestaChkValueDeserializer.h"
#include "AINGTAGenestaActPrintDeserializer.h"
#include "AINGTAGenestaChkFWCWrngDeserializer.h"
#include "AINGTAGenestaChkFWCProcDeserializer.h"
#include "AINGTAGenestaChkBITEMsgDeserializer.h"
#include "AINGTAGenestaActCallDeserializer.h"
#include "AINGTAGenestaActManualDeserializer.h"
#include "AINGTAGenestaActWaitDeserializer.h"
//#include "AINGTAGenestaActionWaitDeserializer.h"
#include "AINGTAGenestaActReleaseDeserializer.h" 
#include "AINGTAGenestaActWhileDeserializer.h"
#include "AINGTAGenestaActInvalidDeserializer.h"
#include "AINGTAGenestaChkSSMDeserializer.h"
#include "AINGTAGenestaActSetSSMDeserializer.h"
#include "AINGTAActionPrint.h"
#include "AINGTAActionPrintTable.h"
#include "AINGTAInvalidCommand.h"
#include "AINGTACheckFwcWarning.h"
#include "AINGTACheckBiteMessage.h"
QString AINGTAGenestaFileParser::_Aircraft;
QString AINGTAGenestaFileParser::_Equipment;
QStringList AINGTAGenestaFileParser::_fileList;
QString AINGTAGenestaFileParser::_exportFolder;
QMap<QString,QString> AINGTAGenestaFileParser::_mapGenestaSymbols;
AINGTAGenestaFileParser::AINGTAGenestaFileParser(const QString& iFilePath,AINGTAElement::ElemType iElemType,const QStringList& iFileList)
{
    _filePath = QDir::cleanPath(iFilePath);
    QFile fileObj(iFilePath);
    int lastDelimiterOccurence = _filePath.lastIndexOf("/");
    QString ElementFileName = _filePath.mid(lastDelimiterOccurence+1);
    QStringList fileParts = ElementFileName.split(".");
    _fileList = iFileList;
    QString elementName;
    QString elementType;
    for(int i=0;i<fileParts.size();i++)
    {
        if(i==0)
            elementName=fileParts.at(i);
        if(i==1)
            elementType=fileParts.at(i);

    }

    _ElemType = iElemType;


    QDomNode rootNode = _errorDoc.createElement(XNODE_ELEMENT);
    rootNode.toElement().setAttribute(XNODE_NAME,elementType);
    _errorDoc.appendChild(rootNode);
}
AINGTAGenestaFileParser::~AINGTAGenestaFileParser()
{

}
AINGTAElement* AINGTAGenestaFileParser:: getElement()
{
    bool rc = false; 
    AINGTAElement* pElement = NULL;
    if (_filePath.isEmpty())
    {
        appendToLog(0,"No file path provided");
        return pElement;
    }

    QFile genestaFileObj(_filePath);
    rc = genestaFileObj.open(QIODevice::Text| QFile::ReadOnly);
    bool bElementCodeExists = false;
    QStringList lstCodeStatements;
    int lineNo=0;
    if(rc)
    {
        while(!genestaFileObj.atEnd())
        {
            lineNo++;
            QString line = genestaFileObj.readLine();
            if(bElementCodeExists)
            {
                lstCodeStatements.append(line);
            }
            if(  (line.contains(GENESTA_OBJ_BEGIN_IDENTIFIER) || line.contains(GENESTA_PROC_BEGIN_IDENTIFIER)) && bElementCodeExists==false)
            {
                bElementCodeExists=true;
                _codeStartLine=lineNo+1;
                if (line.contains(GENESTA_PROC_BEGIN_IDENTIFIER))
                {
                    _ElemType=AINGTAElement::PROCEDURE;
                }
            }
        }

        genestaFileObj.close();

    }
    else
    {
        appendToLog(0,QString("could not open file,%1").arg(_filePath));
        return pElement;
    }

    if (!lstCodeStatements.isEmpty())
        pElement = getElementFromStatements(lstCodeStatements);
    else
        appendToLog(0,"No Object/procedure Code Found");

    return pElement;
}
AINGTAElement* AINGTAGenestaFileParser::getElementFromStatements(const QStringList& iStringList)
{
    AINGTAElement* pElement = NULL;
    bool rC = false;
    if(!iStringList.isEmpty())
        pElement = new AINGTAElement(_ElemType);
    else
        return NULL;

    int globalPos=0;
    bool isProc = _ElemType==AINGTAElement::PROCEDURE ? true:false;
    for(int i=0;i<iStringList.size();i++)
    {

        QString currentStatement = iStringList.at(i);
        currentStatement = currentStatement.trimmed().remove("\n");
        //AINGTACommandBase* pCmd = NULL;
        QString sError;

        QList<AINGTACommandBase*> lstCmds;
        rC = buildCommandsFromStatement(currentStatement,isProc,lstCmds,sError,i);
        QStringList tmpStatementList = currentStatement.split(GENESTA_DELIMITER);
        bool isSkipCmd = false;
        if(tmpStatementList.count()>7)
            isSkipCmd = (tmpStatementList.at(7) == "SKIP");

        if ((i==1 ) && !lstCmds.isEmpty())
        {
            AINGTACommandBase* pLastCommand = lstCmds.last();
            if (pLastCommand!=NULL && pLastCommand->isTitle())
            {
                AINGTACommandBase* pFirstCommand =NULL;
                pElement->getCommand(0,pFirstCommand);
                if (pFirstCommand!=NULL&& pFirstCommand->isTitle()==false)
                {
                    AINGTAActionTitle* pTitle = new AINGTAActionTitle("Title");
                    pTitle->setAction(ACT_TITLE);
                    pTitle->setTitle("Procedures");
                    lstCmds.insert(0,pTitle);
                }

            }
            
        }
        
        if (!lstCmds.isEmpty())
        {
            foreach(AINGTACommandBase* pCmd,lstCmds)
            {

                pCmd->setIgnoreInSCXML(isSkipCmd);
                pElement->insertCommand(pCmd,globalPos,false);
                processPrintCommand(pElement,pCmd,globalPos);
                
                
                int bChildrens=0;
                bChildrens=pCmd->getAllChildrenCount();
                if (bChildrens)
                    globalPos+=bChildrens+1;//children + the command itself
                else
                    globalPos++;

                
            }
        }
        else
            pElement->insertCommand(NULL,globalPos++,false);


        if (!rC)
        {
            appendToLog(_codeStartLine+i,QString("%1 \n\t\t   Content- %2").arg(sError,currentStatement));
        }

    }
    pElement->editTitleInprintTableForGenestaImport();
    return pElement;

}
//AINGTAElement* AINGTAGenestaFileParser::getElementFromStatements(const QStringList& iStringList)
//{
//    AINGTAElement* pElement = NULL;
//    bool rC = false;
//    if(!iStringList.isEmpty())
//        pElement = new AINGTAElement(_ElemType);
//    else
//        return NULL;
//
//    int globalPos=-1;
//    for(int i=0;i<iStringList.size();i++)
//    {
//        globalPos++;
//        QString currentStatement = iStringList.at(i);
//        AINGTACommandBase* pCmd = NULL;
//        QString sError;
//        if(_ElemType == AINGTAElement::PROCEDURE)
//        {
//            //procedure cannot have special characters without accompanying value
//            bool replacedIdentifier = false;
//            if(currentStatement.contains(GENESTA_CHANNEL_VAR_IDENTIFIER1))
//            {
//                QRegExp rgExp;
//                QString sPattern = QString("\\([%1%2%3]").arg(GENESTA_CHANNEL_VAR_IDENTIFIER1,GENESTA_ENGINE_VAR_IDENTIFIER,GENESTA_VAR1_IDENTIFIER);
//                rgExp.setPattern(sPattern);
//                int dPos = rgExp.indexIn(currentStatement,0);
//                QString sChannelValIdenfier = QString("%1%2").arg(GENESTA_CHANNEL_VAR_IDENTIFIER1,"=");
//                int indexOfValStart = currentStatement.indexOf(sChannelValIdenfier);
//                if (indexOfValStart != -1)
//                {
//                    indexOfValStart =  indexOfValStart + sChannelValIdenfier.size();
//                    int indexOfValend = currentStatement.indexOf(")",indexOfValStart);
//                    QString channelVal = currentStatement.mid(indexOfValStart,indexOfValend-indexOfValStart);
//                    if (!channelVal.isEmpty())
//                    {
//                        currentStatement.replace(GENESTA_CHANNEL_VAR_IDENTIFIER1,channelVal);
//                    }
//
//                    rC = buildCommandsFromStatement(currentStatement,true,pCmd,sError);
//                    if (!rC)
//                    {
//                        appendToLog(_codeStartLine+i,sError);
//                    }
//                    pElement->insertCommand(pCmd,globalPos,false);
//                    if (pCmd!=NULL && pCmd->getChildrens().size())
//                    {
//                        globalPos+=pCmd->getChildrens().size();
//                    }
//                   
//
//                }
//                else
//                {
//                    appendToLog(_codeStartLine+i,"Identifier value not found,in procedure idenfiers should have value with them");
//
//                }
//
//                //currentStatement.replace(GENESTA_CHANNEL_VAR_IDENTIFIER1,GTA_CHANNEL_VARIABLE);
//
//            }
//            else
//            {
//                rC = buildCommandsFromStatement(currentStatement,true,pCmd,sError);
//                if (!rC)
//                {
//                    appendToLog(_codeStartLine+i,sError);
//                }
//                pElement->insertCommand(pCmd,globalPos,false);
//                if (pCmd!=NULL && pCmd->getChildrens().size())
//                {
//                    globalPos+=pCmd->getChildrens().size();
//                }
//               
//
//            }
//
//            
//
//
//        }
//        else if(_ElemType == AINGTAElement::OBJECT)
//        {
//            //all special charachters will be replaced by their @VarName@ equivalent
//            //TODO:all variables are to be identified
//            //TODO: confirmation required can object have
//            if(currentStatement.contains(GENESTA_CHANNEL_VAR_IDENTIFIER1))
//                currentStatement.replace(GENESTA_CHANNEL_VAR_IDENTIFIER1,GTA_CHANNEL_VARIABLE);
//
//            if(currentStatement.contains(GENESTA_ENGINE_VAR_IDENTIFIER))
//                currentStatement.replace(GENESTA_ENGINE_VAR_IDENTIFIER,GTA_ENGINE_VARIABLE);
//
//            if (currentStatement.contains(GENESTA_VAR1_IDENTIFIER)
//            {
//            }
//
//            rC = buildCommandsFromStatement(currentStatement,false,pCmd,sError);
//            if (!rC)
//            {
//                appendToLog(_codeStartLine+i,sError);
//            }
//            pElement->insertCommand(pCmd,globalPos,false);
//
//        }
//
//    }
//    return pElement;
//
//}

AINGTAHeader* AINGTAGenestaFileParser::getHeader()
{
    bool rc = false; 
    AINGTAHeader* pHeader = NULL;
    if (_filePath.isEmpty())
    {
        appendToLog(0,"No file path provided");
        return pHeader;
    }

    QFile genestaFileObj(_filePath);
    rc = genestaFileObj.open(QIODevice::Text| QFile::ReadOnly);
    bool headerCodeEnds = false;
    QStringList lstHeaderStatements;
    int lineNo=0;
    if(rc)
    {


        while(!genestaFileObj.atEnd())
        {
            lineNo++;
            QString line = genestaFileObj.readLine();
            if(headerCodeEnds == false)
            {
                lstHeaderStatements.append(line);
            }
            if(  (line.contains(GENESTA_OBJ_BEGIN_IDENTIFIER) || line.contains(GENESTA_PROC_BEGIN_IDENTIFIER)) && headerCodeEnds==false)
            {
                headerCodeEnds=true;
            }
        }

        genestaFileObj.close();

    }
    else
    {
        appendToLog(0,QString("could not open file,%1").arg(_filePath));
        return pHeader;
    }

    if (!lstHeaderStatements.isEmpty())
        pHeader = getHeadertFromStatements(lstHeaderStatements);
    else
        appendToLog(0,"No Object/procedure Code Found");


    return pHeader;
    return NULL;


}


void AINGTAGenestaFileParser::appendToLog(const int& iOrder,const QString& iErrorStatement, const QString& )
{
    QDomNodeList rootNodeLst = _errorDoc.elementsByTagName(XNODE_ELEMENT);
    if(rootNodeLst.size())
    {
        QDomElement rootElement = rootNodeLst.at(0).toElement();
        if(!rootElement.isNull())
        {
            QDomNode errorNode = _errorDoc.createElement(XNODE_ERROR);
            errorNode.toElement().setAttribute(XNODE_ORDER,QString::number(iOrder));
            errorNode.toElement().setAttribute(XATTR_VAL,iErrorStatement);
            rootElement.appendChild(errorNode);
        }

    }


    _errorLog.insert(iOrder,iErrorStatement);

}

bool AINGTAGenestaFileParser:: buildCommandsFromStatement(const QString& iStatement,const bool& isProc,QList<AINGTACommandBase*>& olstCmd,QString& osError, int )
{

    bool rC = false;
    

    QString sStatement = iStatement;	


    int dInc=0;
    if (isProc)
        dInc =1;
    if (!sStatement.isEmpty())
    {
        QStringList lstColmStatement = sStatement.split(GENESTA_DELIMITER);
        int sizeCols = lstColmStatement.size();
        if (lstColmStatement.size())
        {
            
            if (sizeCols != GENESTA_PROC_COLSIZE && isProc == true )
            {
                if (sizeCols == GENESTA_PROC_COLSIZE-1)
                {
                    sStatement=sStatement.trimmed().append(GENESTA_DELIMITER);
                    lstColmStatement = sStatement.split(GENESTA_DELIMITER);
                    sizeCols = lstColmStatement.size();
                }
                if (sizeCols != GENESTA_PROC_COLSIZE && isProc == true )
                {
                    osError = "size of columns does not match with that of process";
                    AINGTAInvalidCommand* pInvalid = new AINGTAInvalidCommand;
                    pInvalid->setMessage(iStatement);
                    olstCmd.append(pInvalid);
                    return rC;
                }
            }

            if (sizeCols != GENESTA_OBJ_COLSIZE && isProc == false)
            {
                if (sizeCols == GENESTA_OBJ_COLSIZE-1)
                {
                    sStatement=sStatement.trimmed().append(GENESTA_DELIMITER);
                    lstColmStatement = sStatement.split(GENESTA_DELIMITER);
                    sizeCols = lstColmStatement.size();
                }
                if (sizeCols != GENESTA_OBJ_COLSIZE && isProc == false)
                {
                    osError = "size of columns does not match with that of object";
                    AINGTAInvalidCommand* pInvalid = new AINGTAInvalidCommand;
                    pInvalid->setMessage(iStatement);
                    olstCmd.append(pInvalid);
                    return rC;
                }
            }



            QString initEnd     = lstColmStatement.at(0);
            //action
            QString sAction     = lstColmStatement.at(0+dInc);
            //control / check statement
            QString sControl    = lstColmStatement.at(1+dInc);
            //comment
            QString sComment    = lstColmStatement.at(2+dInc);
            QString sTimeOut    = lstColmStatement.at(3+dInc);
            QString sPrecision  = lstColmStatement.at(4+dInc);
            QString sActOnFail  = lstColmStatement.at(5+dInc);
            QString sDumpList   = lstColmStatement.at(6+dInc);


            // In genesta ‡ is equivalent to ":" hence replace in applicable places.
            if(sAction.contains("IPR‡"))
            {
                sAction.replace("IPR‡","");
            }
            if(sControl.contains("IPR‡"))
            {
                sControl.replace("IPR‡","");
            }
            if(sComment.contains("IPR‡"))
            {
                sComment.replace("IPR‡","");
            }
            if(sDumpList.contains("IPR‡"))
            {
                sDumpList.replace("IPR‡","");
            }
            sAction.replace(GENESTA_COLON_IDENTIFIER,GENESTA_DELIMITER);
            sControl.replace(GENESTA_COLON_IDENTIFIER,GENESTA_DELIMITER);
            sComment.replace(GENESTA_COLON_IDENTIFIER,GENESTA_DELIMITER);

            sComment.replace(GENESTA_CHANNEL_VAR_IDENTIFIER1,QString("%1%2%1").arg(QString(TAG_IDENTIFIER),GTA_CHANNEL_VARIABLE));
            sComment.replace(GENESTA_CHANNEL_VAR_IDENTIFIER2,QString("%1%2%1").arg(QString(TAG_IDENTIFIER),GTA_CHANNEL_VARIABLE));
            sComment.replace(GENESTA_ENGINE_VAR_IDENTIFIER1,QString("%1%2%1").arg(QString(TAG_IDENTIFIER),GTA_ENGINE_VARIABLE));
            sComment.replace(GENESTA_ENGINE_VAR_IDENTIFIER2,QString("%1%2%1").arg(QString(TAG_IDENTIFIER),GTA_ENGINE_VARIABLE));
            sComment.replace(GENESTA_ENGINE_VAR_IDENTIFIER3,QString("%1%2%1").arg(QString(TAG_IDENTIFIER),GTA_ENGINE_VARIABLE));
            sComment.replace(GENESTA_VAR1_IDENTIFIER,QString("%1%2%1").arg(QString(TAG_IDENTIFIER),GTA_VAR1_VARIABLE));
            sComment.replace(GENESTA_VAR2_IDENTIFIER,QString("%1%2%1").arg(QString(TAG_IDENTIFIER),GTA_VAR2_VARIABLE));

            sComment.replace("†"," ");
            sComment.remove("add description -");
            if(sComment.startsWith("add comment -"))
                sComment.remove("add comment -");
            sComment.trimmed();

            if (sAction.isEmpty() && sControl.isEmpty() && sComment.isEmpty())
            {
                osError = "Invalid statement, could not identify action/check/comment";
                return rC;
            }

            if (!sAction.isEmpty() && !sControl.isEmpty())
            {
                osError = "Both action and check statements exists";
                AINGTAInvalidCommand* pInvalid = new AINGTAInvalidCommand;
                pInvalid->setMessage(iStatement);
                olstCmd.append(pInvalid);
                return rC;
            }


            if (initEnd.contains("INIT"))
            {
                QString initStatement;
                AINGTAActionTitle* pTitle = new AINGTAActionTitle("Title");
                pTitle->setAction(ACT_TITLE);
                pTitle->setTitle("INITIALIZATION");
                olstCmd.append(pTitle);
                /* AINGTAActionPrint* pPrintMessage = new AINGTAActionPrint(ACT_PRINT,COM_PRINT_MSG);
                pPrintMessage->setValues(QStringList()<<"INITIALIZATION");
                olstCmd.append(pPrintMessage);*/
                rC=true;
                rC=getStatementForInit(sAction,initStatement,sComment,osError);
                if (rC)
                {
                    rC = createCommandsFromActionStatement(initStatement,olstCmd,osError,sPrecision,sTimeOut,_mapGenestaSymbols);
                }
                else
                    osError = "INIT statement not recongnized";
            }
            else if (initEnd.contains("END"))
            {
                QString initStatement;
                AINGTAActionTitle* pTitleEnd = new AINGTAActionTitle("Title");
                pTitleEnd->setAction(ACT_TITLE);
                pTitleEnd->setTitle("END OF PROCEDURE ");
                olstCmd.append(pTitleEnd);
                /*AINGTAActionPrint* pPrintMessage = new AINGTAActionPrint(ACT_PRINT,COM_PRINT_MSG);
                pPrintMessage->setValues(QStringList()<<"END OF PROCEDURE ");
                olstCmd.append(pPrintMessage);*/
                rC=true;
                return true;
            }
            else if (!sAction.isEmpty())
            {

                rC=createCommandsFromActionStatement(sAction,olstCmd,osError,sPrecision,sTimeOut,_mapGenestaSymbols);
                
            }
            else if(!sControl.isEmpty())
            {
                rC=createCommandsFromActionStatement(sControl,olstCmd,osError,sPrecision,sTimeOut,_mapGenestaSymbols);
            }
            
            

            if (!sComment.isEmpty() && sAction.isEmpty() && sControl.isEmpty())
            {
                sComment = sComment.trimmed();
                if (sComment.startsWith(GENESTA_TITLE_IDENTIFIER))
                {
                    AINGTAActionTitle* pTitle = new AINGTAActionTitle(ACT_TITLE);
                    pTitle->setTitle(sComment.remove(GENESTA_TITLE_IDENTIFIER).trimmed());
                    olstCmd.append(pTitle);
                    rC=true;
                }
                else
                {
                    AINGTAActionPrint* pPrintMessage = new AINGTAActionPrint(ACT_PRINT,COM_PRINT_MSG);
                    pPrintMessage->setValues(QStringList()<<sComment);
                    //pPrintMessage->setTitle(sComment.remove("add title -"));
                    olstCmd.append(pPrintMessage);
                    rC=true;
                }
                //pPrintMessage->setComment(sComment);
            }

            int noOfCommands = olstCmd.size();
            if (rC == false && noOfCommands==0)
            {
                AINGTAInvalidCommand * pInvalid = new AINGTAInvalidCommand;
                pInvalid->setMessage("Error while importing");
                sComment = QString("%1\n[genesta comment:%2]").arg(osError,sComment);
                olstCmd.append(pInvalid);
                
            }
            for (int i=0;i<olstCmd.size();i++)
            {
                AINGTACommandBase* opCmd = olstCmd.at(i);
                if (opCmd!=NULL)
                {
                    //AINGTAActionPrintTable* pTable = dynamic_cast<AINGTAActionPrintTable*>(opCmd);
                    //if (!sComment.isEmpty() && opCmd!=NULL)
                    //{
                    //   
                    //    if(pTable)
                    //        pTable->setTitleName(sComment);
                    //    else
                    //        opCmd->setComment(sComment);

                    //}

                    //if (sComment.isEmpty() && pTable!=NULL)
                    //{
                    //    pTable->setTitleName(QString("Print_table_%1").arg(dIndex));
                    //}
                    
                    opCmd->setComment(sComment);
                    if(sTimeOut.isEmpty())
                        sTimeOut="100"; //default timeout will be 100ms for all commands
                    if (!sTimeOut.isEmpty())
                    {
                        bool isNum = false;
                        sTimeOut.remove(" ");
                        double dTimeout = sTimeOut.toDouble(&isNum);

                        if (isNum)
                        {
                            if (opCmd!=NULL)
                            {
                                AINGTAActionBase* pActBase = dynamic_cast<AINGTAActionBase*>(opCmd);
                                AINGTACheckBase * pCheckBase = dynamic_cast<AINGTACheckBase*>(opCmd);
                                if (NULL!=pActBase)
                                {
                                    
                                    if(pActBase->getAction()==ACT_SET)
                                    {
                                        dTimeout=dTimeout+3000;
                                        sTimeOut = QString::number(dTimeout);
                                    }
                                    if(pActBase->hasTimeOut())
                                    {
                                        pActBase->setTimeOut(sTimeOut,ACT_TIMEOUT_UNIT_MSEC);
                                        pActBase->setTimeOutsforAllChildren(dTimeout);
                                    }
                                }
                                else if (NULL!=pCheckBase)
                                {
                                    AINGTACheckFwcWarning *pCheckFwcWarning = dynamic_cast<AINGTACheckFwcWarning*>(pCheckBase);
                                    AINGTACheckBiteMessage *pCheckBiteMessage = dynamic_cast<AINGTACheckBiteMessage*>(pCheckBase);

                                    bool isNum = false;
                                    QString timeoutInSec;
                                    double dTimeoutInMs = sTimeOut.toDouble(&isNum);
                                    if(isNum)
                                    {
                                        timeoutInSec = QString::number(dTimeoutInMs/1000);
                                    }
                                    else
                                    {
                                        timeoutInSec = "0";
                                    }

                                    if(NULL != pCheckFwcWarning)
                                    {
                                        pCheckFwcWarning->setWaitForBuffer(timeoutInSec);
                                        pCheckFwcWarning->setTimeOut(timeoutInSec,ACT_TIMEOUT_UNIT_SEC);
                                    }
                                    else if(NULL != pCheckBiteMessage)
                                    {
                                        pCheckBiteMessage->setBuffDepth(timeoutInSec);
                                        pCheckBiteMessage->setTimeOut(timeoutInSec,ACT_TIMEOUT_UNIT_SEC);

                                    }
                                    else
                                        pCheckBase->setTimeOut(sTimeOut,ACT_TIMEOUT_UNIT_MSEC);
                                }

                            }
                            else
                            {
                                if (osError.isEmpty())
                                    osError = "Time out exists for check and action only";

                                rC=false;
                            } 
                        }
                        else
                        {

                            osError = "Time out should be a numeric";

                            rC=false;

                        }
                    }


                    if (!sPrecision.isEmpty())
                    {
                        bool isNum = false;
                        sPrecision.remove(" ");
                        sPrecision.replace(",",".");
                        sPrecision.toDouble(&isNum);

                        if (isNum)
                        {
                            if (opCmd!=NULL)
                            {
                                AINGTAActionBase* pActBase = dynamic_cast<AINGTAActionBase*>(opCmd);
                                AINGTACheckBase * pCheckBase = dynamic_cast<AINGTACheckBase*>(opCmd);
                                if (NULL!=pActBase && !pActBase->isManuallyAdded())
                                {
                                    pActBase->setPrecision(sPrecision,ACT_PRECISION_UNIT_VALUE);
                                    
                                }
                                else if (NULL!=pCheckBase)
                                {
                                    pCheckBase->setPrecision(sPrecision,ACT_PRECISION_UNIT_VALUE);
                                }

                            }
                            else
                            {
                                if (osError.isEmpty())
                                    osError = "Precision exists for check and action only";
                                rC=false;
                            }

                        }
                        else
                        {
                            osError = "Precision Should be numeric";
                            rC = false;

                        }
                    }


                    if (!sActOnFail.isEmpty())
                    {
                        if (opCmd!=NULL)
                        {
                            sActOnFail.remove(" ");
                            if (sActOnFail.contains(ACT_FAIL_CONTINUE,Qt::CaseInsensitive) || sActOnFail.contains(ACT_FAIL_STOP,Qt::CaseInsensitive))
                            {
                                AINGTAActionBase* pActBase = dynamic_cast<AINGTAActionBase*>(opCmd);
                                AINGTACheckBase * pCheckBase = dynamic_cast<AINGTACheckBase*>(opCmd);
                                if (NULL!=pActBase)
                                {
                                    pActBase->setActionOnFail(sActOnFail);
                                }
                                else if (NULL!=pCheckBase)
                                {
                                    pCheckBase->setActionOnFail(sActOnFail);
                                }
                            }
                            else
                            {
                                osError = "Invalid Action value";
                                rC=false;
                            }

                        }
                        else
                        {
                            if (osError.isEmpty())
                                osError = "Action on fail exists for check and action only";
                            rC=false;
                        }
                    }

                    sDumpList.remove("\n");//dump list being last in a row will have new line
                    if (!sDumpList.isEmpty())
                    {
                        //int dSize = sDumpList.size();
                        sDumpList.replace(GENESTA_COLON_IDENTIFIER,GENESTA_DELIMITER);
                        QStringList paramList = sDumpList.split(GENESTA_CMD_DUMP_SEPERATOR);
                        if (!paramList.isEmpty())
                        {
                            // opCmd->setDumpList(paramList);
                            QStringList dumpParameterList;
                            foreach(QString sParameterName, paramList)
                            {
                                QStringList lstParamNames;
                                
                                AINGTAGenestaParserItf::getPreprocessedStatements(sParameterName,lstParamNames);
                                if (!lstParamNames.isEmpty())
                                {
                                    foreach(QString sParamInfo, lstParamNames)
                                    {
                                        AINGTAGenestaParserItf::replaceGenSymbols(sParamInfo);
                                        if (!sParamInfo.isEmpty())
                                        {
                                            dumpParameterList.append(sParamInfo.trimmed());
                                            rC = true;;
                                        }
                                    }

                                }
                                else
                                {
                                    osError = "preprocessing of dump list failed";
                                    break;
                                }
                            }

                            opCmd->setDumpList(dumpParameterList);
                        }
                        else
                            osError= "Dump list not evaluated";

                    }
                }
            }
            

            
        }
        else
            osError = "False statement";
    }
    else
    {
        osError="Statement is empty";
        rC=true; //Do not log empty statements.
    }
    return rC;
}
QString AINGTAGenestaFileParser:: getErrorLog()
{
    QString errorStatements;
    foreach(int lineNo, _errorLog.keys())
    {
        errorStatements.append(QString("\tError: line %1- %2\n").arg(QString::number(lineNo),_errorLog.value(lineNo)));
    }
    return errorStatements;
}
bool AINGTAGenestaFileParser::getStatementForInit(const QString& ipStatement,QString& osEcallStatement,const QString& iComment,QString& oError)
{
    /*If name_init = "A/C ground, not powered (dark cockpit)" Then
        name_obj_init = "init_1"
        'Patch OA
        ElseIf name_init = "A/C ground, engine off, A/C powered" Then
        name_obj_init = "init_2"
        ElseIf name_init = "A/C ground, engine running" Then
        If InStr(Workbooks(XLS_name).Sheets(1).Cells(k, 2), "Engines = 1+2") <> 0 Then
        name_obj_init = "init_3"
        ElseIf InStr(Workbooks(XLS_name).Sheets(1).Cells(k, 2), "Engines = 1 r") <> 0 Then
        name_obj_init = "init_31"
        ElseIf InStr(Workbooks(XLS_name).Sheets(1).Cells(k, 2), "Engines = 1 n") <> 0 Then
        name_obj_init = "init_32"
        End If
        Else
        name_obj_init = "init_1"
        End If*/

    bool rC = false;
    QString initName;
    if (ipStatement.contains("A/C ground, not powered (dark cockpit)"))
    {
        initName ="init_1";
        rC = true;
    }
    else if (ipStatement.contains("A/C ground, engine off, A/C powered"))
    {
        initName ="init_2";
        rC = true;
    }
    else if (ipStatement.contains("A/C ground, engine running"))
    {
        if (ipStatement.contains("Engines = 1 running, 2 not running"))
        {
            initName = "init_31";
            rC = true;
        }
        else if (ipStatement.contains("Engines = 1+2 running"))
        {
            initName = "init_3";
            rC = true;
        }
        else if (ipStatement.contains("Engines = 1 not running"))
        {
            initName = "init_2";
            rC = true;
        }
        else
            oError="Init not recognized, for A/C ground, engine running ";
    }
    else if(ipStatement.contains("A/C in flight"))
    {
        initName = "init_1";
        rC = true;
    }
    else
    {
        oError="Init not recognized";
        rC = false;
    }
    if(iComment.contains("MT-A400m",Qt::CaseInsensitive) && iComment.contains("EPI_TP400",Qt::CaseInsensitive))
    {
        _Aircraft = "MT";
        _Equipment = "EPI";
    }
    else if(iComment.contains("LR-A330neo",Qt::CaseInsensitive) && iComment.contains("RR_TRENT_7000",Qt::CaseInsensitive))
    {
        _Aircraft = "LR";
        _Equipment= "RR";
    }
    else if(iComment.contains("SA-A320neo",Qt::CaseInsensitive) && iComment.contains("CFM_LEAPX",Qt::CaseInsensitive))
    {
        _Aircraft = "SA";
        _Equipment= "CFM";
    }
    else if(iComment.contains("SA-A320neo",Qt::CaseInsensitive) && iComment.contains("PW_1100G",Qt::CaseInsensitive))
    {
        _Aircraft = "SA";
        _Equipment= "PW";
    } 
    else if(iComment.contains("SA-A320neo",Qt::CaseInsensitive) && iComment.contains("PW1100G",Qt::CaseInsensitive))
    {
        _Aircraft = "SA";
        _Equipment= "PW";
    }
    else if(iComment.contains("XWB-A350",Qt::CaseInsensitive) && iComment.contains("RR_TRENT_XWB",Qt::CaseInsensitive))
    {
        _Aircraft = "XWB";
        _Equipment= "RR";
    }
    else if(iComment.contains("DD-A380",Qt::CaseInsensitive) && iComment.contains("EA_GP7200",Qt::CaseInsensitive))
    {
        _Aircraft = "DD";
        _Equipment= "EA";
    }

    if (rC)
    {
        if (_Aircraft.isEmpty() || _Equipment.isEmpty())
        {
            oError = "unknown aircraft and equipment";
            rC=false;
        }
        else
        {
            QString sFileName = QString("%1").arg(initName);
            osEcallStatement = QString("call object - %1").arg(sFileName);
            rC = true;


        }
        
    }
    
    return rC;
}
void AINGTAGenestaFileParser:: applyNamingConventionForAircraftEquip()
{
    if(_Aircraft.contains("MT-A400m",Qt::CaseInsensitive)) 
    {
        _Aircraft = "MT";
    }
    else if(_Aircraft.contains("LR-A330neo",Qt::CaseInsensitive))
    {
        _Aircraft = "LR";
    }
    else if(_Aircraft.contains("SA-A320neo",Qt::CaseInsensitive))
    {
        _Aircraft = "SA";
    }
    else if(_Aircraft.contains("SA-A320neo",Qt::CaseInsensitive))
    {
        _Aircraft = "SA";
    }
    else if(_Aircraft.contains("XWB-A350",Qt::CaseInsensitive))
    {
        _Aircraft = "XWB";
    }
    else if(_Aircraft.contains("DD-A380",Qt::CaseInsensitive))
    {
        _Equipment= "EA";
    }

    if(_Equipment.contains("EPI_TP400",Qt::CaseInsensitive))
    {
        _Equipment = "EPI";
    }
    else if( _Equipment.contains("RR_TRENT_7000",Qt::CaseInsensitive))
    {
        _Equipment= "RR";
    }
    else if(_Equipment.contains("CFM_LEAPX",Qt::CaseInsensitive))
    {
        _Equipment= "CFM";
    }
    else if( _Equipment.contains("PW_1100G",Qt::CaseInsensitive))
    {
        _Equipment= "PW";
    } 
    else if( _Equipment.contains("PW1100G",Qt::CaseInsensitive))
    {
        _Equipment= "PW";
    }
    else if( _Equipment.contains("RR_TRENT_XWB",Qt::CaseInsensitive))
    {
        _Equipment= "RR";
    }
    else if( _Equipment.contains("EA_GP7200",Qt::CaseInsensitive))
    {
        _Equipment= "EA";
    }

}
bool AINGTAGenestaFileParser:: createCommandsFromActionStatement(const QString& iStatement , QList<AINGTACommandBase*>& olstCmd,QString& osError, const QString &iPrecision,const QString &isTimeOut,const QMap<QString,QString>& iMapGenestaSymbols)
{
    bool rC = false;

    AINGTAGenestaParserItf* pSerItf = NULL;
    if (!iStatement.isEmpty())
    {
        iStatement.indexOf(GENESTA_CMD_SET);

        applyNamingConventionForAircraftEquip();
        if (iStatement.indexOf(GENESTA_CMD_SET)==0)
            pSerItf= new AINGTAGenestaActSetDeserializer(iStatement,_Equipment,_Aircraft,isTimeOut);
        else if (iStatement.indexOf(GENESTA_CMD_SET_SSM)==0)
            pSerItf= new AINGTAGenestaActSetSSMDeserializer(iStatement,_Equipment,_Aircraft);
        else if (iStatement.indexOf(GENESTA_CMD_IF)==0)
            pSerItf= new AINGTAGenestaActIFDeserializer(iStatement,COM_CONDITION_IF,_Equipment,_Aircraft,isTimeOut);
        else if (iStatement.indexOf(GENESTA_CMD_WHILE)==0)
            pSerItf= new AINGTAGenestaActWhileDeserializer(iStatement,COM_CONDITION_WHILE,_Equipment,_Aircraft,isTimeOut);
        else if (iStatement.indexOf(GENESTA_CMD_WAIT)==0)
            pSerItf= new AINGTAGenestaActWaitDeserializer(iStatement,_Equipment,_Aircraft,isTimeOut);
		else if (iStatement.indexOf(GENESTA_CMD_PRINT)==0)
            pSerItf= new AINGTAGenestaActPrintDeserializer(iStatement,_Equipment,_Aircraft);
        else if (iStatement.indexOf(GENESTA_CMD_CALL)==0)
            pSerItf= new AINGTAGenestaActCallDeserializer(iStatement,_Equipment,_Aircraft,_fileList,_exportFolder);
        else if (iStatement.indexOf(GENESTA_CMD_CHECK_FWC_WARNING)==0)
            pSerItf= new AINGTAGenestaChkFWCWrngDeserializer(iStatement);
        else if (iStatement.indexOf(GENESTA_CMD_CHECK_BITE_MESSAGE)==0)
            pSerItf= new AINGTAGenestaChkBITEMsgDeserializer(iStatement);
        else if (iStatement.indexOf(GENESTA_CMD_CHECK_VAL)==0)
            pSerItf= new AINGTAGenestaChkValueDeserializer(iStatement,_Equipment,_Aircraft,iPrecision,isTimeOut);
        else if (iStatement.indexOf(GENESTA_CMD_CHECK_SSM)==0)
            pSerItf= new AINGTAGenestaChkSSMDeserializer(iStatement,_Equipment,_Aircraft,iPrecision);
        else if (iStatement.indexOf(GENESTA_CMD_MANUAL_ACT)==0)
            pSerItf= new AINGTAGenestaActManualDeserializer(iStatement);
        else if (iStatement.indexOf(GENESTA_CMD_CHECK_FWC_DISP)==0)
            pSerItf= new AINGTAGenestaActManualDeserializer(iStatement);
        else if (iStatement.indexOf(GENESTA_CMD_CHECK_FWC_PROC)==0)
			pSerItf= new AINGTAGenestaChkFWCProcDeserializer(iStatement);
        else if (iStatement.indexOf(GENESTA_CMD_CHECK_ECAM_DISP)==0)
            pSerItf= new AINGTAGenestaActManualDeserializer(iStatement);
        else if (iStatement.indexOf(GENESTA_CMD_CHECK_VIZ_PROC)==0)
            pSerItf= new AINGTAGenestaActManualDeserializer(iStatement);
        else if (iStatement.indexOf(GENESTA_CMD_CHECK_MANUAL)==0)
            pSerItf= new AINGTAGenestaActManualDeserializer(iStatement);
        else if (iStatement.indexOf(GENESTA_CMD_RELEASE)==0)
            pSerItf= new AINGTAGenestaActReleaseDeserializer(iStatement,_Equipment,_Aircraft);
        else if (iStatement.indexOf("END")==0)
        {
            rC=true;
            return rC;
        }
        else
            pSerItf = new AINGTAGenestaActInvalidDeserializer(iStatement);;



        if (pSerItf)
        {
            pSerItf->setGenestaSymbolMap(iMapGenestaSymbols);
            rC=pSerItf->getCommands(olstCmd);

            if (!rC)
                osError = pSerItf->getLastError();

            delete pSerItf;

        }
        else
            osError="Command not implemented or does not exist";
    }
    else
        osError="Empty statement, command not created";
    return rC;
}
AINGTAHeader* AINGTAGenestaFileParser::getHeadertFromStatements(const QStringList& ilstHeaderStatements)
{
    //bool isStandard=false;
    int line=0;
    AINGTAHeader* pHeader = new AINGTAHeader;
    foreach(QString headerLine, ilstHeaderStatements)
    {
        headerLine.replace("†","-");
        QStringList hdrInfoLSt = headerLine.split(GENESTA_DELIMITER);
        if (!hdrInfoLSt.isEmpty())
        {

            QString hdr = hdrInfoLSt.at(0);
            hdr.remove("²");hdr.remove("*");hdr.remove("^");
            hdrInfoLSt.removeAt(0);
            hdr.remove("\n");

            if(hdr.isEmpty())
                continue;


            QString val = hdrInfoLSt.join("/");
            val.remove("\n");

            foreach(QString symbol,_mapGenestaSymbols.keys())
            {              
                val.replace(symbol,QString("%1%2%1").arg(QString(TAG_IDENTIFIER), _mapGenestaSymbols.value(symbol)));
            }
            if (headerLine.contains("Aircraft :"))
            {
                _Aircraft=val;
                _Aircraft.remove("\n");
            }
            if (headerLine.contains("Equipment :"))
            {
                QStringList eqpInfolst = val.split("_");
                _Equipment=eqpInfolst.at(0);
                _Equipment.remove("\n");
            }
            if (headerLine.contains("Procedure name :")||headerLine.contains("Object name :") )
            {
                _sFileName=val.remove("\n");
                //pHeader->addHeaderItem(hdr,false,_sFileName);
            }
            else
            {
                pHeader->addHeaderItem(hdr,false,val);
            }
        }
        else
            appendToLog(line,"Header: could not evaluate line");

        line++;

        
    }
    return pHeader;

    //foreach(QString headerLine, ilstHeaderStatements)
    //{
    //    if (headerLine.contains(QString("%1 %2").arg(ELEM_PROC,"standards")) || headerLine.contains(QString("%1 %2").arg(ELEM_OBJ,"standards")) )
    //    {
    //        isStandard=true;
    //        continue;
    //    }
    //    if (!isStandard)
    //    {
    //        QStringList hdrInfo = headerLine.split(GENESTA_DELIMITER) ;
    //        if (hdrInfo.size()==2)
    //        {
    //            QString hdrTag = hdrInfo.at(0);
    //            QString hdrVal = hdrInfo.at(1);

    //            //append here
    //                     
    //        }
    //        else
    //            appendToLog(line,"Header: could not evaluate line");
    //    }
    //    else
    //    {
    //        QStringList hdrInfo = headerLine.split(GENESTA_DELIMITER) ;
    //        int hdrInfosixe = hdrInfo.size();

    //        if (!hdrInfo.isEmpty())
    //        {
    //            QString hdrStandards = hdrInfo.at(0);
    //            QStringList hdrStandardInfoLst = hdrStandards.split("/");
    //            if (hdrStandardInfoLst.size()==hdrInfosixe-1)
    //            {
    //                //append here
    //                for (int i=0;i<hdrInfosixe-1;i++)
    //                {
    //                    QString std = hdrStandardInfoLst.at(i);
    //                    QString val = hdrInfo.at(i+1);
    //                    //append here;
    //                    ;

    //                }
    //                

    //            }
    //            else
    //            {

    //            }
    //            


    //        }
    //        else
    //            appendToLog(line,"Header: could not evaluate line");

    //    }

    //    line++;
    //}

    return NULL;

}
void AINGTAGenestaFileParser::insertGenstaSymbolMap(const QString& iSymbolName,const QString& iSymbol)
{
    if (_mapGenestaSymbols.contains(iSymbolName)==false)
    {
        _mapGenestaSymbols.insert(iSymbolName,iSymbol);
    }

}

// print table , 
//In genesta print table's title is determined by comment in previous row. (standalone Comment gets converted to print msg)
void AINGTAGenestaFileParser::processPrintCommand(AINGTAElement* pElement, AINGTACommandBase* pCurrentCmd, int currentIndex)
{
    if(pElement!=NULL && pElement->isEmpty() == false)
    {
        AINGTAActionPrintTable* pPrintTable = dynamic_cast<AINGTAActionPrintTable*>(pCurrentCmd);
        if (pPrintTable)
        {
            AINGTACommandBase* pPreviousCmd = NULL;
            pElement->getCommand(currentIndex-1,pPreviousCmd);
            AINGTAActionPrint* pPrintMsg = dynamic_cast<AINGTAActionPrint*>(pPreviousCmd);
            if (pPrintMsg!=NULL && pPrintMsg->getComplement()==COM_PRINT_MSG)
            {
                QString msg = pPrintMsg->getValues().at(0);
                pPrintTable->setTitleName(msg);
                /* pElement->insertCommand(NULL,currentIndex-1,false);
                pElement->removeCommand(currentIndex-1);*/
            }


        }
    }
    
}
























