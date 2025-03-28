#include "GTALogReportController.h"
#include "GTAScxmlLogParser.h"
#include "GTAUtil.h"
#include "GTAActionParentTitle.h"
#include "GTAActionExpandedCall.h"
#include "GTAActionSetList.h"
#include "GTAActionPrintTable.h"
#include "GTAActionSet.h"
#include "GTAActionForEach.h"
#include "GTAActionCallProcedures.h"
#include "GTACheckBase.h"
#include "GTATestConfig.h"

#pragma warning(push, 0)
#include <QDomDocument>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QDateTime>
#include <QProcess>
#include <QDebug>
#pragma warning(pop)

GTALogReportController::GTALogReportController()
{
}

GTALogReportController::~GTALogReportController()
{

}

/**
  * This function provides an  GTAEditorLogModel for a grouped Title element.
  * The log for an element is analyzed against the element to create a log model
  * @iLogFullPath: Full path of the log to be analyzed
  * @ipTitleBasedElement: element under analysis. Grouping of the element must be done previously
  * @ioLogInstanceIDMap: the Map that holds uuid(key) and relevant line number of the command(value)
  * @oModel: GTAEditorLogModel output
  * @oErrorMessages: any error message occuring during log analysis
  * @return: pass/fail(true/false) status of the log analysis and model creation
  */
 bool GTALogReportController::getLogModel(QString &iLogFullPath,
                                             GTAElement*& ipTitleBasedElement,
                                             QHash<QString, QString> &ioLogInstanceIDMap,
                                             GTAEditorLogModel*& oModel,
                                             QString &oErrorMessages)
{
    bool rc = false;
    // generateLTR is always true, whatever the content of LogFile field
    bool generateLTR = true;
    if(ipTitleBasedElement != NULL)
    {
        bool sRepetativeLog = false;

//        GTAAppSettings appSettings;
        //sRepetativeLog = appSettings.getRepeatedLoggingSetting();

        GTAScxmlLogParser objLogParser(iLogFullPath);

        QHash<QString,LogMessageList*> logMsgTable;

        rc = objLogParser.getLogResult(logMsgTable,sRepetativeLog);

        ipTitleBasedElement->setExecStartDuration(objLogParser.getExecStartTime());
        ipTitleBasedElement->setExecEndDuration(objLogParser.getExecEndTime());

        int childCount = ipTitleBasedElement->getAllChildrenCount();
        QStringList logInstanceList;
        QRegExp regExp("_uuid\\[\\{[a-z0-9]{8}-([a-z0-9]{4}-){3}[a-z0-9]{12}\\}\\]");
        for(int i=0;i<logMsgTable.keys().count();i++)
        {
            QString item=logMsgTable.keys().at(i);
            if(item != NULL && !item.trimmed().isEmpty() && item != "")
            {
                //remove the _uuid[{<uuid>}] from the key for comparision with Instance name later
                //if (item.contains(regExp))
                    //item.remove(regExp);
                logInstanceList.append(GTAUtil::getNewInstanceName(item));
            }
        }

        bool bContainsValidLog = true;
        for(int i = 0; i < childCount; i++)
        {
            GTACommandBase * pCmd = NULL;
            rc = ipTitleBasedElement->getCommand(i,pCmd);
            if(rc && pCmd != NULL)
            {
                QString instanceName = pCmd->getInstanceName();
                QString newInstanceName = GTAUtil::getNewInstanceName(instanceName);
                QRegExp rgExp(newInstanceName,Qt::CaseInsensitive);
                int index = logInstanceList.indexOf(rgExp);
                LogMessageList *logMsgList = NULL;
                if(!(index >= 0))
                {
                    QRegExp rgExp1(instanceName,Qt::CaseInsensitive);
                    index = logInstanceList.indexOf(rgExp1);
                }
                if((index>=0) && !pCmd->isIgnoredInSCXML())
                {
                    newInstanceName = logMsgTable.keys().at(index);
                    logMsgList = logMsgTable[newInstanceName];
                }

                if(logMsgList != NULL)
                {
                    pCmd->setLogMessageList(*logMsgList);
                    QString instance = getInstanceNameList(instanceName).join("/");
                    ioLogInstanceIDMap.insertMulti(pCmd->getObjId().isEmpty()? QString("dummy") : pCmd->getObjId(),instance);
                }
                else
                {

                    if ((pCmd->logCanExist()==true) && !pCmd->isIgnoredInSCXML())
                    {
                        LogMessageList *subLogMsgList = new LogMessageList();
                        if(generateLTR)
                        {
                            pCmd->setLogMessageList(*subLogMsgList);
                            QString instance = getInstanceNameList(instanceName).join("/");
                            ioLogInstanceIDMap.insert(pCmd->getObjId().isEmpty()? QString("dummy") : pCmd->getObjId(),instance);
                        }
                        else if (pCmd->createsCommadsFromTemplate())
                        {
                            GTAActionFTAKinematicMultiOutput *pKinematicCmd = dynamic_cast<GTAActionFTAKinematicMultiOutput*>(pCmd);
                            QString action = pKinematicCmd->getAction();
                            QString comple = pKinematicCmd->getComplement();

                            QString templateName = QString("%1_%2").arg(action,comple);
                            templateName.replace(" ","_");
                        }
                        else
                        {
                            GTAScxmlLogMessage msg;
                            msg.Result = GTAScxmlLogMessage::kNa;
                            subLogMsgList->append(msg);
                            pCmd->setLogMessageList(*subLogMsgList);
                            bContainsValidLog = false;
                            QString subInstanceName = pCmd->getInstanceName();
                            QString ignoreMsg="";

                            //Check about ignore message
                            if(!subInstanceName.isEmpty())
                            {
                                QString msgLog = QString("[Line %1]:%2%3\n").arg(QString::number(i), subInstanceName,ignoreMsg);
                                oErrorMessages.append(msgLog);
                            }
                            QString instance = getInstanceNameList(subInstanceName).join("/");
                            ioLogInstanceIDMap.insertMulti(pCmd->getObjId().isEmpty()? QString("dummy") : pCmd->getObjId(),instance);
                        }
                    }//if logcanexist
                    else if((pCmd->hasReference()|| pCmd->isTitle()) && generateLTR)
                    {
                        GTAScxmlLogMessage msg;
                        msg.Result = GTAScxmlLogMessage::kNa;
                        LogMessageList * subLogMsgList = new LogMessageList();
                        subLogMsgList->append(msg);
                        pCmd->setLogMessageList(*subLogMsgList);
                        ioLogInstanceIDMap.insertMulti(pCmd->getObjId().isEmpty()? QString("dummy") : pCmd->getObjId(),pCmd->getObjId().isEmpty()? QString("Call or Title") : getInstanceNameList(instanceName).join("/"));
                    }
                    else
                    {
                        QString instance = getInstanceNameList(instanceName).join("/");
                        if (pCmd->getInstanceName().contains("END IF",Qt::CaseInsensitive) || pCmd->getInstanceName().contains("END for each",Qt::CaseInsensitive) || (pCmd->getCommandTypeForDisplay().contains("ACTION",Qt::CaseInsensitive) && pCmd->hasReference()))
                        {
                            ioLogInstanceIDMap.insert(pCmd->getObjId().isEmpty()? QString("dummy") : pCmd->getObjId(),instance);
                        }
                        else
                        {
                            //add the command to the ioLogInstanceIDMap
                            ioLogInstanceIDMap.insert(pCmd->getObjId().isEmpty()? QString("dummy") : pCmd->getObjId(),instance);
                        }
                    }
                }

                //SetList has multiple sets with different UUIDs and are available as internal parameter
                //DEV: Update. Implemented MultiSET. Now SET command has only one return value
                GTAActionSetList *pSetList = dynamic_cast<GTAActionSetList*>(pCmd);
                if (pSetList!=NULL)
                {
                    QList<GTAActionSet* > pSets;
                    pSetList->getSetCommandlist(pSets);
                    QString instance = getInstanceNameList(instanceName).join("/");
                    foreach (GTAActionSet* pSet, pSets)
                    {
                        if (pSet!=NULL)
                        {
                            ioLogInstanceIDMap.insertMulti(pSet->getObjId().isEmpty()? QString("dummy"):pSet->getObjId(),instance);
                        }
                    }
                }
            }
        }

        //clear logMsgTable
        foreach(QString key, logMsgTable.keys())
        {
            LogMessageList * pList = logMsgTable.value(key);
            if(pList != NULL)
            {
                delete pList;
                pList = NULL;
            }
        }
        if (bContainsValidLog == false)
        {
            _LastError = "Log mismatch";
            rc =false;
        }
        updateReportStatus(ipTitleBasedElement);
        oModel = new GTAEditorLogModel(ipTitleBasedElement);
    }
    return rc;
}

void GTALogReportController::updateReportStatus(GTAElement *pElem)
{

    int totalChildCnt = pElem->getAllChildrenCount();
    for(int i=0;i<totalChildCnt;i++)
    {
        GTACommandBase *pCmd = NULL;
        pElem->getCommand(i,pCmd);
        if(pCmd == NULL)
            continue;

        GTAActionExpandedCall *pcall = dynamic_cast<GTAActionExpandedCall*>(pCmd);
        GTAActionParentTitle *pTitleCmd = dynamic_cast<GTAActionParentTitle*>(pCmd);
        if(pcall != NULL)
        {
            if(pcall->getGlobalResultStatus().contains("KO"))
            {
                pCmd->setReportStatus(GTACommandBase::INCLUDE);
            }
            else
            {
                pCmd->setReportStatus(GTACommandBase::PARTIALLY_INCLUDE);
            }
        }
        else if(pTitleCmd != NULL)
        {
            GTACommandBase *pParentCmd = pTitleCmd->getParent();
            GTAActionExpandedCall *pSubCall = dynamic_cast<GTAActionExpandedCall*>(pParentCmd);

            if(pSubCall != NULL)
            {
                GTACommandBase::reportSelectionType selType;
                if(pSubCall->getGlobalResultStatus().contains("KO"))
                {
                    pCmd->setReportStatus(GTACommandBase::INCLUDE);
                    selType = GTACommandBase::INCLUDE;
                }
                else
                {
                    pCmd->setReportStatus(GTACommandBase::EXCLUDE);
                    selType = GTACommandBase::EXCLUDE;
                }
                if(pTitleCmd->hasChildren())
                {
                    QList<GTACommandBase*> &lstChildren = pTitleCmd->getChildren();
                    for(int j = 0; j < lstChildren.count(); j++)
                    {
                        GTACommandBase *pTitleChild = lstChildren.at(j);
                        if(pTitleChild != NULL)
                            pTitleChild->setReportStatus(selType);
                    }
                }
            }
        }
        else
        {
            GTACommandBase *pParentCmd = pCmd->getParent();
            GTAActionExpandedCall *pSubCall = dynamic_cast<GTAActionExpandedCall*>(pParentCmd);
            if(pSubCall != NULL)
            {
                if(pSubCall->getGlobalResultStatus().contains("KO"))
                {
                    pCmd->setReportStatus(GTACommandBase::INCLUDE);
                }
                else
                {
                    pCmd->setReportStatus(GTACommandBase::EXCLUDE);
                }
            }
            else
            {
            }
        }
    }
}

/**
  * This function returns the line number of a specific command by deriving it out of the instance name
  * eg. test[0]#Call - abc.fun[1]#Wait for 1s ==> QStringlist([0],[1])
  * @iInstanceName: Instance name of command whose line number needs to be identified
  * @return: List of line number based on call level
  */
QStringList GTALogReportController::getInstanceNameList(const QString &iInstanceName) const
{
    QStringList instances = iInstanceName.split("#");
    QStringList ret;
    for (int i=0;i<instances.count() - 1;i++)
    {
        QString instance = instances.at(i);
        instance.replace("call","").replace(" - ","").replace("object","",Qt::CaseInsensitive).replace("function","",Qt::CaseInsensitive).replace("procedure","",Qt::CaseInsensitive);
        QStringList fullPath = instance.split("/");QStringList actualPath = instance.split("/");
        for (int j=0;j<fullPath.count();j++)
        {
            QString path = fullPath.at(j);
            if (!path.contains("[") && !path.contains("]"))
                actualPath.removeOne(path);
        }
        for (int k=0;k<actualPath.size();k++)
        {
            QString entry = actualPath.at(k);
            int indexstart = entry.indexOf("[");
            entry.remove(0,indexstart);
            actualPath.replace(k,entry);
        }

        ret.append(actualPath);
    }
    if (ret.isEmpty())
        ret.append(iInstanceName);
    return ret;
}

//bool GTALogReportController::loadTemplateFile(const QString &iTmplFileName,const GTAActionFTAKinematicMultiOutput *pKinematciCmd,GTAElement *& opElement)
//{
//    bool rc   = false;
//    opElement = NULL;
//    rc = getElementFromTemplate(iTmplFileName,opElement);
//    if(rc)
//    {
//        rc = generateProcFromTemplate(pKinematciCmd,opElement);
//    }
//    return rc;
//}

//bool GTALogReportController::getElementFromTemplate(const QString & iFileName, GTAElement *& opElement)
//{
//    bool rc = false;
//    QString completeFilePath = _pSystemServices->getTemplateFilePath(iFileName);

//    GTADocumentController docController;
//    GTAHeader* pHdr = NULL;

//    rc = getElementFromDocument(iFileName,opElement,false);
//    if(opElement != NULL)
//        pHdr = opElement->getHeader();

//    //rc = docController.openDocument(completeFilePath,opElement,pHdr);
//    if (NULL!=pHdr)
//    {
//        delete pHdr;pHdr=NULL;
//    }
//    if(! rc)
//        _LastError = getLastError();
//    return rc;
//}

/**
  * This function reads the log for a sequence with multiple logs
  * @iLogFile: input file to be parsed
  * @oLogNames: list of log file names in the iLogFile
  * @oProcNames: list of procedure names in the iLogFile
  * @oStatus: list of procedure result status in the iLogFile
  * @oGlobalStatus: global status computed from the list of status
  */
bool GTALogReportController::readSequenceMultipleLogFile(const QString &iLogFile,
                                                            QStringList &oLogNames,
                                                            QStringList &oProcNames,
                                                            QStringList &oStatus,
                                                            QString &oGlobalStatus)
{
    bool rc = false;
    QFile file(iLogFile);
    if(file.exists())
    {
        rc = file.open(QIODevice::ReadOnly | QIODevice::Text);
        if(rc)
        {
            QTextStream stream(&file);
            while(!stream.atEnd())
            {
                QString line = stream.readLine();
                QStringList parts = line.split(":;:");

                if(parts.count() == 3)                      // proc:;:log:;:status
                {
                    for(int i = 0; i < parts.count(); i++)
                    {
                        QString part = parts.at(i);
                        switch(i)
                        {
                        case 0:
                            oProcNames.append(part);
                            break;

                        case 1:
                            oLogNames.append(part);
                            break;

                        case 2:
                            oStatus.append(part);
                            break;

                        }
                    }
                }
                else if(parts.count() == 2)             //GLOBAL_STATUS:;:statusValue
                {
                    oGlobalStatus = parts.at(1);
                }
            }
            file.close();
        }
        else
        {
            _LastError = QString("Error while opening file (%1)").arg(iLogFile);
        }
    }
    else
    {
        _LastError = QString("Log file (%1) does not exist").arg(iLogFile);
    }
    return rc;

}

/**
  * This function checks if the sequence is executed with multiple logs
  * @iLogFile: absolute path of the logFile
  * @return: true/false based on single/multiple logs
  */
bool GTALogReportController::isSequenceWithMultipleLogs(const QString &iLogFile)
{
    bool isMultiple = false;

    QFile file(iLogFile);
    if(file.exists())
    {
        bool rc = file.open(QIODevice::ReadOnly | QIODevice::Text);
        if(rc)
        {
            QTextStream stream(&file);
            QString firstLine = stream.readLine();
            QStringMatcher matcher("$$Multiple_Logs$$");
            if(matcher.indexIn(firstLine) != -1)
            {
                isMultiple = true;
            }
            file.close();
        }
        else
        {
            _LastError = QString("Error while opening file (%1)").arg(iLogFile);
        }
    }
    else
    {
        _LastError = QString("Log file (%1) does not exist").arg(iLogFile);
    }
    return isMultiple;

}

/**
  * @brief: This function provides a custom log model for a sequence with multiple logs for a grouped Title element.
  * @ipTitleBasedElement: element under analysis. Grouping of the element must be done previously
  * @oModel: GTAEditorLogModel output
  * @oErrorLogList: any error message occuring during log analysis
  * @ioGlobalStatus: global status of all procedures run in the sequence
  * @iProcNames: Procedures in the sequence
  * @iLogNames: logs of the relevant procedures
  * @iExecStatus: status if each procedure run in the sequence
  */
void GTALogReportController::getLogModelForSequence(GTAElement*& ipTitleBasedElement,
                                                       QAbstractItemModel*& oModel,
                                                       QString &oErrorLogList,
                                                       QString &ioGlobalStatus,
                                                       QStringList &iProcNames,
                                                       QStringList &iLogNames,
                                                       QStringList &iExecStatus)
{
    QString execStart="";
    if (ipTitleBasedElement !=NULL)
    {
        int childCount = ipTitleBasedElement->getAllChildrenCount();
        QString status = "NA";
        int resultcount=0;
        for (int i=0;i<childCount;i++)
        {
            GTACommandBase *pCmd = NULL;
            ipTitleBasedElement->getCommand(i,pCmd);
            GTAActionCall *pCall = dynamic_cast<GTAActionCall*>(pCmd);
            QString callName="";
            //checking if the procedure list contains the call to identify if it was executed
            if (pCall != NULL)
            {
                callName = pCall->getElement();
                if (!iProcNames.contains(callName))
                {
                    //encounters a call command but not part of the list. Status is NA for the rest of the elements after it until a known call is identified
                    status = "NA";
                    QString logmessage = QString("%1:;:%2:;:true:;:false:;:NA:;::;:None:;:0").arg(pCmd->getInstanceName(),status);
                    GTAScxmlLogMessage log(logmessage);
                    LogMessageList logMsgList;
                    logMsgList.append(log);
                    pCmd->setLogMessageList(logMsgList);
                    pCmd->setExecutionResult(status);
                    QString msgLog = QString("Call '%1' log couldn't be found. Check execution details!\n").arg(callName);
                    oErrorLogList.append(msgLog);
                    _LastError = msgLog;
                    continue;
                }
                else
                {
                    int index = iProcNames.indexOf(callName);
                    QString logFileName=iLogNames.at(index);
                    status = iExecStatus.at(index);
                    {
                        //remove the index so that repeated calls are processed with proper status
                        //a call command once processed will not be processed again
                        //usecase: seq1
                        //          call fun1
                        //          call fun2
                        //          call fun1
                        //in this case, the iomodel would have 2 fun1. So, after processing the first one, we remove it.
                        iProcNames.removeAt(index);
                        iLogNames.removeAt(index);
                        iExecStatus.removeAt(index);
                    }
                    QFileInfo reportFileInfo(logFileName);
                    if (reportFileInfo.exists() && !execStart.isEmpty())
                    {
                        ipTitleBasedElement->setExecStartDuration(reportFileInfo.lastModified().toString(Qt::TextDate));
                    }
                    else
                    {
                        QString msgLog = QString("Call '%1' log couldn't be found. Check execution details!\n").arg(callName);
                        oErrorLogList.append(msgLog);
                    }
                    ipTitleBasedElement->setExecEndDuration(reportFileInfo.lastModified().toString(Qt::TextDate));
                    resultcount++;
                }
            }
            else
            {
                //add OK/KO to other commands and continue if pCmd is not a Call command
                QString logmessage = QString("%1:;:%2:;:true:;:true:;:NA:;::;:None:;:0").arg(pCmd->getInstanceName(),status);
                GTAScxmlLogMessage log(logmessage);

                LogMessageList logMsgList;
                logMsgList.append(log);
                pCmd->setLogMessageList(logMsgList);
                pCmd->setExecutionResult(status);
                continue;
            }

            //adding a custom log to logmessagelist of the call command to show procedure was a Pass/Fail
            QString logmessage = QString("%1:;:%2:;:true:;:true:;:NA:;::;:None:;:0").arg(pCmd->getInstanceName(),status);
            GTAScxmlLogMessage log(logmessage);
            LogMessageList logMsgList;
            logMsgList.append(log);
            pCmd->setLogMessageList(logMsgList);
            //                pCmd->setExecutionResult(tempoutput.hasFailed);
            pCmd->setExecutionResult(status);

            if (i!=childCount)
            {
                GTACommandBase *pNextCmd=NULL;
                ipTitleBasedElement->getCommand(i+1,pNextCmd);
                if (pNextCmd != NULL)
                {
                    pNextCmd->setLogMessageList(logMsgList);
                    pNextCmd->setExecutionResult(status);
                }
            }

            //change the has failed status to KO if one status is KO
            if (ioGlobalStatus != "KO" && status == "KO")
                ioGlobalStatus = "KO";
        }
        updateReportStatus(ipTitleBasedElement);

        oModel = new GTAEditorLogModel(ipTitleBasedElement);
    }
}

/**
  * This function export the lab test report
  * @ipLogModel: Model in UI to be exported, it should of type GTAEditorTreeModel class
  * @iReportFile: Report file full path where the report is created
  * @iAttachmentList: list of documents to be attached with report
  * @exportLTRA: struct to update the parameters used for exporting LTRA containing save format, timing precission, isLTRA
  */
bool GTALogReportController::exportLTRAToDocx(QAbstractItemModel *ipLogModel,
                                                 const QString &iReportFile,
                                                 const QStringList &iAttachmentList,
                                                 bool isLTRA,
                                                 QString saveFormat,
                                                 QString saveInMs,
                                                 QString &iGTADataDirectoryPath,
                                                 QHash<QString, QString> &iLogInstanceIDMap,
                                                 QString &iGTADBFilePath)
{
    bool rc = false;
    if(ipLogModel != nullptr)
    {
        auto pLog = dynamic_cast<GTAEditorLogModel*>(ipLogModel);
        if(pLog != nullptr)
        {
            GTAElement * pElem = pLog->getDataModel();
            QStringList attachmentList;
            attachmentList.append(iAttachmentList);
            QString tempXmlFile = iReportFile;
            tempXmlFile.replace(".docx",".xml");
            rc = exportLTRAToXml(pElem,tempXmlFile,attachmentList,isLTRA,iGTADataDirectoryPath,iLogInstanceIDMap,iGTADBFilePath);
            if (rc)
            {
                if(saveFormat.contains("DOCX"))
                {
                    QString currentAppPath = QCoreApplication::applicationDirPath();
                    QString strnsformAppPath = QDir::cleanPath( QString("%1%2%3").arg(currentAppPath,QDir::separator(),"GTATransformToDocx.exe"));
                    if (QFileInfo infoExecFile( strnsformAppPath ); !infoExecFile.exists())
                    {
                        rc = false;
                        _LastError = "GTATransformToDocx not deployed";
                    }
                    else
                    {
                        QString outputDocxFile = iReportFile;
                        outputDocxFile.replace(".xml",".docx");
                        auto pProcess = new QProcess();
                        QStringList argumentList;
                        QString pathToolsBenchFile = QCoreApplication::applicationDirPath() + QDir::separator() + FILE_VERSIONS_TOOLS_BENCH;
                        argumentList<<tempXmlFile<<outputDocxFile<<saveInMs<<pathToolsBenchFile;
                        pProcess->start(strnsformAppPath,argumentList);
                        pProcess->waitForFinished(-1);
                        auto output  = QString(pProcess->readAllStandardOutput());
                        if (!output.contains("Successful!",Qt::CaseInsensitive))
                        {
                            rc = false;
                            _LastError = output;
                        }
                    }
                    if(saveFormat != "DOCX + XML")
                    {
                        QFileInfo toSaveInfo(tempXmlFile);
                        if (toSaveInfo.exists())
                        {
                            QFile::remove(QDir::cleanPath(tempXmlFile));
                        }
                    }
                    for(int i=0;i<attachmentList.count();i++)
                    {
                        QString printTableFile = attachmentList.at(i);
                        QFileInfo printTableFileInfo (printTableFile);
                        if(printTableFileInfo.exists() && (printTableFileInfo.fileName().toLower().endsWith(".csv")) && (printTableFileInfo.fileName().toLower().startsWith("print_table_")))
                        {
                            QFile::remove(printTableFile);
                        }
                    }
                }
            }
            else
                _LastError = "Report Generation failed.";
        }
    }
    else
        _LastError = "Report analysis failed or not done";
    return rc;
}


/**
  * This function export the LTRA-Lab Test Report Analysis to xml
  * @pElem: Element to be exported
  * @iReportFile: Report file full path where the report is created
  * @iAttachmentList: list of documents to be attached with report
  */
bool GTALogReportController::exportLTRAToXml(GTAElement * pElem,
                                                const QString & iReportFile,
                                                QStringList & iAttachmentList,
                                                bool isLTR,
                                                QString &iGTADataDirectoryPath,
                                                QHash<QString, QString> &iLogInstanceIDMap,
                                                QString &iGTADBFilePath)
{
    QDomDocument domDoc;
	QDomProcessingInstruction pi = domDoc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"iso-8859-1\"");
	domDoc.appendChild(pi);
    QDomElement reportElem = domDoc.createElement("REPORT");
    QDomElement headerElem = domDoc.createElement("REPORT_HEADER");
    QDomElement summaryElem = domDoc.createElement("REPORT_SUMMARY");
    QDomElement ContentElem = domDoc.createElement("REPORT_INFO");
    QDomElement attachmentElem = domDoc.createElement("ATTACHMENTS");
    QDomElement printTableElem = domDoc.createElement("PRINT_TABLES");
    QDomElement dbuserdElem = domDoc.createElement("DATABASE_FILES");
    QDomElement procedurePurposeElem = domDoc.createElement("PROCEDURE_PURPOSE");
    QDomElement procedureConclusionElem = domDoc.createElement("PROCEDURE_CONCLUSION");

    QStringList attachements;
    QStringList referencedAttachmentFiles;

    QList<GTAActionPrintTable*> printTableLst;

    QString globalResult = pElem->getGlobalResultStatus();
    int numberOfOK = pElem->getNumberOfOK();
    int numberOfKO = pElem->getNumberOfKO();
    int numberOfNA = pElem->getNumberOfNA();
    int numberOfKOWithDefect = pElem->getNumberOfKOWithDefect();
    QString listOfDefects = pElem->getListOfDefects();

    QDomElement summaryInfos = domDoc.createElement("REPORT_SUMMARY_INFOS");
    summaryElem.appendChild(summaryInfos);

    QDomElement summaryInfoElem = domDoc.createElement("INFO");
    summaryInfoElem.setAttribute("TITLE", "Global Result");
    summaryInfoElem.setAttribute("RESULT", globalResult);
    summaryInfos.appendChild(summaryInfoElem);

    summaryInfoElem = domDoc.createElement("INFO");
    summaryInfoElem.setAttribute("TITLE", "Total Number Of OK");
    summaryInfoElem.setAttribute("RESULT", numberOfOK);
    summaryInfos.appendChild(summaryInfoElem);

    summaryInfoElem = domDoc.createElement("INFO");
    summaryInfoElem.setAttribute("TITLE", "Total Number Of KO");
    summaryInfoElem.setAttribute("RESULT", numberOfKO);
    summaryInfos.appendChild(summaryInfoElem);

    summaryInfoElem = domDoc.createElement("INFO");
    summaryInfoElem.setAttribute("TITLE", "Total Number Of NA");
    summaryInfoElem.setAttribute("RESULT", numberOfNA);
    summaryInfos.appendChild(summaryInfoElem);

    summaryInfoElem = domDoc.createElement("INFO");
    summaryInfoElem.setAttribute("TITLE", "List Of Defects");
    summaryInfoElem.setAttribute("RESULT", listOfDefects);
    summaryInfos.appendChild(summaryInfoElem);

    summaryInfoElem = domDoc.createElement("INFO");
    summaryInfoElem.setAttribute("TITLE", "Total Number Of KO With Defects");
    summaryInfoElem.setAttribute("RESULT", numberOfKOWithDefect);
    summaryInfos.appendChild(summaryInfoElem);

    QDomElement summaryItem = domDoc.createElement("REPORT_SUMMARY_ITEMS");
    summaryElem.appendChild(summaryItem);

    int directChildrenCnt = pElem->getDirectChildrenCount();
    for(int i=0;i<directChildrenCnt;i++)
    {
        GTACommandBase *pCmd = NULL;
        pElem->getDirectChildren(i,pCmd);
        if(pCmd!= NULL)
        {
            getCommandXmlInfo(pCmd,ContentElem,domDoc,attachements, printTableLst,isLTR,iGTADataDirectoryPath,iLogInstanceIDMap);
            getReferencedAttachmentFiles(pCmd, referencedAttachmentFiles, iGTADataDirectoryPath);

            QString cmdSatement = "";
            QString execResult = "";
            if(pCmd->hasReference())
            {
                GTAActionExpandedCall *pExpCall = dynamic_cast<GTAActionExpandedCall *>(pCmd);
                cmdSatement = pCmd->getStatement();
                execResult = pExpCall->getGlobalResultStatus();
                QString callName = pExpCall->getElement();
                attachements.append(QDir::cleanPath(QString("%1/%2").arg(iGTADataDirectoryPath,callName)));

            }
            else if(pCmd->isTitle())
            {
                GTAActionParentTitle *pParentTitle = dynamic_cast<GTAActionParentTitle *>(pCmd);
                cmdSatement = pParentTitle->getTitle();
                execResult = pParentTitle->getGlobalResultStatus();

                QDomElement summaryItemElem = domDoc.createElement("ITEM");
                summaryItemElem.setAttribute("TITLE",cmdSatement);
                summaryItemElem.setAttribute("RESULT",execResult);
                summaryItem.appendChild(summaryItemElem);
            }
            else
            {
                cmdSatement = pCmd->getStatement();
                execResult = pCmd->getExecutionResult();

                GTAActionPrintTable *pPrintTableCmd = dynamic_cast<GTAActionPrintTable *>(pCmd);
                if(pPrintTableCmd != NULL)
                    printTableLst.append(pPrintTableCmd);

            }


            if((globalResult == "") || (execResult == "KO"))
                globalResult = execResult;
        }
    }

    attachements.append(referencedAttachmentFiles);
    attachements.removeDuplicates();

    globalResult = pElem->getGlobalResultStatus();


    QFile xmlFile(iReportFile);
    QFileInfo reportInfo(iReportFile);
    QString path = reportInfo.absolutePath();
    QString dirfileName = xmlFile.fileName().remove(".xml");
    QString attchDir = QDir::cleanPath (dirfileName+"_files");

    QString fileName = pElem->getDocumentName();
    QList <GTAFILEInfo> lstFileInfo;
    QString libraryPath = iGTADBFilePath.replace(QString("%1%2").arg(QDir::separator(),DATABASE_FILE),"");
    getFileInfoListUsed(fileName, lstFileInfo, iGTADataDirectoryPath,libraryPath);

    for(int i=0;i<lstFileInfo.count();i++)
    {
        GTAFILEInfo fileInfo = lstFileInfo.at(i);
        QString subFileName = fileInfo._filename;
        QString filePath = fileInfo._filePath;
        QString fileType = fileInfo._fileType;

        QDomElement FileElem = domDoc.createElement("DB_FILE");
        FileElem.setAttribute("FILENAME", subFileName);
        FileElem.setAttribute("FILETYPE",fileType);
        FileElem.setAttribute("FILEPATH",filePath);
        dbuserdElem.appendChild(FileElem);

    }
    QStringList printTableRecords;
    GeneratePrintTableRecord(printTableLst,printTableElem,domDoc,path,printTableRecords,iGTADBFilePath);
    iAttachmentList.append(printTableRecords);
    attachements.append(iAttachmentList);
    attachements.removeDuplicates();

    for(int i=0;i<attachements.count();i++)
    {
        QDomElement itemElem = domDoc.createElement("ITEM");
        itemElem.setAttribute("Path",attachements.at(i));
        attachmentElem.appendChild(itemElem);
    }

    procedurePurposeElem.setAttribute("VALUE", pElem->getPurposeForProcedure());
    procedureConclusionElem.setAttribute("VALUE", pElem->getConclusionForProcedure());

    headerElem.setAttribute("GLOBAL_RESULT",globalResult);
    insertHeaderInfoInReport(pElem,headerElem,domDoc,globalResult);
    reportElem.appendChild(headerElem);

    if (pElem->getElementType() == GTAElement::ElemType::PROCEDURE)
    {
        reportElem.appendChild(procedurePurposeElem);
        reportElem.appendChild(procedureConclusionElem);
    }

    reportElem.appendChild(summaryElem);
    reportElem.appendChild(ContentElem);
    reportElem.appendChild(printTableElem);
    reportElem.appendChild(attachmentElem);
    reportElem.appendChild(dbuserdElem);
    domDoc.appendChild(reportElem);

    bool rc = xmlFile.open(QFile::Text| QFile::WriteOnly);
    if(rc)
    {
        QTextStream writer(&xmlFile);
        QByteArray contents = domDoc.toByteArray();
        writer<<contents;
        xmlFile.close();
    }

    return rc;

}

bool GTALogReportController::getCommandXmlInfo(GTACommandBase * pCmd,
                                                  QDomElement &oElem,
                                                  QDomDocument &iDomDoc,
                                                  QStringList &ioAttachmentList,
                                                  QList<GTAActionPrintTable*> &iPrintTableLst,
                                                  bool isLTR,
                                                  QString iGTADataDirectoryPath,
                                                  QHash<QString, QString> &iLogInstanceIDMap)
{
    QString iDecimal = QString::fromStdString(TestConfig::getInstance()->getDecimalPlaces());

    //add reportstatus included,excluded,partially included.
    GTACommandBase::reportSelectionType isIncludedInReport = pCmd->getReportStatus();
    if(isIncludedInReport == GTACommandBase::EXCLUDE)
        return true;

    static int epocTimeCntr = 0;
    QString cmdStatement = pCmd->getLTRAStatement();
    QString cmdComment = pCmd->getComment();
    QString cmdComplementName = "";
    QString cmdActionName = "";

    QString cmdType = "";

    QString cmdResult = "NA";
    QString cmdTimeValue = "";
    QString cmdTimeUnit = "";
    QString cmdConfirmTimeValue = "";
    QString cmdConfirmTimeUnit = "";
    QString epocTimeStamp = "";
    QString sUserFeedBack = "";
    QString sReferences = "";
    QString epochTimeStamp1 = "";
    bool isIgnored = false;

    QString titleHeading ="";
    QString titleComment ="";

    if (true)
    {
        QRegExp regExp("\\[\\{[a-zA-Z0-9]{8}-([a-zA-Z0-9]{4}-){3}[a-zA-Z0-9]{12}\\}\\]");

        QString cmdstat = cmdStatement;
        //int pos = regExp.indexIn(cmdStatement);
        QStringList internalIDs;// = regExp.capturedTexts();
        while (cmdstat.contains(regExp))
        {
            internalIDs.append(regExp.capturedTexts());
            cmdstat.replace(regExp.capturedTexts().first(),"");
        }

        //the condition to check internal parameter and replace them with line number
        QStringList call_linenumber_list = getInstanceNameList(pCmd->getInstanceName());
        call_linenumber_list.removeLast();
        QString call_linenumber = call_linenumber_list.join("/");
        if (!iLogInstanceIDMap.isEmpty())
        {
            for (auto& id : internalIDs)
            {
                id.replace("[","");
                id.replace("]","");
                if (iLogInstanceIDMap.keys().contains(id))
                {
                    QString prev = QString("_uuid[%1]%2").arg(id,INTERNAL_PARAM_SEP);
                    QStringList values= iLogInstanceIDMap.values(id);
                    QString internal_linenumber = iLogInstanceIDMap.value(id).split("#").last().toUpper();
                    for (const auto& value : values)
                    {
                        QStringList call_line_list = value.split("/");
                        call_line_list.removeLast();
                        QString call_line = call_line_list.join("/");
                        if (call_line == call_linenumber)
                        {
                            internal_linenumber = value;
                            break;
                        }
                    }

                    QString repl = QString("#b##color##darkcyan#( %1 )#darkcyan##color##b#").arg(internal_linenumber);
                    cmdStatement.replace(prev,repl);
                }
            }
        }

        //if there are still uuids in the command statement it means that the relevant logs are missing
        if (cmdStatement.contains(regExp))
        {
            cmdStatement.replace("_uuid[","#b##color##darkcyan#(Internal Parameter: Logs not generated");
            cmdStatement.replace(regExp,"");
            cmdStatement.replace(QString("]%1").arg(INTERNAL_PARAM_SEP),")#darkcyan##color##b#");
        }
    }

    if(pCmd->isTitle())
    {
        GTAActionParentTitle *pTitle = dynamic_cast<GTAActionParentTitle *>(pCmd);
        if(pTitle  != NULL)
        {
            cmdResult = pTitle->getGlobalResultStatus();
            titleHeading = pTitle->getTitle();
            epocTimeStamp = pTitle->getExecutionTime();


            titleComment = pTitle->getComment();
            sUserFeedBack = pTitle->getUserComment();
            sReferences = pTitle->getReferences();
        }
    }
    else if(pCmd->hasReference())
    {
        GTAActionExpandedCall *pExpCall = dynamic_cast<GTAActionExpandedCall *>(pCmd);
        if(pExpCall != NULL)
        {
            cmdResult = pExpCall->getGlobalResultStatus();
            epocTimeStamp = pExpCall->getExecutionTime();

            double epoch = pExpCall->getExecutionEpochTime();
            epochTimeStamp1 = QString::number((epoch*100) / 100.0,'f',4);

            QString callName = pExpCall->getElement();
            sUserFeedBack = pExpCall->getUserComment();
            sReferences = pExpCall->getReferences();
            ioAttachmentList.append(QDir::cleanPath(QString("%1/%2").arg(iGTADataDirectoryPath,callName)));
        }
    }
    else
    {
        //checking if command is a foreach command
        //add the table as attachment
        GTAActionForEach *pForEachCmd = dynamic_cast<GTAActionForEach *>(pCmd);
        if (NULL != pForEachCmd)
        {
            if(pForEachCmd->getRelativePath().isEmpty())
            {
                ioAttachmentList.append(pForEachCmd->getPath());
            }
            else
            {
                QString repoPath = QString::fromStdString(TestConfig::getInstance()->getRepositoryPath());
                ioAttachmentList.append(repoPath + pForEachCmd->getRelativePath());
            }
        }

        cmdResult = pCmd->getExecutionResult();

        //checking if command is a parallel call command
        //add the .pro(s) and .fun(s) as an attachment
        GTAActionCallProcedures *pCallProc = dynamic_cast<GTAActionCallProcedures *>(pCmd);
        if (NULL != pCallProc)
        {
            //DEV: find a way to get the attachments without calling appcontroller
//            foreach(GTACallProcItem item, pCallProc->getCallProcedures())
//            {
//                QString filepath = getAbsoluteFilePathFromUUID(item._UUID);
//                ioAttachmentList.append(filepath);
//            }
            cmdResult = pCallProc->getGlobalResultStatus();
        }

        epocTimeStamp = pCmd->getExecutionTime();

        double epoch = pCmd->getExecutionEpochTime();

        epochTimeStamp1 = QString::number((epoch*100) / 100.0,'f',4);


        sUserFeedBack = pCmd->getUserComment();
        sReferences = pCmd->getReferences();
        isIgnored = pCmd->isIgnoredInSCXML();
        GTAActionPrintTable *pPrintTableCmd = dynamic_cast<GTAActionPrintTable *>(pCmd);
        if(pPrintTableCmd != NULL)
            iPrintTableLst.append(pPrintTableCmd);
    }

    epocTimeStamp.remove("[");
    epocTimeStamp.remove("]");
    //qint64 i64Time = epocTimeStamp.toLongLong();
    QDateTime execTime = QDateTime::fromString(epocTimeStamp);

    QString execDateStr = execTime.date().toString();
    QString execTimeStr = execTime.time().toString();
    QString cmdComments = pCmd->getComment();


    QDomElement timestampElem = iDomDoc.createElement("TIMESTAMP");
    timestampElem.setAttribute("EXEC_TIME",execTimeStr);
    timestampElem.setAttribute("EXEC_DATE",execDateStr);
    if((isLTR) || cmdResult.contains("NA"))
    {
        timestampElem.setAttribute("EPOC_TIME","");//QString::number(i64Time + epocTimeCntr));
        epocTimeCntr++;
    }
    else
        timestampElem.setAttribute("EPOC_TIME",epochTimeStamp1);//QString::number(i64Time));

    QDomElement statusElem = iDomDoc.createElement("STATUS");
    QDomText statusTextNode = iDomDoc.createTextNode(cmdResult);
    statusElem.appendChild(statusTextNode);

    QDomElement statementElem = iDomDoc.createElement("STATEMENT");
    QDomText statementTextNode = iDomDoc.createTextNode(cmdStatement);
    statementElem.appendChild(statementTextNode);

    QDomElement commentsElem = iDomDoc.createElement("COMMENTS");
    QDomText commentTextNode = iDomDoc.createTextNode(cmdComments);
    commentsElem.appendChild(commentTextNode);


    QDomElement feedbackElem = iDomDoc.createElement("USER_FEEDBACK");
    QDomText feedbackTextNode = iDomDoc.createTextNode(sUserFeedBack);
    feedbackElem.appendChild(feedbackTextNode);


    QDomElement refElem = iDomDoc.createElement("DEFECTS");
    QDomText refTextNode = iDomDoc.createTextNode(sReferences);
    refElem.appendChild(refTextNode);

    //V24 DEV
    QString linenumber;
    QDomElement lineElem = iDomDoc.createElement("GTA_LINENUMBER");
    QString id = pCmd->getObjId();
    linenumber = getInstanceNameList(pCmd->getInstanceName()).join("/");

    if (linenumber.isEmpty())
        linenumber = QString("NA");
    QDomText lineTextNode = iDomDoc.createTextNode(linenumber);
    lineElem.appendChild(lineTextNode);

    QString sIsIgnored = isIgnored?"TRUE":"FALSE";

    if(pCmd->getCommandType() == GTACommandBase::ACTION)
    {
        cmdType = "ACTION";
        GTAActionBase *pActCmd = dynamic_cast<GTAActionBase *>(pCmd);
        pActCmd->getTimeOut(cmdTimeValue,cmdTimeUnit);
        pActCmd->getConfCheckTime(cmdConfirmTimeValue,cmdConfirmTimeUnit);
        cmdComplementName = pActCmd->getComplement();
        cmdActionName = pActCmd->getAction();
        if(pCmd->isTitle())
        {
            cmdType = "TITLE";
        }
        if(pCmd->hasReference())
        {
            cmdType = "CALL";
        }
        else if(pCmd->createsCommadsFromTemplate())
        {
            cmdType = "TEMPLATE";

            //keep the kinematic/Template based commands in this section.
            //GTAActionFTAKinematicMultiOutput *pKinematicCmd = dynamic_cast<GTAActionFTAKinematicMultiOutput*>(pCmd);
            //if(pKinematicCmd != NULL)
            //{
            //    iPrintTableLst.append(pKinematicCmd->getPrintTableCmdList());
            //    QString result = pKinematicCmd->getGlobalResultStatus();

            //    GTAScxmlLogMessage msgLogK;
            //    if(!result.isEmpty() && result!= "NA")
            //    {
            //        msgLogK.Result = msgLogK.StringToResultType(result);
            //        msgLogK.LOD = GTAScxmlLogMessage::Main;
            //        logMsgList.append(msgLogK);
            //        pKinematicCmd->setLogMessageList(QList<GTAScxmlLogMessage>()<<msgLogK);
            //    }
            //}
        }
        if(pActCmd->getAction() == ACT_CONDITION)
        {
            cmdType = "CONDITION";
        }
    }
    else if(pCmd->getCommandType() == GTACommandBase::CHECK)
    {
        GTACheckBase * pCheckCmd = dynamic_cast<GTACheckBase *>(pCmd);
        cmdType = "CHECK";
        pCheckCmd->getTimeOut(cmdTimeValue,cmdTimeUnit);
        pCheckCmd->getConfCheckTime(cmdConfirmTimeValue,cmdConfirmTimeUnit);

        cmdComplementName = pCheckCmd->getCheckName();
        cmdActionName = cmdType;
    }
    else
    {
        cmdType = "UNKNOWN";
    }

    if((cmdType == "ACTION") || (cmdType == "CHECK"))
    {
        QDomElement CmdElem = iDomDoc.createElement(cmdType);
        CmdElem.setAttribute("NAME",cmdActionName);
        CmdElem.setAttribute("COM_NAME",cmdComplementName);
        CmdElem.setAttribute("TIMEOUT_VALUE",cmdTimeValue);
        CmdElem.setAttribute("TIMEOUT_UNIT",cmdTimeUnit);
        CmdElem.setAttribute("CONFTIME_VALUE",cmdConfirmTimeValue);
        CmdElem.setAttribute("CONFTIME_UNIT",cmdConfirmTimeUnit);
        CmdElem.setAttribute("IS_IGNORED",sIsIgnored);
        CmdElem.appendChild(timestampElem);
        CmdElem.appendChild(statementElem);
        CmdElem.appendChild(statusElem);
        CmdElem.appendChild(commentsElem);
        CmdElem.appendChild(lineElem);
        if(cmdActionName != ACT_MANUAL)
            CmdElem.appendChild(feedbackElem);

        CmdElem.appendChild(refElem);

        QDomElement otherDetailes = iDomDoc.createElement("OTHER_DETAILS");

        QMap<int, QString> detailMap;
        QHash<QString, QString> detailedInfo = pCmd->getDetailedLogInfo(detailMap);
        QList<int> keys = detailMap.keys();
        for(int i=0;i<keys.count();i++)
        {
            QString paramName = detailMap.value(i);
            QString paramValue =detailedInfo.value(paramName);
            if(cmdActionName == ACT_MANUAL)
            {
                QString tempUpdate =  QString("%1\nUser RunTimeInput: %2").arg(sUserFeedBack, QString("Name:%1:Value:%2").arg(paramName,paramValue));
                QDomElement subFeedbackElem = iDomDoc.createElement("USER_FEEDBACK");
                QDomText subFeedbackTextNode = iDomDoc.createTextNode(tempUpdate);
                subFeedbackElem.appendChild(subFeedbackTextNode);
                CmdElem.appendChild(subFeedbackElem);
            }

            QDomElement paramInfo = iDomDoc.createElement("PARAMINFO");
            //if the parameter is a bench parameter
            if(paramValue.contains("Timestamp"))
            {
                paramInfo.setAttribute("NAME",paramName);

                paramValue.replace("{","");
                paramValue.replace("}","");
                paramValue.replace("\"","");
                QStringList items = paramValue.split(",");
                    
                QDomElement itemElem = iDomDoc.createElement("Name");
                itemElem.appendChild(iDomDoc.createTextNode(paramName));
                paramInfo.appendChild(itemElem);

                for(const QString& item : items)
                {
                    QStringList vals = item.split(":");
                    if(!vals.isEmpty() && !vals.at(0).contains("Timestamp"))
                    {
                        QString key = vals.at(0);
                        QString val = vals.at(1);
                        if(key.contains("Value") && !iDecimal.contains("Default"))
                        {
                            bool ok;
                            int decimalPrecision = iDecimal.toInt(&ok);
                            if(ok)
                            {
                                double dValue = val.toDouble(&ok);
                                if(ok)
                                {
                                    val = QString::number(dValue,'f',decimalPrecision);
                                }
                            }
                        }
                        QDomElement subItemElem = iDomDoc.createElement(key);
                        subItemElem.appendChild(iDomDoc.createTextNode(val));
                        paramInfo.appendChild(subItemElem);
                    }
                }

                //add into the details if the command was KO
                if((cmdResult.contains("KO")))
                {
                    if (detailedInfo.keys().count() >= i+1)
                        otherDetailes.appendChild(paramInfo);
                }
                //else add into the details only for print parameter/table
                else
                {
                    if((cmdActionName == ACT_PRINT) && ((cmdComplementName == COM_PRINT_TABLE) || (cmdComplementName == COM_PRINT_PARAM)))
                    {
                        if (detailedInfo.keys().count() >= i+1)
                            otherDetailes.appendChild(paramInfo);
                    }
                }
            }
            //else the parameter is a local/internal parameter
            else
            {
                paramInfo.setAttribute("NAME",paramName);
                QDomElement itemElem = iDomDoc.createElement(paramName);
                itemElem.appendChild(iDomDoc.createTextNode(paramValue));
                paramInfo.appendChild(itemElem);
                if((cmdActionName == ACT_PRINT) && ((cmdComplementName == COM_PRINT_TABLE) || (cmdComplementName == COM_PRINT_PARAM)))
                {
                    if (detailedInfo.keys().count() >= i+1)
                        otherDetailes.appendChild(paramInfo);
                }
            }
        }
        CmdElem.appendChild(otherDetailes);

        if (cmdActionName == ACT_CALL_PROCS)
        {
            GTAActionCallProcedures *pCallProc = dynamic_cast<GTAActionCallProcedures *>(pCmd);
            if (NULL != pCallProc)
            {
                //here to create xml nodes for all the commands inside call
                QList<GTACommandBase*> childList = pCallProc->getChildren();
                for(int i=0;i<childList.count();i++)
                {
                    GTACommandBase *pChildCmd = childList.at(i);
                    if(pChildCmd != NULL)
                    {
                        //it is known that all commands are call commands. There are no other commands in children
                        getCommandXmlInfo(pChildCmd,CmdElem,iDomDoc,ioAttachmentList,iPrintTableLst,isLTR,iGTADataDirectoryPath,iLogInstanceIDMap);
                    }
                }
            }
        }
        oElem.appendChild(CmdElem);
    }
    else if(cmdType == "TEMPLATE")
    {
        //Update Kinematic Template here.
    }
    else if(cmdType == "TITLE")
    {
        QDomElement titleElem = iDomDoc.createElement("TITLE");

        QDomText commentText = iDomDoc.createTextNode(titleComment);
        QDomText headingText = iDomDoc.createTextNode(titleHeading);
        QDomElement titleCommentElem =  iDomDoc.createElement("COMMENTS");
        QDomElement titleHeadingElem = iDomDoc.createElement("HEADING");


        titleCommentElem.appendChild(commentText);
        titleHeadingElem.appendChild(headingText);

        titleElem.setAttribute("IS_IGNORED",sIsIgnored);
        titleElem.appendChild(titleHeadingElem);
        titleElem.appendChild(titleCommentElem);
        titleElem.appendChild(statusElem);
        titleElem.appendChild(timestampElem);
        //here to create xml nodes for all the commands inside title.
        GTAActionParentTitle *pTitle = dynamic_cast<GTAActionParentTitle *>(pCmd);
        if(pTitle  != NULL)
        {
            QList<GTACommandBase*> childList = pTitle->getChildren();
            for(int i=0;i<childList.count();i++)
            {
                GTACommandBase *pTitleChildCmd = childList.at(i);
                if(pTitleChildCmd != NULL)
                {
                    getCommandXmlInfo(pTitleChildCmd,titleElem,iDomDoc,ioAttachmentList,iPrintTableLst,isLTR,iGTADataDirectoryPath,iLogInstanceIDMap);
                }
            }
        }
        oElem.appendChild(titleElem);

    }
    else if(cmdType == "CONDITION")
    {
        QDomElement CmdElem = iDomDoc.createElement(cmdType);
        CmdElem.setAttribute("NAME",cmdActionName);
        CmdElem.setAttribute("COM_NAME",cmdComplementName);
        CmdElem.setAttribute("TIMEOUT_VALUE",cmdTimeValue);
        CmdElem.setAttribute("TIMEOUT_UNIT",cmdTimeUnit);
        CmdElem.setAttribute("CONFTIME_VALUE",cmdConfirmTimeValue);
        CmdElem.setAttribute("CONFTIME_UNIT",cmdConfirmTimeUnit);
        CmdElem.setAttribute("IS_IGNORED",sIsIgnored);
        CmdElem.appendChild(timestampElem);
        CmdElem.appendChild(statementElem);
        CmdElem.appendChild(statusElem);
        CmdElem.appendChild(commentsElem);
        CmdElem.appendChild(lineElem);

        QDomElement otherDetailes = iDomDoc.createElement("OTHER_DETAILS");

        QMap<int, QString> detailMap;
        QHash<QString, QString> detailedInfo = pCmd->getDetailedLogInfo(detailMap);
        QList<int> keys = detailMap.keys();
        for(int i=0;i<keys.count();i++)
        {
            QString paramName = detailMap.value(i);
            QString paramValue =detailedInfo.value(paramName);
            QDomElement paramInfo = iDomDoc.createElement("PARAMINFO");
            paramInfo.setAttribute("NAME",paramName);

            if(paramValue.contains("Timestamp"))
            {

                paramValue.replace("{","");
                paramValue.replace("}","");
                paramValue.replace("\"","");
                QStringList items = paramValue.split(",");

                for(int k=0;k<items.count();k++)
                {
                    QString item = items.at(k);
                    QStringList vals = item.split(":");
                    if(!vals.isEmpty())
                    {
                        if(!vals.at(0).contains("Timestamp"))
                        {
                            QDomElement itemElem = iDomDoc.createElement(vals.at(0));
                            itemElem.appendChild(iDomDoc.createTextNode(vals.at(1)));
                            paramInfo.appendChild(itemElem);
                        }
                    }
                }
                otherDetailes.appendChild(paramInfo);
            }
        }
        CmdElem.appendChild(otherDetailes);
        oElem.appendChild(CmdElem);

        //here to create xml nodes for all the commands inside title.
        //GTAActionParentTitle *pTitle = dynamic_cast<GTAActionParentTitle *>(pCmd);
        if(pCmd  != NULL)
        {
            QList<GTACommandBase*> childList = pCmd->getChildren();;
            for(int i=0;i<childList.count();i++)
            {
                GTACommandBase *pCondChildCmd = childList.at(i);
                if(pCondChildCmd != NULL)
                {
                    getCommandXmlInfo(pCondChildCmd,CmdElem,iDomDoc,ioAttachmentList,iPrintTableLst,isLTR,iGTADataDirectoryPath,iLogInstanceIDMap);
                }
            }
        }
        oElem.appendChild(CmdElem);
    }
    else if(cmdType == "CALL")
    {
        QDomElement CmdElem = iDomDoc.createElement(cmdType);
        CmdElem.setAttribute("NAME",cmdActionName);
        CmdElem.setAttribute("COM_NAME",cmdComplementName);
        CmdElem.setAttribute("TIMEOUT_VALUE",cmdTimeValue);
        CmdElem.setAttribute("TIMEOUT_UNIT",cmdTimeUnit);
        CmdElem.setAttribute("CONFTIME_VALUE",cmdConfirmTimeValue);
        CmdElem.setAttribute("CONFTIME_UNIT",cmdConfirmTimeUnit);
        CmdElem.setAttribute("IS_IGNORED",sIsIgnored);
        CmdElem.appendChild(timestampElem);
        CmdElem.appendChild(statementElem);
        CmdElem.appendChild(statusElem);
        CmdElem.appendChild(commentsElem);
        CmdElem.appendChild(lineElem);

        //here to create xml nodes for all the commands inside call.
        GTAActionExpandedCall *pExpandedCall = dynamic_cast<GTAActionExpandedCall *>(pCmd);
        if(pExpandedCall  != NULL)
        {
            QList<GTACommandBase*> childList = pExpandedCall->getChildren();
            for(int i=0;i<childList.count();i++)
            {
                GTACommandBase *pTitleChildCmd = childList.at(i);
                if(pTitleChildCmd != NULL)
                {
                    getCommandXmlInfo(pTitleChildCmd,CmdElem,iDomDoc,ioAttachmentList,iPrintTableLst,isLTR,iGTADataDirectoryPath,iLogInstanceIDMap);
                }
            }
        }
        oElem.appendChild(CmdElem);
    }
    else
    {

    }
    return true;
}

bool GTALogReportController::GeneratePrintTableRecord(QList<GTAActionPrintTable*> iPrintTableLst,
                                                         QDomElement &oElem,
                                                         QDomDocument &iDomDoc,
                                                         const QString iOutDir,
                                                         QStringList &ioAttachmentList,
                                                         QString &iGTADBFilePath)
{
    bool generatePrintTableinDoc = TestConfig::getInstance()->getPrintTableStatus();
    QHash<QString, GTAPrintTableRecord> printTableRecord;
    QStringList varList;
    foreach(GTAActionPrintTable *pPrintTableCmd, iPrintTableLst)
    {

        varList.append(pPrintTableCmd->getVariableList());
    }
    varList.removeDuplicates();
    QString dbFile = iGTADBFilePath;
    GTADataController dataCtrlr;
    QStringList invalidParams;
    QHash<QString,GTAICDParameter> tempParamList;
    bool rc = dataCtrlr.getParametersDetail(varList,dbFile,tempParamList,invalidParams);
    if (!iPrintTableLst.isEmpty())
    {
        for (auto* pPrintTableCmd : iPrintTableLst)
        {
            GTAPrintTableTitleRecord titleRecord;
            titleRecord.setTitleName(pPrintTableCmd->getTitleName());
            titleRecord.extractTitleInfo(pPrintTableCmd);
            QString printTableName = pPrintTableCmd->getTableName();
            GTAPrintTableRecord printRecord;
            if(printTableRecord.contains(printTableName))
            {
                printRecord = printTableRecord.value(printTableName);
                printRecord.insertTitleRecord(titleRecord);
            }
            else
            {
                printRecord.insertTitleRecord(titleRecord);
            }

            printRecord.setTableName(printTableName);
            for (const auto& paramName : pPrintTableCmd->getVariableList())
            {
                GTAICDParameter icdParam = tempParamList.value(paramName);
                printTableParamInfo paraInfo;
                paraInfo.paramName = paramName;
                paraInfo.signalType = QString("%1_%2").arg(icdParam.getMedia(),icdParam.getValueType());
                paraInfo.signalUnit = icdParam.getUnit();
                printRecord.insertParamInfo(paramName,paraInfo);
            }
            printTableRecord.insert(printTableName,printRecord);
        }
    }

    for (const auto& printTableName : printTableRecord.keys())
    {
        GTAPrintTableRecord tableRecord = printTableRecord.value(printTableName);
        QDomElement printTableElem = iDomDoc.createElement("PRINT_TABLE");
        QString csvFileName = QString("%1/print_table_%2.csv").arg(iOutDir,printTableName);
        printTableElem.setAttribute("NAME",printTableName);
        QFile csvName(csvFileName);
        rc = csvName.open(QFile::Text| QFile::WriteOnly);
        if(true)
        {
            QTextStream writer(&csvName);
            ioAttachmentList.append(csvFileName);

            QHash<QString,QStringList>  rprtData = tableRecord.getReportData();

            //Start special conditioning
            //foreach implementation
            //better if implemented from widget and print table end [Future]
            //done to enable print table for multiple prints
            int columns=1;
            QStringList header;
            for (const auto pramName : rprtData.keys())
            {
                QStringList colLst = rprtData.value(pramName);
                if (colLst.count()>columns)
                    columns = colLst.count();
            }
            header << tableRecord.getHeader();
            if (tableRecord.getHeader().count() != (columns+3))
            {
                QString tablename = tableRecord.getHeader().last();
                for (int i=0;i<columns-1;i++)
                {
                    header << QString("%1_%2").arg(tablename,QString::number(i));
                }
            }
            //Stop special conditioning

            QString colum = header.join(";");
            QByteArray contents (colum.toUtf8 ());
            writer<<contents;
            writer<<"\n";


            QDomElement headerElem = iDomDoc.createElement("TABLE_HEADER");
            headerElem.setAttribute("VALUE",colum);

            printTableElem.appendChild(headerElem);


            for (const auto& pramName : rprtData.keys())
            {
                QStringList items;
                QString signalType= tableRecord.getParamSignalType(pramName);
                QString unit = tableRecord.getParamSignalUnit(pramName);
                QStringList values = rprtData.value(pramName);
                items.append(pramName);
                items.append(signalType);
                items.append(unit);
                items.append(values);
                colum = items.join(";");
                QByteArray subContents (colum.toUtf8 ());
                writer<< subContents;
                writer<<"\n";

                QDomElement ParamElem = iDomDoc.createElement("PARAMETER_INFO");
                ParamElem.setAttribute("NAME",pramName);
                ParamElem.setAttribute("TYPE",signalType);
                ParamElem.setAttribute("UNIT",unit);
                QDomElement valuesElem = iDomDoc.createElement("VALUES");

                for(int k=0;k<values.count();k++)
                {
                    QDomElement valueElem = iDomDoc.createElement("VALUE");
                    QString paramValue = values.at(k);
                    if(paramValue.contains("Timestamp"))
                    {

                        paramValue.replace("{","");
                        paramValue.replace("}","");
                        paramValue.replace("\"","");
                        QStringList subItems = paramValue.split(",");

                        for(const auto item : subItems)
                        {
                            QStringList vals = item.split(":");
                            if(!vals.isEmpty())
                            {
                                if(vals.at(0).contains("Timestamp"))
                                {
                                    QString epochTime = vals.at(1);
                                    if(epochTime.contains("."))
                                        epochTime.replace(".","");


                                    qint64 i64Time = epochTime.toLongLong();
                                    QDateTime execTime = QDateTime::fromMSecsSinceEpoch(i64Time);
                                    QString execDateStr = execTime.date().toString();
                                    QString execTimeStr = execTime.time().toString();



                                    QDomElement itemElem = iDomDoc.createElement("Time");
                                    itemElem.appendChild(iDomDoc.createTextNode(execDateStr+" "+execTimeStr));
                                    valueElem.appendChild(itemElem);
                                }
                                else
                                {
                                    QDomElement itemElem = iDomDoc.createElement(vals.at(0));
                                    itemElem.appendChild(iDomDoc.createTextNode(vals.at(1)));
                                    valueElem.appendChild(itemElem);
                                }

                            }
                        }
                    }
                    //foreach implementation
                    else if(pramName.contains("line.") && !paramValue.contains("Timestamp"))
                    {
                        QDomElement itemElem = iDomDoc.createElement("Value");
                        itemElem.appendChild(iDomDoc.createTextNode(paramValue));
                        valueElem.appendChild(itemElem);
                    }
                    //adding localparameters in the print table
                    else
                    {
                        QDomElement itemElem = iDomDoc.createElement("Value");
                        itemElem.appendChild(iDomDoc.createTextNode(paramValue));
                        valueElem.appendChild(itemElem);
                    }
                    valuesElem.appendChild(valueElem);
                }


                //                QDomText valueTxt = iDomDoc.createTextNode(values.join(";"));

                ParamElem.appendChild(valuesElem);
                printTableElem.appendChild(ParamElem);

            }
        }
        if(generatePrintTableinDoc)
            oElem.appendChild(printTableElem);
    }
    return true;
}

bool GTALogReportController::getFileInfoListUsed(const QString& iFileName,
                                                    QList <GTAFILEInfo> & lstFileInfo,
                                                    QString &iGTADataDirectoryPath,
                                                    QString &iGTALibraryPath)
{
    bool rc = false;

    GTADocumentController docCtrlr;

    QString sDataDirectory = iGTADataDirectoryPath;
    QString completeFilePath = QString("%1/%2").arg(sDataDirectory,iFileName);

    rc = docCtrlr.getFileInfoFromDoc(completeFilePath,lstFileInfo);
    for(int i=0 ; i<lstFileInfo.count() ; i++)
    {
        if(!(lstFileInfo.at(i)._filePath.contains(":/") || lstFileInfo.at(i)._filePath.startsWith("//")))
            lstFileInfo[i]._filePath = QString("%1%2").arg(iGTALibraryPath,lstFileInfo.at(i)._filePath);

    }

    if (!rc)
        _LastError = docCtrlr.getLastError();

    return rc;
}

bool GTALogReportController::insertHeaderInfoInReport(GTAElement *ipElem, QDomElement &iElem,QDomDocument &iDomDoc, const QString &iGlobalResult)
{
    GTAHeader* pDocHeader = ipElem->getHeader();

    QStringList headerItems;
    QDomElement reportInfoElement;
    if ( pDocHeader!=NULL)
    {
        // REPORTINFO node

        reportInfoElement = iDomDoc.createElement("REPORTINFO");


        iElem.appendChild(reportInfoElement);

        QDateTime currentDateTimeVal = QDateTime::currentDateTime();
        QString stodaysDate = currentDateTimeVal.date().toString();
        QDomElement dateElement = iDomDoc.createElement("DATE");
        QDomText dateNode = iDomDoc.createTextNode(stodaysDate);
        dateElement.appendChild(dateNode);
        reportInfoElement.appendChild(dateElement);

        QString testTitle = ipElem->getName();//mapped to LTR Title if filled
        int desIndex = pDocHeader->getIndex("LTRA_Title");
        if (desIndex!=-1)
            if (!pDocHeader->getFieldValue(desIndex).isEmpty())
                testTitle = pDocHeader->getFieldValue(desIndex);
        QDomElement titleElement = iDomDoc.createElement("TITLE");
        titleElement.appendChild(iDomDoc.createTextNode(testTitle));
        reportInfoElement.appendChild(titleElement);
        headerItems.append("LTRA_Title");


        QString ltrDesignerName; //mapped to Designer name
        desIndex = pDocHeader->getIndex("Designer Name");
        headerItems.append("Designer Name");
        if (desIndex!=-1)
            ltrDesignerName = pDocHeader->getFieldValue(desIndex);

        QDomElement designerNameElement = iDomDoc.createElement("DESIGNERNAME");
        designerNameElement.appendChild(iDomDoc.createTextNode(ltrDesignerName));
        reportInfoElement.appendChild(designerNameElement);


        //Facility used
        QString sfacilityUsed;//mapped to Facility
        desIndex = pDocHeader->getIndex("Facility");
        headerItems.append("Facility");
        if (desIndex!=-1)
            sfacilityUsed = pDocHeader->getFieldValue(desIndex);


        QDomElement ltrFacilityElem = iDomDoc.createElement("FACILITY");
        ltrFacilityElem.appendChild(iDomDoc.createTextNode(sfacilityUsed));

        reportInfoElement.appendChild(ltrFacilityElem);

        QDomElement globalRes = iDomDoc.createElement("GLOBAL_RESULT");
        globalRes.appendChild(iDomDoc.createTextNode(iGlobalResult));
        reportInfoElement.appendChild(globalRes);

        QString ltrReference;//mapped to reference
        desIndex = pDocHeader->getIndex("LTRA_Reference_Number");
        //headerItems.append("LTR_Title_Reference");
        if (desIndex!=-1)
            ltrReference = pDocHeader->getFieldValue(desIndex);

        QDomElement ltrReferenceElement = iDomDoc.createElement("LTRREF");
        ltrReferenceElement.appendChild(iDomDoc.createTextNode(ltrReference));
        reportInfoElement.appendChild(ltrReferenceElement);
        headerItems.append("LTRA_Reference_Number");

        QString gtaVersion = QString(TOOL_VERSION_STR);

        QDomElement ltrGTAVersion = iDomDoc.createElement("GTAVERSION");
        ltrGTAVersion.appendChild(iDomDoc.createTextNode(gtaVersion));
        reportInfoElement.appendChild(ltrGTAVersion);
        // TESTDETAIL node
        QDomElement testDetailElement = iDomDoc.createElement("TESTDETAIL");
        iElem.appendChild(testDetailElement);

        //Test number
        QString sTestNumber;//mapped to reference
        desIndex = pDocHeader->getIndex("Test_Number");
        headerItems.append("Test_Number");
        if (desIndex!=-1)
            sTestNumber = pDocHeader->getFieldValue(desIndex);

        QDomElement testDuration = iDomDoc.createElement("Item");
        QString execDuration = ipElem->getExecutionduration();
        testDuration.setAttribute("value",execDuration);
        testDuration.setAttribute("name","TestDuration");
        testDetailElement.appendChild(testDuration);

        QDomElement ltrTestItem = iDomDoc.createElement("Item");
        ltrTestItem.setAttribute("value", sTestNumber);
        ltrTestItem.setAttribute("name","Test_Number");
        testDetailElement.appendChild(ltrTestItem);

        //Test title
        QString sDescription;//mapped to Description
        desIndex = pDocHeader->getIndex("DESCRIPTION");
        headerItems.append("DESCRIPTION");
        if (desIndex!=-1)
            sDescription = pDocHeader->getFieldValue(desIndex);


        QDomElement ltrTestTitle = iDomDoc.createElement("Item");
        ltrTestTitle.setAttribute("value", sDescription);
        ltrTestTitle.setAttribute("name","Test Title");
        testDetailElement.appendChild(ltrTestTitle);

        QString attachmentStatus = "YES";
        QDomElement attachmentStatusItem = iDomDoc.createElement("Item");
        attachmentStatusItem.setAttribute("value", attachmentStatus);
        attachmentStatusItem.setAttribute("name","Attachment");

        testDetailElement.appendChild(attachmentStatusItem);
        // to be filled from header dom


        //Specification Node

        QDomElement specificatinNode = iDomDoc.createElement("SPECIFICATION");
        iElem.appendChild(specificatinNode);


        QString specificationIssueReq;//mapped to Facility
        desIndex = pDocHeader->getIndex("Specification Issue/Requirement");
        headerItems.append("Specification Issue/Requirement");
        if (desIndex!=-1)
            specificationIssueReq = pDocHeader->getFieldValue(desIndex);

        QStringList specList = specificationIssueReq.split("/");


        if(!specList.isEmpty())
        {
            QDomElement subAttachmentStatusItem = iDomDoc.createElement("Item");
            subAttachmentStatusItem.setAttribute("value", specList.at(0));
            subAttachmentStatusItem.setAttribute("name","Specification issue / section");
            specificatinNode.appendChild(subAttachmentStatusItem);

            if (specList.size()>=2)
            {
                QDomElement subSubAttachmentStatusItem = iDomDoc.createElement("Item");
                subSubAttachmentStatusItem.setAttribute("value", specList.at(1));
                subSubAttachmentStatusItem.setAttribute("name","Requirement");
                specificatinNode.appendChild(subSubAttachmentStatusItem);
            }
        }
        /////////////////////////////////////////////////////////////
        //new node OtherHeaderItems not present in list headerItems;
        int hdrSize = pDocHeader->itemSize();

        QDomElement otherHeaderItemNode = iDomDoc.createElement("OTHERDETAILS");
        for (int i=0;i<hdrSize;i++)
        {
            QString sFieldName = pDocHeader->getFieldName(i);
            if (!headerItems.contains(sFieldName))
            {
                QDomElement itemElement = iDomDoc.createElement("Item");
                itemElement.setAttribute("name",sFieldName);
                desIndex = pDocHeader->getIndex(sFieldName);
                itemElement.setAttribute("value", pDocHeader->getFieldValue(desIndex));

                if (pDocHeader->getShowInLTRA(i))
                {
                    otherHeaderItemNode.appendChild(itemElement);
                }
                    
            }
        }

        iElem.appendChild(otherHeaderItemNode);

    }
    return true;
}

void GTALogReportController::getReferencedAttachmentFiles(GTACommandBase* pCmd, QStringList& ioAttachmentList, const QString& iGTADataDirectoryPath)
{
	QString cmdType = "";
	QString cmdActionName = "";

	if (pCmd->hasReference())
	{
		GTAActionExpandedCall* pExpCall = dynamic_cast<GTAActionExpandedCall*>(pCmd);
		if (pExpCall != NULL)
		{
			const QString callName = pExpCall->getElement();
			ioAttachmentList.append(QDir::cleanPath(QString("%1/%2").arg(iGTADataDirectoryPath, callName)));
		}
	}
	else
	{
		//checking if command is a foreach command
		//add the table as attachment
		GTAActionForEach* pForEachCmd = dynamic_cast<GTAActionForEach*>(pCmd);
		if (NULL != pForEachCmd)
		{
			if (pForEachCmd->getRelativePath().isEmpty())
			{
				ioAttachmentList.append(pForEachCmd->getPath());
			}
			else
			{
				const QString repoPath = QString::fromStdString(TestConfig::getInstance()->getRepositoryPath());
				ioAttachmentList.append(repoPath + pForEachCmd->getRelativePath());
			}
		}
	}

	if (pCmd->getCommandType() == GTACommandBase::ACTION)
	{
		cmdType = "ACTION";

		GTAActionBase* pActCmd = dynamic_cast<GTAActionBase*>(pCmd);
		if (pActCmd)
		{
			cmdActionName = pActCmd->getAction();

			if (pCmd->isTitle())
			{
				cmdType = "TITLE";
			}

			if (pCmd->hasReference())
			{
				cmdType = "CALL";
			}
			else if (pCmd->createsCommadsFromTemplate())
			{
				cmdType = "TEMPLATE";
			}

			if (pActCmd->getAction() == ACT_CONDITION)
			{
				cmdType = "CONDITION";
			}
		}
	}
	else if (pCmd->getCommandType() == GTACommandBase::CHECK)
	{
		cmdType = "CHECK";
		cmdActionName = cmdType;
	}
	else
	{
		cmdType = "UNKNOWN";
	}

	if ((cmdType == "ACTION") || (cmdType == "CHECK"))
	{
		if (cmdActionName == ACT_CALL_PROCS)
		{
			GTAActionCallProcedures* pCallProc = dynamic_cast<GTAActionCallProcedures*>(pCmd);
			if (NULL != pCallProc)
			{
				QList<GTACommandBase*> childList = pCallProc->getChildren();
                for (GTACommandBase* pChildCmd : childList)
				{
					if (pChildCmd != NULL)
					{
                        getReferencedAttachmentFiles(pChildCmd, ioAttachmentList, iGTADataDirectoryPath);
					}
				}
			}
		}
	}
	else if (cmdType == "TITLE")
	{
		GTAActionParentTitle* pTitle = dynamic_cast<GTAActionParentTitle*>(pCmd);
		if (pTitle != NULL)
		{
			QList<GTACommandBase*> childList = pTitle->getChildren();
            for (GTACommandBase* pTitleChildCmd : childList)
			{
				if (pTitleChildCmd != NULL)
				{
                    getReferencedAttachmentFiles(pTitleChildCmd, ioAttachmentList, iGTADataDirectoryPath);
				}
			}
		}
	}
	else if (cmdType == "CONDITION")
	{
		if (pCmd != NULL)
		{
			QList<GTACommandBase*> childList = pCmd->getChildren();
            for (GTACommandBase* pCondChildCmd : childList)
			{
				if (pCondChildCmd != NULL)
				{
                    getReferencedAttachmentFiles(pCondChildCmd, ioAttachmentList, iGTADataDirectoryPath);
				}
			}
		}
	}
	else if (cmdType == "CALL")
	{
		GTAActionExpandedCall* pExpandedCall = dynamic_cast<GTAActionExpandedCall*>(pCmd);
		if (pExpandedCall != NULL)
		{
			QList<GTACommandBase*> childList = pExpandedCall->getChildren();
            for (GTACommandBase* pCallChildCmd : childList)
			{
				if (pCallChildCmd != NULL)
				{
                    getReferencedAttachmentFiles(pCallChildCmd, ioAttachmentList, iGTADataDirectoryPath);
				}
			}
		}
	}
}