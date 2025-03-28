#ifndef GTAMDICONTROLLER_H
#define GTAMDICONTROLLER_H
#include "GTAControllers.h"
#include "GTAAppController.h"
#include "GTAViewController.h"
#include "GTADocumentController.h"
#include "GTAEditorViewController.h"

#pragma warning(push, 0)
#include <QObject>
#include <QList>
#include <QUndoStack>
#pragma warning(pop)

class GTAControllers_SHARED_EXPORT GTAMdiController :public QObject
{
    Q_OBJECT

public:

    static GTAMdiController* getMDIController(QObject* ipObject=0);
    bool getClipboardModel(QAbstractItemModel*& pModel);

    bool getModelForTreeView( QAbstractItemModel*& pModel ,const int& iRow, GTAElement *pElement);
    QString getLastError();
    bool saveDocument(const QString & iName, bool iOverWrite,GTAElement * ipElement, GTAHeader* ipHeader,const QString &iDataDirectory);
    bool headerExists(const QString& iName)const;
    bool exportToScxml(GTAElement* pElement, const QString & fileName, const QString &iFilePath,bool iHasUnsubscibeAtStart,bool iHasUnsubscibeAtEnd);
    bool saveAsDocument(const QString &iElemName);
    bool getFileInfoListUsed(const QString& iFileName, QList <GTAFILEInfo> & lstFileInfo);

    QHash<QString,QStringList> getActionCommandList(GTAUtil::ElementType iType,QStringList& orderList );
    QList<QString> getCheckCommandList(GTAUtil::ElementType iType);
    GTAEditorViewController * getActiveViewController()const;
    ErrorMessageList CheckCompatibility();
    bool getElementFromDocument(const QString & iFileName, GTAElement *& opElement,bool isUUID = false);
    QString getFilePathFromTreeModel(const QModelIndex &iIndex);
    bool launch(LaunchParameters parameters);
    bool isDocumentValid(const QString & iDocFilePath);
    QString getAbsoluteFilePathFromUUID(const QString &iUUID);
    bool generateScxml(bool isMultipleLogs,GTAElement *pElement,
                       QString iRelativePath,const QList<GTAActionCall *> iCallProcLst,
                       QStringList &oScxmlList,QStringList &oLogLst,
                       QStringList &oFailedList,QStringList &oProcList);

    void executeSequence(bool isMultipleLogs,const QStringList &iScxmlList,
                         const QStringList &iLogLst,const QStringList &iFailedList,
                         const QStringList &iProcList);
    bool isValidLogPresent(const QString &iLogName,QString &oLogFilePath);
    void onUpdateApplicationLogs(QStringList list);
    bool getUnsubscribeFileParamOnly() { return _unsubscibeFileParamOnly; }
    void setUnsubscribeFileParamOnly(const bool unsubscribeFileParamOnly);


signals:
    void updateClipBoard();
    void updateEditingActions(bool iVal);
    void toggleEditorActions(bool iToggleVal);
    void updateTitleCollapseActionIcon(bool iVal);
    void updateCollapseToTitle(bool iVal);
    void updateMoveDownAction(bool iVal);
    void updateMoveUpAction(bool iVal);
    void refreshReadOnlyDocStatus(bool iVal);
    void updateActiveStack(QUndoStack *&undoStack);
    void removeStack(QUndoStack * &undoStack);
    void sigCallActionDropped(int& iRow, QString& iFileName);
    void updateErrorLog(const ErrorMessageList &iErrorList);
    void updateErrorLogMsg(QString iMessage);
    void elementCreatedForJump(GTAAppController::DisplayContext iDisplayContext);
    void removeWindow();
    void updateClearAllIcon(bool);
    void saveEditorDocFinished(const QString &iFilePath);
    void openInNewWindow(const QString &iFileRelPath,bool isUUID);
    void ShowProgressBar(QString iMsg);
    void HideProgressBar();
    void toggleLaunchButton(bool toggleStatus);
    void updateOutputWindow(const QStringList iMsgLst);
    void updateApplicationLogs(const QStringList iMsgLst);

public slots:
    void onClearClipBoardItems();
    void onGetHeaderTemplateFilePath(QString &templFilePath);
    void onGetGTADataDirectory(QString &oDirPath);
    void onGetCopiedItems(QList<GTACommandBase*> &oCopiedItems);
    void onSetCopiedItem(GTACommandBase* selectedItem);
    void onSetCopiedItems(QList<GTACommandBase*> lstSelectedRows);
    void onUpdateActiveViewController(GTAEditorViewController * pViewController);
private:
    GTAMdiController();
    static GTAMdiController *_pMDIController;

    GTAViewController *_pViewController;
    QList<GTACommandBase*> _lstOfCopiedItems;
    QString _LastError;
    GTAEditorViewController *  _pActiveViewController;       //View controller corresponding to active sub window
    bool _unsubscibeFileParamOnly;

};

#endif // GTAMDICONTROLLER_H
