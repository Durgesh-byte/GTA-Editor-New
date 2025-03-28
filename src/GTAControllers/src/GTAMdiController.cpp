#include "GTAMdiController.h"
#include "GTAElement.h"
#include "GTACommandBase.h"
#include "GTAElementViewModel.h"
#include "GTASystemServices.h"
//#include "GTAEditorViewController.h"


GTAMdiController *GTAMdiController::_pMDIController = nullptr;

GTAMdiController::GTAMdiController()
{
    _pViewController = GTAViewController::getGTAViewController(this);
    _pActiveViewController = nullptr;
}

GTAMdiController* GTAMdiController::getMDIController(QObject *)
{
    if(nullptr == _pMDIController)
    {
        _pMDIController = new GTAMdiController();
    }
    //else
    //{
        return _pMDIController;
    //}
}



bool GTAMdiController::getClipboardModel(QAbstractItemModel*& pModel)
{
    bool rc = false;

    if (!_lstOfCopiedItems.isEmpty())
    {
        int noOfCopiedItems = _lstOfCopiedItems.size();
        GTAElement* pElement = new GTAElement(GTAElement::CUSTOM);
        int cmdCnt=0;
        for (int i=0;i<noOfCopiedItems;i++)
        {
            GTACommandBase* pMasterCmd = _lstOfCopiedItems.at(i);
            if (nullptr!=pMasterCmd)
            {
                pElement->insertCommand(pMasterCmd,cmdCnt,false);
                int childrenCnt = pMasterCmd->getAllChildrenCount();
                if (childrenCnt)
                    cmdCnt = cmdCnt+childrenCnt;

                cmdCnt++;
            }
        }

        if (pElement)
        {
            pModel = new GTAElementViewModel(pElement);
            rc = true;

        }
    }
    else
        _LastError = "Noting copied[Clipboard Empty]";
    return rc;
}

void GTAMdiController::onSetCopiedItem(GTACommandBase* selectedItem)
{
    _lstOfCopiedItems.append(selectedItem);
}

void GTAMdiController::onSetCopiedItems(QList<GTACommandBase*> lstSelectedRows)
{
    onClearClipBoardItems();
    _lstOfCopiedItems = lstSelectedRows;
}

void GTAMdiController::onGetCopiedItems(QList<GTACommandBase*> &oCopiedItems)
{
    for(int i = 0; i < _lstOfCopiedItems.count(); i++)
    {
        GTACommandBase * pCmd = _lstOfCopiedItems.at(i);
		
        GTACommandBase * pClone = pCmd == NULL ? pCmd : pCmd->getClone();
        oCopiedItems.append(pClone);
    }

}


void GTAMdiController::onClearClipBoardItems()
{
    foreach(GTACommandBase* pCmd,_lstOfCopiedItems)
    {
        if (pCmd!=nullptr)
        {
            delete pCmd;
            pCmd  = nullptr;
        }
    }

    _lstOfCopiedItems.clear();
}


bool GTAMdiController::getModelForTreeView( QAbstractItemModel*& pModel ,const int& iRow, GTAElement *pElement)
{
    bool rc = false;

    if (_pViewController)
    {
        rc = _pViewController->getModelForTreeView(pModel ,iRow, pElement);
        if (!rc)
            _LastError =  _pViewController->getLastError();
    }

    return rc;

}

QString GTAMdiController::getLastError()
{
    return _LastError;
}


bool GTAMdiController::saveAsDocument(const QString &)
{
    return true;
}

bool GTAMdiController::saveDocument(const QString & iName, bool iOverWrite,GTAElement * ipElement, GTAHeader* ipHeader,const QString &iDataDirectory)
{
    bool rc = false;
    GTAAppController *pAppCtrl = GTAAppController::getGTAAppController();
    if(pAppCtrl)
    {
        rc = pAppCtrl->saveDocument(iName,iOverWrite,ipElement,ipHeader,iDataDirectory);
        if(!rc)
        {
            _LastError = pAppCtrl->getLastError();
        }
    }
    return rc;
}

void GTAMdiController::onGetHeaderTemplateFilePath(QString &oTemplFilePath)
{

    GTAAppController *pAppCtrl = GTAAppController::getGTAAppController();
    if(pAppCtrl)
    {
        GTASystemServices* pSystemServices = pAppCtrl->getSystemService();
        if(pSystemServices)
        {
            oTemplFilePath = pSystemServices->getHeaderTemplateFilePath();
        }
    }
}

bool GTAMdiController::headerExists(const QString& iName)const
{
    bool rc = false;
    GTAAppController *pAppCtrl = GTAAppController::getGTAAppController();
    if(pAppCtrl)
    {
        rc = pAppCtrl->headerExists(iName);
    }
    return rc;
}


void GTAMdiController::onGetGTADataDirectory(QString &oDirPath)
{
    GTAAppController *pAppCtrl = GTAAppController::getGTAAppController();
    if(pAppCtrl)
    {
        oDirPath = pAppCtrl->getGTADataDirectory();
    }
}

/**
 * @brief Triggers the export of the SCXML for the select element
 * @param pElement Current procedure element
 * @param fileName Name of the current document
 * @param FilePath Path of the current document
 * @param iHasUnsubscibeAtStart
 * @param iHasUnsubscibeAtEnd
 * @return bool True if everything went well
 */
bool GTAMdiController::exportToScxml(GTAElement* pElement, const QString& fileName, const QString& iFilePath, bool iHasUnsubscibeAtStart, bool iHasUnsubscibeAtEnd)
{
    bool rc = false;
    GTAAppController *pAppCtrl = GTAAppController::getGTAAppController();
    if(pAppCtrl)
    {
        QStringList varList;
        pAppCtrl->setUnsubscribeFileParamOnly(getUnsubscribeFileParamOnly());
        rc = pAppCtrl->exportToScxml(pElement,fileName,iFilePath,iHasUnsubscibeAtStart,iHasUnsubscibeAtEnd,varList);
        if(!rc)
        {
            _LastError = pAppCtrl->getLastError();
        }
    }
    return rc;
}


bool GTAMdiController::getFileInfoListUsed(const QString& iFileName, QList <GTAFILEInfo> & lstFileInfo)
{
    bool rc = false;
    GTAAppController *pAppCtrl = GTAAppController::getGTAAppController();
    if(pAppCtrl)
    {
        rc = pAppCtrl->getFileInfoListUsed(iFileName, lstFileInfo);
        if(!rc)
        {
            _LastError = pAppCtrl->getLastError();
        }
    }
    return rc;
}

//void GTAMdiController::onCallActionDropped(int& iRow, QString& iFileName)
//{
//    emit sigCallActionDropped(iRow,iFileName);
//}

QHash<QString,QStringList> GTAMdiController::getActionCommandList(GTAUtil::ElementType iType,QStringList& orderList )
{
    QHash<QString,QStringList> oCommandList;
    GTAAppController *pAppCtrl = GTAAppController::getGTAAppController();
    if(pAppCtrl)
    {
        oCommandList = pAppCtrl->getActionCommandList(iType,orderList);
    }
    return oCommandList;
}

QList<QString> GTAMdiController::getCheckCommandList(GTAUtil::ElementType iType)
{
    QList<QString> oCheckCommandList;
    GTAAppController *pAppCtrl = GTAAppController::getGTAAppController();
    if(pAppCtrl)
    {
        oCheckCommandList = pAppCtrl->getCheckCommandList(iType);
    }
    return oCheckCommandList;
}


void GTAMdiController::onUpdateActiveViewController(GTAEditorViewController * pViewController)
{
    _pActiveViewController = pViewController;
}

GTAEditorViewController * GTAMdiController::getActiveViewController()const
{
    return _pActiveViewController;
}


ErrorMessageList GTAMdiController::CheckCompatibility()
{
    ErrorMessageList ErrorList;
    GTAAppController *pAppCtrl = GTAAppController::getGTAAppController();
    if(pAppCtrl)
    {
        ErrorList = pAppCtrl->CheckCompatibility();
    }
    return ErrorList;
}

bool GTAMdiController::getElementFromDocument(const QString & iFileName, GTAElement *& opElement, bool isUUID)
{
    bool rc = false;
    GTAAppController *pAppCtrl = GTAAppController::getGTAAppController();
    if(pAppCtrl)
    {
        rc = pAppCtrl->getElementFromDocument(iFileName,opElement,isUUID);
    }
    return rc;
}


QString GTAMdiController::getFilePathFromTreeModel(const QModelIndex &iIndex)
{
    QString path;
    GTAAppController *pAppCtrl = GTAAppController::getGTAAppController();
    if(pAppCtrl)
    {
        path = pAppCtrl->getFilePathFromTreeModel(iIndex);
    }
    return path;
}

bool GTAMdiController::generateScxml(bool isMultipleLogs,GTAElement *pElement,
                                        QString iRelativePath, const QList<GTAActionCall *> iCallProcLst,
                                        QStringList &oScxmlList,QStringList &oLogLst,
                                        QStringList &oFailedList,QStringList &oProcList)
{
    bool rc = false;
    GTAAppController *pAppCtrl = GTAAppController::getGTAAppController();
    if(pAppCtrl)
    {
        rc = pAppCtrl->generateScxml(isMultipleLogs,pElement,iRelativePath,iCallProcLst,oScxmlList,oLogLst,oFailedList,oProcList);
    }

    return rc;
}


void GTAMdiController::executeSequence(bool isMultipleLogs,const QStringList &iScxmlList,
                                          const QStringList &iLogLst,const QStringList &iFailedList,
                                          const QStringList &iProcList)
{
    GTAAppController *pAppCtrl = GTAAppController::getGTAAppController();
    if(pAppCtrl)
    {
        connect(pAppCtrl,SIGNAL(toggleLaunchButton(bool)),this,SIGNAL(toggleLaunchButton(bool)));
        pAppCtrl->executeSequence(isMultipleLogs,iScxmlList,iLogLst,iFailedList,iProcList);
    }
}

//bool GTAMdiController::launch(const QString& iElementName, QString& errorMsg,bool iHasUnsubscribeStart, bool iHasUnsubscribeEnd,QStringList& iVarList)
bool GTAMdiController::launch(LaunchParameters parameters)
{
    bool rc = false;
    GTAAppController *pAppCtrl = GTAAppController::getGTAAppController();
    if(pAppCtrl)
    {
        for (int i = 0; rc == false && i < 100; i++)
        {
            try
            {
                rc = pAppCtrl->launch(parameters);
            }
            catch (...)
            {
                rc = false;
                pAppCtrl->setLastError("SCXML generation terminated due to memory management exception.\nPlease try to launch SCXML generation again.");
            }
        }
        if(!rc)
        {
            _LastError = pAppCtrl->getLastError();
        }
    }
    return rc;
}

bool GTAMdiController::isDocumentValid(const QString & iDocFilePath)
{
    bool rc = false;
    GTADocumentController docCtrl;
    rc = docCtrl.isDocumentValid(iDocFilePath);
    return rc;
}

QString GTAMdiController::getAbsoluteFilePathFromUUID(const QString &iUUID)
{
    QString absFilePath = false;
    GTAAppController *pAppCtrl = GTAAppController::getGTAAppController();
    if(pAppCtrl)
    {
        absFilePath = pAppCtrl->getAbsoluteFilePathFromUUID(iUUID);
    }
    return absFilePath;
}

bool GTAMdiController::isValidLogPresent(const QString &iLogName,QString &oLogFilePath)
{

    bool isValidLog = false;
    GTAAppController *pAppCtrl = GTAAppController::getGTAAppController();
    if(pAppCtrl)
    {
        isValidLog = pAppCtrl->isValidLogPresent(iLogName,oLogFilePath);
    }
    return isValidLog;
}


void GTAMdiController::onUpdateApplicationLogs(QStringList list)
{
    emit updateApplicationLogs(list);
}

/**
 * @brief: This method sets the unsubscribeFileParamOnly value which will be used to create scxml
 * @param: unsubscribeFileParamOnly : if true then only File params will be unsubscribed
 */
void GTAMdiController::setUnsubscribeFileParamOnly(const bool unsubscribeFileParamOnly)
{
    _unsubscibeFileParamOnly = unsubscribeFileParamOnly;
}
