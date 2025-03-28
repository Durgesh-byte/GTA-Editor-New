#include "GTAAppController.h"
#include "GTAOneClickTestRig.h"
#include "GTAOneClickApplications.h"
#include "GTAOneClickConfiguration.h"
#include "GTAOneClickOptions.h"
#include "GTAScxmlLogParser.h"
#include "GTAOneClickLaunchApplicationApplications.h"
#include "GTAOneClickLaunchApplicationOptions.h"
#include "GTAHeader.h"
#include "GTAHeaderTemplateParser.h"

#include "GTAICDParser.h"
#include "GTAUtil.h"
#include "GTAHeaderTableModel.h"
#include "GTAConfigurationFileParser.h"
#include "GTAElementViewModel.h"
#include "GTACommandBase.h"
#include "GTAActionIfEnd.h"
#include "GTAActionIF.h"
#include "GTAActionElse.h"
#include "GTAActionCallProcedures.h"
#include "GTAExportScxml.h"
#include "GTADocumentController.h"
#include "GTAExportScxml.h"
#include "GTASCXMLControllerNew.h"
#include "GTAGenericToolCommand.h"
#include "GTAActionForEach.h"
#include "GTAActionCall.h"
#include "GTAEditorLogModel.h"
#include "GTACheckCompatibility.h"
#include "GTAActionTitle.h"
#include "GTACommandConfig.h"
#include "GTAViewController.h"
#include "GTAGenericSCXMLController.h"
#include "GTATreeViewModel.h"
//#include "GTAPluginFileParserBase.h"
#include "GTAEditorViewController.h"
#include "GTAMdiController.h"
#include "GTACommandList.h"
//#include "GTASequencerMain.h"
#include "GTATitleBasedExecutionModel.h"
#include "GTAValidationWidgetModel.h"
#include "GTAActionForEachSerializer.h"
#include "GTALogReportController.h"
#include <chrono>
#include <sstream>
#include <algorithm>
#include "GTATestConfig.h"

#pragma warning(push, 0)
#include <QFile>
#include <QSqlQuery>
#include <QDateTime>
#include <QDomProcessingInstruction>
#include <QXmlQuery>
#include <QCoreApplication>
#include <QProgressBar>
#include <QProgressDialog>
#include <QUuid>
#include <QSortFilterProxyModel>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QDir>
#include <QFutureWatcher>
#include <QtConcurrentRun>
#include <QHeaderView>
#pragma warning(pop)

//TODO: Fix overshadowing
#pragma warning (disable : 4456)
//TODO: Fix conversion from copy to ref
#pragma warning (disable : 4239)
GTAAppController* GTAAppController::_pAppController=0;
GTAAppController::GTAAppController(QObject* ipObject):QObject(ipObject)
{
    _pSystemServices = NULL;
    _pViewController = GTAViewController::getGTAViewController(this);

    _pGenericToolController = new GTAGenericToolController();
    _treeRootNode = NULL;
    _pEquipmentModel = NULL;

    pLaunchSCXMLProcess = NULL;
    _pPluginInterface = nullptr;
    _pSequencerMain = NULL;
    _isSequencerRunning = false;
    _pSVNController = NULL;
    _plugins.clear();
	_dbDataController = new GTADbDataController();
	connect(_dbDataController, SIGNAL(onDuplicateUUIDDetected(QString, QString)), this, SLOT(onDuplicateUUIDFound(QString, QString)));

}

GTAAppController* GTAAppController::getGTAAppController(QObject* ipObject)
{
    if(_pAppController==0)
        _pAppController = new GTAAppController(ipObject);
    return _pAppController;
}

GTAAppController::~GTAAppController()
{
    if(_treeRootNode != NULL)
    {
        delete _treeRootNode;
        _treeRootNode = NULL;
    }
    if(_pEquipmentModel != NULL)
    {
        delete _pEquipmentModel;
        _pEquipmentModel = NULL;
    }
    if(_pSystemServices != NULL)
    {
        delete _pSystemServices;
        _pSystemServices = NULL;
    }
    //DEV comment:memory leak plug
    if(_pPluginInterface != NULL)
    {
        delete _pPluginInterface;
        _pPluginInterface = NULL;
    }
    //DEV comment:memory leak plug
    if (_pGenericToolController != NULL)
    {
        delete _pGenericToolController;
        _pGenericToolController = NULL;
    }
    if (_pViewController != NULL)
    {
        delete _pViewController;
        _pViewController = NULL;
    }
    if(_pSVNController != NULL)
    {
        delete _pSVNController;
        _pSVNController = NULL;
    }



    if (_dbDataController != NULL)
    {
        delete _dbDataController;
        _dbDataController = NULL;
    }


    foreach(QPluginLoader* pluginLoader, _plugins)
    {
        pluginLoader->unload();
        delete pluginLoader;
        pluginLoader = NULL;
    }
    _plugins.clear();
}
void GTAAppController::onToolClosure()
{
    if(_treeRootNode != NULL)
    {
        storeFavoritesInXml();
    }

    _HistoryVarList.clear();
    updateHistoryTable(_CurrentSessionVarList);
    commitDatabase();
    closeDatabase();
    if(_pSystemServices != NULL)
    {
        commitSessionData();
        cleanTemporaryDirectory();
    }
}

bool GTAAppController::isSequencerOpen()
{
    return _isSequencerRunning;
}

bool GTAAppController::CloseSequencer()
{
    bool retValue;
    emit closeSequencer(retValue);
    return retValue;
}

void GTAAppController::writeTreeNode(QDomDocument *iDomDoc,QDomElement &oElement, const GTATreeItem *pItem)
{
    if(pItem)
    {
        QDomElement domElement;
        if(pItem->isFavourite())
        {
            domElement = iDomDoc->createElement("NODE");
            QString name = pItem->getName();
            QString absoluteRelPath = pItem->getAbsoluteRelPath();
            QString relativePath = pItem->getRelativePath();
            QString extension = pItem->getExtension();
            domElement.setAttribute("Name",name);
            domElement.setAttribute("AbsoluteRelPath",absoluteRelPath);
            domElement.setAttribute("RelativePath",relativePath);
            domElement.setAttribute("Extension",extension);
            oElement.appendChild(domElement);
            //            iDomDoc->appendChild(oElement);
        }
        if(pItem->hasChildren())
        {
            QList<GTATreeItem*> children = pItem->getChildren();
            for(int i = 0; i < children.count(); i++)
            {
                GTATreeItem *childItem = children.at(i);
                writeTreeNode(iDomDoc,oElement,childItem);
            }
        }
    }
}

void GTAAppController::storeFavoritesInXml()
{
    if(_treeRootNode != NULL && _treeRootNode->hasChildren())
    {
        QString favFile = _pSystemServices->getFavoritesFilePath();
        QFileInfo fileInfo(favFile);
        QDir dir;
        if(fileInfo.exists())
        {
            QString repositoryPath = fileInfo.absolutePath();
            dir.cd(repositoryPath);
            if(dir.exists())
                dir.remove(favFile);
        }

        QDomDocument *domDoc = new QDomDocument();
        QDomProcessingInstruction procInstruct = domDoc->createProcessingInstruction("xml","version=\"1.0\"");
        domDoc->appendChild(procInstruct);
        QDomElement domElement = domDoc->createElement("Favorites");
        domDoc->appendChild(domElement);
        writeTreeNode(domDoc,domElement,_treeRootNode);


        QFile file(favFile);
        if(file.open(QIODevice::WriteOnly | QFile::Text))
        {
            QTextStream stream( &file );
            stream << domDoc->toString();
            file.close();
        }

        //DEV comment:memory leak plug
        if (domDoc != NULL)
        {
            delete domDoc;
            domDoc = NULL;
        }
        //

    }
}

bool GTAAppController::isGenericSCXMLMode()
{
    return _pSystemServices->isGenerateGenericSCXML();
}

bool GTAAppController::setProgramConfiguration()
{
    TestConfig* testConf = TestConfig::getInstance();
    bool rc = true;

    if (_pSystemServices == NULL)
    {
        QString sLibraryPath = QString::fromStdString(testConf->getLibraryPath());
        QString sRepositoryPath = QString::fromStdString(testConf->getRepositoryPath());
        QString sToolDataPath = QString::fromStdString(testConf->getToolDataPath());
        QString sLogDirPath = QString::fromStdString(testConf->getLogDirPath());


        QFileInfo sLibraryInfo(sLibraryPath);
        QFileInfo sRepositorynfo(sRepositoryPath);
        QFileInfo sToolDataInfo(sToolDataPath);
        QFileInfo logDirInfo(sLogDirPath);

        if (!sLibraryInfo.exists())
        {
            _LastError = "Library path does not exist";
            qWarning() << "Library path does not exist";
            rc = false;
        }
        if (!sRepositorynfo.exists())
        {
            _LastError = "Repository path does not exist";
            qWarning() << "Repository path does not exist";
            rc = false;
        }
        if (!sToolDataInfo.exists())
        {
            _LastError = "Tool data path does not exist";
            qWarning() << "Tool data path does not exist";
            rc = false;
        }
        if (!logDirInfo.exists()) // not mandatory
        {
            qWarning() << "Log path does not exist";
        }

        if (!rc && _displayConsole) {
            {
                QTextStream out(stdout);
                out << "Warning: " << _LastError << "\n";
            }
        }
        _pSystemServices = GTASystemServices::getSystemServices();

        GTAUtil::setRepositoryPath(sRepositoryPath);
        GTAUtil::setToolDataPath(sToolDataPath);
    }

    if (rc)
    {
        QList<GTALiveToolParams> gtaLiveToolList;
		QList<QString> toolList, tempList;
		GTALiveToolParams toolParam;
        toolList = QString::fromStdString(testConf->getToolList()).split(":");
        toolList.removeLast();

		for (int i = 0; i < toolList.size(); i++)
		{
			tempList = toolList.at(i).split(',');
			toolParam.toolName = tempList.first();
			toolParam.toolIP = tempList.at(1);
			toolParam.port = tempList.last().toInt();
			gtaLiveToolList.insert(i, toolParam);
		}

        _pSystemServices->updateServiceSettings(
            QString::fromStdString(testConf->getDecimalPlaces()),
            QString::fromStdString(testConf->getLogMessages()),
            testConf->getLogEnable(),
            QString::fromStdString(testConf->getManualActionTimeoutValue()),
            testConf->getManualActionTimeoutStatus(),
            testConf->getVirtualPMR(),
            testConf->getResizeRow(),
            testConf->getPrintTableStatus(),
            testConf->getBenchDBStartup(),
            testConf->getProcDBStartup(),
            testConf->getSaveBeforeExport(),
            testConf->getChannelSelection(),
            QString::fromStdString(testConf->getEngineSelection()),
            testConf->getPIRInvokeForSCXML(),
            testConf->getGenericSCXML(),
            testConf->getPIRWaitTime(),
            QString::fromStdString(testConf->getPIRWaitTimeVal()),
            testConf->getNewSCXMLStruct(),
            testConf->getStandAloneExecMode(),
            testConf->getENInvokeLocalVar(),
            testConf->getNewLogFormat(),
            testConf->getInitSubscribeStart(),
            testConf->getInitSubscribeEnd(),
            testConf->getUSCXMLBasedSCXMLEN(),
            QString::fromStdString(testConf->getDebugModeIP()),
            testConf->getDebugModePort(),
            testConf->getValidationEnable(),
            gtaLiveToolList,
            QString::fromStdString(testConf->getBISGIP()),
            testConf->getBISGPort());
        //PR1969:switching of ini was causing stale db connection which resulted parameter not found in db error . So, triggering updateDatabase() which disconnects stale db connections and initiates new db connection
        updateDatabase();
	}

    return rc;
}

bool GTAAppController::setProgramConfiguration(
                                                      const QString& iDecimalPlace,
                                                      const QString& iLogMessage,
                                                      bool iLogEnable,
                                                      bool ibSaveElementDuringExportScxml,
                                                      bool channelSelectionSetting,
                                                      const QString& engineSetting,
                                                      bool iPirInvokeForSCXML,
                                                      bool ibGenericSCXMLState,
                                                      bool isPirWaitTimeSet, const QString& iTime, bool isNewSCXMLStruct,
                                                      bool iVirtualPMR,
                                                      bool iResizeRow,
                                                      bool iStandaloneExecMode,
                                                      bool iBenchDbStartup,
                                                      bool iProcDbStartup,
                                                      bool iPrintTableStatus,
                                                      bool iEnInvokeForLocalVar,
                                                      bool iNewLogFormat,
                                                      bool iInitStartSubscribeStart,
                                                      bool iInitStartSubscribeEnd,
                                                      const QString& iManualActTimeoutVal,
                                                      bool iManualActTimeoutStat,
                                                      bool iUSCXMLBasedSCXMLEn,
                                                      QString idebugIP,
                                                      int idebugPort,
                                                      bool iValidationWidgetEnabled,
                                                      QList<GTALiveToolParams>& iLiveToolList,
                                                      QString iLiveGtaBisgIP,
                                                      int iLiveGtaBisgPort)

{
    TestConfig* testConf = TestConfig::getInstance();
    bool rc = true;

    if (_pSystemServices == NULL)
    {
        // read ini file
        QString sLibraryPath = QString::fromStdString(testConf->getLibraryPath());
        QString sRepositoryPath = QString::fromStdString(testConf->getRepositoryPath());
        QString sLogDirPath = QString::fromStdString(testConf->getLogDirPath());

        GTAUtil::setRepositoryPath(sRepositoryPath);
        GTAUtil::setToolDataPath(QString::fromStdString(testConf->getToolDataPath()));

        QString sDefaultPath = QDir::cleanPath(QString("c:/users/public"));
        QFileInfo sLibraryInfo(sLibraryPath);
        if (!sLibraryInfo.exists())
        {
            _LastError = "Library path does not exist";
            if (sLibraryPath.isEmpty())
            {
                testConf->setLibraryPath(sDefaultPath.toStdString());
                GTAUtil::setRepositoryPath(sDefaultPath);
            }
            rc = false;
        }
        QFileInfo sRepositorynfo(sRepositoryPath);
        if (!sRepositorynfo.exists())
        {
            _LastError = "Repository path does not exist";
            if (sRepositoryPath.isEmpty())
                testConf->setRepositoryPath(sDefaultPath.toStdString());
            rc = false;

        }
        
        QFileInfo logDirInfo(sLogDirPath);
        if (!logDirInfo.exists())
        {
            sLogDirPath = _pSystemServices->getDefaultLogDirectory();
            testConf->setLogDirPath(sLogDirPath.toStdString());
        }
    }

    if (rc)
    {
        QList<GTALiveToolParams> gtaLiveToolList;
        QList<QString> toolList, tempList;
        GTALiveToolParams toolParam;
        toolList = QString::fromStdString(testConf->getToolList()).split(":");
        toolList.removeLast();

        for (int i = 0; i < toolList.size(); i++)
        {
            tempList = toolList.at(i).split(',');
            toolParam.toolName = tempList.first();
            toolParam.toolIP = tempList.at(1);
            toolParam.port = tempList.last().toInt();
            gtaLiveToolList.insert(i, toolParam);
        }

		_pSystemServices->updateServiceSettings(
            iDecimalPlace,
			iLogMessage,
			iLogEnable,
			iManualActTimeoutVal,
			iManualActTimeoutStat,
			iVirtualPMR,
			iResizeRow,
			iPrintTableStatus,
			iBenchDbStartup,
            iProcDbStartup,
			ibSaveElementDuringExportScxml,
			channelSelectionSetting,
			engineSetting,
			iPirInvokeForSCXML,
			ibGenericSCXMLState,
			isPirWaitTimeSet,
			iTime,
			isNewSCXMLStruct,
			iStandaloneExecMode,
			iEnInvokeForLocalVar,
			iNewLogFormat,
			iInitStartSubscribeStart,
			iInitStartSubscribeEnd,
			iUSCXMLBasedSCXMLEn,
			idebugIP,
			idebugPort,
			iValidationWidgetEnabled,
			iLiveToolList,
			iLiveGtaBisgIP,
			iLiveGtaBisgPort);
		updateDatabase();

	}
	return rc;
}


bool GTAAppController::isEditableCommand(GTACommandBase* pCmd)
{
    bool bOK = true;
    if (NULL!=pCmd)
        bOK = pCmd->IsUserEditable();
    return bOK;
}


//DEAD CODE
QList<GTAHeader *>  GTAAppController::getHeaderList()
{
    QList<GTAHeader *> oList;
    return oList;
}

//DEAD CODE
bool GTAAppController:: setInitialHeaderView(QTableView*)
{

    //    QString headerTemplateFilePath=_pSystemServices->getHeaderTemplateFilePath();

    //    GTADocumentController docController;
    //    GTAHeader* pHeader=NULL;
    //    GTAElement* pElement=NULL;
    bool rC = false;
    //    rC = docController.openDocument(headerTemplateFilePath,pElement,pHeader,false);


    //    if (rC)
    //    {
    //        rC = _pViewController->setHeaderView(ipView,pHeader);
    //        if (rC)
    //        {
    //            GTAElement* pElement = _pViewController->getElement();
    //            if (NULL!= pElement  && NULL!= pHeader)
    //                pElement->setHeader(pHeader);
    //        }
    //    }
    //    else
    //    {
    //        ipView->setModel(NULL);
    //        _LastError=docController.getLastError();
    //    }

    return rC;
}
//DEAD CODE
bool GTAAppController:: setInitialEditorView(QTableView*)
{
    //GTAViewController* _pViewController = GTAViewController::getGTAViewController(this);
    //    if(_pViewController)
    //        return _pViewController->setInitialEditorView(ipView);
    //    else
    return true;
}

//DEAD CODE
void GTAAppController::addEditorRows(const int&)
{

    //    //GTAViewController* _pViewController = GTAViewController::getGTAViewController(this);
    //    if(_pViewController)
    //        _pViewController->addEditorRows(iSelectedRow);
}
//DEAD CODE
bool GTAAppController::areSelectedCommandsBalanced(QList<int>&)
{
    return true;
    //    return _pViewController->isSelectedCommandsBalanced(ioLstSelectedRows);
}
//DEAD CODE
bool GTAAppController::areSelectedCommandsNested(const QList<int>&)
{
    return true;
    //    return _pViewController->areSelectedCommandsNested(iLstSelectedRows);
}

void GTAAppController::updateParameters()
{
    QList<GTAICDParameter*> oParamList;
    QStringList icdFiles = _pSystemServices->getICDFiles();
    QStringList pirFiles = _pSystemServices->getPIRFiles();
    QStringList pmrFiles = _pSystemServices->getPMRFiles();
    icdFiles.append(pirFiles); icdFiles.append(pmrFiles);
    foreach(QString file, icdFiles)
    {
        GTAICDParser parser = GTAICDParser(file);

        // Get the fullpath and the last modification
        QFile newFile(file);
        QFileInfo fileInfo;
        fileInfo.setFile(newFile);
        QDateTime created = fileInfo.lastModified();

        if(!_databaseFiles.contains(file))
        {
            _databaseFiles.insert(file, created);

            parser.parse();
            parser.getParameterList(oParamList);
        }
        else
        {
            // Detection of any modification on a file already in database
            if(_databaseFiles.value(file) != created)
            {
                parser.parse();
                parser.getParameterList(oParamList);
            }
        }
//        parser.parse();
//        parser.getParameterList(oParamList);
    }
}

QHash<QString,QString> GTAAppController::getImageList(const QString &, const QString &,
                                                         const QString &) const
{
    QHash<QString,QString> oImageList;
    QStringList imgList = _pSystemServices->getImageFiles();
    foreach(QString imgPath, imgList)
    {
        QFileInfo imgFileInfo(imgPath);
        QString fileName = imgFileInfo.fileName();
        oImageList.insert(fileName,imgPath);
    }
    return oImageList;
}
QList<GTAMessage> GTAAppController::getMessageList(const QString &, GTAMessage::MessageType) const
{
    QList<GTAMessage> oMessageList;
    return oMessageList;
}

//DEAD CODE
void GTAAppController::deleteEditorRows(const QList<int>&)
{

    //    if (_pViewController)
    //        _pViewController->deleteEditorRows(lstSelectedRows);
}

//DEAD CODE
void GTAAppController::clearAllinEditor()
{
    //    GTAViewController* _pViewController = GTAViewController::getGTAViewController(this);
    //    _pViewController->clearAllinEditor();

}

GTAViewController* GTAAppController::getViewController(void)
{
    return _pViewController;
}

//DEAD CODE
bool GTAAppController::pasteEditorCopiedRow(const int&)
{

    bool rC = false;
    //    if (!_lstOfRowsToCopy.isEmpty())
    //    {
    //        if (_pViewController)
    //        {

    //            rC=_pViewController->pasteEditorCopiedRow(iRow);
    //            _LastError=_pViewController->getLastError();
    //        }
    //    }
    //    else
    //        _LastError="Nothing to paste";
    return rC;
}
//DEAD CODE
bool GTAAppController::pasteEditorCopiedRow(const int&, const QList<GTACommandBase*> CmdsPasted)
{

    bool rC = false;
    //    if (!CmdsPasted.isEmpty())
    //    {
    //        if (_pViewController)
    //        {

    //            rC=_pViewController->pasteEditorCopiedRow(iRow,CmdsPasted);
    //            _LastError=_pViewController->getLastError();
    //        }
    //    }
    //    else
    //        _LastError="Nothing to paste";
    return rC;
}


//DEAD CODE
QAbstractItemModel* GTAAppController::getModel(const EditorType&)
{
    //    GTAViewController* _pViewController = GTAViewController::getGTAViewController(this);
    //    return _pViewController->buildEditorModels(ieType);
    return NULL;
}

//DEAD CODE
QString GTAAppController::getLoadedElemPath()const
{
    //    GTAElement *pElement = _pViewController->getElement();
    //    QString path;
    //    path = pElement->getRelativeFilePath();
    //    const QString gtaDataDir = getGTADataDirectory();
    //    QString fullPath = QString("%1/%2").arg(gtaDataDir,path);
    //    return QDir::cleanPath(fullPath);

    return QString("");
}


//Modified for MDI do not change
bool GTAAppController::saveDocument(const QString & iName, bool iOverWrite,GTAElement * ipElement, GTAHeader* ipHeader,const QString &iFilePath)
{
    bool rc = false;
    if (NULL==ipHeader)
    {
        _LastError = "Header is not found";
        return false;
    }

    if (NULL != ipElement)
    {
        ipElement->setName(iName);
        QString fileExt = GTAAppController::getExtensionForElementType(ipElement->getElementType());

        GTADocumentController docController;

        QString gtaDataDir = _pSystemServices->getDataDirectory();
        QString completePath;
        if(iFilePath.isEmpty())
            completePath = QString("%1/%2.%3").arg(gtaDataDir,iName,fileExt);
        else if(iFilePath.startsWith("/") && !iFilePath.contains(gtaDataDir))
        {
            completePath = QString("%1%2").arg(gtaDataDir,iFilePath);
        }
        else
            completePath = iFilePath;

        QFileInfo docFileInfo(completePath);
        bool safeToOverwrite = iOverWrite;
        if(docFileInfo.exists())
        {
            QString docUUID;
            docController.getDocumentInfo(completePath, docUUID);
            if(ipElement->getUuid() == docUUID)
            {
                safeToOverwrite = true;
            }

        }
        QString gtaDirPath = getGTADataDirectory();
        QString temp = completePath;
        QString relativePath = temp.replace(gtaDirPath,"");
        ipElement->setRelativeFilePath(QDir::cleanPath(relativePath));
        ipElement->setHeader(ipHeader);
        ipElement->setGtaVersion(QString(TOOL_VERSION_STR));
        ipElement->setModifiedTime(QDateTime::currentDateTime().toString());

        rc = docController.saveDocument(ipElement, ipHeader->getName(), completePath, safeToOverwrite);
        if(! rc)
        {
            _LastError = docController.getLastError();
        }
        else
        {
            updateTreeItem(completePath, Save);
            QString filepath = ipElement->getRelativeFilePath();
			QStringList filepaths = filepath.split("/");
			QString filename = filepaths.last();
			_dbDataController->updateSvnStatus(filename, ipElement->getUuid());
			// Update SVN status (Modified)
            ipElement->setSaveSatus(GTAElement::SaveOK);
        }
    }
    return rc;
}

//DEAD CODE
bool GTAAppController::headerInEditorMode()
{
    //    if (!_pViewController)
    //        return false;

    //    else
    //    {
    //        GTAHeader* pHeader = _pViewController->getHeader();
    //        return pHeader->inEditMode();
    //    }
    return false;

}

//DEAD CODE
bool GTAAppController::saveHeader(const QString& iName)const
{
    bool rC = false;
    QString sDataDirectory=_pSystemServices->getDataDirectory();
    QString completePath = sDataDirectory+ "\\"+ iName+".hdr";
    QString headerFilePath = QDir::cleanPath(completePath);
    if (_pViewController)
    {
        GTAHeader *pHeader= _pViewController->getHeader();
        if(pHeader != NULL  )
        {
            //pHeader->dumpDomNOde();//TODO:remove after debug
            GTADocumentController DocCtrl;
            rC = DocCtrl.saveDocument(pHeader,headerFilePath,true);
            if (rC)
                pHeader->setEditMode(false);
        }
    }
    return rC;

}

//WORKING
bool GTAAppController::headerExists(const QString& iName)const
{
    bool rC = false;

    QString sDataDirectory=_pSystemServices->getDataDirectory();
    QString completePath = sDataDirectory+ "\\"+ iName +".hdr";
    QString headerFilePath = QDir::cleanPath(completePath);
    QFileInfo fileInfo(headerFilePath);
    {
        if (fileInfo.exists())
            rC=true;
    }

    return rC;

}

//DEAD CODE
QString GTAAppController::getHeaderName()const
{
    if (_pViewController)
        return _pViewController->getHeaderName();
    else
        return QString();
}
//DEAD CODE
bool   GTAAppController::headerComplete(QStringList& oEmptyManField)
{
    bool rC = false;
    if (_pViewController)
        rC= _pViewController->headerComplete(oEmptyManField);
    return rC;
}



QHash<QString,QStringList> GTAAppController::getActionCommandList(GTAUtil::ElementType iType,QStringList& orderList )
{

    QHash<QString,QStringList> oCommandList;
    QString commandSet;
    switch(iType)
    {
    case GTAUtil::OBJECT: commandSet = "OBJECT_CMD_SET"; break;
    case GTAUtil::FUNCTION: commandSet = "FUNCTION_CMD_SET"; break;
    case GTAUtil::PROCEDURE: commandSet = "PROCEDURE_CMD_SET"; break;
    case GTAUtil::TEMPLATE: commandSet = "TEMPLATE_CMD_SET"; break;
    case GTAUtil::SEQUENCE: commandSet = "SEQUENCE_CMD_SET"; break;

    }
    if(! commandSet.isEmpty())
    {
        QString xmlFile = _pSystemServices->getApplicationConfigXmlFilePath();
        GTAConfigurationFileParser configFileParser(xmlFile);
        oCommandList = configFileParser.getActionCommandList(commandSet);

        /*if(oCommandList.contains(ACT_GEN_TOOL))
        {
            QStringList errList;
            QStringList genToolComplements = _pGenericToolController->getAllToolConfigrations(errList);
            if(!errList.isEmpty())
            {
                //oCommandList.insert(ACT_GEN_TOOL,genToolComplements);             Add logic for displaying the erorrs in the error box
            }
            if(!genToolComplements.isEmpty())
            {
                oCommandList.insert(ACT_GEN_TOOL,genToolComplements);
            }
        }*/
        orderList=configFileParser.getActionOrderList();
    }
    return oCommandList;
}

QList<QString> GTAAppController::getCheckCommandList(GTAUtil::ElementType iType)
{
    QList<QString> oCheckCommandList;
    QString commandSet;
    switch(iType)
    {
    case GTAUtil::OBJECT: commandSet = "OBJECT_CMD_SET"; break;
    case GTAUtil::FUNCTION: commandSet = "FUNCTION_CMD_SET"; break;
    case GTAUtil::PROCEDURE: commandSet = "PROCEDURE_CMD_SET"; break;
    case GTAUtil::TEMPLATE: commandSet = "TEMPLATE_CMD_SET"; break;
    case GTAUtil::SEQUENCE: commandSet = "SEQUENCE_CMD_SET"; break;
    }

    if(! commandSet.isEmpty())
    {
        QString xmlFile = _pSystemServices->getApplicationConfigXmlFilePath();
        GTAConfigurationFileParser configFileParser(xmlFile);
        oCheckCommandList = configFileParser.getCheckCommandList(commandSet);

    }
    return oCheckCommandList;
}


/**
 * This function checks if an element of a certain type can be converted to another
 * @ipElement : input element to check which is being saved as
 * @iDestType : the type into which the current file will be saved
 * @return - bool : returns true if the save as is valid
*/
bool GTAAppController::isSaveAsValid(const GTAElement *ipElement, QString &iDestType)
{
    bool isValid = true;
    if(ipElement != NULL)
    {
        GTAElement::ElemType elemType = getElementTypeFromExtension(iDestType);
        GTAUtil::ElementType utilElememtType = GTAUtil::UNKNOWN;
        switch(elemType)
        {
        case GTAElement::OBJECT :
            utilElememtType = GTAUtil::OBJECT;
            break;
        case GTAElement::FUNCTION :
            utilElememtType = GTAUtil::FUNCTION;
            break;

        case GTAElement::PROCEDURE :
            utilElememtType = GTAUtil::PROCEDURE;
            break;

        case GTAElement::SEQUENCE :
            utilElememtType = GTAUtil::SEQUENCE;
            break;

        case GTAElement::TEMPLATE :
            utilElememtType = GTAUtil::TEMPLATE;
            break;
        }
        QHash<QString,QStringList> actions = getActionCommandList(utilElememtType);
        QList<QString> checks = getCheckCommandList(utilElememtType);
        int count = ipElement->getDirectChildrenCount();
        for(int i  = 0; i < count; i++)
        {
            GTACommandBase *pCmd = NULL;
            bool rc = ipElement->getCommand(i,pCmd);
            if(rc && (pCmd != NULL))
            {
                GTACommandBase::CommandType cmdType = pCmd->getCommandType();
                if(cmdType == GTACommandBase::ACTION)
                {
                    GTAActionBase *pActionCmd = dynamic_cast<GTAActionBase*>(pCmd);
                    if(pActionCmd)
                    {
                        QString action = pActionCmd->getAction();
                        QString complement = pActionCmd->getComplement();
                        QStringList keys = actions.keys();
                        if(!keys.isEmpty())
                        {
                            if(keys.contains(action))
                            {
                                QStringList values = actions.value(action);
                                if(!values.isEmpty())
                                {
                                    if(values.contains(complement))
                                    {
                                        continue;
                                    }
                                    else
                                    {
                                        isValid = false;
                                        break;
                                    }
                                }
                            }
                            else
                            {
                                isValid = false;
                                break;
                            }
                        }
                    }
                }
                else if(cmdType == GTACommandBase::CHECK)
                {
                    GTACheckBase *pCheckCmd = dynamic_cast<GTACheckBase *>(pCmd);
                    if(pCheckCmd)
                    {
                        QString checkName = pCheckCmd->getCheckName();
                        if(!checks.isEmpty())
                        {
                            if(checks.contains(checkName))
                            {
                                continue;
                            }
                            else
                            {
                                isValid = false;
                                break;
                            }
                        }

                    }
                }

            }
        }
    }
    return isValid;
}

//DEAD CODE
bool GTAAppController::editAction(const int& iRow, const QVariant& value)
{
    bool rC = false;
    if (_pViewController)
    {
        rC = _pViewController->editAction(iRow,value);
        if (!rC)
            _LastError = _pViewController->getLastError();
    }
    return rC;

}
//DEAD CODE
bool GTAAppController::editCheck(const int& iRow,const QVariant& value)
{

    bool rC = false;
    if (_pViewController)
    {
        rC=_pViewController->editAction(iRow,value);
        if (!rC)
            _LastError = _pViewController->getLastError();
    }
    return rC;

}

//DEAD CODE
GTACommandBase* GTAAppController::getCommandForSelectedRow(const int& iSelRow)const
{


    return _pViewController->getCommand(iSelRow);
}

//DEAD CODE
void GTAAppController::updateEditorView()
{
    _pViewController->updateEditorView();
}

//DEAD CODE
bool GTAAppController::setEditorRowCopy1(QList<int>& lstSelectedRows,const QList<GTACommandBase*> &iCmdList)
{
    bool rc = false;

    if(_pViewController)
    {
        rc = _pViewController->setCopiedItems(lstSelectedRows,iCmdList);
        if(!lstSelectedRows.isEmpty())
            _lstOfRowsToCopy = QList<int>(lstSelectedRows);
    }
    return rc;
}

//DEAD CODE
bool GTAAppController::setEditorRowCopy(QList<int>& lstSelectedRows, bool isNewCommand)
{
    bool rc = false;

    if(_pViewController)
    {
        rc = _pViewController->setCopiedItems(lstSelectedRows,isNewCommand);
        if(!lstSelectedRows.isEmpty())
            _lstOfRowsToCopy = QList<int>(lstSelectedRows);
    }
    return rc;
}


//DEAD CODE
QString GTAAppController::getHeaderNameFromEditorDoc(const QString& sFileName)
{
    bool rc=false;QString headerName ;
    QString sDataDirectory = _pSystemServices->getDataDirectory();
    QString completeFilePath = sDataDirectory;
    QString sPathSeperator="/";
    completeFilePath.append(sPathSeperator);
    completeFilePath.append(sFileName);
    rc=  QFile::exists(completeFilePath);
    if (rc)
    {
        QFile xmlFileObj(completeFilePath);
        rc = xmlFileObj.open(QFile::Text| QFile::ReadOnly);
        if (rc)
        {
            QDomDocument DomDoc;
            rc = DomDoc.setContent(&xmlFileObj);
            if (rc)
            {
                QDomNodeList listOfNode = DomDoc.elementsByTagName("HEADER");
                QDomNode domNode= listOfNode.at(0);
                QDomElement domElement= domNode.toElement();
                headerName=domElement.attribute("NAME");
                return headerName;

            }
            xmlFileObj.close();
        }

    }

    return headerName;
}


QString GTAAppController::getAbsoluteFilePathFromUUID(const QString &iUUID)
{
    QString absolutePath;
    if(_treeRootNode)
    {
        GTATreeItem *foundNode = _treeRootNode->findNode(iUUID,GTATreeItem::UUID);
        if(foundNode)
        {
            QString absRelPath = foundNode->getAbsoluteRelPath();
            absolutePath = QDir::cleanPath(QString("%1/%2").arg(getGTADataDirectory(),absRelPath));
        }
    }
    return absolutePath;
}

void GTAAppController::updatevalidationStatusonDV(const QString &iUUID,const QString iValidationStatus,const QString iValidatorName, const QString iValidationDate)
{
    if(_treeRootNode)
    {
        GTATreeItem *foundNode = _treeRootNode->findNode(iUUID,GTATreeItem::UUID);
        if(foundNode)
        {
            foundNode->setValidationStatus(iValidationStatus);
            foundNode->setValidatedBy(iValidatorName);
            if (iValidationDate.isEmpty())
                foundNode->setValidationDate(QDateTime::currentDateTime().toString());
            else
                foundNode->setValidationDate(iValidationDate);
		
            QString fileName = getGTADataDirectory() + foundNode->getAbsoluteRelPath();

            GTADocumentController docCtrl;
            docCtrl.setValidationInfo(fileName, iValidationStatus, iValidatorName, iValidationDate);

            QFileInfo fileInfo(fileName);
            QString absPath = fileInfo.absolutePath();
            QString absFilePath = fileInfo.absoluteFilePath();

            updateTreeItem(absFilePath, Save, "");
        }
    }
}

QStringList GTAAppController::getAllRelFilePathsFromUUID(const QString &iUUID)
{
    QStringList relPathLst;
    if(_treeRootNode)
    {
        QList<GTATreeItem *> foundNodeLst = _treeRootNode->findAllNodes(iUUID,GTATreeItem::UUID);
        for(int i = 0 ; i < foundNodeLst.count(); i++)
        {
            GTATreeItem *pNode = foundNodeLst.at(i);
            if(pNode)
            {
                QString absRelPath = pNode->getAbsoluteRelPath();
                relPathLst.append(absRelPath);

            }
        }
    }
    return relPathLst;
}

/**
  * This function open the gta document
  * iFileName : file name of the document to be opened e.g. abc.obj
  * opElemenet : The document object to be retrieved
  * isUUID : if file needs to be opened via uuid, pass true
  * ioOnlyModel : true/false returns no header/header
  * isCompleteFilePath : if iFileName argument is complete file path (Used for getting GTAElement object for svn Temp file in svn check for modifications)
  * return true if the document is open successfully otherwise false
  */
bool GTAAppController::getElementFromDocument(const QString & iFileName, GTAElement *& opElemenet,bool isUUID,bool ioOnlyModel,bool isCompleteFilePath)
{
    QTextStream out(stdout);
    bool rc = false;
    QString sDataDirectory = _pSystemServices->getDataDirectory();
    QString fileName;
    if(isUUID)
    {
        fileName = QDir::cleanPath(getAbsoluteFilePathFromUUID(iFileName)); // in this case iFileName has UUID
    }
    else if(isCompleteFilePath)
    {
        fileName = iFileName;
    }
    else
    {
        fileName = QDir::cleanPath(QString("%1/%2").arg(sDataDirectory,iFileName));
    }


    GTADocumentController docController;
    GTAHeader* pHdr = NULL;

    QFileInfo info(fileName);
    if (!info.exists()) {
        out << "File " << fileName << " info doesn't exit" << "\n";
        return false;
    }   

    rc = docController.openDocument(fileName,opElemenet,pHdr,ioOnlyModel);
    if (opElemenet != NULL)
    {
        opElemenet->updateCallCommands();
    }

    if(rc && (opElemenet != NULL) && pHdr)
    {
         opElemenet->setHeader(pHdr);
    }
    else
    {
        _LastError = docController.getLastError();
    }
    return rc;
}

bool GTAAppController::getHeaderFromDocument(const QString & iFileName, GTAHeader *& opHeader)
{
    bool rc = false;
    if(_pViewController != NULL && _pSystemServices != NULL)
    {
        GTADocumentController docController;
        GTAElement* pElement = NULL;
        GTAHeader* pHeader = NULL;
        QString sDataDirectory = _pSystemServices->getDataDirectory();
        QString completeFilePath = QString("%1/%2").arg(sDataDirectory,iFileName);
        QFileInfo info(completeFilePath);
        if(!info.exists())
            return false;
        rc = docController.openDocument(completeFilePath,pElement,pHeader,false);
        if (!rc)
        {
            _LastError=docController.getLastError();
        }
        else
        {
            if ( pHeader)
            {
                opHeader = pHeader;
                rc = true;
            }
            if (NULL!=pElement)
            {
                delete pElement;pElement=NULL;
            }

        }
    }

    return rc;
}

bool GTAAppController::createHeaderModelFromDoc(const QString& iFileName, QAbstractItemModel*& opEditorModel)
{
    bool rC = false;
    GTAHeader* pHeader = NULL;
    rC = getHeaderFromDocument(iFileName, pHeader);
    if (rC && pHeader!=NULL)
    {
        rC=_pViewController->createHeaderModel(pHeader,opEditorModel);
    }
    return rC;

}
bool GTAAppController::createAndSetElementFromDataDirectoryDoc( const QString& iFileName,EditorType isType )
{

    bool rc = false;
    if(_pViewController)
    {
        GTAElement* pElement = NULL;
        GTAHeader* pHeader = NULL;
        QString sDataDirectory = _pSystemServices->getDataDirectory();
        QString completeFilePath = QString("%1/%2").arg(sDataDirectory,iFileName);

        GTADocumentController docController;
        //        rc = docController.openDocument(completeFilePath,pElement,pHeader,false);
        rc = getElementFromDocument(iFileName,pElement,false);
        if(pElement != NULL)
            pHeader = pElement->getHeader();
        if (rc && pElement)
        {
            rc =  _pViewController->setElement(pElement,isType);
            if (!rc)
            {
                _LastError = _pViewController->getLastError();
                return rc;
            }

            if (pHeader)
            {
                pElement->setHeader(pHeader);
                rc = _pViewController->setHeader(pHeader);
                if (!rc)
                {
                    _LastError = _pViewController->getLastError();
                    return rc;
                }

            }

        }
        else
        {
            _LastError = docController.getLastError();
            _LastError.append("\nFile may be corrupted or is conflicted");


        }

    }

    return rc;

}
bool GTAAppController::getElementModel(QAbstractItemModel*& opModel)
{
    bool rc = false;
    if(_pViewController)
    {
        _pViewController->createSetElementModel(opModel);
        rc=true;
    }

    return rc;

}
bool GTAAppController::getHeaderModel(QAbstractItemModel*& opModel)
{
    bool rc = false;
    if(_pViewController)
    {
        _pViewController->createSetHeaderModel(opModel);
        rc=true;
    }

    return rc;
}

/**
 * @brief Controls the export of the SCXML
 * @param pElement Current procedure element
 * @param fileName Name of the current document
 * @param FilePath Path of the current document
 * @param iHasUnsubscibeAtStart
 * @param iHasUnsubscibeAtEnd
 * @param iVarList
 * @param mode
 * @return bool True if everything went well
 */
bool GTAAppController::exportToScxml(GTAElement* pElement, const QString& fileName, const QString& iFilePath,
                                        bool iHasUnsubscibeAtStart, bool iHasUnsubscibeAtEnd,QStringList& iVarList,
                                        GTAAppController::ExecutionMode mode)
{
    bool rc = true;

    if (_pSystemServices != NULL)
    {
        QString scxmlTmplDir = _pSystemServices->getScxmlTemplateDir();

        if(pElement != NULL)
        {
            TestConfig* testConf = TestConfig::getInstance();

            bool bSaveDocument = testConf->getSaveBeforeExport();
            bool isNewSCXMLStruct = testConf->getNewSCXMLStruct();
            bool isGenericSCXML = testConf->getGenericSCXML();
            bool isGenInvokeForLocalVar = testConf->getENInvokeLocalVar();

            if(bSaveDocument)
            {
                QString uuid = pElement->getUuid();
                QString absoluteFilePath = getAbsoluteFilePathFromUUID(uuid);
                if(QFile::exists(absoluteFilePath))
                {
                    GTAHeader * header = pElement->getHeader();
                    rc = saveDocument(fileName,true,pElement,header,absoluteFilePath);
                }
            }

            if(rc == true)
            {
                if((!isNewSCXMLStruct) && (!isGenericSCXML))
                {
                    GTAExportScxml* ExportData = new GTAExportScxml(scxmlTmplDir);
                    rc = ExportData->exportToScxml(pElement,iFilePath,iHasUnsubscibeAtStart,iHasUnsubscibeAtEnd);
                    if (!rc)
                        _LastError = ExportData->getLastError();

                    if (ExportData != NULL)
                    {
                        delete ExportData;
                        ExportData = NULL;
                    }
                }

                else if(isGenericSCXML)
                {
                    QString newSCXMLTemplDir = _pSystemServices->getNewSCXMLTemplateDirectory();
                    GTAGenericSCXMLController * ExportData = new GTAGenericSCXMLController(newSCXMLTemplDir,isGenInvokeForLocalVar);
                    rc = ExportData->exportToScxml(pElement,iFilePath);
                    if (!rc)
                        _LastError = ExportData->getLastError();

                    if (ExportData != NULL)
                    {
                        delete ExportData;
                        ExportData = NULL;
                    }
                }

                else if(isNewSCXMLStruct)
                {
                    GTASCXMLControllerNew* ExportData = new GTASCXMLControllerNew(scxmlTmplDir);
                    ExportData->setUnsubscibeFileParamOnly(getUnsubscribeFileParamOnly());
                    rc = ExportData->exportToScxml(pElement,iFilePath,iHasUnsubscibeAtStart,iHasUnsubscibeAtEnd,iVarList,mode);
                    if (!rc)
                        _LastError = ExportData->getLastError();

                    if (ExportData != NULL)
                    {
                        delete ExportData;
                        ExportData = NULL;
                    }
                }
                else
                {

                }
            }
        }
    }
    return rc;
}

bool GTAAppController::exportDocToScxml(const QString & iDocName, const QString &iFilePath,
                                           bool iHasUnsubscibeAtStart,bool iHasUnsubscibeAtEnd, bool fullFilepath)
{
    bool rc = true;
    if (_pViewController != NULL && _pSystemServices != NULL)
    {
        QString scxmlTmplDir = _pSystemServices->getScxmlTemplateDir();
        GTAElement * pElement = NULL;
        rc = this->getElementFromDocument(iDocName,pElement,false, false, fullFilepath);
        if(rc  && pElement != NULL)
        {
            QString elementName = QString(iDocName);
            elementName.remove(elementName.indexOf("."),elementName.size()-elementName.indexOf("."));

            QStringList sepFileName = elementName.split("/",QString::SkipEmptyParts);
            QString name;
            if(!sepFileName.isEmpty())
                name = sepFileName.last();

            QString filePath;
            if (iFilePath.contains(".scxml")) {
                filePath = QDir::cleanPath(iFilePath);
            }
            else {
                filePath = QDir::cleanPath(QString("%1%2%3%4").arg(QString(iFilePath), QDir::separator(), name, QString(".scxml")));
            }
            
            TestConfig* testConf = TestConfig::getInstance();
            bool isNewSCXMLStruct = testConf->getNewSCXMLStruct();
            bool isGenericSCXML = testConf->getGenericSCXML();
            bool isGenInvokeForLocalVar = testConf->getENInvokeLocalVar();
            
            if(!isNewSCXMLStruct && !isGenericSCXML)
            {
                GTAExportScxml * ExportData = new GTAExportScxml(scxmlTmplDir);
                rc = ExportData->exportToScxml(pElement,QDir::cleanPath(filePath),iHasUnsubscibeAtStart,iHasUnsubscibeAtEnd);
                if (!rc)
                    _LastError = ExportData->getLastError();
                if (ExportData != NULL)
                {
                    delete ExportData;
                    ExportData = NULL;
                }
            }
            else if(isNewSCXMLStruct)
            {
                GTASCXMLControllerNew * ExportData = new GTASCXMLControllerNew(scxmlTmplDir);
                ExportData->setUnsubscibeFileParamOnly(getUnsubscribeFileParamOnly());
                QStringList varList;
                rc = ExportData->exportToScxml(pElement,QDir::cleanPath(filePath),iHasUnsubscibeAtStart,iHasUnsubscibeAtEnd,varList);
                if (!rc)
                    _LastError = ExportData->getLastError();
                    
                if (ExportData != NULL)
                {
                    delete ExportData;
                    ExportData = NULL;
                }
            }
            if(isGenericSCXML)
            {
   
                GTAGenericSCXMLController * ExportData = new GTAGenericSCXMLController(scxmlTmplDir,isGenInvokeForLocalVar);
                QString genericfilePath = QDir::cleanPath(QString("%1%2%3%4").arg(QString(iFilePath),QDir::separator(),name,QString(".gscxml")));
                rc = ExportData->exportToScxml(pElement,QDir::cleanPath(genericfilePath));
                if (!rc)
                    _LastError = ExportData->getLastError();
                if (ExportData != NULL)
                {
                    delete ExportData;
                    ExportData = NULL;
                }
            }
            if (pElement != NULL)
            {
                delete pElement;
                pElement = NULL;
            }

        }
    }
    return rc;
}

//TODO: To be implemented by I04DAIVYTE-1097
bool GTAAppController::exportCsvToPro([[maybe_unused]] const QString& csvPath, [[maybe_unused]] const QString& outputPath) {
    return false;
}

/**
  * The function delete provide document name, if it is not being used by another document
  * iFileName : Document to be deleted from database
  * iUsedByDocument: retrieve the list of document uses the iFileName
  * return true if iFileName is not used by another document otherwise false
  */
bool GTAAppController::deleteDocument(const QString iFileName, QStringList &iUsedByDocument)
{
    bool rc = false;
    iUsedByDocument = getDocumentUses(iFileName);

    QString dataDirPath = getGTADataDirectory();
    QString fullPath = QDir::cleanPath(QString("%1%2%3").arg(dataDirPath,QDir::separator(),iFileName));


    QString FileName;
    QString relPath;
    if(iFileName.count("/") == 1)
    {
        FileName = iFileName;
        FileName.remove("/");
    }
    else if(iFileName.count("/") > 1)
    {
        QStringList temp = iFileName.split("/",QString::SkipEmptyParts);
        FileName = temp.takeLast();
        relPath = temp.join("/");
    }
    if(iUsedByDocument.isEmpty())
    {

        QString docPath = _pSystemServices->getDataDirectory();

        if(!relPath.isEmpty())
        {
            docPath = QDir::cleanPath(QString("%1%2%3").arg(docPath,QDir::separator(),relPath));
        }

        rc = removeItemInFileSystem(docPath, FileName);

        updateTreeItem(fullPath,Delete);
        _FavList.removeAll(FileName);
        //        removeFromFavorites(iIndex);
    }
    return rc;
}

/**
  * The function provides removal mechanism for files and directories separately
  * pathToItem is the path to directory which contains the item
  * itemName is the name of file or directory to be removed
  * return true if the file is successfully removed
  */
bool GTAAppController::removeItemInFileSystem(const QString &pathToItem, const QString &itemName)
{
    QFileInfo fi(pathToItem + "/" + itemName);
    if (fi.exists() && fi.isFile())
    {
        QDir Dir(pathToItem);
        return Dir.remove(itemName);
    }

    QDir Dir(pathToItem + "/" + itemName);
    return Dir.removeRecursively();
}

/**
  * This Function return the list of document which are using the provided document
  * iDocFileName: The name of file in current configuration
  * return the list of files uses iDocFileName in current configuration
  */
QStringList GTAAppController::getDocumentUses(const QString & iDocFileName)
{
    QStringList oList;
    QStringList fileExtList;fileExtList<<"*.obj"<<"*.fun"<<"*.pro"<<"*.seq"<<"*.tmpl";

    //    QStringList sepFilePath = iDocFileName.split("/",QString::SkipEmptyParts);
    //    QString fileName = sepFilePath.takeLast();
    QString docPath = _pSystemServices->getDataDirectory();
    QString absoluteFilePath = QDir::cleanPath(QString("%1%2").arg(docPath,iDocFileName));
    QString absolutePath;
    QFileInfo fileInfo(absoluteFilePath);
    if(fileInfo.exists())
    {
        absolutePath = fileInfo.absolutePath();
    }


    QStringList listOfFileInSeach;
    foreach(QString fileExt, fileExtList)
    {

        QStringList listOfFile = _pSystemServices->getDocumentByExtension(fileExt);
        listOfFileInSeach.append(listOfFile);
    }

    listOfFileInSeach.removeDuplicates();
    if(listOfFileInSeach.contains(absoluteFilePath))
    {
        listOfFileInSeach.removeOne(absoluteFilePath);
    }
    foreach(QString xmlFile, listOfFileInSeach)
    {
        // QString xmlFileFullPath = QString("%1/%2").arg(docPath,xmlFile);
        QFile objFile(xmlFile);
        if (objFile.open(QFile::Text | QFile::ReadOnly))
        {
            QDomDocument domDoc;
            domDoc.setContent(&objFile);
            objFile.close();

            QDomElement rootElem = domDoc.documentElement();
            QDomNodeList actionNodeList = rootElem.elementsByTagName(XNODE_ACTION);
            int itemCount = actionNodeList.count();
            for(int i = 0; i < itemCount; i++)
            {
                QDomElement elemOnAction = actionNodeList.at(i).toElement();
                QString actionName = elemOnAction.attribute(XNODE_NAME);
                if(actionName == ACT_CALL)
                {
                    QDomNodeList actionChildNodes =  elemOnAction.childNodes();
                    for(int j = 0 ;j < actionChildNodes.count() ; j++)
                    {
                        QDomElement actionChild = actionChildNodes.at(j).toElement();
                        if(actionChild.nodeName() == XNODE_ELEMENT)
                        {
                            QString documentName = actionChild.attribute(XNODE_NAME);
                            if(!documentName.startsWith("/"))
                            {
                                documentName.prepend("/");
                            }
                            if(iDocFileName.compare(documentName,Qt::CaseInsensitive) == 0)
                            {
                                oList.append(xmlFile);
                                break;
                            }
                        }
                    }
                }
            }

        }
    }
    oList.removeDuplicates();
    return oList;
}

/**
* This Function provides the list of documents which are used by input document.
* iDocName: The name of file in current configuration
* iDocFileName: The name of file in current configuration
* return the list of files uses iDocFileName in current configuration
*/
void GTAAppController::getDocumentsUsed(const QString& iDocName,QStringList & iolstDocs,QStringList& ioLstOfErrors)
{
    GTAElement* pElement = NULL;
    bool rc = getElementFromDocument(iDocName,pElement,false);
    if (rc)
    {
        if (NULL!=pElement)
        {
            int rows = pElement->getAllChildrenCount();
            for (int i=0;i<rows;i++)
            {
                GTACommandBase* pCmd = NULL;
                pElement->getCommand(i,pCmd);
                GTAActionCallProcedures* pCallProcs = dynamic_cast<GTAActionCallProcedures*>(pCmd);
                if (NULL!=pCmd && pCmd->hasReference())
                {
                    GTAActionCall* pCall = dynamic_cast<GTAActionCall*> (pCmd);
                    if (pCall)
                    {
                        QString uuid = pCall->getRefrenceFileUUID();
                        QString elementName = getAbsoluteFilePathFromUUID(uuid);
                        QString dataDir = getGTADataDirectory();
                        elementName.replace(dataDir,"").trimmed();
                        if (!elementName.isEmpty())
                        {
                            if(!elementName.startsWith("/"))
                                elementName.prepend("/");
                            if (!iolstDocs.contains(elementName))
                            {
                                iolstDocs.append(elementName);
                                getDocumentsUsed(elementName,iolstDocs,ioLstOfErrors);
                            }
                        }
                    }
                }
                else if (pCallProcs != NULL)
                {
                    QString dataDir = getGTADataDirectory();
                    foreach (GTACallProcItem proc, pCallProcs->getCallProcedures())
                    {
                        QString uuid = proc._UUID;
                        QString elementName = getAbsoluteFilePathFromUUID(uuid);
                        elementName.replace(dataDir,"").trimmed();
                        if (!elementName.isEmpty())
                        {
                            if(!elementName.startsWith("/"))
                                elementName.prepend("/");
                            if (!iolstDocs.contains(elementName))
                            {
                                iolstDocs.append(elementName);
                                getDocumentsUsed(elementName,iolstDocs,ioLstOfErrors);
                            }
                        }
                    }
                }
            }
            delete pElement;
        }
        else
            ioLstOfErrors.append(QString("Could not open document \"%1\"").arg(iDocName));
    }
    else
    {
        ioLstOfErrors.append(getLastError());
    }



}
//void GTAAppController::reAttachView(QTableView* pIview)
//{
//
//    if (NULL!=  pIview)
//    {
//        if(NULL!=_pViewController)
//            _pViewController->reAttachView( pIview);
//    }
//}

//********************************************************************************************
// Search Functionality -- START
//********************************************************************************************

QSqlDatabase GTAAppController::getLocalDB(/*const QString &iProgName, const QString &iEquipName, const QString &iStdName*/)
{
    bool rc = false;
    QSqlDatabase oDatabase;

    QString dbFilePath = _pSystemServices->getDatabaseFile();
    GTADataController DataCtrl(dbFilePath);
    rc = DataCtrl.openDatabase();
    if (rc)
    {
        oDatabase = DataCtrl.getLocalDatabase();
    }     
    else
    {
        QString errmsg = DataCtrl.getLastError();
    }

    return oDatabase;
}

bool GTAAppController::updateTableVersionInParamDB()
{
    bool rc = _DataCtrl->updateTableVersionInParamDB();
    return rc;
}
/* This function allow to update or rebuild the Database (BENCH)
* @input: forceUpdate - delete and create a new table (first use)
* @input: TestConfigUpdate - ???
* @return: structLoadFileInfo - Contains the files to Insert, Update and Remove, the global status and the different messages to display
*/
structLoadFileInfo GTAAppController::loadICDPMRPIRFilesInDB(bool forceUpdate,int TestConfigUpdate)
{
    // Get the file path of the PARAMETER_DB file
	QString dbFilePath = _pSystemServices->getDatabaseFile();
    QString bdPath = QString::fromStdString(TestConfig::getInstance()->getLibraryPath());
    bdPath.replace("\\", "/");

	// Start measuring time
	auto begin = std::chrono::high_resolution_clock::now();

    GTADataController DataCtrl(dbFilePath);
    QStringList icdFiles = _pSystemServices->getICDFiles();
    QStringList pirFiles = _pSystemServices->getPIRFiles();
    QStringList pmrFile = _pSystemServices->getPMRFiles();
    QStringList modelIcdFiles = _pSystemServices->getModelICDFiles();
    QStringList vcomFiles = _pSystemServices->getVCOMFiles();
    QStringList bdconfFiles = _pSystemServices->getBenchDBConfigFiles();

	// We need to get the current ToolId and compare it
	QString toolIdToUse = _pSystemServices->getCurrentToolId();
	if (_pSystemServices->readAndGetToolIdFromXml() != toolIdToUse)
	{
		toolIdToUse = _pSystemServices->readAndGetToolIdFromXml();
		_pSystemServices->setCurrentToolId(toolIdToUse);
	}

    icdFiles.append(pirFiles);
    icdFiles.append(pmrFile);
    icdFiles.append(modelIcdFiles);
    icdFiles.append(vcomFiles);

    if(TestConfigUpdate == 1)
        icdFiles.append(bdconfFiles);
    else if(TestConfigUpdate == 2)
    {
        icdFiles.clear() ;
        icdFiles.append(bdconfFiles);
    }
    structLoadFileInfo resultObj;

    QFile file(dbFilePath);
    bool exists = file.exists();
    QStringList filestoParse;
    foreach (QString file, icdFiles)
    {
        filestoParse.append(file.replace(bdPath,""));
    }

    resultObj._icdFiles=filestoParse;

    if(exists)
    {
        if(forceUpdate)
        {
            createTemporaryEquipmentSettings();
            DataCtrl.deleteTableInParamDb(dbFilePath);
            DataCtrl.createTableInParamDB();
        }

        DataCtrl.getFileListToUpdateParameter(resultObj._icdFiles,resultObj._FileToUpdateList,resultObj._FileToRemoveList);
		bool ret = false;
		if (!resultObj._FileToRemoveList.isEmpty())
			ret = DataCtrl.removeParameterInFiles(resultObj._FileToRemoveList);
		if(!resultObj._FileToUpdateList.isEmpty())
			ret = DataCtrl.updateParameterFromFiles(resultObj._FileToUpdateList, toolIdToUse);
        if(forceUpdate)
            getTemporaryEquipmentSettigns();
		
        if(!ret)
        {
            _LastError = DataCtrl.getLastError();
        }
		if(resultObj._FileToUpdateList.isEmpty() && resultObj._FileToRemoveList.isEmpty())
		{
			_LastError = "Database is already up-to-date";
		}
        resultObj._execStatus = ret;
    }
    else
    {
        if(_pSystemServices->createTemplateDbFile())
        {
            bool rc = DataCtrl.isTableExisting(PARAMETER_TABLE);
            if(!rc)
            {
                rc = DataCtrl.createTableInParamDB();
            }
            if(rc)
            {
                if (icdFiles.empty())
                {
                    _LastError = "No ICD/PMR/PIR files found in library.";
                    resultObj._execStatus = false;
                    return resultObj;
                }

                bool rc = DataCtrl.updateParameterFromFiles(resultObj._icdFiles, toolIdToUse);
                resultObj._isNew = true;
                resultObj._execStatus = rc;
                if (!rc)
                {
                    _LastError=DataCtrl.getLastError();
                }
            }
            else
            {
                _LastError = "Could not create table";
                resultObj._execStatus=false;
            }
        }
        else
        {
            _LastError = "Could not locate DB template in Library";
            resultObj._execStatus=false;
        }
    }

	// Stop measuring time and calculate the elapsed time
	auto end = std::chrono::high_resolution_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

	QString logFilePath = QDir::cleanPath("C:\\temp\\chrono.txt");
	QFile logFile(logFilePath);
	bool frc = logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
	if (frc)
	{
		QTextStream stream(&logFile);
		QString action = (forceUpdate) ? "rebuilt " : "updated ";
		QString errorContents = "\nDatabase (" + bdPath + ") is "+ action + "in: " + QString::number(elapsed.count() * 1e-9) + " s";
		stream << errorContents;
		logFile.close();
	}
    return resultObj;
}


void GTAAppController::getTemporaryEquipmentSettigns()
{
    QString tempEquipmentFilePath = _pSystemServices->getTempEquipmentQueryFilePath();
    QStringList queryList;
    QFile file(tempEquipmentFilePath);
    if(file.exists())
    {
        bool rc = file.open((QIODevice::ReadOnly | QIODevice::Text));
        if(rc)
        {
            QTextStream in(&file);

            while(!in.atEnd())
            {
                QString query = in.readLine();
                queryList.append(query);
            }

            if(!queryList.isEmpty())
            {
                QString dbFilePath = _pSystemServices->getDatabaseFile();
                GTADataController db(dbFilePath);
                db.updateEquipmentData(queryList);
            }
        }
        file.close();
    }
}

bool GTAAppController::createTemporaryEquipmentSettings()
{
    bool rc = false;
    QString tempEquipmentFilePath = _pSystemServices->getTempEquipmentQueryFilePath();

    QFile file(tempEquipmentFilePath);
    rc = file.open((QIODevice::WriteOnly | QIODevice::Text));
    if(rc)
    {
        QTextStream out(&file);
        QString dbFilePath = _pSystemServices->getDatabaseFile();
        GTADataController db(dbFilePath);
        QStringList queryList = db.getUpdateQueryListForEquipments();
        file.close();
    }

    return rc;
}


// Modified for MDI do not change or remove
QStringList GTAAppController::getItemsToDisplay(const QString iExtn, bool ignoreUsedByDocs, bool isResultView, QHash<QString, QString> &docMap)
{
    QStringList docList;
    GTAMdiController *pMdiCtrl = GTAMdiController::getMDIController();
    if(pMdiCtrl != NULL && _pViewController != NULL)
    {        
        docList = _pViewController->getDocumentByExtension(_treeRootNode,iExtn,docMap);

        GTAEditorViewController * pViewController = NULL;
        pViewController = pMdiCtrl->getActiveViewController();
        if(pViewController)
        {

            if (ignoreUsedByDocs )
            {
                QString sDocName = pViewController->getElement()->getDocumentName();

                if(!sDocName.isEmpty())
                {
                    QStringList usedByDocs = this->getDocumentUses(sDocName);

                    foreach(QString sUsedByDoc, usedByDocs)
                    {
                        docList.removeAll(sUsedByDoc);
                    }
                }
                docList.removeAll(sDocName);

            }
            if(!isResultView)
            {
                QString currentElementName = pViewController->getElement()->getDocumentName();
                docList.removeAll(currentElementName);
            }
        }
    }
    return docList;
}

QStringList GTAAppController::getOneClickItemsToDisplay(/*const QString &iProgName , const QString &iEquipName, const QString &iStdName*/)
{
    return  _pSystemServices->getOneClickConfigFileList();
}
QStringList GTAAppController::getOneClickLauncAppItemsToDisplay(/*const QString &iProgName , const QString &iEquipName, const QString &iStdName*/)
{
    return  _pSystemServices->getOneClickLauncAppConfigFileList();
}
QStringList GTAAppController::getPrintTables()
{
    QStringList lst;
    GTAMdiController *pMdiCtrl = GTAMdiController::getMDIController();
    if(pMdiCtrl)
    {
        GTAEditorViewController *pEditorCtrl = pMdiCtrl->getActiveViewController();
        if(pEditorCtrl)
        {
            GTAElement* pElem = pEditorCtrl->getElement();
            if (pElem)
                lst=pElem->getPrintTables();
        }
    }
    return lst;
}
QStringList GTAAppController::getIRTItemsToDisplay(/*const QString &iProgName , const QString &iEquipName, const QString &iStdName*/)
{

    return  _pSystemServices->getIRTSeqeunceFileList();
}
QStringList GTAAppController::getRoboticArmItemsToDisplay(/*const QString &iProgName , const QString &iEquipName, const QString &iStdName*/)
{
    return  _pSystemServices->getRoboArmSeqeunceFileList();
}
void GTAAppController::closeDatabase()
{
    QString dbFilePath = _pSystemServices->getDatabaseFile();
    GTADataController DataCtrl(dbFilePath);
    DataCtrl.closeDatabase();
}

//********************************************************************************************
// Search Functionality -- END
//********************************************************************************************
bool GTAAppController::getOneClickConfigFileDetails(const QString & iConfigFile, GTAOneClickTestRig * oTestRig)
{
    bool rc = false;
    QString TestRigName;
    QString TestRigType;
    if(_pSystemServices != NULL && !iConfigFile.isEmpty())
    {
        QString ConfFilePath = _pSystemServices->getOneClickConfigFilePath(iConfigFile);
        QFileInfo fileInfo(ConfFilePath);
        rc = fileInfo.exists();
        if(rc)
        {
            QFile xmlFileObj(ConfFilePath);
            rc = xmlFileObj.open(QFile::Text | QFile::ReadOnly);
            if(rc)
            {
                QDomDocument domDoc;
                domDoc.setContent(&xmlFileObj);
                xmlFileObj.close();
                QDomElement rootElem = domDoc.documentElement();
                if(rootElem.nodeName() == "Test_Rig")
                {
                    rc = true;
                    TestRigName = rootElem.attributes().namedItem("Name").nodeValue();
                    oTestRig->setTestRigName(TestRigName);
                    TestRigType = rootElem.attributes().namedItem("Type").nodeValue();
                    oTestRig->setTestRigType(TestRigType);
                    QDomNodeList ConfNodes = rootElem.childNodes();
                    QList<GTAOneClickConfiguration> confs;
                    for(int ConfIdx=0;ConfIdx<ConfNodes.count();ConfIdx++)
                    {
                        GTAOneClickOptions Options;
                        GTAOneClickConfiguration Conf;
                        QDomNode confNode = ConfNodes.at(ConfIdx);
                        QString confName = confNode.attributes().namedItem("name").nodeValue();
                        Conf.setConfigurationName(confName);
                        QList<GTAOneClickApplications> Apps;
                        QDomNodeList AppNodes = confNode.childNodes();
                        for(int AppIdx=0;AppIdx<AppNodes.count();AppIdx++)
                        {
                            GTAOneClickApplications App;
                            GTAOneClickOptions opts;
                            QDomNode AppNode = AppNodes.at(AppIdx);
                            App.setAppName(confName);
                            QString appVersion = AppNode.attributes().namedItem("name").nodeValue();
                            if(!(appVersion.isEmpty() || appVersion.isNull()))
                            {
                                App.setAppVersion(AppNode.attributes().namedItem("name").nodeValue());
                                App.setAppCommType(AppNode.attributes().namedItem("comm_type").nodeValue());
                                QDomNode DesNode = AppNode.namedItem("description");
                                QString description = DesNode.childNodes().at(0).nodeValue();
                                App.setDescription(description);
                                QDomElement AppElement = AppNode.toElement();
                                QDomNodeList Options;
                                if(!AppElement.isNull())
                                {
                                    Options = AppElement.elementsByTagName("option");
                                }

                                for(int optIdx=0;optIdx<Options.count();optIdx++)
                                {
                                    GTAOneClickOption opt;
                                    QDomNode OptNode = Options.at(optIdx);
                                    QString optCond = OptNode.attributes().namedItem("condition").nodeValue();
                                    opt.setOptionCondition(optCond);
                                    QString defValue = OptNode.attributes().namedItem("default").nodeValue();
                                    defValue = defValue.trimmed();
                                    opt.setOptionDefaultvalue(defValue);
                                    QString optName = OptNode.attributes().namedItem("name").nodeValue();
                                    opt.setOptionName(optName);
                                    QString optType = OptNode.attributes().namedItem("type").nodeValue().toLower();
                                    opt.setOptionType(optType);
                                    QString optConfigPath = OptNode.attributes().namedItem("ConfigPath").nodeValue().toLower();
                                    opt.setOptionConfigPath(optConfigPath);
                                    QString optDefIniPath = OptNode.attributes().namedItem("ini_path_default").nodeValue().toLower();
                                    opt.setOptionDefIniPath(optDefIniPath);

                                    if(OptNode.hasChildNodes())
                                    {
                                        QDomNodeList valueNodes = OptNode.childNodes();
                                        QStringList values;
                                        for(int valIdx=0;valIdx<valueNodes.count();valIdx++)
                                        {
                                            QDomNode valueNode= valueNodes.at(valIdx);
                                            QString value  = valueNode.childNodes().at(0).nodeValue();
                                            value = value.trimmed();
                                            if(!(value.isEmpty()||value.isNull()))
                                                values.append(value);
                                        }
                                        opt.setOptionValues(values);
                                        opt.setOptionHasValues(true);
                                    }
                                    opts.setOptionType(optName,optType);
                                    opts.addOptionToList(optName);
                                    if(optCond.isEmpty()||optCond.isNull() || !optCond.contains("=="))
                                    {
                                        opts.setDefOption(optName,opt);
                                    }
                                    else
                                    {
                                        opts.setCondOptions(optName,opt);
                                    }

                                }
                                App.setOptions(opts);
                                Apps.append(App);
                            }
                        }
                        Conf.setApplications(Apps);
                        confs.append(Conf);
                    }

                    oTestRig->setConfigurations(confs);
                }
                else
                {
                    rc = false;
                }
            }
        }
    }
    return rc;
}

bool GTAAppController::getIRTFunctionList(const QString & iSequenceFile, QStringList & oFunctionList)
{
    bool rc = false;
    if(_pSystemServices != NULL && ! iSequenceFile.isEmpty())
    {
        QString functionFilePath = _pSystemServices->getIRTSequenceFilePath(iSequenceFile);
        QFileInfo fileInfo(functionFilePath);
        rc = fileInfo.exists();
        if(rc)
        {
            QFile xmlFileObj(functionFilePath);
            rc = xmlFileObj.open(QFile::Text | QFile::ReadOnly);
            if(rc)
            {
                QDomDocument domDoc;
                domDoc.setContent(&xmlFileObj);
                xmlFileObj.close();

                QDomElement rootElem = domDoc.documentElement();
                QDomNodeList childList = rootElem.childNodes();
                for(int i =0 ; i <childList.count(); i++)
                {
                    QDomNode child = childList.at(i);
                    if(child.isElement())
                    {
                        QDomElement domElem = child.toElement();
                        if(domElem.hasAttribute("name"))
                        {
                            QString functionName = domElem.attribute("name");
                            oFunctionList.append(functionName);
                            rc = true;
                        }
                    }
                }
            }
        }
    }
    return rc;
}


QString GTAAppController::getRoboArmSequenceFilePath(const QString & iFileName)
{
    QString filePath = _pSystemServices->getRoboArmSequenceFilePath(iFileName);
    return filePath;
}

QString GTAAppController::getIRTSequenceFilePath(const QString &iFileName)
{
    QString filePath = _pSystemServices->getIRTSequenceFilePath(iFileName);
    return filePath;
}

QString GTAAppController::getOneClickConfigFilePath(const QString & iFileName)
{
    QString filePath = _pSystemServices->getOneClickConfigFilePath(iFileName);
    return filePath;
}

QString GTAAppController::getOneClickLaunchAppConfigPath()
{
    QString filePath = _pSystemServices->getOneClickLaunchAppConfigPath();
    return filePath;
}

bool GTAAppController::getRoboArmFunctionList(const QString & iSequenceFile, QStringList & oFunctionList)
{
    bool rc = false;
    if(_pSystemServices != NULL && ! iSequenceFile.isEmpty())
    {
        QString functionFilePath = _pSystemServices->getRoboArmSequenceFilePath(iSequenceFile);
        QFileInfo fileInfo(functionFilePath);
        rc = fileInfo.exists();
        if(rc)
        {
            QFile xmlFileObj(functionFilePath);
            rc = xmlFileObj.open(QFile::Text | QFile::ReadOnly);
            if(rc)
            {
                QDomDocument domDoc;
                domDoc.setContent(&xmlFileObj);
                xmlFileObj.close();

                QDomElement rootElem = domDoc.documentElement();
                QDomNodeList childList = rootElem.childNodes();
                for(int i =0 ; i <childList.count(); i++)
                {
                    QDomNode child = childList.at(i);
                    if(child.isElement())
                    {
                        QDomElement domElem = child.toElement();
                        if(domElem.hasAttribute("name"))
                        {
                            QString functionName = domElem.attribute("name");
                            oFunctionList.append(functionName);
                            rc = true;
                        }
                    }
                }
            }
        }
    }
    return rc;
}
bool GTAAppController::getIRTFunctionContents(const QString iSequenceFileName,const QString & iFunctionName, QList<QDomNode>  & oXmlRpcNodeList)
{
    bool rc = false;
    if(_pSystemServices != NULL && ! iSequenceFileName.isEmpty())
    {
        QString functionFilePath = _pSystemServices->getIRTSequenceFilePath(iSequenceFileName);
        QFileInfo fileInfo(functionFilePath);
        rc = fileInfo.exists();
        if(rc)
        {
            QFile xmlFileObj(functionFilePath);
            rc = xmlFileObj.open(QFile::Text | QFile::ReadOnly);
            if(rc)
            {
                QDomDocument domDoc;
                domDoc.setContent(&xmlFileObj);
                xmlFileObj.close();

                QDomElement rootElem = domDoc.documentElement();
                QDomNodeList childList = rootElem.childNodes();
                for(int i =0 ; i <childList.count(); i++)
                {
                    QDomNode child = childList.at(i);
                    if(child.isElement())
                    {
                        QDomElement domElem = child.toElement();
                        if(domElem.hasAttribute("name"))
                        {
                            QString functionName = domElem.attribute("name");
                            if(iFunctionName == functionName)
                            {
                                QDomNodeList domFuncChildList = domElem.childNodes();
                                int stepCount = domFuncChildList.count();
                                for(int j =0; j < stepCount; j++ )
                                {
                                    QDomNode domFuncChild = domFuncChildList.at(j);
                                    //                                    QDomNode domImportedNode = domDoc.importNode(domFuncChild,true);
                                    //                                    oXmlRpcNodeList.append(domImportedNode);
                                    oXmlRpcNodeList.append(domFuncChild.cloneNode());
                                    QString nodeName = domFuncChild.nodeName();

                                }

                                rc = true;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    return rc;
}

bool GTAAppController::getIRTFunctionContents(const QString iSequenceFileName, const QString & iFunctionName, QByteArray & oXmlDocContents)
{
    bool rc = false;
    if(_pSystemServices != NULL && ! iSequenceFileName.isEmpty())
    {
        QDomDocument xmlrpcDomDoc;//(iSequenceFileName);

        QString itemName = iFunctionName;
        if(iFunctionName.contains(" "))
        {
            itemName = itemName.replace(" ","_");
        }
        QDomElement xmlrpcRootElem = xmlrpcDomDoc.createElement(itemName);
        xmlrpcDomDoc.appendChild(xmlrpcRootElem);

        QString functionFilePath = _pSystemServices->getIRTSequenceFilePath(iSequenceFileName);
        QFileInfo fileInfo(functionFilePath);
        rc = fileInfo.exists();
        if(rc)
        {
            QFile xmlFileObj(functionFilePath);
            rc = xmlFileObj.open(QFile::Text | QFile::ReadOnly);
            if(rc)
            {
                QDomDocument domDoc;
                domDoc.setContent(&xmlFileObj);
                xmlFileObj.close();

                QDomElement rootElem = domDoc.documentElement();
                QDomNodeList childList = rootElem.childNodes();
                for(int i =0 ; i <childList.count(); i++)
                {
                    QDomNode child = childList.at(i);
                    if(child.isElement())
                    {
                        QDomElement domElem = child.toElement();
                        if(domElem.hasAttribute("name"))
                        {
                            QString functionName = domElem.attribute("name");
                            if(iFunctionName == functionName)
                            {
                                QDomNodeList domFuncChildList = domElem.childNodes();
                                int stepCount = domFuncChildList.count();
                                for(int j =0; j < stepCount; j++ )
                                {
                                    QDomNode domFuncChild = domFuncChildList.at(j);
                                    QDomNode xmlrpcFunction = xmlrpcDomDoc.importNode(domFuncChild,true);
                                    xmlrpcRootElem.appendChild(xmlrpcFunction);
                                }
                                rc = true;
                                break;
                            }
                        }
                    }
                }
            }
        }
        if(! xmlrpcDomDoc.isNull())
        {
            oXmlDocContents = xmlrpcDomDoc.toByteArray();
            if(! oXmlDocContents.isEmpty() && (! oXmlDocContents.isNull()))
            {
                rc = true;
            }
            else rc = false;
        }
        else rc = false;
    }
    return rc;
}

bool GTAAppController::getIRTFunctionContents(const QList<QDomNode>  & FunctionContents, QStringList & oContents)
{
    bool rc = false;
    int contentCount = FunctionContents.count();
    if(contentCount > 0)
    {
        rc = true;
        for(int i = 0; i<contentCount; i++)
        {
            QDomNode domNode = FunctionContents.at(i);
            if(domNode.isElement())
            {
                QDomElement xmlRpcElem = domNode.toElement();

                QString rootNodeName = xmlRpcElem.nodeName();
                QString rootName = xmlRpcElem.attribute("name");
                QString toolID = xmlRpcElem.attribute("tool_id");
                QString toolType = xmlRpcElem.attribute("tool_type");
                QString xmlRpcContent = QString("<%1 name=\"%2\" tool_id=\"%3\" tool_type=\"%4\">").arg(rootNodeName,rootName,toolID,toolType);
                oContents.append(xmlRpcContent);
                QDomNodeList domFunctionList = xmlRpcElem.childNodes();
                for(int j = 0; j < domFunctionList.count(); j++)
                {
                    QDomNode domFunctionNode = domFunctionList.at(j);
                    if(domFunctionNode.isElement())
                    {
                        QDomElement domFuncElem = domFunctionNode.toElement();
                        QString funcNodeName = domFuncElem.nodeName();
                        QString funcName= domFuncElem.attribute("name");
                        QString xmlFunctionContent = QString("\t<%1 name=\"%2\">").arg(funcNodeName,funcName);
                        oContents.append(xmlFunctionContent);

                        QDomNodeList domArgList = domFuncElem.childNodes();
                        for(int k = 0; k < domArgList.count(); k++)
                        {
                            QDomNode domArg = domArgList.at(k);
                            if(domArg.isElement())
                            {
                                QDomElement domArgElem = domArg.toElement();
                                QString argNodeName = domArgElem.nodeName();
                                QString argName = domArgElem.attribute("name");
                                QString argValue = domArgElem.attribute("value");
                                QString xmlArgContent = QString("\t\t<%1 name=\"%2\" value=\"%3\"/>").arg(argNodeName,argName,argValue);
                                oContents.append(xmlArgContent);
                            }
                        }
                        oContents.append(QString("\t</%1>").arg(funcNodeName));
                    }
                }
                if(rootNodeName.contains(" "))
                {
                    rootNodeName = rootNodeName.replace(" ","_");
                }
                oContents.append(QString("</%1>").arg(rootNodeName));
            }
        }
    }
    return rc;
}
bool GTAAppController::getRoboArmFunctionContents(const QString iSequenceFileName, const QString & iFunctionName, QByteArray & oXmlDocContents)
{
    bool rc = false;
    if(_pSystemServices != NULL && ! iSequenceFileName.isEmpty())
    {
        QDomDocument xmlrpcDomDoc;//(iSequenceFileName);
        QString itemName = iFunctionName;
        if(iFunctionName.contains(" "))
        {
            itemName = itemName.replace(" ","_");
        }
        QDomElement xmlrpcRootElem = xmlrpcDomDoc.createElement(itemName);
        xmlrpcDomDoc.appendChild(xmlrpcRootElem);

        QString functionFilePath = _pSystemServices->getRoboArmSequenceFilePath(iSequenceFileName);
        QFileInfo fileInfo(functionFilePath);
        rc = fileInfo.exists();
        if(rc)
        {
            QFile xmlFileObj(functionFilePath);
            rc = xmlFileObj.open(QFile::Text | QFile::ReadOnly);
            if(rc)
            {
                QDomDocument domDoc;
                domDoc.setContent(&xmlFileObj);
                xmlFileObj.close();

                QDomElement rootElem = domDoc.documentElement();
                QDomNodeList childList = rootElem.childNodes();
                for(int i =0 ; i <childList.count(); i++)
                {
                    QDomNode child = childList.at(i);
                    if(child.isElement())
                    {
                        QDomElement domElem = child.toElement();
                        if(domElem.hasAttribute("name"))
                        {
                            QString functionName = domElem.attribute("name");
                            if(iFunctionName == functionName)
                            {
                                QDomNodeList domFuncChildList = domElem.childNodes();
                                int stepCount = domFuncChildList.count();
                                for(int j =0; j < stepCount; j++ )
                                {
                                    QDomNode domFuncChild = domFuncChildList.at(j);
                                    QDomNode xmlrpcFunction = xmlrpcDomDoc.importNode(domFuncChild,true);
                                    xmlrpcRootElem.appendChild(xmlrpcFunction);
                                }
                                rc = true;
                                break;
                            }
                        }
                    }
                }
            }
        }
        if(! xmlrpcDomDoc.isNull())
        {
            oXmlDocContents = xmlrpcDomDoc.toByteArray();
            if(! oXmlDocContents.isEmpty() && (! oXmlDocContents.isNull()))
            {
                rc = true;
            }
            else rc = false;
        }
        else rc = false;
    }
    return rc;
}

bool GTAAppController::getRoboArmFunctionContents(const QList<QDomNode>  & FunctionContents, QStringList & oContents)
{
    bool rc = false;
    int contentCount = FunctionContents.count();
    if(contentCount > 0)
    {
        rc = true;
        for(int i = 0; i<contentCount; i++)
        {
            QDomNode domNode = FunctionContents.at(i);
            if(domNode.isElement())
            {
                QDomElement xmlRpcElem = domNode.toElement();

                QString rootNodeName = xmlRpcElem.nodeName();
                QString rootName = xmlRpcElem.attribute("name");
                QString toolID = xmlRpcElem.attribute("tool_id");
                QString toolType = xmlRpcElem.attribute("tool_type");
                QString xmlRpcContent = QString("<%1 name=\"%2\" tool_id=\"%3\" tool_type=\"%4\">").arg(rootNodeName,rootName,toolID,toolType);
                oContents.append(xmlRpcContent);
                QDomNodeList domFunctionList = xmlRpcElem.childNodes();
                for(int j = 0; j < domFunctionList.count(); j++)
                {
                    QDomNode domFunctionNode = domFunctionList.at(j);
                    if(domFunctionNode.isElement())
                    {
                        QDomElement domFuncElem = domFunctionNode.toElement();
                        QString funcNodeName = domFuncElem.nodeName();
                        QString funcName= domFuncElem.attribute("name");
                        QString xmlFunctionContent = QString("\t<%1 name=\"%2\">").arg(funcNodeName,funcName);
                        oContents.append(xmlFunctionContent);

                        QDomNodeList domArgList = domFuncElem.childNodes();
                        for(int k = 0; k < domArgList.count(); k++)
                        {
                            QDomNode domArg = domArgList.at(k);
                            if(domArg.isElement())
                            {
                                QDomElement domArgElem = domArg.toElement();
                                QString argNodeName = domArgElem.nodeName();
                                QString argName = domArgElem.attribute("name");
                                QString argValue = domArgElem.attribute("value");
                                QString xmlArgContent = QString("\t\t<%1 name=\"%2\" value=\"%3\"/>").arg(argNodeName,argName,argValue);
                                oContents.append(xmlArgContent);
                            }
                        }
                        oContents.append(QString("\t</%1>").arg(funcNodeName));
                    }
                }
                if(rootNodeName.contains(" "))
                {
                    rootNodeName = rootNodeName.replace(" ","_");
                }
                oContents.append(QString("</%1>").arg(rootNodeName));
            }
        }
    }
    return rc;
}
bool GTAAppController::getRoboArmFunctionContents(const QString iSequenceFileName,const QString & iFunctionName, QList<QDomNode>  & oXmlRpcNodeList)
{
    bool rc = false;
    if(_pSystemServices != NULL && ! iSequenceFileName.isEmpty())
    {
        QString functionFilePath = _pSystemServices->getRoboArmSequenceFilePath(iSequenceFileName);
        QFileInfo fileInfo(functionFilePath);
        rc = fileInfo.exists();
        if(rc)
        {
            QFile xmlFileObj(functionFilePath);
            rc = xmlFileObj.open(QFile::Text | QFile::ReadOnly);
            if(rc)
            {
                QDomDocument domDoc;
                domDoc.setContent(&xmlFileObj);
                xmlFileObj.close();

                QDomElement rootElem = domDoc.documentElement();
                QDomNodeList childList = rootElem.childNodes();
                for(int i =0 ; i <childList.count(); i++)
                {
                    QDomNode child = childList.at(i);
                    if(child.isElement())
                    {
                        QDomElement domElem = child.toElement();
                        if(domElem.hasAttribute("name"))
                        {
                            QString functionName = domElem.attribute("name");
                            if(iFunctionName == functionName)
                            {
                                QDomNodeList domFuncChildList = domElem.childNodes();
                                int stepCount = domFuncChildList.count();
                                for(int j =0; j < stepCount; j++ )
                                {
                                    QDomNode domFuncChild = domFuncChildList.at(j);
                                    //                                    QDomNode domImportedNode = domDoc.importNode(domFuncChild,true);
                                    //                                    oXmlRpcNodeList.append(domImportedNode);
                                    oXmlRpcNodeList.append(domFuncChild.cloneNode());
                                    QString nodeName = domFuncChild.nodeName();

                                }

                                rc = true;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    return rc;
}
bool GTAAppController::getIRTFunctionContents(const QString iSequenceFile, QStringList &oXmlFileContents)
{
    bool rc = false;

    if(_pSystemServices != NULL)
    {
        QDomNodeList FunctionContents;
        QString sequenceFilePath= _pSystemServices->getRoboArmSequenceFilePath(iSequenceFile);
        if(! sequenceFilePath.isEmpty())
        {
            QFile xmlFileObj(sequenceFilePath);
            rc = xmlFileObj.exists();
            if(rc)
            {
                rc = xmlFileObj.open(QFile::Text | QFile::ReadOnly);
                QTextStream reader(&xmlFileObj);
                while(! reader.atEnd())
                {
                    oXmlFileContents.append(reader.readLine());
                }
            }
            //            for(int i = 0; i<FunctionContents.count(); i++)
            //            {
            //                QDomNode domNode = FunctionContents.at(i);
            //                if(domNode.isElement())
            //                {
            //                    QDomElement xmlRpcElem = domNode.toElement();

            //                    QString rootNodeName = xmlRpcElem.nodeName();
            //                    QString rootName = xmlRpcElem.attribute("name");
            //                    QString toolID = xmlRpcElem.attribute("tool_id");
            //                    QString toolType = xmlRpcElem.attribute("tool_type");
            //                    QString xmlRpcContent = QString("<%1 name=\"%2\" tool_id=\"%3\" tool_type=\"%4\">").arg(rootNodeName,rootName,toolID,toolType);
            //                    oContents.append(xmlRpcContent);
            //                    QDomNodeList domFunctionList = xmlRpcElem.childNodes();
            //                    for(int j = 0; j < domFunctionList.count(); j++)
            //                    {
            //                        QDomNode domFunctionNode = domFunctionList.at(j);
            //                        if(domFunctionNode.isElement())
            //                        {
            //                            QDomElement domFuncElem = domFunctionNode.toElement();
            //                            QString funcNodeName = domFuncElem.nodeName();
            //                            QString funcName= domFuncElem.attribute("name");
            //                            QString xmlFunctionContent = QString("\t<%1 name=\"%2\">").arg(funcNodeName,funcName);
            //                            oContents.append(xmlFunctionContent);

            //                            QDomNodeList domArgList = domFuncElem.childNodes();
            //                            for(int k = 0; k < domArgList.count(); k++)
            //                            {
            //                                QDomNode domArg = domArgList.at(k);
            //                                if(domArg.isElement())
            //                                {
            //                                    QDomElement domArgElem = domArg.toElement();
            //                                    QString argNodeName = domArgElem.nodeName();
            //                                    QString argName = domArgElem.attribute("name");
            //                                    QString argValue = domArgElem.attribute("value");
            //                                    QString xmlArgContent = QString("\t\t<%1 name=\"%2\" value=\"%3\"/>").arg(argNodeName,argName,argValue);
            //                                    oContents.append(xmlArgContent);
            //                                }
            //                            }
            //                            oContents.append(QString("\t</%1").arg(funcNodeName));
            //                        }
            //                    }
            //                    oContents.append(QString("</%1").arg(rootNodeName));
            //                }
            //            }
        }

    }

    return rc;
}

bool GTAAppController:: isDataEmpty()
{
    bool bOk = false;
    GTAMdiController *pMdiCtrl = GTAMdiController::getMDIController();
    if(pMdiCtrl)
    {
        GTAEditorViewController *pEditorCtrl = pMdiCtrl->getActiveViewController();
        if(pEditorCtrl)
        {
            bOk = pEditorCtrl->getElement()->isEmpty();
        }
    }
    return bOk;
}
QString  GTAAppController::getExtensionForElementType(GTAElement::ElemType elemType)
{
    QString sExt;

    switch(elemType)
    {
    case GTAElement::OBJECT : sExt="obj";
        break;
    case GTAElement::FUNCTION : sExt="fun";
        break;
    case GTAElement::PROCEDURE : sExt="pro";
        break;
    case GTAElement::SEQUENCE : sExt="seq";
        break;
    case GTAElement::TEMPLATE : sExt="tmpl";
        break;
    default:
        ;
        break;
    }
    return sExt;
}



/**
 * This function returns enum value GTAElement::ElemType corresponding to extension
 * @iExtension : extension of a file
 * @return : GTAElement::ElemType enum value
*/
GTAElement::ElemType  GTAAppController::getElementTypeFromExtension(QString &iExtension)
{
    GTAElement::ElemType type = GTAElement::UNKNOWN;

    if(!iExtension.isEmpty())
    {
        if(iExtension.startsWith("."))
        {
            iExtension.replace(".","");
            iExtension.trimmed();
        }
        if(iExtension == "obj")
        {
            type = GTAElement::OBJECT;
        }
        else if(iExtension == "fun")
        {
            type = GTAElement::FUNCTION;
        }
        else if(iExtension == "pro")
        {
            type = GTAElement::PROCEDURE;
        }
        else if(iExtension == "seq")
        {
            type = GTAElement::SEQUENCE;
        }
        else if(iExtension == "tmpl")
        {
            type = GTAElement::TEMPLATE;
        }
    }
    return type;
}

bool GTAAppController::stopSCXML()
{
    QString errorMsg = "";
    bool rc = true;

    QProcess stopProcess;
    QString killFilePath = _pSystemServices->getScxmlExitExeFilePath();
    QStringList args;
    stopProcess.start(killFilePath,args);
    return rc;
}

/**
      * This function connects and launches the Sequencer Main Window for sequence execution
      * @isMultipleLogs: Argument for sequencer to play single or multiple logs
      * @iScxmlList: List of SCXMLs generated
   * @iLogLst: Name of Logs
   * @iFailedList: List of Failed SCXMLs
   * @iProcList: List of Procedures to be executed
      */
void GTAAppController::executeSequence(bool isMultipleLogs,const QStringList &iScxmlList,
                                          const QStringList &iLogLst,const QStringList &iFailedList,
                                          const QStringList &iProcList)
{
    if(_pSystemServices != NULL)
    {
        QString scxmlDir = _pSystemServices->getScxmlDirectory();
        QString logDir = _pSystemServices->getLogDirectory();

        //        if(_pSequencerMain != NULL)
        //        {
        //            delete _pSequencerMain;
        //            _pSequencerMain = NULL;
        //        }

        _pSequencerMain = new GTASequencerMain();
        connect(_pSequencerMain,SIGNAL(sequencerWindowClosed()),this,SLOT(onSequencerClosed()));
        connect(this,SIGNAL(closeSequencer(bool&)),_pSequencerMain,SIGNAL(closeSequencer(bool&)));
        _isSequencerRunning = true;
        _pSequencerMain->startSequencer(logDir,scxmlDir,isMultipleLogs,iScxmlList,iLogLst,iFailedList,iProcList,_pSystemServices->getToolDataPath());
    }
}



bool GTAAppController::isValidLogPresent(const QString &iLogName, QString &oLogFilePath)
{
    bool isLogValid = false;
    if(_pSystemServices)
    {
        QString gtaLogDir = _pSystemServices->getLogDirectory();
        if(!iLogName.isEmpty() && !gtaLogDir.isEmpty())
        {
            QDir dir(gtaLogDir);
            if(dir.exists())
            {
                QStringList filters;
                filters << "*.log";
                QStringList files = dir.entryList(filters);
                if(files.contains(iLogName))
                {
                    oLogFilePath = QDir::cleanPath(QString("%1%2%3").arg(gtaLogDir,QDir::separator(),iLogName));
                    isLogValid = true;
                }

            }
        }
    }

    return isLogValid;
}

void GTAAppController::onSequencerClosed()
{
    _isSequencerRunning = false;
    if (NULL != _pSequencerMain)
    {
        delete _pSequencerMain;
        _pSequencerMain = NULL;
    }

    emit toggleLaunchButton(true);
}


/**
 * This function takes the Title based element (groouped by using GroupByTitle in AppCtrl), creates its scxml and launches it with the scheduler
 * @pTitleElem : grouped element
 * @iElemName : element name
 * @return : returns true if successful
*/

bool GTAAppController::launchTitleBasedElement(GTAElement *pTitleElem,const QString &iElemName,QStringList& iVarList)
{
    bool rc = false;
    QString errorMsg;
    if(pTitleElem)
    {
        QString scxmlDir = _pSystemServices->getScxmlDirectory();
        QString logDir = _pSystemServices->getLogDirectory();

        if (!QFile::exists(scxmlDir))
        {
            errorMsg= QString("Scxml path %1 does not exist").arg(scxmlDir);
            _LastError = errorMsg;
            rc = false;
        }
        if (!QFile::exists(logDir))
        {
            errorMsg= QString("Scxml execution log path %1 does not exist").arg(logDir);
            _LastError = errorMsg;
            rc = false;
        }

        bool isGenericSCXML = TestConfig::getInstance()->getGenericSCXML();
        QString scXmlFileName = iElemName + ".scxml";
        if(isGenericSCXML)
            scXmlFileName = iElemName + ".gscxml";
        QString scxmlFilePath = QDir::cleanPath(QString("%1/%2").arg(scxmlDir,scXmlFileName));
        QString logFileName = iElemName + ".log";
        QString EmologFilePath = QDir::cleanPath(QString("%1/%2").arg(logDir,logFileName));


        rc = exportToScxml(pTitleElem,iElemName,scxmlFilePath,true,true,iVarList);
        if (rc)
        {

            QString exeFile = _pSystemServices->getScxmlLaunchExeFilePath();
            if (QFile::exists(exeFile))
            {
                QProcess *pLocalLaunchSCXMLProcess = new QProcess();
                connect(pLocalLaunchSCXMLProcess,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(onLaunchProcessExited(int,QProcess::ExitStatus)));
                QStringList argument;
                if(EmologFilePath.contains("\""))
                    EmologFilePath.replace("\"","");

                if(scxmlFilePath.contains("\""))
                    scxmlFilePath.replace("\"","");


                QString elementPath = _pAppController->getAbsoluteFilePathFromUUID(pTitleElem->getUuid());
                _pSystemServices->setLastEmoLogPath( QDir::cleanPath(EmologFilePath), QDir::cleanPath(elementPath));
                argument<<QDir::cleanPath(EmologFilePath)<<QDir::cleanPath(scxmlFilePath);
                pLocalLaunchSCXMLProcess->start(exeFile,argument);
            }
            else
            {
                errorMsg = "Error: Executable file is missing, Execution fail ";
                _LastError = errorMsg;
                rc = false;
            }
        }
        else
        {
            errorMsg = getLastError();
            errorMsg +=  "Fail to create scxml, launch stopped";
            _LastError.clear();
            _LastError = errorMsg;
            rc = false;
        }

    }
    return rc;
}

/**
 * This slot is called after a QProcess has finished its execution
 *
*/
void GTAAppController::onLaunchProcessExited(int exitCode, QProcess::ExitStatus exitStatus)
{
    QProcess *launchProcess = dynamic_cast<QProcess*>(sender());
    if(launchProcess == NULL)
        return;
    else
    {
        if(exitStatus == QProcess::NormalExit)
        {
            //            do something - like enabling launch button etc etc
        }
        else
        {
            QString err = QString("Launch process failed with exit code %1").arg(QString::number(exitCode));
            _LastError += err;
        }
        launchProcess->deleteLater();
    }

}


bool GTAAppController::launch(LaunchParameters parameters)
{

    QString isElementName = parameters.elementName;
    QString errorMsg = parameters.errorMsg;
    bool iHasUnsubscribeStart = parameters.hasUnsubscribeStart;
    bool iHasUnsubscribeEnd = parameters.hasUnsubscribeEnd;
    bool iHasUnsubscribeFileParamOnly = parameters.hasUnsubscribeFileParamOnly;
    QStringList iVarList = parameters.varList;
    int mode = parameters.mode;
    QString windowUUID = parameters.uuid;
    GTAAppController::ExecutionMode Mode = GTAAppController::UNKNOWN;
    switch(mode)
    {
    case 0:
        Mode = GTAAppController::NORMAL_MODE;
        break;


    case 1:
        Mode = GTAAppController::SEQUENCER_MODE;
        break;

    case 2:
        Mode = GTAAppController::DEBUG_MODE;
        break;

    }


    bool rc = false;
    QString scxmlDir = _pSystemServices->getScxmlDirectory();
    QString logDir = _pSystemServices->getLogDirectory();

    //MDI controller is a singleton class
    GTAMdiController *pMdiCtrl = GTAMdiController::getMDIController();

    GTAEditorViewController *pViewController;
    if(pMdiCtrl != NULL)
    {
        pViewController = pMdiCtrl->getActiveViewController();
        if (pViewController)
        {
            if (!QFile::exists(scxmlDir))
            {
                errorMsg = QString("Scxml path %1 does not exist").arg(scxmlDir);
                _LastError = errorMsg;
                return false;
            }
            if (!QFile::exists(logDir))
            {
                errorMsg = QString("Scxml execution log path %1 does not exist").arg(logDir);
                _LastError = errorMsg;
                return false;
            }

            bool isGenericSCXML = TestConfig::getInstance()->getGenericSCXML();
            QString scXmlFileName = isElementName + ".scxml";
            if (isGenericSCXML)
                scXmlFileName = isElementName + ".gscxml";
            QString scxmlFilePath = QDir::cleanPath(QString("%1/%2").arg(scxmlDir, scXmlFileName));
            QString logFileName = isElementName + ".log";
            QString EmologFilePath = QDir::cleanPath(QString("%1/%2").arg(logDir, logFileName));

            // GTAElement::ElemType currElementType = pViewController->getElement()->getElementType();

            QString controllerUuid;
            GTAElement* pElement = pViewController->getElement();
            if (pElement)
            {
                controllerUuid = pElement->getUuid();
            }

            //failsafe added because the pViewController is wrongly selected due to miss in signal
    //        QString filename = pElement->getName();

            bool fileselection = true;
            if (windowUUID != controllerUuid)
            {
                _LastError = "Error: No file selected to execute";
                fileselection = false;
            }

            if (pElement && fileselection)
            {
                setUnsubscribeFileParamOnly(iHasUnsubscribeFileParamOnly);
                //Generate SCXML
                rc = this->exportToScxml(pElement, isElementName, scxmlFilePath, iHasUnsubscribeStart, iHasUnsubscribeEnd, iVarList, Mode);
                if (rc)
                {

                    QString elementPath; //= iAbsoluteFilePath;


                    QString path = pElement->getRelativeFilePath();
                    QString gtaDataDir = getGTADataDirectory();
                    elementPath = QString("%1/%2").arg(gtaDataDir, path);
                    elementPath = QDir::cleanPath(elementPath);


                    QString exeFile = _pSystemServices->getScxmlLaunchExeFilePath();
                    if (QFile::exists(exeFile))
                    {

                        /*if(pLaunchSCXMLProcess != NULL)
                        {
                            delete pLaunchSCXMLProcess;
                            pLaunchSCXMLProcess = NULL;
                        }*/
                        pLaunchSCXMLProcess = new QProcess();

                        QStringList argument;
                        if (EmologFilePath.contains("\""))
                            EmologFilePath.replace("\"", "");

                        if (scxmlFilePath.contains("\""))
                            scxmlFilePath.replace("\"", "");


                        _pSystemServices->setLastEmoLogPath(QDir::cleanPath(EmologFilePath), QDir::cleanPath(elementPath));
                        argument << QDir::cleanPath(EmologFilePath) << QDir::cleanPath(scxmlFilePath);
                        pLaunchSCXMLProcess->start(exeFile, argument);
                    }
                    else
                    {
                        errorMsg = "Error: Executable file is missing, Execution fail ";
                        _LastError = errorMsg;
                        rc = false;
                    }
                }
                else
                {
                    errorMsg = getLastError();
                    errorMsg += "Fail to create scxml, launch stopped";
                    _LastError.clear();
                    _LastError = errorMsg;
                    rc = false;
                }

            }
        }
    }

    return rc;

}

// This fuction is used to generate SCXML as an input to sequencer
bool GTAAppController::generateScxml(bool isMultipleLogs, GTAElement *pElement,
                                        QString iRelativePath, const QList<GTAActionCall *> iCallProcLst,
                                        QStringList &oScxmlList, QStringList &oLogLst,
                                        QStringList &oFailedList, QStringList &oProcList)
{
    bool rc = false;

    QString scxmlDir = _pSystemServices->getScxmlDirectory();
    QString logDir = _pSystemServices->getLogDirectory();

    QString errorMsg;
    if (!QFile::exists(scxmlDir))
    {
        errorMsg= QString("Scxml path %1 does not exist").arg(scxmlDir);
        _LastError = errorMsg;
        return false;
    }
    if (!QFile::exists(logDir))
    {
        errorMsg= QString("Scxml execution log path %1 does not exist").arg(logDir);
        _LastError = errorMsg;
        return false;
    }

    QStringList varList;
    if(isMultipleLogs)
    {
        for(int i = 0; i < iCallProcLst.count(); i++)
        {
            GTAActionCall *pCall = iCallProcLst.at(i);
            if(pCall)
            {
                QString CallName = pCall->getElement();
                QString callElementName = CallName.split("/").last();
                callElementName = callElementName.mid(0,callElementName.lastIndexOf(".")).trimmed();

                QString refUUID = pCall->getRefrenceFileUUID();
                GTAElement *pCallElement;
                bool retVal = getElementFromDocument(refUUID,pCallElement,true);
                if(retVal == true && pCallElement != NULL)
                {

                    pCallElement->replaceTag(pCall->getTagValue());                     //takes care of tags for the calls of procedures in the sequence
                    //not recursive since calls further made in these procedures and
                    //their resolution will be taken care by exportScxml() logic

                    bool isGenericSCXML = TestConfig::getInstance()->getGenericSCXML();
                    QString scXmlFileName = callElementName + ".scxml";
                    if(isGenericSCXML)
                        scXmlFileName = callElementName + ".gscxml";
                    QString scxmlFilePath = QDir::cleanPath(QString("%1/%2").arg(scxmlDir,scXmlFileName));
                    QString logFileName = callElementName + ".log";

                    QString absFilePath = getAbsoluteFilePathFromUUID(refUUID);
                    if(!absFilePath.isEmpty())
                    {

                        bool returnVal = exportToScxml(pCallElement,callElementName,scxmlFilePath,true,true,varList);
                        rc |= returnVal;
                        if(returnVal)
                        {
                            //                            oGeneratedScxmlLst.insert(scxmlFilePath,logFileName);
                            oProcList.append(CallName);
                            oScxmlList.append(scxmlFilePath);
                            oLogLst.append(logFileName);

                        }
                        else
                        {
                            oFailedList.append(CallName);
                        }
                    }


                }
                else
                {
                    rc |= false;
                    oFailedList.append(QString("%1 file was not found").arg(CallName));
                }
            }
        }

    }
    else
    {
        QString ElementName = iRelativePath.split("/").last();
        QString isElementName = ElementName.mid(0,ElementName.lastIndexOf("."));

        bool isGenericSCXML = TestConfig::getInstance()->getGenericSCXML();
        QString scXmlFileName = isElementName + ".scxml";
        if(isGenericSCXML)
            scXmlFileName = isElementName + ".gscxml";
        QString scxmlFilePath = QDir::cleanPath(QString("%1/%2").arg(scxmlDir,scXmlFileName));
        QString logFileName = isElementName + ".log";

        rc = exportToScxml(pElement,isElementName,scxmlFilePath,true,true,varList);
        if(rc)
        {
            //            oGeneratedScxmlLst.insert(scxmlFilePath,logFileName);
            oScxmlList.append(scxmlFilePath);
            oLogLst.append(logFileName);

        }
        else
        {
            oFailedList.append(iRelativePath);
        }

    }

    return rc;
}

//DEAD CODE
bool GTAAppController::clearRow(int iSelectedRow,QString & oErrMsg, QList<int> &oClearedRowId)
{
    bool rc = true;
    if(iSelectedRow >= 0)
    {
        GTACommandBase * pCommand = getCommandForSelectedRow(iSelectedRow);
        if(pCommand != NULL)
        {
            if(pCommand->IsDeletable())
            {
                int numberOfChildren = pCommand->getAllChildrenCount();

                addEditorRows(numberOfChildren + iSelectedRow);
                for(int i=0;i<numberOfChildren;i++)
                {
                    addEditorRows(numberOfChildren + iSelectedRow);
                }

                if(pCommand->hasChildren())
                {
                    oClearedRowId.append(iSelectedRow);
                    for(int i=1;i<=numberOfChildren;i++)
                        oClearedRowId.append(iSelectedRow + i);

                }
                QList<int> deleteRowList;
                deleteRowList.append(iSelectedRow);
                deleteEditorRows(deleteRowList);

            }
            else
            {
                oErrMsg = "Non user editable commands cannot be cleared ,only parent of the command can be cleared";
                rc = false;
            }
        }
    }
    return rc;
}
bool GTAAppController::getParameterByName(const QString &iParamName,GTAICDParameter & oParameter)
{
    bool rc = false;
    if(_pSystemServices != NULL)
    {
        QString dataBaseFile = _pSystemServices->getDatabaseFile();
        GTADataController DataCtrl;
        rc =  DataCtrl.getParameterDetail(iParamName,dataBaseFile,oParameter);
    }
    return rc;
}
QStringList GTAAppController::getLocalVariables(bool iInScope,bool isSubscribeParam, int iIndex)
{
    QStringList oVarList;

    GTAMdiController *pMdiCtrl = GTAMdiController::getMDIController();
    if(pMdiCtrl != NULL)// && _pViewController != NULL)
    {
        GTAEditorViewController *pEditorCtrl = pMdiCtrl->getActiveViewController();


        if(pEditorCtrl != NULL)
        {
            int index = -1;
            if(iInScope)
            {
                index = pEditorCtrl->getSelectedRowInEditor();
            }
            else
            {
                GTAElement * pElement = pEditorCtrl->getElement();
                if(pElement != NULL)
                {
                    index = pElement->getAllChildrenCount();
                }
            }
            if(iInScope && iIndex>-1)
            {
                index = iIndex;
            }

            if(index >= 0)
            {
                for(int i = 0 ; i < index; i++)
                {
                    GTACommandBase * pCmd = pEditorCtrl->getCommand(i);
                    if(pCmd != NULL)
                    {

                        if(pCmd->hasReference())
                        {

                            GTAActionCall *pCallCmd = dynamic_cast<GTAActionCall *>(pCmd);
                            if(pCallCmd != NULL)
                            {
                                QString fileName = pCallCmd->getElement();
                                GTAElement *pCallElem = NULL;
                                bool rc= this->getElementFromDocument(pCallCmd->getRefrenceFileUUID(),pCallElem,true);
                                if(rc && pCallElem != NULL)
                                {
                                    QStringList cmdVarList;
                                    pCallElem->getVariableList(cmdVarList);
                                    if(! cmdVarList.isEmpty())
                                        oVarList.append(cmdVarList);
                                }
                            }
                        }
                        else
                        {

                            if(isSubscribeParam)
                            {
                                GTAActionBase *pActCmd = dynamic_cast<GTAActionBase *>(pCmd);
                                if((pActCmd != NULL) && (pActCmd->getAction() == ACT_SUBSCRIBE))
                                {

                                    QStringList cmdVarList = pCmd->getVariableList();
                                    if(! cmdVarList.isEmpty())
                                        oVarList.append(cmdVarList);
						                                
                                }
								else if ((pActCmd != NULL) && (pActCmd->getAction() == ACT_UNSUBSCRIBE)) {
									QStringList cmdVarList = pCmd->getVariableList();
									if (!cmdVarList.isEmpty()) {
										foreach(QString cmdVar, cmdVarList) {
											oVarList.removeOne(cmdVar);
										}
									}
								}

                            }
                            else
                            {
                                QStringList cmdVarList = pCmd->getVariableList();
                                if(! cmdVarList.isEmpty())
                                    oVarList.append(cmdVarList);
                            }
                        }
                    }
                }
            }
        }
    }
    return oVarList;
}

QStringList GTAAppController::getLocalVariables(bool iInScope, bool isSubscribeParam, QStringList knownVarList, int iIndex)
{
	QStringList oVarList;

	GTAMdiController *pMdiCtrl = GTAMdiController::getMDIController();
	if (pMdiCtrl != NULL)// && _pViewController != NULL)
	{
		GTAEditorViewController *pEditorCtrl = pMdiCtrl->getActiveViewController();

		if (pEditorCtrl != NULL)
		{
			int index = -1;
			if (iInScope)
			{
				index = pEditorCtrl->getSelectedRowInEditor();
			}
			else
			{
				GTAElement * pElement = pEditorCtrl->getElement();
				if (pElement != NULL)
				{
					index = pElement->getAllChildrenCount();
				}
			}
			if (iInScope && iIndex>-1)
			{
				index = iIndex;
			}

			if (index >= 0)
			{
				for (int i = 0; i < index; i++)
				{
					GTACommandBase * pCmd = pEditorCtrl->getCommand(i);
					if (pCmd != NULL)
					{

						if (pCmd->hasReference())
						{

							GTAActionCall *pCallCmd = dynamic_cast<GTAActionCall *>(pCmd);
							if (pCallCmd != NULL)
							{
								QString fileName = pCallCmd->getElement();
								GTAElement *pCallElem = NULL;
								bool rc = this->getElementFromDocument(pCallCmd->getRefrenceFileUUID(), pCallElem, true);
								if (rc && pCallElem != NULL)
								{
									QStringList cmdVarList;
									pCallElem->getVariableList(cmdVarList);
									if (!cmdVarList.isEmpty())
										oVarList.append(cmdVarList);
								}
							}
						}
						else
						{

							if (isSubscribeParam)
							{
								GTAActionBase *pActCmd = dynamic_cast<GTAActionBase *>(pCmd);
								if ((pActCmd != NULL) && (pActCmd->getAction() == ACT_SUBSCRIBE))
								{

									QStringList cmdVarList = pCmd->getVariableList();
									if (!cmdVarList.isEmpty())
										oVarList.append(cmdVarList);
								}
								else if ((pActCmd != NULL) && (pActCmd->getAction() == ACT_UNSUBSCRIBE)) {
									QStringList cmdVarList = pCmd->getVariableList();
									if (!cmdVarList.isEmpty()) {
										foreach(QString cmdVar, cmdVarList) {
											oVarList.removeOne(cmdVar);
										}
									}
								}
								if ( i == (index - 1)) {
									// GTACommandBase * pCmd = pEditorCtrl->getCommand(i+1);
									//QStringList cmdVarList = pCmd->getVariableList();
									/*if (!cmdVarList.isEmpty()) {
										foreach(QString cmdVar, cmdVarList) {
											oVarList.removeOne(cmdVar);
										}
									}*/
									if (!knownVarList.isEmpty()) {
										foreach(QString cmdVar, knownVarList) {
											oVarList.removeOne(cmdVar);
										}
									}
								}
							}
							else
							{
								QStringList cmdVarList = pCmd->getVariableList();
								if (!cmdVarList.isEmpty())
									oVarList.append(cmdVarList);
							}
						}
					}
				}
			}
		}
	}
	return oVarList;
}

bool GTAAppController::getModelForTreeView( QAbstractItemModel*& ,const int&)
{
    bool rc = false;
    //    if (_pViewController)
    //    {
    //        rc = _pViewController->getModelForTreeView(pModel ,iRow);
    //        if (!rc)
    //            _LastError= _pViewController->getLastError();
    //    }
    return rc;
}

bool GTAAppController::getModelForTreeView( QAbstractItemModel*& pModel ,const QString& iElementName)
{
    bool rc = false;
    if (_pViewController)
    {
        GTAElement* pElement = NULL;
        rc = getElementFromDocument(iElementName,pElement,false);
        if (rc==true && pElement!=NULL)
        {
            pElement->setElementType(GTAElement::CUSTOM);
            QStringList varList,checkforRep;
            GTAElement *groupedCallElem = new GTAElement(GTAElement::CUSTOM);
            GroupElementByTitleComplete(pElement,varList,groupedCallElem,checkforRep);
            rc = _pViewController->getModelForTreeView(pModel,groupedCallElem,iElementName);
            if (!rc)
                _LastError = _pViewController->getLastError();
        }
        else
            _LastError = _pViewController->getLastError();


    }
    return rc;

}


/**
 * This function takes the Title based element (groouped by using GroupByTitle in AppCtrl), creates its scxml and launches it with the scheduler
 * @pTitleElem : grouped element
 * @iElemName : element name
 * @oVarList : List of variables identified while grouping by title. this list is to be used in SCXML generation
 * @return : returns true if successful
*/
bool GTAAppController::getModelForSelectiveExecution( QAbstractItemModel*& iopModel ,const QString& iElementName, bool isUUID, QStringList &oVarList)
{
    bool rc = false;

    GTAElement* pElement = NULL;
    rc = getElementFromDocument(iElementName,pElement,isUUID);
    if ((rc == true) && (pElement != NULL))
    {

        pElement->updateCommandInstanceName(QString());
        GTAElement* pTitleBasedElement = new GTAElement(pElement->getElementType());
        
        QStringList checkforRep;
        GroupElementByTitleComplete(pElement,oVarList, pTitleBasedElement,checkforRep,false);
        if (!checkforRep.isEmpty())
        {
            _LastError += "Grouping failed. Possible recursion";
            rc = false;
        }
        if(pTitleBasedElement)
        {
            GTACommandBase *pNullCmd = NULL;
            pTitleBasedElement->getDirectChildren(0,pNullCmd);
            if(pNullCmd == NULL)
            {
                pTitleBasedElement->removeCommand(0);
            }


            iopModel = new GTATitleBasedExecutionModel(pTitleBasedElement);
        }
        else
            rc = false;

    }
    else
    {
        _LastError += getLastError();
    }

    return rc;

}
//bool GTAAppController::getModelForCopiedItem( QAbstractItemModel*& pModel )
//{
//    bool rc = false;
//    if (_pViewController)
//    {
//        rc = _pViewController->getClipboardModel(pModel);
//        if(!rc)
//            _LastError = _pViewController->getLastError();
//    }

//    return rc;

//}

//DEAD CODE
bool GTAAppController::detailViewExists(const int& iSelectedRow)
{
    bool rC=false;
    GTACommandBase* pSelectedCmd=getCommandForSelectedRow(iSelectedRow);
    if(pSelectedCmd)
    {
        if(pSelectedCmd->canHaveChildren()||pSelectedCmd->hasReference())
            rC=true;
    }

    return rC;
}
/**
  * Return the user name from the windows environment
  */
QString GTAAppController::getUserName()
{
    QStringList infosList = QProcess::systemEnvironment();
    QString tmp;
    QString loginName = QString();;
    for(int i = 0; i < infosList.size(); ++i)
    {
        tmp = infosList.at(i);
        if(tmp.contains("USERNAME=", Qt::CaseInsensitive))
        {
            loginName = tmp.remove("USERNAME=").trimmed();
            break;
        }
    }
    return loginName;
}
//bool GTAAppController::addheaderField(const QString& iName,const QString& iVal,const QString& iMandatory,const QString& iDescription)
//{
//    bool rc = false;
//    if (_pViewController)
//    {
//        rc = _pViewController->addheaderField(iName,iVal,iMandatory,iDescription);
//        if (!rc)
//            _LastError = _pViewController->getLastError();
//    }

//    return rc;

//}
//bool GTAAppController::editHeaderField(const int& iRow, const QString& iName,const QString& iVal,const QString& iMandatory,const QString& iDescription)
//{
//    bool rc = false;
//    if (_pViewController)
//    {
//        bool bMandatory = true;
//        if (iMandatory.contains("No",Qt::CaseInsensitive))
//            bMandatory=false;

//        rc = _pViewController->editHeaderField(iRow,iName,iVal,bMandatory,iDescription);
//        if (!rc)
//            _LastError = _pViewController->getLastError();
//    }

//    return rc;

//}
//bool GTAAppController::moveHeaderRowItemUp(const int& iRow)
//{
//    bool rc = false;
//    if (_pViewController)
//    {
//        rc =_pViewController->moveHeaderRowItemUp(iRow);
//    }
//    return rc;
//}
//bool GTAAppController::moveHeaderRowItemDown(const int& iRow)
//{

//    bool rc = false;
//    if (_pViewController)
//    {
//        rc =_pViewController->moveHeaderRowItemDown(iRow);
//    }
//    return rc;
//    return rc;
//}

bool GTAAppController::getFileInfoListUsed(const QString& iFileName, QList <GTAFILEInfo> & lstFileInfo)
{
    bool rc = false;


    GTADocumentController docCtrlr;


    QString sDataDirectory = _pSystemServices->getDataDirectory();
    QString completeFilePath = QString("%1/%2").arg(sDataDirectory,iFileName);








    rc = docCtrlr.getFileInfoFromDoc(completeFilePath,lstFileInfo);
    for(int i=0 ; i<lstFileInfo.count() ; i++)
    {
        if(!(lstFileInfo.at(i)._filePath.contains(":/") || lstFileInfo.at(i)._filePath.startsWith("//")))
            lstFileInfo[i]._filePath = QString("%1%2").arg(getGTALibraryPath(),lstFileInfo.at(i)._filePath);

    }

    if (!rc)
    {
        _LastError = docCtrlr.getLastError();
    }
    return rc;
}

//bool GTAAppController::showICDDetails(const bool& iStatus , const QString& iFileName )
//{
//    QList <GTAFILEInfo> lstFileInfo;

//    bool rc = getFileInfoListUsed(iFileName, lstFileInfo);
//    if (_pViewController)
//    {
//        rc = _pViewController->showICDDetails(iStatus,lstFileInfo);
//        if (!rc)
//        {
//            _LastError = _pViewController->getLastError();
//        }
//    }
//    return rc;
//}

bool GTAAppController::getICDDetails(const QStringList& iParameterList, QList<GTAICDParameter>& parameterList)
{

    bool rc = false;
    QString dbFile = _pSystemServices->getDatabaseFile();
    GTADataController dataCtrlr;

    QStringList invalidParams;
    QHash<QString,GTAICDParameter> tempParamList;
    rc = dataCtrlr.getParametersDetail(iParameterList,dbFile,tempParamList,invalidParams);

    for(int i=0;i<tempParamList.keys().count();i++)
    {
        QString tempParamName = tempParamList.keys().at(i);

        parameterList.append(tempParamList.value(tempParamName));
    }

    /*if(rc)
        foreach(GTAICDParameter paramICD, paramDetails)
        {
            QString sAppName = paramICD.getApplicationName();
            QString sName = paramICD.getName();
            QString sType = paramICD.getSourceType();
            QString sMandatory = "FALSE";
            QString sDescrtiption="Icd name used for the corresponding application";

            GTAFILEInfo fileInfo;
            fileInfo._Application = sAppName;
            fileInfo._filename=sName;
            fileInfo._fileType=sType;
            fileInfo._Mandatory = sMandatory;
            fileInfo._fileDescription = sDescrtiption;

            lstFileInfo.append(fileInfo);
        }
    else
    {
        _LastError = dataCtrlr.getLastError();
    }*/
    return rc;
}
/**
  * This function retrieve the variable list of all the commands of element including call commands
  * @pElem : Element to be scan for variables
  */
//QStringList GTAAppController::getAllVariableList(const GTAElement * pElem)
//{
//    QStringList oVarList;
//    if(pElem != NULL)
//    {
//
//        QString fileName;
//        int cmdCount = pElem->getAllChildrenCount();
//        for(int i= 0; i< cmdCount; i++)
//        {
//            GTACommandBase * pCmd =NULL;
//            pElem->getCommand(i,pCmd);
//            if(pCmd != NULL)
//            {
//                if(pCmd->hasReference())
//                {
//                    GTAActionCall * pCallCmd = dynamic_cast<GTAActionCall*>(pCmd);
//                    if(pCallCmd != NULL)
//                    {
//                        GTAElement * pElemOnCall = NULL;
//                        bool rc =  getElementFromDocument(fileName,pElemOnCall);
//                        if(rc && pElemOnCall != NULL)
//                        {
//                            QStringList varListFromCall = getAllVariableList(pElemOnCall);
//                            if(!varListFromCall.isEmpty())
//                                oVarList.append(varListFromCall);
//                        }
//                    }
//                }
//                else
//                {
//                    QStringList varList = pCmd->getVariableList();
//                    if(! varList.isEmpty())
//                        oVarList.append(varList);
//                }
//            }
//        }
//
//    }
//    return oVarList;
//}


bool GTAAppController::getFwcData(QList<GTAFWCData> &oFwcData/*,const QString & iProgram, const QString & iEquiment, const QString & iStandard*/) const
{
    GTADataController dataCtrl;
    bool rc = dataCtrl.getFwcData(oFwcData/*,iProgram,iEquiment,iStandard*/);
    return rc;

}

/**
  * Processed all business login to Log Controller
  * This function reads the sequence file with multiple logs
  * @iLogFile: absolute path of the logFile
  * @return: true/false based on log parsing pass/failed
  */
bool GTAAppController::readSequenceMultipleLogFile(const QString &iLogFile,
                                                      QStringList &oLogNames,
                                                      QStringList &oProcNames,
                                                      QStringList &oStatus,
                                                      QString &oGlobalStatus,
                                                      LogModelSequenceIO &oLogModel)
{
    GTALogReportController logCtr;
    bool rc = logCtr.readSequenceMultipleLogFile(iLogFile,oLogNames,oProcNames,oStatus,oGlobalStatus);
    if (rc)
    {
        oLogModel.globalStatus = oGlobalStatus;
        oLogModel.procNames.append(oProcNames);
        oLogModel.logNames.append(oLogNames);
        oLogModel.status.append(oStatus);
    }
    else
    {
        _LastError = logCtr.getLastError();
        qInfo()<<_LastError;
    }
    return rc;
}


/**
  * Processed all business login to Log Controller
  * This function checks if the sequence is executed with multiple logs
  * @iLogFile: absolute path of the logFile
  * @return: true/false based on single/multiple logs
  */
bool GTAAppController::isSequenceWithMultipleLogs(const QString &iLogFile)
{
    GTALogReportController logController;
    return logController.isSequenceWithMultipleLogs(iLogFile);
}

/**
  * This function provides a custom element grouping by managing Calls under titles for multiple logs
  * @ipElem: Input Element which is used to create a grouped element
  * @oGroupElem: grouped element created from input
  */
void GTAAppController::groupElementbyTitleForSequence(GTAElement *ipElem,
                                                         GTAElement *oGroupElem,
                                                         bool iIsValidationGroupingNeeded)
{
    ipElem->updateCommandInstanceName(QString());
    GTAActionPrint *printCmd =new GTAActionPrint();
    printCmd->setValues(QStringList("DummyStatus: Check Relevant LTRA For Details"));
    printCmd->setInstanceName("Dummy Print Command");
    GTAActionTitle *pTitle = new GTAActionTitle(ACT_TITLE);
    pTitle->setTitle("Dummy title : Sequence Start");
    GTAActionParentTitle *pTitleCmd = new GTAActionParentTitle(pTitle,"");
    int titleChildCnt = 0;

    if (pTitle != NULL)
    {
        delete pTitle;
        pTitle = NULL;
    }

    for (int k=0;k<ipElem->getDirectChildrenCount();k++)
    {
        GTACommandBase *pCmd1 = NULL;
        ipElem->getDirectChildren(k,pCmd1);
        if (pCmd1 != NULL)
        {
            if (pCmd1->isIgnoredInSCXML())
                continue;
            GTACommandBase *pCmd = pCmd1->getClone();
            if (pCmd == NULL)
                continue;

            if (pCmd->isTitle())
            {
                if (pTitleCmd != NULL)
                    oGroupElem->appendCommand(pTitleCmd);
                GTAActionTitle *pActTitleCmd = dynamic_cast<GTAActionTitle*>(pCmd);
                pTitleCmd = new GTAActionParentTitle(pActTitleCmd,"");
                titleChildCnt = 0;
                pTitleCmd->setInstanceName(pCmd->getInstanceName());

                if (pActTitleCmd != NULL)
                {
                    delete pActTitleCmd;
                    pActTitleCmd = NULL;
                }
            }
            else if (pTitleCmd && pCmd->hasReference())
            {
                GTAActionCall *pCallCmd = dynamic_cast<GTAActionCall *>(pCmd);
                if (pCallCmd != NULL)
                {
                    GTAActionExpandedCall *pExtCall = new GTAActionExpandedCall(*pCallCmd);
                    QString functionNameWithArg = pCallCmd->getFunctionWithArgument();
                    if (!iIsValidationGroupingNeeded)
                        pExtCall->insertChildren(printCmd->getClone(),0);
                    pExtCall->setInstanceName(pCmd->getInstanceName());
                    pTitleCmd->insertChildren(pExtCall,titleChildCnt++);
                }
            }
            else if (pTitleCmd)
            {
                pCmd->setComment("Analyze Individual Procedures for Logs");
                pTitleCmd->insertChildren(pCmd,titleChildCnt++);
            }
        }
    }

    //using only clones of the command as dummy children
    //removing the command at the end
    if (NULL != printCmd)
    {
        delete printCmd;
        printCmd = NULL;
    }

    if (pTitleCmd != NULL)
    {
        oGroupElem->appendCommand(pTitleCmd);
    }
}

/**
  * This function export the lab test report for Multiple Logs in a sequence
  * @ipLogModel: Model in UI to be exported, it should of type GTAEditorTreeModel class
  * @iReportFile: Report file full path where the report is created
  * @iAttachmentList: list of documents to be attached with report
  * @isLTRA: LTR to be generated or not
  * @iomodel: structure containing sequnce name and proc and log paths for multiple logs
  * @return: true/false boolean
  */
bool GTAAppController::exportLTRAToDocxForMultipleLogs(QAbstractItemModel *ipLogModel,
                                                          const QString &iReportFile,
                                                          QStringList &iAttachmentList,
                                                          LogModelSequenceIO iomodel,
                                                          ExportLTRAParams exportLTRA)
{
    bool rc = false;
    if (nullptr != ipLogModel)
    {
        QString iFileName = iomodel.SeqDocFile;
        GTAElement * ipElem = nullptr;
        rc = getElementFromDocument(iFileName,ipElem,false,false);
        if (NULL != ipElem && rc)
        {
            GTAElement* pTitleBasedElement = new GTAElement(ipElem->getElementType());            

            GTACommandBase *pNullCmd = NULL;
            pTitleBasedElement->getDirectChildren(0,pNullCmd);
            QString execStart="";
            if(pNullCmd == NULL)
            {
                pTitleBasedElement->removeCommand(0);
            }
            pTitleBasedElement->setName(ipElem->getName());

            GTAHeader* pHeader = NULL;
            if(getHeaderFromDocument(iFileName,pHeader))
            {
                pTitleBasedElement->setHeader(pHeader);
            }


            if (pTitleBasedElement !=NULL)
            {
                int childCount = pTitleBasedElement->getAllChildrenCount();
                LogModelInput input;
                LogModelOutput tempoutput;

                int resultcount=0;
                for (int i=0;i<childCount;i++)
                {
                    GTACommandBase *pCmd = NULL;
                    pTitleBasedElement->getCommand(i,pCmd);
                    GTAActionCall *pCall = dynamic_cast<GTAActionCall*>(pCmd);
                    QString callName="";
                    //checking if the procedure list contains the call to identify if it was executed
                    if (pCall != NULL)
                    {
                        callName = pCall->getElement();
                        if (!iomodel.procNames.contains(callName))
                            continue;
                        else
                        {
                            int index = iomodel.procNames.indexOf(callName);
                            input.docFileName=iomodel.procNames.at(index);
                            input.logFileName=iomodel.logNames.at(index);
                            resultcount++;
                        }
                    }
                    else
                    {
                        continue;
                    }

                    tempoutput = getLogModel(input);

                    //exporting the call to LTRA and adding docx to attachments
                    if (tempoutput.returnStatus)
                    {
                        QString  docFilenamePath = input.logFileName;
                        QFileInfo  info(QDir::cleanPath(docFilenamePath.replace(".log",".docx")));
                        QString defaultFileName  = info.completeBaseName();
                        QFileInfo reportFileInfo(docFilenamePath);
                        QFile fileReport;

                        if (reportFileInfo.exists())
                        {
                            fileReport.remove(defaultFileName);
                        }

                        if(! defaultFileName.isEmpty())
                        {
                            QStringList attachments;
                            attachments<<input.docFileName<<input.logFileName;
                            //exportLTRA.isLTRA = isLTR;
                            //rc = exportLTRAToDocx(tempoutput.pModel,docFilenamePath,attachments,isLTR);//TODO:add timing precision parameter based on requirement
                            rc = exportLTRAToDocx(tempoutput.pModel,docFilenamePath,attachments,exportLTRA);
                        }
                        if (rc)
                        {
                            iAttachmentList.append(docFilenamePath);
                        }

                        //DEV comment:memory leak plug
                        if (NULL != tempoutput.pModel)
                        {
                            delete tempoutput.pModel;
                            tempoutput.pModel = NULL;
                        }
                        if (nullptr != tempoutput.pValidationModel)
                        {
                            delete tempoutput.pValidationModel;
                            tempoutput.pValidationModel = nullptr;
                        }
                        //
                    }
                    else
                    {
                        rc = false;
                        _LastError.append(tempoutput.logErrorMessage);
                    }
                }
            }
        }
        rc = exportLTRAToDocx(ipLogModel,iReportFile,iAttachmentList,exportLTRA);
    }
    return rc;
}

/**
  * This function provides a custom log model for a sequence with multiple logs.
  * @iomodel: LogModelSequenceIO structure for input of log and proc paths of the procedures
  * @return: LodModelOutput structure that contains, columns, model, error logs and return status
  */
LogModelOutput GTAAppController::getLogModelForSequence(LogModelSequenceIO &iomodel)
{
    LogModelOutput output;
    output.hasFailed = iomodel.globalStatus;

    QString iFileName = iomodel.SeqDocFile;

    GTAElement * ipElem = NULL;
    bool rc1 = getElementFromDocument(iFileName,ipElem,false,false);

    if (ipElem != nullptr && rc1)
    {
        GTAElement* pTitleBasedElement = new GTAElement(ipElem->getElementType());
        GTAElement* pTitleBasedElementforValidation = new GTAElement(*ipElem);
        //remove all children but retain all other information
        pTitleBasedElementforValidation->ClearAll();

        groupElementbyTitleForSequence(ipElem,pTitleBasedElement);
        groupElementbyTitleForSequence(ipElem,pTitleBasedElementforValidation,true); //grouping the elements/list which need to be validated

        GTACommandBase *pNullCmd = NULL;
        pTitleBasedElement->getDirectChildren(0,pNullCmd);
        QString execStart="";
        if(pNullCmd == NULL)
        {
            pTitleBasedElement->removeCommand(0);
        }
        pTitleBasedElement->setName(ipElem->getName());

        GTAHeader* pHeader = NULL;
        if(getHeaderFromDocument(iFileName,pHeader))
        {
            pTitleBasedElement->setHeader(pHeader);
        }

        GTALogReportController logCtrl;
        logCtrl.getLogModelForSequence(pTitleBasedElement,output.pModel,output.logErrorMessage,output.hasFailed,iomodel.procNames,iomodel.logNames,iomodel.status);
        output.returnStatus = true;
        output.pValidationModel = new GTAValidationWidgetModel(pTitleBasedElementforValidation); //Validation Widget model pointer

        GTAEditorLogModel* pModel = dynamic_cast<GTAEditorLogModel*>(output.pModel);
        if (nullptr != pModel)
            output.columnList = pModel->getColumnNameList();

        delete ipElem;
        ipElem = nullptr;

    }
    return output;
}

/**
  * This function provides a log model for a procedure.
  * @iInput: LogModelInput structure with log and proc paths of the sequence
  * @igenerateValidationModel: generate/skip the validation model (true/false)
  * @return: LodModelOutput structure that contains, columns, model, error logs and return status
  */
LogModelOutput GTAAppController::getLogModel(LogModelInput input,bool igenerateValidationModel)
{
    bool rc = false;
    LogModelOutput output;
    QString iFileName = input.docFileName;

    if( _pSystemServices != NULL && _pViewController != NULL)
    {
        GTAElement * pElem = NULL;
        rc = getElementFromDocument(iFileName,pElem,false,false);
        if(pElem != NULL )
        {
            pElem->updateCommandInstanceName(QString());
            GTAElement* pTitleBasedElement = new GTAElement(pElem->getElementType());
            GTAElement* pTitleBasedElementforValidation = nullptr;
            if (igenerateValidationModel)
            {
                pTitleBasedElementforValidation = new GTAElement(*pElem);
                //remove all children but retain all other information
                pTitleBasedElementforValidation->ClearAll();
            }

            QStringList variableList;

            QStringList checkForRep;
            GroupElementByTitleComplete(pElem,variableList, pTitleBasedElement,checkForRep,false,false);
            if (igenerateValidationModel)
                GroupElementByTitleComplete(pElem,variableList, pTitleBasedElementforValidation,checkForRep,false,true);

            //check if grouping was properly done
            //show error if not
            if (!checkForRep.isEmpty())
            {
                _LastError =  "Grouping failed and model cannot be created";
                output.hasFailed = true;
                output.logErrorMessage = _LastError;
                output.pModel = nullptr;
                output.returnStatus = false;
                return output;
            }

            GTACommandBase *pNullCmd = NULL;
            pTitleBasedElement->getDirectChildren(0,pNullCmd);
            if(pNullCmd == NULL)
            {
                pTitleBasedElement->removeCommand(0);
            }
            pTitleBasedElement->setName(pElem->getName());

            GTAHeader* pHeader = NULL;
            if(getHeaderFromDocument(iFileName,pHeader))
            {
                pTitleBasedElement->setHeader(pHeader);
            }
            pTitleBasedElement->setPurposeForProcedure(pElem->getPurposeForProcedure());
            pTitleBasedElement->setConclusionForProcedure(pElem->getConclusionForProcedure());
            
            GTALogReportController reportController;
            GTAEditorLogModel *pModel;

            _LastError.clear();
            // When a log mismatch is detected, this result is false
            QString logFileFullPath = getGTALogDirectory() + input.logFileName;
            rc = reportController.getLogModel(logFileFullPath,pTitleBasedElement,output.ioLogInstanceIDMap,pModel,_LastError);

            if (rc && nullptr != pModel)
            {
                output.columnList = pModel->getColumnNameList();
                output.pModel = pModel;
                output.logErrorMessage = _LastError;
            }

            if (igenerateValidationModel)
                output.pValidationModel = new GTAValidationWidgetModel(pTitleBasedElementforValidation);
            output.hasFailed = pTitleBasedElement->getGlobalResultStatus();
            output.returnStatus = rc;
        }

        //DEV comment:memory leak plug
        delete pElem;
        pElem = NULL;
        //
    }
    return output;
}

/**
  * This function export the lab test report
  * @ipLogModel: Model in UI to be exported, it should of type GTAEditorTreeModel class
  * @iReportFile: Report file full path where the report is created
  * @iAttachmentList: list of documents to be attached with report
  * @iDocType: Format of report, supported iDocType are 0-xml, 1-csv
  */

bool GTAAppController::exportLTRAToHTML(QAbstractItemModel *ipLogModel, const QString &iReportFile, const QStringList &iAttachmentList, int iDocType,bool)
{
    bool rc = false;

    if(ipLogModel != NULL)
    {
        GTAEditorLogModel * pLog = dynamic_cast<GTAEditorLogModel*>(ipLogModel);

        if(pLog != NULL)
        {
            GTAElement * pElem = pLog->getDataModel();
            if(iDocType == 0)
            {
                QString tempXmlFile = iReportFile;
                tempXmlFile.replace(".html",".xml");
                rc = exportLTRAToXml1(pElem,tempXmlFile,iAttachmentList);

                if (rc)
                {
                    QString sReportTemplateFile = _pSystemServices->getLTRATemplateFilePath();
                    QFileInfo infoTempFile(sReportTemplateFile);

                    QString currentAppPath = QCoreApplication::applicationDirPath();
                    QString strnsformAppPath = QDir::cleanPath( QString("%1%2%3").arg(currentAppPath,QDir::separator(),"GTATransformToHtml.exe"));
                    QFileInfo infoExecFile( strnsformAppPath );



                    //TODO: temp file should be removed.  once process is running after process finished it should be deleted (signal:finished )
                    QFileInfo info(tempXmlFile);
                    QString reportFileName = info.fileName();
                    QString attchDir = info.absolutePath()+QDir::separator()+QString(reportFileName.remove(".xml")+"_files");
                    QString newTempFile = QDir::cleanPath(attchDir+QDir::separator()+info.fileName());
                    QFile::copy(QDir::cleanPath(tempXmlFile),newTempFile);
                    QFile::remove(tempXmlFile);

                    if (!infoTempFile.exists())
                    {
                        rc = false;
                        _LastError = "GTALTRATemplate.xsl file missing/not deployed";
                    }
                    else if (!infoExecFile.exists())
                    {
                        rc = false;
                        _LastError = "GTATransformToHtml not deployed";
                    }
                    else
                    {
                        /*QFileInfo toSaveInfo(iReportFile);
                        QString directoryToSave = toSaveInfo.absolutePath();
                        QString reportTemplateFile = QDir::cleanPath(QString("%1%2%3").arg(directoryToSave,QDir::separator(),"GTALTRATemplate.xsl"));*/

                        QString outputHtmlFile = iReportFile;
                        outputHtmlFile.replace(".xml",".html");
                        QProcess *pProcess = new QProcess();
                        QStringList argument;
                        argument<<sReportTemplateFile<<newTempFile<<outputHtmlFile;
                        //IKM Blocked it.
                        pProcess->start(strnsformAppPath,argument);

                    }


                    QFileInfo toSaveInfo(tempXmlFile);
                    if (toSaveInfo.exists())
                    {
                        QFile::remove(QDir::cleanPath(tempXmlFile));
                    }


                    //TODO , THE LOGO PATH SHOULD BE PROVIDED BY SYSTEM SERVICES
                    //copy logo to _file path

                    QString sReportTemplatePath =_pSystemServices->getLTRATemplateFolderPath();
                    QDir reportDir(sReportTemplatePath);
                    QStringList filterLst; filterLst<<"*.png";
                    QStringList imageFiles = reportDir.entryList(filterLst);
                    if (!imageFiles.isEmpty())
                    {
                        QFileInfo imgFileInfo(imageFiles.at(0));
                        QString sReportAirbusLogoPath = QString("%1%2%3").arg(sReportTemplatePath,QDir::separator(),imgFileInfo.fileName());
                        sReportAirbusLogoPath = QDir::cleanPath(sReportAirbusLogoPath);


                        QString sAirbusLogoNewFile = attchDir+QDir::separator()+imgFileInfo.fileName();
                        QFile::copy(sReportAirbusLogoPath,QDir::cleanPath(sAirbusLogoNewFile));
                    }

                }
                else
                    _LastError = "Report Generation failed.";


            }
            else
                rc = exportLTRAToCsv(pElem,iReportFile,iAttachmentList);
        }


    }
    else
        _LastError = "Report analysis failed or not done";
    return rc;
}
/**
  * This function export the lab test report
  * @ipLogModel: Model in UI to be exported, it should of type GTAEditorTreeModel class
  * @iReportFile: Report file full path where the report is created
  * @iAttachmentList: list of documents to be attached with report
  * @exportLTRA: struct to update the parameters used for exporting LTRA containing save format, timing precission, isLTRA
  */
bool GTAAppController::exportLTRAToDocx(QAbstractItemModel *ipLogModel,
                                           const QString &iReportFile,
                                           const QStringList &iAttachmentList,
                                           ExportLTRAParams exportLTRA)
{
    bool rc = false;
    //TODO: Fix conversion from copy to ref
    #pragma warning (disable : 4436)

    // Update element in log model   
    auto pLog = dynamic_cast<GTAEditorLogModel*>(ipLogModel);
    QString docFileName = pLog->getDataModel()->getDocumentName();
    foreach(auto attachment, iAttachmentList)
    {
        if (!attachment.endsWith("/" + docFileName))
            continue;
        
        GTAElement* currentElem = nullptr;
        getElementFromDocument(attachment, currentElem, false, true, true);
        if (currentElem != nullptr)
            pLog->updateElement(currentElem);
    }           

    // Export now
    GTALogReportController logCtrl;
    rc = logCtrl.exportLTRAToDocx(pLog,
                             iReportFile,
                             iAttachmentList,
                             exportLTRA.isLTRA,
                             exportLTRA.saveFormat,
                             exportLTRA.saveInMilliseconds,
                             getGTADataDirectory(),
                             exportLTRA.ioLogInstanceIDMap,
                             _pSystemServices->getDatabaseFile());

    if (!rc)
        _LastError = "Report analysis failed or not done";

    return rc;
}


bool GTAAppController::convertToDocx(GTAElement *pElement,const QString &iPrintDocPath,const QString &iFileName)
{
    bool rc = false;

    if(pElement)
    {

        rc = exportElementToXml(pElement,iPrintDocPath,iFileName);
        if(rc)
        {

            QString currentAppPath = QCoreApplication::applicationDirPath();
            QString strnsformAppPath = QDir::cleanPath( QString("%1%2%3").arg(currentAppPath,QDir::separator(),"GTATransformToDocx.exe"));
            QFileInfo infoExecFile( strnsformAppPath );

            QString xmlFile;
            if (!infoExecFile.exists())
            {
                rc = false;
                _LastError = "GTATransformToDocx not deployed";
            }
            else
            {
                QString docxFile = QString("%1/%2.docx").arg(iPrintDocPath,iFileName);
                xmlFile = QString("%1/%2.xml").arg(iPrintDocPath,iFileName);
                QProcess *pProcess = new QProcess();

                QStringList argument;
                argument<<xmlFile<<docxFile<<QString("PRINT_DOC");
                pProcess->start(strnsformAppPath,argument);
                pProcess->waitForFinished(-1);
                QString output  = QString(pProcess->readAllStandardOutput());
                if (!output.contains("Successful!",Qt::CaseInsensitive))
                {
                    rc = false;
                    _LastError = output;
                }
            }
            QFileInfo xmlFileInfo(xmlFile);
            if (xmlFileInfo.exists())
            {
                //DEV: change made so that xml file is also present when user wants to print a document.
                //requested by Franck because docx creation was taking a long time!
                //QFile::remove(QDir::cleanPath(xmlFile));
            }
        }
        else
        {
            _LastError += "could not convert to intermediate xml file";
        }

    }
    return rc;
}

void GTAAppController::createXmlElement(GTACommandBase* pCmd, QDomElement &oCommandsElem,QDomDocument &oDomDoc)
{
    if(pCmd)
    {
        QString statement = pCmd->getStatement();
        GTACommandBase::CommandType type = pCmd->getCommandType();
        QString sType;
        QString commandType;
        QString comment = pCmd->getComment();
        QString timeOut, timeOutUnit;
        QString precision,precisionUnit;
        QString actionOnFail;
        QString dumpList = pCmd->getDumpList().join(",");
        QString isIgnored = ((pCmd->isIgnoredInSCXML() == true ) ? "TRUE" : "FALSE");

        if(type == GTACommandBase::ACTION)
        {

            GTAActionBase *pActBase = dynamic_cast<GTAActionBase *>(pCmd);
            if(pActBase)
            {

                pActBase->getTimeOut(timeOut,timeOutUnit);
                pActBase->getPrecision(precision,precisionUnit);
                actionOnFail = pActBase->getActionOnFail();

                GTAActionPrint *printCmd = dynamic_cast<GTAActionPrint*>(pActBase);
                if(printCmd)
                {
                    sType = "Print";
                }

                else if(pActBase->isTitle())
                {
                    sType = "Title";
                }
                else if((pActBase->hasChildren() && !pActBase->hasReference()) || pCmd->IsEndCommand())
                {
                    sType = "Condition";
                }

                commandType = "Action";

            }
        }
        else if(type == GTACommandBase::CHECK)
        {
            commandType = "Check";

            GTACheckBase *pCheckBase = dynamic_cast<GTACheckBase *>(pCmd);
            if(pCheckBase)
            {
                pCheckBase->getTimeOut(timeOut,timeOutUnit);
                pCheckBase->getPrecision(precision,precisionUnit);
                actionOnFail = pCheckBase->getActionOnFail();
            }
        }

        QDomElement pCommand = oDomDoc.createElement(commandType);
        pCommand.setAttribute("STATEMENT",statement);
//        pCommand.setAttribute("COMMAND",commandType);
        pCommand.setAttribute("TYPE",sType);
        pCommand.setAttribute("COMMENT",comment);
        pCommand.setAttribute("PRECISION",precision);
        pCommand.setAttribute("PRECISION_UNIT",precisionUnit);
        pCommand.setAttribute("TIMEOUT",timeOut);
        pCommand.setAttribute("TIMEOUT_UNIT",timeOutUnit);
        pCommand.setAttribute("ACTION_ON_FAIL",actionOnFail);
        pCommand.setAttribute("DUMP_LIST",dumpList);
        pCommand.setAttribute("IS_IGNORED",isIgnored);
        oCommandsElem.appendChild(pCommand);

        if(!pCmd->hasReference() && pCmd->hasChildren())
        {
            QList<GTACommandBase *> lstChildren = pCmd->getChildren();
            if(!lstChildren.isEmpty())
            {
                for(int i = 0; i < lstChildren.count(); i++)
                {
                    GTACommandBase *cmdObj = lstChildren.at(i);

                    createXmlElement(cmdObj,oCommandsElem,oDomDoc);

                }
            }

        }
    }
}

/**
  * This function exports a (.pro/.fun/.obj/.seq) to an xml file
  * @pElement: Element that nedds to be converted to xml
  * @iPrintDocPath: The folder path where the file is to be saved.
  * @iFileName: name with which file must be saved
  * return : true if successful else false
  */

bool GTAAppController::exportElementToXml(GTAElement *pElement,const QString &iPrintDocPath,const QString &iFileName)
{
    QDomDocument domDoc;

    QDomElement rootElement = domDoc.createElement("ELEMENT");
    QDomElement headerElem = domDoc.createElement("HEADER");

    QDomElement commandsElem = domDoc.createElement("COMMANDS");

    QString name = pElement->getName();
    QString ext = getExtensionForElementType(pElement->getElementType());
    rootElement.setAttribute("NAME",QString("%1.%2").arg(name,ext));


    if(pElement)
    {


        int directChildrenCnt = pElement->getDirectChildrenCount();
        for(int i = 0; i < directChildrenCnt; i++)
        {
            GTACommandBase *pCmd = NULL;
            pElement->getDirectChildren(i,pCmd);
            if(pCmd)
            {

                createXmlElement(pCmd,commandsElem,domDoc);

            }
        }


        QDomElement DomHeader;
        GTADocumentController docCtrl;
        bool ret = docCtrl.getHeaderDom(pElement,domDoc,DomHeader);
        if(ret && (! DomHeader.isNull()))
        {
            headerElem.appendChild(DomHeader);
            // docCtrl.getFileInfoDom(domDoc,pElement,DomHeader);
        }

    }
    rootElement.appendChild(headerElem);
    rootElement.appendChild(commandsElem);
    domDoc.appendChild(rootElement);

    QString path = QString("%1/%2.xml").arg(iPrintDocPath,iFileName);

    QFile xmlFile(path);
    //    QFileInfo reportInfo(iPrintDocPath);

    bool rc = xmlFile.open(QFile::Text| QFile::WriteOnly);
    if(rc)
    {
        QTextStream writer(&xmlFile);
        writer << domDoc;
        xmlFile.close();
    }

    return rc;
}



bool GTAAppController::exportLTRAToXml1(GTAElement * pElem,const QString & iReportFile,const QStringList & iAttachmentList,bool isLTR)
{
    bool rc = false;
    QStringList attachmentList;
    attachmentList.append(iAttachmentList);
    QList<GTAActionCall *> CallList;
    if(pElem != NULL)
    {
        //bool bRepeatedLogging = false;
        //GTAAppSettings appSettings;
        //bRepeatedLogging = appSettings.getRepeatedLoggingSetting();

        //procInstr.setData(QString("<?xml version='1.0' encoding='UTF-8'?>"));
        QString TestSubject = QDir::cleanPath(getGTADataDirectory()+QDir::separator()+pElem->getName());
        GTAElement::ElemType sElemType = pElem->getElementType();

        QString elemExtension = GTAAppController::getExtensionForElementType(sElemType);
        TestSubject = QString("%1.%2").arg(TestSubject,elemExtension);

        QHash<QString, QList<GTACommandBase*>> titleBasedCmdList;
        pElem->groupCommandByTitleLTRA(titleBasedCmdList);

        QList<GTAActionPrintTable*> printTableLst;

        if (!titleBasedCmdList.isEmpty())
        {
            bool bMultipleTitleCommands = false;
            if (titleBasedCmdList.size()>1)
                bMultipleTitleCommands=true;
            else if (titleBasedCmdList.size()==1)
            {
                QString colNo1 = QString::number(0);
                GTACommandBase* pFistCmd =NULL;
                pFistCmd= titleBasedCmdList.value(colNo1).at(0);
                if (pFistCmd!=NULL && pFistCmd->isTitle())
                {
                    bMultipleTitleCommands=true;
                }
            }
            QDomDocument domDoc;
            QDomProcessingInstruction procInstr=domDoc.createProcessingInstruction("xml-stylesheet","type=\"text/xsl\" href=\"GTALTRATemplate.xsl\"");
            domDoc.appendChild(procInstr);
            QDomElement elementLogInfo = domDoc.createElement("LogInfo");
            if(!isLTR)
                elementLogInfo.setAttribute("Status","OK");
            else
                elementLogInfo.setAttribute("Status","");
            //TODO status updated not based on inputs

            //header .. move to different function getLTRAHeaderDom

            //LTR summary getHeaderFromDocument

            GTAHeader* pDocHeader = pElem->getHeader();

            QDomElement headerElement = domDoc.createElement("header");
            QStringList headerItems;
            QDomElement reportInfoElement;
            if ( pDocHeader!=NULL)
            {
                // REPORTINFO node

                reportInfoElement = domDoc.createElement("REPORTINFO");
                headerElement.appendChild(reportInfoElement);

                QDateTime currentDateTimeVal = QDateTime::currentDateTime();
                QString stodaysDate = currentDateTimeVal.date().toString();
                QDomElement dateElement = domDoc.createElement("DATE");
                QDomText dateNode = domDoc.createTextNode(stodaysDate);
                dateElement.appendChild(dateNode);
                reportInfoElement.appendChild(dateElement);

                QString testTitle = pElem->getName();
                QDomElement titleElement = domDoc.createElement("TITLE");
                titleElement.appendChild(domDoc.createTextNode(testTitle));
                reportInfoElement.appendChild(titleElement);


                QString ltrDesignerName; //mapped to Designer name
                int desIndex = pDocHeader->getIndex("Designer Name");
                headerItems.append("Designer Name");
                if (desIndex!=-1)
                    ltrDesignerName = pDocHeader->getFieldValue(desIndex);

                QDomElement designerNameElement = domDoc.createElement("DESIGNERNAME");
                designerNameElement.appendChild(domDoc.createTextNode(ltrDesignerName));
                reportInfoElement.appendChild(designerNameElement);

                QString ltrReference;//mapped to reference
                desIndex = pDocHeader->getIndex("LTR_Title_Reference");
                headerItems.append("LTR_Title_Reference");
                if (desIndex!=-1)
                    ltrReference = pDocHeader->getFieldValue(desIndex);


                QDomElement ltrRefElement = domDoc.createElement("LTRREF");
                ltrRefElement.appendChild(domDoc.createTextNode(ltrReference));
                reportInfoElement.appendChild(ltrRefElement);

                // TESTDETAIL node
                QDomElement testDetailElement = domDoc.createElement("TESTDETAIL");
                headerElement.appendChild(testDetailElement);

                //Test number
                QString sTestNumber;//mapped to reference
                desIndex = pDocHeader->getIndex("Test_Number");
                headerItems.append("Test_Number");
                if (desIndex!=-1)
                    sTestNumber = pDocHeader->getFieldValue(desIndex);


                QDomElement ltrTestItem = domDoc.createElement("Item");
                ltrTestItem.setAttribute("value", sTestNumber);
                ltrTestItem.setAttribute("name","Test_Number");
                testDetailElement.appendChild(ltrTestItem);


                //Test title
                QString sDescription;//mapped to Description
                desIndex = pDocHeader->getIndex("DESCRIPTION");
                headerItems.append("DESCRIPTION");
                if (desIndex!=-1)
                    sDescription = pDocHeader->getFieldValue(desIndex);


                QDomElement ltrTestTitle = domDoc.createElement("Item");
                ltrTestTitle.setAttribute("value", sDescription);
                ltrTestTitle.setAttribute("name","Test Title");
                testDetailElement.appendChild(ltrTestTitle);

                //Facility used
                QString sfacilityUsed;//mapped to Facility
                desIndex = pDocHeader->getIndex("Facility");
                headerItems.append("Facility");
                if (desIndex!=-1)
                    sfacilityUsed = pDocHeader->getFieldValue(desIndex);


                QDomElement ltrFacilityElem = domDoc.createElement("Item");
                ltrFacilityElem.setAttribute("value", sfacilityUsed);
                ltrFacilityElem.setAttribute("name","Facility Used");
                testDetailElement.appendChild(ltrFacilityElem);

                QString attachmentStatus = iAttachmentList.isEmpty()?"No":"YES";
                QDomElement attachmentStatusItem = domDoc.createElement("Item");
                attachmentStatusItem.setAttribute("value", attachmentStatus);
                attachmentStatusItem.setAttribute("name","Attachment");

                testDetailElement.appendChild(attachmentStatusItem);
                // to be filled from header dom

                //Specification Node

                QDomElement specificatinNode = domDoc.createElement("SPECIFICATION");
                headerElement.appendChild(specificatinNode);


                QString specificationIssueReq;//mapped to Facility
                desIndex = pDocHeader->getIndex("Specification Issue/Requirement");
                headerItems.append("Specification Issue/Requirement");
                if (desIndex!=-1)
                    specificationIssueReq = pDocHeader->getFieldValue(desIndex);

                QStringList specList = specificationIssueReq.split("/");


                if(!specList.isEmpty())
                {
                    QDomElement attachmentStatusItem = domDoc.createElement("Item");
                    attachmentStatusItem.setAttribute("value", specList.at(0));
                    attachmentStatusItem.setAttribute("name","Specification issue / section");
                    specificatinNode.appendChild(attachmentStatusItem);

                    if (specList.size()>=2)
                    {
                        QDomElement attachmentStatusItem = domDoc.createElement("Item");
                        attachmentStatusItem.setAttribute("value", specList.at(1));
                        attachmentStatusItem.setAttribute("name","Requirement");
                        specificatinNode.appendChild(attachmentStatusItem);
                    }
                }
                /////////////////////////////////////////////////////////////
                //new node OtherHeaderItems not present in list headerItems;
                int hdrSize = pDocHeader->itemSize();

                QDomElement otherHeaderItemNode = domDoc.createElement("OTHERDETAILS");
                for (int i=0;i<hdrSize;i++)
                {
                    QString sFieldName = pDocHeader->getFieldName(i);
                    if (!headerItems.contains(sFieldName))
                    {
                        QDomElement itemElement = domDoc.createElement("Item");
                        itemElement.setAttribute("name",sFieldName);
                        desIndex = pDocHeader->getIndex(sFieldName);
                        itemElement.setAttribute("value", pDocHeader->getFieldValue(desIndex));

                        if (pDocHeader->getShowInLTRA(i))
                        {
                            otherHeaderItemNode.appendChild(itemElement);
                        }

                    }
                }
                headerElement.appendChild(otherHeaderItemNode);

            }

            bool bInsertedTimeInfo = false;

            QDomElement procedurePurposeElem = domDoc.createElement("PROCEDURE_PURPOSE");
            QDomElement procedureConclusionElem = domDoc.createElement("PROCEDURE_CONCLUSION");

            procedurePurposeElem.setAttribute("value", pElem->getPurposeForProcedure());
            procedureConclusionElem.setAttribute("value", pElem->getConclusionForProcedure());

            QDomElement summaryElement = domDoc.createElement("summary");


            domDoc.appendChild(elementLogInfo);
            elementLogInfo.appendChild(headerElement);

            if (pElem->getElementType() == GTAElement::ElemType::PROCEDURE)
            {
                elementLogInfo.appendChild(procedurePurposeElem);
                elementLogInfo.appendChild(procedureConclusionElem);
            }

            elementLogInfo.appendChild(summaryElement);


            QDomElement procElement = domDoc.createElement("PROC");
            elementLogInfo.appendChild(procElement);

            QHashIterator<QString, QList<GTACommandBase*>> iterHsh(titleBasedCmdList);

            QFile objFile(iReportFile);
            QFileInfo info(iReportFile);
            QString reportFileName = info.fileName();
            QStringList parameterList;


            for (int i=0;i<=titleBasedCmdList.size();i++)
            {
                QString colNo = QString::number(i);
                if (titleBasedCmdList.contains(colNo))
                {


                    QList<GTACommandBase*> cmdList = titleBasedCmdList.value(colNo);

                    GTACommandBase* pStartingCmd=NULL;

                    if (!cmdList.isEmpty())
                    {
                        bool itemStatus = true; //All command under one title will have one status
                        pStartingCmd = cmdList.at(0);
                        int startintCmdCnt =0;
                        //Each title heading.
                        QString titleHeading;
                        QString titleComment;
                        QDomElement titleElement;
                        QString titleStatus = "";
                        bool insertedItemForSingleCommand=false;
                        if (pStartingCmd->isTitle())
                        {
                            titleHeading=pStartingCmd->getStatement();
                            titleComment=pStartingCmd->getComment();
                            startintCmdCnt++;

                            titleElement = domDoc.createElement("title");
                            titleElement.setAttribute("Heading",titleHeading);
                            titleElement.setAttribute("commandComment",titleComment);
                            procElement.appendChild(titleElement);



                            GTAActionParentTitle* parentTitle = dynamic_cast<GTAActionParentTitle*>(pStartingCmd);
                            if (parentTitle)
                            {
                                titleStatus= parentTitle->getGlobalResultStatus();
                                if (titleStatus.isEmpty())
                                    titleStatus = "NA";
                                QDomElement statusInfoElem = domDoc.createElement("status");
                                QDomText domStatusTxt = domDoc.createTextNode(titleStatus);
                                statusInfoElem.appendChild(domStatusTxt);

                                titleElement.appendChild(statusInfoElem);

                            }


                        }




                        bool addToAct=false;
                        bool addToChk=false;
                        QDomElement actionTypeElement;
                        QList<int> indentList;
                        for (int ii =startintCmdCnt;ii<cmdList.size();ii++)
                        {
                            GTACommandBase * pCmd = NULL;
                            pCmd= cmdList.at(ii);

                            if(pCmd!=NULL && pCmd->getReportStatus()==GTACommandBase::EXCLUDE)
                                continue;

                            if (pCmd!=NULL)
                            {
                                GTACommandBase* pMasterCmd = pCmd->getMasterCmd();
                                if(pMasterCmd != NULL && pMasterCmd!=pCmd)
                                {
                                    if(pMasterCmd->hasReference() && pMasterCmd->getCollapsed())
                                    {
                                        continue;
                                    }
                                }
                            }
                            if (!startintCmdCnt && pCmd->getParent()==NULL)
                            {
                                titleElement = domDoc.createElement("title");
                                titleElement.setAttribute("Heading","");
                                titleElement.setAttribute("commandComment",titleComment);
                                procElement.appendChild(titleElement);
                            }

                            if (pCmd!=NULL)
                            {
                                if (!indentList.isEmpty())
                                {
                                    for(int j= indentList.size()-1;j>=0;j--)
                                    {
                                        if (indentList.at(j)==0)
                                        {

                                            indentList.removeLast();
                                        }
                                        else
                                            break;

                                    }

                                }
                                if (!indentList.isEmpty())
                                {
                                    int lastDecrement = indentList.takeAt(indentList.size()-1);
                                    lastDecrement--;
                                    indentList.append(lastDecrement);
                                }
                                QStringList varList = pCmd->getVariableList();
                                if(! varList.isEmpty())
                                {
                                    parameterList.append(varList);
                                }
                                QList<GTAScxmlLogMessage> logMsgList = pCmd->getLogMessageList();

                                /*if(logMsgList.isEmpty() && !pCmd->hasReference())
                                    continue;*/
                                QString userComment = pCmd->getUserLogComment();
                                QString cmdComment = pCmd->getComment();
                                QString description = "";
                                QString actionName, complement,actionOnFail,timeOut, timeOutUnit, precision, precisionType;
                                QDomElement commandElement;


                                if (pCmd->isTitle())
                                {
                                    commandElement = domDoc.createElement("ADD_TITLE");
                                    commandElement.setAttribute("title",pCmd->getStatement());
                                }
                                else if(pCmd->getCommandType() == GTACommandBase::ACTION)
                                {
                                    /*if(pCmd->hasReference() && logMsgList.isEmpty())
                                        continue;*/
                                    GTAActionBase * pAction = dynamic_cast<GTAActionBase*>(pCmd);
                                    if(pAction != NULL)
                                    {
                                        actionName = pAction->getAction();
                                        complement = pAction->getComplement();
                                        pAction->getTimeOut(timeOut,timeOutUnit);
                                        pAction->getPrecision(precision,precisionType);
                                        actionOnFail = pAction->getActionOnFail();

                                        QString actionNodeName = QString("ACT");
                                        QString actcompName ;
                                        if (!actionName.isEmpty())
                                            actcompName.append(actionName);
                                        if (!complement.isEmpty())
                                            actcompName.append("_"+complement);

                                        if(!addToAct)
                                        {
                                            addToAct = true;
                                            addToChk = false;
                                            actionTypeElement = domDoc.createElement("ACTIONS");
                                            //actionTypeElement.setAttribute("isUnderTitle",startintCmdCnt>0?"true":"false");
                                        }
                                        if(!startintCmdCnt && pCmd->getParent()==NULL)
                                            actionTypeElement = domDoc.createElement("ACTIONS");
                                        commandElement = domDoc.createElement(actionNodeName);
                                        commandElement.setAttribute("isUnderTitle",startintCmdCnt>0?"true":"false");
                                        QString confirmationTime,ConfTimeUnit;
                                        pAction->getConfCheckTime(confirmationTime,ConfTimeUnit);
                                        commandElement.setAttribute("confirmationTime",QString("%1%2").arg(confirmationTime,ConfTimeUnit));

                                        QString timeOutTime,timeOutUnit;
                                        pAction->getTimeOut(timeOutTime,timeOutUnit);
                                        commandElement.setAttribute("timeOut",QString("%1%2").arg(timeOutTime,timeOutUnit));

                                        QString sUserFeedback = pAction->getUserComment();
                                        QString sReference = pAction->getReferences();
                                        commandElement.setAttribute("userFeedback",sUserFeedback);
                                        commandElement.setAttribute("references",sReference);

                                        if(!actcompName.isEmpty())
                                        {
                                            actcompName.replace(QString(" "),QString("_"));
                                            commandElement.setAttribute("actcomp",actcompName);

                                        }
                                    }
                                }
                                else if(pCmd->getCommandType() == GTACommandBase::CHECK)
                                {
                                    GTACheckBase * pCheck = dynamic_cast<GTACheckBase*>(pCmd);
                                    if(pCheck != NULL)
                                    {
                                        actionName = pCheck->getCheckName();

                                        pCheck->getTimeOut(timeOut,timeOutUnit);
                                        pCheck->getPrecision(precision,precisionType);
                                        actionOnFail = pCheck->getActionOnFail();

                                        QString actionNodeName = QString("CHK");
                                        QString actcompName=actionName;


                                        actionNodeName.replace(" ","_");
                                        if(!addToChk)
                                        {
                                            addToAct = false;
                                            addToChk = true;
                                            actionTypeElement = domDoc.createElement("RESULTS");
                                            //actionTypeElement.setAttribute("isUnderTitle",startintCmdCnt>0?"true":"false");
                                        }
                                        if(!startintCmdCnt && pCmd->getParent()==NULL)
                                            actionTypeElement = domDoc.createElement("RESULTS");

                                        commandElement = domDoc.createElement(actionNodeName);
                                        commandElement.setAttribute("isUnderTitle",startintCmdCnt>0?"true":"false");
                                        if(!actcompName.isEmpty())
                                        {
                                            commandElement.setAttribute("actcomp",actcompName);
                                            actcompName.replace(QString(" "),QString("_"));
                                        }
                                        QString confirmationTime,ConfTimeUnit;
                                        pCheck->getConfCheckTime(confirmationTime,ConfTimeUnit);
                                        commandElement.setAttribute("confirmationTime",QString("%1%2").arg(confirmationTime,ConfTimeUnit));

                                        QString timeOutTime,timeOutUnit;
                                        pCheck->getTimeOut(timeOutTime,timeOutUnit);
                                        commandElement.setAttribute("timeOut",QString("%1%2").arg(timeOutTime,timeOutUnit));


                                        QString sUserFeedback = pCheck->getUserComment();
                                        QString sReference = pCheck->getReferences();
                                        commandElement.setAttribute("userFeedback",sUserFeedback);
                                        commandElement.setAttribute("references",sReference);

                                    }
                                }
                                if(!precisionType.isEmpty() && precision.isEmpty())
                                    precision = precision + precisionType;

                                if(!timeOutUnit.isEmpty() && timeOutUnit.contains("ms") && ! timeOut.isEmpty())
                                {
                                    double val = timeOut.toDouble() * 1000;
                                    timeOut = QString::number(val);
                                }
                                if(! timeOut.isEmpty())
                                    timeOut +="s";

                                QString statment = pCmd->getStatement();
                                //statment.replace("\n"," ");

                                commandElement.setAttribute("indent",indentList.size());

                                if (pCmd->hasChildren() || pCmd->hasReference())
                                {
                                    int indentChild= pCmd->getChildren().count();
                                    //code for indentation
                                    if (indentChild && ((!pCmd->getCollapsed() && pCmd->hasReference()) || pCmd->hasChildren()))
                                        indentList.append(indentChild);
                                    ///////////////////////////////////////////
                                }


                                if(pCmd->hasReference())
                                {


                                    GTAActionCall* pCall = dynamic_cast<GTAActionCall*>(pCmd);
                                    QString funcName = pCall->getElement();
                                    //IKM
                                    if(iAttachmentList.contains(TestSubject))
                                    {
                                        attachmentList.append(QDir::cleanPath(getGTADataDirectory()+QDir::separator()+funcName));
                                    }

                                    GTAHeader* pHeader = NULL;
                                    QString resultStatusVal = "NA";
                                    bool headerRc = getHeaderFromDocument(funcName,pHeader);
                                    if(pHeader!=NULL && headerRc)
                                    {
                                        int desIndex = pHeader->getIndex("DESCRIPTION");
                                        if (desIndex!=-1)
                                        {
                                            description = pHeader->getFieldValue(desIndex);
                                        }
                                    }
                                    if(!pCall->getChildren().isEmpty())
                                    {
                                        GTACommandBase* pFirstChild = pCall->getFirstNonCallChild();
                                        QString result = pCall->getGlobalResultStatus();
                                        if (pFirstChild)
                                        {
                                            QList<GTAScxmlLogMessage> chldMsLogList = pFirstChild->getLogMessageList();
                                            foreach(GTAScxmlLogMessage objLog, chldMsLogList)
                                            {
                                                if (objLog.LOD==GTAScxmlLogMessage::Main)
                                                {
                                                    if (!commandElement.isNull())
                                                    {
                                                        if (!objLog.TimeStamp.isEmpty())
                                                        {
                                                            QString epocTimeStamp = objLog.TimeStamp;
                                                            epocTimeStamp.remove("[");
                                                            epocTimeStamp.remove("]");
                                                            qint64 i64Time = epocTimeStamp.toLongLong();
                                                            QDateTime execTime = QDateTime::fromMSecsSinceEpoch(i64Time);
                                                            QString execDateStr = execTime.date().toString();
                                                            QString execTimeStr = execTime.time().toString();
                                                            QDomElement element = domDoc.createElement("timestamp");
                                                            QDomText domTxt = domDoc.createTextNode(QString("%1 (%2)").arg(execTimeStr,execDateStr));
                                                            if(result == "OK")
                                                                resultStatusVal = "OK";
                                                            else if(result == "KO")
                                                                resultStatusVal = "KO";
                                                            element.appendChild(domTxt);
                                                            commandElement.appendChild(element);
                                                        }
                                                    }
                                                }
                                            }
                                        }

                                    }
                                    QString callStatement;
                                    callStatement=pCall->getElement();
                                    QStringList tagVals;
                                    QMap<QString,QString> mapOfCallValue = pCall->getTagValue();
                                    QMapIterator<QString, QString> iMapIter(mapOfCallValue);
                                    while (iMapIter.hasNext()) {
                                        iMapIter.next();
                                        tagVals.append(QString("%1=%2").arg(iMapIter.key(),iMapIter.value()));
                                    }
                                    QString tagVal = tagVals.join(",");
                                    if(!tagVals.isEmpty())
                                    {
                                        callStatement.append(QString(" [ %1 ]").arg(tagVal));
                                    }
                                    QDomElement element = domDoc.createElement("statement");
                                    QDomText domTxt = domDoc.createTextNode(callStatement);
                                    element.appendChild(domTxt);

                                    QDomElement element1 = domDoc.createElement("status");
                                    QDomText domTxt2;
                                    //                                    if(resultStatusVal)
                                    //                                        domTxt2 = domDoc.createTextNode("KO");
                                    //                                    else
                                    //                                        domTxt2 = domDoc.createTextNode("OK");
                                    if(!isLTR)
                                        domTxt2 = domDoc.createTextNode(resultStatusVal);
                                    else
                                        domTxt2 = domDoc.createTextNode("");
                                    element1.appendChild(domTxt2);
                                    commandElement.appendChild(element1);
                                    commandElement.appendChild(element);


                                    //summaryElement.appendChild(itemElement);
                                }
                                else if (pCmd->hasChildren())
                                {
                                    //code for indentation
                                    int indentChild= pCmd->getChildren().count();//getAllChildrenCount();
                                    if (indentChild && pCmd->getCollapsed())
                                        indentList.append(indentChild);
                                    ///////////////////////////////////////////
                                }


                                if (!commandElement.isNull())
                                    actionTypeElement.appendChild(commandElement);

                                commandElement.setAttribute("userLogcomment","");
                                commandElement.setAttribute("commandComment","");
                                commandElement.setAttribute("description",description);


                                GTAActionPrintTable* pTable = dynamic_cast<GTAActionPrintTable*>(pCmd);
                                if(NULL!=pTable)
                                    printTableLst.append(pTable);
                                GTAActionFTAKinematicMultiOutput *pKinematicCmd = dynamic_cast<GTAActionFTAKinematicMultiOutput*>(pCmd);
                                if(pKinematicCmd != NULL)
                                {
                                    printTableLst.append(pKinematicCmd->getPrintTableCmdList());
                                    QString result = pKinematicCmd->getGlobalResultStatus();

                                    GTAScxmlLogMessage msgLogK;
                                    if(!result.isEmpty() && result!= "NA")
                                    {

                                        msgLogK.Result = msgLogK.StringToResultType(result);
                                        msgLogK.LOD = GTAScxmlLogMessage::Main;
                                        logMsgList.append(msgLogK);
                                        pKinematicCmd->setLogMessageList(QList<GTAScxmlLogMessage>()<<msgLogK);
                                        bMultipleTitleCommands = true;

                                    }
                                }
                                if(logMsgList.isEmpty() ==false /*&&   pCmd->getParent()==NULL*/)
                                {
                                    QDomElement otherDetailelement = domDoc.createElement("otherDetails");
                                    QDomElement referencesDom= domDoc.createElement("DEFECTS");
                                    QStringList otherDetailLst;



                                    foreach(GTAScxmlLogMessage objLog, logMsgList)
                                    {
                                        if (objLog.LOD==GTAScxmlLogMessage::Main)
                                        {
                                            if(objLog.Result==GTAScxmlLogMessage::KO)
                                            {
                                                itemStatus=false;

                                            }
                                            if (!commandElement.isNull())
                                            {
                                                if (!objLog.TimeStamp.isEmpty())
                                                {
                                                    QString epocTimeStamp = objLog.TimeStamp;
                                                    epocTimeStamp.remove("[");
                                                    epocTimeStamp.remove("]");
                                                    qint64 i64Time = epocTimeStamp.toLongLong();
                                                    QDateTime execTime = QDateTime::fromMSecsSinceEpoch(i64Time);
                                                    QString execDateStr = execTime.date().toString();
                                                    QString execTimeStr = execTime.time().toString();
                                                    QDomElement element = domDoc.createElement("timestamp");

                                                    QDomText domTxt = domDoc.createTextNode(QString("%1 (%2)").arg(execTimeStr,execDateStr));
                                                    element.appendChild(domTxt);
                                                    commandElement.appendChild(element);

                                                    if (!bInsertedTimeInfo && !reportInfoElement.isNull())
                                                    {
                                                        QDomElement elementTimeReport = domDoc.createElement("executionTime");
                                                        QDomText domTimeTxt = domDoc.createTextNode(QString("%1 (%2)").arg(execTimeStr,execDateStr));
                                                        elementTimeReport.appendChild(domTimeTxt);
                                                        reportInfoElement.appendChild(elementTimeReport);
                                                        bInsertedTimeInfo=true;
                                                    }
                                                }

                                                QString resultStatus = objLog.ResultTypeToString(objLog.Result);
                                                if (!resultStatus.isEmpty())
                                                {
                                                    QDomElement element = domDoc.createElement("status");
                                                    QDomText domTxt;
                                                    if(!isLTR)
                                                        domTxt = domDoc.createTextNode(resultStatus);
                                                    else
                                                        domTxt = domDoc.createTextNode("");
                                                    element.appendChild(domTxt);
                                                    commandElement.appendChild(element);
                                                }

                                                if (!statment.isEmpty())
                                                {
                                                    QDomElement element = domDoc.createElement("statement");
                                                    QDomText domTxt = domDoc.createTextNode(statment);
                                                    element.appendChild(domTxt);
                                                    commandElement.appendChild(element);
                                                }

                                                //commandElement.setAttribute("timestamp",objLog.TimeStamp);
                                                commandElement.setAttribute("userLogcomment",userComment);
                                                commandElement.setAttribute("commandComment",cmdComment);


                                                if(bMultipleTitleCommands == false)
                                                {
                                                    insertedItemForSingleCommand = true;
                                                    if (!resultStatus.isEmpty() )
                                                    {
                                                        GTACommandBase* pParent = pCmd->getParent();
                                                        if (pParent!=NULL)
                                                        {
                                                            if(!pParent->hasReference())
                                                            {
                                                                QDomElement itemElement = domDoc.createElement("item");
                                                                itemElement.setAttribute("status",resultStatus);
                                                                QDomText domTxt = domDoc.createTextNode(statment);
                                                                itemElement.appendChild(domTxt);
                                                                summaryElement.appendChild(itemElement);
                                                            }
                                                            else
                                                            {

                                                                QString statementofParent = pParent->getStatement();
                                                                QString resultStatusofParent = "NA";
                                                                GTAActionCall *pCallcmd = dynamic_cast<GTAActionCall *>(pParent);
                                                                if(!CallList.contains(pCallcmd))
                                                                {
                                                                    if(pCallcmd != NULL)
                                                                    {
                                                                        resultStatusofParent = pCallcmd->getGlobalResultStatus();
                                                                        CallList.append(pCallcmd);
                                                                    }
                                                                    QDomElement itemElement = domDoc.createElement("item");
                                                                    itemElement.setAttribute("status",resultStatusofParent);
                                                                    QDomText domTxt = domDoc.createTextNode(statementofParent);
                                                                    itemElement.appendChild(domTxt);
                                                                    summaryElement.appendChild(itemElement);
                                                                }
                                                            }
                                                        }
                                                        else
                                                        {

                                                            QDomElement itemElement = domDoc.createElement("item");
                                                            itemElement.setAttribute("status",resultStatus);
                                                            QDomText domTxt = domDoc.createTextNode(statment);
                                                            itemElement.appendChild(domTxt);
                                                            summaryElement.appendChild(itemElement);

                                                        }
                                                    }

                                                }
                                            }
                                        }//LOD:Main
                                        else
                                            if (objLog.LOD==GTAScxmlLogMessage::Detail)
                                            {

                                                if (pCmd)
                                                {
                                                    GTAActionIRT* pIRTcmd = dynamic_cast<GTAActionIRT*>(pCmd);
                                                    if (pIRTcmd)
                                                    {
                                                        QString currentVal = objLog.CurrentValue;
                                                        QDateTime dateTime2;

                                                        QString imgFile = QString(QString("%1_%2.png")).arg("image",dateTime2.currentDateTime().toString("dd.MM.yyyy.hh.mm.ss"));
                                                        QString imgFilePath = QDir::cleanPath(QString("%1%2%3").arg(this->getTempDataDirectory(),QDir::separator(),imgFile));


                                                        //GTAUtil::writeImage(currentVal,imgFilePath);
                                                        QDomElement domRefNode = domDoc.createElement(QString("%1").arg("DEFECT"));
                                                        referencesDom.appendChild(domRefNode);
                                                        domRefNode.setAttribute("type","Image");
                                                        domRefNode.setAttribute("source",QString("./%1_files/%2").arg(reportFileName.remove(".xml"),imgFile));
                                                        attachmentList.append(imgFilePath);

                                                    }
                                                    else
                                                    {
                                                        if(pCmd->hasLoop())// && bRepeatedLogging == false)
                                                        {
                                                            ;// do nothing;


                                                        }
                                                        else
                                                        {
                                                            QDomElement infoElement = domDoc.createElement("info");
                                                            QDomElement paramName = domDoc.createElement("paramName");
                                                            QDomElement paramVal = domDoc.createElement("value");

                                                            QDomText domTxtParam = domDoc.createTextNode(objLog.ExpectedValue);
                                                            QDomText domTxtVal = domDoc.createTextNode(objLog.CurrentValue);
                                                            paramName.appendChild(domTxtParam);
                                                            paramVal.appendChild(domTxtVal);

                                                            infoElement.appendChild(paramName);
                                                            infoElement.appendChild(paramVal);
                                                            otherDetailelement.appendChild(infoElement);
                                                        }
                                                        //QString otherDetail=QString("%1=%2").arg(objLog.ExpectedValue,objLog.CurrentValue);
                                                        ////extract other detial here
                                                        //otherDetailLst.append(otherDetail);
                                                    }
                                                }
                                            }
                                    }//foreach logMsgLst
                                    /*if (!otherDetailLst.isEmpty())
                                    {
                                        QDomText domTxt = domDoc.createTextNode(QString("%1").arg(otherDetailLst.join(",")));
                                        otherDetailelement.appendChild(domTxt);

                                    }*/

                                    commandElement.appendChild(referencesDom);
                                    commandElement.appendChild(otherDetailelement);

                                }//if logMsgLst.isEmpty()

                                if (pCmd->IsUserEditable())
                                {
                                    if (!statment.isEmpty())
                                    {
                                        QDomElement element = domDoc.createElement("statement");
                                        QDomText domTxt = domDoc.createTextNode(statment);
                                        element.appendChild(domTxt);
                                        commandElement.appendChild(element);
                                    }
                                }
                                else
                                {
                                    if (!statment.isEmpty())
                                    {
                                        QDomElement element = domDoc.createElement("statement");
                                        QDomText domTxt = domDoc.createTextNode(statment);
                                        element.appendChild(domTxt);
                                        commandElement.appendChild(element);
                                    }
                                }
                                if (pCmd->hasReference())
                                {
                                    QDomElement indentInfoElement = domDoc.createElement("ADD_INDENT");
                                    indentInfoElement.setAttribute("count",pCmd->getAllChildrenCount());
                                    titleElement.appendChild(indentInfoElement);

                                }
                                if(!actionTypeElement.isNull())
                                    titleElement.appendChild(actionTypeElement);
                            }

                        }//for each command under title

                        if (itemStatus==false)
                            elementLogInfo.setAttribute("Status","KO");


                        /*if(! parameterList.isEmpty())
                        {
                            QDomElement ElemOnLoginfo= domDoc.createElement("PARAMETERLIST");
                            parameterList.removeDuplicates();

                            foreach(QString param, parameterList)
                            {
                                QDomElement ElemOnParamList= domDoc.createElement("parameter");
                                QDomText domTxt = domDoc.createTextNode(param);
                                ElemOnParamList.appendChild(domTxt);
                                ElemOnLoginfo.appendChild(ElemOnParamList);
                            }
                            elementLogInfo.appendChild(ElemOnLoginfo);
                        }*/


                        QDomElement itemElement = domDoc.createElement("item");
                        if (bMultipleTitleCommands)
                        {
                            if (cmdList.size()==1)
                            {
                                itemElement.setAttribute("status","NA");
                            }
                            else if(itemStatus)
                                itemElement.setAttribute("status","OK");
                            else
                                itemElement.setAttribute("status","KO");
                            if(!titleStatus.isEmpty()) // over-ride if title status exists
                                itemElement.setAttribute("status",titleStatus);
                            if(isLTR)
                                itemElement.setAttribute("status","");
                            QDomText domTxt1 = domDoc.createTextNode(pStartingCmd->getStatement());
                            itemElement.appendChild(domTxt1);
                            summaryElement.appendChild(itemElement);


                        }
                        else if(insertedItemForSingleCommand == false )
                        {
                            QDomText domTxt1 = domDoc.createTextNode(pStartingCmd->getStatement());
                            itemElement.appendChild(domTxt1);
                            summaryElement.appendChild(itemElement);
                        }
                    }
                }
            }

            QString attchDir = QDir::cleanPath (info.absolutePath()+QDir::separator()+QString(reportFileName.remove(".xml")+"_files")); //so that on moving html associated links are moved together
            GTAUtil::clearDirectory(attchDir);
            QDir attachmentDir(attchDir);
            attachmentDir.mkdir(attchDir);

            if(! attachmentList.isEmpty())
            {
                QDomElement ElemOnAttachment = domDoc.createElement("ATTACHEMENTS");
                elementLogInfo.appendChild(ElemOnAttachment);
                foreach(QString attachment, attachmentList)
                {
                    QFileInfo info1(attachment);
                    QFile::copy(attachment,attchDir+QDir::separator()+QString(info1.fileName()));
                    QDomElement ElemOnFile = domDoc.createElement("FILE");
                    ElemOnAttachment.appendChild(ElemOnFile);
                    ElemOnFile.setAttribute("name",info1.fileName());
                    QString pathAttrVal = QString("%1%2%3%4").arg(QString("./"),QString(reportFileName.remove(".xml")+"_files"),QString("/"),QString(info1.fileName()) );
                    ElemOnFile.setAttribute("path",pathAttrVal );
                }
            }

            if(! parameterList.isEmpty())
            {
                parameterList.removeDuplicates();
                QString parameterFilePath = QDir :: cleanPath( QString("%1%2%3").arg(attchDir,QDir::separator(),"parameters.txt"));
                QFile paramListFile(parameterFilePath );
                paramListFile.open(QFile::Text| QFile::WriteOnly);


                QTextStream paramWriterstream(&paramListFile);
                QByteArray contents = parameterList.join("\n").toLatin1();

                paramWriterstream<<contents;
                paramListFile.close();

                QDomElement ElemOnLoginfo= domDoc.createElement("PARAMETERLIST");
                parameterList.removeDuplicates();
                QDomElement ElemOnParamList= domDoc.createElement("parameter");
                QDomText domTxt = domDoc.createTextNode(parameterFilePath);
                ElemOnParamList.appendChild(domTxt);
                ElemOnLoginfo.appendChild(ElemOnParamList);
                elementLogInfo.appendChild(ElemOnLoginfo);


            }

            rc = objFile.open(QFile::Text| QFile::WriteOnly);
            if(rc)
            {
                QTextStream writer(&objFile);
                QByteArray contents =  domDoc.toByteArray();
                writer<<contents;
                objFile.close();
            }
            QHash<QString, GTAPrintTableRecord> printTableRecord;
            QStringList varList;
            foreach(GTAActionPrintTable *pPrintTableCmd, printTableLst)
            {
                varList.append(pPrintTableCmd->getVariableList());
            }

            QString dbFile = _pSystemServices->getDatabaseFile();
            GTADataController dataCtrlr;
            QStringList invalidParams;
            QHash<QString,GTAICDParameter> tempParamList;
            rc = dataCtrlr.getParametersDetail(varList,dbFile,tempParamList,invalidParams);
            if (!printTableLst.isEmpty())
            {
                foreach(GTAActionPrintTable *pPrintTableCmd, printTableLst)
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
                    foreach(QString paramName, pPrintTableCmd->getVariableList())
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

            foreach(QString printTableName,printTableRecord.keys())
            {
                GTAPrintTableRecord tableRecord = printTableRecord.value(printTableName);
                QString csvFileName = QString("%1/%2.csv").arg(attchDir,printTableName);
                QFile csvName(csvFileName);
                rc = csvName.open(QFile::Text| QFile::WriteOnly);
                if(rc)
                {
                    QTextStream writer(&csvName);
                    QHash<QString,QStringList>  rprtData = tableRecord.getReportData();
                    QString colum = tableRecord.getHeader().join(";");
                    QByteArray contents (colum.toUtf8 ());
                    writer<<contents;
                    writer<<"\n";
                    foreach(QString pramName,rprtData.keys())
                    {
                        QStringList items;
                        QString signalType= tableRecord.getParamSignalType(pramName);
                        QString unit = tableRecord.getParamSignalUnit(pramName);
                        items.append(pramName);
                        items.append(signalType);
                        items.append(unit);
                        items.append(rprtData.value(pramName));
                        colum = items.join(";");
                        QByteArray contents (colum.toUtf8 ());
                        writer<<contents;
                        writer<<"\n";
                    }

                    objFile.close();
                }

            }
            rc=true;
        }
    }
    return rc;
}

/**
  * This function export the LTRA-Lab Test Report Analysis to csv, delimiter is ':::'
  * @pElem: Element to be exported
  * @iReportFile: Report file full path where the report is created
  * @iAttachmentList: list of documents to be attached with report
  */
bool GTAAppController::exportLTRAToCsv(GTAElement * pElem,const QString & iReportFile,const QStringList & iAttachmentList)
{
    bool rc = false;
    if(pElem != NULL)
    {
        QFile objFile(iReportFile);
        rc = objFile.open(QFile::Text| QFile::WriteOnly);
        if(rc == false)
            return rc;

        QTextStream writer(&objFile);
        QString delimS = ":::";

        //file header
        QString logFileHeader = "LogType";
        logFileHeader += delimS + "TimeStamp";
        logFileHeader += delimS + "ActionName";
        logFileHeader += delimS + "Complement";
        logFileHeader += delimS + "TimeOut";
        logFileHeader += delimS + "ActionOnFail";
        logFileHeader += delimS + "Precision";
        logFileHeader += delimS + "CmdComment";
        logFileHeader += delimS + "Statment";
        logFileHeader += delimS + "Identifier";
        logFileHeader += delimS + "Result";
        logFileHeader += delimS + "Status";
        logFileHeader += delimS + "LOD";
        logFileHeader += delimS + "ExpectedValue";
        logFileHeader += delimS + "CurrentValue";
        logFileHeader += delimS + "FunctionStatus";
        logFileHeader += delimS + "UserLogComment";
        writer<<logFileHeader<<"\n";

        QStringList parameterList;
        int childCount =  pElem->getAllChildrenCount();
        for(int i =0 ; i< childCount; i++)
        {
            GTACommandBase * pCmd = NULL;
            pElem->getCommand(i,pCmd);
            if(pCmd != NULL)
            {
                //QString instanceName = pCmd->getInstanceName();
                QStringList varList = pCmd->getVariableList();
                if(! varList.isEmpty())
                {
                    parameterList.append(varList);
                }
                QList<GTAScxmlLogMessage> logMsgList = pCmd->getLogMessageList();
                QString userLogComment = pCmd->getUserLogComment();
                QString cmdComment = pCmd->getComment();
                QString actionName, complement,actionOnFail,timeOut, timeOutUnit, precision, precisionType;
                if(pCmd->getCommandType() == GTACommandBase::ACTION)
                {
                    GTAActionBase * pAction = dynamic_cast<GTAActionBase*>(pCmd);
                    if(pAction != NULL)
                    {
                        actionName = pAction->getAction();
                        complement = pAction->getComplement();
                        pAction->getTimeOut(timeOut,timeOutUnit);
                        pAction->getPrecision(precision,precisionType);
                        actionOnFail = pAction->getActionOnFail();
                    }
                }
                else if(pCmd->getCommandType() == GTACommandBase::CHECK)
                {
                    GTAActionBase * pCheck = dynamic_cast<GTAActionBase*>(pCmd);
                    if(pCheck != NULL)
                    {
                        actionName = pCheck->getAction();
                        complement = pCheck->getComplement();
                        pCheck->getTimeOut(timeOut,timeOutUnit);
                        pCheck->getPrecision(precision,precisionType);
                        actionOnFail = pCheck->getActionOnFail();
                    }
                }
                if(!precisionType.isEmpty() && precision.isEmpty())
                    precision = precision + precisionType;

                if(!timeOutUnit.isEmpty() && timeOutUnit.contains("ms") && ! timeOut.isEmpty())
                {
                    double val = timeOut.toDouble() * 1000;
                    timeOut = QString::number(val);
                }
                if(! timeOut.isEmpty())
                    timeOut +="s";

                QString statment = pCmd->getStatement();
                statment.replace("\n"," ");

                if(! logMsgList.isEmpty())
                {
                    foreach(GTAScxmlLogMessage objLog, logMsgList)
                    {
                        QString logMessageS = "log";
                        logMessageS += delimS + objLog.TimeStamp;
                        logMessageS += delimS + actionName;
                        logMessageS += delimS + complement;
                        logMessageS += delimS + timeOut;
                        logMessageS += delimS + actionOnFail;
                        logMessageS += delimS + precision;
                        logMessageS += delimS + cmdComment;
                        logMessageS += delimS + statment;
                        logMessageS += delimS + objLog.Identifier;
                        logMessageS += delimS + objLog.ResultTypeToString(objLog.Result);
                        logMessageS += delimS + objLog.StatusTypeToString(objLog.Status);
                        logMessageS += delimS + objLog.LevelOfDetailToString(objLog.LOD);
                        logMessageS += delimS + objLog.ExpectedValue;
                        logMessageS += delimS + objLog.CurrentValue;
                        logMessageS += delimS + objLog.FunctionStatus;
                        logMessageS += delimS + userLogComment;
                        writer<<logMessageS<<"\n";
                    }
                }
                else
                {
                    QString logMessageS = "log";
                    logMessageS += delimS + "";
                    logMessageS += delimS + actionName;
                    logMessageS += delimS + complement;
                    logMessageS += delimS + timeOut;
                    logMessageS += delimS + actionOnFail;
                    logMessageS += delimS + precision;
                    logMessageS += delimS + cmdComment;
                    logMessageS += delimS + statment;
                    logMessageS += delimS + "";
                    logMessageS += delimS + "";
                    logMessageS += delimS + "";
                    logMessageS += delimS + "";
                    logMessageS += delimS + "";
                    logMessageS += delimS + "";
                    logMessageS += delimS + "";
                    logMessageS += delimS + userLogComment;
                    writer<<logMessageS<<"\n";
                }
            }
        }
        if(! iAttachmentList.isEmpty())
        {
            foreach(QString attachment, iAttachmentList)
            {
                QString logAttachmentS = "attachment";
                logAttachmentS += delimS + attachment;
                writer<<logAttachmentS<<"\n";
            }
        }

        parameterList.removeDuplicates();
        if(! parameterList.isEmpty())
        {
            foreach(QString param, parameterList)
            {
                GTAExportScxml::VariableType varType = GTAExportScxml::IsTypeOf(param);
                if(varType == GTAExportScxml::Unknown)
                {
                    QString logParamS = "parameter";
                    logParamS += delimS + param;
                    writer<<logParamS<<"\n";
                }
            }
        }

        objFile.close();
    }
    return rc;
}

void GTAAppController::saveImportElement(GTAElement *pElem, const QString &iDataDir, bool iOverwriteFiles)
{
    QString name = pElem->getName();
    QFutureWatcher<bool> * FutureWatcher = new QFutureWatcher<bool>();
    FutureWatcher->setObjectName(pElem->getDocumentName());
    connect(FutureWatcher,SIGNAL(finished()),this,SLOT(onSaveEditorDocFinished()));
    QFuture<bool> Future = QtConcurrent::run(this,&GTAAppController::saveDocument,name,iOverwriteFiles,pElem,pElem->getHeader(),iDataDir);
    FutureWatcher->setFuture(Future);

}

void GTAAppController::setImportedFilesCount(int filesCount)
{
    _importedFilesCount = filesCount;
    if (_importedFilesCount == _saveImportsCount)
    {
        _importedFilesCount = -1;
        _saveImportsCount = 0;
        emit pluginFilesImported();
    }
}

void GTAAppController::onSaveEditorDocFinished()
{
    QFutureWatcher<bool> *pWatcher = dynamic_cast<QFutureWatcher<bool> *>(sender());
    if(pWatcher == NULL)
        return;
    else
    {
        QString FWobjectName = pWatcher->objectName();

        _saveImportsCount++;
        if (_importedFilesCount == _saveImportsCount)
        {
            _importedFilesCount = -1;
            _saveImportsCount = 0;
            emit pluginFilesImported();
        }
        pWatcher->deleteLater();
    }
}




bool GTAAppController::loadPlugin()
{

    bool rc = false;
    if (nullptr == _pPluginInterface)
    {
        QString appDirPath = QDir::cleanPath(qApp->applicationDirPath());


        QDir plugingDir(appDirPath);
        QStringList nameFilters;
        nameFilters << "GTA*.dll";
        QStringList dllList = plugingDir.entryList(nameFilters, QDir::Files | QDir::NoSymLinks);

        foreach(QString file, dllList)
        {
            QPluginLoader *loader = new QPluginLoader(plugingDir.absoluteFilePath(file));
            QObject *plugin = loader->instance();
            if (plugin)
            {
                _pPluginInterface = dynamic_cast<GTAPluginInterface *>(plugin);
                if (_pPluginInterface != NULL)
                {
                    _plugins.append(loader);
                    rc = true;
                    break;
                }

            }
            else
            {
                delete loader;
                loader = nullptr;
            }

        }
    }

    return rc;

}

/**
  * This function unloads the Plugin (GTAExtPlugin.dll) Unloading the plugin provides freeing of library which is not needed.
  */
bool GTAAppController::unloadPlugin()
{
    bool rc = false;
    if (nullptr != _pPluginInterface)
    {
        foreach(QPluginLoader* pluginLoader, _plugins)
        {
            rc = pluginLoader->unload();
            if (rc)
            {
                delete pluginLoader;
                pluginLoader = nullptr;
                //under the Qt documentation that states that unload will properly destroy the instance.
                _pPluginInterface = nullptr;
            }
        }
        _plugins.clear();
    }
    return rc;
}

QStringList GTAAppController::getParserTypes()
{
    QStringList parserTypes;
    if(nullptr != _pPluginInterface)
    {
        parserTypes = _pPluginInterface->getParserList();
    }
    return parserTypes;
}

PluginOutput GTAAppController::importExternalFile(const QString &iImportType, const QString &iExportPath, const QString &iImportPath, const QString &iErrorPath, const bool &iOverwriteFiles)
{
    PluginOutput output;

    if(nullptr != _pPluginInterface)
    {
        QObject *pObj = dynamic_cast<QObject*>(_pPluginInterface);
        //the line below is written with the understanding that this function can only be called one time per execution
        _importedFilesCount = -1;
        _saveImportsCount = 0;
        if(pObj)
        {
            connect(pObj,SIGNAL(saveImportElement(GTAElement*,QString,bool)),this,SLOT(saveImportElement(GTAElement*,QString,bool)),Qt::UniqueConnection);
        }
        output = _pPluginInterface->importExternalFile(iImportType,iImportPath,iExportPath,iErrorPath,iOverwriteFiles);
    }

    return output;
}

GTAPluginInterface * GTAAppController::getPluginInterface()
{
    return _pPluginInterface;
}

QString GTAAppController::getGTALogDirectory()
{
    if(_pSystemServices!=NULL)
    {
        return _pSystemServices->getLogDirectory();
    }

    return QString();
}

QString GTAAppController::getGTATableDirectory()
{
    if (_pSystemServices != NULL)
    {
        return _pSystemServices->getTableDirectory();
    }

    return QString();
}

QString GTAAppController::getGTADataDirectory()const
{
    if(_pSystemServices!=NULL)
    {
        return _pSystemServices->getDataDirectory();
    }

    return QString();
}

QString GTAAppController::getGTAScxmlDirectory()const
{
    if (_pSystemServices != NULL)
    {
        return _pSystemServices->getScxmlDirectory();
    }

    return QString();
}

QString GTAAppController::getGTALibraryPath()
{
    if(_pSystemServices!=NULL)
    {
        return _pSystemServices->getLibraryPath();
    }

    return QString();
}
QString GTAAppController::getGTADbFilePath()
{
    if(_pSystemServices!=NULL)
    {
        return _pSystemServices->getDatabaseFile();
    }
    return QString();
}

QString GTAAppController::getExportDocDirectory()
{
    if(_pSystemServices!=NULL)
    {
        return _pSystemServices->getExportDocDirectory();
    }

    return QString();
}
ErrorMessageList GTAAppController::CheckCompatibility()
{
    ErrorMessageList errors;
    GTAMdiController *pMdiCtrl = GTAMdiController::getMDIController();
    if(pMdiCtrl)
    {
        GTAEditorViewController *pEditorCtrl = pMdiCtrl->getActiveViewController();
        if(pEditorCtrl)
        {

            GTAElement * pElem = pEditorCtrl->getElement();
            if(pElem != NULL)
            {
                QString sDataDirectory = _pSystemServices->getDataDirectory();
                QString dbFile = _pSystemServices->getDatabaseFile();
                GTACheckCompatibility checkComp;

                errors = checkComp.CheckCompatibility(sDataDirectory, dbFile, pElem, false);
            }
        }
    }
    return errors;
}

//to be modified for mdi
void GTAAppController::getListOfUnReleasedParams(GTAElement*,QStringList&)
{
    //    if(ipElem != NULL)
    //    {
    //        QString sDataDirectory = _pSystemServices->getDataDirectory();
    //        QString dbFile = _pSystemServices->getDatabaseFile();
    //        GTACheckCompatibility checkComp;
    //        checkComp.CheckCompatibility(sDataDirectory,dbFile,ipElem,olstOfParamToRelease,true);
    //        if (olstOfParamToRelease.isEmpty()==false)
    //        {
    //            olstOfParamToRelease.removeDuplicates();
    //        }
    //    }

}

QString GTAAppController::getTempDataDirectory()

{
    if (NULL!=_pSystemServices)
    {
        return _pSystemServices->getTemporaryDirectory();
    }
    return QString();

}
void GTAAppController::cleanTemporaryDirectory()
{
    bool rc = false;
    QString tempdir = QDir::cleanPath(this->getTempDataDirectory());
    QDir tmpDirObj(tempdir);

    QFileInfoList fileInfoList = tmpDirObj.entryInfoList(QDir::Files);
    foreach(QFileInfo absPath, fileInfoList)
    {
        if (absPath.exists())
        {
            QString filePath = absPath.absoluteFilePath();
            rc = tmpDirObj.remove(filePath);

        }
    }


    /*QStringList sdirInfoList = tmpDirObj.entryList(QDir::AllDirs);
   /* QFileInfoList dirInfoList = tmpDirObj.entryInfoList(QDir::AllDirs);
   QStringList sfileInfoList = tmpDirObj.entryList(QDir::Files);
  foreach(QFileInfo absPath, dirInfoList)
   {
       if (absPath.exists())
       {
           QString filePath = absPath.absoluteFilePath();
           rc = tmpDirObj.remove(filePath);
       }
   }*/



}



//bool GTAAppController::isHeaderRowEditable(const int& iRow)
//{
//    bool rC = true;
//    if (_pViewController)
//    {
//        GTAHeader* pHeader = _pViewController->getHeader();
//        if (NULL == pHeader)
//        {
//            GTAElement* pElement =  _pViewController->getElement();
//            if (pElement!=NULL)
//            {
//                pHeader= pElement->getHeader();
//            }

//        }
//        if (NULL!=pHeader)
//        {

//            if (iRow>=pHeader->getFieldNodeSize())
//            {
//                rC= false;
//                _LastError = "ICD parameter cannot be edited";
//            }

//        }
//        else
//        {
//            rC=false;
//            _LastError = "No Header found";
//        }



//    }

//    return rC;
//}


//bool GTAAppController::isHeaderRowDeletable(const int& iRow)
//{
//    bool rC = true;
//    if (_pViewController)
//    {
//        GTAHeader* pHeader = _pViewController->getHeader();
//        if (NULL == pHeader)
//        {
//            GTAElement* pElement =  _pViewController->getElement();
//            if (pElement!=NULL)
//            {
//                pHeader= pElement->getHeader();
//            }

//        }
//        if (NULL!=pHeader)
//        {

//            if (iRow>=pHeader->getFieldNodeSize())
//            {
//                rC= false;
//                _LastError = "ICD parameter cannot be edited/deleted";
//            }
//            else
//            {
//                QString sFieldName = pHeader->getFieldName(iRow);
//                if (pHeader->isNodeMandatory(sFieldName))
//                {
//                    rC = false;
//                    _LastError ="Header field is Mandatory cannot be delted";
//                }
//            }

//        }
//        else
//        {
//            rC=false;
//            _LastError = "No Header found";
//        }



//    }


//    return rC;
//}
//bool GTAAppController::removeHeaderItem(const int& iRow)
//{
//    bool rc = false;
//    if (_pAppController)
//    {
//        rc =_pViewController->removeHeaderField(iRow);
//    }

//    if (!rc)
//        _LastError=_pViewController->getLastError();
//    return rc;

//}
//bool GTAAppController::getHeaderFieldInfo(const int& iRow, QString& iFieldName, QString& iFieldVal, QString& iMandatoryInfo, QString& iFieldDescription)
//{
//    bool rC = true;
//    if (_pViewController)
//    {
//        GTAHeader* pHeader = _pViewController->getHeader();
//        if (NULL == pHeader)
//        {
//            GTAElement* pElement =  _pViewController->getElement();
//            if (pElement!=NULL)
//            {
//                pHeader= pElement->getHeader();
//            }

//        }
//        if (NULL!=pHeader)
//        {

//            if (iRow>=pHeader->getFieldNodeSize())
//            {
//                rC= false;
//                _LastError = "ICD parameter cannot be edited/deleted";
//            }
//            else
//            {
//                iFieldName = pHeader->getFieldName(iRow);
//                iFieldVal = pHeader->getFieldValue(iRow);
//                iFieldDescription = pHeader->getFieldDescription(iRow);
//                iMandatoryInfo="No";
//                if (pHeader->isNodeMandatory(iFieldName))
//                    iMandatoryInfo="Yes";
//            }

//        }
//        else
//        {
//            rC=false;
//            _LastError = "No Header found";
//        }



//    }

//    return rC;
//}
//bool GTAAppController::isEditorRowMovableUpwards(const int& selectedItem)
//{
//    bool rc = false;
//    if (_pViewController)
//    {
//        rc = _pViewController->isEditorRowMovableUpwards(selectedItem);
//    }
//    return rc;
//}
//bool GTAAppController::moveEditorRowItemUp(const int& selectedItem)
//{
//    bool rc = false;
//    if (_pViewController)
//    {
//        rc = _pViewController->moveEditorRowItemUp(selectedItem);
//    }
//    return rc;
//}
//bool GTAAppController::isEditorRowMovableDownwards(const int& selectedItem)
//{
//    bool rc = false;
//    if (_pViewController)
//    {
//        rc = _pViewController->isEditorRowMovableDownwards(selectedItem);
//    }
//    return rc;
//}
//bool GTAAppController::moveEditorRowItemDown(const int& selectedItem)
//{
//    bool rc = false;
//    if (_pViewController)
//    {
//        rc = _pViewController->moveEditorRowItemDown(selectedItem);
//    }
//    return rc;
//}
//bool GTAAppController::searchHighlight(const QRegExp& searchString,QList<int>& iSelectedRows,bool ibSearchUp,bool singleSearch,bool ibTextSearch,QModelIndex &oHighlightedRowIdx)
//{
//    bool rc = false;
//    if (_pViewController)
//    {
//        int lastSelectedRow =-1;
//        if (singleSearch==true)
//        {

//            lastSelectedRow= _pViewController->getLastHighligtedRow();
//            if (ibSearchUp)
//                lastSelectedRow = (lastSelectedRow==0||lastSelectedRow==-1)? _pViewController->getElement()->getAllChildrenCount():lastSelectedRow--;
//            else
//                lastSelectedRow++;

//            if (lastSelectedRow>-1 && iSelectedRows.isEmpty())
//                iSelectedRows.append(lastSelectedRow);
//        }

//        rc = _pViewController->searchHighlight(searchString,iSelectedRows,ibSearchUp,ibTextSearch,oHighlightedRowIdx);
//    }
//    return rc;
//}
//void GTAAppController::resetDisplaycontext()
//{
//    if (_pViewController)
//    {
//        _pViewController->resetDisplayContext();
//    }
//}
//void GTAAppController::setDisplaycontextToCompatibilityCheck()
//{
//    if (_pViewController)
//    {
//        _pViewController->setDisplaycontextToCompatibilityCheck();
//    }
//}
//bool GTAAppController::hightlightReplace(const int& iSelectedRow,const QRegExp& searchString,const QString& sReplace,bool ibSearchUp,QModelIndex &oHighlightedRowIdx)
//{
//    bool rc = false;
//    if (_pViewController)
//    {

//        QList<int> selectedRows;
//        selectedRows.append(iSelectedRow);
//        rc = _pViewController->hightlightReplace(searchString,sReplace,selectedRows,ibSearchUp,oHighlightedRowIdx);
//    }
//    return rc;
//}
//bool GTAAppController::replaceAllString(const QRegExp& searchString,const QString& sReplace)
//{
//    bool rc = false;
//    if (_pViewController)
//    {

//        rc = _pViewController->replaceAllString(searchString,sReplace);
//    }
//    return rc;
//}
//void GTAAppController::highLightLastChild(const QModelIndex& ipIndex)const
//{

//    if (_pViewController)
//    {

//        _pViewController->highLightLastChild(ipIndex);
//    }

//}

QStringList GTAAppController::getAudioRecogItemsToDisplay()
{
    QStringList  audioConfigList = _pSystemServices->getAudioRecogConfigFileList();
    QStringList AudioInfo;

    foreach(QString audioFile, audioConfigList)
    {
        QFile file(audioFile);
        if(!file.open(QIODevice::ReadOnly)) {
            _LastError.append( "Audio File could not be opened.");
        }
        QTextStream in(&file);

        while(!in.atEnd()) {
            QString line = in.readLine();
            if(line.toLower().endsWith(".wav"))
            {
                QStringList items = line.split(".");
                items.removeLast();
                QString data = items.join(".");
                if(!data.isEmpty())
                    AudioInfo.append(data);
            }
        }
    }
    AudioInfo.removeDuplicates();
    return AudioInfo;
}

//DEAD CODE
void GTAAppController::getRowsContainingTitle(QList<int>& oLstOfRows)
{
    if (_pViewController)
    {
        _pViewController->getRowsContainingTitle(oLstOfRows);
    }
}

void GTAAppController::updateCurrentSessionVarList()
{
    QStringList currentSessionList = getVarListForAutoComplete();
    _CurrentSessionVarList.append(currentSessionList);
    _CurrentSessionVarList.removeDuplicates();
}

QStringList GTAAppController::getVariablesForAutoComplete()
{
    QStringList temp;
    temp.append(_CurrentSessionVarList);
    QStringList currentSessionList = getVarListForAutoComplete();
    _CurrentSessionVarList.append(currentSessionList);
    _CurrentSessionVarList.removeDuplicates();
    currentSessionList.append(_HistoryVarList);
    currentSessionList.append(temp);
    currentSessionList.removeDuplicates();
    return currentSessionList;
}

QStringList GTAAppController::getVarListForAutoComplete()
{
    QStringList lstOfVars;
    GTAMdiController *pMdiCtlr = GTAMdiController::getMDIController();
    if(pMdiCtlr)
    {
        GTAEditorViewController *pEditorCtrl = pMdiCtlr->getActiveViewController();
        if(pEditorCtrl)
        {
            GTAElement* pCurElement = pEditorCtrl->getElement();
            if(pCurElement)
            {
                pCurElement->getVariableList(lstOfVars);
            }
        }
    }
    lstOfVars.removeDuplicates();
    return lstOfVars;
}

QStringList GTAAppController::getVariablesForRelease()
{
    QStringList releaseParams;
    GTAMdiController *pMdiCtlr = GTAMdiController::getMDIController();
    if(pMdiCtlr)
    {
        GTAEditorViewController *pEditorCtrl = pMdiCtlr->getActiveViewController();
        if(pEditorCtrl)
        {
            GTAElement * pElem = pEditorCtrl->getElement();
            GTADataController dataCtrlr;
            GTADocumentController doCtrl;
            QStringList chkLstForRecursion;
            QStringList paramList = doCtrl.getAllVariableList(pElem,chkLstForRecursion,true);
            QStringList paramItems;
            QHash<QString,GTAICDParameter> ParamHash;
            foreach(QString paramName,paramList)
            {
                //failsafe to check query doesn't fail
                paramName.replace("'","");
                paramItems.append(" NAME='"+paramName+"' ");
            }
            QString whereClause = paramItems.join(" OR ");
            QString query = QString("SELECT * FROM %1 WHERE PARAMTYPE='VMAC' AND (%2);").arg(PARAMETER_TABLE,whereClause);

            QString dbFile = _pSystemServices->getDatabaseFile();
            dataCtrlr.fillQuery(dbFile,query,releaseParams);
        }
    }
    return releaseParams;
}

QStringList GTAAppController::getFavoriteList(const QString& iFilterStr)
{

    QStringList filteredList;
    foreach(QString favItemName, _FavList)
    {
        if (favItemName.contains(iFilterStr) || favItemName.isEmpty())
            filteredList.append(favItemName);
    }

    return filteredList;
}
bool GTAAppController:: initializeFavList()
{
    //    QString favFile = _pSystemServices->getFavoritesFilePath();
    bool rc = true;
    //    QFileInfo favFileInfo(favFile);
    //    if (favFileInfo.exists())
    //    {

    //        QFile file(favFile);

    //        if(!file.open(QIODevice::ReadOnly)) {
    //            _LastError.append( "\nfav File could not be opened.");
    //            return false;
    //        }
    //        else
    //        {
    //            QTextStream in(&file);

    //            while(!in.atEnd()) {
    //                QString line = in.readLine();
    //                _FavList.append(line);
    //            }

    //            file.close();
    //        }

    //    }
    //    else
    //    {
    //        QFile file(favFile);
    //        if(file.open(QIODevice::WriteOnly)){
    //            _LastError.append( "\nfav File could not be opened.");
    //            return false;
    //        }
    //        else
    //            file.close();
    //    }

    //    if (!_FavList.isEmpty())
    //    {
    //        QString dataDir = _pSystemServices->getDataDirectory();
    //        foreach(QString favFile, _FavList)
    //        {
    //            QFileInfo fileInfo(QString("%1%2%3").arg(dataDir,QDir::separator(),favFile));
    //            if (!fileInfo.exists())
    //            {
    //                _FavList.removeAll(favFile);rc = false;
    //            }
    //        }
    //        if (rc==false)
    //        {
    //            _LastError = "Broken link found in favorites has been removed";
    //        }
    //    }

    //    if (!_pViewController->createFavoriteDataModel(_FavList))
    //    {
    //        _LastError = _pViewController->getLastError();
    //        rc = false;

    //    }
    return rc;
}

void GTAAppController::addToFavorites(const QString&, const QModelIndex &iIndex)
{
    if (_pViewController)
    {

        GTATreeItem *pItem = _pAppController->getTreeNodeFromIndex(iIndex);
        if(pItem)
        {
            pItem->setIsFavourite(true);
        }

    }
}

bool GTAAppController:: removeFromFavorites(const QModelIndex &iIndex, const QString &iFileName)
{
    bool rc = false;
    if (_pViewController)
    {
        GTATreeItem *pFavRootNode = _pViewController->getFavRootTreeNode();
        if(pFavRootNode)
        {
            GTATreeItem *pItem = _pAppController->getTreeNodeFromIndex(iIndex);
            if(pItem && pItem->isFavourite())
            {

                rc = pFavRootNode->removeItem(pItem);

                if(_treeRootNode)
                {
                    GTATreeItem* foundItem = _treeRootNode->findNode(iFileName,GTATreeItem::ABS_REL_FILE_PATH);
                    if(foundItem)
                    {
                        foundItem->setIsFavourite(false);

                    }
                }
            }
        }
    }
    return rc;
}

QAbstractItemModel* GTAAppController::  getElemDataBrowserModel()const
{
    QAbstractItemModel* pElemBrowserModel = NULL;
    if (_pViewController)
        pElemBrowserModel = _pViewController->getElemDataBrowserModel();
    return pElemBrowserModel;

}

QAbstractItemModel* GTAAppController::getElemDataBrowserModel(bool) const
{
    QAbstractItemModel* pElemBrowserModel = NULL;
    if (_pViewController)

        pElemBrowserModel = _pViewController->getTreeBrowserModel();
    return pElemBrowserModel;

}

QAbstractItemModel* GTAAppController::getFavoriteBrowserDataModel(bool isFavItemRemoved)const

{
    QAbstractItemModel* pFavDataModel = NULL;
    if (_pViewController)
    {
        pFavDataModel = _pViewController->getFavoriteBrowserDataModel(isFavItemRemoved);

    }
    return pFavDataModel;

}

bool GTAAppController::createElementDataBrowserModel(const QStringList&)
{
    bool rc = false;

    if (_pViewController)
    {

        //QSqlQueryModel model = getRecordsFromFileReferenceTable();
        //		_pViewController->createElementDataBrowserModel();//(iFilter,QSqlQueryModel());

    }

    return rc;

}
//DEAD CODE
QModelIndex GTAAppController::getElementDataBrowserIndexForDataDir()
{

    if (_pViewController)
    {
        QString dataDirectory = _pSystemServices->getDataDirectory();
        return _pViewController->getElementDataBrowserIndexForDataDir(dataDirectory);
    }

    return QModelIndex();

}
//DEAD CODE
void GTAAppController::setFiltersOnFavorites(const QString& filter)
{
    if (filter.isEmpty())
    {
        _pViewController->createFavoriteDataModel(_FavList);
    }
    else
        _pViewController->setFiltersOnFavorites(filter);

}
//DEAD CODE
void GTAAppController::setFiltersOnElemBrowser(const QStringList& filter, const int &iSearchType)
{
    if (_pViewController)
    {
        _pViewController->setFiltersOnElemBrowser(filter,iSearchType);
    }
}

void  GTAAppController::commitSessionData()
{
    _pSystemServices->commitLastLaunchPath();
    GTADataController db;
    db.commitUserDatabase();
    db.closeUserDatabase();
}

QString GTAAppController::executeSVNAction(SVNInputParam svnInput)
{
    QString svnError, svnFileError;
    QString iSvnCommand = svnInput.command;
    QModelIndex icmdIndex = svnInput.cmdIndex;
    QModelIndexList icmdIndexList = svnInput.cmdIndexList;
    QStringList iSvnUsrCredential = svnInput.userCredential;
    QString resolveOption = svnInput.resolveOption;
    QStringList selectedFiles = svnInput.fileList;
    QString commitMsg = svnInput.commitMessage;

    QString fileName = getFilePathFromTreeModel(icmdIndex);

    if(fileName.isEmpty()
        && (iSvnCommand != "Clear SVN Authentication cache")
        && (iSvnCommand != "Update Directory")
        && (iSvnCommand != "Update All Elements")
        && (iSvnCommand != "Add All Elements")
        && (iSvnCommand != "Commit All Elements")) //TODO: These should be consts defined in header
    {
        svnError = "Failed to fetch selected file";
    }
    else
    {
        _pSVNController = new GTASVNController();

        if (iSvnCommand == "Clear SVN Authentication cache")
            _pSVNController->cleanSVNauthenticationData(getGTADataDirectory(), svnError);

        else if (iSvnCommand == "Update Directory")
            _pSVNController->svnUpdateDir(getGTADataDirectory(), iSvnUsrCredential, svnError);

        else if(iSvnCommand.contains("Update All Elements"))
        {
            if(!selectedFiles.isEmpty())
            {
                foreach(QString file, selectedFiles)
                {
                    _pSVNController->svnUpdate(file,getGTADataDirectory(),iSvnUsrCredential,svnFileError);
                    if(!svnFileError.isEmpty())
                        svnError.append(QString("%1 : %2").arg(file,svnFileError));
                }
            }
        }
        else if (iSvnCommand.contains("Add All Elements"))
        {
            QString gtaDirectory = getGTADataDirectory();
            if (!selectedFiles.isEmpty())
            {
                foreach(QString file, selectedFiles)
                {
                    _pSVNController->svnAdd(file, gtaDirectory, iSvnUsrCredential, svnFileError);
                    if (!svnFileError.isEmpty())
                        svnError.append(QString("%1 : %2").arg(file, svnFileError));
                }
            }
        }
        else if(iSvnCommand.contains("Commit All Elements"))
        {
            QString gtaDirectory = getGTADataDirectory();
            if(!selectedFiles.isEmpty())
            {
                foreach(QString file, selectedFiles)
                {
                    _pSVNController->svnCommit(file,gtaDirectory,iSvnUsrCredential,svnFileError,commitMsg);
                    if(!svnFileError.isEmpty())
                        svnError.append(QString("%1 : %2").arg(file,svnFileError));
                }
            }
        }

        else if (iSvnCommand.contains("Resolve Conflict"))
            _pSVNController->svnResolve(fileName, getGTADataDirectory(), iSvnUsrCredential, resolveOption, svnError);

        else if (iSvnCommand.contains("Check for modification"))
        {
            QString svnTempFilePath;
            GTAElement* pElem;
            QString GTADataDirectory = getGTADataDirectory();

            getElementFromDocument(fileName, pElem, false);
            QString completeFilePath = QDir::cleanPath(QString("%1%2%3").arg(GTADataDirectory, QDir::separator(), fileName));
            QFileInfo info(completeFilePath);
            QString errorCode;
            if (!info.isFile() && info.exists())
            {
                svnError.append("The selected path is not a file\nCan Not check for modifications.");
                return svnError;
            }
            else
            {
                svnTempFilePath = _pSVNController->exportSVNFile(fileName, GTADataDirectory, _pSystemServices->getTemporaryDirectory(), errorCode, iSvnUsrCredential);
            }
            GTAElement* pElemTempFile;
            if (!svnTempFilePath.isEmpty() && errorCode.isEmpty())
            {
                getElementFromDocument(svnTempFilePath, pElemTempFile, false, true, true);
            }
            else
            {
                svnError.append(errorCode);
                return svnError;
            }
            _pSVNController->svnCheckForModification(fileName, pElem, pElemTempFile, getGTADataDirectory(), _pSystemServices->getTemporaryDirectory(), iSvnUsrCredential, svnError);
        }

        else
        {
            if (!icmdIndexList.isEmpty())
            {
                QString gtaDirectory = getGTADataDirectory();
                for (const QString& file : selectedFiles)
                {
                    if (iSvnCommand == "Add")
                        _pSVNController->svnAdd(file, gtaDirectory, iSvnUsrCredential, svnError);

                    else if (iSvnCommand == "Commit")
                        _pSVNController->svnCommit(file, gtaDirectory, iSvnUsrCredential, svnError, commitMsg);

                    else if (iSvnCommand.contains("Delete"))
                        _pSVNController->svnDelete(file, gtaDirectory, iSvnUsrCredential, svnError);

                    else if (iSvnCommand == "Update")
                        _pSVNController->svnUpdate(file, gtaDirectory, iSvnUsrCredential, svnError);

                    else if (iSvnCommand == "Lock")
                        _pSVNController->svnLock(file, gtaDirectory, iSvnUsrCredential, svnError);

                    else if (iSvnCommand == "Unlock")
                        _pSVNController->svnUnLock(file, gtaDirectory, iSvnUsrCredential, svnError);

                    else if (iSvnCommand == "Revert")
                        _pSVNController->svnRevert(file, gtaDirectory, iSvnUsrCredential, svnError);

                    if (!svnError.isEmpty())
                        svnError.append(QString("%1 : %2").arg(file, svnError));
                }
            }
        }
    }
    return svnError;
}

bool GTAAppController::outputSVNStatus(const QString& iPathToCheck, QString& oErrorMsg, QString& output)
{
    QString pOutputStatus, pOutputDiff;
    QStringList commandArgsDiff, commandArgsStatus;

    commandArgsDiff << "diff" << "--summarize" << iPathToCheck;
    if (_pSVNController->executeSVNCommand(commandArgsDiff, pOutputDiff, oErrorMsg))
    {
        commandArgsStatus << "status" << iPathToCheck;
        bool rc = _pSVNController->executeSVNCommand(commandArgsStatus, pOutputStatus, oErrorMsg);

        QStringList items = QString(pOutputStatus + pOutputDiff).split("\n");
        int size = items.size();
        for (int i = 0; i<size; i++)
        {
            QString item = items[i];
            QStringList itemList = item.split(" ", QString::SkipEmptyParts);
            if(!itemList.isEmpty())
            {
                QString path = itemList.size() > 1 ? itemList.at(1) : QString();
                QString stat = itemList.size() > 1 ? itemList.at(0) : QString();
                QDir pathDir(path.replace("\\", "/").remove("\r"));
                if (pathDir.exists() && stat == "?")
                {
                    QFileInfoList fileList = QDir(path).entryInfoList(QStringList() << "*", QDir::Files | QDir::NoDotAndDotDot | QDir::Dirs);
                    for (QFileInfo& file : fileList)
                    {
                        int j = fileList.indexOf(file);
                        items.insert(i + j + 1, QString("%1       %2").arg("?", file.absoluteFilePath().replace("/", "\\")));
                        size = items.size();
                    }
                }
            }
        }
        items.removeDuplicates();
        output = items.join("\n");
        return rc;
    }
    return false;
}

void GTAAppController::checkFileSvnStatus(QStringList& values, QHash<QString, QString>& oItems, bool getUnversioned, bool getOnlyUnversioned)
{
    QString file = values.last();
    QStringList fileItems = file.split("\\");
    QStringList gtaDirLs;
    QStringList fileRelPathLs;
    auto test = getGTADataDirectory();
    QString gtaDir = "";
    for (auto& item : fileItems)
    {
        if (gtaDir != getGTADataDirectory())
        {
            gtaDirLs.append(item);
            gtaDir = gtaDirLs.join("/");
            continue;
        }
        fileRelPathLs.append(item);
    }
    file = fileRelPathLs.join("/");
    file.replace(">", " ");
    QString status = values.first();
    if (!getOnlyUnversioned) {
        if (status == "?")
        {
            if (getUnversioned)
                oItems.insert(file, "Unversioned");
        }
        else if (status == "M")
            oItems.insert(file, "Modified");
        else if (status == "D")
            oItems.insert(file, "Deleted");
        else if (status == "A")
            oItems.insert(file, "Added");
        else if (status == "I")
            oItems.insert(file, "Ignored");
        else if (status == "!")
            oItems.insert(file, "Missing");
        else if (status == "*")
            oItems.insert(file, "New Version Available");
    }
    else {
        if (status == "?")
            oItems.insert(file, "Unversioned");
        else if (status == "I")
            oItems.insert(file, "Ignored");
        else if (status == "*")
            oItems.insert(file, "New Version Available");
    }
}

bool GTAAppController::getAllModifiedUnversionFiles(const QString & iPathToCheck,QHash<QString,QString> &oItems,bool getUnversioned, QStringList usrCredentials, QString &oErrorMsg, bool getOnlyUnversioned)
{
    bool rc = false;
    QString output;
    rc = outputSVNStatus(iPathToCheck, oErrorMsg, output);
    if (rc == true && oErrorMsg.isEmpty() == true)
    {
        output = output.replace(" ", ">");
        QStringList items = output.split("\n");
        foreach(QString item, items)
        {
            if (!item.isEmpty())
            {
                QStringList values = item.trimmed().split(">", QString::SkipEmptyParts);
                if (values.count() >= 2)
                    checkFileSvnStatus(values, oItems, getUnversioned, getOnlyUnversioned);
            }
        }
    }
    return rc;
}

bool GTAAppController::parseOneClickLaunchAppConfig(const QString &iConfigName, GTAOneClickLaunchApplicationTestRig &oTestRig)
{
    bool rc = true;
    QString TestRigName;
    if(_pSystemServices != NULL && !iConfigName.isEmpty())
    {
        QString path = _pSystemServices->getOneClickLaunchAppConfigPath();
        QString LAConfig = QString("%1%2%3").arg(path,QDir::separator(),iConfigName);
        QFileInfo fileInfo(LAConfig);
        rc = fileInfo.exists();
        if(rc)
        {
            QFile xmlFileObj(LAConfig);
            rc = xmlFileObj.open(QFile::Text | QFile::ReadOnly);
            if(rc)
            {
                QDomDocument domDoc;
                domDoc.setContent(&xmlFileObj);
                xmlFileObj.close();
                QDomElement rootElem = domDoc.documentElement();
                if(!rootElem.isNull() && rootElem.nodeName() == "Test_Rig")
                {
                    rc = true;
                    TestRigName = rootElem.attributes().namedItem("Name").nodeValue();
                    oTestRig.setTestRigName(TestRigName);
                    QDomNodeList sAppNodes = rootElem.elementsByTagName("Application");
                    for(int appIdx=0;appIdx<sAppNodes.count();appIdx++)
                    {
                        QDomNode appNode=sAppNodes.at(appIdx);
                        GTAOneClickLaunchApplicationApplications sApp;
                        QString name = appNode.attributes().namedItem("name").nodeValue();
                        QString type = appNode.attributes().namedItem("comm_type").nodeValue();
                        QString toolID = appNode.attributes().namedItem("tool_id").nodeValue();
                        QString toolType = appNode.attributes().namedItem("type").nodeValue();
                        QString killMode = appNode.attributes().namedItem("Kill_mode").nodeValue();
                        QDomNode descriptionNode = appNode.namedItem("description");
                        QString description;
                        if(!descriptionNode.isNull())
                        {
                            QDomNode desNode = descriptionNode.firstChild();
                            if(!desNode.isNull())
                                description = desNode.nodeValue();
                        }
                        sApp.setAppType(type);
                        sApp.setAppName(name);
                        sApp.setAppDescription(description);
                        sApp.setAppToolId(toolID);
                        sApp.setAppToolType(toolType);
                        if(killMode.contains("no_kill"))
                            sApp.setKillMode(false);
                        else if(killMode.contains("kill"))
                            sApp.setKillMode(true);
                        else
                            sApp.setKillMode(false);


                        QDomNodeList sOptionNodes = appNode.toElement().elementsByTagName("Option");
                        for(int optIdx=0;optIdx<sOptionNodes.count();optIdx++)
                        {
                            QDomNode optNode = sOptionNodes.at(optIdx);
                            GTAOneClickLaunchApplicationOptions sOption;
                            QString optName = optNode.attributes().namedItem("name").nodeValue();
                            QString optType = optNode.attributes().namedItem("type").nodeValue();
                            QString optDef = optNode.attributes().namedItem("default").nodeValue();

                            QDomNodeList sValueNodes = optNode.toElement().elementsByTagName("value");
                            for(int valIdx=0;valIdx<sValueNodes.count();valIdx++)
                            {
                                QDomNode valNode = sValueNodes.at(valIdx);
                                GTAOneClickLaunchApplicationValue sValue;
                                QString valName = valNode.attributes().namedItem("name").nodeValue();
                                QDomNodeList sArgNodes = valNode.toElement().elementsByTagName("argument");
                                for(int argIdx=0;argIdx<sArgNodes.count();argIdx++)
                                {
                                    QDomNode argNode = sArgNodes.at(argIdx);
                                    GTAOneClickLaunchApplicationArgs sArg;
                                    QString argValue = argNode.attributes().namedItem("value").nodeValue();
                                    QString argType = argNode.attributes().namedItem("type").nodeValue();
                                    QString argName = argNode.attributes().namedItem("name").nodeValue();
                                    sArg.setArgName(argName);
                                    sArg.setArgType(argType);
                                    sArg.setArgValue(argValue);
                                    sValue.insertArg(sArg);
                                }
                                sValue.setValueName(valName);
                                sOption.insertValue(sValue);
                            }
                            sOption.setOptionDefault(optDef);
                            sOption.setOptionName(optName);
                            sOption.setOptionType(optType);
                            sApp.insertOption(sOption);
                        }
                        oTestRig.insertApplication(sApp);
                    }
                }
                else
                {
                    rc = false;
                }
            }
        }
    }
    return rc;
}

//DEAD CODE
bool GTAAppController::clearTempDirectory()
{
    //QString tempDir = _pSystemServices->getTemporaryDirectory();
    //QDir dir(tempDir);
    //QStringList items = dir.entryInfoList(QDir::Files);
    bool rc =true;
    //foreach(QString item, items)
    //{
    //    QFileInfo fileInfo(item);
    //    if(fileInfo.isFile())
    //    {
    //        QFile tempFile(item);
    //        rc = tempFile.remove(item);
    //        QFileInfo fileInfo1(item);
    //        if(fileInfo1.exists())
    //            rc= false;
    //    }
    //}
    //if(rc)
    //{
    //    rc = dir.rmdir(tempDir,true);
    //}
    return rc;
}

void GTAAppController::setLTRACollapsedState(bool isCollapsed,QAbstractItemModel *ipLogModel,QModelIndex iIndex)
{
    if(ipLogModel != NULL)
    {
        if((iIndex.isValid()))
        {
            GTACommandBase *pCmd = (GTACommandBase *)(iIndex.internalPointer());
            if(pCmd != NULL)
            {
                pCmd->setCollapsed(isCollapsed);
            }
        }
    }
}

void GTAAppController::setLTRACollapsedState(bool isCollapsed,QAbstractItemModel *ipLogModel)
{
    GTAEditorLogModel * pLogModel = dynamic_cast<GTAEditorLogModel*>(ipLogModel);
    if (NULL!=pLogModel)
    {
        GTAElement* pElement = pLogModel->getDataModel();
        if (NULL!=pElement)
        {
            int cnt = pElement->getAllChildrenCount();
            for(int i=0;i<cnt;i++)
            {
                GTACommandBase *pCmd = NULL;
                pElement->getCommand(i,pCmd);
                if(pCmd!=NULL)
                    pCmd->setCollapsed(isCollapsed);
            }
        }
    }
}

void GTAAppController::checkUncheckAllCommands(bool isSelected,QAbstractItemModel *ipLogModel)
{
    GTAEditorLogModel * pLogModel = dynamic_cast<GTAEditorLogModel*>(ipLogModel);
    if (NULL!=pLogModel)
    {
        GTAElement* pElement = pLogModel->getDataModel();
        if (NULL!=pElement)
        {
            int cnt = pElement->getAllChildrenCount();
            for(int i=0;i<cnt;i++)
            {
                GTACommandBase *pCmd = NULL;
                pElement->getCommand(i,pCmd);
                if(pCmd!=NULL)
                {
                    if(isSelected)
                        pCmd->setReportStatus(GTACommandBase::INCLUDE);
                    else
                        pCmd->setReportStatus(GTACommandBase::EXCLUDE);


                }
            }
        }
    }
}

void GTAAppController::SaveAnalyzedResult(QAbstractItemModel *ipLogModel)
{
    GTAEditorLogModel * pLogModel = dynamic_cast<GTAEditorLogModel*>(ipLogModel);
    if (NULL!=pLogModel)
    {
        GTAElement* pElement = pLogModel->getDataModel();
        if (NULL!=pElement)
        {
            QString elemName = pElement->getName();
            
            saveDocument(QString("%1_result").arg(elemName),true,pElement,pElement->getHeader());
        }
    }
}

//bool GTAAppController::saveDocumentFromResult(GTAElement * ipElement)
//{
//    bool rc = false;
//    if(NULL == ipElement->getHeader())
//    {

//        _LastError = "Header is not found";
//        return false;
//    }
//    if(NULL != ipElement)
//    {
//        QString fileExt = getExtensionForElementType(ipElement->getElementType());

//        QString sDataDirectory=_pSystemServices->getDataDirectory();


//        QString completePath = QString("%1/%2.%3").arg(sDataDirectory,iName,fileExt);
//        QFileInfo docFileInfo(completePath);

//        if(docFileInfo.exists())
//        {

//        }

//        rc = docController.saveDocument(ipElement,ipHeader->getName(),completePath,safeToOverwrite);
//        if(! rc)
//        {
//            _LastError = docController.getLastError();
//        }
//        else
//            ipElement->setSaveSatus(GTAElement::SaveOK);
//    }
//    return rc;
//}

QStringList GTAAppController::getPrintTableParameters(const QString &isTableName)
{
    QStringList lst;
    GTAMdiController *pMdiCtrl = GTAMdiController::getMDIController();
    if(pMdiCtrl)
    {
        GTAEditorViewController * pViewController = NULL;
        pViewController = pMdiCtrl->getActiveViewController();
        if (pViewController)
        {
            GTAElement* pElement = pViewController->getElement();
            if (pElement!=NULL)
            {
                lst= pElement->getPrintTableParams(isTableName);

            }
        }
    }
    return lst;
}
void GTAAppController::editPrintTables(const QString& iName, const QStringList& iLst )
{
    GTAMdiController *pMdiCtrl = GTAMdiController::getMDIController();
    if(pMdiCtrl)
    {
        GTAEditorViewController * pViewController = NULL;
        pViewController = pMdiCtrl->getActiveViewController();
        if(pViewController)

        {
            GTAElement* pElement = pViewController->getElement();
            pElement->editPrintTables(iName,iLst);
        }
    }

}
bool GTAAppController::parseKinematicReferenceFile(const QString& iRefFilePath, QStringList &oRefFileData)
{
    bool rc =false;
    if (!iRefFilePath.isEmpty())
    {
        QFile refFile(iRefFilePath);
        if (refFile.exists())
        {
            refFile.open(QFile::Text | QFile::ReadOnly);
            QTextStream in(&refFile);
            while(!in.atEnd())
            {
                QString line = in.readLine();
                if (line.startsWith("{") && line.contains("}"))
                {
                    rc =true;
                    QStringList lines = line.split("}");
                    QString txt = lines.first();
                    oRefFileData.append(txt.remove("{"));
                }
            }
            refFile.close();
        }
    }
    return rc;
}
bool GTAAppController::loadTemplateFile(const QString &iTmplFileName,const GTAActionFTAKinematicMultiOutput *pKinematciCmd,GTAElement *& opElement)
{
    bool rc   = false;
    opElement = NULL;
    rc = getElementFromTemplate(iTmplFileName,opElement);
    if(rc)
    {
        rc = generateProcFromTemplate(pKinematciCmd,opElement);
    }
    return rc;
}

bool GTAAppController::getElementFromTemplate(const QString & iFileName, GTAElement *& opElement)
{
    bool rc = false;
    QString completeFilePath = _pSystemServices->getTemplateFilePath(iFileName);

    GTADocumentController docController;
    GTAHeader* pHdr = NULL;

    rc = getElementFromDocument(iFileName,opElement,false);
    if(opElement != NULL)
        pHdr = opElement->getHeader();

    //rc = docController.openDocument(completeFilePath,opElement,pHdr);
    if (NULL!=pHdr)
    {
        delete pHdr;pHdr=NULL;
    }
    if(! rc)
        _LastError = getLastError();
    return rc;
}

bool GTAAppController::generateProcFromTemplate(const GTAActionFTAKinematicMultiOutput *pKinematciCmd, GTAElement *& opElement)
{
    bool rc = false;
    //TODO MAKE A TEMPLATE COMMAND BASE TO CREATE COMMON VIRTUAL FUNCTIONS USED IN THIS FUNCTION BY ANY TEMPLATE COMMAND.
    QStringList inputParamList = pKinematciCmd->getInputVariableList();
    QStringList outputParamList = pKinematciCmd->getOutputVariableList();
    QMap <QString,QString> iTagValMap(pKinematciCmd->getTagValuePairForTemapltes());
    int inputCntr = 0;
    for(int i=0;i<inputParamList.count();i++)
    {
        QString paramName = inputParamList.at(i);

        if(!paramName.trimmed().isEmpty())
        {
            QString tagName = QString("%1%2").arg("InputParam",QString::number(inputCntr));
            QString tagFlag = QString("%1%2").arg("InputFlag",QString::number(inputCntr));
            iTagValMap.insert(tagName,paramName);
            iTagValMap.insert(tagFlag,"1");
            iTagValMap.insert(QString("%1%2").arg("inIdx",QString::number(inputCntr)),QString::number(inputCntr));
            inputCntr++;
        }
    }
    inputCntr = 0;
    for(int i=0;i<outputParamList.count();i++)
    {
        QString paramName = outputParamList.at(i);

        if(!paramName.trimmed().isEmpty())
        {
            QString tagName = QString("%1%2").arg("OutputParam",QString::number(inputCntr));
            QString tagFlag = QString("%1%2").arg("OutputFlag",QString::number(inputCntr));
            iTagValMap.insert(tagName,paramName);
            iTagValMap.insert(tagFlag,"1");
            iTagValMap.insert(QString("%1%2").arg("outIdx",QString::number(inputCntr)),QString::number(inputCntr));
            inputCntr++;

        }
    }
    QString templateInstanceName = pKinematciCmd->getCurrentName();
    QString tableName = QString("%1_%2").arg(templateInstanceName,"table");


    iTagValMap.insert("NUM_IN",QString::number(inputParamList.count()));
    iTagValMap.insert("NUM_OUT",QString::number(outputParamList.count()));
    iTagValMap.insert("NUMIO",QString::number(outputParamList.count() + inputParamList.count()));
    iTagValMap.insert("table",tableName);


    opElement->replaceTag(iTagValMap);
    opElement->replaceUntagged();
    rc =true;

    //QString headerTemplateFilePath=_pSystemServices->getHeaderTemplateFilePath();

    //GTADocumentController docController;
    //GTAHeader* pHeader=NULL;
    //GTAElement* pElement=NULL;
    //rc = docController.openDocument(headerTemplateFilePath,pElement,pHeader,false);
    //if (rc)
    //{
    //    if (NULL!= opElement  && NULL!= pHeader)
    //        opElement->setHeader(pHeader);
    //}

    //QString itemName = pKinematciCmd->getCurrentName();
    //saveDocument(itemName,true,opElement,pHeader);

    return rc;
}
QString GTAAppController::getLogDirectory()const
{
    if(_pSystemServices)
        return _pSystemServices->getLogDirectory();
    else
        return QString();

}
QString GTAAppController::getLastEmoLogPath()const
{
    if(_pSystemServices)
        return _pSystemServices->getLastEmoLogPath();
    else
        return QString();
}
QString GTAAppController::getLastEmoElementPath()const
{
    if(_pSystemServices)
        return _pSystemServices->getLastEmoElementPath();
    else
        return QString();

}
QStringList GTAAppController::getFailureCommandNames()
{
    QStringList lst;
    GTAMdiController *pMdiCtrl = GTAMdiController::getMDIController();
    if(pMdiCtrl)
    {
        GTAEditorViewController *pEditorCtrl = pMdiCtrl->getActiveViewController();
        if(pEditorCtrl)
        {
            GTAElement* pElem = pEditorCtrl->getElement();
            if (pElem)
                lst=pElem->getFailureCommands();
        }
    }
    return lst;
}
GTACommandBase* GTAAppController::getFailureCommand(const QString& iFailureName)
{

    GTACommandBase* baseCmd = NULL;
    GTAMdiController *pMdiCtrl = GTAMdiController::getMDIController();
    if(pMdiCtrl)
    {
        GTAEditorViewController *pEditorCtrl = pMdiCtrl->getActiveViewController();
        if(pEditorCtrl)
        {
            GTAElement* pElem = pEditorCtrl->getElement();
            if (pElem)
                baseCmd=pElem->getFailureCommand(iFailureName);
        }
    }
    return baseCmd;
}
QStringList GTAAppController::getPrintTableItem(const QString& iPrintTableName)
{
    QStringList lst;

    GTAMdiController *pMdiCtrl = GTAMdiController::getMDIController();
    if(pMdiCtrl)
    {
        GTAEditorViewController *pEditorCtrl = pMdiCtrl->getActiveViewController();
        if(pEditorCtrl)
        {

            //            GTACommandBase* baseCmd = NULL;
            GTAElement* pElem = pEditorCtrl->getElement();
            if (pElem && iPrintTableName.isEmpty()==false)
            {
                lst = pElem->getPrintTableItems(iPrintTableName);
            }

        }
    }
    return lst;

}
QString GTAAppController::getScxmlDirectory()
{
    QString scxmlDir;
    if(_pSystemServices)
        scxmlDir = _pSystemServices->getScxmlDirectory();
    return scxmlDir;
}

//DEAD CODE
void GTAAppController::setHiddenRows( const QStringList& iRows)
{
    if (NULL!=_pViewController)
    {
        _pViewController->setHiddenRows( iRows);
    }
}

//DEAD CODE
QStringList GTAAppController::getHiddenRows()
{
    if (NULL!=_pViewController)
    {
        return _pViewController->getHiddenRows( );
    }
    return QStringList();
}

bool  GTAAppController::getTotalTimeOutForDocumentInMs(const QString& iDocName,double& oTime)
{
    bool rc = false;
    oTime=0.0;
    if (iDocName.isEmpty()==false)
    {
        GTAElement* pElement = NULL;
        rc = getElementFromDocument(iDocName,pElement,false);
        QStringList strLst;
        if (rc && pElement!=NULL && _pViewController!=NULL)
        {
            _pViewController->createExpandedTreeElement(pElement,strLst);
            oTime=pElement->getTotalTimeOutInMs();
            delete pElement;
            pElement=NULL;
        }
    }

    return rc;
}
QString GTAAppController::getCommandProperty(const QString & iCmdName, const QString & iProperty)
{

    if (_pSystemServices)
    {
        QString cmdPropertyXmlFile = _pSystemServices->getCommandPropertyConfigXMLPath();
        QFileInfo cpxFileInfo(QDir::cleanPath(cmdPropertyXmlFile));
        if (cpxFileInfo.exists())
        {
            QFile xmlFileObj(QDir::cleanPath(cmdPropertyXmlFile));
            bool rc = xmlFileObj.open(QFile::Text| QFile::ReadOnly);
            if(rc)
            {
                QDomDocument DomDoc;
                rc = DomDoc.setContent(&xmlFileObj);
                if(rc)
                {
                    QDomElement rootElement = DomDoc.documentElement();
                    QDomNodeList actionNodes = rootElement.elementsByTagName(XNODE_ACTION);
                    for (int ai=0;ai<actionNodes.size();ai++)
                    {
                        QString commandName;
                        QDomElement actionElement = actionNodes.at(ai).toElement();
                        QString sActionName = actionElement.attribute(XATTR_NAME).trimmed();
                        QDomNodeList complementElementLst = actionElement.elementsByTagName(XNODE_COMPLEMENT);
                        if (complementElementLst.isEmpty())
                        {
                            commandName = sActionName;
                            if (commandName.trimmed() == iCmdName)
                            {
                                if(actionElement.hasAttribute(iProperty))
                                    return actionElement.attribute(iProperty,QString());
                            }
                        }

                        else
                        {
                            QString sComplementName ;
                            for (int ci=0;ci<complementElementLst.size();ci++)
                            {
                                QDomElement complementElement =complementElementLst.at(ci).toElement();
                                sComplementName = complementElement.attribute(XATTR_NAME).trimmed();
                                commandName = QString ("%1 %2").arg(sActionName,sComplementName);
                                if (commandName.trimmed() == iCmdName)
                                {
                                    if(complementElement.hasAttribute(iProperty))
                                        return complementElement.attribute(iProperty,QString());
                                }

                            }

                        }
                    }

                    QDomNodeList checkNodes = rootElement.elementsByTagName(XNODE_CHECK);
                    for (int ai=0;ai<checkNodes.size();ai++)
                    {
                        QString commandName;
                        QDomElement checkElement = checkNodes.at(ai).toElement();
                        QString sCheckName = checkElement.attribute(XATTR_NAME);
                        QDomNodeList complementElementLst = checkElement.elementsByTagName(XNODE_COMPLEMENT);
                        if (complementElementLst.isEmpty())
                        {
                            commandName = sCheckName;
                            if (commandName.trimmed() == iCmdName)
                            {
                                if(checkElement.hasAttribute(iProperty))
                                    return checkElement.attribute(iProperty,QString());
                            }
                        }
                    }


                }

            }
        }
        /*else
        {
            bool rc =  generateCommandPropertyFile();
            if(rc)
                return getCommandProperty(iCmdName, iProperty);
        }*/
    }
    return QString();
}
bool GTAAppController::generateCmdPropertyFileIfAbsent()
{
    QString gtaXmlFile = _pSystemServices->getCommandPropertyConfigXMLPath();

    QFile xmlFileObj(QDir::cleanPath(gtaXmlFile));
    bool rc = xmlFileObj.open(QFile::Text| QFile::ReadOnly);
    if(rc)
    {
        QDomDocument DomDoc;
        rc = DomDoc.setContent(&xmlFileObj);

    }

    if (!rc)
    {
        rc=generateCommandPropertyFile();
    }

    return rc;

}

bool GTAAppController::generateCommandPropertyFile()
{


    QString gtaXmlFile = _pSystemServices->getApplicationConfigXmlFilePath();


    QFile xmlFileObj(QDir::cleanPath(gtaXmlFile));
    bool rc = xmlFileObj.open(QFile::Text| QFile::ReadOnly);
    if(rc)
    {
        QDomDocument DomDoc;
        rc = DomDoc.setContent(&xmlFileObj);
        if(rc)
        {
            QString commandPropertyFilePath = _pSystemServices->getCommandPropertyConfigXMLPath();
            QFile FileObj(commandPropertyFilePath);
            rc = FileObj.open(QFile::WriteOnly | QFile::Text);
            if(! rc)
            {
                _LastError = QString("Unable to open file %1").arg(commandPropertyFilePath);
                return rc;
            }


            QDomDocument oDomDoc;
            QDomProcessingInstruction procInstruct = oDomDoc.createProcessingInstruction("xml","version=\"1.0\"");
            oDomDoc.appendChild(procInstruct);

            QDomElement rootElementofGTAXML = DomDoc.documentElement();
            QDomElement rootElementOfCPXML = oDomDoc.createElement(XNODE_COMMANDS);

            QStringList appendedNodesLst;
            QDomNodeList actionNodes = rootElementofGTAXML.elementsByTagName(XNODE_ACTION);
            //keep a default SET VMAC. Requirement by Olivier
            QDomElement VmacActionElement = oDomDoc.createElement(XNODE_ACTION);
            VmacActionElement.setAttribute(XATTR_NAME,"set VMAC");
            VmacActionElement.setAttribute(XATTR_INVOKE_DELAY,SX_INVOKE_DEFAULT_DELAY);
            VmacActionElement.setAttribute(XATTR_INVOKE_DELAY_UNIT,SX_INVOKE_DEFAULT_DELAY_UNIT);
            rootElementOfCPXML.appendChild(VmacActionElement);

            for (int ai=0;ai<actionNodes.size();ai++)
            {
                QDomElement actionElement = actionNodes.at(ai).toElement();
                QString sActionName = actionElement.attribute(XATTR_NAME);
                if (sActionName.isEmpty() == false && appendedNodesLst.contains(sActionName) == false)
                {


                    appendedNodesLst.append(sActionName);
                    QDomElement newActionElement = oDomDoc.createElement(XNODE_ACTION);
                    newActionElement.setAttribute(XATTR_NAME,sActionName);
                    rootElementOfCPXML.appendChild(newActionElement);
                    //setting a default property, for time being its invoke delay
                    QDomNodeList complements = actionElement.childNodes();
                    if (complements.isEmpty())
                    {
                        newActionElement.setAttribute(XATTR_INVOKE_DELAY,SX_INVOKE_DEFAULT_DELAY);
                        newActionElement.setAttribute(XATTR_INVOKE_DELAY_UNIT,SX_INVOKE_DEFAULT_DELAY_UNIT);
                    }
                    else
                    {
                        for (int i=0;i<complements.size();i++)
                        {
                            QString complementName = complements.at(i).toElement().attribute(XATTR_NAME);
                            if(complementName.isEmpty()==false)
                            {
                                QDomElement newComplement = oDomDoc.createElement(XNODE_COMPLEMENT);
                                newComplement.setAttribute(XATTR_NAME,complementName);
                                newComplement.setAttribute(XATTR_INVOKE_DELAY,SX_INVOKE_DEFAULT_DELAY);
                                newComplement.setAttribute(XATTR_INVOKE_DELAY_UNIT,SX_INVOKE_DEFAULT_DELAY_UNIT);
                                newActionElement.appendChild(newComplement);

                            }

                        }
                    }


                }
            }
            QDomNodeList checkNodes = rootElementofGTAXML.elementsByTagName(XNODE_CHECK);
            for (int ci=0;ci<checkNodes.size();ci++)
            {
                QDomElement checkElement = checkNodes.at(ci).toElement();
                QString sCheckName = checkElement.attribute(XATTR_NAME);
                if (sCheckName.isEmpty() == false && appendedNodesLst.contains(sCheckName) == false)
                {
                    appendedNodesLst.append(sCheckName);
                    QDomElement newCheckElement = oDomDoc.createElement(XNODE_CHECK);
                    newCheckElement.setAttribute(XATTR_NAME,sCheckName);
                    //setting a default property, for time being its invoke delay
                    newCheckElement.setAttribute(XATTR_INVOKE_DELAY,SX_INVOKE_DEFAULT_DELAY);
                    newCheckElement.setAttribute(XATTR_INVOKE_DELAY_UNIT,SX_INVOKE_DEFAULT_DELAY_UNIT);
                    //adding default attributes to CHECK commands. This ensures the Dom Element is added to the root
                    rootElementOfCPXML.appendChild(newCheckElement);
                }
            }


            oDomDoc.appendChild(rootElementOfCPXML);
            QString contents = oDomDoc.toString();
            QTextStream stream(&FileObj);
            stream <<contents;

            rc = FileObj.flush();
            if(! rc)
                _LastError = "Unable to write the file in database, please check you have enough permission and database has sufficient size";

            FileObj.close();

        }
    }

    return rc;

}

void GTAAppController::updateReportStatus(GTAElement *pElem)
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
            GTAActionExpandedCall *pcall = dynamic_cast<GTAActionExpandedCall*>(pParentCmd);

            if(pcall != NULL)
            {
                GTACommandBase::reportSelectionType selType;
                if(pcall->getGlobalResultStatus().contains("KO"))
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
            GTAActionExpandedCall *pcall = dynamic_cast<GTAActionExpandedCall*>(pParentCmd);
            if(pcall != NULL)
            {
                if(pcall->getGlobalResultStatus().contains("KO"))
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


QString GTAAppController::getToolConfigurationPath()
{
    if(_pSystemServices)
    {
        return QDir::cleanPath(_pSystemServices->getToolConfigurationPath());
    }

	return QString();
}

QHash<QString,QString> GTAAppController::getAllExternalToolNames()
{
    QStringList errList;
    QHash<QString,QString> genTools = _pGenericToolController->getAllToolConfigrations(errList);
    return genTools;
}

QList<GTAGenericDB> GTAAppController::getUserDefinedDb()
{
    return _pGenericToolController->getUserDefinedDb();
}

QList<GTAGenericFunction> GTAAppController::getAllExternalToolFunctionNames(const QString &iToolName)
{

    return _pGenericToolController->getGenericToolFunctions(iToolName);
}

QList<GTAGenericParamData> GTAAppController::getGenericToolDefinitions(const QString &iToolName)
{
    return _pGenericToolController->getGenericToolDefintions(iToolName);
}

QList<GTAGenericFunction> GTAAppController::getInputFileFunctions(const QString &iInputFilePath)
{
    return _pGenericToolController->getInputFileFunctions(iInputFilePath);
}

bool GTAAppController::getActionOnFailInfoForGenericTool()
{
    if(_pGenericToolController)
    {
        return _pGenericToolController->hasActionOnfail();
    }
    else
        return false;
}

bool GTAAppController::getTimeOutInfoForGenericTool()
{
    if(_pGenericToolController)
    {
        return _pGenericToolController->hasTimeOut();
    }
    else
        return false;
}

bool GTAAppController::getDumpListInfoForGenericTool()
{
    if(_pGenericToolController)
    {
        return _pGenericToolController->hasDumpList();
    }
    else
        return false;
}


bool GTAAppController::getExternalCommentInfoForGenericTool()
{
    if(_pGenericToolController)
    {
        return _pGenericToolController->hasComment();
    }
    else
        return false;
}

void GTAAppController::getCommandList(QList<CommandInternalsReturnInfo> &oRetInfo,const QString &iCmdName)
{
    parseInternalParamInfoXml();

    GTAMdiController *pMdiCtrl = GTAMdiController::getMDIController();
    if(pMdiCtrl)
    {

        GTAEditorViewController *pEditorCtrl = pMdiCtrl->getActiveViewController();
        if(pEditorCtrl)
        {
            GTAElement *pElem = pEditorCtrl->getElement();
            QHash<QString, GTACommandBase *> cmdList;

            getCommandListFromElem(pElem,cmdList,QString(),iCmdName,false);

            QList<QString> keys = cmdList.keys();
            for(int i=0;i<keys.count();i++)
            {
                QString location = keys.at(i);
                QString uuid;
                QString refLocation;
                if(location.contains("$"))
                {
                    QStringList items = location.split("$");
                    refLocation = items.at(1);
                    uuid = items.at(0);
                }


                GTACommandBase *pBaseCmd = cmdList[location];
                GTAActionBase *pActCmd = dynamic_cast<GTAActionBase *>(pBaseCmd);
                GTACheckBase *pChkCmd = dynamic_cast<GTACheckBase *>(pBaseCmd);
                if(pActCmd!= NULL)
                {
                    GTAGenericToolCommand *pGenCmd = dynamic_cast<GTAGenericToolCommand *>(pActCmd);
                    if(pGenCmd != NULL)
                    {
                        QString toolName = pGenCmd->getToolName();
                        QList<GTAGenericFunction> funcs = pGenCmd->getCommand();
                        for(int j=0;j<funcs.count();j++)
                        {
                            GTAGenericFunction func = funcs.at(j);
                            QString funcName = func.getFunctionName();
                            QString retParamName = func.getReturnParamName();
                            QString retParamType = func.getReturnParamType();
                            if(retParamType == "struct")
                            {
                                getFuncStructList(retParamName,oRetInfo,location,funcName,toolName,retParamName, pGenCmd);
                            }
                            else
                            {
                                CommandInternalsReturnInfo retInfo;
                                retInfo.funcName = funcName.simplified().remove(' ');
                                retInfo.location = uuid;
                                retInfo.refLoc= refLocation;
                                retInfo.retName = QString("%1").arg(retParamName);
                                retInfo.retType = retParamType;
                                retInfo.toolName = toolName;
                                retInfo.cmdName = QString("%1 %2").arg(pGenCmd->getAction(),pGenCmd->getComplement()).trimmed().replace(" ","_");
                                retInfo.pCmd = pGenCmd;
                                oRetInfo.append(retInfo);
                            }
                        }
                    }
                    else
                    {
                        CommandInternalParameterInfo info;
                        info.commandName = pActCmd->getAction();
                        info.complement = pActCmd->getComplement();
                        int idx = _InternalParamInfoLst.indexOf(info);

                        if(idx>=0)
                        {
                            info = _InternalParamInfoLst.at(idx);

                            //the action command has Setlist command containing multiple sets
                            //Internal Parameter of each set is different. As of now, only Setlist has an UUID, Set Command doesn't.
                            GTAActionSetList *pSetlst = dynamic_cast<GTAActionSetList *>(pActCmd);
                            if (pSetlst!=NULL)
                            {
                                QList<GTAActionSet* > pSetList;
                                pSetlst->getSetCommandlist(pSetList);
                                CommandInternalsReturnInfo retInfo;
                                foreach(InternalParameterInfo item, info.internalParamInfoLst)
                                {
                                    foreach (GTAActionSet *pSet,pSetList)
                                    {
                                        retInfo.retName = item.paramName;
                                        retInfo.retType = item.paramType;

                                        retInfo.cmdName = iCmdName;
                                        retInfo.location = pSet->getObjId();
                                        retInfo.refLoc= refLocation;
                                        retInfo.pCmd = pSet;
                                        //to be used as identifier in SCXML generation
                                        retInfo.funcName = iCmdName.simplified().remove(' ');
                                        retInfo.desc = pSet->getStatement();
                                        oRetInfo.append(retInfo);
                                    }
                                }

                            }
                            else
                            {
                                CommandInternalsReturnInfo retInfo;
                                retInfo.cmdName = iCmdName;
                                retInfo.location = uuid;
                                retInfo.refLoc= refLocation;
                                retInfo.pCmd = pActCmd;
                                //to be used as identifier in SCXML generation
                                retInfo.funcName = iCmdName.simplified().remove(' ');
                                foreach(InternalParameterInfo item, info.internalParamInfoLst)
                                {
                                    retInfo.retName = item.paramName;
                                    retInfo.retType = item.paramType;
                                    if(item.paramType == "list")
                                    {
                                        QList<QString> cols= pActCmd->getHeaderColumns();
                                        //(story #358311)
                                        GTAActionForEach *pForEach = dynamic_cast<GTAActionForEach*>(pActCmd);
                                        if(pForEach != NULL)
                                        {
                                            QString repoPath = QString::fromStdString(TestConfig::getInstance()->getRepositoryPath());
                                            if(cols.isEmpty() && !(pForEach->getPath().contains(repoPath) && !(pForEach->getRelativePath().isEmpty())))
                                            {
                                                QString fullpath = repoPath + pForEach->getRelativePath();
                                                pForEach->setPath(fullpath);
                                                pForEach->setColumnMap();
                                                cols = pForEach->getHeaderColumns();
                                            }

                                        }

                                        for(int j=0;j<cols.count();j++)
                                        {
                                            retInfo.retName = QString("%1.%2").arg(item.paramName,cols.at(j));
                                            retInfo.funcName = cols.at(j);
                                            oRetInfo.append(retInfo);
                                        }
                                    }
                                    else
                                        oRetInfo.append(retInfo);
                                }
                            }
                        }
                    }
                }
                else if (pChkCmd != NULL)
                {
                    CommandInternalParameterInfo info;
                    info.commandName = "check";
                    info.complement = pChkCmd->getCheckName();
                    int idx = _InternalParamInfoLst.indexOf(info);
                    if(idx>=0)
                    {
                        info = _InternalParamInfoLst.at(idx);
                        CommandInternalsReturnInfo retInfo;
                        retInfo.cmdName = iCmdName;
                        retInfo.location = uuid;
                        retInfo.refLoc= refLocation;
                        retInfo.pCmd = pChkCmd;
                        //to be used as identifier
                        retInfo.funcName = iCmdName.simplified().remove(' ');
                        foreach(InternalParameterInfo item, info.internalParamInfoLst)
                        {
                            retInfo.retName = item.paramName;
                            retInfo.retType = item.paramType;
                            oRetInfo.append(retInfo);
                        }
                    }
                }
                else
                {
                    //invalid command
                }
            }
        }
    }
}


void GTAAppController::getFuncStructList(QString &iRetName, QList<CommandInternalsReturnInfo> &oRetInfo,const QString &iLocation, const QString &iFuncName,const QString &iToolName, const QString &iRetStr,GTAGenericToolCommand *pGenCmd)
{

    if(pGenCmd)
    {
        QList<GTAGenericAttribute> attrs;
        GTAGenericParamData data;
        QList<GTAGenericParamData> defs = pGenCmd->getDefinitions();
        bool found = false;
        for(int i=0;i<defs.count();i++)
        {
            data = defs.at(i);
            if(data.getParamDataName() == iRetName)
            {
                found = true;
                break;

            }
        }

        if(found)
        {
            attrs = data.getParamAttributeList();


            for(int k=0;k<attrs.count();k++)
            {
                CommandInternalsReturnInfo retInfo;
                GTAGenericAttribute attr = attrs.at(k);
                QString retName = attr.getAttributeName();
                QString retType = attr.getAttributeType();
                QString desc = attr.getAttributeDescription();
                if(retType == "struct")
                {
                    QString retStr = QString("%1.%2").arg(iRetStr,retName);
                    getFuncStructList(retName,oRetInfo,iLocation,iFuncName,iToolName,retStr,pGenCmd);
                }
                else
                {


                    QString uuid;
                    QString refLocation;
                    if(iLocation.contains("$"))
                    {
                        QStringList items = iLocation.split("$");
                        refLocation = items.at(1);
                        uuid = items.at(0);
                    }
                    retInfo.funcName = iFuncName;
                    retInfo.location = uuid;
                    retInfo.refLoc = refLocation;
                    retInfo.retName = QString("%1.%2").arg(iRetStr,retName);
                    retInfo.retType = retType;
                    retInfo.toolName = iToolName;
                    retInfo.desc = desc;
                    retInfo.cmdName  = QString("%1 %2").arg(pGenCmd->getAction(),pGenCmd->getComplement()).trimmed().replace(" ","_");
                    retInfo.pCmd = pGenCmd;
                    oRetInfo.append(retInfo);
                }
            }
        }
    }
}

QList<GTACommandBase*> GTAAppController::getParentList(GTAElement* &ipElem,int idx)
{
    QList<GTACommandBase*> pParentlst;
    std::optional<GTACommandBase*> cmdParent;

    //obtaining parent of the current selected row. this limits the search only in the parent command scope
    GTAMdiController *pMdiCtrl = GTAMdiController::getMDIController();
    GTAEditorViewController * pViewController = NULL;
    pViewController = pMdiCtrl->getActiveViewController();
    int index = 0;
    if (idx>0) {
        index = idx;
        //filling parent for last selected row in pCmdBase
        cmdParent = ipElem->getParentForIndex(index);
    }
    else {
        if(pViewController) {
            GTAElement *loadedElem = pViewController->getElement();
            if (ipElem != NULL) {
                if ((loadedElem != NULL) && (loadedElem->getName() == ipElem->getName())) {
                    index = pViewController->getSelectedRowInEditor();
                    cmdParent = loadedElem->getParentForIndex(index);
                }
                else {
                    cmdParent = ipElem->getParentForIndex(index);
                }
            }
        }
    }

    //pushing all parents for the last selected row into pParentlst
    if (cmdParent) {
        pParentlst.append(*cmdParent);
        //replace with get index and get parent for index
        GTAElement* loadedElem = pViewController->getElement();
        auto parentOfParent = loadedElem->getParentOfCommand(*cmdParent);
        while(parentOfParent) {
            pParentlst.append(*parentOfParent);
            parentOfParent = loadedElem->getParentOfCommand(*parentOfParent);
        }
    }

    return pParentlst;

}


void GTAAppController::getCommandListFromElem(const GTAElement *ipElem, QHash<QString, GTACommandBase *> &oCmdList, const QString &iElemName, const QString &iCmdName,bool isCallCommand)
{
    if(ipElem != NULL)
    {
        GTAElement *pElem = (GTAElement*)ipElem;

        QString elemName = pElem->getDocumentName();
        QString cmdPath = elemName;

        GTAElement * pElement = (GTAElement*)pElem;
        pElement->setName(pElem->getName());

        GTAElement* pTitleBasedElement = NULL;
        pElement->createActionElementGroupedByTitle(pTitleBasedElement);
        pTitleBasedElement->setName(pElem->getName());
        pTitleBasedElement->updateCommandInstanceName(iElemName);

        int cnt = pTitleBasedElement->getAllChildrenCount();

        QList<GTACommandBase*> parentList = getParentList(pTitleBasedElement);
        QList<QString> UUIDLst;
        foreach (GTACommandBase* parent, parentList)
        {
            UUIDLst.append(parent->getObjId());
        }

        int linecount = 0;

        for(int i=0;i<cnt;i++)
        {
            if(!iElemName.isEmpty())
            {
                cmdPath = QString("%1/%2[%3]").arg(iElemName,elemName,QString::number(linecount));
            }
            else
            {
                cmdPath  = QString("%1[%2]").arg(elemName,QString::number(linecount));
            }
            GTACommandBase *pCmd = NULL;
            pTitleBasedElement->getCommand(i,pCmd);

            if(pCmd != NULL)
            {
                GTAActionBase *pActCmd = dynamic_cast<GTAActionBase *>(pCmd);
                GTACheckBase *pChkCmd = dynamic_cast<GTACheckBase *>(pCmd);
                QString fullCmdName = "";

                if(pActCmd != NULL)
                {
                    QString complement = pActCmd->getComplement();
                    bool actionsCond = (complement == COM_CONDITION_DO_WHILE || complement == COM_CONDITION_WHILE || complement == COM_CONDITION_FOR_EACH || complement == COM_CONDITION_IF);
                    //condition to skip while, if, for and do while on a call command loop
                    if (isCallCommand && actionsCond)
                        continue;
                    //condition to show loops only in scope of the highlighted area
                    if (UUIDLst.contains(pActCmd->getObjId()) || !actionsCond || complement == COM_CONDITION_IF)
                        fullCmdName = QString("%1 %2").arg(pActCmd->getAction(),pActCmd->getComplement()).trimmed().replace(" ","_");
                    if(pActCmd->getAction() == ACT_CALL)
                    {
                        GTAActionCall *pCall = dynamic_cast<GTAActionCall *>(pActCmd);
                        GTAElement *pCallElem = NULL;
                        getElementFromDocument(pCall->getRefrenceFileUUID(),pCallElem,true);
                        if(pCallElem != NULL)
                        {
                            getCommandListFromElem(pCallElem,oCmdList,cmdPath,iCmdName,true);
                        }
                    }
                }
                else if(pChkCmd!= NULL)
                {
                    fullCmdName = QString("%1 %2").arg("check",pChkCmd->getCheckName()).trimmed().replace(" ","_");
                }
                else
                {
                    //do nothing
                }

                if (fullCmdName != "title_end")
                    linecount++;

                if((fullCmdName == iCmdName) || iCmdName.trimmed().isEmpty())
                {
                    QString uuid = pCmd->getObjId();
                    oCmdList.insert(QString("%1$%2").arg(uuid,cmdPath),pCmd);
                }
                else
                {

                }
            }
        }
    }
}

/**
  * This function groups an element according to relevant titles
  * @pElem: Input Element which is used to create a grouped element
  * @oVariableList: VariableList of all variables used in the element
  * @oGroupElem: grouped element created from input
  * @iIsParentIgnored: flag to identify if parent (call command) is ignored
  * @iIsValidationGroupingNeeded: flag to identify if the call is coming from validation to do the grouping accordingly
  */
void GTAAppController::GroupElementByTitleComplete(const GTAElement *pElem,
                                                      QStringList &oVariableList,
                                                      GTAElement *oGroupedElem,
                                                      QStringList &icheckforRep,
                                                      bool iIsParentIgnored,
                                                      bool iIsValidationGroupingNeeded)
{
    int cmdCnt = pElem->getDirectChildrenCount();
    // int GloblPosition = 0;
    GTAActionTitle *pTitle = new GTAActionTitle(ACT_TITLE);
    QString uuid = pElem->getUuid();


    GTAElement::ElemType elemType = pElem->getElementType();
    QString titleDesc = "StartOfTestProcedure";
    if(elemType == GTAElement::SEQUENCE)
        titleDesc = "Dummy title : Sequence Start";
    else if(elemType == GTAElement::FUNCTION)
        titleDesc = "Dummy title : Function Start";
    else if(elemType == GTAElement::OBJECT)
        titleDesc = "Dummy title : Object Start";
    else if(elemType == GTAElement::PROCEDURE)
        titleDesc = "Dummy title : Procedure Start";
    else
        titleDesc = "StartOfTestProcedure";
    pTitle->setTitle(titleDesc);


    GTAActionParentTitle *pTitleCmd = new GTAActionParentTitle(pTitle,"");
    int titleChildCnt = 0;

    //DEV comment:memory leak plug
    if (pTitle != NULL)
    {
        delete pTitle;
        pTitle = NULL;
    }
    //

    for(int i=0;i<cmdCnt;i++)
    {
        GTACommandBase *pCmd1 = NULL;
        pElem->getDirectChildren(i,pCmd1);
        if(pCmd1 != NULL)
        {

            if(pCmd1->isIgnoredInSCXML())
            {
                continue;
            }
            GTACommandBase *pCmd = pCmd1->getClone();
            if(pCmd == NULL)
            {
                continue;
            }


            oVariableList.append(pCmd->getVariableList());
            if(pCmd->isTitle())
            {
                if(pTitleCmd != NULL)
                {
                    //pTitleCmd->insertChildren(new GTAActionParentTitleEnd,pTitleCmd->getChildrens().size());
                    oGroupedElem->appendCommand(pTitleCmd);
                }
                GTAActionTitle *pActTitleCmd = dynamic_cast<GTAActionTitle*>(pCmd);
                pTitleCmd = new GTAActionParentTitle(pActTitleCmd,"");
                titleChildCnt = 0;
                pTitleCmd->setInstanceName(pCmd->getInstanceName());

                //DEV comment:memory leak plug
                if (pActTitleCmd != NULL)
                {
                    delete pActTitleCmd;
                    pActTitleCmd = NULL;
                }
                //
            }

            else if(pTitleCmd && pCmd->hasReference())
            {
                GTAActionCall *pCallCmd = dynamic_cast<GTAActionCall *>(pCmd);
                pCallCmd->updateTagValue();
                if(pCallCmd != NULL)
                {
                    if(iIsParentIgnored)
                    {
                        pCallCmd->setIgnoreInSCXML(true);
                    }
                    GTAActionExpandedCall *pExtCall = new GTAActionExpandedCall(*pCallCmd);
                    bool ignoredInSCXML = pCallCmd->isIgnoredInSCXML();
                    QString callElemName = pCallCmd->getElement();
                    QString functionNameWithArg = pCallCmd->getFunctionWithArgument();
                    QString complement = pCallCmd->getComplement();
                    pExtCall->setTagValue(pCallCmd->getTagValue());

                    //Load the call document in the element;
                    GTAElement* pElemOnCall=NULL;
                    bool rc = getElementFromDocument(pCallCmd->getRefrenceFileUUID(),pElemOnCall,true);

                    if(rc && pElemOnCall != NULL )
                    {
                        /*
                          grouping of a call element only if it was not called previously.
                          The grouping is limited to one recursive call
                          This is done to ensure there is no infinite recursion
                        */
                        //check if a parent is called from a child. mark it as recursion if present.
                        /*
                         fun1:
                         call fun2
                            call fun3
                                call fun4
                                    ...
                                        call funN
                         under no circumstances can a nested function call its parent. Not allowed.
                         This becomes a recursion and causes an infinite loop.
                        */
                        if (!icheckforRep.contains(uuid))
                        {
                            icheckforRep.append(uuid);
                            pElemOnCall->replaceTag(pCallCmd->getTagValue());
                            pElemOnCall->updateCommandInstanceName(pCallCmd->getInstanceName());

                            GTAElement *groupedCallElem = new GTAElement(GTAElement::CUSTOM);
                            GroupElementByTitleComplete(pElemOnCall,oVariableList,groupedCallElem,icheckforRep,ignoredInSCXML,iIsValidationGroupingNeeded);
                            //remove the current element from the list to allow another call
                            icheckforRep.removeOne(uuid);
                            if(groupedCallElem != NULL)
                            {

                                groupedCallElem->replaceTag(pCallCmd->getTagValue());
                                //groupedCallElem->updateCommandInstanceName(pCallCmd->getInstanceName());
                                int size =groupedCallElem->getDirectChildrenCount();
                                int callChildCntr = 0;
                                for (int j=0;j<size;j++)
                                {
                                    GTACommandBase* pCurCmd = NULL;
                                    groupedCallElem->getDirectChildren(j,pCurCmd);

                                    if (pCurCmd != NULL)
                                    {
                                        pExtCall->insertChildren(pCurCmd->getClone(),callChildCntr++);
                                    }
                                }
                                //adding instance name to the Call command for Result Analysis
                                pExtCall->setInstanceName(pCallCmd->getInstanceName());
                                //DEV comment:memory leak plug
                                delete groupedCallElem;
                                groupedCallElem = NULL;
                                //

                                //handle rest of the functions differently if a call monitor is present
                                if (complement == COM_CALL_MON)
                                {
                                    //put the rest of the commands into a dummy element and send it for further grouping.
                                    GTAElement *dummyElement = new GTAElement(GTAElement::CUSTOM);
                                    for (int k=i+1;k<cmdCnt;k++)
                                    {
                                        GTACommandBase* pNextCmd = NULL;
                                        pElem->getDirectChildren(k,pNextCmd);
                                        if (NULL != pNextCmd)
                                            dummyElement->appendCommand(pNextCmd->getClone());
                                    }
                                    GTAElement *groupedCallElem = new GTAElement(GTAElement::CUSTOM);
                                    GroupElementByTitleComplete(dummyElement,oVariableList,groupedCallElem,icheckforRep,iIsParentIgnored,false);

                                    //add the grouped element children under one title or as children of call monitor itself
                                    //device a new child list for monitor itself. Here you can call all commands that run parallel to the monitor
                                    GTAActionTitle *pDummyTitle = new GTAActionTitle(ACT_TITLE);
                                    pDummyTitle->setTitle("ParallelProcedure");
                                    GTAActionParentTitle *pParentTitleCmd = new GTAActionParentTitle(pDummyTitle,"");

                                    //DEV comment:memory leak plug
                                    if (pDummyTitle != NULL)
                                    {
                                        delete pDummyTitle;
                                        pDummyTitle = NULL;
                                    }
                                    //
                                    int parentChildCount = 0;
                                    for (int ii=0;ii<groupedCallElem->getDirectChildrenCount();ii++)
                                    {
                                        GTACommandBase *pCmd12 = NULL;
                                        groupedCallElem->getDirectChildren(ii,pCmd12);
                                        if(pCmd12 != NULL)
                                        {
                                            pParentTitleCmd->insertChildren(pCmd12->getClone(),parentChildCount++);
                                        }
                                    }
                                    if (pParentTitleCmd->hasChildren())
                                    {
                                        GTAActionTitle *pFaltuTitle = new GTAActionTitle(ACT_TITLE);
                                        pFaltuTitle->setTitle("ParallelProcedure1");
                                        GTAActionParentTitle *pParentFaltuTitleCmd = new GTAActionParentTitle(pFaltuTitle,"");

                                        //DEV comment:memory leak plug
                                        if (pFaltuTitle != NULL)
                                        {
                                            delete pFaltuTitle;
                                            pFaltuTitle = NULL;
                                        }
                                        //

                                        int parallelChildCount = 0;
                                        for (int forchildren=0;forchildren<pExtCall->getChildren().count();forchildren++)
                                        {
                                            GTACommandBase* pCommand = pExtCall->getChildren().at(forchildren);
//                                            pParentFaltuTitleCmd->insertChildren()
                                            if (NULL != pCommand)
                                            {
                                                pParentFaltuTitleCmd->insertChildren(pCommand->getClone(),parallelChildCount++);
                                            }
                                        }
                                        pExtCall->clearChildrenList();
                                        pExtCall->insertChildren(pParentFaltuTitleCmd,0);
                                        pExtCall->insertChildren(pParentTitleCmd,1);

//                                        pExtCall->insertChildren(pParentTitleCmd,callChildCntr++);
                                    }

                                    if(pTitleCmd != NULL)
                                    {
                                        pTitleCmd->insertChildren(pExtCall,titleChildCnt++);
                                    }
                                    break;
                                }

                                if(pTitleCmd != NULL)
                                {
                                    pTitleCmd->insertChildren(pExtCall,titleChildCnt++);
                                }
                            }
                        }
                        else
                        {
                            //the blank expanded call command is added to the title to show the call command in the title
                            //it is just placed to tell the user that the
                            if(pTitleCmd != NULL)
                            {
                                pExtCall->clearChildrenList();
                                pTitleCmd->insertChildren(pExtCall,titleChildCnt++);
                            }
                        }

                    }
                    else
                    {
                        pExtCall->setBrokenLink(true);
                    }
                    //DEV comment:memory leak plug
                    delete pCallCmd;
                    pCallCmd = NULL;
                    if (pElemOnCall != NULL)
                    {
                        delete pElemOnCall;
                        pElemOnCall = NULL;
                    }
                    //
                }

            }
            else if(pTitleCmd)// && pCmd->canBeCopied())
            {
                GTAActionCallProcedures *pCallParallel = dynamic_cast<GTAActionCallProcedures *>(pCmd);

                if(iIsParentIgnored)
                {
                    pCmd->setIgnoreInSCXML(true);
                }
                if(pCmd->canHaveChildren())
                {
                    GTAActionParentTitle *childrenTitle = NULL;//new GTAActionParentTitle(NULL,"");
                    createCommandGroupedByTitle(pCmd,childrenTitle,oVariableList,icheckforRep,iIsParentIgnored,iIsValidationGroupingNeeded);
                    if(childrenTitle != NULL)
                    {
                        QList<GTACommandBase *> childChildren = childrenTitle->getChildren();
                        if(childChildren.count()> 0)
                        {

                            pCmd->clearChildrenList();
                            //valCount is used only to group call elements if validation grouping is needed
                            int valCount = 0;
                            for(int j=0;j<childChildren.count();j++)
                            {
                                GTACommandBase *pchildTitleCmd =childChildren.at(j);
                                if(pchildTitleCmd != NULL)
                                {
                                    if (iIsValidationGroupingNeeded)
                                    {
                                        if (pchildTitleCmd->hasReference())
                                            pCmd->insertChildren(pchildTitleCmd->getClone(),valCount++);
                                    }
                                    else
                                        pCmd->insertChildren(pchildTitleCmd->getClone(),j);
                                }
                            }
                        }
                        //DEV comment:memory leak plug
                        delete childrenTitle;
                        childrenTitle = NULL;
                        //
                    }
                    pTitleCmd->insertChildren(pCmd,titleChildCnt++);
                }
                else if (pCallParallel != NULL)
                {
                    GTACommandBase *pCallProcCmd = NULL;
                    GTAActionParentTitle *childrenGroupedTitle = NULL;
                    pCallProcCmd = new GTAActionCallProcedures(*pCallParallel);
                    pCallProcCmd->clearChildrenList();

                    //removing children because all parallel calls are saved in a list of GTACallProcItem
                    //no harm done in removing children as it was never intended to have one
                    //first we insert all parallel calls into it as call commands, then we simply semd it for grouping like a condition command
                    pCallParallel->clearChildrenList();
                    for (int lst = 0; lst<pCallParallel->getCallProcedures().count();lst++)
                    {
                        GTACallProcItem proc = pCallParallel->getCallProcedures().at(lst);
                        QString uuid = proc._UUID;
                        GTAActionCall *pCall = new GTAActionCall();
                        pCall->setElement(proc._elementName);

                        pCall->setAction(ACT_CALL);
                        pCall->setComplement(proc._type == GTACallProcItem::PROCEDURE? COM_CALL_PROC : COM_CALL_FUNC);
                        pCall->setObjId();
                        pCall->setRefrenceFileUUID(uuid);
                        pCall->setTagValue(proc._tagValue);
                        pCall->setIsParallelInSCXML(true);
                        pCall->setInstanceName(pCallParallel->getInstanceName());
                        pCallProcCmd->insertChildren(pCall,lst);
                    }
                    createCommandGroupedByTitle(pCallProcCmd,childrenGroupedTitle,oVariableList,icheckforRep,iIsParentIgnored,iIsValidationGroupingNeeded);

                    //add children of childrenGroupedTitle to the pCallParallel
                    pCallProcCmd->clearChildrenList();
                    if(childrenGroupedTitle != NULL)
                    {
                        QList<GTACommandBase *> childChildren = childrenGroupedTitle->getChildren();
                        if(childChildren.count()> 0)
                        {
                            for(int j=0;j<childChildren.count();j++)
                            {
                                GTACommandBase *pchildTitleCmd =childChildren.at(j);
                                if(pchildTitleCmd != NULL)
                                {
                                    pCallParallel->insertChildren(pchildTitleCmd,j);
                                }
                            }
                        }
                    }

                    pCallProcCmd->clearChildrenList();
                    childrenGroupedTitle->clearChildrenList();

                    if (pCallProcCmd != NULL)
                    {
                        delete  pCallProcCmd;
                        pCallProcCmd = NULL;
                    }
                    if (childrenGroupedTitle != NULL)
                    {
                        delete  childrenGroupedTitle;
                        childrenGroupedTitle = NULL;
                    }


                    if (pCallParallel->getChildren().count() > 0)
                        pTitleCmd->insertChildren(pCallParallel,titleChildCnt++);
                }
                else
                {
                    if (!iIsValidationGroupingNeeded)
                        pTitleCmd->insertChildren(pCmd,titleChildCnt++);
                }

            }

        }
    }
    if(pTitleCmd != NULL)
    {
        //pTitleCmd->insertChildren(new GTAActionParentTitleEnd,pTitleCmd->getChildrens().size());
        oGroupedElem->appendCommand(pTitleCmd);
    }
}
void GTAAppController::createCommandGroupedByTitle(GTACommandBase *&ipCmd,
                                                      GTAActionParentTitle *&ipTitleCmd,
                                                      QStringList &oVariableList,
                                                      QStringList &icheckforRep,
                                                      bool iIsParentIgnored,
                                                      bool iIsValidationGroupingNeeded)
{
    int GlobalTitleCnt = 0;//ipTitleCmd->getChildrens().size();
    if(ipCmd != NULL)
    {
        QList<GTACommandBase*> children = ipCmd->getChildren();
        GTAActionTitle *pTitle = new GTAActionTitle(ACT_TITLE);
        pTitle->setTitle("StartOfTestProcedure");
        GTAActionParentTitle *pTitleCmd = new GTAActionParentTitle(pTitle,"");
        int titleChildCnt = 0;

        //DEV comment:memory leak plug
        if (pTitle != NULL)
        {
            delete pTitle;
            pTitle = NULL;
        }
        //

        for(int i=0;i<children.count();i++)
        {
            GTACommandBase * pCmd1 = children.at(i);
            if(pCmd1 != NULL)
            {
                GTACommandBase *pCmd = pCmd1->getClone();
                if(pCmd == NULL)
                    continue;
                oVariableList.append(pCmd->getVariableList());
                if(pCmd->isTitle())
                {
                    if(pTitleCmd != NULL)
                    {
                        if(ipTitleCmd != NULL)
                        {
                            GlobalTitleCnt = ipTitleCmd->getChildren().size();
                            //pTitleCmd->insertChildren(new GTAActionParentTitleEnd,pTitleCmd->getChildrens().size());
                            ipTitleCmd->insertChildren(pTitleCmd,GlobalTitleCnt++);
                        }
                        else
                        {
                            ipTitleCmd = new GTAActionParentTitle(*pTitleCmd);
                        }

                    }
                    GTAActionTitle *pActTitleCmd = dynamic_cast<GTAActionTitle*>(pCmd);
                    pTitleCmd = new GTAActionParentTitle(pActTitleCmd,"");
                    titleChildCnt = 0;

                    //DEV comment:memory leak plug
                    if (pActTitleCmd != NULL)
                    {
                        delete pActTitleCmd;
                        pActTitleCmd = NULL;
                    }
                    //

                }
                else if(pTitleCmd && pCmd->hasReference())
                {
                    GTAActionCall *pCallCmd = dynamic_cast<GTAActionCall *>(pCmd);
                    if(pCallCmd != NULL)
                    {
                        if(iIsParentIgnored)
                        {
                            pCallCmd->setIgnoreInSCXML(true);
                        }

                        GTAActionExpandedCall *pExtCall = new GTAActionExpandedCall(*pCallCmd);
                        QString callElemName = pCallCmd->getElement();
                        bool ignoredInSCXML = pCallCmd->isIgnoredInSCXML();
                        QString functionNameWithArg = pCallCmd->getFunctionWithArgument();
                        pExtCall->setTagValue(pCallCmd->getTagValue());

                        //Load the call document in the element;
                        GTAElement* pElemOnCall=NULL;
                        bool rc = getElementFromDocument(pCallCmd->getRefrenceFileUUID(),pElemOnCall,true);

                        if(rc && pElemOnCall != NULL )
                        {
                            pElemOnCall->replaceTag(pCallCmd->getTagValue());
                            pElemOnCall->updateCommandInstanceName(pCallCmd->getInstanceName());
                            GTAElement *groupedCallElem = new GTAElement(GTAElement::CUSTOM);
                            GroupElementByTitleComplete(pElemOnCall,oVariableList,groupedCallElem,icheckforRep,ignoredInSCXML,iIsValidationGroupingNeeded);
                            if(groupedCallElem != NULL)
                            {
                                groupedCallElem->replaceTag(pCallCmd->getTagValue());
                                //groupedCallElem->updateCommandInstanceName(pCallCmd->getInstanceName());
                                int size =groupedCallElem->getDirectChildrenCount();
                                int callChildCntr = 0;
                                for (int j=0;j<size;j++)
                                {
                                    GTACommandBase* pCurCmd = NULL;
                                    groupedCallElem->getDirectChildren(j,pCurCmd);

                                    if (pCurCmd != NULL)
                                    {
                                        pExtCall->insertChildren(pCurCmd->getClone(),callChildCntr++);
                                    }
                                }

                                //DEV comment:memory leak plug
                                delete groupedCallElem;
                                groupedCallElem = NULL;
                                //

                                //handle rest of the functions differently if a call monitor is present
                                if (pCallCmd->getComplement() == COM_CALL_MON)
                                {
                                    //put the rest of the commands into a dummy element and send it for further grouping.
                                    GTAElement *dummyElement = new GTAElement(GTAElement::CUSTOM);
                                    for (int k=i+1;k<children.count();k++)
                                    {
                                        GTACommandBase* pNextCmd = NULL;
                                        pNextCmd = children.at(k);
                                        if (NULL != pNextCmd)
                                            dummyElement->appendCommand(pNextCmd->getClone());
                                    }
                                    GTAElement *groupedCallElem = new GTAElement(GTAElement::CUSTOM);
                                    GroupElementByTitleComplete(dummyElement,oVariableList,groupedCallElem,icheckforRep,iIsParentIgnored,false);

                                    //add the grouped element children under one title or as children of call monitor itself
                                    //device a new child list for monitor itself. Here you can call all commands that run parallel to the monitor
                                    GTAActionTitle *pDummyTitle = new GTAActionTitle(ACT_TITLE);
                                    pDummyTitle->setTitle("ParallelProcedure");
                                    GTAActionParentTitle *pParentTitleCmd = new GTAActionParentTitle(pDummyTitle,"");

                                    //DEV comment:memory leak plug
                                    if (pDummyTitle != NULL)
                                    {
                                        delete pDummyTitle;
                                        pDummyTitle = NULL;
                                    }
                                    //
                                    int parentChildCount = 0;
                                    for (int ii=0;ii<groupedCallElem->getDirectChildrenCount();ii++)
                                    {
                                        GTACommandBase *pCmd12 = NULL;
                                        groupedCallElem->getDirectChildren(ii,pCmd12);
                                        if(pCmd12 != NULL)
                                        {
                                            pParentTitleCmd->insertChildren(pCmd12->getClone(),parentChildCount++);
                                        }
                                    }
                                    if (pParentTitleCmd->hasChildren())
                                    {
                                        GTAActionTitle *pFaltuTitle = new GTAActionTitle(ACT_TITLE);
                                        pFaltuTitle->setTitle("ParallelProcedure1");
                                        GTAActionParentTitle *pParentFaltuTitleCmd = new GTAActionParentTitle(pFaltuTitle,"");

                                        //DEV comment:memory leak plug
                                        if (pFaltuTitle != NULL)
                                        {
                                            delete pFaltuTitle;
                                            pFaltuTitle = NULL;
                                        }
                                        //

                                        int parallelChildCount = 0;
                                        for (int forchildren=0;forchildren<pExtCall->getChildren().count();forchildren++)
                                        {
                                            GTACommandBase* pCommand = pExtCall->getChildren().at(forchildren);
                                            if (NULL != pCommand)
                                            {
                                                pParentFaltuTitleCmd->insertChildren(pCommand->getClone(),parallelChildCount++);
                                            }
                                        }
                                        pExtCall->clearChildrenList();
                                        pExtCall->insertChildren(pParentFaltuTitleCmd,0);
                                        pExtCall->insertChildren(pParentTitleCmd,1);

                                    }

                                    if(pTitleCmd != NULL)
                                    {
                                        pTitleCmd->insertChildren(pExtCall,titleChildCnt++);
                                    }
                                    break;
                                }

                                if(pTitleCmd != NULL)
                                {
                                    pTitleCmd->insertChildren(pExtCall,titleChildCnt++);
                                }
                            }
                        }
                        else
                        {
                            pExtCall->setBrokenLink(true);
                        }
                        //DEV comment:memory leak plug
                        //delete pCallCmd;
                        //pCallCmd = NULL;
                        if (pElemOnCall != NULL)
                        {
                            delete pElemOnCall;
                            pElemOnCall = NULL;
                        }
                        //
                    }
                }
                else// if(pCmd->canBeCopied())
                {
                    GTAActionCallProcedures *pCallParallel = dynamic_cast<GTAActionCallProcedures *>(pCmd);
                    if(pTitleCmd)
                    {
                        if(iIsParentIgnored)
                        {
                            pCmd->setIgnoreInSCXML(true);
                        }

                        if(pCmd->canHaveChildren())
                        {
                            //GTAActionTitle *pTitle = new GTAActionTitle(ACT_TITLE);
                            //pTitle->setTitle("");
                            GTAActionParentTitle *childrenTitle = NULL;//new GTAActionParentTitle(NULL,"");
                            createCommandGroupedByTitle(pCmd,childrenTitle,oVariableList,icheckforRep,iIsParentIgnored,iIsValidationGroupingNeeded);
                            if(childrenTitle != NULL)
                            {
                                QList<GTACommandBase *> childChildren = childrenTitle->getChildren();
                                if(childChildren.count()> 0)
                                {

                                    pCmd->clearChildrenList();
                                    int valCount = 0;
                                    for(int j=0;j<childChildren.count();j++)
                                    {
                                        GTACommandBase *pchildTitleCmd =childChildren.at(j);
                                        if(pchildTitleCmd != NULL)
                                        {
                                            if (iIsValidationGroupingNeeded)
                                                if (pchildTitleCmd->hasReference())
                                                    pCmd->insertChildren(pchildTitleCmd->getClone(),valCount++);
                                            pCmd->insertChildren(pchildTitleCmd->getClone(),j);
                                        }
                                    }
                                }
                                //DEV comment:memory leak plug
                                delete childrenTitle;
                                childrenTitle = NULL;
                                //
                            }
                            pTitleCmd->insertChildren(pCmd,titleChildCnt++);
                        }
                        else if (pCallParallel != NULL)
                        {
                            GTACommandBase *pCallProcCmd = NULL;
                            GTAActionParentTitle *childrenGroupedTitle = NULL;
                            pCallProcCmd = new GTAActionCallProcedures(*pCallParallel);
                            pCallProcCmd->clearChildrenList();

                            //removing children because all parallel calls are saved in a list of GTACallProcItem
                            //no harm done in removing children as it was never intended to have one
                            //first we insert all parallel calls into it as call commands, then we simply semd it for grouping like a condition command
                            pCallParallel->clearChildrenList();
                            for (int lst = 0; lst<pCallParallel->getCallProcedures().count();lst++)
                            {
                                GTACallProcItem proc = pCallParallel->getCallProcedures().at(lst);
                                QString uuid = proc._UUID;
                                GTAActionCall *pCall = new GTAActionCall();
                                if (proc._type == GTACallProcItem::PROCEDURE)
                                    pCall->setElement(proc._elementName+".pro");
                                else
                                    pCall->setElement(proc._elementName+".fun");
                                pCall->setAction(ACT_CALL);
                                pCall->setComplement(proc._type == GTACallProcItem::PROCEDURE? COM_CALL_PROC : COM_CALL_FUNC);
                                pCall->setObjId();
                                pCall->setRefrenceFileUUID(uuid);
                                pCall->setTagValue(proc._tagValue);
                                pCall->setIsParallelInSCXML(true);
                                pCall->setInstanceName(pCallParallel->getInstanceName());
                                pCallProcCmd->insertChildren(pCall,lst);
                            }
                            createCommandGroupedByTitle(pCallProcCmd,childrenGroupedTitle,oVariableList,icheckforRep,iIsParentIgnored,iIsValidationGroupingNeeded);

                            //add children of childrenGroupedTitle to the pCallParallel
                            pCallProcCmd->clearChildrenList();
                            if(childrenGroupedTitle != NULL)
                            {
                                QList<GTACommandBase *> childChildren = childrenGroupedTitle->getChildren();
                                if(childChildren.count()> 0)
                                {
                                    for(int j=0;j<childChildren.count();j++)
                                    {
                                        GTACommandBase *pchildTitleCmd =childChildren.at(j);
                                        if(pchildTitleCmd != NULL)
                                        {
                                            pCallParallel->insertChildren(pchildTitleCmd,j);
                                        }
                                    }
                                }
                            }

                            pCallProcCmd->clearChildrenList();
                            childrenGroupedTitle->clearChildrenList();

                            if (pCallProcCmd != NULL)
                            {
                                delete  pCallProcCmd;
                                pCallProcCmd = NULL;
                            }
                            if (childrenGroupedTitle != NULL)
                            {
                                delete  childrenGroupedTitle;
                                childrenGroupedTitle = NULL;
                            }


                            //check if there are valid children to call in parallel
                            if (pCallParallel->getChildren().count() > 0)
                                pTitleCmd->insertChildren(pCallParallel,titleChildCnt++);
                        }
                        else
                        {
                            if (!iIsValidationGroupingNeeded)
                                pTitleCmd->insertChildren(pCmd,titleChildCnt++);
                        }
                    }
                    else
                    {
                        if(iIsParentIgnored)
                        {
                            pCmd->setIgnoreInSCXML(true);
                        }
                        if(pCmd->canHaveChildren())
                        {
                            //GTAActionTitle *pTitle = new GTAActionTitle(ACT_TITLE);
                            //pTitle->setTitle("");
                            GTAActionParentTitle *childrenTitle = new GTAActionParentTitle(NULL,"");
                            createCommandGroupedByTitle(pCmd,childrenTitle,oVariableList,icheckforRep,iIsParentIgnored,iIsValidationGroupingNeeded);
                            //childrenTitle->insertChildren(new GTAActionParentTitleEnd,childrenTitle->getChildrens().size());
                            if(childrenTitle != NULL)
                            {
                                QList<GTACommandBase *> childChildren = childrenTitle->getChildren();
                                if(childChildren.count()> 0)
                                {

                                    pCmd->clearChildrenList();
                                    int valCount = 0;
                                    for(int j=0;j<childChildren.count();j++)
                                    {
                                        GTACommandBase *pchildTitleCmd =childChildren.at(j);
                                        if(pchildTitleCmd != NULL)
                                        {
                                            if (iIsValidationGroupingNeeded)
                                                if (pchildTitleCmd->hasReference())
                                                    pCmd->insertChildren(pchildTitleCmd->getClone(),valCount++);
                                            pCmd->insertChildren(pchildTitleCmd->getClone(),j);
                                        }
                                    }
                                }
                                //DEV comment:memory leak plug
                                delete childrenTitle;
                                childrenTitle = NULL;
                                //
                            }
                        }
                        if (!iIsValidationGroupingNeeded)
                            ipTitleCmd->insertChildren(pCmd,ipTitleCmd->getChildren().size());
                    }
                }
                //delete pCmd1;
                //pCmd1 = NULL;
            }

        }
        if(pTitleCmd != NULL)
        {
            if(ipTitleCmd != NULL)
            {
                GlobalTitleCnt = ipTitleCmd->getChildren().size();
                //pTitleCmd->insertChildren(new GTAActionParentTitleEnd,pTitleCmd->getChildrens().size());
                ipTitleCmd->insertChildren(pTitleCmd,GlobalTitleCnt++);
            }
            else
            {
                ipTitleCmd = new GTAActionParentTitle(*pTitleCmd);

            }
            //pTitleCmd->insertChildren(new GTAActionParentTitleEnd,pTitleCmd->getChildrens().size());
        }
    }

}


//===================================
//DEV: Update monitor function or remove the comment when monitor is implemented
bool GTAAppController::updateGroupingForMonitor(const GTAElement *pElem,
                                                   QStringList &oVariableList,
                                                   GTAElement *,
                                                   QStringList &icheckforRep,
                                                   bool iIsParentIgnored)
{
    int cmdCnt = pElem->getDirectChildrenCount();
    // int GloblPosition = 0;
    GTAActionTitle *pTitle = new GTAActionTitle(ACT_TITLE);
    QString uuid = pElem->getUuid();


    GTAElement::ElemType elemType = pElem->getElementType();
    QString titleDesc = "StartOfTestProcedure";
    if(elemType == GTAElement::SEQUENCE)
        titleDesc = "Dummy title : Sequence Start";
    else if(elemType == GTAElement::FUNCTION)
        titleDesc = "Dummy title : Function Start";
    else if(elemType == GTAElement::OBJECT)
        titleDesc = "Dummy title : Object Start";
    else if(elemType == GTAElement::PROCEDURE)
        titleDesc = "Dummy title : Procedure Start";
    else
        titleDesc = "StartOfTestProcedure";
    pTitle->setTitle(titleDesc);


    // GTAActionParentTitle *pTitleCmd = new GTAActionParentTitle(pTitle,"");
    // int titleChildCnt = 0;

    //DEV comment:memory leak plug
    if (pTitle != NULL)
    {
        delete pTitle;
        pTitle = NULL;
    }
    //

    for(int i=0;i<cmdCnt;i++)
    {
        GTACommandBase *pCmd1 = NULL;
        pElem->getDirectChildren(i,pCmd1);
        if(pCmd1 != NULL)
        {

            if(pCmd1->isIgnoredInSCXML())
            {
                continue;
            }
            GTACommandBase *pCmd = pCmd1->getClone();
            if(pCmd == NULL)
            {
                continue;
            }
            if (pCmd->hasReference())
            {
                GTAActionCall *pCallCmd = dynamic_cast<GTAActionCall *>(pCmd);
                if(pCallCmd != NULL)
                {
                    GTAActionExpandedCall *pExtCall = new GTAActionExpandedCall(*pCallCmd);
                    QString callElemName = pCallCmd->getElement();
                    // bool ignoredInSCXML = pCallCmd->isIgnoredInSCXML();
                    QString functionNameWithArg = pCallCmd->getFunctionWithArgument();
                    pExtCall->setTagValue(pCallCmd->getTagValue());
                    QString complement = pExtCall->getComplement();
                    if (complement == COM_CALL_MON)
                    {
                        //put the rest of the commands into a dummy element and send it for further grouping.
                        GTAElement *dummyElement = new GTAElement(pElem->getElementType());
                        for (int k=i+1;k<cmdCnt;k++)
                        {
                            GTACommandBase* pNextCmd = NULL;
                            pElem->getDirectChildren(k,pNextCmd);
                            dummyElement->appendCommand(pNextCmd->getClone());
                        }
                        GTAElement *groupedCallElem = new GTAElement(GTAElement::CUSTOM);
                        GroupElementByTitleComplete(dummyElement,oVariableList,groupedCallElem,icheckforRep,iIsParentIgnored,false);

                        //add the grouped element children under one title or as children of call monitor itself
                        //device a new child list for monitor itself. Here you can call all commands that run parallel to the monitor

                    }
                }
            }
        }
    }
    return false;
}

bool GTAAppController::updateHistoryTable(const QStringList &iUpdateList)
{
    bool rc = false;
    if(_pSystemServices)
    {
        QString dbFilePath = _pSystemServices->getDatabaseFile();
        GTADataController DataCtrl(dbFilePath);
        rc = DataCtrl.updateHistoryTable(iUpdateList);
    }
    return rc;
}


GTATreeItem* GTAAppController::getRootTreeNode()const
{
    return _treeRootNode;
}

bool GTAAppController::updateSvnStatusNestedSetOfTreeItems(const QString& directory, const GTATreeItem* parent, const QSet<QString>& changedFiles)
{
    QList< GTATreeItem*> childrenList = parent->getChildren();
    bool rc = false;
    foreach(GTATreeItem * child, childrenList)
    {
        if (child->getExtension() == "folder")
            updateSvnStatusNestedSetOfTreeItems(directory + "/" + child->getName(), child, changedFiles);

        QString absoluteFilePath = QString("%1/%2").arg(directory, child->getName());
        if (changedFiles.contains(absoluteFilePath))
        {
            rc = updateSvnStatusOfTheFile(absoluteFilePath, child);
            QString absRelPath = child->getAbsoluteRelPath();
            _DataTreeMap.remove(absRelPath);
            _DataTreeMap.insert(absRelPath, child);
            _dbDataController->deleteFile(absRelPath);
            _dbDataController->addFile(child);
        }
    }
    return rc;
};

bool GTAAppController::updateSvnStatusSetOfTreeItems(const QSet<QString>& svnChangedFiles)
{
    QString iDataDirectoryPath =  _pSystemServices->getDataDirectory();
    GTATreeItem* pParent = _treeRootNode;
    _dbDataController->getAllChildren(_treeRootNode);
    return updateSvnStatusNestedSetOfTreeItems(iDataDirectoryPath, pParent, svnChangedFiles);
}

/**
* @brief Updates the content of the tree view.
* @param rebuild set to true resets the tree.
* @return bool true if then storage went fine.
*/
bool GTAAppController::updateTreeStructure(bool rebuild, bool isInit)
{
    bool rc = false;

    QString dataDirectoryPath = _pSystemServices->getDataDirectory();
    if (!rebuild)
    {
        if (_treeRootNode == NULL)
        {
            QString root = "root";
            _treeRootNode = new GTATreeItem(root, root, root, root, root);
        }
        if (!_DataTreeMap.isEmpty())
        {
            _DataTreeMap.clear();
        }
        if (!isInit)
            updateTreeStructureDB(_treeRootNode, dataDirectoryPath);
    }
    else
    {
        if (_treeRootNode != NULL)
        {
            delete _treeRootNode;
            _treeRootNode = NULL;
        }
        QString root = "root";
        _treeRootNode = new GTATreeItem(root, root, root, root, root);
        _DataTreeMap.clear();
        _dbDataController->setIsNew(true);
        _dbDataController->deleteDb(dataDirectoryPath);
    }

    _dbDataController->openDb(dataDirectoryPath);
    _FileWatcherList.clear();

    _DataTreeMap.insert(dataDirectoryPath, _treeRootNode);
    rc = storeTreeStructure(dataDirectoryPath, _treeRootNode);
	_dbDataController->setIsNew(false);

    return rc;
}

void GTAAppController::checkDataTreeMapConsistency()
{
    QStringList keys = _DataTreeMap.keys();
    QStringList foldersToRemove;
    for(int i = 0; i < keys.count(); i++)
    {
        QString path = keys.at(i);
        QDir dir(QDir::cleanPath(path));
        if(!dir.exists())
        {
            GTATreeItem *pParent = _DataTreeMap.value(path);
            if(_treeRootNode != NULL)
            {
                if(pParent->getExtension() == "folder")
                {
                    foldersToRemove.append(path);
                }
                _treeRootNode->removeItem(pParent);

            }

        }
    }
    for(int i = 0; i < foldersToRemove.count(); i++)
    {
        QString path = foldersToRemove.at(i);
        _DataTreeMap.remove(path);
    }
}

void GTAAppController::checkTreeNodeConsistency(const QString &iDirPathChange)
{
    GTATreeItem *pParent = NULL;
    QString dataDirPath;
    QDir dir(iDirPathChange);
    if(dir.exists())
    {
        QStringList filters;
        filters << "*.seq" << "*.pro" << "*.fun" << "*.obj";
        dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);
        dir.setSorting(QDir::Name);
        dir.setNameFilters(filters);
        QFileInfoList fileLst = dir.entryInfoList();
        QSet<QString> L1;
        QSet<QString> temp;
        QSet<QString> L2;
        QSet<QString> L3;
        QSet<QString> L4;
        QSet<QString> L5;
        for(int i = 0; i < fileLst.count(); i++ )
        {
            QFileInfo file = fileLst.at(i);
            QString absFilePath = file.absoluteFilePath();
            L1.insert(QDir::cleanPath(absFilePath));
        }

        if(_DataTreeMap.contains(iDirPathChange))
        {
            pParent = _DataTreeMap.value(iDirPathChange);
            QList<GTATreeItem *> children = pParent->getChildren();
            dataDirPath = getGTADataDirectory();

            for(int i = 0; i < children.count(); i++)
            {
                GTATreeItem *pChild = children.at(i);
                if(pChild && (pChild->getExtension() != "folder"))
                {
                    QString name = pChild->getName();
                    QString absRelPath = pChild->getAbsoluteRelPath();
                    QString fullPath = QDir::cleanPath(QString("%1%2%3").arg(dataDirPath,QDir::separator(),absRelPath));
                    L2.insert(fullPath);
                }
            }
        }

        temp = L2;
        L3 = L2.subtract(L1);
        L4 = L1.subtract(temp);
        L5 = L3.unite(L4);

        QStringList FinalList =  L5.toList();
        for(int i = 0; i < FinalList.count(); i++)
        {
            QString path = FinalList.at(i);
            QFileInfo fileInfo(path);
            if(!fileInfo.exists())
            {
                if(pParent != NULL)
                {
                    QString absRel = path.replace(dataDirPath,"",Qt::CaseInsensitive);
                    GTATreeItem *fileNode = pParent->findNode(absRel,GTATreeItem::ABS_REL_FILE_PATH);
                    if(fileNode != NULL)
                    {
                        pParent->removeItem(fileNode);
                    }

                }
            }
        }
    }
}

bool GTAAppController::insertIntoTreeStructure(const QStringList &iListItems)
{
    bool rc= false;

    checkDataTreeMapConsistency();
    checkTreeNodeConsistency(iListItems.at(0));

    for(int i=0;i<iListItems.count();i++)
    {
        QString dirPath = iListItems.at(i);
        GTATreeItem *pParent;
        if(_DataTreeMap.contains(dirPath))
        {
            pParent  = _DataTreeMap.value(dirPath) ;
            storeTreeStructure(dirPath,pParent);
            rc = true;
        }
    }
    return rc;
}

/**
* @brief Processes a single file with respect to the action performed.
* @param iFile the path to the file to process.
* @param iActionType Action to perform with the file.
* @param iNewName In case the action is "rename", it is the new name given to the file.
* @return bool true if the process went fine.
*/
bool GTAAppController::updateTreeItem(const QString& iFile, TreeViewActionType iActionType, const QString& iNewName)
{
    bool rc = false;
    QFileInfo file(iFile);
    QString fileName = file.fileName();
    QString absPath = file.absolutePath();

    QString DataDirPath = _pSystemServices->getDataDirectory();
    QString temp = iFile;
    QString relativePath = temp.replace(DataDirPath, "", Qt::CaseInsensitive);
    QString parentRelPath = temp.replace("/" + fileName, "", Qt::CaseInsensitive);

    QString extension = QString();
    QString absoluteFilePath = file.absoluteFilePath();
    QString strTemp = absoluteFilePath;
    QString absRelPath = strTemp.replace(DataDirPath, "", Qt::CaseInsensitive);

    if (fileName.contains("."))
    {
        QStringList splitList = fileName.split(".");
        extension = splitList.last();
    }

    GTADocumentController docCtrl;
    // to read & update validation status in document viewer -->
    QString uuid, modifiedDate, authName, validatorName, validationStatus, validationDate, createdDate;
    QString description, gtaVersion, maxTimeEstimated;
    docCtrl.getDescAndInfo(absoluteFilePath, uuid, modifiedDate, authName, validatorName, validationStatus, validationDate, createdDate, description, gtaVersion, maxTimeEstimated);

    GTATreeItem* pItem = new GTATreeItem(fileName, absRelPath, relativePath, extension, description);
    pItem->setUUID(uuid);
    pItem->setModifiedDate(modifiedDate);
    pItem->setAuthorName(authName);
    pItem->setValidatedBy(validatorName);
    pItem->setValidationDate(validationDate);
    pItem->setGtaVersion(gtaVersion);
    pItem->setMaxTimeEstimated(maxTimeEstimated);

    //managing icons when svn is mapped for files
    QProcess myProcess;
    myProcess.start("svn", QStringList() << "status" << absoluteFilePath);
    rc = myProcess.waitForFinished();
    QString stdOutput = myProcess.readAllStandardOutput();
    QString errorCode = myProcess.readAllStandardError();
    if ((!errorCode.isEmpty() && stdOutput.isEmpty()) || !rc)
    {
        pItem->setSvnStatus("Not SVN");
    }
    else if (!stdOutput.isEmpty() && errorCode.isEmpty())
    {
        stdOutput.replace(" ", ">");
        QStringList values = stdOutput.split(">", QString::SkipEmptyParts);
        QString value;
        //Handles "changelist" in the console text
        if (stdOutput.contains("Changelist"))
        {
            QString changeValue = values.at(2);
            QStringList values1 = changeValue.split(":", QString::SkipEmptyParts);
            value = values1.last().simplified();
        }
        else
        {
            value = values.first().simplified();
        }
        pItem->setSvnStatus(value.toUpper());
    }

    //set validation Status for all elements with no status (backward compatibility)
    if (validationStatus.isEmpty())
        validationStatus = XNODE_NOT_VALIDATED;
    pItem->setValidationStatus(validationStatus);

    if (createdDate.isEmpty())
        pItem->setCreatedDate(file.created().toString());
    else
        pItem->setCreatedDate(createdDate);

    GTATreeItem* parent = _DataTreeMap.value(parentRelPath);
    if (parent == NULL)
        _treeRootNode->addChild(pItem);
    else
    {
        pItem->setParent(parent);
        parent->addChild(pItem);
    }

    _dbDataController->addFile(pItem);

    rc = storeTreeStructure(_pSystemServices->getDataDirectory(), _treeRootNode);

    if (_DataTreeMap.contains(relativePath))
    {
        GTATreeItem* pParent = _DataTreeMap.value(relativePath);
        if (pParent == NULL)
        {
            QString tempRelPath = relativePath;
            tempRelPath.replace(DataDirPath, "");
            pParent = _DataTreeMap.value(tempRelPath);
        }

        if (pParent != NULL)
        {
            GTATreeItem* fileNode = pParent->findNode(relativePath, GTATreeItem::ABS_REL_FILE_PATH);
            if (fileNode != NULL)
            {
                if (iActionType == Save)
                {
                    GTADocumentController docCtrl;
                    // to read & update validation status in document viewer -->
                    QString uuid, modifiedDate, authName, validatorName, validationStatus, validationDate, createdDate, absRelPath, gtaVersion, description, maxTimeEstimated;
                    docCtrl.getDescAndInfo(iFile, uuid, modifiedDate, authName, validatorName, validationStatus, validationDate, createdDate, description, gtaVersion, maxTimeEstimated);
                    fileNode->setName(fileName);
                    fileNode->setDescription(description);
                    fileNode->setGtaVersion(gtaVersion);
                    fileNode->setUUID(uuid);
                    fileNode->setCreatedDate(createdDate);
                    fileNode->setAuthorName(authName);
                    fileNode->setModifiedDate(modifiedDate);
                    fileNode->setValidationStatus(validationStatus);
                    fileNode->setValidatedBy(validatorName);
                    fileNode->setValidationDate(validationDate);
                    fileNode->setMaxTimeEstimated(maxTimeEstimated);
                    _dbDataController->updateFile(fileNode);
                }
                else if (iActionType == Rename)
                {
                    GTADocumentController docCtrl;
                    // to read & update validation status in document viewer -->
                    QString uuid, modifiedDate, authName, validatorName, validationStatus, validationDate, createdDate, absRelPath, gtaVersion, description, maxTimeEstimated;
                    docCtrl.getDescAndInfo(iNewName, uuid, modifiedDate, authName, validatorName, validationStatus, validationDate, createdDate, description, gtaVersion, maxTimeEstimated);

                    absRelPath = iNewName;
                    absRelPath = absRelPath.replace(DataDirPath, "", Qt::CaseInsensitive);

                    QFileInfo newFile(iNewName);
                    QString newName = newFile.fileName();
                    QString oldPath = fileNode->getAbsoluteRelPath();
                    fileNode->setName(newName);
                    fileNode->setDescription(description);
                    fileNode->setGtaVersion(gtaVersion);
                    fileNode->setUUID(uuid);
                    fileNode->setCreatedDate(createdDate);
                    fileNode->setAuthorName(authName);
                    fileNode->setModifiedDate(modifiedDate);
                    fileNode->setValidationStatus(validationStatus);
                    fileNode->setValidatedBy(validatorName);
                    fileNode->setValidationDate(validationDate);
                    fileNode->setAbsoluteRelPath(absRelPath);
                    fileNode->setMaxTimeEstimated(maxTimeEstimated);
                    _dbDataController->updateFile(fileNode, oldPath);
                }
                else if (iActionType == Delete)
                {
                    pParent->removeItem(fileNode);
                    _dbDataController->deleteFile(fileNode->getAbsoluteRelPath());
                }
                rc = true;
            }
        }
    }
    updateTreeStructureDB(_treeRootNode, DataDirPath);
    return rc;
}

/**
* @brief Updates the svn status of the single tree item.
* @param absoluteFilePath the path to the file to process.
* @param child pointer to the treeItem corresponding to the file.
* @return bool true if the process went fine.
*/
bool GTAAppController::updateSvnStatusOfTheFile(QString &absoluteFilePath, GTATreeItem *child)
{
    QProcess myProcess;
    myProcess.start("svn", QStringList() << "info" << absoluteFilePath);
    bool rc = myProcess.waitForFinished();
    QString stdOutput = myProcess.readAllStandardOutput();
    QString errorCode = myProcess.readAllStandardError();

    if (errorCode.isEmpty()) {
        myProcess.start("svn", QStringList() << "status" << absoluteFilePath);
        rc = myProcess.waitForFinished();
        stdOutput = myProcess.readAllStandardOutput();
        errorCode = myProcess.readAllStandardError();
        //stdOutput
        if ((!errorCode.isEmpty() && stdOutput.isEmpty()) || !rc)
        {
            child->setSvnStatus("Not SVN");
        }
        else if (!stdOutput.isEmpty() && errorCode.isEmpty())
        {
            stdOutput.replace(" ", ">");
            QStringList values = stdOutput.split(">", QString::SkipEmptyParts);
            QString value;
            //Handles "changelist" in the console text
            if (stdOutput.contains("Changelist"))
            {
                QString changeValue = values.at(2);
                QStringList values1 = changeValue.split(":", QString::SkipEmptyParts);
                value = values1.last().simplified();
            }
            else
            {
                value = values.first().simplified();
            }
            child->setSvnStatus(value.toUpper());
        }
        else if (stdOutput.isEmpty() && errorCode.isEmpty())
        {
            QString value = " ";
            child->setSvnStatus(value.toUpper());
        }
    }
    return true;
}

void GTAAppController::sortDataTreeMap()
{
    QList<QString> sortedKeys = _DataTreeMap.keys();
    qSort(sortedKeys.begin(), sortedKeys.end()); // Sort the keys alphabetically
    QHash<QString, GTATreeItem*> sortedMap;

    foreach(const QString & key, sortedKeys)
    {
        sortedMap.insert(key, _DataTreeMap.value(key));
    }

    _DataTreeMap = sortedMap;
}

/**
* @brief Updates the content of the tree view with respect to either work_db (if _dbDataController->getIsNew() is false) or the system files (if _dbDataController->getIsNew() is true). This method is recursive.
* @param iDataDirectoryPath The current node path. If it's a folder, it will eventually call itself with its children; if it's a file, it is processed accordingly.
* @param pParent The current TreeItem node. Same remarks as for iDataDirectoryPath.
* @param isRefreshTree
* @return bool true if then storage went fine.
*/
bool GTAAppController::storeTreeStructure(const QString& iDataDirectoryPath, GTATreeItem*& pParent)
{
    bool rc = false;
    GTADocumentController docCtrl;
    QDir dataDir(iDataDirectoryPath);
    if (!_FileWatcherList.contains(iDataDirectoryPath))
        _FileWatcherList.append(iDataDirectoryPath);
    dataDir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Dirs);
    dataDir.setSorting(QDir::Name | QDir::DirsFirst);
    /*QStringList filters;
    filters << "*.pro" << "*.seq" << "*.fun" << "*.obj" << "*.xml";
    dataDir.setNameFilters(filters);*/

    QFileInfoList listFilesFolders = dataDir.entryInfoList();
    if (!_dbDataController->getIsNew())
    {
        if (iDataDirectoryPath == _pSystemServices->getDataDirectory())
        {
            if (_treeRootNode == NULL)
            {
                QString root = "root";
                _treeRootNode = new GTATreeItem(root, root, root, root, root);
            }

            if (!_DataTreeMap.contains(iDataDirectoryPath)) {
                _DataTreeMap.insert(iDataDirectoryPath, _treeRootNode);
            }
            sortDataTreeMap();
        }

        _dbDataController->getAllChildren(pParent);

        foreach(GTATreeItem * child, pParent->getChildren())
        {
            if (child->getExtension() == "folder")
            {
                // We have to send the new absolute path
                storeTreeStructure(iDataDirectoryPath + "/" + child->getName(), child);
            }
            if (!_DataTreeMap.contains(child->getAbsoluteRelPath())) {
                _DataTreeMap.insert(child->getAbsoluteRelPath(), child);
            }
        }
        rc = true;

    }
    else if (listFilesFolders.count() > 0)
    {
        for (int i = 0; i < listFilesFolders.count(); i++)
        {
            QFileInfo file = listFilesFolders.at(i);
            QString fileName = file.fileName();
            QString extension = QString();

            // For FILE
            if (file.isFile())
            {
                QString DataDirPath = _pSystemServices->getDataDirectory();
                QString absoluteFilePath = file.absoluteFilePath();

                QString strTemp = absoluteFilePath;
                QString absRelPath = strTemp.replace(DataDirPath, "", Qt::CaseInsensitive);

                if (fileName.endsWith(".pro"))
                {
                    extension = QString("pro");
                }
                else if (fileName.endsWith(".seq"))
                {
                    extension = QString("seq");
                }
                else if (fileName.endsWith(".fun"))
                {
                    extension = QString("fun");
                }
                else if (fileName.endsWith(".obj"))
                {
                    extension = QString("obj");
                }
                else
                {
                    continue;
                }

                if (pParent->containsFile(absRelPath))
                {
                    continue;
                }

                QString relativePath;
                QString absolutePath = file.absolutePath();

                if (absolutePath == DataDirPath)
                {
                    // DATA/ aka Root
                    relativePath = "";
                }
                else
                {
                    QString temp = absolutePath;
                    relativePath = temp.replace(DataDirPath, "", Qt::CaseInsensitive);
                }

                // to read & update validation status in document viewer -->
                QString uuid, modifiedDate, authName, validatorName, validationStatus, validationDate, createdDate;
                //QString description = docCtrl.getDescriptionFromHeader(absoluteFilePath);
                //docCtrl.getDocumentInfo(absoluteFilePath, uuid, modifiedDate, authName,validatorName,validationStatus,validationDate,createdDate);
                QString description, gtaVersion, maxTimeEstimated;

                docCtrl.getDescAndInfo(absoluteFilePath, uuid, modifiedDate, authName, validatorName, validationStatus, validationDate, createdDate, description, gtaVersion, maxTimeEstimated);

                GTATreeItem* pItem = new GTATreeItem(fileName, absRelPath, relativePath, extension, description);
                pItem->setUUID(uuid);
                pItem->setModifiedDate(modifiedDate);
                pItem->setAuthorName(authName);
                pItem->setValidatedBy(validatorName);
                pItem->setValidationDate(validationDate);
                pItem->setGtaVersion(gtaVersion);
                pItem->setMaxTimeEstimated(maxTimeEstimated);

                //managing icons when svn is mapped for files
                //QProcess myProcess;
                //myProcess.start("svn", QStringList() << "status" << absoluteFilePath);
                //rc = myProcess.waitForFinished();
                //QString stdOutput = myProcess.readAllStandardOutput();
                //QString errorCode = myProcess.readAllStandardError();
                //if ((!errorCode.isEmpty() && stdOutput.isEmpty()) || !rc)
                //{
                //    pItem->setSvnStatus("Not SVN");
                //}
                //else if (!stdOutput.isEmpty() && errorCode.isEmpty())
                //{
                //    stdOutput.replace(" ", ">");
                //    QStringList values = stdOutput.split(">", QString::SkipEmptyParts);
                //    QString value;
                    //Handles "changelist" in the console text
                //    if (stdOutput.contains("Changelist"))
                //    {
                //        QString changeValue = values.at(2);
                //        QStringList values1 = changeValue.split(":", QString::SkipEmptyParts);
                //        value = values1.last().simplified();
                //    }
                //    else
                //    {
                 //       value = values.first().simplified();
                 //   }
                 //   pItem->setSvnStatus(value.toUpper());
                //}

                //set validation Status for all elemens with no status (backward compatibility)
                if (validationStatus.isEmpty())
                    validationStatus = XNODE_NOT_VALIDATED;
                pItem->setValidationStatus(validationStatus);

                if (createdDate.isEmpty())
                    pItem->setCreatedDate(file.created().toString());
                else
                    pItem->setCreatedDate(createdDate);
                //<--to read & update validation status in document viewer
                pParent->addChild(pItem);

                _dbDataController->addFile(pItem);

            }
            else if (file.isDir())
            {
                extension = QString("folder");
                QString DataDirPath = _pSystemServices->getDataDirectory();
                QString absoluteParentPath = file.absolutePath();
                QString relativeParentPath;

                if (absoluteParentPath == DataDirPath)
                {
                    // DATA/ aka ROOT
                    relativeParentPath = "";
                }
                else
                {
                    // We are not in DATA/ but at least in a sub-folder
                    QString temp = absoluteParentPath;
                    relativeParentPath = temp.replace(DataDirPath, "", Qt::CaseInsensitive);
                }
                QString absoluteFilePath = file.absoluteFilePath();
                QString strTemp = absoluteFilePath;
                // We get only the relativePath
                QString absoluteRelativePath = strTemp.replace(DataDirPath, "", Qt::CaseInsensitive);
                QString insideDirPath = file.absoluteFilePath();

                GTATreeItem* pItem = new GTATreeItem(fileName, absoluteRelativePath, relativeParentPath, extension, QString());
                //set validation Status for all folders
                pItem->setValidationStatus("NA");

                //managing icons when svn is mapped for folders
                /*
                QProcess myProcess;
                myProcess.start("svn", QStringList() << "status" << "-N" << insideDirPath);
                rc = myProcess.waitForFinished();
                QString stdOutput = myProcess.readAllStandardOutput();
                QString errorCode = myProcess.readAllStandardError();
                if ((!errorCode.isEmpty() && stdOutput.isEmpty()) || !rc)
                {
                    pItem->setSvnStatus("Not SVN");
                }
                else if (!stdOutput.isEmpty() && errorCode.isEmpty())
                {
                    stdOutput.replace(" ", ">");
                    QStringList values = stdOutput.split(">", QString::SkipEmptyParts);
                    QString value;

                    //Handles "changelist" in the console text
                    if (stdOutput.contains("Changelist"))
                    {
                        QString changeValue = values.at(2);
                        QStringList values1 = changeValue.split(":", QString::SkipEmptyParts);
                        value = values1.last().simplified();
                    }
                    else
                    {
                        value = values.first().simplified();
                    }
                    pItem->setSvnStatus(value.toUpper());
                }*/

                pParent->addChild(pItem);
                if (!_DataTreeMap.contains(absoluteRelativePath)) {
                    _DataTreeMap.insert(absoluteRelativePath, pItem);
                }
                //_DataTreeMap.insert(absoluteRelativePath, pItem);
                storeTreeStructure(insideDirPath, pItem);
                _dbDataController->addFile(pItem);

            }
        }
        rc = true;
    }
    return rc;
}


GTATreeItem GTAAppController::treeItemFromFileInfo(QFileInfo file)
{
	return GTATreeItem();
}

bool GTAAppController::updateTreeStructureDB(GTATreeItem*& pParent, QString iDataDirectoryPath)
{
	bool rc = false;
	GTADocumentController docCtrl;
	QDir dataDir(iDataDirectoryPath);
	if (!_FileWatcherList.contains(iDataDirectoryPath))
		_FileWatcherList.append(iDataDirectoryPath);
	dataDir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Dirs);
	dataDir.setSorting(QDir::Name | QDir::DirsFirst);

	QFileInfoList listFilesFolders = dataDir.entryInfoList();
	QString DataDirPath = _pSystemServices->getDataDirectory();
	QDateTime dbModified = _dbDataController->getLastModified(DataDirPath + "/../WORK_DB.db");
	QHash<QString, QString> fileLastModifTable = _dbDataController->getLastModifiedByParent(pParent->getAbsoluteRelPath());

	if (listFilesFolders.count() > 0)
	{
		for (int i = 0; i < listFilesFolders.count(); i++)
		{
			QFileInfo file = listFilesFolders.at(i);
			QString fileName = file.fileName();
			QString extension;

			if (file.isFile())
			{
				QString absoluteFilePath = file.absoluteFilePath();
				QString strTemp = absoluteFilePath;
				QString absRelPath = strTemp.replace(DataDirPath, "", Qt::CaseInsensitive);
				
				if (fileLastModifTable.contains(absRelPath))
				{
					QDateTime lastModifiedFileDB = QDateTime::fromString(fileLastModifTable.value(absRelPath));
					QDateTime lastModifiedFile = file.lastModified();
					
					if (lastModifiedFileDB < lastModifiedFile)
					{
						fileLastModifTable.remove(absRelPath);
						continue;
					}
					else
					{
						if (fileName.endsWith(".pro"))
						{
							extension = QString("pro");
						}
						else if (fileName.endsWith(".seq"))
						{
							extension = QString("seq");
						}
						else if (fileName.endsWith(".fun"))
						{
							extension = QString("fun");
						}
						else if (fileName.endsWith(".obj"))
						{
							extension = QString("obj");
						}
						else
						{
							continue;
						}

						QString relativePath;
						QString absolutePath = file.absolutePath();

						if (absolutePath == DataDirPath)
						{
							relativePath = "";
						}
						else
						{
							QString temp = absolutePath;
							relativePath = temp.replace(DataDirPath, "", Qt::CaseInsensitive);
						}
						
						// to read & update validation status in document viewer -->
						QString uuid, modifiedDate, authName, validatorName, validationStatus, validationDate, createdDate;
						QString description, gtaVersion, maxTimeEstimated;
						docCtrl.getDescAndInfo(absoluteFilePath, uuid, modifiedDate, authName, validatorName, validationStatus, validationDate, createdDate, description, gtaVersion, maxTimeEstimated);
						GTATreeItem *pItem = new GTATreeItem(fileName, absRelPath, relativePath, extension, description);
						pItem->setUUID(uuid);
						pItem->setModifiedDate(modifiedDate);
						pItem->setAuthorName(authName);
						pItem->setValidatedBy(validatorName);
						pItem->setValidationDate(validationDate);
						pItem->setGtaVersion(gtaVersion);
                        pItem->setMaxTimeEstimated(maxTimeEstimated);

						//managing icons when svn is mapped for files
                        /*
						QProcess myProcess;
						myProcess.start("svn", QStringList() << "status" << absoluteFilePath);
						rc = myProcess.waitForFinished();
						QString stdOutput = myProcess.readAllStandardOutput();
						QString errorCode = myProcess.readAllStandardError();
						if ((!errorCode.isEmpty() && stdOutput.isEmpty()) || !rc)
						{
							pItem->setSvnStatus("Not SVN");
						}
						else if (!stdOutput.isEmpty() && errorCode.isEmpty())
						{
							stdOutput.replace(" ", ">");
							QStringList values = stdOutput.split(">", QString::SkipEmptyParts);
							QString value;
							//Handles "changelist" in the console text
							if (stdOutput.contains("Changelist"))
							{
								QString changeValue = values.at(2);
								QStringList values1 = changeValue.split(":", QString::SkipEmptyParts);
								value = values1.last().simplified();
							}
							else
							{
								value = values.first().simplified();
							}
							pItem->setSvnStatus(value.toUpper());
						}*/

						//set validation Status for all elemens with no status (backward compatibility)
						if (validationStatus.isEmpty())
							validationStatus = XNODE_NOT_VALIDATED;
						pItem->setValidationStatus(validationStatus);

						if (createdDate.isEmpty())
							pItem->setCreatedDate(file.created().toString());
						else
							pItem->setCreatedDate(createdDate);
						//<--to read & update validation status in document viewer
						_dbDataController->updateFile(pItem);
						fileLastModifTable.remove(absRelPath);
					}
				}
				else
				{
					if (fileName.endsWith(".pro"))
					{
						extension = QString("pro");
					}
					else if (fileName.endsWith(".seq"))
					{
						extension = QString("seq");
					}
					else if (fileName.endsWith(".fun"))
					{
						extension = QString("fun");
					}
					else if (fileName.endsWith(".obj"))
					{
						extension = QString("obj");
					}
					else
					{
						continue;
					}

					QString relativePath;
					QString absolutePath = file.absolutePath();

					if (absolutePath == DataDirPath)
					{
						relativePath = "";
					}
					else
					{
						QString temp = absolutePath;
						relativePath = temp.replace(DataDirPath, "", Qt::CaseInsensitive);
					}
					
					// to read & update validation status in document viewer -->
					QString uuid, modifiedDate, authName, validatorName, validationStatus, validationDate, createdDate;
					QString description, gtaVersion, maxTimeEstimated;
					docCtrl.getDescAndInfo(absoluteFilePath, uuid, modifiedDate, authName, validatorName, validationStatus, validationDate, createdDate, description, gtaVersion, maxTimeEstimated);
                    
                    GTATreeItem *pItem = new GTATreeItem(fileName, absRelPath, relativePath, extension, description);
					pItem->setUUID(uuid);
					pItem->setModifiedDate(modifiedDate);
					pItem->setAuthorName(authName);
					pItem->setValidatedBy(validatorName);
					pItem->setValidationDate(validationDate);
					pItem->setGtaVersion(gtaVersion);
                    pItem->setMaxTimeEstimated(maxTimeEstimated);

					//managing icons when svn is mapped for files
                    /*
					QProcess myProcess;
					myProcess.start("svn", QStringList() << "status" << absoluteFilePath);
					rc = myProcess.waitForFinished();
					QString stdOutput = myProcess.readAllStandardOutput();
					QString errorCode = myProcess.readAllStandardError();
					if ((!errorCode.isEmpty() && stdOutput.isEmpty()) || !rc)
					{
						pItem->setSvnStatus("Not SVN");
					}
					else if (!stdOutput.isEmpty() && errorCode.isEmpty())
					{
						stdOutput.replace(" ", ">");
						QStringList values = stdOutput.split(">", QString::SkipEmptyParts);
						QString value;
						//Handles "changelist" in the console text
						if (stdOutput.contains("Changelist"))
						{
							QString changeValue = values.at(2);
							QStringList values1 = changeValue.split(":", QString::SkipEmptyParts);
							value = values1.last().simplified();
						}
						else
						{
							value = values.first().simplified();
						}
						pItem->setSvnStatus(value.toUpper());
					}*/

					//set validation Status for all elemens with no status (backward compatibility)
					if (validationStatus.isEmpty())
						validationStatus = XNODE_NOT_VALIDATED;
					pItem->setValidationStatus(validationStatus);

					if (createdDate.isEmpty())
						pItem->setCreatedDate(file.created().toString());
					else
						pItem->setCreatedDate(createdDate);
					//<--to read & update validation status in document viewer

					pParent->addChild(pItem);
					_dbDataController->addFile(pItem);
					_DataTreeMap.insert(absRelPath,pItem);
					fileLastModifTable.remove(absRelPath);
				}
			}
			else if (file.isDir())
			{
				extension = QString("folder");
				QString absolutePath = file.absolutePath();
				QString DataDirPath = _pSystemServices->getDataDirectory();
				QString relativePath;

				if (absolutePath == DataDirPath)
				{
					relativePath = "";
				}
				else
				{
					QString temp = absolutePath;
					relativePath = temp.replace(DataDirPath, "", Qt::CaseInsensitive);
				}
				QString absoluteFilePath = file.absoluteFilePath();

				QString absRelPath = absoluteFilePath.replace(DataDirPath, "", Qt::CaseInsensitive);

                QString insideDirPath = file.absoluteFilePath();
                QString temp(insideDirPath);
                QString relDirPath = temp.replace(DataDirPath, "", Qt::CaseInsensitive);

				if (fileLastModifTable.contains(absRelPath))
				{
					/*QDateTime lastModifiedFileDB = QDateTime::fromString(fileLastModifTable.value(absRelPath));
					QDateTime lastModifiedFile = file.lastModified();
					if (lastModifiedFileDB < lastModifiedFile)
					{
						fileLastModifTable.remove(absRelPath);
						continue;
					}
					else*/
					{
						

						/*if (_DataTreeMap.contains(insideDirPath))
							continue;*/

						GTATreeItem *pItem = new GTATreeItem(fileName, absRelPath, relativePath, extension, QString());
						//set validation Status for all folders
						pItem->setValidationStatus("NA");

						//managing icons when svn is mapped for folders
                        /*
						QProcess myProcess;
						myProcess.start("svn", QStringList() << "status" << "-N" << insideDirPath);
						rc = myProcess.waitForFinished();
						QString stdOutput = myProcess.readAllStandardOutput();
						QString errorCode = myProcess.readAllStandardError();
						if ((!errorCode.isEmpty() && stdOutput.isEmpty()) || !rc)
						{
							pItem->setSvnStatus("Not SVN");
						}
						else if (!stdOutput.isEmpty() && errorCode.isEmpty())
						{
							stdOutput.replace(" ", ">");
							QStringList values = stdOutput.split(">", QString::SkipEmptyParts);
							QString value;

							//Handles "changelist" in the console text
							if (stdOutput.contains("Changelist"))
							{
								QString changeValue = values.at(2);
								QStringList values1 = changeValue.split(":", QString::SkipEmptyParts);
								value = values1.last().simplified();
							}
							else
							{
								value = values.first().simplified();
							}
							pItem->setSvnStatus(value.toUpper());
						}*/

						//pParent->addChild(pItem);

                        if (_DataTreeMap.contains(relDirPath))
                            _DataTreeMap.insert(relDirPath, pItem);
						
						_dbDataController->updateFile(pItem);
						fileLastModifTable.remove(absRelPath);
						updateTreeStructureDB(pItem, insideDirPath);
					}
				}
				else
				{
					QString insideDirPath = file.absoluteFilePath();
                    QString temp(insideDirPath);
					QString relativePath = temp.replace(DataDirPath, "", Qt::CaseInsensitive);
					if (_DataTreeMap.contains(relativePath))
						continue;
					
					GTATreeItem *pItem = new GTATreeItem(fileName, absRelPath, relativePath, extension, QString());
					//set validation Status for all folders
					pItem->setValidationStatus("NA");

					//managing icons when svn is mapped for folders
                    /*
					QProcess myProcess;
					myProcess.start("svn", QStringList() << "status" << "-N" << insideDirPath);
					rc = myProcess.waitForFinished();
					QString stdOutput = myProcess.readAllStandardOutput();
					QString errorCode = myProcess.readAllStandardError();
					if ((!errorCode.isEmpty() && stdOutput.isEmpty()) || !rc)
					{
						pItem->setSvnStatus("Not SVN");
					}
					else if (!stdOutput.isEmpty() && errorCode.isEmpty())
					{
						stdOutput.replace(" ", ">");
						QStringList values = stdOutput.split(">", QString::SkipEmptyParts);
						QString value;

						//Handles "changelist" in the console text
						if (stdOutput.contains("Changelist"))
						{
							QString changeValue = values.at(2);
							QStringList values1 = changeValue.split(":", QString::SkipEmptyParts);
							value = values1.last().simplified();
						}
						else
						{
							value = values.first().simplified();
						}
						pItem->setSvnStatus(value.toUpper());
					}*/

                    if (_DataTreeMap.contains(relDirPath))
                        _DataTreeMap.insert(relDirPath, pItem);

					pParent->addChild(pItem);
					_dbDataController->addFile(pItem);
					fileLastModifTable.remove(absRelPath);
					updateTreeStructureDB(pItem, insideDirPath);
				}
			}
		}
		if (fileLastModifTable.count() > 0)
		{
			QStringList filePathList = fileLastModifTable.keys();
			for (int i = 0; i < filePathList.count(); i++)
			{
				_dbDataController->deleteFile(filePathList.at(i));
			}
		}
		rc = true;
	}
	else if(fileLastModifTable.count() > 0)
	{
		QStringList filePathList = fileLastModifTable.keys();
		for (int i = 0; i < filePathList.count(); i++)
		{
			_dbDataController->deleteFile(filePathList.at(i));
		}
		rc = true;
	}
	return rc;
}

/**
* @brief Initialize parameter DB
*/
bool GTAAppController::initializeDatabase()
{
    bool rc = true;
    if(_pSystemServices)
    {
        QString dbFilePath = _pSystemServices->getDatabaseFile();
        QFile file(dbFilePath);
        if(!file.exists())
            rc = _pSystemServices->createTemplateDbFile();

        if(rc)
        {
            GTADataController db(dbFilePath);
            rc = db.openDatabase();
            db.createHistoryTable(dbFilePath);
            _HistoryVarList = db.getParametersFromHistory();

            QString sessionDbPath = _pSystemServices->getUserDatabasePath();
            db.openUserDatabase(sessionDbPath);
            db.createSessionTable(sessionDbPath);
        }
    }

    return rc;
}

void GTAAppController::updateDatabase()
{
    bool rc = true;
    if(_pSystemServices)
    {
        // closing old connection
        GTADataController dataCtrl;
        QString path = dataCtrl.getDatabasePath();
        QString dbFilePath = _pSystemServices->getDatabaseFile();
        if(path == dbFilePath)
        {
            return;
        }

        dataCtrl.updateHistoryTable(_CurrentSessionVarList);
        dataCtrl.commitDatabase();
        dataCtrl.closeDatabase();

        // setting database to new path. New connection.

        QFile file(dbFilePath);
        if(!file.exists())
            rc = _pSystemServices->createTemplateDbFile();

        if(rc)
        {
            GTADataController db(dbFilePath);
            rc = db.openDatabase();
            db.createHistoryTable(dbFilePath);
        }
    }
}


bool GTAAppController::commitDatabase()
{
    bool rc = false;
    if(_pSystemServices)
    {
        QString dbFilePath = _pSystemServices->getDatabaseFile();
        GTADataController db(dbFilePath);
        rc = db.commitDatabase();
    }
    return rc;
}


GTATreeItem * GTAAppController::getTreeNodeFromIndex(const QModelIndex &iIndex)
{
    GTATreeItem * pItem = NULL;
    if(_pViewController)
    {
        QSortFilterProxyModel *treeViewModel1  = (QSortFilterProxyModel *)iIndex.model();
        if (treeViewModel1!=NULL)
        {
            GTATreeViewModel * treeViewModel = (GTATreeViewModel*)treeViewModel1->sourceModel();
            if (treeViewModel!=NULL)
            {
				QModelIndex currIndex = treeViewModel1->mapToSource(iIndex);
                QModelIndex pParentIndex =currIndex.parent();
                if(!pParentIndex.isValid())
                {
                    pParentIndex = QModelIndex();
                }
                QModelIndex pIndex = treeViewModel->index(currIndex.row(),currIndex.column(),pParentIndex);
                if(pIndex.isValid())
                {
                    pItem = treeViewModel->nodeFromIndex(pIndex);
                }
            }
        }
    }
    return pItem;
}

QString GTAAppController::getFilePathFromTreeModel(const QModelIndex &iIndex,bool iFileUUIDNeeded)
{
    if(_pViewController)
    {
        QSortFilterProxyModel *treeViewModel1  = (QSortFilterProxyModel *)iIndex.model();
        if (treeViewModel1!=NULL)
        {
            GTATreeViewModel * treeViewModel = (GTATreeViewModel*)treeViewModel1->sourceModel();
            if (treeViewModel!=NULL)
            {

                QModelIndex currIndex = treeViewModel1->mapToSource(iIndex);
                QModelIndex pParentIndex =currIndex.parent();
                if(!pParentIndex.isValid())
                {
                    pParentIndex = QModelIndex();
                }
				
				QModelIndex pIndex = treeViewModel->index(currIndex.row(),0,pParentIndex);
                QModelIndex pIndex1 = treeViewModel->index(currIndex.row(),3,pParentIndex);
                QString FileUUID,FilePath;
                if (pIndex1.isValid())
                {
                    FileUUID = treeViewModel->getFileUUIDForIndex(pIndex1);
                }
                if(pIndex.isValid())
                {
                    FilePath = treeViewModel->getAbsolutePathForIndex(pIndex);
                }
                return (iFileUUIDNeeded?(QString("%1::%2").arg(FilePath,FileUUID)):FilePath);
            }
        }
    }
    return QString();
}


QStringList GTAAppController::getFileWatcherList()const
{
    return _FileWatcherList;
}

void GTAAppController::readFavoritesFromDb()
{

    QString favFile = _pSystemServices->getFavoritesFilePath();
    QFileInfo fileInfo(favFile);
    if(fileInfo.exists())
    {
        QFile xmlFile(favFile);
        if(xmlFile.open(QIODevice::ReadOnly | QFile::Text))
        {
            QDomDocument *pDomDoc = new QDomDocument();
            if(pDomDoc != NULL)
            {
                QString errMsg;
                int errLine;
                int errCol;
                if(pDomDoc->setContent(&xmlFile,false,&errMsg,&errLine,&errCol))
                {
                    QDomElement rootElement = pDomDoc->documentElement();
                    if(rootElement.hasChildNodes())
                    {
                        QString dataDirPath = _pSystemServices->getDataDirectory();
                        QDomNodeList items = rootElement.childNodes();
                        for(int i = 0; i < items.count(); i++)
                        {
                            QDomNode nodeItem = items.at(i);
                            QString absoluteRelPath = nodeItem.attributes().namedItem("AbsoluteRelPath").nodeValue();
                            QString absolutePath = QDir::cleanPath(QString("%1%2%3").arg(dataDirPath,QDir::separator(),absoluteRelPath));
                            QFileInfo itemFileInfo(absolutePath);
                            if(itemFileInfo.exists())
                            {
                                if(_treeRootNode)
                                {

                                    GTATreeItem *foundNode= _treeRootNode->findNode(absoluteRelPath,GTATreeItem::ABS_REL_FILE_PATH);
                                    if(foundNode)
                                    {
                                        foundNode->setIsFavourite(true);
                                    }
                                }
                            }
                        }
                    }
                }
                delete pDomDoc;
                pDomDoc = NULL;

            }
        }
        xmlFile.close();
    }
}

bool GTAAppController::renameFile(const QModelIndex &indexToLoad, const QString &iNewName)
{
    bool rc = false;
    GTATreeItem *pItem = _pAppController->getTreeNodeFromIndex(indexToLoad);
    if(pItem)
    {
        QString extension = (pItem->getExtension() == "folder") ? "" : "."+pItem->getExtension();
        QString fileName = pItem->getAbsoluteRelPath();

        GTASystemServices* pSystemService = _pAppController->getSystemService();
        QString dataDirPath = pSystemService->getDataDirectory();

        fileName = QDir::cleanPath(QString("%1%2%3").arg(dataDirPath, QString("/"),fileName));
        QFile file(fileName);
        if(file.exists())
        {
            QFileInfo fileInfo(fileName);
            QString absPath =  fileInfo.absolutePath();
            QString absFilePath = fileInfo.absoluteFilePath();

            QString newName = QDir::cleanPath(QString("%1%2%3%4").arg(absPath, QString("/"),iNewName,extension));

            if (extension != "")
            {
                rc = file.rename(newName);
                if (rc)
                {
                    GTADocumentController docCtrl;

                    QString relPath = newName;
                    relPath.replace(dataDirPath, "");
                    relPath = QDir::cleanPath(relPath);

                    docCtrl.setDocumentName(newName,relPath,iNewName);
                    updateTreeItem(absFilePath,Rename,newName);
                }
            }
            else
            {
                // This part is a hack, to be honest. It turns out performing a move is way more efficient than renaming each children of the folder.
                // All in all, refactoring the whole tree view / db part is a User Story of its own. For now, let's do this, cause it works.
                // Let the rest in God's hands.
                rc = true;
                QDir(absPath).mkdir(iNewName);
                QList<GTATreeItem*> selected = pItem->getChildren();
                moveFiles(selected, newName);
                QDir(absPath).rmdir(fileName);
            }
        }
    }
    return rc;
}

void GTAAppController::getSCXMLToolID(QList<GTASCXMLToolId> &oToolIDList,GTASCXMLToolId  &oDefaultToolVal)const
{
    oToolIDList = _ToolIDList;
    oDefaultToolVal = _DefaultToolVal;
}

QList<GTASCXMLToolId> GTAAppController::readToolIdFromXml(GTASCXMLToolId &oDefaultToolVal)
{
    QList<GTASCXMLToolId> ToolID;
    if(_pSystemServices)
    {
        QString scxmlToolIDPath = _pSystemServices->getSCXMLToolIDFilePath();
        QFile toolIdFile(scxmlToolIDPath);
        if(toolIdFile.exists())
        {
            QFile xmlFile(scxmlToolIDPath);
            if(xmlFile.open(QIODevice::ReadOnly | QFile::Text))
            {
                QDomDocument *pDomDoc = new QDomDocument();
                if(pDomDoc != NULL)
                {
                    QString errMsg;
                    int errLine;
                    int errCol;
                    if(pDomDoc->setContent(&xmlFile,false,&errMsg,&errLine,&errCol))
                    {
                        QDomElement rootElement = pDomDoc->documentElement();
                        if(rootElement.hasChildNodes())
                        {

                            QDomNodeList toolIdNodeList = rootElement.childNodes();
                            for(int i = 0; i < toolIdNodeList.count(); i++)
                            {
                                QDomNode toolIdNode = toolIdNodeList.at(i);
                                if(toolIdNode.nodeName() == "tool")
                                {
                                    QDomNamedNodeMap toolIdAttr = toolIdNode.attributes();
                                    QString id = toolIdAttr.namedItem("id").nodeValue();
                                    QString toolType = toolIdAttr.namedItem("toolType").nodeValue();
                                    QString defaultValue = toolIdAttr.namedItem("default").nodeValue();
                                    bool defValue;
                                    GTASCXMLToolId toolIDObj;
                                    toolIDObj.setId(id);
                                    toolIDObj.setToolType(toolType);
                                    if(defaultValue == "true")
                                    {
                                        defValue = true;
                                        toolIDObj.setDefaultVal(true);
                                        oDefaultToolVal = toolIDObj;
                                    }
                                    ToolID.append(toolIDObj);
                                }
                            }
                        }
                        delete pDomDoc;
                        pDomDoc = NULL;
                        
                    }
                    else
                    {
                        delete pDomDoc;
                        pDomDoc = NULL;
                    }
                }
            }
        }
    }
    return ToolID;
}


GTASqlTableModel * GTAAppController::getEquipmentModel()const
{
    return _pEquipmentModel;
}


void GTAAppController::processEquipmentData()
{
    _ToolIDList = readToolIdFromXml(_DefaultToolVal);
}

void GTAAppController::onNewToolIdsAdded(const QList<GTASCXMLToolId> &iCompleteList)
{
    QList<GTASCXMLToolId> diffList;
    for(int i = 0; i < iCompleteList.count(); i++)
    {
        GTASCXMLToolId obj = iCompleteList.at(i);
        if(!_ToolIDList.contains(obj))
        {
            diffList.append(obj);
        }
    }

    if(_pSystemServices && diffList.count() > 0)
    {
        QString scxmlToolIDPath = _pSystemServices->getSCXMLToolIDFilePath();

        QFile xmlFileObj(QDir::cleanPath(scxmlToolIDPath));
        bool rc = xmlFileObj.open(QFile::Text| QFile::ReadWrite);
        if(rc)
        {
            QDomDocument DomDoc;
            rc = DomDoc.setContent(&xmlFileObj);
            if(rc)
            {
                xmlFileObj.close();
                QFile xmlNewFileObj(scxmlToolIDPath);
                bool rC = xmlNewFileObj.open(QFile::Text | QFile::WriteOnly | QFile::Truncate);
                if(rC)
                {
                    QDomElement rootElement = DomDoc.documentElement();
                    for(int i = 0; i < diffList.count(); i++)
                    {
                        GTASCXMLToolId obj = diffList.at(i);
                        QString toolName = obj.getId();
                        QString toolType = obj.getToolType();
                        QString sDefVal = QString("false");
                        QString status = QString("false");
                        QString equipName = obj.getListEquipName();
                        
                        bool defVal= obj.isDefault();
                        if(defVal)
                            sDefVal = QString("true");


                        QDomElement toolElement = DomDoc.createElement("tool");
                        toolElement.setAttribute("id",toolName);
                        toolElement.setAttribute("toolType",toolType);
                        toolElement.setAttribute("default",sDefVal);
                        toolElement.setAttribute("activate", status);
                        toolElement.setAttribute("equipName", equipName);
                        
                        rootElement.appendChild(toolElement);
                    }
                    QTextStream stream(&xmlNewFileObj);
                    stream << DomDoc.toString();
                    xmlNewFileObj.close();
                }
            }
        }
        _ToolIDList = iCompleteList;
    }
}


void GTAAppController::onRemoveToolID(const QList<GTASCXMLToolId> &iUpdatedList, const GTASCXMLToolId &iRemoveObj)
{

    if(_pSystemServices)
    {
        QString scxmlToolIDPath = _pSystemServices->getSCXMLToolIDFilePath();

        QFile xmlFileObj(QDir::cleanPath(scxmlToolIDPath));
        bool rc = xmlFileObj.open(QFile::Text| QFile::ReadWrite);
        if(rc)
        {
            QDomDocument DomDoc;
            rc = DomDoc.setContent(&xmlFileObj);
            if(rc)
            {

                xmlFileObj.close();
                QFile xmlNewFileObj(scxmlToolIDPath);
                bool rC = xmlNewFileObj.open(QFile::Text | QFile::WriteOnly | QFile::Truncate);
                if(rC)
                {
                    QDomElement rootElement = DomDoc.documentElement();
                    QDomNodeList nodeList = rootElement.childNodes();
                    for(int i = 0; i < nodeList.count(); i++)
                    {
                        QDomNode node = nodeList.at(i);
                        if(node.nodeName() == "tool")
                        {
                            QString removeName = iRemoveObj.getId();
                            QString toolName = node.attributes().namedItem("id").nodeValue();
                            if(toolName == removeName)
                            {
                                rootElement.removeChild(node);
                                break;
                            }
                        }
                    }

                    QTextStream stream(&xmlNewFileObj);
                    stream << DomDoc.toString();
                    xmlNewFileObj.close();
                    _ToolIDList = iUpdatedList;
                }
            }
        }
    }
}

const QString GTAAppController::getDatabaseVersion() 
{
    QString dbFilePath = _pSystemServices->getDatabaseFile();
    GTADataController DataCtrl(dbFilePath);
    QString version = DataCtrl.getDatabaseVersion();
    return version;
}
void GTAAppController::getInternalVariablesForCmd(const QString &)
{
    // implement this from cmdXML
}

bool GTAAppController::parseGenericDBFile(QList<GTAGenericDB> &oUserDefDb)
{
    bool rc = false;

    if(_pSystemServices)
    {
        QString genericDBFilePath = _pSystemServices->getGenericDBPath();
        QDir dir(genericDBFilePath);

        if(dir.exists())
        {
            QString configFile = QDir::cleanPath(QString("%1%2%3").arg(genericDBFilePath,QDir::separator(),"Generic DB.xml"));
            QFile xmlFile(configFile);
            if(xmlFile.exists())
            {
                bool fileCheck = xmlFile.open(QFile::ReadOnly | QFile::Text);
                QDomDocument * pDomDoc = NULL;
                if (fileCheck)
                {
                    pDomDoc = new QDomDocument();
                    if(pDomDoc != NULL)
                    {
                        if( ! pDomDoc->setContent(&xmlFile))
                        {
                            delete pDomDoc;
                            pDomDoc = NULL;
                            return rc;

                        }

                        QDomNodeList lstNodeDb = pDomDoc->elementsByTagName("DB");
                        for(int i = 0; i < lstNodeDb.count(); i++)
                        {
                            QDomNode nodeDb = lstNodeDb.at(i);
                            QDomNamedNodeMap attrNodeDb = nodeDb.attributes();
                            QString name = attrNodeDb.namedItem("name").nodeValue();
                            GTAGenericDB genDbObj;
                            genDbObj.setName(name);
                            QString path = attrNodeDb.namedItem("path").nodeValue();
                            genDbObj.setPath(path);
                            QString relativePath = attrNodeDb.namedItem("relativePath").nodeValue();
                            genDbObj.setRelativePath(relativePath);

                            QString delimiter = attrNodeDb.namedItem("delimiter").nodeValue();
                            genDbObj.setDelimiter(delimiter);

                            QDomNode attributeNode = nodeDb.childNodes().at(0);
                            if(attributeNode.nodeName() == "attributes")
                            {
                                QList<int> colsConcatList;
                                QString cols = attributeNode.attributes().namedItem("columnToReadFromSearchResult").nodeValue();
                                QStringList temp = cols.split(";",QString::SkipEmptyParts);
                                for(int x = 0; x < temp.count(); x++)
                                {
                                    int val = temp.at(x).toInt();
                                    colsConcatList.append(val);
                                }
                                genDbObj.setColsConcatList(colsConcatList);

                                QString concatStr = attributeNode.attributes().namedItem("concatString").nodeValue();
                                genDbObj.setConcatString(concatStr);

                                QDomNodeList lstAttributes = attributeNode.childNodes();
                                for(int j = 0; j < lstAttributes.count(); j++)
                                {
                                    QDomNode attribute = lstAttributes.at(j);
                                    QString id = attribute.attributes().namedItem("id").nodeValue();
                                    QString attrName = attribute.attributes().namedItem("name").nodeValue();
                                    QString column = attribute.attributes().namedItem("column").nodeValue();
                                    genDbObj.addAttribute(id.toInt(),attrName, column.toInt());
                                }
                            }
                            oUserDefDb.append(genDbObj);
                        }
                    }
                }
                xmlFile.close();
            }
        }

        if(oUserDefDb.count())
            rc = true;
    }
    return rc;
}

bool GTAAppController::parseInternalParamInfoXml()
{
    bool rc = false;

    if(_pSystemServices)
    {
        QString internalParamInfoFilePath = QDir::cleanPath(_pSystemServices->getInternalParamInfoFilePath());
        QFile xmlFile(internalParamInfoFilePath);
        if(xmlFile.exists())
        {
            bool fileCheck = xmlFile.open(QFile::ReadOnly | QFile::Text);
            QDomDocument * pDomDoc = NULL;
            if (fileCheck)
            {
                pDomDoc = new QDomDocument();
                if(pDomDoc != NULL)
                {
                    if( ! pDomDoc->setContent(&xmlFile))
                    {
                        delete pDomDoc;
                        pDomDoc = NULL;
                        return rc;

                    }
                }


                QDomNodeList rootComplexTypesLst = pDomDoc->elementsByTagName("ComplexTypes");
                if(rootComplexTypesLst.count())
                {
                    _ComplexParameterTypeMap.clear();
                    QDomNodeList complexTypesLst = rootComplexTypesLst.at(0).childNodes();
                    for(int i = 0; i < complexTypesLst.count(); i++)
                    {
                        QDomNode complexTypeNode = complexTypesLst.at(i);
                        QString complexTypeName = complexTypeNode.attributes().namedItem("name").nodeValue();

                        QDomNodeList complexItemLst = complexTypeNode.childNodes();
                        for(int j = 0; j < complexItemLst.count(); j++)
                        {
                            ComplexItemType item;
                            QDomNode complexItem = complexItemLst.at(j);
                            item.name = complexItem.attributes().namedItem("name").nodeValue();
                            item.type = complexItem.attributes().namedItem("type").nodeValue();
                            _ComplexParameterTypeMap.insertMulti(complexTypeName,item);
                        }

                    }
                }
                QStringList complexTypes;
                if(_ComplexParameterTypeMap.count())
                {
                    complexTypes = _ComplexParameterTypeMap.keys();
                    complexTypes.removeDuplicates();
                }
                _InternalParamInfoLst.clear();
                QDomNodeList rootCommandList = pDomDoc->elementsByTagName("CommandList");
                if(rootCommandList.count())
                {
                    QDomNodeList commandLst = rootCommandList.at(0).childNodes();
                    for(int i = 0; i < commandLst.count(); i++)
                    {

                        QDomNode commandNode = commandLst.at(i);
                        CommandInternalParameterInfo command;
                        command.commandName = commandNode.attributes().namedItem("name").nodeValue();
                        command.complement = commandNode.attributes().namedItem("complement").nodeValue();

                        QDomNodeList commandNodeChildren = commandNode.childNodes();
                        QList<InternalParameterInfo> internalParamLst;
                        ParameterReturnInfo commandReturnType;
                        for(int j = 0; j < commandNodeChildren.count(); j++)
                        {
                            QDomNode childNode = commandNodeChildren.at(j);
                            if(childNode.nodeName() == "internalParameter")
                            {

                                QString paramType = childNode.attributes().namedItem("type").nodeValue();
                                QString name = childNode.attributes().namedItem("name").nodeValue();
                                resolveInternalParamComplexTypes(complexTypes,paramType,name,internalParamLst);
                                command.internalParamInfoLst.append(internalParamLst);

                            }
                            else if(childNode.nodeName() == "return")
                            {
                                commandReturnType.name = childNode.attributes().namedItem("name").nodeValue();
                                commandReturnType.returnType = childNode.attributes().namedItem("type").nodeValue();
                                command.commandReturnType = commandReturnType;
                            }
                        }

                        _InternalParamInfoLst.append(command);
                    }

                }

                delete pDomDoc;
                pDomDoc = NULL;


            }
            xmlFile.close();
            rc = true;
        }

    }
    return rc;
}


void GTAAppController::resolveInternalParamComplexTypes(const QStringList &iComplexTypes, const QString &iParamType, const QString &iParamName, QList<InternalParameterInfo> &oInternalParamLst)
{
    if(iComplexTypes.contains(iParamType))
    {

        QString name = iParamName;
        QList<ComplexItemType> complexItemsLst = _ComplexParameterTypeMap.values(iParamType);
        for(int k = 0; k < complexItemsLst.count(); k++)
        {
            InternalParameterInfo parameterInfo;
            ComplexItemType complexItem = complexItemsLst.at(k);
            QString itemName = complexItem.name;
            QString itemType = complexItem.type;
            name = QString("%1.%2").arg(iParamName,itemName);
            parameterInfo.paramName = name;
            parameterInfo.paramType = itemType;

            if(iComplexTypes.contains(itemType))
            {
                resolveInternalParamComplexTypes(iComplexTypes,itemType,name,oInternalParamLst);
            }
            else
            {
                oInternalParamLst.append(parameterInfo);
            }

        }

    }
    else
    {
        InternalParameterInfo parameterInfo;
        parameterInfo.paramName = iParamName;
        parameterInfo.paramType = iParamType;
        oInternalParamLst.append(parameterInfo);
    }
}

QFileInfoList GTAAppController::getFileListFromDir(const QString &iRootDirPath,bool isRecursive)
{
    QFileInfoList fileList;
    QDir dir(iRootDirPath);
    if(isRecursive)
    {

        //QStringList filters;
        //filters << "*.seq" << "*.pro" << "*.fun" << "*.obj";
        dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Dirs);
        dir.setSorting(QDir::Name | QDir::DirsFirst);
        // dir.setNameFilters(filters);
        QFileInfoList AllfilesList = dir.entryInfoList();
        foreach(QFileInfo file, AllfilesList)
        {
            if(file.isFile())
            {
                QString fileName = file.fileName();
                if(fileName.endsWith(".pro") || fileName.endsWith(".seq") || fileName.endsWith(".fun")
                        ||fileName.endsWith(".obj"))
                {
                    fileList.append(file);

                }
            }
            else if(file.isDir())
            {
                fileList.append(getFileListFromDir(file.absoluteFilePath(),isRecursive));
            }
        }
    }
    else
    {
        QStringList filters;
        filters << "*.seq" << "*.pro" << "*.fun" << "*.obj";
        dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);
        dir.setSorting(QDir::Name);
        dir.setNameFilters(filters);
        fileList = dir.entryInfoList();
    }
    return fileList;
}

void GTAAppController::resolveSameUUID(const QFileInfoList fileLst)
{

    _UUIDFileNameHash.clear();
    if(fileLst.count() > 0)
    {

        //      adding UUID and corresponding absolute file path in a hash
        GTADocumentController docCtrl;
        foreach(QFileInfo file, fileLst)
        {
            QString absFilePath = file.absoluteFilePath();
            QString uuid;
            docCtrl.getDocumentInfo(absFilePath,uuid);
            _UUIDFileNameHash.insertMulti(uuid,absFilePath);

        }

        // removing files with unique UUIDs and retaining the ones with non unique UUIDs
        QStringList keys = _UUIDFileNameHash.uniqueKeys();
        foreach(QString key, keys)
        {
            QStringList values = _UUIDFileNameHash.values(key);
            if(values.count() > 1)
            {
                foreach(QString value, values)
                {

                    QString newUuid = QUuid::createUuid().toString();
                    docCtrl.setDocumentUUID(QDir::cleanPath(value),newUuid);
                }
            }
            else if(values.count() == 1)
            {
                _UUIDFileNameHash.remove(key);
            }
        }
    }

}


QString GTAAppController::portFiles(const QString &iDirPath, bool isRecursive)
{
    QDir dir(iDirPath);
    QFileInfoList fileLst;
	QFileInfoList fileLstToEdit;
    QStringList errorList;
    if(dir.exists())
    {
        //getting all the files from the target folder
        fileLst = getFileListFromDir(iDirPath,isRecursive);
		for (int i = 0; i < fileLst.length(); i++) {
			if (fileLst[i].fileName().endsWith(".pro") || fileLst[i].fileName().endsWith(".seq")) {
				fileLstToEdit.append(fileLst[i]);
			}
		}

        //resolving non unique UUIds
        resolveSameUUID(fileLstToEdit);

        _callResolutionHash.clear();
        // resolving call references
        foreach(QFileInfo file, fileLstToEdit)
        {
            QString absoluteFilePath = file.absoluteFilePath();
            GTAElement *pElement = NULL;
            GTAHeader *pHeader = NULL;
            GTADocumentController docCtrl;
            bool rc = docCtrl.openDocument(absoluteFilePath,pElement,pHeader,false);
            if(pElement != NULL && pHeader != NULL)
                pElement->setHeader(pHeader);
            if(rc && pElement != NULL)
            {
                errorList.append(portElement(pElement,iDirPath,absoluteFilePath));
                if(pElement)
                {
                    QString headerName;
                    if(pHeader)
                        headerName = pHeader->getName();
                    docCtrl.saveDocument(pElement,headerName,absoluteFilePath,true);
                    //TODO: Add fail handling for Save Document
                }
            }
            else
            {
                QString openError = docCtrl.getLastError();
                errorList.append(openError);
            }

        }

        errorList.append(QString("Porting Completed!!!"));
        QDateTime time = QDateTime::currentDateTime();
        QDate date = time.date();
        QString sDate = date.toString(QString("dd.MM.yyyy"));
        QTime logTime = time.time();
        QString sTime = logTime.toString(QString("hh:mm:ss"));
        sTime.replace(":","-");
        QString logFileName  = QString("PortingLog_%1_%2.txt").arg(sDate,sTime);

        QString logFilePath = QDir::cleanPath(QString("%1/%2").arg(iDirPath,logFileName));
        QFile logFile(logFilePath);

        bool frc = logFile.open(QIODevice::WriteOnly | QIODevice::Text);
        if(frc)
        {
            QTextStream stream(&logFile);
            QString errorContents = errorList.join("\n");
            stream <<errorContents;
            logFile.close();
        }

    }
    return iDirPath;

}


QStringList GTAAppController::portElement(GTAElement *&pElement, const QString & iRootDirPath, const QString &iElementAbsPath)
{
    QStringList logList;
    if(pElement)
    {
        GTADocumentController docCtrl;
        int childCount = pElement->getAllChildrenCount();
        for(int i = 0; i < childCount ; i++)
        {
            GTACommandBase * pBase = NULL;
            pElement->getCommand(i,pBase);

            if(pBase != NULL)
            {

                if (pBase->hasReference())
                {
                    GTAActionCall *pCall = dynamic_cast<GTAActionCall*>(pBase);
                    if(pCall != NULL)
                    {
                        QString refID = pCall->getRefrenceFileUUID();
                        QString absFilePath = getAbsoluteFilePathFromUUID(refID);
                        QFile file(QDir::cleanPath(absFilePath));
                        if(!file.exists()) // || absFilePath.isEmpty())
                        {
                            //                    if(refID.isEmpty())
                            //                    {
                            QString callElem = pCall->getElement();
                            QString callElemAbsPath = QDir::cleanPath(QString("%1/%2").arg(iRootDirPath,callElem));
                            QString docID;
                            if(_callResolutionHash.contains(callElemAbsPath))
                            {
                                docID = _callResolutionHash.value(callElemAbsPath);
                            }
                            else
                            {
                                docCtrl.getDocumentInfo(callElemAbsPath,docID);
                                _callResolutionHash.insert(callElemAbsPath,docID);
                            }

                            if(!docID.isEmpty())
                            {

                                pCall->setRefrenceFileUUID(docID);
                                GTACommandBase *pCloneCall = pCall->getClone();
                                if(pCloneCall)
                                {
                                    pElement->updateCommand(i,pCloneCall);
                                }
                            }
                            else
                            {
                                QString lineNum = QString::number(i);
                                QString errorMsg = QString("Unresolved call reference in : %1 : at line %2 : %3 not found").arg(iElementAbsPath,lineNum,callElemAbsPath);
                                logList.append(errorMsg);
                            }

                        }
                    }
                }
                else
                {

                    GTAActionManual * pManualAct = dynamic_cast<GTAActionManual*>(pBase);
                    if(pManualAct)
                    {
                        QString htmlMsg = pManualAct->getHtmlMessage();
                        QString message = pManualAct->getMessage();
                        if(htmlMsg.isEmpty() && !message.isEmpty())
                            pManualAct->setHTMLMessage(message);
                    }
                }

            }
        }
    }
    return logList;
}

void GTAAppController::deleteSession(const QStringList &iSelectedItems)
{

    GTADataController db;
    db.deleteSession(iSelectedItems);
}

void GTAAppController::saveSession(const QString &iSession,const QStringList &iFileLst)
{
    GTADataController dbCtrl;
    dbCtrl.saveSession(iSession,iFileLst);
}

void GTAAppController::addNewSession(const QString &iSession,const QStringList &iFileLst)
{

    GTADataController db;
    db.addNewSession(iSession,iFileLst);

}

QStringList GTAAppController::getSessionNames()
{
    GTADataController dbCtrl;
    return dbCtrl.getSessionNames();
}

QStringList GTAAppController::getSessionFiles(const QString &iSessionName)
{
    GTADataController dbCtrl;
    return dbCtrl.getSessionFiles(iSessionName);
}

bool GTAAppController::readSessionInfoFile(QString &oLastSessionName, QStringList &oRecentSessionLst)
{
    bool rc = false;
    if(_pSystemServices != NULL)
    {
        QString sessionFilePath = _pSystemServices->getSessionInfoFilePath();
        if(!sessionFilePath.isEmpty())
        {
            QFile xmlFile(sessionFilePath);
            if(xmlFile.exists())
            {
                bool fileCheck = xmlFile.open(QFile::ReadOnly | QFile::Text);
                QDomDocument * pDomDoc = NULL;
                if (fileCheck)
                {
                    pDomDoc = new QDomDocument();
                    if(pDomDoc != NULL)
                    {
                        if( ! pDomDoc->setContent(&xmlFile))
                        {
                            delete pDomDoc;
                            pDomDoc = NULL;
                            return rc;

                        }
                        QDomNodeList lastSessionNodeLst = pDomDoc->elementsByTagName("LastSession");
                        QDomNode lastSessionNode;

                        if(lastSessionNodeLst.count())
                        {
                            lastSessionNode = lastSessionNodeLst.at(0);
                            oLastSessionName = lastSessionNode.attributes().namedItem("name").nodeValue();
                        }

                        QDomNodeList recentSessionNodeLst = pDomDoc->elementsByTagName("RecentSessions");
                        if(recentSessionNodeLst.count())
                        {
                            QDomNodeList sessionNodeLst = recentSessionNodeLst.at(0).childNodes();
                            for(int i = 0; i < sessionNodeLst.count();i++)
                            {
                                QDomNode recentSessionNode = sessionNodeLst.at(i);
                                oRecentSessionLst << recentSessionNode.attributes().namedItem("name").nodeValue();
                            }
                        }

                        delete pDomDoc;
                        pDomDoc = NULL;

                    }
                }
            }
            xmlFile.close();
            rc = true;
        }
    }
    return rc;
}



void GTAAppController::writeSessionInfoFile(const QStringList &iRecentSessionLst)
{
    if(iRecentSessionLst.count() && _pSystemServices != NULL)
    {

        QString sessionFilePath = _pSystemServices->getSessionInfoFilePath();
        QFile xmlFileObj(QDir::cleanPath(sessionFilePath));
        bool rc = xmlFileObj.open(QIODevice::Text| QIODevice::ReadWrite);
        if(rc)
        {
            QDomDocument DomDoc;
            rc = DomDoc.setContent(&xmlFileObj);
            if(rc)
            {
                xmlFileObj.close();
                QFile xmlNewFileObj(sessionFilePath);
                bool rC = xmlNewFileObj.open(QIODevice::Text | QIODevice::ReadWrite | QIODevice::Truncate);
                if(rC)
                {

                    DomDoc.clear();
                    QDomElement rootElement = DomDoc.createElement("GTASessionInfo");
                    DomDoc.appendChild(rootElement);

                    QString lastSession = iRecentSessionLst.first();

                    QDomElement lastSessionElem = DomDoc.createElement("LastSession");
                    lastSessionElem.setAttribute("name",lastSession);
                    rootElement.appendChild(lastSessionElem);

                    QDomElement recentSessionElem = DomDoc.createElement("RecentSessions");
                    rootElement.appendChild(recentSessionElem);

                    foreach(QString sessionName, iRecentSessionLst)
                    {
                        QDomElement sessionElem = DomDoc.createElement("Session");
                        sessionElem.setAttribute("name",sessionName);
                        recentSessionElem.appendChild(sessionElem);
                    }

                    QTextStream stream(&xmlNewFileObj);
                    QString temp = DomDoc.toString();
                    stream << DomDoc.toString();
                    //                    stream << temp;
                    xmlNewFileObj.close();

                }
            }

        }

    }
}

// Dev Comment : Title based execution backup
//QStringList GTAAppController::getAllDocsUsedInElement(const QString &iUUID)
//{
//    QStringList uuids;
//    GTAElement *pElement = NULL;
//    bool rc = getElementFromDocument(iUUID,pElement,true,false);
//    if(rc && (pElement != NULL))
//    {
//        int count = pElement->getDirectChildrenCount();
//        for(int i = 0; i < count; i++)
//        {
//            GTACommandBase *pCmd = NULL;
//            pElement->getCommand(i,pCmd);
//            if(pCmd && pCmd->hasReference())
//            {
//                GTAActionCall *pCall = dynamic_cast<GTAActionCall*>(pCmd);
//                if(pCall)
//                {
//                    //DO we need to consider cases when uuid is not there ? lets think
//                    QString uuid = pCall->getRefrenceFileUUID();
//                    uuids << uuid;

//                }
//            }
//        }
//    }
//    return uuids;
//}

/**
* @brief Updates the work db and the tree view
* @param None
* @return bool true if the updating went fine
*/
bool GTAAppController::updateWorkDB()
{
	bool rc = false;
	QString dataDirectoryPath = _pSystemServices->getDataDirectory();
	updateTreeStructureDB(_treeRootNode, dataDirectoryPath);
	if (_treeRootNode == NULL)
	{
		QString root = "root";
		_treeRootNode = new GTATreeItem(root, root, root, root, root);
	}
	if (!_DataTreeMap.isEmpty())
	{
		_DataTreeMap.clear();
		_DataTreeMap.insert(dataDirectoryPath, _treeRootNode);
	}
	_FileWatcherList.clear();
	rc = storeTreeStructure(dataDirectoryPath, _treeRootNode);
	return rc;
}

void GTAAppController::importFilesToWorkDB(QStringList pathList, QString parent)
{
	_duplicateFiles.clear();
	QString dataDirectoryPath = _pSystemServices->getDataDirectory();
	if (!parent.isEmpty())
		dataDirectoryPath.append(parent);
	foreach(QString path, pathList)
	{
		QFile file(path);
		QFileInfo fileInfo(path);
		if (file.exists())
		{
			QString filename(fileInfo.fileName());
			if (QFile::exists(dataDirectoryPath + "/" + filename))
			{
				_duplicateFiles.insert(path,dataDirectoryPath + "/" + filename);
			}
			else if (!file.copy(dataDirectoryPath + "/" + filename)) 
			{
				QString toto = file.error();
			}
		}
	}
	if (!_duplicateFiles.isEmpty())
	{
		emit onImportAlreadyExists(_duplicateFiles);
	}
} 

void GTAAppController::importFolderToWorkDB(QString source, QString destination)
{
	bool refresh = false;
	QDir dir(source);

	if (!dir.exists())
		return;
	if (destination.isEmpty())
	{
		_duplicateFiles.clear();
		refresh = true;
		destination = _pSystemServices->getDataDirectory() + QString("/") + dir.dirName();
	}
	dir.mkpath(destination);
	foreach(QString d, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
		QString newdest = destination + QString("/") + d;

		QString newsource = source + QString("/") + d;

		importFolderToWorkDB(newsource, newdest);
	}

	foreach(QString path, dir.entryList(QDir::Files))
	{
		QFile file(source + "/" + path);
		QFileInfo fileInfo(source + "/" + path);
		QString fileName = file.fileName();
		QString extension;
		if (fileName.endsWith(".pro"))
		{
			extension = QString("pro");
		}
		else if (fileName.endsWith(".seq"))
		{
			extension = QString("seq");
		}
		else if (fileName.endsWith(".fun"))
		{
			extension = QString("fun");
		}
		else if (fileName.endsWith(".obj"))
		{
			extension = QString("obj");
		}
		else
		{
			continue;
		}
		if (file.exists())
		{
			QString filename(fileInfo.fileName());
			if (QFile::exists(destination + "/" + filename))
			{
				_duplicateFiles.insert(source + "/" + path,destination + "/" + filename);
			}
			else if (!file.copy(destination + "/" + filename)) {
				QString toto = file.error();
			}
		}
	}
	if (!_duplicateFiles.isEmpty())
	{
		emit onImportAlreadyExists(_duplicateFiles);
	}
}

/**
* @brief Rebuilds the database [DEPRECATED]
* @return bool true if the update went fine
*/
bool GTAAppController::rebuildDB()
{
	QString dataDirectoryPath = _pSystemServices->getDataDirectory();
	_dbDataController->deleteDb(dataDirectoryPath);
	_treeRootNode->clearChildren();
	_dbDataController->openDb(dataDirectoryPath);
	return updateWorkDB();
}

/**
* @brief Deletes a file in the system
* @param path the path of the file to remove
* @return void
*/
void GTAAppController::deleteFile(QString path)
{
	QFile file( _pSystemServices->getDataDirectory() + path);
    file.remove();
}

void GTAAppController::deleteAndImport(QHash <QString, QString> duplicatesToReplace)
{
	QHash <QString, QString>::iterator i;
	for (i = duplicatesToReplace.begin(); i != duplicatesToReplace.end(); ++i)
	{
		QFile toRemove(i.value());
		if (toRemove.exists())
		{
			toRemove.remove();
		}
		QFile toCopy(i.key());
		toCopy.copy(i.value());
	}
}

/**
* @brief Detects whether there is a duplicate file ith respect to its UUID
* @param oldPath the old file with the same UUID
* @param newPath the new file with the same UUID
* @return void
*/
void GTAAppController::onDuplicateUUIDFound(QString oldFile, QString newFile)
{
	QString DataDirPath = _pSystemServices->getDataDirectory();
	QFile oldPath(DataDirPath + QDir::separator() + oldFile);
	QFile newPath(DataDirPath + QDir::separator() + newFile);
	bool canEmitOld = true, canEmitNew = true;
	if (!oldPath.exists())
	{
		_dbDataController->deleteFile(oldFile);
		canEmitOld = false;
	}
	if (!newPath.exists())
	{
		_dbDataController->deleteFile(newFile);
		canEmitNew = false;
	}
	if (canEmitOld && canEmitNew)
		emit onDuplicateUUID(oldFile, newFile);
}

/**
 * @brief This method deals with moving files in the system arborescence
 * @param selectedItems The selected items in the list of files / folders to move
 * @param dir The directory where to move the files
 * @param isRoot
 * @return 
 */
void GTAAppController::moveFiles(QList<GTATreeItem *> selectedItems, QString dir)
{
	foreach(GTATreeItem * pItem, selectedItems)
	{
		QString absRelPath = pItem->getAbsoluteRelPath();
		QString DataDirPath = _pSystemServices->getDataDirectory();
		if (pItem->getExtension() == "folder")
		{
			QDir folder(DataDirPath + absRelPath);
			folder.mkpath(dir + QString("/") + pItem->getName());
			QString newRoot = dir.replace(DataDirPath, QString());
			_dbDataController->updateFolderRelPath(QString(dir + QString("/") + pItem->getName()), newRoot, pItem->getName());
			QList<GTATreeItem *> childrenList = pItem->getChildren(); 
			moveFiles(childrenList, QString(DataDirPath + dir + QString("/") + pItem->getName()));
			folder.rmdir(DataDirPath + absRelPath);
		}
		else
		{
			QFile toMove(DataDirPath + absRelPath);
			if (toMove.exists())
			{
                toMove.rename(dir + QString("/") + pItem->getName());
                QString temp = QString(dir); //< Using a temp variable to avoid modifying dir, which "replace" does.
                QString newRoot = temp.replace(DataDirPath, QString());
                _dbDataController->updateRelPath(QString(newRoot + "/" + pItem->getName()), newRoot, pItem->getName());
			}
		}
	}
}

QString GTAAppController::getExtensionFromTreeModel(const QModelIndex &iIndex)
{
	if (_pViewController)
	{
		QSortFilterProxyModel *treeViewModel1 = (QSortFilterProxyModel *)iIndex.model();
		if (treeViewModel1 != NULL)
		{
			GTATreeViewModel * treeViewModel = (GTATreeViewModel*)treeViewModel1->sourceModel();
			if (treeViewModel != NULL)
			{
				QModelIndex currIndex = treeViewModel1->mapToSource(iIndex);
				QModelIndex pParentIndex = currIndex.parent();
				if (!pParentIndex.isValid())
				{
					pParentIndex = QModelIndex();
				}
				QModelIndex pIndex = treeViewModel->index(currIndex.row(), 0, pParentIndex);
				QModelIndex pIndex1 = treeViewModel->index(currIndex.row(), 3, pParentIndex);
				QString extension;
				if (pIndex.isValid())
				{
					return treeViewModel->getExtensionForIndex(pIndex);
				}			
			}
		}
	}
	return QString();
}

/**
 * @brief: This method sets the unsubscribeFileParamOnly value which will be used to create scxml
 * @param: unsubscribeFileParamOnly : if true then only File params will be unsubscribed
 */
void GTAAppController::setUnsubscribeFileParamOnly(const bool unsubscribeFileParamOnly)
{
    _unsubscibeFileParamOnly = unsubscribeFileParamOnly;
}

/**
 * @brief Read all version write in the output file created by the script SCRIPT_VERSION_TOOLS.
 * 
 * @return Liste of version write in this file. 
 */
void GTAAppController::parserBenchVersionsFile(QString& full_infos) {
    QString line = "";
    QFile file(QCoreApplication::applicationDirPath() + QDir::separator() + FILE_VERSIONS_TOOLS_BENCH);
    if (file.exists()){
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            while (!stream.atEnd()) {
                line = stream.readLine();
                full_infos += line + "\n";
            }
        }
        file.close();
    }
}

/**
 * @brief Launch the script SCRIPT_VERSION_TOOLS witch parse all folder listed in a file named path_file.txt in ToolsData folder and note version in the output file.
 * 
 * @return boolean to indicate if output file is correct created or not. 
 */
bool GTAAppController::launchScriptBenchVersionsFile() const{
    QProcess::execute(QCoreApplication::applicationDirPath() + QDir::separator() + QString(SCRIPT_VERSION_TOOLS) + " -i " + GTAUtil::getToolDataPath() + " -o " + QCoreApplication::applicationDirPath());
    QFileInfo fileInformation(QCoreApplication::applicationDirPath() + QDir::separator() + FILE_VERSIONS_TOOLS_BENCH);
    return fileInformation.exists();
}
