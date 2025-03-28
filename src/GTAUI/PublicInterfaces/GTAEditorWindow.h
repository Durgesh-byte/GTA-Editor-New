#pragma once

#pragma warning (push, 0)
#include "GTAUI.h"
#include "GTAElement.h"
#include "GTAElementViewModel.h"
#include "GTAEditorViewController.h"
#include "GTAActionSelectorWidget.h"
#include "GTACheckWidget.h"
#include "GTAMdiController.h"
#include "GTACommandBase.h"
#include "GTAAppController.h"
#include "GTADebugger.h"
#include <GTAProgress.h>
#include "GTAEditorFrame.h"
#include "CustomizedCommandInterface.h"

#include <QMessageBox>
#include <QWidget>
#include <QMdiSubWindow>
#include <QModelIndex>
#include <QUndoStack>
#include <QMenu>
#pragma (pop)


class GTAAppController;
class GTAActionSelectorWidget;
class GTACheckWidget;
class GTACommentWidget;
class GTAHeaderFieldEdit;
class GTASearchReplaceDialog;
class GTAHeaderWidget;
class GTAPurposeAndConclusionDialog;
struct GTACustomizedInterface;
struct GTACustomizedCheck;
struct GTACustomizedAction;

using TabIndices = GTAEditorFrame::TabIndices;

namespace Ui {
class GTAEditorWindow;
}

class GTAUI_SHARED_EXPORT GTAEditorWindow : public QWidget
{
    Q_OBJECT

public:

    enum ExecutionMode{NORMAL_MODE,SEQUENCER_MODE,DEBUG_MODE};
    explicit GTAEditorWindow(QWidget* parent = nullptr, const QString &iFileName = QString(""));
    ~GTAEditorWindow();
    void openDocument(const QString &iFileName,bool isOutsideDataDir = false);
    void setWindowTitle(const QString &iFileName);
    void F2KeyPressed();
    void F9KeyPressed();
    void clearRow(QList<int> lstSelectedRows, QList<int> &oInvalidCmdList);
    void undoClearRow(const QList<int> &iIndexList, QList<GTACommandBase*> &iCmdList);
    void addCommand(int selectedRow, QVariant cmd);
    void deleteRow(int selectionStatus, QList<int>& lstSelectedRows);
    void addRow(int selectedRowStatus, QList<int> &selectedRows);
    void insertCommands(const QList<int> &iIndexList, const QList<GTACommandBase*> &iCmdList,bool isSrcUndoRedo = false);
    void cutRow(const int &selectionStatus, QList<int> &ioLstSelectedRows);
    void pasteRow(const QList<int> lstSelectedRows, bool &oPastStatus);
    void pasteRow(const QList<int> lstSelectedRows, bool &oPastStatus,const QList<GTACommandBase*> CmdsPasted);
    void showBetweenSelectedRows(int status,QList<int> selectedRows);
    void showAllRows();
    void HideRows( QList<int> iLstRows);
    void showRows(QList<int> iLstRows);
    GTACommandBase * getCommandForSelectedRow(const int &iSelRow)const;
    int  getSelectedEditorRows(QList<int>& oSelectedRows);
    int  getSelectedEditorRow(int& oSelectedRow);
    bool editAction(const int& iRow, const QVariant& value);
    bool editCheck(const int& iRow,const QVariant& value);
    void getManualActionRows(QList<int>& oManualActionRows);
    void getManualActionDependencies(const int& iRow, QList<int>& oManualActionDep, QList<int>& oManualActionDepWhichAreManAct);
    void getAllRows(QList<int>& oAllRows);

    void deleteMultipleRowSelection(const QList<int>& lstSelectedRows);
    /**
      * @brief : This function strike off/remove strike off depending on user inputs for selected rows.
      * @iIgnoreStatus: Takes true: to ignore & false: to remove ignore of selected rows
     */
    void ignoreInSCXMLOnOff(bool iIgnoreStatus);
    void ignoreManualActions(const QList<int> manAcToBeIgnored);
    void updateEditorView();
    void moveEditorRowUp(const QList<int> selectedRows);
    void moveEditorRowDown(const QList<int> selectedRows);
    void collapseAllToTitle(const bool &iCollapseTitle);
    void onReadOnlyDocToggled(bool isReadOnlyDocToggled);
    QString getUUID()const;

    void F10KeyPressed();
    void ShiftF10KeyPressed();
    void F6KeyPressed();
    void ShiftF6KeyPressed();

    /**
      * @brief: function call to check whether selected rows contains read only commands
      *         return true if selected rows has read only commands otherwise false
      */
    bool getReadOnlyCmdStatus();

    void autoAddElse(int& row);    


public:
    void setFileType(const QString &iFileType);
    void setAuthorName(const QString &iAuthor); // V26 editor view author column enhancement #322480
    void setMaxTimeEstimated(const QString& iMaxTimeEstimated);
    void setUndoRedoForMTE(const QString& old_value, const QString& new_value);
    QString getMaxTimeEstimated();
    void onSave();
    void setFileDetails(const QString &iElementName, const QString &iRelativePath, const QString &iFileType = QString(""), const QString &iAuthorName = QString(), const QString & iMaxTimeEstimated = QString());
    void setInputCsvFilePath(const QString& iInputCsvFilePath);
    void setOutputCsvFilePath(const QString& iOutputCsvFilePath);
    void displayHeader();
    void displayPurposeAndConclusionDialog();
    void setPurposeAndConclusionToElement(const QString& summary, const QString& conclusion);
    void highlightHeaderTableRow(int row);
    bool isHeaderRowEditable(const int& iRow);
    bool getHeaderFieldInfo(const int& iRow, QString& iFieldName, QString& iFieldVal, QString& iMandatoryInfo, QString& iFieldDescription, bool& iShowInLTRA);
    bool isHeaderFieldMandatory(const int& iRow, bool& isMandatory);
    bool isHeaderFieldDefaultMandatory(const int& iRow);
    QString getLastError();
    bool editHeaderField(const int& iRow, const QString& iName,const QString& iVal,const QString& iMandatory,const QString& iDescription, const bool& iShowLTRA);
    bool isHeaderRowDeletable(const int& iRow);
    bool removeHeaderItem(const int& iRow);
    bool moveHeaderRowItemUp(const int& iRow);
    bool moveHeaderRowItemDown(const int& iRow);
    bool headerExists(const QString& iName)const;
    QString getHeaderName()const;
    bool headerComplete(QStringList& oEmptyManField);
    bool createHeaderModelFromDoc(const QString& iFileName, QAbstractItemModel*& opEditorModel);
    bool saveHeader(const QString& iName)const;
    bool deleteHeader(const QString& iName)const;
    bool setInitialHeaderView(QTableView* ipView);
    QString getRelativePath()const;
    QString getElementName()const;
    bool isDataEmpty();
    void exportSXCML();
    void exportAsPlainTextXml();
    GTAElement *getElement()const;
    void clearEditorPage();
    void clearAll();
    void undoClearAll(GTAElement *pElement);
    bool showICDDetails(bool iStatus,QString &oErrMsg);
    void CallActionDropped(int& iRow, QString& fileName,QString &fileRefUUID);
    void highlightRow(const int &iRowId, GTAAppController::DisplayContext iDisplayContext);
    void setJumpFlag(bool iVal);
    void setJumpContext(GTAAppController::DisplayContext iDisplayContext);
    void createCallSequence(const QModelIndexList &iSelectedItems,const QString &iType);

    void launch(ExecutionMode mode ,QMessageBox::ButtonRole role);
    void debug();
    bool saveFile();
    bool isFileModified(bool emitSignal = false);
    void setGlobalSave(bool val);
    void setGlobalSaveStatus(int &iStatus);
    bool isValidLogAvailable(QString &oLogPath);

    //----- Indentation ---------
    void hideColumns();
    void showColumns();
    //----- Indentation ---------

//    void setIsLaunchSequencer(bool iVal){_isLaunchSequencer = iVal;}
//    bool isLaunchSequencer()const{return _isLaunchSequencer;}


    void setExecutionMode(ExecutionMode mode);
    ExecutionMode getExecutionMode()const {return _executionMode;}
    void disableEditingActions(bool bDisable);
    void setResetLiveMode(const QString iFileName, bool iVal);
    void clearSelectedRows();
    int getRowToBeSelected() const;

    TabIndices getFramePosition() const;
    void setFramePosition(const TabIndices& position);

    bool getUnsubscribeAtStartResults() const;
    void setUnsubscribeAtStartResults(bool);
    bool getUnsubscribeAtEndResults() const;
    void setUnsubscribeAtEndResults(bool);
    void setUnsubscribeFileParamOnly(bool);
    bool getUnsubscribeFileParamOnly() const;
private slots:
    void validateName(QString iName);
    void onCreationOfElementCompleted();
    void onSingleClickOfRow(const QModelIndex & iIndex );
    void onDoubleClickOfRow(const QModelIndex & iIndex );
    void setMoveUpDownButtonEnabled(QModelIndex index);
    void setMoveUpDownButtonEnabled();

    void displayContextMenu(const QPoint &iPos);
    void ScrollEditorPage();
    void focusInEvent(QFocusEvent *event);
    void closeEvent(QCloseEvent *event);
    void exportScxmlSlot();
    void highLightLastChild(QModelIndex index);
//    void onPortingCompleted();
    void displayLiveEditorContextMenu(const QPoint &iPos, QWidget * pSourceWidget);
	void checkIsFileModified();
    /**
      * @brief: This slot invoked on repitions execute context menu trigger.
      *         This method copy the selected row and paste n-times at the end in Live Edidor.
      */
    void onRepeatSnippet();
    void autoAddExternalAction(const QString& cmdType, bool isOnOk);
    std::optional<QString> findReturnStatusConditionArg(int& row, const QString& cmdName);

public slots:
    void onEditCommand();
    void onInsertAction();
    void onInsertCheck();
    void onStartEditAction();
    void onReopenWindow();
    void onStartEditCheck();
    void onClearRow();
    void onAddRow();
    void onDeleteRow();
    void onEditorCopyRow();
    void onEditorPasteRow();
    void onOpenCallInNewWindow();
    void onShowBetweenSelectedRows();
    void onEditorCutRow();
    void onHideSelecteRowTitle();
    void onMoveEditorRowUp();
    void onMoveEditorRowDown();
    void onIgnoreOnSCXML();
    void onIgnoreOffSCXML();
    void selectAllRows();
    std::tuple<QList<int>, bool> selectRow(int row);
    QHash<QString, QStringList> getActionCommandList();
    void setCustomizedActionCommandInWidget(const QString& actionName);
    std::tuple<QString, QString> setActionMenuSenders(QAction* pActionSender);
    bool isActionComplementWrong(GTAActionBase* pAction, const QString& actionsender, const QString& menusender, QString& action, QString& complement);
    void setActionEditableDependingOnParam();
    void setCheckEditableDependingOnParam();
    void setRowIndex(int idx);
    void appendCustomizedAction(std::shared_ptr<CustomizedAction>, ErrorMessageList& errorLogs);
    QList<QString> getCheckCommandList();
    void appendCustomizedCheck(std::shared_ptr<CustomizedCheck>, ErrorMessageList& errorLogs);
    void onSetReadOnlyStatusForCmd();
    void onCollapseAllToTitle(bool iCollapseTitle);
    void onShowCommandDetail();
    void onClearPage();
    void show();
    void onSaveEditorDocFinished();
    void onHeaderFieldAdded(const QString& iName,const QString& iVal,const QString& iMandatory,const QString& iDescription);

    void checkCompatibility();
    void displayCompatibilityReport();
    void onEscapeKeyTriggered();
    void onCompatibilityCheckForLaunchFinished();
    void onLauchSCXMLCompleted();
    void searchAndHighlightNext(QRegExp iSearchString,bool ibSearchUp,bool ibTextSearch);
    void searchAndHighlightAll(QRegExp iSearchString,bool iTextSearch,QHash<int,QString> &oFoundRows,bool isHighlight = true, bool isMultiSearch = true);
    void replaceAndHighightNext(QRegExp iSearchString,QString iReplaceString,bool ibSearchUp);
    bool replaceAllText(QRegExp iSearchString,QString iReplaceString);
    bool replaceAllEquipment(QRegExp iSearchString, QString iReplaceString);
    void resetEditorDisplayMode();
    void onSetFocus();
    void onSequenceExecutionCompleted();
    void onUpdateOutputWindow(const QStringList iMsgLst);

    void onDebuggingCompleted();


    void onCheckLineHasBreakpoint(int iLineNumber, bool &oHasBreakpoint);
    void onDebuggerHitBreakpoint(int iLineNumber);
    bool userInputUpdated();
    void onLaunchDebugMode(bool iVal);
	void removeEmptyRows();
    void hideProgressBar();

private:
    void showProgressBar(const QString& iMsg);
    void initMembers();
    void loadEditor(const QString &iFileName, bool isOutsideDataDir = false);
    void storeColoumWidths();
    bool detailViewExists(const int& iSelectedRow);
    void createContextMenu(const QString iFileName);
    void resizeEditorToContents();
    void resizeRowsToContents();

    QString getExtension(const QString &iElementName);
    QString getAbsolutePath()const;
    inline void assignPaste() { PasteCallNow = "copy";PasteCallPrev = "copy";}
    void elementCreationSuccessful();
    int saveUnsavedFile();
//    void portElement();

    bool searchHighlight(const QRegExp& searchString,QList<int>& iSelectedRows,bool ibSearchUp,bool singleSearch,bool ibTextSearch,
                         QModelIndex &oHighlightedRowIdx,bool isHighlight, QHash<int,QString> &oFoundRows);
    bool hightlightReplace(const int& iSelectedRow,const QRegExp& searchString,const QString& sReplace,bool ibSearchUp,QModelIndex &oHighlightedRowIdx);
    GTAElementViewModel::DisplayContext getConvertedDisplayContext(GTAAppController::DisplayContext iDisplayContext);
    bool isSequenceValidForMultipleLogs(QList<GTAActionCall*> &ocallProcLst);
    void onToggleEditorConnections(bool iVal);
    void traverseChildren(GTACommandBase *pParent, int &counterCommands, bool iIgnoreStatus,GTACommandBase* pEndChild);
    void toggleIgnore(GTACommandBase * pCurrentCommand, bool iIgnoreStatus);
    void checkIgnoreStatusOfChildren(GTACommandBase *pCurrentCommand,int &count, bool iIgnoreStatus);
    void setEndCmdIgnore(GTACommandBase* pCmd, bool iIgnoreStatus);
    void createLiveEditorContextMenu(const QString iFileName);
    void resizeLiveEditorHeader();

signals:
    void rowCopiedToClipBoard();
    void disableEditingActionsInMain(bool iVal);
    void toggleEditorActionsinMain(bool iToggleVal);
    void toggleTitleCollapseActionIcon(bool iVal);
    void collapseToTitle(bool iVal);
    void updateMoveDownAction(bool iVal);
    void updateMoveUpAction(bool iVal);
    void updateReadOnlyDocStatus(bool iVal);
    void updateActiveStack(QUndoStack * &undoStack);
    void removeStack(QUndoStack * &undoStack);
    void updateActiveViewController(GTAEditorViewController* pViewController);
    void headerFieldUpdatedInModel(bool val, const QString &iErrorMsg);
    void updateErrorLog(const ErrorMessageList &iErrorList);
    void updateErrorLogMsg(QString iMessage);
    void elementCreatedForJump(GTAAppController::DisplayContext iDisplayContext);
    void removeWindow();
    void updateClearAllIcon(bool);
    void saveEditorDocFinished(const QString &iFilePath);
    void openInNewWindow(const QString &iFileRelPath,bool isUUID);
    void ShowProgressBar(QString iMsg);
    void HideProgressBar();
    void saveModifiedFile(const QString &iUUID);
    void toggleLaunchButton(bool toggleStatus);
    void userActionRegistered(GTADebugger::UserAction);
    void updateOutputWindow(const QStringList iMsgLst);
    void launchDebugMode(bool iVal);
    void updateActiveSubWindow();
    void fileHasChanged(TabIndices, QString);
    void headerChangesDone();

private:
    Ui::GTAEditorWindow *ui;
    GTAActionSelectorWidget* _pActionWindow;
    GTACheckWidget * _pCheckWindow;

    GTAProgress* _pProgressBar;
    GTAElement *_pElement;
    GTAElementViewModel*_pElementViewModel;
    QTableView *_pEditorView;
    //    QTableView *_pHeaderView;

    QMenu *_pContextMenu;

    QAction* _pAddRow;
    QAction* _pDeleteRow;
    QAction* _pClearRow;
    QAction* _pCutRow;
    QAction* _pCopyRow;
    QAction* _pPasteRow;
    QAction* _pEsc;
    QAction * _pMoveRowUpAction;
    QAction * _pMoveDownAction;
    QAction* _pSelectAllRows;

    QUndoStack *_pUndoStack;
    QAction *_pUndoAction;
    QAction *_pRedoAction;
    QAction * _pOpenCallInNewWindow;
    QMenu * _pOpenCallInNewWindowMenu;
    QMenu *_pLiveEditorContextMenu;
    bool _unsubscribeAtStartResults = true; 
    bool _unsubscribeAtEndResults = true;
    bool _unsubscribeFileParamOnly = true;

    GTAEditorViewController *_pEditorController;
    GTAMdiController    *_pMDIController;

    //    QString _CurrentElementType;
    QList<int> _ColWidths;
    bool _titleCollapseToggled;
    bool _ReadOnlyDocToggled;

    QString _elementName; // only name with extension
    QString _absFilePath;  //absolute file path of the element
    QString _relativePath; // path excluding data path along with file name at the end
    QString _AuthorName; //V26 editor view author column enhancement #322480
    QString _maxTimeEstimated;
    

    GTAHeaderWidget *_pHeaderWidget;
    GTAPurposeAndConclusionDialog *_pSummaryAndConclusionDlg;
    bool _isJump;
    GTAAppController::DisplayContext _contextOnJump;

    QString PasteCallNow; //saurav
    QString PasteCallPrev;
    QString lastCalled; //saurav
    QString _windowTitle;
    int _lastindex;
    bool _isGlobalSave;
    int _globalSaveStatus;
    bool _isLaunchSequencer;
    ExecutionMode _executionMode;
    int _CurrentExecutionLine;
    int _lineJumpCnt;
//    QPointer <GTADebugger> _pDebugger;
//    QPointer <QThread> _pDebuggerThread;

    GTADebugger * _pDebugger;
    QThread * _pDebuggerThread;
    GTADebugger::UserAction _action;

    bool _isDebugInProgress;
    GTACommandBase *_pExecutingCmd;
    bool _enableNextPreviousContinue;

    // attributes related to the subwindow position in the editor frame
    TabIndices _framePosition;

    // global command argument that contains the command in the selected line "to be able to get the actual command when we want to change it and make a Ctrl+Z"
    QVariant _actualCmd;

    int _reopenWindowRow;
    bool _openActionAfterCheck = false;
    int _rowToBeSelected;
};  
