//#ifndef GTAFSIICONTROLLER_H
//#define GTAFSIICONTROLLER_H

//#include "GTAElement.h"
//#include "GTACommandBase.h"
//#include "GTACommandList.h"
//#include "GTAFSIIFile.h"
//#include "GTAFSIICommandBase.h"
//#include "GTAFSIICommandList.h"
//#include <QSqlDatabase>
//#include <QFile>
//#include <QFileInfo>


//#define PARAM_NOT_FOUND " Parameter not found in database"



//class GTAFSIIController
//{
//    QHash<QString,QString> _ExternalXmlPathMap;
//    QHash<QString, GTAElement*> _TestProcElemMap;
//    QHash<QString, GTAElement*> _FunctionElemMap;


//public:
//    GTAFSIIController();

//    static GTAFSIIController* getFSIIController();
//    bool parseFiles(QSqlDatabase &iParamDB, const QString &iImportPath, const QString &iErrorPath);
//    bool parseFile(QSqlDatabase &iParamDB, const QFileInfo &iFSIIFile, const QString &iErrorPath);
//    void convertToGTA(QList<GTAFSIIFile> fs2Data);
//    void createProcedure(GTAFSIIFile &iFS2Data);
//    void createFunction(GTAFSIIFile &iFS2Data);
//    void convertToGTACommands(QList<GTAFSIICommandBase*> &iFS2CmdList,QList<GTACommandBase*> &oCmdList,const QString iCallCmdRelPath);
//    GTACommandBase* createSetCommand(GTAFSIISet *pFS2Set);
//    GTACommandBase* createWaitForCommand(GTAFSIIWait *pFS2Wait);
//    GTACommandBase* createWaitUntilCommand(GTAFSIIWaitWhile *pFS2WaitWhile);
//    GTACommandBase* createPrintMessageCommand(GTAFSIILogEntry* pFS2LogEntry);
//    GTACommandBase* createCallCommand(GTAFSIIFunctionCall* pFunctionCall,const QString &iCallCmdRelPath);
//    GTACommandBase* createConditionWhileCommand(GTAFSIIRunXTimes* pRunXTimes,const QString iCallCmdRelPath);
//    GTACommandBase* createConditionWhileCommand(GTAFSIILoop* pLoop,const QString iCallCmdRelPath);
//    GTACommandBase* createManualActionCommand(GTAFSIIPause* pPause);
//    GTACommandBase* createIfCommand(GTAFSIIIf* pFS2If);
//    GTACommandBase* createCheckValueCommand(GTAFSIICheck* pFS2Check);
//    QHash<QString,QString> getExternalXmlPathMap()const;
//    QHash<QString, GTAElement*>  getTestProcElementMap();
//    QHash<QString, GTAElement*> getFunctionElementMap();
//    void processFSParameters();
//    QString processFunctionalStatus(const QString &iParamName,const QString &iParamVal);
//private:
//    static GTAFSIIController* _pFSIIController;
//    QSqlDatabase _ParameterDatabase;
//    QHash<QString,QString> _ParameterMap;
//    QHash<QString, GTAFSIISignal> _FSParamAndSignalType;
//    QFile _ErrorFile;

//};

//#endif // GTAFSIICONTROLLER_H
