#ifndef GTALOGREPORTCONTROLLER_H
#define GTALOGREPORTCONTROLLER_H

#include "GTAControllers.h"
#include "GTAElement.h"
#include "GTAEditorLogModel.h"
#include "GTAActionFTAKinematicMultiOutput.h"
#include "GTAHeader.h"
#include "GTADataController.h"
#include "GTAScxmlLogMessage.h"
#include "GTAICDParameter.h"
#include "GTADocumentController.h"

#pragma warning(push, 0)
#include <QAbstractItemModel>
#include <QDomDocument>
#include <QCoreApplication>
#include <QDir>
#pragma warning(pop)

class GTALogReportController
{
public:
    GTALogReportController();
    ~GTALogReportController();

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
    bool getLogModel(QString &iLogFullPath,
                     GTAElement*& ipTitleBasedElement,
                     QHash<QString, QString> &ioLogInstanceIDMap,
                     GTAEditorLogModel*& oModel,
                     QString &oErrorMessages);

    /**
      * This function reads the log for a sequence with multiple logs
      * @iLogFile: input file to be parsed
      * @oLogNames: list of log file names in the iLogFile
      * @oProcNames: list of procedure names in the iLogFile
      * @oStatus: list of procedure result status in the iLogFile
      * @oGlobalStatus: global status computed from the list of status
      */
    bool readSequenceMultipleLogFile(const QString &iLogFile,
                                     QStringList &oLogNames,
                                     QStringList &oProcNames,
                                     QStringList &oStatus,
                                     QString &oGlobalStatus);
    /**
      * This function provides the last error in the controller instance
      * @return: error description in the form of QString
      */
    QString getLastError() { return _LastError; }
    /**
      * This function checks if the sequence is executed with multiple logs
      * @iLogFile: absolute path of the logFile
      * @return: true/false based on single/multiple logs
      */
    bool isSequenceWithMultipleLogs(const QString &iLogFile);

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
    void getLogModelForSequence(GTAElement*& ipTitleBasedElement,
                                QAbstractItemModel*& oModel,
                                QString &oErrorLogList,
                                QString &ioGlobalStatus,
                                QStringList &iProcNames,
                                QStringList &iLogNames,
                                QStringList &iExecStatus);

    /**
      * This function export the lab test report
      * @ipLogModel: Model in UI to be exported, it should of type GTAEditorTreeModel class
      * @iReportFile: Report file full path where the report is created
      * @iAttachmentList: list of documents to be attached with report
      * @exportLTRA: struct to update the parameters used for exporting LTRA containing save format, timing precission, isLTRA
      */
    bool exportLTRAToDocx(QAbstractItemModel *ipLogModel,
                          const QString &iReportFile,
                          const QStringList &iAttachmentList,
                          bool isLTR,
                          QString saveFormat,
                          QString saveInMs,
                          QString &iGTADataDirectoryPath,
                          QHash<QString, QString> &iLogInstanceIDMap,
                          QString &iGTADBFilePath);

private:
    void updateReportStatus(GTAElement *ipElem);
    bool loadTemplateFile(const QString &iTmplFileName,
                          const GTAActionFTAKinematicMultiOutput *pKinematciCmd,
                          GTAElement *& opElement);
    bool getElementFromTemplate(const QString & iFileName, GTAElement *& opElement);

    /**
      * This function export the LTRA-Lab Test Report Analysis to xml
      * @pElem: Element to be exported
      * @iReportFile: Report file full path where the report is created
      * @iAttachmentList: list of documents to be attached with report
      */
    bool exportLTRAToXml(GTAElement * pElem,
                         const QString & iReportFile,
                         QStringList & iAttachmentList,
                         bool isLTR,
                         QString &iGTADataDirectoryPath,
                         QHash<QString, QString> &iLogInstanceIDMap,
                         QString &iGTADBFilePath);

    bool getCommandXmlInfo(GTACommandBase * pCmd,
                           QDomElement &oElem,
                           QDomDocument &iDomDoc,
                           QStringList &ioAttachmentList,
                           QList<GTAActionPrintTable*> &iPrintTableLst,
                           bool isLTR,
                           QString iGTADataDirectoryPath,
                           QHash<QString, QString> &iLogInstanceIDMap);

    bool GeneratePrintTableRecord(QList<GTAActionPrintTable*> iPrintTableLst,
                                  QDomElement &oElem,
                                  QDomDocument &iDomDoc,
                                  const QString iOutDir,
                                  QStringList &ioAttachmentList,
                                  QString &iGTADBFilePath);

    bool getFileInfoListUsed(const QString& iFileName,
                             QList <GTAFILEInfo> & lstFileInfo,
                             QString &iGTADataDirectoryPath,
                             QString &iGTALibraryPath);

    bool insertHeaderInfoInReport(GTAElement *ipElem, QDomElement &iElem,QDomDocument &iDomDoc, const QString &iGlobalResult);

    /**
      * This function returns the line number of a specific command by deriving it out of the instance name
      * eg. test[0]#Call - abc.fun[1]#Wait for 1s ==> QStringlist([0],[1])
      * @iInstanceName: Instance name of command whose line number needs to be identified
      * @return: List of line number based on call level
      */
    QStringList getInstanceNameList(const QString &iInstanceName) const;

    void getReferencedAttachmentFiles(GTACommandBase* pCmd, QStringList& ioAttachmentList, const QString& iGTADataDirectoryPath);

    QString _LastError;
};

class GTAControllers_SHARED_EXPORT GTAPrintTableTitleRecord
{
public:
    void setTitleName(const QString &iName){_Title = iName;}
    QString getTitleName() const{return _Title;}
    void extractTitleInfo(const GTAActionPrintTable *pCmd)
    {
        _Title = pCmd->getTitleName();
        QList<GTAScxmlLogMessage> logMsgList = pCmd->getLogMessageList();
        if(!logMsgList.isEmpty())
        {
            foreach(GTAScxmlLogMessage logMsg, logMsgList)
            {
                if(logMsg.LOD == GTAScxmlLogMessage::Detail)
                {
                    QString expVal = logMsg.ExpectedValue;
                    QString curVal = logMsg.CurrentValue;
                    if(_VarValMap.contains(expVal))
                    {
                        QString val = _VarValMap[expVal];
                        QStringList vals;
                        vals.append(val);
                        vals.append(curVal);
                        _VarValMap[expVal] = vals.join(";");
                    }
                    else
                        _VarValMap.insert(expVal,curVal);
                }
            }
        }
        else
        {
            foreach(QString var,pCmd->getVariableList())
            {
                _VarValMap.insert(var,"0");
            }
        }
    }
    QHash<QString,QString> getVarValInfo() const {return _VarValMap;}

private:
    QHash<QString,QString> _VarValMap;
    QString _Title;
};

struct GTAControllers_SHARED_EXPORT printTableParamInfo
{
    QString paramName;
    QString signalType;
    QString signalUnit;
};

class GTAControllers_SHARED_EXPORT GTAPrintTableRecord
{
public:
    void setTableName(const QString &iName) {_TableName = iName;}
    QString getTableName() const{return _TableName;}
    void insertTitleRecord(GTAPrintTableTitleRecord iTitleRecord){_TitleRecordList.append(iTitleRecord);}
    QList<GTAPrintTableTitleRecord> getTitleRecord() const{return _TitleRecordList;}

    QHash<QString,QStringList> getReportData()
    {
        _Header.append("Parameter");
        _Header.append("Signal Type");
        _Header.append("Unit");
        for (const auto& titleRecord : _TitleRecordList)
        {
            _Header.append(titleRecord.getTitleName());
            QHash<QString,QString> varInfo = titleRecord.getVarValInfo();

            for (auto param : varInfo.keys())
            {
                QString val = varInfo.value(param);
                if(_varInfo.contains(param))
                {
                    QStringList vals = _varInfo.value(param);
                    if(val.contains(";"))
                        vals.append(val.split(";"));
                    else
                        vals.append(val);

                    _varInfo[param] = vals;
                }
                else
                {
                    QStringList vals;
                    if(val.contains(";"))
                        vals.append(val.split(";"));
                    else
                        vals.append(val);
                    _varInfo.insert(param,vals);
                }
            }
        }
        return _varInfo;
    }



    void insertParamInfo(const QString &iName, const printTableParamInfo& iParamInfo)
    {
        if(!_paramInfoHash.contains(iName))
        {
            _paramInfoHash.insert(iName,iParamInfo);
        }
    }
    QStringList getHeader(){return _Header;}
    QString getParamSignalType(const QString &iParamName){return _paramInfoHash.value(iParamName).signalType;}
    QString getParamSignalUnit(const QString &iParamName){return _paramInfoHash.value(iParamName).signalUnit;}
private:
    QHash<QString,QStringList> _varInfo;
    QString _TableName;
    QStringList _Header;
    QList<GTAPrintTableTitleRecord> _TitleRecordList;
    QHash<QString, printTableParamInfo> _paramInfoHash;
};

#endif // GTALOGREPORTCONTROLLER_H
