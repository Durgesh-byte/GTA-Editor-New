#ifndef GTAMAINWINDOW_H
#define GTAMAINWINDOW_H

#include <GTAProgress.h>
#include "GTANewFileCreationDialog.h"
#include "GTAUI.h"
#include "GTAViewController.h"
#include "GTARenameDialog.h"
#include "GTAEquipmentMap.h"
#include "GTASCXMLToolId.h"
#include "GTAMdiController.h"
#include "GTAEditorWindow.h"
#include "GTAPortingDialog.h"
#include "GTAResultMdiArea.h"
#include "GTATreeView.h"
#include "GTAControllerLauncherWidget.h"

#include "CsvToProConverter.h"

#pragma warning (push, 0)
#include <QMainWindow>
#include <QComboBox>
#include <QPushButton>
#include <QShortcut>
#include <QAction>
#include <QPoint>
#include <QMenu>
#include <QLabel>
#include <QStringListModel>
#include <QTableView>
#include <QUndoStack>
#include <QUndoGroup>
#include <QUndoCommand>
#include <QFileSystemWatcher>
#include <QFutureWatcher>
#include <QFileSystemModel>
#include <QStack>
#include <QActionGroup>
#include <QDialogButtonBox>
#include <QPushButton>

#include <algorithm>
#include <chrono>
#include <functional>
#include <optional>

#include "GTATreeView.h"
#include "GTAEditorFrame.h"

class GTAAppController;
class GTASettingsWidget;
class GTAFileSystemModel;
class GTAHeaderFieldEdit;
class GTASearchReplaceDialog;
class GTAExportDocWidget;
class GTAKeyBoardShortcutsWidget;
class GTALogFrame;
class GTACompatibilityReport;
class GTAFSIIImportWidget;
class GTAImportDialog;
class GTANewFileCreationDialog;

struct exportDocumentOutput
{
    exportDocumentOutput() : retVal(false), errorMessages(QStringList()) {}
    bool retVal;
    QStringList errorMessages;
};

struct exportDocumentInput
{
    exportDocumentInput() : SaveFolder(QString()),
        FolderName(QString()),
        lstOfDocuments(QStringList()),
        bIncludeBDB(false),
        bIncludeSCXML(false),
        bCompress(false),
        FileName(QString()),
        LogFile(QString()) {}
    QString SaveFolder;
    QString FolderName;
    QStringList lstOfDocuments;
    bool bIncludeBDB;
    bool bIncludeSCXML;
    bool bCompress;
    QString FileName;
    QString LogFile;
};

namespace Ui {
class GTAMainWindow;
}

using TabIndices = GTAEditorFrame::TabIndices;

class GTAUI_SHARED_EXPORT GTAMainWindow : public QMainWindow
{
    Q_OBJECT
    enum EditorViewMode{ReadOnly,Edit,Create};
public:
    explicit GTAMainWindow(QString app_config_file_path, QString bench_config_folder_path, QString bench_config_file_name, QWidget* parent = 0);
    ~GTAMainWindow();
    void setAppController(GTAAppController* ipAppController);
    void initialize(bool showSettingsNotValid = true);
    bool checkApplicationConfiguration( QString& isErrorMsg);
    void updateRecordingVideoToolFile(const QString& filepath, const std::string& toolName);
    void clearRow(QList<int> lstSelectedRows, QList<int> &oInvalidCmdList);
    void undoClearRow(const QList<int> &iIndexList, QList<GTACommandBase*> &iCmdList);
    void updateTree(bool rebuildDb, bool isInit = false);
    bool checkDatabaseConsistency();
    void toggleUpdateReloadDB(bool iStatus);
    GTAEditorWindow* createNewSubWindow(const QString& iFilename = "");
    QWidget* getActiveSubWindow();

private:
    QString _TestReportCsvSavePath = "";
    QString _current_app_config_file_path;
    QString _current_bench_config_file_name;
    QString _current_bench_config_file_folder;
    QString _last_csv_selection_path;
    QString _last_plain_xml_export_path = "";
    bool _isConsole = false;
    bool _validationStatusToBeUpdatedOnTreeView = false;
    GTASystemServices* _pSystemServices;

    void initMembers();
    void createToolbars();
    bool linkToDatabase();
    void createEditorToolBar();

    bool initializeConfiguration();

    void resetSCXMLInitSubscribeChecks();
    bool isFileLoaded(const QString &iRelPath, bool isUUID = false);
    GTAEditorWindow* getLoadedWindow(const QString &iRelPath);

    void readSettings();
    void readSettings2();
    void ignoreInSCXMLOnOff(bool iVal);
    void connectFoldersToWatcher();
    void updateDataBrowserModel();
    void initialiseResultPage();
    void initEditor();
    void createTabContextMenu();

    void updateMoveUI();

    //Create Document viewer header context menu
    void createDVHeaderContextMenu();

    /**Save the last selection settings of document viewer
      @index: take the index of the column
      @actionText: header name
      @isChecked: to know whether the particular column is checked or not
    */
    void saveDVSelectionSettings(int iIndex,QString iactionText, bool iIsChecked, int iListSize);

    void createDebugWindowContextMenu();
    void createAppLogWindowContextMenu();
    ErrorMessageList createDBErrorLogs(QStringList &iList)const; 
    void restoreLastSession();
    void loadSession(const QString &iSessionName);
    int savePrompt(QList<GTAEditorWindow *> &iLstModifiedFiles);
    int saveSession(bool isGlobalSave = false);

    void transferDataToFile(GTAEditorWindow* pSubWindow);

    //Csv related public fields
public:
    CsvToProConverter _csvConverter;
    QString _customPath = "";
    QString _customFileName = "";
    QString _last_csv_selection_file;
    QString _dateTestReport = "";
    void setTestReportCsvSavePath(const QString& path) { this->_TestReportCsvSavePath = path; }
    void setForceOverwrite(bool forceOverwrite) {
        _pNewFileDialog->_forceOverwrite = forceOverwrite;
    }
    void setIsConsole(bool isConsole) {
        _isConsole = isConsole;
        _pNewFileDialog->_isConsole = isConsole;
        _csvConverter.setIsConsole(isConsole);
    }
    void addCsvFile(const QString& fileName, const QString& authorName) {
        onAddFile(GTANewFileCreationDialog::NEW_CSV2PRO_FILE, fileName, authorName);
    }
    ErrorMessageList& getLogs() {
        return _csvConverter.getLogs();
    }

    /**
     * This function creates a new blank result analysis page and attached to Result Mdi Area
     * @return: Subwindow object of new page created
    */
    GTALogFrame* createNewResultPage();

    GTAAppController* getAppController();

private:
	/**
	 *  This function allow to clear the Log Window before running CheckCompatibility() function
	*/
	void clearLogWindow();

    /**
      * This function creates result tool bar (expand/collapse all,Select/deselect all, new tab) and add actions to them
     */
    void createResultToolbars();

    /**
      * This function sets the window title of new created result page similar to notepad++ style
      * @newLogFrame: Input newLogFrame subwindow for which window title is to set
     */
    void setNewWindowTitle(GTALogFrame *&newLogFrame);


    /**
      * This function creates context menu for result pages
     */
	void initResultTabbar();

    /**
     * This function returns widget object if window title is present in Result Mdi Area otherwise null
     * @titleName: window title of result page i.e. relative path.
     * @return: QWidget object containing title name
    */
    QWidget* getWidgetFromTitle(QString titleName);


private:

    Ui::GTAMainWindow* ui;
    GTAAppController* _pAppController;
    GTAMdiController* _pMdiController;

    //created for new from option.
    bool _isNewFromSelected;
    bool _isSaveAsOption;
    bool _headerChangesDone = false;
    QString _lastOpenedDoc;
    QList<int> _ColWidths;
    QComboBox* _pProgramCB;
    QComboBox* _pEquipmentCB;
    QComboBox* _pStandardCB;
    QAction* _pLoadProgAction; 
    QAction* _pReloadProgAction;
    QAction* _pCheckCompatibilityAction;
    QAction* _pLaunchScxmlAction;
    QAction* _pLaunchSequencerAction;
    QAction* _pTitleBasedExecution;
    QAction*_pDebugAction;
    QAction* _pStopSCXML;
	__int64 _t1;
	__int64 _t2;

    GTASearchReplaceDialog*  _pSearchReplaceDialog;
    GTASettingsWidget*       _pSettingWidget;
    GTANewFileCreationDialog* _pNewFileDialog;

    QFutureWatcher<void>* _equipmentDataWatcher;

    // This variable is used to show the compatibility check report
    // on the widget.
    GTACompatibilityReport* _ReportWidget;

    QFileSystemModel* _pFileSystemHeader;

    QHash<QString,QString> _DataTypeExtMap;
    QString _CurrentElementType;

    QMenu* _pContextMenuOnDV;
    QMenu* _pContextMenuOnDVHeader;

    QAction* _pOpenDocAction;
    QAction* _pDeleteDocAction;
    QAction* _pDocUsedByAction;
    QAction* _pMoveRowUpAction;
    QAction* _pMoveDownAction;
    QAction* _pCollapseToTitleAction ; 
    QAction* _pExportDocAction;
    QAction* _pShowDetailOnDV ; 
    QAction* _pShowDependencies;
    QAction* _pAddToFavOnDV; 
    QAction* _pRemoveFromFav;
    QAction* _pCreateCallSequence;
    QAction* _pRename;
    QAction* _pUpdateValidationStatus;
    QAction* _pExportSCXMLs;
    QAction* _pExportPlainXmls;
    QAction* _pExportCsvs;
    QAction* _pSaveAsAction;
    QAction* _pRefreshTreeAction;
	QAction* _pMoveFiles;
    QAction* _pAddFolder;
    QAction* _pIgnoreOn;
    QAction* _pIgnoreOff;
    QAction* _pSelectManActToBeIgnored;
    QAction* _pReadOnly;
    QAction* _pExportLTRA;
    QAction* _pExportPlainXml;
    QAction* _pExportSCXML;
    QAction* _pConvertCsvToPro;
    QAction* _pGenerateTestReport;
    QAction* _pSaveAllAction;
    QAction* _pSwitchPerspective;
    QAction* _pGTAControllerLauncher;

    QAction* _pGitInit;
    QAction* _pGitStatus;
    QAction* _pGitFolder;

    QAction* _pOpenContainingFolderFromTree;
    QAction* _pCommitChangesToGit;
    QAction* _pDiffFiles;
    QAction* _pAddFileToCommit;
    QAction* _pRemoveFileFromGitWorkspace;

    QAction* _pAddToExecutionList;

    GTALogFrame* _pLogFrame;

    QTableView* _pClipBoardWindow;
    GTAKeyBoardShortcutsWidget* _pShortcuthelpWindow;

    GTAProgress* _pProgressBar;
    GTAProgress* _TreeProgressBar;

    GTAImportDialog* _pImportDialog;
    GTAExportDocWidget* _pExportDoctWidget;
    GTARenameDialog* _pRenameWidget;

    bool _titleCollapseToggled;
    bool _hideRowToggled;
    bool _ReadOnlyDocToggled;

    QAction* _pReadOnlyForDoc;
    QAction* _pSaveAs;
    QAction* _pClearPage;
    QAction* _pShowHeader;

    QUndoGroup *_pUndoStackGrp;
    QAction* _pUndoAction;
    QAction* _pRedoAction;

    QFileSystemWatcher _FileWatcher;
    QString _SaveStatus;

    bool _isTreeInit;
    bool _isDBInit;

    int _jumpToLine;

    int _exportDocToSCXMLcount;
    int _exportDocToSCXMLcount_SLOT;

    QModelIndexList _selectedForSeqList;

    QString lastCalled;


    QList<GTAEditorWindow*> _lstWindows;
    QHash<GTAEditorWindow*,QMdiSubWindow*> _EditorWindowMap;
    GTAEditorWindow* _pEditorWindow;

    QMenu* _pDebugWindowContextMenu;
    QAction* _pClearDebugWindow;
    QAction* _pCopydebugWindowText;

    QMenu* _pAppLogWindowContextMenu;
    QAction* _pClearAppLogWindow;
    QAction* _pCopyAppLogWindowText;

    QMenu* _pTabContextMenu;
    QAction* _pSave;
    QAction* _pClose;
    QAction* _pCloseAll;
    QAction* _pCloseAllButThis;
    QAction* _pMoveView;
    QAction* _pCopyFullPath;
    QAction* _pOpenContainingFolder;
    QAction* _pSwitchToEditorMode;
    QAction* _pEnableDisableUnsubscribe;
    QAction* _pEditTagVariables;
    QAction* _pEditMaxTimeEstimated;
    QAction* _pViewResult;

    QMap<QString,QDockWidget*> _DockWidgetsMap;     // <key,value> pair of <Dock window name,Dock Widget>
    QActionGroup* _pActionGrp;
    QList<QAction*> _recentSessions;

	QActionGroup* _pActivePerspectiveViewGrp;
    QActionGroup* _pActiveSubwindowViewGrp;

    QShortcut* _F10Key;
    QShortcut* _ShiftF10Key;
    QShortcut* _F6Key;
    QShortcut* _ShiftF6Key;
    QShortcut* _ConvertCsv;

    QTextEdit* _pOutputWindow;
    QTextEdit* _pApplicationLogs;

    QAction* _pIndentedView;

    QStringList _DVHeaderColList;
    QStringList _usrCredentials;
    QString _svnActionCommand;
	
    QAction* _pExpandAllResAction;
    QAction* _pCollapseAllResAction;
    QAction* _pSelectAllResAction;
    QAction* _pDeSelectAllResAction;

    GTAResultMdiArea* _pResultMdiArea;
    QAction* _pNewResTabAction;
	QMenu* _pResultTabContextMenu;
    SVNInputParam* _pSvnParamsTempContainer = nullptr;

    QPoint _rightClickPos;
    int _nbActionsTriggered;
    QList<QToolBar*> _toolBars;
    void hideRightClickMenu();

    bool _startUnsubscribeEnable;
    bool _endUnsubscribeEnable;
    bool _unsubscribeFileParamOnlyEnable;

    GTAControllerLauncherWidget *_pGTAControllerLauncherWidget;

signals:
    void refreshEditorTV();
    void HideProgressBar();
    void PluginWidgetClosed(bool);

protected:
    void closeEvent(QCloseEvent *event);
    void mousePressEvent(QMouseEvent* event) override;
    
private:
    bool isDockWidgetPresent(const QString &idockWdgtName);
    QDockWidget * addDockWidgetsToMainWindow(const QString & idockWdgtName);
    void updateRecentMenu(const QString &iCurrSessionName = QString());
    void addSessionToRecents(const QString &iSessionName);
    void removeSessionFromRecents(const QStringList &iSessionLst);
    int updateRecentSessionInfo();
    QStringList getSVNCredential();
    QStringList getSVNSelectedFiles(QString svnCommand, QString &isUpdateDir, QString &commitMessage, QString &errMsg);
    void prepareSvnAddAllCommand(const QString& svnActionCommand, SVNInputParam& svnParams);
    void moveFiles(QModelIndexList selectedFiles, QString dir);
    
private slots:

    void onVisibilityChanged();
    void updateIniFile();
    void refresh_ui();
    void on_cancel_settings();
    void open_bench_file_selection();
    void onDirectoryChanged(QString dir);


    //program configuration loading
    void loadProgram();
    //header
    void onDataSelection(QModelIndex);
    void onDeleteDataPBClicked();
    /**
     * @param brief This method deletes svn-locked state before deleting
     * @param QModelIndex Takes the index of selected item
    */
    void unlockDocumentInSvnBeforeDeleting(QModelIndex);
    void onDocumentUsedByClicked();
    void onAddRow();
    
    void onInsertAction();
    void onInsertCheck();
    void onDeleteRow();

    void onClearRow();
    void onEditorCutRow();
    void onEditorCopyRow();
    void onEditorPasteRow();
    void onSaveEditorDoc();
    void onViewResultsClicked();

    void onLaunchPBClicked();
    void onStopSCXMLClicked();

    void dockWidgetVisibility(bool);
    void dockWidgetViewActionUpdate(bool);
    
    void displayContextMenu(const QPoint &iPos);
    void displayHeaderContextMenu(const QPoint &iPos);

    void updateDatabaseFromLib(bool );
    int getSelectedEditorRows(QList<int>& oSelectedRows);

    void onSearchTypeChanged(QString columnName);
    void onDataSeachTextChange(const QString & iText);
    void onDataTypeValChange(const QString & iText);

    void updateElementBrowserFilter(const QString &iSearchStr, const QString & iFileExt);

    void onExportPBClicked();

    /**
     * This function exports test report to csv file
    */
public slots:
    QString getLastRecordFromLogFile(const QString& logFullPath);
    QString getLogFullPath(const QString& elementName);
    void generateReport(const QString& logFullPath, const QString& elementName, bool silentMode = false);
    QString findLatestLogFile(const QString& folderPath, const QString& baseLogFileName);
    QString getCsvTableFullPath(const QString& elementName);
    void onConvertCsvToProClicked();

private slots:
    void packChecksFromLog(QList<QStringList>& checks);
    bool testCaseTimeStampIdAreNumeric(QString& testcase, QString& timestamp, QString& row);
    void appendChecksFromLine(QString& line, QList<QStringList>& checks);
    void appendTestCasesFromLine(QString& line, QList<QStringList>& testcases);
    QList<QStringList> getCheckValueReportFromLog(const QString& logFullPath);
    QList<QStringList> getPrintTimeValueReportFromLog(const QString& logFullPath);
    QList<QStringList> getTestCasesReportFromLog(const QString& logFullPath);
    QString getTestReportFileNameToSave(const QString& elementName, bool silentMode);
    bool writeCheckValueReportToCsv(const QString& elementName, QVector<QVector<QString>>& checks, bool silentMode = false);
    QVector<QVector<QString>> makeTestReport(const QList<QStringList>& fullListOfChecks, const QList<QStringList>& fullListOfTimes, const QList<QStringList>& fullListOfTestCases);
    void onExportTestReportToCsvClicked();
    void onExportTestReportToCsvToolBarClicked(bool silentMode = false);
    void onExportAsPlainTextXmlClicked();

    /**
     * This function exports the .seq or .pro file to scxml file
     * @param: filename is the name for the exported file.
    */
    void toScxmlExportTheFile(const QString& filename, const QString& directory);

    void onExportSCXMLPbClicked();

    void onExecuteTitle(const QString &iUuid, GTAElement *pTitleBasedElem,QStringList& iVarList);

    void onKeyF9Pressed();

    void onKeyF2Pressed();

    void onSettingsActionClick();

    void setUITheme(QColor iColor = QColor(244, 247, 252));

    /**
      * The function show the image, which represent the task is going on
      * Call must delete the return QLabel object once the process is completed
      */
    void  showWorkInProgress();

    /**
      * This function launches the theme setting window
      */
    void onThemeActionClick();

    void onAboutClick();

    void onDoubleClickInTreeView(const QModelIndex &);
    void onOpenDocument(bool  =  false, bool  = false, bool  = false ,bool  = false);

    void onSaveAsItem();
    void onSaveAs();
    void onSaveAll();
    void saveAll(QList<GTAEditorWindow*> &lstFiles,int &iStatus);

    void EnableExport(bool iVal = true);

    void EnableFileEditorAction(bool iVal = true);

    //    void onImportFlatScript();
    void startImportingExternalFiles();
    void startImportingExternalFilesinParallel(bool);

    void onImport();

    /**
      * Slot of compatibility check tool bar button.
      */
    void onCheckCompatibilityPBClicked();

    void onGitInitClicked();

    /**
     * @brief Checks git status of all pro/seq/obj files in tree view
    */
    void onGitStatusClicked(bool enablePopups = true);

    /**
     * @brief Opens .git directory for current workspace if it exists
    */
    void onGitDirectoryClicked();

    void onRename();
    void onOpenDocumentFromFile();
    void onMoveEditorRowUp();
    void onMoveEditorRowDown();

    void onSearchReplaceShow();
    void on_actionSearch_Replace_triggered();
    void searchAndHighlightNext(QRegExp iSearchString,bool ibSearchUp, bool ibTextSearch);
    void resetEditorDisplayMode();
    void searchAndHighlightAll(QRegExp iSearchString,bool iTextSearch);
    void searchInAllDocuments(QRegExp iSearchString,bool iTextSearch);
    void replaceAndHighightNext(QRegExp iSearchString,QString iReplaceString,bool ibSearchUp);
    void replaceAllText(QRegExp iSearchString,QString iReplaceString);
    void replaceInAllDocuments(QRegExp iSearchString,QString iReplaceString);
    void replaceAllEquipment(QRegExp iSearchString, QString iReplaceString);
    void searchRequirement(QRegExp iSearchString);
    void searchTag(QRegExp iSearchString);
    void searchComment(QRegExp iSearchString);
    void onAddToFavorites();
    void onCollapseAllToTitle();
    void onRemoveEmptyRows();
    bool loadAndupdateConfiguration(bool isLibPathChanged, bool isRepoPathChanged, bool isToolDataPathChanged, bool rebuildWorkDb);
    void setBatchConfigDBFiles(const QString& iIniFilePath, bool iRemoveOldEntries);
	void modifyFilePath(QString dataPath, QString repoPath);
    void onGitDbChanged();

    void onUpdateDBFinished();

    void initializeSettingsWidget();

    //void startLaunchTool();
    void showTreeProgressBar();
    void hideTreeProgressBar();

    bool onLoadDBFinished();

    void openUserGuide();
    void onExportDocsClicked();
    QStringList exportDocuments(exportDocumentInput iInput,const QString &sDataFolder, const QString & sFolder, const QString &iElemName);
    void exportDocuments(const QString& iSaveFolder,const QString& iFolderName,const QStringList& ilstOfDocuments, bool ibIncludeBDB, bool ibIncludeSCXML,bool ibCompress, const QString &iFileName,const QString &iLogFile);
    void onShowElementDetail();
    void onShowDependencies();
    void populateDataDVContextMenu(QModelIndex);
    void onRemoveFromFavorites();
    void onCallActionDropped(int& iRow, QString& fileName);
    
    bool checkIfAllfilesInsideFolderAreSelected(const QSet<QString>& filesSelected, const QStringList& filesInsideFolder, QStringList& uncheckedFiles);
    void makeListOfSelectedFiles(const QModelIndexList& selectedIdx, const QString& svnCommand = "");
    void onSVNAction();
    void onHideSelecteRowTitle(); 

    void onShowClipBoard();
    void createCallSequence();
    void reloadProgram();
    void onDisplayKeyboarShortCutHelp();
    void onImportExternalFilesFinished();

    void onIgnoreOffSCXML();
    void onIgnoreOnSCXML();
    void onSelectManActToBeIgnored();
    void onSelectAllRows();

    void onSetReadOnlyStatus();
    void onReadOnlyDocToggled();
    
    void updateDataBrowserModel(const QString &ipath);
    void onInitDBFinished();
    void onTreeUpdateFinished_(bool rc);
    void onTreeUpdateFinished();
    void onOnlyInitDB();
    void onUpdateEquipmentFinished();
    void onRefreshTree();
	void onMoveFiles();
    void onDroppedFiles(QModelIndexList selectedFiles, QModelIndex target);
    void onAddFile(
        GTANewFileCreationDialog::DialogMode mode = GTANewFileCreationDialog::NEWFILE,
        const QString& fileName = "",
        const QString& authorName = "",
        const QString& fileType = ".pro"
    );
    void onAddFolder();


    void onClearPage();
    void onShowHeader();
    void onShowPurposeAndConclusion();
    void onHighlightHeaderTableRow(int row);
    void onPerspectiveChanged(bool );
    void onActionEditorViewTriggered(bool);
    void onChangeToEditorView(bool);
    void onMdiStyleChanged();
    void displayTabContextMenu(QPoint iPoint);
    void closeOverwrittenFileTabs(const QString& absPath, const QString& dataDir);
    void displayDebugContextMenu(QPoint iPos);
    void displayApplicationLogContextMenu(QPoint iPos);
    void onClose();
    void onLastWindowClosed();
    void saveModifiedFiles();
    void onCloseAllSubwindows();
    void onCloseAllButThis();
    void moveToOtherView();
    void onOpenContainingFolder();
    void onSwitchToEditorView();
    void onEnableDisableUnsubscribe();
    void onSwitchClicked();
    void onPortingWizardClicked();
    void onSessionManagerSelected();
    void onRecentSessionSelected();
    void onPrintElement();
    void onClearDebugWindow();
    void onCopyDebugWindow();

    void onClearAppLogWindow();
    void onCopyAppLogWindow();

    void onClickGTAControllerLauncher();

    QAction * indentedView();

    /**
      * @brief : This function updates editor view and saves validation status to respective files. Function is usually run in a thread
      * @iValidator: Takes validator name who did the validation
      *	@iVal - List of file for which validation info has been updated
     */
    void onUpdateValidationInfo(QString validator,QStringList uuidList);
    /**
      * @brief : This function is reached after validation statu is updated. It triggers a editor window call
     */
    void onupdateValidationInfoFinished();
    void showQtInfo();

    void showExtLibsInfo();

    /**
      * This function edits the tag variables properties.
      */
    void onEditTagVariables();

    void onEditMaxTimeEstimated();

    /**
      * This function saves the updated taglist with porperties to GTAElement
      * @tagList: list of struct for tag variable properties
      */
    void onUserInputTagVarDesc(QString iFileName, QList<TagVariablesDesc> tagList);

    /**
      * This function populates the context menu in result page
      */
    void displayResultTabContextMenu(QPoint iPoint);

	void refreshTreeView();
	void onUpdateTreeItem();

    void onOpenContainingFolderFromTree();
    void onCommitChangesToGit();
    void onDiffFiles();
    void onAddFileForCommit();
    void onRemoveFileFromGitWorkspace();
    void UpdateGitActionMenuItems(QModelIndex index);
    void UpdateProcedureDashBoardMenuItem(QModelIndexList indexList);
    void onAddToExecutionList();

    void onOpenVersionTools() const;

public slots:

    void onRestoreFileWatcher();
    void onDisconnectFileWatcher();
    void onExportTestReportToCsv(bool silentMode = false);
    void onRenameDone(QString iName);
    void showBetweenSelectedRows();
    void onNewToolIdsAdded(const QList<GTASCXMLToolId> &iList);
    void onRemoveToolID(const QList<GTASCXMLToolId> &iList, const GTASCXMLToolId &iRemoveObj);
    void onUpdateClipBoard();
    void showHideHeaderColinDV();
    void disableEditingActions(bool bDisable);
    void toggleEditorActions(bool iToggleVal);
    void toggleTitleCollapseActionIcon(bool iset);
    void onUpdateCollapseToTitle(bool iVal);
    void onUpdateRowMoveDownAction(bool iVal);
    void onUpdateRowMoveUpAction(bool iVal);
    void setReadOnlyDocStatus(bool readOnlyStatus);
    void onUpdateActiveStack(QUndoStack * &undoStack);
    void onRemoveStack(QUndoStack * &undoStack);
    void onNewFileDetailsAdded();
    bool canCurrentValueBeEmpty(const QString& command);
    void onNewCsvToProConvertedFileDetailsAdded();
    void onSaveAsFileDetailsAdded();
    void onUpdateErrorLog(const ErrorMessageList &iErrorList);
    void onUpdateErrorLogMsg(QString iLogMsg);
    void onJumpToElement(const QString &iRelativeFilePath, const int &iLineNumber,GTAAppController::DisplayContext iDisplayContext,bool isUUID = false);
    void onJumpToElementHeader(const QString& iRelativeFilePath, const int& iLineNumber, GTAAppController::DisplayContext iDisplayContext, bool isUUID = false);
    void onElementCreatedForJump(GTAAppController::DisplayContext iDisplayContext);
    void onSequenceCreated();
    void onSaveToDocxReady();

    void onExportStarted(const QString& iSaveFolder,const QString& iFolderName,const QStringList& ilstOfDocuments, bool ibIncludeBDB, bool ibIncludeSCXML,bool ibCompress, const QString &iFileName,const QString &iLogFile);
    void onExportDocumentsFinished();
    void onSaveEditorDocFinished(const QString &iFilePath);
    void openDocument(const QString &fileName);
    void openFileByName(const QString& fileName);
    void onOpenInNewWindow(const QString &iFileRelPath,bool isUUID);
    void showProgressBar(const QString& iMsg, const QRect& pos);
    void hideProgressBar();
    void onStartPortingFiles(const QString &iPortDirPath);
    void onPortingFinished();
    void onWidgetClosed();
    void exportDoctoScxmlSlot();
    void updateSearchResults(const QHash<QString,SearchResults> &SearchResultMap, bool isHeader=false);

    void onDeleteSession(const QStringList &iSelectedItems);
    void onAddNewSession(const QString &iSessionName);
    void onSaveSession();
    void onSwitchSession(const QString &iSessionName);
    void onCloseSession();
    void onToggleLaunchButton(bool toggleStatus);

    void onOutputWindowOpened(bool isChecked);
    void onApplicationLogOpened(bool isChecked);
    void onLogWindowOpened(bool isChecked);

    void onSaveToDocxFinished();
    void onUpdateOutputWindow(const QStringList iMsgLst);
    void onUpdateApplicationLogs(const QStringList iMsgLst);
    /**
      * @brief : when log option has been reset to different option.
      *	@iLoglevel - new option selected by user.
      */
    void onLogMessageLevelUpdated(QString iLoglevel);

    /**
      * @brief : This function checks whether validation widget has been executed or not
      */
    void executeValidationWidget(bool isSwitchingView = false);

    /**
      * @brief : This function carries all function i.e. updation/display of validation info of respective files
      * @iValidator: Takes validator name who did the validation
      *	@iVal - List of file for which validation info has been updated
     */
    void onValidationWindowClosed(QString iValidator,QStringList iVal);

    void onSvnActionFinished();

    /**
      * This slot called for result toolbar action (expand/collapse all,Select/deselect all, new tab) and export LTRA/LTR action
     */
    void onResultToolbarClicked();

	void onImportAlreadyExists(QHash <QString, QString>);

    /**
      * @brief : This function refreshes the files system viewwer post plugin files saved
     */
    void onPluginFileSavingFinished();
	void onDuplicateUUIDFound(QString oldPath, QString newPath);
	void onImportFile();
	void onUpdateDataDB();
    void onCreateDataDB(bool message = true);
};

#endif // GTAMAINWINDOW_H
