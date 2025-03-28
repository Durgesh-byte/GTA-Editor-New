#include "GTAMainWindow.h"
#include "GTAAppController.h"
#include "GTASystemServices.h"
#include "GTAActionSelectorWidget.h"
#include "GTACheckWidget.h"
#include "GTACommentWidget.h"
#include "GTAGenestaImportWidget.h"
#include "GTAInitConfigWidget.h"
#include "GTASaveEditorWidget.h"
#include "GTASettingsWidget.h"
#include "GTACommandVariant.h"
#include "GTAUtil.h"
#include "GTAElementTreeViewWidget.h"
#include "GTAUITheme.h"
#include "GTAUserSettings.h"
#include "GTAMessageBox.h"
#include "GTAHeaderFieldEdit.h"
#include "GTASearchReplaceDialog.h"
#include "GTAElementTableView.h"
#include "GTAElementDependenciesWidget.h"
#include "GTAExportDocWidget.h"
#include "GTASVNListAllWidget.h"
#include "GTASVNResolveOptionDialog.h"
#include "GTAActionCall.h"
#include "GTAKeyBoardShortcutsWidget.h"
#include "GTALogFrame.h"
#include "GTACompatibilityReport.h"
#include "GTAUndoRedoAddCmd.h"
#include "GTAUndoRedoIgnoreOnOff.h"
#include "GTAUndoRedoToggleReadOnly.h"
#include "GTAUndoRedoCollapseToTitle.h"
#include "GTAUndoRedoAddRow.h"
#include "GTAUndoRedoDeleteRow.h"
#include "GTAUndoRedoClearRow.h"
#include "GTAUndoRedoCut.h"
#include "GTAUndoRedoPaste.h"
#include "GTAUndoRedoMoveRowsUp.h"
#include "GTAUndoRedoMoveRowsDown.h"
#include "GTAUndoRedoHideRow.h"
#include "GTAUndoRedoShowAllRows.h"
#include "GTAImportDialog.h"
#include "GTAEditorWindow.h"
#include "GTANewFileCreationDialog.h"
#include "GTASearchResults.h"
#include "GTAWidget.h"
#include "GTASessionManager.h"
#include "GTASavePrompt.h"
#include "GTATitleBasedExecution.h"
#include "GTAValidationListWidget.h"
#include "GTADebugger.h"
#include "GTAAppLogMsgRedirector.h"
#include "GTASVNAuthenticationWidget.h"
#include "GTASVNCheckForModificationsWidget.h"
#include "GTATagVariableDescWidget.h"
#include "GTATestConfig.h"
#include "GTAIgnoreManualAction.h"

#include "GTAGitController.h"
#include "GTAPurposeAndConclusionDialog.h"

#include <iostream>
#include <qinputdialog.h>
#include <cstdlib>

#pragma warning (push, 0)
#include "ui_GTAMainWindow.h"
#include <QFileDialog>
#include <QtConcurrentRun>
#include <QFuture>
#include <QFutureWatcher>
#include <QFileDialog>
#include <QHeaderView>
#include <QMessageBox>
#include <QSplashScreen>
#include <QMovie>
#include <QPixmap>
#include <QBitmap>
#include <QPointer>
#include <QFileInfo>
#include <QDesktopWidget>
#include <QListWidget>
#include <QTimer>
#include <QDesktopServices>
#include "QUrl"
#include <QTableView>
#include <QtAlgorithms>
#include <QSettings>
#include <QPixmap>
#include <QMutableListIterator>
#include <QSet>
#include <QPushButton>
#include <QFormLayout>
#include <qinputdialog.h>
#include <qprogressdialog.h>
#include <filesystem>
#include <QApplication>

namespace fs = std::filesystem;
#pragma warning (pop)

#define EDITOR_PAGE 0
#define RESULT_PAGE 1
#define LIVE_PALLETE 2

#define DOCK_WDGT_OUTPUT "Debug Output Window"
#define DOCK_WDGT_SEARCH_RESULTS "Search Results"
#define DOCK_WDGT_APPLICATION_LOGS "Application Log Window"
#define DOCK_WDGT_LOGS "Log Window"

const int RECENT_MAX_COUNT = 10;


GTAMainWindow::GTAMainWindow(QString app_config_file_path, QString bench_config_folder_path, QString bench_config_file_name, QWidget* parent) :
    QMainWindow(parent), _pProgramCB(nullptr), _pEquipmentCB(nullptr), _pStandardCB(nullptr), _pLoadProgAction(nullptr),
    ui(new Ui::GTAMainWindow)
{

    ui->setupUi(this);
    _pMdiController = GTAMdiController::getMDIController();
    _DockWidgetsMap.clear();
    _pResultMdiArea = GTAResultMdiArea::createResultMdiArea();

    // do not write any code before these lines. The order is important.

    TestConfig* testConf = TestConfig::getInstance(); // first

    _current_app_config_file_path = app_config_file_path;
    _current_bench_config_file_folder = bench_config_folder_path;
    _current_bench_config_file_name = bench_config_file_name;

    testConf->initialize(_current_bench_config_file_folder + _current_bench_config_file_name);

    connect(ui->CB_ini_files, SIGNAL(activated(int)), this, SLOT(updateIniFile()));
    connect(ui->button_browse_ini, SIGNAL(clicked()), this, SLOT(open_bench_file_selection()));
    connect(ui->button_refresh_ui, SIGNAL(clicked()), this, SLOT(refresh_ui()));

    connect(_pMdiController, SIGNAL(updateClipBoard()), this, SLOT(onUpdateClipBoard()));
    connect(_pMdiController, SIGNAL(updateEditingActions(bool)), this, SLOT(disableEditingActions(bool)));
    connect(_pMdiController, SIGNAL(toggleEditorActions(bool)), this, SLOT(toggleEditorActions(bool)));
    connect(_pMdiController, SIGNAL(updateTitleCollapseActionIcon(bool)), this, SLOT(toggleTitleCollapseActionIcon(bool)));
    connect(_pMdiController, SIGNAL(updateCollapseToTitle(bool)), this, SLOT(onUpdateCollapseToTitle(bool)));
    connect(_pMdiController, SIGNAL(updateMoveDownAction(bool)), this, SLOT(onUpdateRowMoveDownAction(bool)));
    connect(_pMdiController, SIGNAL(updateMoveUpAction(bool)), this, SLOT(onUpdateRowMoveUpAction(bool)));
    connect(_pMdiController, SIGNAL(refreshReadOnlyDocStatus(bool)), this, SLOT(setReadOnlyDocStatus(bool)));
    connect(_pMdiController, SIGNAL(updateActiveStack(QUndoStack*&)), this, SLOT(onUpdateActiveStack(QUndoStack*&)));
    connect(_pMdiController, SIGNAL(removeStack(QUndoStack*&)), this, SLOT(onRemoveStack(QUndoStack*&)));
    connect(_pMdiController, SIGNAL(sigCallActionDropped(int&, QString&)), this, SLOT(onCallActionDropped(int&, QString&)));
    connect(_pMdiController, SIGNAL(updateErrorLog(ErrorMessageList)), this, SLOT(onUpdateErrorLog(ErrorMessageList)));
    connect(_pMdiController, SIGNAL(updateErrorLogMsg(QString)), this, SLOT(onUpdateErrorLogMsg(QString)));
    connect(_pMdiController, SIGNAL(elementCreatedForJump(GTAAppController::DisplayContext)), this, SLOT(onElementCreatedForJump(GTAAppController::DisplayContext)));
    connect(_pMdiController, SIGNAL(saveEditorDocFinished(QString)), this, SLOT(onSaveEditorDocFinished(QString)));
    connect(_pMdiController, SIGNAL(openInNewWindow(QString, bool)), this, SLOT(onOpenInNewWindow(QString, bool)));
    connect(_pMdiController, SIGNAL(ShowProgressBar(QString)), this, SLOT(showProgressBar(QString)));
    connect(_pMdiController, SIGNAL(HideProgressBar()), this, SLOT(hideProgressBar()));
    connect(_pMdiController, SIGNAL(toggleLaunchButton(bool)), this, SLOT(onToggleLaunchButton(bool)));
    connect(_pMdiController, SIGNAL(updateOutputWindow(QStringList)), this, SLOT(onUpdateOutputWindow(QStringList)));
    connect(_pMdiController, &GTAMdiController::updateApplicationLogs, this, &GTAMainWindow::onUpdateApplicationLogs);

    _equipmentDataWatcher = new QFutureWatcher<void>();
    _pUndoStackGrp = new QUndoGroup(this);

    _pIgnoreOn = nullptr;
    _pIgnoreOff = nullptr;
    _pIndentedView = nullptr;
    _pOutputWindow = nullptr;
    _pApplicationLogs = nullptr;
    initMembers();

    this->setUITheme();


    _pExportLTRA = false;
    _pExportSCXML = false;

    _SaveStatus = QString("");

    createToolbars();
    createEditorToolBar();
    createTabContextMenu();
    createDVHeaderContextMenu(); // to create Document viewer columns depend

    createResultToolbars();

    _DataTypeExtMap.insert(ELEM_SEQ, "*.seq");
    _DataTypeExtMap.insert(ELEM_PROC, "*.pro");
    _DataTypeExtMap.insert(ELEM_FUNC, "*.fun");
    _DataTypeExtMap.insert(ELEM_OBJ, "*.obj");

    EnableExport(true);

    _pNewFileDialog = new GTANewFileCreationDialog(this);
    _pNewFileDialog->hide();
    connect(_pNewFileDialog, SIGNAL(newFileAccepted()), this, SLOT(onNewFileDetailsAdded()));
    connect(_pNewFileDialog, SIGNAL(saveAsAccepted()), this, SLOT(onSaveAsFileDetailsAdded()));
    connect(_pNewFileDialog, SIGNAL(sequenceCreated()), this, SLOT(onSequenceCreated()));
    connect(_pNewFileDialog, SIGNAL(saveAsDocxAccepted()), this, SLOT(onSaveToDocxReady()));
    connect(_pNewFileDialog, SIGNAL(newCsvToProConvertedFileAccepted()), this, SLOT(onNewCsvToProConvertedFileDetailsAdded()));

    _pExportDoctWidget = new GTAExportDocWidget();
    _pExportDoctWidget->hide();

    _ReportWidget = new GTACompatibilityReport(this);
    _ReportWidget->hide();
    //Dev comment : Krutika : commneted following line cause it was redundant as _ReportWidget is a dockable widget now
    //    connect(_ReportWidget,SIGNAL(onHide()),this,SLOT(resetEditorDisplayMode()));
    connect(_ReportWidget, SIGNAL(jumpToElement(QString, int, GTAAppController::DisplayContext, bool)), this, SLOT(onJumpToElement(QString, int, GTAAppController::DisplayContext, bool)));
    connect(_ReportWidget, SIGNAL(jumpToElementHeader(QString, int, GTAAppController::DisplayContext, bool)), this, SLOT(onJumpToElementHeader(QString, int, GTAAppController::DisplayContext, bool)));

    _pImportDialog = new GTAImportDialog(this);
    _pImportDialog->hide();

    _pRenameWidget = new GTARenameDialog(this);
    _pRenameWidget->hide();


    connect(ui->DataTV, SIGNAL(clicked(QModelIndex)), this, SLOT(onDataSelection(QModelIndex)));
    connect(ui->DataTV, SIGNAL(itemsDropped(QModelIndexList, QModelIndex)), this, SLOT(onDroppedFiles(QModelIndexList, QModelIndex)));

    //-------------------------------------------------------------------------------------
    //Dev comment : left to move
    connect(ui->actionScxml, SIGNAL(triggered()), this, SLOT(onExportPBClicked()));
    QShortcut* exportShortcut = new QShortcut(QKeySequence(Qt::Key_Control + Qt::Key_Shift + Qt::Key_E), this);
    connect(exportShortcut, SIGNAL(activated()), this, SLOT(onExportPBClicked()));

    //this->addAction(pTabCloseAct);
    //---------------------------------------------------------------------------------------------


    connect(_pRenameWidget, SIGNAL(renameDone(QString)), this, SLOT(onRenameDone(QString)));

    //-------------------------------------NEEDS TO CHANGE-------------------------------


    connect(ui->actionLaunch_Scxml, SIGNAL(triggered()), this, SLOT(onLaunchPBClicked()));
    connect(ui->actionLaunch_with_Sequencer, SIGNAL(triggered()), this, SLOT(onLaunchPBClicked()));
    //----------------------------------------------------------------------------------

    connect(ui->actionData_Browser, SIGNAL(toggled(bool)), this, SLOT(dockWidgetVisibility(bool)));
    connect(ui->DataBrowserDW, SIGNAL(visibilityChanged(bool)), this, SLOT(dockWidgetViewActionUpdate(bool)));


    ui->actionApplication_Logs->setObjectName(DOCK_WDGT_APPLICATION_LOGS);
    ui->actionApplication_Logs->setCheckable(true);
    ui->actionApplication_Logs->setChecked(false);
    connect(ui->actionApplication_Logs, SIGNAL(toggled(bool)), this, SLOT(onApplicationLogOpened(bool)));


    ui->actionLogView->setObjectName(DOCK_WDGT_LOGS);
    ui->actionLogView->setCheckable(true);
    ui->actionLogView->setChecked(false);
    connect(ui->actionLogView, SIGNAL(toggled(bool)), this, SLOT(onLogWindowOpened(bool)));


    connect(ui->actionUpdate_Database, SIGNAL(toggled(bool)), this, SLOT(updateDatabaseFromLib(bool)));
    connect(ui->DataTV, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onDoubleClickInTreeView(QModelIndex)));
    connect(ui->ExternalPluginImport, SIGNAL(triggered()), this, SLOT(onImport()));
    connect(_pImportDialog, SIGNAL(startImport()), this, SLOT(startImportingExternalFiles()));
    connect(_pImportDialog, SIGNAL(widgetclosed()), this, SLOT(onImportExternalFilesFinished()));
    //connect(_pImportWidget, SIGNAL(startGenestaImport()), this, SLOT(startImportingGenestaLibrary()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(onOpenDocumentFromFile()));
    connect(ui->actionOpen_bench_configuration, SIGNAL(triggered()), this, SLOT(open_bench_file_selection()));
    connect(ui->SearchTypeCB, SIGNAL(currentIndexChanged(int)), this, SLOT(onSearchTypeChanged(int)));
    //connect(ui->SearchTypeCB,SIGNAL(currentIndexChanged(QString)),this,SLOT(onSearchTypeChanged(QString)));
    connect(ui->actionEditorView, SIGNAL(triggered(bool)), this, SLOT(onActionEditorViewTriggered(bool)));
    connect(ui->actionResultAnalyzerView, SIGNAL(triggered(bool)), this, SLOT(onPerspectiveChanged(bool)));
    connect(ui->actionLiveView, &QAction::triggered, this, &GTAMainWindow::onPerspectiveChanged);
    connect(ui->actionTabbedView, SIGNAL(triggered()), this, SLOT(onMdiStyleChanged()));
    connect(ui->actionMultiWindowView, SIGNAL(triggered()), this, SLOT(onMdiStyleChanged()));

    ui->actionData_Browser->setChecked(true);
    ui->actionLogView->setChecked(false);
    ui->actionApplication_Logs->setChecked(false);

    QStringList dataTypeList = _DataTypeExtMap.keys();
    dataTypeList.prepend("All");
    dataTypeList.append("Favorites");
    ui->DataTypeCB->addItems(dataTypeList);


    connect(ui->DataSearchLE, SIGNAL(textEdited(QString)), this, SLOT(onDataSeachTextChange(QString)));
    connect(ui->DataTypeCB, SIGNAL(currentIndexChanged(QString)), this, SLOT(onDataTypeValChange(QString)));
    QShortcut* F2Shortcut = new QShortcut(QKeySequence(Qt::Key_F2), this);
    connect(F2Shortcut, SIGNAL(activated()), this, SLOT(onKeyF2Pressed()));

    QShortcut* F9Shortcut = new QShortcut(QKeySequence(Qt::Key_F9), this);
    connect(F9Shortcut, SIGNAL(activated()), this, SLOT(onKeyF9Pressed()));

    connect(ui->DataTV, SIGNAL(clicked(QModelIndex)), this, SLOT(populateDataDVContextMenu(QModelIndex)));
    connect(ui->actionConfiguration, SIGNAL(triggered()), this, SLOT(onSettingsActionClick()));
    connect(ui->actionTheme, SIGNAL(triggered()), this, SLOT(onThemeActionClick()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(onAboutClick()));
    connect(ui->actionKeyboard_Shortcuts, SIGNAL(triggered()), this, SLOT(onDisplayKeyboarShortCutHelp()));
    //    connect(ui->getICDCkB,SIGNAL(toggled(bool)),this,SLOT(onShowICDDetails(bool)));
    connect(ui->actionUser_Guide, SIGNAL(triggered()), this, SLOT(openUserGuide()));
    connect(ui->actionSession_Manager, SIGNAL(triggered()), this, SLOT(onSessionManagerSelected()));
    connect(ui->actionPrint, SIGNAL(triggered()), this, SLOT(onPrintElement()));

    connect(ui->actionVersion_Tools, SIGNAL(triggered()), this , SLOT(onOpenVersionTools()));

    //load the last saved theme
    GTAUITheme objTheme;
    objTheme.loadTheme();

    //show context menu
    connect(ui->DataTV, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(displayContextMenu(QPoint)));

    //show the context menu on header
    ui->DataTV->header()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->DataTV->header(), SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(displayHeaderContextMenu(QPoint)));
    _DVHeaderColList.clear();

    EnableExport(true);

    EnableFileEditorAction(false);

    _pProgressBar = new GTAProgress(nullptr);
    _TreeProgressBar = new GTAProgress(this);
    _pSearchReplaceDialog = new GTASearchReplaceDialog(this);
    _pActionGrp = new QActionGroup(this);
    _pActionGrp->setExclusive(true);



    connect(ui->actionSearch_Replace, SIGNAL(triggered()), this, SLOT(onSearchReplaceShow()));
    connect(_pSearchReplaceDialog, SIGNAL(searchNext(QRegExp, bool, bool)), this, SLOT(searchAndHighlightNext(QRegExp, bool, bool)));
    connect(_pSearchReplaceDialog, SIGNAL(searchHighlightAllString(QRegExp, bool)), this, SLOT(searchAndHighlightAll(QRegExp, bool)));
    connect(_pSearchReplaceDialog, SIGNAL(searchInAllDocuments(QRegExp, bool)), this, SLOT(searchInAllDocuments(QRegExp, bool)));
    connect(_pSearchReplaceDialog, SIGNAL(dlgCloseEvent()), this, SLOT(resetEditorDisplayMode()));
    connect(_pSearchReplaceDialog, SIGNAL(replaceSearchNext(QRegExp, QString, bool)), this, SLOT(replaceAndHighightNext(QRegExp, QString, bool)));
    connect(_pSearchReplaceDialog, SIGNAL(replaceAll(QRegExp, QString)), this, SLOT(replaceAllText(QRegExp, QString)));
    connect(_pSearchReplaceDialog, SIGNAL(replaceInAllDocuments(QRegExp, QString)), this, SLOT(replaceInAllDocuments(QRegExp, QString)));
    connect(_pSearchReplaceDialog, SIGNAL(replaceAllEquipment(QRegExp, QString)), this, SLOT(replaceAllEquipment(QRegExp, QString)));
    connect(_pSearchReplaceDialog, SIGNAL(searchRequirement(QRegExp)), this, SLOT(searchRequirement(QRegExp)));
    connect(_pSearchReplaceDialog, SIGNAL(searchTag(QRegExp)), this, SLOT(searchTag(QRegExp)));
    connect(_pSearchReplaceDialog, SIGNAL(searchComment(QRegExp)), this, SLOT(searchComment(QRegExp)));

    connect(_pExportDoctWidget, SIGNAL(startExporting(const QString&, const QString&, const QStringList&, bool, bool, bool, const QString&, const QString&)),
        this, SLOT(exportDocuments(const QString&, const QString&, const QStringList&, bool, bool, bool, const QString&, const QString&)));

    //    connect(ui->HeaderCB,SIGNAL(currentIndexChanged(const QString &)), this, SLOT(onLoadHeaderDoc(const QString &)) );
    //    connect(ui->toggleReadOnlyDocTB,SIGNAL(clicked()), this, SLOT(onReadOnlyDocToggled()) );

    connect(ui->actionView_Header, SIGNAL(triggered()), this, SLOT(onShowHeader()));
    connect(ui->actionSummary_conclusion, SIGNAL(triggered()), this, SLOT(onShowPurposeAndConclusion()));


    _F10Key = nullptr;
    _ShiftF10Key = nullptr;
    _F6Key = nullptr;
    _ShiftF6Key = nullptr;

    _F10Key = new QShortcut(QKeySequence(Qt::Key_F10), this);                    //Step next
    _ShiftF10Key = new QShortcut(QKeySequence(Qt::SHIFT + Qt::Key_F10), this);   //Step previos
    _F6Key = new QShortcut(QKeySequence(Qt::Key_F6), this);                      //continue or nxt breakpoint
    _ShiftF6Key = new QShortcut(QKeySequence(Qt::SHIFT + Qt::Key_F6), this);     //Stop the execution
    _ConvertCsv = new QShortcut(QKeySequence(Qt::ALT + Qt::Key_C), this);        //Convert Csv to scxml
    connect(_F10Key, SIGNAL(activated()), this, SLOT(onDebuggingShortcut()));
    connect(_ShiftF10Key, SIGNAL(activated()), this, SLOT(onDebuggingShortcut()));
    connect(_F6Key, SIGNAL(activated()), this, SLOT(onDebuggingShortcut()));
    connect(_ShiftF6Key, SIGNAL(activated()), this, SLOT(onDebuggingShortcut()));
    connect(_ConvertCsv, SIGNAL(activated()), this, SLOT(onConvertCsvToProClicked()));


    _titleCollapseToggled = false;
    _hideRowToggled = false;
    _pReadOnly = false;
    _ReadOnlyDocToggled = false;
    setReadOnlyDocStatus(_ReadOnlyDocToggled);

    _pClipBoardWindow = new QTableView;
    _pShortcuthelpWindow = nullptr;
    ui->DataSearchLE->clear();
    /*readSettings();*/
    ui->MultiPerspectiveWidget->setCurrentIndex(EDITOR_PAGE);
    _selectedForSeqList.clear();
    ui->ResultToolBar->setVisible(false);
    ui->ResultToolBar->setEnabled(false);


    _pActivePerspectiveViewGrp = new QActionGroup(this);
    _pActivePerspectiveViewGrp->addAction(ui->actionEditorView);
    _pActivePerspectiveViewGrp->addAction(ui->actionResultAnalyzerView);
    _pActivePerspectiveViewGrp->addAction(ui->actionLiveView);
    ui->actionEditorView->setCheckable(true);
    ui->actionResultAnalyzerView->setCheckable(true);
    ui->actionLiveView->setCheckable(true);
    _pActivePerspectiveViewGrp->setExclusive(true);

    _pActiveSubwindowViewGrp = new QActionGroup(this);
    _pActiveSubwindowViewGrp->addAction(ui->actionTabbedView);
    _pActiveSubwindowViewGrp->addAction(ui->actionMultiWindowView);
    ui->actionTabbedView->setCheckable(true);
    ui->actionMultiWindowView->setCheckable(true);
    _pActiveSubwindowViewGrp->setExclusive(true);
    ui->actionTabbedView->setChecked(true);

    ui->actionEditorView->setChecked(true);

    connect(ui->importPB, SIGNAL(clicked()), this, SLOT(onImportFile()));
    connect(ui->updatePB, SIGNAL(clicked()), this, SLOT(onUpdateDataDB()));
    connect(ui->createPB, SIGNAL(clicked()), this, SLOT(onCreateDataDB()));

    connect(_pNewFileDialog, SIGNAL(restore_file_watcher()), this, SLOT(onRestoreFileWatcher()));
    connect(_pNewFileDialog, SIGNAL(disconnect_file_watcher()), this, SLOT(onDisconnectFileWatcher()));

    connect(ui->EditorFrame, SIGNAL(closeWindowAndTab()), this, SLOT(onClose()));
    connect(ui->EditorFrame, SIGNAL(displayTabContextMenu(QPoint)), this, SLOT(displayTabContextMenu(QPoint)));

    // Right-click menu show/hide
    ui->menuShow_Hide->addSeparator();
    _toolBars = this->findChildren<QToolBar*>();
    _rightClickPos = QPoint();
    foreach(QToolBar* toolBar, _toolBars)
    {
        QAction* toolBarAction = ui->menuShow_Hide->addAction(QIcon(), toolBar->windowTitle());
        toolBarAction->setCheckable(true);
        toolBarAction->setChecked(toolBar->isEnabled());
        connect(toolBarAction, SIGNAL(toggled(bool)), toolBar, SLOT(setVisible(bool)));
    }

    _pGTAControllerLauncherWidget = new GTAControllerLauncherWidget(this);
}

/**
* @brief Filter mouse press event in order to keep the menu visible on right click until all preferences are set
* @param event the mouse event on button press
*/
void GTAMainWindow::mousePressEvent(QMouseEvent* event)
{
    // List of widgets where the main context menu can be requested
    QList listOfObjNames = { ui->actionData_Browser->text(), ui->actionApplication_Logs->objectName(), ui->actionLogView->objectName()};
    foreach(QToolBar* toolBar, _toolBars)
        listOfObjNames.append(toolBar->objectName());
    
    if (event->button() == Qt::RightButton)
    {
        QWidget* clickedWidget = this->childAt(event->pos());

        bool isQToolButton = clickedWidget->metaObject()->className() == QToolButton().metaObject()->className();
        bool widgetInList = listOfObjNames.contains(clickedWidget->objectName()) || listOfObjNames.contains(clickedWidget->windowTitle());
        bool pWidgetInList = listOfObjNames.contains(clickedWidget->parentWidget()->objectName()) && isQToolButton;

        if (widgetInList || pWidgetInList)
        {
            // Save the click position
            _rightClickPos = event->pos();

            // Set connections
            connect(ui->actionData_Browser, SIGNAL(toggled(bool)), this, SLOT(onVisibilityChanged()));
            connect(ui->actionApplication_Logs, SIGNAL(toggled(bool)), this, SLOT(onVisibilityChanged()));
            connect(ui->actionLogView, SIGNAL(toggled(bool)), this, SLOT(onVisibilityChanged()));
            foreach(QToolBar* toolBar, _toolBars)
                connect(toolBar, SIGNAL(visibilityChanged(bool)), this, SLOT(onVisibilityChanged()));

            // Display menu
            _nbActionsTriggered = 0;
            this->setContextMenuPolicy(Qt::NoContextMenu);              // do not execute the inner contextual menu
            ui->menuShow_Hide->exec(this->mapToGlobal(_rightClickPos)); // but display the edited menu instead
        }
        else
        {
            hideRightClickMenu();
        }
    }
    else
    {
        hideRightClickMenu();
    }
}

/**
* @brief Slot on widget visibility change
*/
void GTAMainWindow::onVisibilityChanged()
{
    foreach(QToolBar * toolBar, _toolBars)
    {
        if (toolBar->isHidden())
        {
            this->removeToolBar(toolBar);
        }
        else
        {
            this->addToolBar(toolBar);
        }
    }

    // TODO: Investigate why the application crashes when clicking over than about 30 clicks.
    // A temporary solution is to force the menu to be closed after 25 clicks.
    _nbActionsTriggered++;
    if (_rightClickPos != QPoint() && _nbActionsTriggered < 25)
    {
        ui->menuShow_Hide->exec(this->mapToGlobal(_rightClickPos));
    }
    else
    {
        hideRightClickMenu();
    }
}

/**
* @brief Hide the right click menu and remove connections
*/
void GTAMainWindow::hideRightClickMenu()
{
    _rightClickPos = QPoint();
    _nbActionsTriggered = 0;
    ui->menuShow_Hide->hide();
    ui->menuShow_Hide->close();
}

bool GTAMainWindow::checkApplicationConfiguration(QString& isErrorMsg)
{
    bool rc = false;
    GTASystemServices* psystemServices = GTASystemServices::getSystemServices();
    if (nullptr!=psystemServices)
    {

        rc = psystemServices->toolFilesExists(isErrorMsg);
        // psystemServices->deleteLater();
    }
    else
        isErrorMsg = "System service initialisation failed";

    return rc;

}

void GTAMainWindow::on_actionSearch_Replace_triggered()
{
    _pSearchReplaceDialog->show();
}

void GTAMainWindow::onSearchReplaceShow()
{
    _pSearchReplaceDialog->setDefaultTab();
    _pSearchReplaceDialog->show();
}

GTAMainWindow::~GTAMainWindow()
{
    if(_ReportWidget != nullptr)
    {
        _ReportWidget->setVisible(false);
    }

    if (_pAppController)
    {
        showProgressBar("Please wait while GTA closes", this->geometry());
        delete _pAppController;
        hideProgressBar();
    }
	if (_pMdiController)
	{
		delete _pMdiController;
		_pMdiController = nullptr;
	}


    if(_pProgramCB != nullptr ) delete _pProgramCB;
    if(_pEquipmentCB != nullptr ) delete _pEquipmentCB;
    if(_pStandardCB != nullptr ) delete _pStandardCB;
    if(_pLoadProgAction != nullptr ) delete _pLoadProgAction;
    if(_pLaunchScxmlAction != nullptr) delete _pLaunchScxmlAction;
    if(_pTitleBasedExecution != nullptr)
    {
        delete _pTitleBasedExecution;
        _pTitleBasedExecution = nullptr;
    }
    if(_pDebugAction != nullptr){delete _pDebugAction; _pDebugAction = nullptr;}
    if(_pLaunchSequencerAction != nullptr) delete _pLaunchSequencerAction;
    if(_pStopSCXML != nullptr) delete _pStopSCXML;
    if(_pCheckCompatibilityAction != nullptr) delete _pCheckCompatibilityAction;
    if(_pFileSystemHeader != nullptr ) delete _pFileSystemHeader;
    if(_pProgressBar != nullptr) {delete _pProgressBar; _pProgressBar = nullptr;}
    if(_TreeProgressBar != nullptr) {delete _TreeProgressBar; _TreeProgressBar = nullptr;}
    if(_pExportDoctWidget != nullptr) delete _pExportDoctWidget;
    if (_ReportWidget!=nullptr) delete _ReportWidget;
    if(_pImportDialog != nullptr) {delete _pImportDialog; _pImportDialog = nullptr;}
    if(_pTabContextMenu != nullptr) {delete _pTabContextMenu; _pTabContextMenu = nullptr;}
    if(_pDebugWindowContextMenu != nullptr){delete _pDebugWindowContextMenu; _pDebugWindowContextMenu = nullptr;}
    if(_pAppLogWindowContextMenu != nullptr) {delete _pAppLogWindowContextMenu; _pAppLogWindowContextMenu = nullptr;}
    if(_pActionGrp != nullptr) {delete _pActionGrp; _pActionGrp = nullptr;}
    if(_pApplicationLogs != nullptr){delete _pApplicationLogs; _pApplicationLogs = nullptr;}
    if(_pOutputWindow != nullptr){delete _pOutputWindow; _pOutputWindow = nullptr;}
    if(_pContextMenuOnDVHeader != nullptr) {delete _pContextMenuOnDVHeader; _pContextMenuOnDVHeader =nullptr;}
    if (_pExpandAllResAction != nullptr) { delete _pExpandAllResAction; _pExpandAllResAction = nullptr; }
    if (_pCollapseAllResAction != nullptr) { delete _pCollapseAllResAction; _pCollapseAllResAction = nullptr; }
    if (_pSelectAllResAction != nullptr) { delete _pSelectAllResAction; _pSelectAllResAction = nullptr; }
    if (_pDeSelectAllResAction != nullptr) { delete _pDeSelectAllResAction; _pDeSelectAllResAction = nullptr; }
    if (_pNewResTabAction != nullptr) { delete _pNewResTabAction; _pNewResTabAction = nullptr; }
    if (_pResultTabContextMenu != nullptr) { delete _pResultTabContextMenu; _pResultTabContextMenu = nullptr; }
    if (_pActivePerspectiveViewGrp != nullptr)
    {
        _pActivePerspectiveViewGrp->removeAction(ui->actionEditorView);
        _pActivePerspectiveViewGrp->removeAction(ui->actionResultAnalyzerView);
        _pActivePerspectiveViewGrp->removeAction(ui->actionLiveView);
        delete _pActivePerspectiveViewGrp; _pActivePerspectiveViewGrp = nullptr;
    }
    if (_pActiveSubwindowViewGrp != nullptr)
    {
        _pActiveSubwindowViewGrp->removeAction(ui->actionTabbedView);
        _pActiveSubwindowViewGrp->removeAction(ui->actionMultiWindowView);
        delete _pActiveSubwindowViewGrp; _pActiveSubwindowViewGrp = nullptr;
    }
    qDeleteAll(_DockWidgetsMap);
    qDeleteAll(_recentSessions);

    // take care of TestConfig singleton's memory
    TestConfig::resetInstance();

    delete ui;

    initMembers();
}

void GTAMainWindow::initMembers()
{
    _pAppController=nullptr;
    _pProgramCB=nullptr;
    _pEquipmentCB=nullptr;
    _pStandardCB=nullptr;
    _pLoadProgAction=nullptr;
    _pTitleBasedExecution = nullptr;
    _pDebugAction = nullptr;
    _pLaunchScxmlAction =nullptr;
    _pLaunchSequencerAction = nullptr;
    _pStopSCXML = nullptr;
    _pCheckCompatibilityAction = nullptr;
    _pFileSystemHeader = nullptr;
    _pProgressBar  = nullptr;
    _TreeProgressBar = nullptr;
    _ReportWidget = nullptr;
    _pDebugWindowContextMenu = nullptr;
    _pAppLogWindowContextMenu = nullptr;
    _pClearDebugWindow = nullptr;
    _pCopydebugWindowText = nullptr;
    _pExpandAllResAction = nullptr;
    _pCollapseAllResAction = nullptr;
    _pSelectAllResAction = nullptr;
    _pDeSelectAllResAction = nullptr;
    _pNewResTabAction = nullptr;
    _pResultTabContextMenu = nullptr;
}

void GTAMainWindow::onDataSelection(QModelIndex iIndex)
{
    EnableFileEditorAction(false);
    if((iIndex.isValid()) && (iIndex.row() >= 0) && (iIndex.column() == 0 || iIndex.column() == 1))
    {
        EnableFileEditorAction(true);
    }
}

void GTAMainWindow::createDVHeaderContextMenu()
{
    _pContextMenuOnDVHeader = new QMenu();

    QAction* EntityName = _pContextMenuOnDVHeader->addAction(QIcon(),"Name",this,SLOT(showHideHeaderColinDV()));
    QAction* Description = _pContextMenuOnDVHeader->addAction(QIcon(),"Description",this,SLOT(showHideHeaderColinDV()));
    QAction* GtaVersion = _pContextMenuOnDVHeader->addAction(QIcon(), "GTA Version", this, SLOT(showHideHeaderColinDV()));
    QAction* UuidName = _pContextMenuOnDVHeader->addAction(QIcon(),"UUID",this,SLOT(showHideHeaderColinDV()));
    QAction* CreatedDate = _pContextMenuOnDVHeader->addAction(QIcon(),"Date Created",this,SLOT(showHideHeaderColinDV()));
    QAction* AuthorName = _pContextMenuOnDVHeader->addAction(QIcon(),"Author",this,SLOT(showHideHeaderColinDV()));
    QAction* ModifiedDate = _pContextMenuOnDVHeader->addAction(QIcon(),"Last Modified",this,SLOT(showHideHeaderColinDV()));
    QAction* ValidationStats = _pContextMenuOnDVHeader->addAction(QIcon(),"Validation Status",this,SLOT(showHideHeaderColinDV()));
    QAction* Validator = _pContextMenuOnDVHeader->addAction(QIcon(),"Validator",this,SLOT(showHideHeaderColinDV()));
    QAction* ValidatedDate = _pContextMenuOnDVHeader->addAction(QIcon(),"Date Validated",this,SLOT(showHideHeaderColinDV()));
	//QAction* Favourites = _pContextMenuOnDVHeader->addAction(QIcon(),"Favourite",this,SLOT(showHideHeaderColinDV()));
    QAction* GitStatus = _pContextMenuOnDVHeader->addAction(QIcon(), "Git status", this, SLOT(showHideHeaderColinDV()));

    //Showing name columns always ticked but disabled
    EntityName->setCheckable(true);
    EntityName->setDisabled(true);
    EntityName->setChecked(true);           
    EntityName->setToolTip("Always Enabled");

    //Allow checking of other column filters
    Description->setCheckable(true);    
    AuthorName->setCheckable(true);      
    /*Favourites->setCheckable(true);*/     
    UuidName->setCheckable(true);
    Validator->setCheckable(true);     
    ValidationStats->setCheckable(true);     
    ValidatedDate->setCheckable(true);  
    ModifiedDate->setCheckable(true);
    CreatedDate->setCheckable(true);     
    GtaVersion->setCheckable(true);
    GitStatus->setCheckable(true);

    //Keep column filters unchecked by default
    Description->setChecked(false);     
    AuthorName->setChecked(false);      
    /*Favourites->setChecked(false);*/      
    UuidName->setChecked(false);
    Validator->setChecked(false);      
    ValidationStats->setChecked(false);      
    ValidatedDate->setChecked(false);  
    ModifiedDate->setChecked(false);
    CreatedDate->setChecked(false);      
    GtaVersion->setChecked(false);
    GitStatus->setChecked(false);

}

void GTAMainWindow::showHideHeaderColinDV()
{
    QObject *pSender = sender();
    QAction *pAction = dynamic_cast<QAction*>(pSender);
    QString actionText = pAction->text();
    int index = -1;
    if(_pAppController)
    {
        if (_pAppController->getViewController())
        {
            if (!_pAppController->getViewController()->getColumnList().isEmpty())
            {
                index = _pAppController->getViewController()->getColumnList().indexOf(actionText);

                if(pAction != nullptr)
                {
                    int count = _pAppController->getViewController()->getColumnList().count();
                    if(index >= 0 && index < count)
                    {
                        bool isCheck = pAction->isChecked();
                        saveDVSelectionSettings(index,actionText,pAction->isChecked(),count);

                        //show/hide the respective header column
                        ui->DataTV->setColumnHidden(index, !(pAction->isChecked()));
                        ui->DataTV->resizeColumnToContents(index);
                        //save the header back to the settings as ByteArray for later use
                        QHeaderView *DataTVHeader = ui->DataTV->header();
                        QByteArray dataTVHeaderStatus;
                        if(DataTVHeader)
                            dataTVHeaderStatus = DataTVHeader->saveState();

                        QByteArray hex_array = dataTVHeaderStatus.toHex();
                        std::string dataTVHeaderStatus_str(hex_array.constData(), hex_array.length());
                        TestConfig::getInstance()->setDataTVGeometry(dataTVHeaderStatus_str);
                    }
                }
            }
        }
    }
}

void GTAMainWindow::saveDVSelectionSettings(int iIndex,QString iactionText, bool iIsChecked, int iListSize)
{
    if (iIndex >= 0 && iIndex < iListSize)
    {
        if (_DVHeaderColList.contains(iactionText) && !iIsChecked)
        {
            _DVHeaderColList.removeAll(iactionText);
        }
        else
        {
            _DVHeaderColList.append(iactionText);
        }

        QString HeaderList;
        for (int i = 0; i < _DVHeaderColList.count(); i++)
            HeaderList.append(QString("%1:").arg(_DVHeaderColList.at(i)));

        TestConfig::getInstance()->setDVSavedList(HeaderList.toStdString());
    }
}

/**
* @brief Initializes the right-click context menu of the procedure tabwidget.
*/
void GTAMainWindow::createTabContextMenu()
{
    _pTabContextMenu = new QMenu();
    _pSave = _pTabContextMenu->addAction(QIcon(),"Save",this,SLOT(onSaveEditorDoc()));
    _pClose = _pTabContextMenu->addAction(QIcon(),"Close",this,SLOT(onClose()));
    _pCloseAll = _pTabContextMenu->addAction(QIcon(),"Close all",this, SLOT(onCloseAllSubwindows()));
    _pCloseAllButThis = _pTabContextMenu->addAction(QIcon(),"Close all but this",this, SLOT(onCloseAllButThis()));
    _pTabContextMenu->addSeparator();

    _pMoveView = _pTabContextMenu->addAction(QIcon(), "Move to other view", this, SLOT(moveToOtherView()));
    _pTabContextMenu->addSeparator();

    _pEditTagVariables = _pTabContextMenu->addAction(QIcon(),"Edit Tag Variables Properties",this,SLOT(onEditTagVariables()));
    _pTabContextMenu->addSeparator();

    _pEditMaxTimeEstimated = _pTabContextMenu->addAction(QIcon(), "Edit Maximum Time Estimated", this, SLOT(onEditMaxTimeEstimated()));
    _pTabContextMenu->addSeparator();

    _pOpenContainingFolder = _pTabContextMenu->addAction(QIcon(),"Open containing folder",this, SLOT(onOpenContainingFolder()));
    _pTabContextMenu->addSeparator();

    _pSwitchToEditorMode = _pTabContextMenu->addAction(QIcon(),"Switch to edit mode",this,SLOT(onSwitchToEditorView()));
    _pTabContextMenu->addSeparator();

    _pEnableDisableUnsubscribe = _pTabContextMenu->addAction(QIcon(), "Enable/Disable Unsubscribe", this,SLOT(onEnableDisableUnsubscribe()));
    _pTabContextMenu->addSeparator();

    _pSelectManActToBeIgnored = _pTabContextMenu->addAction(QIcon(), "Enable/Disable Manual Actions", this, SLOT(onSelectManActToBeIgnored()));
    _pTabContextMenu->addSeparator();

    _pViewResult = _pTabContextMenu->addAction(QIcon(),"View Results",this,SLOT(onViewResultsClicked()));

    _pResultTabContextMenu = new QMenu();
     _pResultTabContextMenu->addAction(QIcon(), "Close", this, SLOT(onClose()));
	_pResultTabContextMenu->addAction(QIcon(), "Close All", this, SLOT(onCloseAllSubwindows()));
	_pResultTabContextMenu->addAction(QIcon(), "Close all but this", this, SLOT(onCloseAllButThis()));
}

/**
* @brief Updates the clickability of the _pMoveView action in the context menu according to the number of windows.
*/
void GTAMainWindow::updateMoveUI()
{
    uint viewsCount = ui->EditorFrame->getViewsCount();
    if (viewsCount == 0 || viewsCount == 1 && ui->EditorFrame->getTabWidget(0)->count() <= 1)
        _pMoveView->setEnabled(false);
    else
        _pMoveView->setEnabled(true);
}

/**
* @brief Create a new window in the procedure editor and add it to the editor frame.
* @param iFilename Name of the document.
* @return Created new window.
*/
GTAEditorWindow* GTAMainWindow::createNewSubWindow(const QString& iFilename)
{
    std::optional<uint> viewIndexWrapper = ui->EditorFrame->getCurrentTabIndices().viewIndex;
    uint viewIndex = (!viewIndexWrapper.has_value()) ? 0 : *viewIndexWrapper;

    GTAEditorWindow* pNewWindow = new GTAEditorWindow(this, iFilename);
    ui->EditorFrame->addWindow(pNewWindow, iFilename, viewIndex);
    pNewWindow->setFramePosition(ui->EditorFrame->getCurrentTabIndices());
    updateMoveUI();
    return pNewWindow;
}

/**
* @brief Closes a single selected procedure window.
*/
void GTAMainWindow::onClose()
{
    int index = ui->MultiPerspectiveWidget->currentIndex();
    if (index == RESULT_PAGE) {
        QMdiSubWindow* pActiveSubWindow = _pResultMdiArea->activeSubWindow();
        if (pActiveSubWindow)
            pActiveSubWindow->close();
    }
    else {
        GTAEditorWindow* activeWindow = qobject_cast<GTAEditorWindow*>(ui->EditorFrame->getCurrentWindow());
        if (activeWindow && activeWindow->isFileModified())
        {
            int retVal = QMessageBox::warning(this, QString("Save"), QString("File Modified. Save changes to %1?").arg(activeWindow->getElementName()), QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
            if (retVal == QMessageBox::Yes)
            {
                activeWindow->onSave();
            }
            else if (retVal == QMessageBox::Cancel)
                return;
        }
        TabIndices tabIndices = ui->EditorFrame->getCurrentTabIndices();
        if (tabIndices.viewIndex.has_value() && tabIndices.tabIndex.has_value())
        {
            ui->EditorFrame->closeTab(*tabIndices.viewIndex, *tabIndices.tabIndex);

            QTabWidget* currentTabWidget = ui->EditorFrame->getTabWidget(*tabIndices.viewIndex);
            if (ui->EditorFrame->getViewsCount() > 1 && currentTabWidget->count() == 0)
            {
                ui->EditorFrame->closeView(*tabIndices.viewIndex);
                ui->EditorFrame->updateWindowsPosition(0, -1);
            }
            else
            {
                int tabIndex;
                if (*tabIndices.tabIndex == 0 || ui->EditorFrame->getTabWidget(*tabIndices.viewIndex)->count() < 2)
                    tabIndex = 0;
                else
                    tabIndex = *tabIndices.tabIndex;

                ui->EditorFrame->updateWindowsPosition(*tabIndices.viewIndex, tabIndex);
            }
            updateMoveUI();
        }

        if (!ui->EditorFrame->isAnyWindowOpened()) {
            onLastWindowClosed();
        }
    }

}

/**
 * @brief Clean up after last editor window was closed
*/
void GTAMainWindow::onLastWindowClosed() {
    ui->EditorFrame->deleteAllViews();
    _pMdiController->onUpdateActiveViewController(nullptr);
}

/**
* @brief saves all modified files in case of close all or close all but this commands.
*/
void GTAMainWindow::saveModifiedFiles()
{
    for (uint viewIndex = 0; viewIndex < ui->EditorFrame->getViewsCount(); viewIndex++)
    {
        QList<GTAEditorWindow*> windowsList = ui->EditorFrame->getAllWindows(viewIndex);
        for (auto window : windowsList)
        {
            if (window->isFileModified())
            {
                int retVal = QMessageBox::warning(this, QString("Save"), QString("File Modified. Save changes to %1?").arg(window->getElementName()), QMessageBox::Yes, QMessageBox::No);
                if (retVal == QMessageBox::Yes)
                {
                    window->onSave();
                }
            }
        }
    }
}

/**
* @brief Closes all the procedure windows of the currently focused view.
*/
void GTAMainWindow::onCloseAllSubwindows()
{
    int index = ui->MultiPerspectiveWidget->currentIndex();
    if (index == EDITOR_PAGE)
    {
        saveModifiedFiles();
        TabIndices tabIndices = ui->EditorFrame->getCurrentTabIndices();
        if (tabIndices.viewIndex.has_value())
        {
            ui->EditorFrame->closeAllTabs(*tabIndices.viewIndex);
            if (ui->EditorFrame->getViewsCount() > 1)
            {
                ui->EditorFrame->closeView(*tabIndices.viewIndex);
                ui->EditorFrame->updateWindowsPosition(0, -1);
            }
            updateMoveUI();
        }
    }
    else if(index == RESULT_PAGE)
    {
        QList<QMdiSubWindow*> subWindowLst = _pResultMdiArea->subWindowList(QMdiArea::ActivationHistoryOrder);
        std::for_each(subWindowLst.begin(), subWindowLst.end(), [&](auto w) {w->close(); });
    }
    _pUndoStackGrp->setActiveStack(nullptr);

    if (!ui->EditorFrame->isAnyWindowOpened()) {
        onLastWindowClosed();
    }
}

/**
* @brief Closes all the procedure windows of the currently focused view except the window in focus.
*/
void GTAMainWindow::onCloseAllButThis()
{
    int index = ui->MultiPerspectiveWidget->currentIndex();
    if (index == EDITOR_PAGE)
    {
        saveModifiedFiles();
        TabIndices tabIndices = ui->EditorFrame->getCurrentTabIndices();
        if (tabIndices.viewIndex.has_value() && tabIndices.tabIndex.has_value())
        {
            QTabWidget* currentTabWidget = ui->EditorFrame->getTabWidget(*tabIndices.viewIndex);
            GTAEditorWindow* pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(currentTabWidget->widget(*tabIndices.tabIndex));
            ui->EditorFrame->closeAllTabs(*tabIndices.viewIndex, *tabIndices.tabIndex);
            pCurrSubWindow->setFramePosition(TabIndices(*tabIndices.viewIndex, 0));
            ui->EditorFrame->updateWindowsPosition(*tabIndices.viewIndex, 0);
            updateMoveUI();
        }
    }
    else if (index == RESULT_PAGE)
    {
        QList<QMdiSubWindow*> subWindowLst = _pResultMdiArea->subWindowList(QMdiArea::ActivationHistoryOrder);
        QMdiSubWindow* pActiveSubWindow = _pResultMdiArea->activeSubWindow();
        subWindowLst.removeOne(pActiveSubWindow);
        std::for_each(subWindowLst.begin(), subWindowLst.end(), [&](auto w) {w->close(); });
    }
    _pUndoStackGrp->setActiveStack(nullptr);
}

/**
* @brief Slot that moves a specific window to the other available view, thereby creating a split or removing it if needed.
*/
void GTAMainWindow::moveToOtherView()
{
    GTAEditorWindow* currentWindow = dynamic_cast<GTAEditorWindow*>(ui->EditorFrame->getCurrentWindow());
    if (currentWindow != nullptr)
    {
        // retrieve current window infos
        QString relativePath = currentWindow->getRelativePath();
        QString title = currentWindow->windowTitle();
        TabIndices windowIndices = currentWindow->getFramePosition();

        // move the window
        uint viewIndex = (windowIndices.viewIndex == 0) ? 1 : 0;
        ui->EditorFrame->addWindow(std::move(currentWindow), title, viewIndex);
        currentWindow->setFramePosition(ui->EditorFrame->getCurrentTabIndices());

        // close the view if there are no more tabs
        if (ui->EditorFrame->getTabWidget(*windowIndices.viewIndex)->count() == 0)
        {
            ui->EditorFrame->closeView(*windowIndices.viewIndex);
            ui->EditorFrame->updateWindowsPosition(0, -1);
        }
        ui->EditorFrame->focusCurrent();
    }
}

void GTAMainWindow::onSwitchToEditorView()
{
    GTAEditorWindow *pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if (pCurrSubWindow != nullptr)
    {
        pCurrSubWindow->onEscapeKeyTriggered();
    }
}

void GTAMainWindow::onEnableDisableUnsubscribe()
{
    GTAEditorWindow* currentWindow = dynamic_cast<GTAEditorWindow*>(ui->EditorFrame->getCurrentWindow());
    if (currentWindow != nullptr)
    {
        QDialog dialog(this);
        dialog.setWindowTitle("Enable/Disable Unsubscribe");

        QVBoxLayout mainLayout(&dialog);
        QGridLayout gridLayout;

        QCheckBox checkBoxStartEnable("Enable", &dialog);
        QCheckBox checkBoxStartDisable("Disable", &dialog);
        gridLayout.addWidget(new QLabel("Unsubscribe At Start:"), 0, 0);
        gridLayout.addWidget(&checkBoxStartEnable, 0, 1);
        gridLayout.addWidget(&checkBoxStartDisable, 0, 2);

        QCheckBox checkBoxEndEnable("Enable", &dialog);
        QCheckBox checkBoxEndDisable("Disable", &dialog);
        gridLayout.addWidget(new QLabel("Unsubscribe At End:"), 1, 0);
        gridLayout.addWidget(&checkBoxEndEnable, 1, 1);
        gridLayout.addWidget(&checkBoxEndDisable, 1, 2);

        QCheckBox checkBoxFileParamOnlyEable("Enable", &dialog);
        QCheckBox checkBoxFileParamOnlyDisable("Disable", &dialog);
        gridLayout.addWidget(new QLabel("Unsubscribe File Param Only:"), 2, 0);
        gridLayout.addWidget(&checkBoxFileParamOnlyEable, 2, 1);
        gridLayout.addWidget(&checkBoxFileParamOnlyDisable, 2, 2);

        mainLayout.addLayout(&gridLayout);

        bool startEnable = currentWindow->getUnsubscribeAtStartResults();
        bool startDisable = !startEnable;
        bool endEnable = currentWindow->getUnsubscribeAtEndResults();
        bool endDisable = !endEnable;
        bool unsubscribeFileParamOnlyEnable = currentWindow->getUnsubscribeFileParamOnly();
        bool unsubscribeFileParamOnlyDisable = !unsubscribeFileParamOnlyEnable;

        _startUnsubscribeEnable = startEnable;
        _endUnsubscribeEnable = endEnable;
        _unsubscribeFileParamOnlyEnable = unsubscribeFileParamOnlyEnable;

        checkBoxStartEnable.setChecked(startEnable);
        checkBoxStartDisable.setChecked(startDisable);
        checkBoxEndEnable.setChecked(endEnable);
        checkBoxEndDisable.setChecked(endDisable);
        checkBoxFileParamOnlyEable.setChecked(unsubscribeFileParamOnlyEnable);
        checkBoxFileParamOnlyDisable.setChecked(unsubscribeFileParamOnlyDisable);

        if (endDisable)
        {
            checkBoxFileParamOnlyDisable.setEnabled(false);
            checkBoxFileParamOnlyEable.setEnabled(false);
        }

        QObject::connect(&checkBoxStartEnable, &QCheckBox::toggled, [&checkBoxStartDisable](bool checked) {
            checkBoxStartDisable.setChecked(!checked);
            });
        QObject::connect(&checkBoxStartDisable, &QCheckBox::toggled, [&checkBoxStartEnable](bool checked) {
            checkBoxStartEnable.setChecked(!checked);
            });

        QObject::connect(&checkBoxEndEnable, &QCheckBox::toggled, [&checkBoxEndDisable,
            &checkBoxFileParamOnlyEable, &checkBoxFileParamOnlyDisable](bool checked) {
            checkBoxEndDisable.setChecked(!checked);

            if (checkBoxFileParamOnlyEable.isEnabled())
            {
                checkBoxFileParamOnlyEable.setChecked(checked);
            }
            checkBoxFileParamOnlyDisable.setEnabled(checked);
            checkBoxFileParamOnlyEable.setEnabled(checked);
            });
        QObject::connect(&checkBoxEndDisable, &QCheckBox::toggled, [&checkBoxEndEnable,
            &checkBoxFileParamOnlyEable, &checkBoxFileParamOnlyDisable](bool checked) {
            checkBoxEndEnable.setChecked(!checked);
            });

        QObject::connect(&checkBoxFileParamOnlyEable, &QCheckBox::toggled, [&checkBoxFileParamOnlyDisable](bool checked) {
            checkBoxFileParamOnlyDisable.setChecked(!checked);
            });
        QObject::connect(&checkBoxFileParamOnlyDisable, &QCheckBox::toggled, [&checkBoxFileParamOnlyEable](bool checked) {
            checkBoxFileParamOnlyEable.setChecked(!checked);
            });

        QHBoxLayout buttonLayout;
        QPushButton okButton("OK", &dialog);
        QPushButton cancelButton("Cancel", &dialog);
        buttonLayout.addWidget(&okButton);
        buttonLayout.addWidget(&cancelButton);

        mainLayout.addLayout(&buttonLayout);

        QObject::connect(&okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
        QObject::connect(&cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

        if (dialog.exec() == QDialog::Accepted)
        {
            currentWindow->setUnsubscribeAtStartResults(checkBoxStartEnable.isChecked());
            currentWindow->setUnsubscribeAtEndResults(checkBoxEndEnable.isChecked());
            currentWindow->setUnsubscribeFileParamOnly(checkBoxFileParamOnlyEable.isChecked());
            _startUnsubscribeEnable = checkBoxStartEnable.isChecked();
            _endUnsubscribeEnable = checkBoxEndEnable.isChecked();
            _unsubscribeFileParamOnlyEnable = checkBoxFileParamOnlyEable.isChecked();
        }
    }
}

void GTAMainWindow::onOpenContainingFolder()
{

    GTAEditorWindow *pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if (pCurrSubWindow != nullptr && _pAppController != nullptr)
    {
        QString dataDir = _pAppController->getGTADataDirectory();
        QString relativePath = pCurrSubWindow->getRelativePath();
        QString elemName = pCurrSubWindow->getElementName();
        QString absFilePath = QDir::cleanPath(QString("%1%2").arg(dataDir,relativePath));
        if(!absFilePath.isEmpty())
        {
            QFile file(absFilePath);


            if(file.exists())
            {

                QString folderPath = absFilePath.replace(elemName,"");
                //   if(folderPath.startsWith("\\\\"))
                //  {
                //      folderPath = QDir::cleanPath(folderPath);
                //      folderPath.prepend("\\");
                //  }
                //   else
                folderPath = QDir::cleanPath(folderPath);
                folderPath.replace("/","\\");
                QString cmd = QString("explorer %1").arg(folderPath);
                QProcess::execute(cmd);
                //                QDesktopServices desktopService;
                //                bool rc = desktopService.openUrl(QUrl(QDir::cleanPath(folderPath),QUrl::TolerantMode));
            }
            else
            {
                QMessageBox::critical(this,"Error","File not present");
            }
        }
    }

}

/**
* @brief Display the context menu when right-clicking on a procedure tab
* @param iPoint Coordinate of the mouse when right-clicking (forwarded via the incoming signal)
*/
void GTAMainWindow::displayTabContextMenu(QPoint iPoint)
{
    QTabWidget* current = ui->EditorFrame->getCurrentTabWidget();
    ui->EditorFrame->setFocus();
    _pTabContextMenu->popup(current->tabBar()->mapToGlobal(iPoint));
}

void GTAMainWindow::closeOverwrittenFileTabs(const QString& absPath, const QString& dataDir)
{
    QFile file(absPath);
    if (file.exists())
    {
        QList<std::pair<int, int>> tabsToClose;
        TabIndices currentTabIndices = ui->EditorFrame->getCurrentTabIndices();
        if (currentTabIndices.viewIndex.has_value())
        {
            auto views = ui->EditorFrame->getViewsCount();
            for (int viewIdx = 0; viewIdx < views; viewIdx++)
            {
                auto allTabs = ui->EditorFrame->getAllWindows(viewIdx);
                allTabs.size();
                for (int tabIdx = 0; tabIdx < allTabs.size(); tabIdx++)
                {
                    auto tabRelPath = allTabs.at(tabIdx)->getRelativePath();
                    auto tabAbsPath = QDir::cleanPath(QString("%1%2").arg(dataDir, tabRelPath));
                    if (absPath == tabAbsPath)
                        tabsToClose.append({ viewIdx , tabIdx });
                }
            }
        }

        for (auto& tab : tabsToClose)
            ui->EditorFrame->closeTab(tab.first, tab.second);
    }
}

void GTAMainWindow::onNewFileDetailsAdded()
{
    if(_pAppController)
    {
        if(sender() == _pNewFileDialog)
        {
            bool isValid = _pNewFileDialog->isFileValid();
            if(isValid)
            {
                QString fileName = _pNewFileDialog->getFileName();
                QString filePath = _pNewFileDialog->getFilePath();
                QString fileType = _pNewFileDialog->getFileType();
                QString authName = _pNewFileDialog->getAuthorName();
                QString maxTimeEstimated = _pNewFileDialog->getMaxTimeEstimated();
                QString dataDir = _pAppController->getGTADataDirectory();
                QString absPath = QDir::cleanPath(QString("%1/%2%3").arg(dataDir, fileName, fileType));
                QString elementName = QString("%1%2").arg(fileName,fileType);

                elementName.trimmed();
                filePath.replace(dataDir,"");
                filePath.trimmed();
                QString relFilePath = QString("%1/%2").arg(filePath,elementName);

                closeOverwrittenFileTabs(absPath, dataDir);
                auto pSubWindow = createNewSubWindow(elementName);

                if(pSubWindow)
                {
                    pSubWindow->setInputCsvFilePath(_last_csv_selection_file);
                    pSubWindow->setFileDetails(elementName,relFilePath,fileType, authName, maxTimeEstimated);
                    if(_csvConverter.isViableCsvObject())
                        transferDataToFile(pSubWindow);
                    pSubWindow->saveFile();
                    if (!_isConsole)
                    {
                        pSubWindow->show();                        
                    }                    
                }
                auto gitControler = GtaGitController::getGtaGitController();
                if (gitControler->isRepoOpened()) {
                    onGitStatusClicked(false);
                }
                //updateTree(false, false, {QString("%1%2").arg(dataDir, relFilePath)});
                //TODO: updating tree view item which was just created
            }
        }
    }
}

void GTAMainWindow::transferDataToFile(GTAEditorWindow* pSubWindow) {
    bool status = _csvConverter.convert(pSubWindow, _pNewFileDialog->getFileName());

    ErrorMessageList& logs = _csvConverter.getLogs();
    onUpdateErrorLog(logs);
    // saving the .pro file
    pSubWindow->onSave();
    pSubWindow->setRowIndex(0);
    pSubWindow->setInputCsvFilePath(_last_csv_selection_file);
    _ReportWidget->onSaveLogCsvConversion(_pNewFileDialog->getFileName(), logs, _isConsole);
    _csvConverter.reset();
}

bool GTAMainWindow::canCurrentValueBeEmpty(const QString& command) {
    const QStringList allowedEmptyValCommands = { "Function" };
    for (const auto& allowedCmd : allowedEmptyValCommands) {
        if (command.contains(allowedCmd)) {
            return true;
        }
    }

    return false;
}

/**
* @brief creates new file and fills it with content imported from csv
*/
void GTAMainWindow::onNewCsvToProConvertedFileDetailsAdded()
{
    onNewFileDetailsAdded();
}

void GTAMainWindow::initEditor()
{
    _pAppController->readFavoritesFromDb();
    _pAppController->parseInternalParamInfoXml();
    ui->DataTV->resizeColumnToContents(0);
}

void GTAMainWindow::setAppController(GTAAppController* ipAppController)
{
    _pAppController = ipAppController;
    connect(_pAppController,SIGNAL(pluginFilesImported()),this,SLOT(onPluginFileSavingFinished()));
	connect(_pAppController, SIGNAL(onImportAlreadyExists(QHash <QString, QString>)), this, SLOT(onImportAlreadyExists(QHash <QString, QString>)));
	connect(_pAppController, SIGNAL(onDuplicateUUID(QString, QString)), this, SLOT(onDuplicateUUIDFound(QString, QString)));
	connect(_pAppController, SIGNAL(onUpdateTreeItem()), this, SLOT(onUpdateTreeItem()));

}

void GTAMainWindow::createToolbars()
{
    //---------------------------------------
    //Program configuration Tool Bar
    //---------------------------------------
    _pProgramCB = new QComboBox();
    _pProgramCB->setWindowTitle("Program");

    _pEquipmentCB = new QComboBox();
    _pEquipmentCB->setWindowTitle("Equipment");

    _pStandardCB = new QComboBox();
    _pStandardCB->setWindowTitle("Standard");

    QString loadShortHelp = "Update DB\nThis command updates the database (fast)";
    QString reloadDBHelp  = "Rebuild entirely DB\nThis command will re-create the database (long)";


    _pProgramCB->hide();
    _pEquipmentCB->hide();
    _pStandardCB->hide();
    

    ui->ProgramSelToolBar->addWidget(_pProgramCB);
    ui->ProgramSelToolBar->addWidget(_pEquipmentCB);
    ui->ProgramSelToolBar->addWidget(_pStandardCB);
    _pLoadProgAction = ui->ProgramSelToolBar->addAction(QIcon(":/images/forms/img/Load.png"),loadShortHelp,this,SLOT(loadProgram()));
    _pReloadProgAction = ui->ProgramSelToolBar->addAction(QIcon(":/images/forms/img/forceDBUpdate.png"),reloadDBHelp,this,SLOT(reloadProgram()));

    _pLaunchScxmlAction = ui->GeneralToolBar->addAction(QIcon(":/images/forms/img/Launch.png"),"Launch scxml",
                                                        this,SLOT(onLaunchPBClicked()));
    _pLaunchScxmlAction->setToolTip("This command launches the scxml of open element in editor");


    _pStopSCXML = ui->GeneralToolBar->addAction(QIcon(":/images/forms/img/StopSCXML.png"),"Stop scxml\nThis command stops the scxml being executed",
                                                this,SLOT(onStopSCXMLClicked()));

    _pStopSCXML->setVisible(false);
    _pCheckCompatibilityAction = ui->GeneralToolBar->addAction(QIcon(":/images/Validate"),"Check Compatibility\nThis command validates the document",this,SLOT(onCheckCompatibilityPBClicked()));
    _pCheckCompatibilityAction->setEnabled(true);

    _pGTAControllerLauncher = ui->GeneralToolBar->addAction(QIcon(":/images/forms/img/AddToDashboardLauncher.png"), "Show Procedure Dashboard Launcher", this, SLOT(onClickGTAControllerLauncher()));
}

/**
  * This function creates result tool bar (expand/collapse all,Select/deselect all, new tab) and add actions to them
 */
void GTAMainWindow::createResultToolbars()
{ 
    _pExpandAllResAction = ui->ResultToolBar->addAction(QIcon(":/images/forms/img/Expandall.png"), "Expand All\nAlt++", this, SLOT(onResultToolbarClicked()));
    _pExpandAllResAction->setShortcut(QKeySequence(Qt::ALT + Qt::Key_Plus));
    _pExpandAllResAction->setParent(ui->ResultToolBar);

    _pCollapseAllResAction = ui->ResultToolBar->addAction(QIcon(":/images/forms/img/Collapseall.gif"), "Collapse All\nAlt+-", this, SLOT(onResultToolbarClicked()));
    _pCollapseAllResAction->setShortcut(QKeySequence(Qt::ALT + Qt::Key_Minus));
    _pCollapseAllResAction->setParent(ui->ResultToolBar);

    _pSelectAllResAction = ui->ResultToolBar->addAction(QIcon(":/images/forms/img/SelectAll.png"), "Select All\nCtrl+A", this, SLOT(onResultToolbarClicked()));
    _pSelectAllResAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_A));
    _pSelectAllResAction->setParent(ui->ResultToolBar);

    _pDeSelectAllResAction = ui->ResultToolBar->addAction(QIcon(":/images/forms/img/DeselectAll.png"), "De-select All\nCtrl+Shift+A", this, SLOT(onResultToolbarClicked()));
    _pDeSelectAllResAction->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_A));
    _pDeSelectAllResAction->setParent(ui->ResultToolBar);

    //New tab/page icon for result analysis view
    _pNewResTabAction = ui->ResultToolBar->addAction(QIcon(":/images/forms/img/New_Tab.png"), "New Result Page\nCtrl+T ", this, SLOT(onResultToolbarClicked()));
    _pNewResTabAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_T));
    _pNewResTabAction->setParent(ui->ResultToolBar);

}

/**
  * This slot called for result toolbar action (expand/collapse all,Select/deselect all, new tab) and export LTRA/LTR action
 */
void GTAMainWindow::onResultToolbarClicked()
{
    if ((sender() == _pExpandAllResAction) || (sender() == _pCollapseAllResAction)
            || (sender() == _pSelectAllResAction) || (sender() == _pDeSelectAllResAction)
            || (sender() == ui->actionLTR) ||(sender() == _pExportLTRA))
    {
        GTALogFrame * activeLogFrame = dynamic_cast<GTALogFrame*>(getActiveSubWindow());
        if (nullptr != activeLogFrame)
        {
            if (sender() == _pExpandAllResAction)
            {
                activeLogFrame->callToExpandAll();
            }
            else if (sender() == _pCollapseAllResAction)
            {
                activeLogFrame->callToCollapseAll();
            }
            else if (sender() == _pSelectAllResAction)
            {
                activeLogFrame->selectAllCommands();
            }
            else if (sender() == _pDeSelectAllResAction)
            {
                activeLogFrame->deselectAllCommands();
            }
            else if ((sender() == ui->actionLTR) ||(sender() == _pExportLTRA))
            {
                activeLogFrame->exportReport();
            }
        }
    }
    else if(sender() == _pNewResTabAction)
    {
        GTALogFrame * newLogFrame = createNewResultPage();
        newLogFrame->setFocus();
        newLogFrame->show();
    }
}

void parse_ini_path(QString& path, QString& folder, QString& name)
{
    char delimiter1 = '/';
    char delimiter2 = '\\';
    std::string path_ = path.toStdString();
    size_t pos = path.size() - 1;
    std::string name_ = "";
    std::string folder_ = "";
    bool read_folder = false;

    while (pos > 0)
    {
        if (path_[pos] != delimiter1 && path_[pos] != delimiter2 && !read_folder)
            name_.insert(0, 1, path_[pos]);
        else
        {
        read_folder = true;
        folder_.insert(0, 1, path_[pos]);
        }
        pos--;
    }
    folder_.insert(0, 1, path_[0]);
    folder = QString::fromStdString(folder_);
    name = QString::fromStdString(name_);
}

void GTAMainWindow::open_bench_file_selection()
{
    QString fullpath = QFileDialog::getOpenFileName(this, "Open a file", _current_bench_config_file_folder, "Initialization file (*.ini)");
    if (fullpath.isEmpty())
        return;

    QString folder, name;
    parse_ini_path(fullpath, folder, name);

    if (folder != ui->mutable_label_current_ini_folder->text())
    {
        QStringList ini_file_list = QDir(folder).entryList(QStringList() << "*.ini");
        ui->CB_ini_files->clear();
        foreach(QString filename, ini_file_list)
            ui->CB_ini_files->addItem(filename);
        ui->mutable_label_current_ini_folder->setText(folder);
    }
    int index = ui->CB_ini_files->findText(name);
    ui->CB_ini_files->setCurrentIndex(index);
    updateIniFile();
}

void GTAMainWindow::loadProgram()
{
    QMessageBox msgBox(this);
    msgBox.setText("Update the Database. \nDo you want to continue?");
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setWindowTitle("Editor Warning");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    int ret = msgBox.exec();
    QString sProgram, sEqip, sStd;
    if (ret == QMessageBox::Yes)
    {
        QStringList FileToUpdateList;
        QStringList FileToRemoveList;
        QStringList icdFiles;

        structLoadFileInfo sobjDBFiles;

        if (_pAppController)
        {
            showProgressBar("Updating Database ... Please Wait", this->geometry());
            QFutureWatcher<structLoadFileInfo>* FutureWatcher = new QFutureWatcher<structLoadFileInfo>();
            connect(FutureWatcher, SIGNAL(finished()), this, SLOT(onUpdateDBFinished()));
            QFuture<structLoadFileInfo> Future = QtConcurrent::run(_pAppController, &GTAAppController::loadICDPMRPIRFilesInDB, false, 0);
            FutureWatcher->setFuture(Future);
        }
    }

}

void GTAMainWindow::reloadProgram()
{
    QMessageBox msgBox(this);
    msgBox.setText("The old database will be deleted. \nDo you want to rebuild?");
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setWindowTitle("Editor Warning");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);

    if (msgBox.exec() == QMessageBox::Yes)
    {
        structLoadFileInfo sobjDBFiles;
        if (_pAppController)
        {
            showProgressBar("Rebuilding Database ... Please Wait", this->geometry());
            _pAppController->updateTableVersionInParamDB();
            QFutureWatcher<structLoadFileInfo>* FutureWatcher = new QFutureWatcher<structLoadFileInfo>();
            connect(FutureWatcher, SIGNAL(finished()), this, SLOT(onUpdateDBFinished()));
            QFuture<structLoadFileInfo> Future = QtConcurrent::run(_pAppController, &GTAAppController::loadICDPMRPIRFilesInDB, true, 0);
            FutureWatcher->setFuture(Future);
        }
    }
}

ErrorMessageList GTAMainWindow::createDBErrorLogs(QStringList& iList)const
{
    ErrorMessageList msgList;
    foreach(QString message, iList)
    {
        ErrorMessage logMessage;
        logMessage.description = message;
        logMessage.errorType = ErrorMessage::ErrorType::kInfo;
        logMessage.source = ErrorMessage::MsgSource::kDatabase;
        msgList.append(logMessage);
    }
    return msgList;
}

//ErrorMessageList GTAMainWindow::createCsvToProErrorLogs(const QList<LogInfo>& iList) const {
//    ErrorMessageList msgList;
//    for(const auto& logInfo : iList){
//        ErrorMessage logMessage;
//        logMessage.description = logInfo.msg.description.simplified();
//        logMessage.errorType = logInfo.errorType;
//        logMessage.source = ErrorMessage::MsgSource:kExportDoc; //Could possibly create separate category?
//        logMessage.lineNumber = QString(" [row: %1; col: %2; TC:%3]").arg(logInfo.row).arg(logInfo.col).arg(logInfo.testCase);
//        logMessage.fileName = _pNewFileDialog->getFileName();
//        msgList.append(logMessage);
//    }
//    return msgList;
//}

/* This SLOT is call when an action on the Database is required
 * @return: message might be displayed msg is not empty
 */
void GTAMainWindow::onUpdateDBFinished()
{
    QFutureWatcher<structLoadFileInfo> *pWatcher = dynamic_cast<QFutureWatcher<structLoadFileInfo> *>(sender());
    if(pWatcher == nullptr)
        return;
    else
    {
        structLoadFileInfo resultObj =  pWatcher->result();
        pWatcher->deleteLater();

        if(!resultObj._execStatus )
        {
            QString msg = _pAppController->getLastError();
            if(!msg.isEmpty())
            {
                ErrorMessageList ErrorList;
                ErrorMessage log;
                if(msg == "File list is empty" || "Database is already up-to-date")
					log.errorType = ErrorMessage::ErrorType::kInfo;
				else
					log.errorType = ErrorMessage::ErrorType::kError;
                log.source = ErrorMessage::MsgSource::kDatabase;
                log.description = msg;
                log.lineNumber = QString("---");
                log.fileName  = QString("None");
                ErrorList.append(log);
                onUpdateErrorLog(ErrorList);
            }
        }
        else  
        {
            hideProgressBar();
            if(resultObj._isNew== false )
            {
                QStringList msg;
                if(!resultObj._FileToRemoveList.isEmpty())
                {
                    msg.append("Following files are removed from the Database");
                    msg.append(resultObj._FileToRemoveList);
                }
                if(!resultObj._FileToUpdateList.isEmpty())
                {
                    msg.append("Following files are inserted/updated in the Database");
                    msg.append(resultObj._FileToUpdateList);
                }
                if(!msg.isEmpty())
                {
                    ErrorMessageList msgList = createDBErrorLogs(msg);
                    onUpdateErrorLog(msgList);
                }
            }
            else
            {
                QString msg ="";
                if(!resultObj._icdFiles.isEmpty())
                {
                    msg.append("Following files are added in database\n");
                    msg.append(QString("%1").arg(resultObj._icdFiles.join("\n")));
                }

                if(!msg.isEmpty())
                {
                    onUpdateErrorLogMsg(msg);
                }
            }
        }
        ui->DataTypeCB->setCurrentIndex(0);
    }
    hideProgressBar();
    if(!linkToDatabase())
    {
        QMessageBox::critical(this,"Error","Unable to open database");
    }
    
    _pSettingWidget->setEquipmentToolName(_pAppController->getSystemService()->getEquipmentToolName());
}

/**
* @brief Initialization of the UI settings widget. Also sets some GTA parameters upon initialization.
*/
void GTAMainWindow::initializeSettingsWidget()
{
    _pSettingWidget = new GTASettingsWidget(this);
    _pSettingWidget->hide();
    connect(_pSettingWidget, SIGNAL(cancel_settings()), this, SLOT(on_cancel_settings()));
    connect(_pSettingWidget, SIGNAL(settingsFinished(bool, bool, bool, bool)), this, SLOT(loadAndupdateConfiguration(bool, bool, bool, bool)));
    connect(_pSettingWidget, SIGNAL(settingsOKPB(QString, QString, bool)), this, SLOT(modifyFilePath(QString, QString)));
    connect(_pSettingWidget, SIGNAL(logLevelUpdated(QString)), this, SLOT(onLogMessageLevelUpdated(QString)));
    connect(_pSettingWidget, SIGNAL(newToolIdsAdded(const QList<GTASCXMLToolId>&)), this, SLOT(onNewToolIdsAdded(const QList<GTASCXMLToolId>&)));
    connect(_pSettingWidget,
            SIGNAL(removeToolID(const QList<GTASCXMLToolId>&, const GTASCXMLToolId&)),
            this,
            SLOT(onRemoveToolID(const QList<GTASCXMLToolId>&, const GTASCXMLToolId&))
    );
    connect(_pSettingWidget, SIGNAL(gitDbChanged()), this, SLOT(onGitDbChanged()));

    ui->DBpathLB->clear();
    if (!_pSettingWidget->getCurrentLibFilePath().isEmpty())
    {
        ui->DBpathLB->setVisible(true);
        QString libPath = _pSettingWidget->getCurrentLibFilePath();
        libPath.replace(QString("\\"), QString("/"));
        libPath.remove(0, libPath.lastIndexOf(QChar('/')) + 1);
        ui->DBpathLB->setText("<b>Bench DB :</b>\t" + libPath);
    }
    else
    {
        ui->DBpathLB->setVisible(false);
    }

    ui->WKpathLB->clear();
    if (!_pSettingWidget->getCurrentRepoFilePath().isEmpty())
    {
        ui->WKpathLB->setVisible(true);
        QString repoPath = _pSettingWidget->getCurrentRepoFilePath();
        repoPath.replace(QString("\\"), QString("/"));
        repoPath.remove(0, repoPath.lastIndexOf(QChar('/')) + 1);
        ui->WKpathLB->setText("<b>Workspace :</b>\t " + repoPath);
    }
    else
    {
        ui->WKpathLB->setVisible(false);
    }
}

/**
 * @brief Updates the recording video tool file with a new tool name.
 *
 * This function opens an XML file (specified by the filepath) and updates the 'name',
 * 'toolDisplayName', and 'toolId' attributes of the GENERIC_TOOL element and its child function elements
 * with the provided tool name. The updates are then saved back to the file.
 *
 * @param filepath The path to the XML file that contains the tool configuration.
 * @param toolName The new tool name to be set in the file. This is a standard string.
 *
 * @note The function performs the following steps:
 *       - Opens the specified file in read-only mode.
 *       - Parses the file into a QDomDocument (a DOM tree representation).
 *       - Navigates to the GENERIC_TOOL element and updates its 'name' and 'toolDisplayName' attributes.
 *       - Iterates over each child 'function' element and updates its 'toolId' attribute.
 *       - Saves the modified DOM tree back to the file in a formatted manner.
 *       - If the file cannot be opened for reading or writing, or if parsing fails,
 *         the function outputs a debug message and exits without modifying the file.
 */
void GTAMainWindow::updateRecordingVideoToolFile(const QString& filepath, const std::string& toolName)
{
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open the file for reading.";
        return;
    }

    QDomDocument document;
    if (!document.setContent(&file)) {
        qDebug() << "Failed to parse the file into a DOM tree.";
        file.close();
        return;
    }
    file.close();

    // Navigate to the GENERIC_TOOL element and modify its attributes
    QDomElement root = document.documentElement();
    root.setAttribute("name", QString::fromStdString(toolName));
    root.setAttribute("toolDisplayName", QString::fromStdString(toolName));

    // Navigate to each function element and modify its attributes
    QDomNodeList functions = root.elementsByTagName("function");
    for (int i = 0; i < functions.count(); ++i) {
        QDomElement function = functions.at(i).toElement();
        function.setAttribute("toolId", QString::fromStdString(toolName));
    }

    // Save the modified document
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qDebug() << "Failed to open the file for writing.";
        return;
    }

    QTextStream stream(&file);
    stream.setGenerateByteOrderMark(true); 
    stream.setCodec("UTF-8"); 

    QString xmlString;
    QTextStream stringStream(&xmlString);
    document.save(stringStream, 4);

    // Replace line endings with CR
    xmlString.replace("\n", "\r");

    stream << xmlString; // Write the modified string to the file
    file.close();
}

/**
 * @brief Performs the initialization of the application (geometry, configuration, controllers,...)
 * @return void
 */
void GTAMainWindow::initialize(bool showSettingsNotValid)
{
    TestConfig* testConf = TestConfig::getInstance();
    bool validity = initializeConfiguration();

    // restore geometry
    readSettings();

    initializeSettingsWidget();

    if (!testConf->getRecordingToolName().empty())
    {
        std::string benchDatabasePath = testConf->getLibraryPath();
        QDir dir(QString::fromStdString(benchDatabasePath));
        dir.cd(LIB_TOOL_CONFIG_DIR);
        QString filePath = dir.filePath(RECORDING_TOOL_FILE);
        updateRecordingVideoToolFile(filePath, testConf->getRecordingToolName());
    }

    if (_pAppController)
    {
        _pAppController->setProgramConfiguration(); 
        
        if (!validity && showSettingsNotValid)
        {
            GTASystemServices *pSysService = _pAppController->getSystemService();
            if(pSysService != nullptr)
            {
                _pSettingWidget->setDefaultLogDirPath(pSysService->getDefaultLogDirectory());
            }

            int default_index = ui->CB_ini_files->findText(_current_bench_config_file_name);
            ui->CB_ini_files->setCurrentIndex(default_index);
            ui->mutable_label_current_ini_folder->setText(_current_bench_config_file_folder);

            _pSettingWidget->show();
            QMessageBox::critical(_pSettingWidget, "Settings error", "Settings are not valid.");
            _pSettingWidget->loadSettings();
            _isTreeInit = false;
            _isDBInit = false;
        }
        else
        {
            int default_index = ui->CB_ini_files->findText(_current_bench_config_file_name);
            ui->CB_ini_files->setCurrentIndex(default_index);
            ui->mutable_label_current_ini_folder->setText(_current_bench_config_file_folder);

            modifyFilePath(QString::fromStdString(testConf->getLibraryPath()),
                QString::fromStdString(testConf->getRepositoryPath()));

            _pSettingWidget->setInitialSettingFlag(false);
            _isTreeInit = true;
            _isDBInit = true;

            QString dataDirPath = _pAppController->getGTADataDirectory();
            _FileWatcher.addPath(dataDirPath);
            connect(&_FileWatcher, SIGNAL(directoryChanged(QString)),this,SLOT(onDirectoryChanged(QString)));

            // init the tree view
            showTreeProgressBar();
            bool rebuildProcDb = testConf->getProcDBStartup();
            updateTree(rebuildProcDb, true);

            // initialize parameter DB
            testConf->setBenchDBStartup(true); // FIX FOR THE VERSION 31.01.00 // SEE TICKET JIRA I04DAIVYTE-918.
            if (testConf->getBenchDBStartup())
            {
                showProgressBar("Initializing Database", QRect(this->geometry().x(), 
                                                               this->geometry().y() + this->geometry().y() + 10, 
                                                               this->geometry().x(), 
                                                               this->geometry().y()));

                QFutureWatcher<bool>* DBWatcher = new QFutureWatcher<bool>();
                connect(DBWatcher,SIGNAL(finished()), this, SLOT(onOnlyInitDB()));
                QFuture<bool> DBFuture = QtConcurrent::run(_pAppController, &GTAAppController::initializeDatabase);
                DBWatcher->setFuture(DBFuture);
            }	
        }

    } 
    bool logEnableResult = testConf->getLogEnable();
	if(logEnableResult)
	{
		onLogWindowOpened(logEnableResult);
        QString iLoglevel = QString::fromStdString(testConf->getLogMessages());
		onLogMessageLevelUpdated(iLoglevel);
	}
}

void GTAMainWindow::onUpdateEquipmentFinished()
{
    QFutureWatcher<void> *pWatcher = dynamic_cast<QFutureWatcher<void> *>(sender());
    if(pWatcher == nullptr)
        return;

    pWatcher->deleteLater();
    if(_pSettingWidget != nullptr)
    {
        //        _pSettingWidget->enEquipTab(true);
        //        _pSettingWidget->refreshEquipTable();
    }
    toggleUpdateReloadDB(true);

}

void GTAMainWindow::onOnlyInitDB()
{
    QFutureWatcher<bool> *pWatcher = dynamic_cast<QFutureWatcher<bool> *>(sender());
    if(pWatcher == nullptr)
        return;
    else
    {
        bool rc = pWatcher->result();
        if(rc)
        {
            hideProgressBar();
            initEditor();
            initialiseResultPage();
            linkToDatabase();
            checkDatabaseConsistency(); 
        }
    } 
    emit HideProgressBar();
    pWatcher->deleteLater();
    //showProgressBar("Updating Equipment Database");
    toggleUpdateReloadDB(false);
    connect(_equipmentDataWatcher, SIGNAL(finished()), this, SLOT(onUpdateEquipmentFinished()));
    QFuture<void> equipmentDataFuture = QtConcurrent::run(_pAppController, &GTAAppController::processEquipmentData);
    _equipmentDataWatcher->setFuture(equipmentDataFuture);
    restoreLastSession();
}

void GTAMainWindow::restoreLastSession()
{
    if(_pAppController)
    {
        //showProgressBar("Restoring last session", this->geometry());
        QString lastSession;
        QStringList recentSessionLst;
        bool rc = _pAppController->readSessionInfoFile(lastSession,recentSessionLst);
        if(rc)
        {

            if(!recentSessionLst.isEmpty())
            {
                if(_pActionGrp != nullptr)
                {
                    delete _pActionGrp;
                    _pActionGrp = nullptr;
                }
                bool found = false;
                _pActionGrp = new QActionGroup(this);
                for(int i  = recentSessionLst.count() - 1 ; i >= 0; i--)
                    //                foreach(QString recentSession, recentSessionLst)
                {

                    QString recentSession = recentSessionLst.at(i);
                    QAction *action = new QAction(recentSession,ui->menuRecent_Sessions);
                    action->setCheckable(true);
                    action->setObjectName(recentSession);
                    if(lastSession == recentSession)
                    {
                        action->setChecked(true);
                    }
                    _pActionGrp->addAction(action);
                    connect(action,SIGNAL(triggered()),this,SLOT(onRecentSessionSelected()));
                    _recentSessions.append(action);
                    found |= true;

                }
                if(found)
                    updateRecentMenu();
            }

            if(!lastSession.isEmpty())
            {
                loadSession(lastSession);
            }
        } 
        //hideProgressBar();
    }
    
}

void GTAMainWindow::loadSession(const QString &iSessionName)
{
    QStringList files = _pAppController->getSessionFiles(iSessionName);
    for (const QString& file : files)
        openDocument(file);
}

void GTAMainWindow::onRecentSessionSelected()
{
    QAction *pAction = dynamic_cast<QAction*>(sender());
    if(pAction)
    {
        QString sessionName = pAction->text();
        onSwitchSession(sessionName);
    }
}

void GTAMainWindow::toggleUpdateReloadDB(bool iStatus)
{
    _pLoadProgAction->setEnabled(iStatus);
    _pReloadProgAction->setEnabled(iStatus);
}

void GTAMainWindow::onInitDBFinished()
{
    bool retVal;
    QFutureWatcher<bool> *pWatcher = dynamic_cast<QFutureWatcher<bool> *>(sender());
    if(pWatcher == nullptr)
        return;
    else
    {
        bool rc = pWatcher->result();
        if(rc)
        {
            retVal = checkDatabaseConsistency();
            if(retVal)
                loadAndupdateConfiguration(true, true, true, true);
        }
    }
    pWatcher->deleteLater();

    if(!retVal)
        _pAppController->generateCmdPropertyFileIfAbsent();
    toggleUpdateReloadDB(false);
    //showProgressBar("Updating Equipment Database");
    connect(_equipmentDataWatcher, SIGNAL(finished()), this, SLOT(onUpdateEquipmentFinished()));
    QFuture<void> equipmentDataFuture = QtConcurrent::run(_pAppController, &GTAAppController::processEquipmentData);
    _equipmentDataWatcher->setFuture(equipmentDataFuture);
    restoreLastSession();
}

void GTAMainWindow:: setBatchConfigDBFiles(const QString& iIniFilePath, bool iRemoveOldEntries)
{
    //The ini file is in windows format. That means path will containt "\" which is treated as special
    //char by QSettings so input ini file converts to correct format and then QSettings are used.

    if(_pAppController)
    {

        QFile symbolFileObj(iIniFilePath);
        bool frc = symbolFileObj.open(QFile::ReadOnly | QIODevice::Text);
        if( frc)
        {
            QTextStream reader(&symbolFileObj);
            QString contents = reader.readAll();
            contents.replace(QString("\\"),QString("/"));


            GTASystemServices* pServices = _pAppController->getSystemService();
            QString sTempPath = pServices->getTemporaryDirectory();
            QString sFileSavePath = QString("%1%2%3").arg(sTempPath,QDir::separator(),"TemproryBatchConfig.ini");
            QFile tempIniFile(QDir::cleanPath(sFileSavePath));
            tempIniFile.open(QFile::WriteOnly | QIODevice::Text);
            QTextStream writer(&tempIniFile);
            writer<<contents;
            tempIniFile.close();
            QStringList filesNotFound = pServices->setBenchDBConfigFiles(sFileSavePath,iRemoveOldEntries);
        }
    }

}

void GTAMainWindow::onLogMessageLevelUpdated(QString iLoglevel)
{
    qInstallMessageHandler(0);
    if(iLoglevel.compare(LOG_INFO,Qt::CaseInsensitive) == 0)
    {
        qInstallMessageHandler(GTAAppLogMsgRedirector::info_Qt5);
    }
    if(iLoglevel.compare(LOG_DEBUG,Qt::CaseInsensitive) == 0)
    {
        qInstallMessageHandler(GTAAppLogMsgRedirector::debug_Qt5);
    }
    if(iLoglevel.compare(LOG_ALL,Qt::CaseInsensitive) == 0)
    {
        qInstallMessageHandler(GTAAppLogMsgRedirector::all_Qt5);
    }

}

/**
* @brief Launches a update of the bench configuration when windows folders are being modified directly.
* @param dir The directory where the changes occurred. (not useful in this method, but have to be specified since this is a slot).
* @return void
*/
void GTAMainWindow::onDirectoryChanged(QString dir)
{
    onDisconnectFileWatcher();
    QMessageBox::StandardButton response = QMessageBox::question(
        this,
        "Updating database",
        "Folders have changed.\nInitiating an update of the database.\nThis may take some time.\n\nDo you want to continue?",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
    );

    if (response == QMessageBox::Yes) {
        onCreateDataDB(false);
    }
    connectFoldersToWatcher();
    auto gitCtrl = GtaGitController::getGtaGitController();
    if (gitCtrl->isRepoOpened()) {
        onGitStatusClicked(false);
    }
}

/**
* @brief Updates the bench configuration. Sets the ui features, sets the program configuration and updates the database.
* @param isLibPathChanged True if the library path (bench db) has changed.
* @param isRepoPathChanged True if the repository path (workspace) has changed.
* @return bool true if the update is correctly performed.
*/
bool GTAMainWindow::loadAndupdateConfiguration(bool isLibPathChanged, bool isRepoPathChanged, bool isToolDataPathChanged, bool rebuildWorkDb)
{
    bool bOk = false;
    bool configurationDone = false;


    if (_pAppController)
    {

        ui->DBpathLB->clear();
        QString libPath = _pSettingWidget->getCurrentLibFilePath();
        libPath.replace(QString("\\"), QString("/"));
        libPath.remove(0, libPath.lastIndexOf(QChar('/')) + 1);
        ui->DBpathLB->setText("<b>Bench DB :</b>\t" + libPath);

        ui->WKpathLB->clear();
        QString repoPath = _pSettingWidget->getCurrentRepoFilePath();
        repoPath.replace(QString("\\"), QString("/"));
        repoPath.remove(0, repoPath.lastIndexOf(QChar('/')) + 1);
        ui->WKpathLB->setText("<b>Workspace :</b>\t " + repoPath);

        int TestConfigDBUpdateStatus = 0;
        if (sender() == _pSettingWidget)
        {
            QString time;
            bool bSaveSCXMLSetting = _pSettingWidget->getSaveElementForSCXML();
            bool bChannelSelectionSetting = _pSettingWidget->getChannelSelectionSetting();
            QString sEngineSelected = _pSettingWidget->getEngineTypeSelection();
            bool bPirInvokeInSCXML = _pSettingWidget->getPirInvokeInSCXML();
            bool bGenericSCXMLState = _pSettingWidget->getGenericSCXMLState();
            bool isPirWaitTimeSet = _pSettingWidget->getPIRWaitTimeSetting(time);
            bool isNewSCXMLStruct = _pSettingWidget->getNewSCXMLState();            
            QString sDecimalPlaceSelected = _pSettingWidget->getDecimalPlaceSelected();
            QString sLogMessages = _pSettingWidget->getLogMessageSelected();
            bool logEnabled = _pSettingWidget->getLogEnabled();
            bool virtualPMR = _pSettingWidget->getVirtualPMR();
            bool resizeRow = _pSettingWidget->getResizeRow();
            TestConfigDBUpdateStatus = int(_pSettingWidget->updateDBStatusForTestConfig());
            bool standaloneModeExec = _pSettingWidget->getStandaloneModeExec();
            QString iniFilePath = _pSettingWidget->getTestConfigFilePath();
            bool isConfigFileUpdateDb = _pSettingWidget->isConfigFileUpdateDB();
            bool isRemoveOldEntries = _pSettingWidget->isTestConfigDataUpdate();
            bool isBenchDbStartUp = _pSettingWidget->isBenchDbStartup();
            bool isProcDbStartup = _pSettingWidget->isProcDbStartup();
            bool ltraPrintTable = _pSettingWidget->getLTRAPrintTableStatus();
            bool enInvokeForLocalVar = _pSettingWidget->getEnInvokeForLocalVar();
            bool newLogFormat = _pSettingWidget->getNewLogFormatStatus();
            bool initSubStart = _pSettingWidget->getInitSubscribeStartStatus();
            bool initSubEnd = _pSettingWidget->getInitSubscribeEndStatus();
            bool manualActTimeoutStat = _pSettingWidget->getManualActionTimeoutStatus();
            QString manualActTimeoutVal = _pSettingWidget->getManualActionTimeoutValue();            
            bool bUSCXMLBasedSCXMLEn = _pSettingWidget->getUSCXMLBasedSCXMLCBStatus();
            QString debugIP = _pSettingWidget->getDebugModeIP();
            int debugPort = _pSettingWidget->getDebugModePort();
            bool validationWidgetEnabled = _pSettingWidget->getValidationEnabler();
            QList<GTALiveToolParams> liveTools = _pSettingWidget->getLiveViewToolList();
            QString gtaLiveBisgIP = _pSettingWidget->getGtaLiveBisgIP();
            int gtaLiveBisgPort = _pSettingWidget->getGtaLiveBisgPort();

            bOk = _pAppController->setProgramConfiguration(
                sDecimalPlaceSelected,
                sLogMessages,
                logEnabled,
                bSaveSCXMLSetting,
                bChannelSelectionSetting,
                sEngineSelected,
                bPirInvokeInSCXML,
                bGenericSCXMLState,
                isPirWaitTimeSet, time,
                isNewSCXMLStruct,
                virtualPMR,
                resizeRow,
                standaloneModeExec,
                isBenchDbStartUp,
                isProcDbStartup,
                ltraPrintTable,
                enInvokeForLocalVar,
                newLogFormat,
                initSubStart,
                initSubEnd,
                manualActTimeoutVal,
                manualActTimeoutStat,
                bUSCXMLBasedSCXMLEn,
                debugIP,
                debugPort,
                validationWidgetEnabled,
                liveTools,
                gtaLiveBisgIP,
                gtaLiveBisgPort);

            configurationDone = true;
            _pSettingWidget->setInitialSettingFlag(false);
            _pSettingWidget->hide();

            if (isRepoPathChanged || (_isTreeInit == false))
            {
                //showTreeProgressBar();
                //updateTree();
                _isTreeInit = true;
            }
            if (isConfigFileUpdateDb)
            {
                setBatchConfigDBFiles(iniFilePath, isRemoveOldEntries);
            }
        }

        TestConfig* testConf = TestConfig::getInstance();
        if (!testConf->getRecordingToolName().empty())
        {
            std::string benchDatabasePath = testConf->getLibraryPath();
            QDir dir(QString::fromStdString(benchDatabasePath));
            dir.cd(LIB_TOOL_CONFIG_DIR);
            QString filePath = dir.filePath(RECORDING_TOOL_FILE);
            updateRecordingVideoToolFile(filePath, testConf->getRecordingToolName());
        }

        if (configurationDone == false)
        {
            bOk = _pAppController->setProgramConfiguration();
            checkDatabaseConsistency();  
        }
        _pAppController->generateCmdPropertyFileIfAbsent();

        if (_isDBInit == false)
        {
            QFutureWatcher<bool>* DBWatcher = new QFutureWatcher<bool>();
            connect(DBWatcher, SIGNAL(finished()), this, SLOT(onInitDBFinished()));
            QFuture<bool> DBFuture = QtConcurrent::run(_pAppController, &GTAAppController::initializeDatabase);
            DBWatcher->setFuture(DBFuture);
            _isDBInit = true;
        }
        else if (isRepoPathChanged) {
            onCloseAllSubwindows();
        }
        else if (isLibPathChanged || (TestConfigDBUpdateStatus > 0))
        {
            QString newLibraryPath = QString(_pAppController->getGTALibraryPath() + "/PARAMETER_DB");
            // To avoid rebuilding the database, we check if the file already exists (PARAMETER_DB)
            if (!QFileInfo::exists(newLibraryPath))
            {
                showProgressBar("Loading ICD/PMR/PIR files into DB ...", this->geometry());
                QFutureWatcher<structLoadFileInfo>* FutureWatcher = new QFutureWatcher<structLoadFileInfo>();
                connect(FutureWatcher, SIGNAL(finished()), this, SLOT(onLoadDBFinished()));
                QFuture<structLoadFileInfo> Future = QtConcurrent::run(_pAppController, &GTAAppController::loadICDPMRPIRFilesInDB, true, TestConfigDBUpdateStatus);
                FutureWatcher->setFuture(Future);
            }
        }
        bOk = true;
    }
    TestConfig::getInstance()->saveConf();

    // update the ui and db
    _current_bench_config_file_folder = ui->mutable_label_current_ini_folder->text();
    _current_bench_config_file_name = ui->CB_ini_files->currentText();

    int default_index = ui->CB_ini_files->findText(_current_bench_config_file_name);
    ui->CB_ini_files->setCurrentIndex(default_index);
    ui->mutable_label_current_ini_folder->setText(_current_bench_config_file_folder);

    if (rebuildWorkDb)
        onCreateDataDB((QWidget*)sender() != _pSettingWidget); // don't print dialog box twice (first from settings widget).
    else
        updateTree(false, true);
    refresh_ui();

    return bOk;
}

bool GTAMainWindow::onLoadDBFinished()
{

    bool bOk = false;
    QFutureWatcher<structLoadFileInfo> *pWatcher = dynamic_cast<QFutureWatcher<structLoadFileInfo> *>(sender());
    if(pWatcher == nullptr)
        return false;
    else
    {

        structLoadFileInfo rc  = pWatcher->result();
        bOk=rc._execStatus;
        if (!bOk)
        {
            //QMessageBox::warning(this,"Database warning",_pAppController->getLastError());
            QString msg = _pAppController->getLastError();
            if(!msg.isEmpty())
            {

                //                ui->LogDW->show();
                //                _ReportWidget->show();
                //                _ReportWidget->setText(msg,ErrorMessage::INFO,ErrorMessage::DATABASE);

                ErrorMessageList ErrorList;
                ErrorMessage log;
                log.errorType = ErrorMessage::ErrorType::kInfo;
                log.source = ErrorMessage::MsgSource::kDatabase;
                log.description = msg;
                log.lineNumber = QString();
                log.fileName  = QString();
                ErrorList.append(log);
                onUpdateErrorLog(ErrorList);
            }
        }

        initEditor();
        initialiseResultPage();
        showProgressBar("Linking to DB", this->geometry());
        linkToDatabase();
    } 
    pWatcher->deleteLater();
    
    hideProgressBar();
    return bOk;


}

void GTAMainWindow::onAddRow()
{
    GTAEditorWindow *pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if (pCurrSubWindow)
    {
        pCurrSubWindow->onAddRow();
    }
}

void GTAMainWindow::onDeleteRow()
{
    GTAEditorWindow *pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if (pCurrSubWindow)
    {
        pCurrSubWindow->onDeleteRow();
    }
}

void GTAMainWindow::onInsertAction()
{
    GTAEditorWindow *pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if (pCurrSubWindow)
    {
        pCurrSubWindow->onInsertAction();

    }
}

void GTAMainWindow::onInsertCheck()
{
    GTAEditorWindow *pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if (pCurrSubWindow)
    {
        pCurrSubWindow->onInsertCheck();

    }
}

void GTAMainWindow::dockWidgetVisibility(bool)
{

    //    if(ui->actionLogView->isChecked())
    //    {
    //        ui->LogDW->show();
    //    }
    //    else
    //        ui->LogDW->hide();


    if(ui->actionData_Browser->isChecked())
    {
        ui->DataBrowserDW->show();
    }
    else
        ui->DataBrowserDW->hide();
    
}

void GTAMainWindow::dockWidgetViewActionUpdate(bool)
{
    bool isOnEditorView = ui->DataBrowserDW->isVisible();
    ui->actionData_Browser->setChecked(isOnEditorView);    
    ui->actionView_Header->setEnabled(isOnEditorView);
    ui->actionSummary_conclusion->setEnabled(isOnEditorView);

    auto pDockWdgt = dynamic_cast<QDockWidget*>(sender());
    QString objName = pDockWdgt->objectName();
    if(pDockWdgt != nullptr)
    {
        if(objName == DOCK_WDGT_APPLICATION_LOGS)
        {
            if(pDockWdgt->isHidden())
                ui->actionApplication_Logs->setChecked(false);
            else
                ui->actionApplication_Logs->setChecked(true);
        }
        else if(objName == DOCK_WDGT_LOGS)
        {
            if(pDockWdgt->isHidden())
                ui->actionLogView->setChecked(false);
            else
                ui->actionLogView->setChecked(true);
        }        
    }
}

void GTAMainWindow::onShowElementDetail()
{
    if (_pAppController)
    {

        QModelIndexList selectedIndex = ui->DataTV->selectionModel()->selectedIndexes();
        if (!selectedIndex.isEmpty())
        {
            QModelIndex index = selectedIndex.at(0);
            if (index.isValid())
            {

                QString fileName = _pAppController->getFilePathFromTreeModel(index);
                if(fileName.isEmpty())
                    return;

                QAbstractItemModel* pModelTree=nullptr;
                
                if(_pAppController->getModelForTreeView(pModelTree,fileName))
                {
                    if (nullptr!=pModelTree)
                    {
                        QPointer<GTAElementTreeViewWidget> pWidget = new GTAElementTreeViewWidget(this);
                        pModelTree->setParent(pWidget);
                        pWidget->setModel(pModelTree);
                        pWidget->setMinimumWidth(1000);
                        pWidget->setName(fileName);
                        pWidget->show();
                    }
                }
                else
                {
                    QString lastError = _pAppController->getLastError();
                    QMessageBox::critical(this,"TreeView error",lastError);
                }

            }

        }
    }
}

/**
 * @brief Show the dependencies of a selected document in the Document Viewer.
 * Dependencies include Procedures,Functions,Objects,External tools,CSV input files (if used)
 * that are used inside the document
*/
void GTAMainWindow::onShowDependencies()
{
    if (ui->DataTV->selectionModel() != nullptr && _pAppController != nullptr)
    {
        QModelIndexList selectedItems = ui->DataTV->selectionModel()->selectedIndexes();
        if (!selectedItems.isEmpty())
        {
            QString fileName = _pAppController->getFilePathFromTreeModel(selectedItems.at(0));
            if (fileName.isEmpty())
                return;
            QPointer<GTAElementDependenciesWidget> pWidget = new GTAElementDependenciesWidget(fileName, _pAppController, this);
            pWidget->show();        
        }
    }
}

void GTAMainWindow::displayHeaderContextMenu(const QPoint &iPos)
{
    if (_pAppController==nullptr)
        return;

    std::string resultListStr = TestConfig::getInstance()->getDVSavedList();
    QStringList resultList = QString::fromStdString(resultListStr).split(":");
    foreach(QString column,_pAppController->getViewController()->getColumnList())
    {
        if (!column.contains(DV_NAME_COL))
            ui->DataTV->setColumnHidden(_pAppController->getViewController()->getColumnList().indexOf(column), true);
    }

    /*std::string headerState_str = TestConfig::getInstance()->getDataTVGeometry();
    QByteArray headerState(headerState_str.c_str(), headerState_str.length());

    if (!headerState.isEmpty())
        ui->DataTV->header()->restoreState(headerState);*/

    if (_pContextMenuOnDVHeader != nullptr)
    {
        foreach(QAction* action,_pContextMenuOnDVHeader->actions())
        {
            if (resultList.contains(action->text()))
            {
                ui->DataTV->setColumnHidden(_pAppController->getViewController()->getColumnList().indexOf(action->text()), false);
                action->setChecked(true);
//                if (!_DVHeaderColList.contains(action->text()))
                    _DVHeaderColList.append(action->text());
            }
            else
                action->setChecked(false);
        }
        _DVHeaderColList.removeDuplicates();
    }

    QWidget * pSourceWidget = (QWidget*)sender();
    if(pSourceWidget != nullptr && pSourceWidget == ui->DataTV->header() && _pContextMenuOnDVHeader != nullptr)
    {
        _pContextMenuOnDVHeader->exec(ui->DataTV->header()->viewport()->mapToGlobal(iPos));
    }
}

void GTAMainWindow::displayContextMenu(const QPoint &iPos)
{
    if (_pAppController==nullptr)
        return;

    QWidget * pSourceWidget = (QWidget*)sender();
    if(pSourceWidget != nullptr && pSourceWidget == ui->DataTV && _pContextMenuOnDV != nullptr)
    {

        QModelIndexList indexList = ui->DataTV->selectionModel()->selectedRows(0);
        if(indexList.count())
        {
            if (indexList.count() > 1)
                _pAddFolder->setVisible(false);
            else
                _pAddFolder->setVisible(true);

            UpdateProcedureDashBoardMenuItem(indexList);

            QModelIndex index = indexList.at(0);
            if((index.column() == 0 || index.column() == 1) && index.isValid())
            {
                QString fileName = _pAppController->getFilePathFromTreeModel(index);
                if(!fileName.isEmpty())
                {
                    UpdateGitActionMenuItems(index);

					QString extension = _pAppController->getExtensionFromTreeModel(index);
                    _pExportCsvs->setEnabled(true);

					if (extension != "folder")
					{
						_pRefreshTreeAction->setVisible(false);
						_pContextMenuOnDV->exec(ui->DataTV->viewport()->mapToGlobal(iPos));
					}
					else
					{
						_pDeleteDocAction->setVisible(true);
						_pMoveFiles->setVisible(true);
						_pAddToFavOnDV->setVisible(false);
						_pRemoveFromFav->setVisible(false);
						_pCreateCallSequence->setVisible(false);
						_pOpenDocAction->setVisible(false);
						_pDocUsedByAction->setVisible(false);
						_pExportDocAction->setVisible(false);
						_pShowDetailOnDV->setVisible(false);
                        _pShowDependencies->setVisible(false);
						_pUpdateValidationStatus->setVisible(false);
						_pExportSCXMLs->setVisible(false);
                        _pSaveAsAction->setVisible(false);
						_pRefreshTreeAction->setVisible(false);
						_pContextMenuOnDV->exec(ui->DataTV->viewport()->mapToGlobal(iPos));
					}
                }
            }
        }
        else
        {
            _pDeleteDocAction->setVisible(false);
            _pAddToFavOnDV->setVisible(false);
            _pRemoveFromFav->setVisible(false);
            _pCreateCallSequence->setVisible(false);
            _pOpenDocAction->setVisible(false);
            _pDocUsedByAction->setVisible(false);
            _pExportDocAction->setVisible(false);
            _pShowDetailOnDV->setVisible(false);
            _pShowDependencies->setVisible(false);
            _pRename->setVisible(false);
			_pMoveFiles->setVisible(false);
            _pUpdateValidationStatus->setVisible(false);
            _pExportSCXMLs->setVisible(false);
            _pSaveAsAction->setVisible(false);
   
            //only refresh tree is true
            _pRefreshTreeAction->setVisible(true);

            _pContextMenuOnDV->exec(ui->DataTV->viewport()->mapToGlobal(iPos));


        }
    }
}

void GTAMainWindow::updateDatabaseFromLib(bool )
{
    _pAppController->updateParameters();
}

void GTAMainWindow::onClearRow()
{
    GTAEditorWindow *pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if (pCurrSubWindow)
    {
        pCurrSubWindow->onClearRow();
    }
}

void GTAMainWindow::onEditorCutRow()
{
    GTAEditorWindow *pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if (pCurrSubWindow)
    {
        pCurrSubWindow->onEditorCutRow();
    }

}

void GTAMainWindow::onEditorCopyRow()
{
    GTAEditorWindow *pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if (pCurrSubWindow)
    {
        pCurrSubWindow->onEditorCopyRow();
    }
}

void GTAMainWindow::onEditorPasteRow()
{
    GTAEditorWindow *pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if(pCurrSubWindow)
    {
        pCurrSubWindow->onEditorPasteRow();
    }
}

void GTAMainWindow::onViewResultsClicked()
{
    GTAEditorWindow *pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if(pCurrSubWindow)
    {
        QString logPath;
        if(pCurrSubWindow->isValidLogAvailable(logPath))
        {
            QString relativePath = pCurrSubWindow->getRelativePath();

            if(!logPath.isEmpty())
            {
                GTALogFrame *logPage = dynamic_cast<GTALogFrame *>(getWidgetFromTitle(relativePath));
                if(nullptr == logPage)
                {
                    // create a new result page in ResultMdi Area if corresponding result page is not present
                    logPage = createNewResultPage();
                    if (nullptr != logPage)
                    {
                        logPage->setLog(logPath);
                        logPage->setElementRelativePath(relativePath);
                        logPage->disableLogGroupBox();
                        logPage->analyzeResults();
                    }
                }
                ui->actionResultAnalyzerView->trigger();
                logPage->setFocus();
            }
        }
        else
        {
            QMessageBox::warning(this,"Error","Results cannot be displayd as logs for this procedure are unavailable because the procdure was stopped or not excuted",QMessageBox::Ok);
        }
    }
}

int  GTAMainWindow::getSelectedEditorRows(QList<int>& oSelectedRows)
{
    int retVal;

    GTAEditorWindow *pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if (pCurrSubWindow)
    {
        retVal = pCurrSubWindow->getSelectedEditorRows(oSelectedRows);
    }
    return retVal;

}

bool GTAMainWindow::linkToDatabase()
{

    bool rc = false;
    if(_pAppController == nullptr)
        return rc;

    //Set view for element browser
	QItemSelectionModel *model = ui->DataTV->selectionModel();
	ui->DataTV->setModel(nullptr);
	if (nullptr != model)
	{
		delete model;
		model = nullptr;
	}
    ui->DataTV->setModel(_pAppController->getElemDataBrowserModel(true));
    ui->DataTV->resizeColumnToContents(0);

    std::string headerState_str = TestConfig::getInstance()->getDataTVGeometry();
    QByteArray headerStateHex(headerState_str.c_str(), static_cast<int>(headerState_str.length()));
    QByteArray headerState = QByteArray::fromHex(headerStateHex);

    if(!headerState.isEmpty())
        ui->DataTV->header()->restoreState(headerState);

    //set view for header browser
    GTASystemServices* pSystemService = _pAppController->getSystemService();
    QString dataDir = pSystemService->getDataDirectory();

    QStringList headerFilter;
    headerFilter<<"*.hdr";
    if(_pFileSystemHeader == nullptr)
        _pFileSystemHeader = new QFileSystemModel(this);

    _pFileSystemHeader->setRootPath(dataDir);
    _pFileSystemHeader->setNameFilters(headerFilter);
    _pFileSystemHeader->setNameFilterDisables(false);
    _pFileSystemHeader->setFilter(QDir::Files);

    rc = true;
    return rc;

}

void GTAMainWindow::onSearchTypeChanged(QString colName)
{
    QString elemType = ui->DataTypeCB->currentText();
    QString searchStr = ui->DataSearchLE->text();
    updateElementBrowserFilter(searchStr,elemType);
}

void GTAMainWindow::onDataSeachTextChange(const QString & iText)
{
    QString dataType = ui->DataTypeCB->currentText();
    updateElementBrowserFilter(iText,dataType);
}

void GTAMainWindow::onDataTypeValChange(const QString & iText)
{
    QString searchStr = ui->DataSearchLE->text();
    updateElementBrowserFilter(searchStr,iText);
}

void GTAMainWindow::updateElementBrowserFilter(const QString &iSearchStr, const QString & iDataType)
{
    int type=0;
    QString colName;
    colName = ui->SearchTypeCB->currentText();

    if(!colName.isEmpty())
        if(_pAppController->getViewController()->getColumnList().count() > 0)
			type = _pAppController->getViewController()->getColumnList().indexOf(colName);

    QString iSearchStrCopy = iSearchStr;
    QString searchStr;

    if (iSearchStrCopy.contains("*"))
    {
        iSearchStrCopy.replace("*", ".*");
    }

    if (iSearchStrCopy.contains("?"))
    {
        iSearchStrCopy.replace("?", ".");
    }

    if (iDataType != "Favorites")
    {
		QItemSelectionModel *model = ui->DataTV->selectionModel();
		ui->DataTV->setModel(nullptr);
		if (nullptr != model)
		{
			delete model;
			model = nullptr;
		}
        ui->DataTV->setModel(_pAppController->getElemDataBrowserModel(true));

        std::string headerState_str = TestConfig::getInstance()->getDataTVGeometry();
        QByteArray headerStateHex(headerState_str.c_str(), static_cast<int>(headerState_str.length()));
        QByteArray headerState = QByteArray::fromHex(headerStateHex);

        if(!headerState.isEmpty())
            ui->DataTV->header()->restoreState(headerState);

    }
    QString fileExt = _DataTypeExtMap.value(iDataType);
    if (iDataType == "Favorites")
    {
        _pAppController->getViewController()->updateFavoriteDataModel();
		QItemSelectionModel *model = ui->DataTV->selectionModel();
		ui->DataTV->setModel(nullptr);
		if (nullptr != model)
		{
			delete model;
			model = nullptr;
		}
        ui->DataTV->setModel(_pAppController->getFavoriteBrowserDataModel(false));
        ui->DataTV->resizeColumnToContents(0);
        ui->DataTV->setEditTriggers(QListView::NoEditTriggers);
    }
    else if(fileExt.isEmpty())
    {

        _pAppController->setFiltersOnElemBrowser(QStringList()<< iSearchStrCopy,type);
    }
    else
    {
        if(iSearchStrCopy.isEmpty())
        {

            fileExt.replace("*","");
            _pAppController->setFiltersOnElemBrowser(QStringList()<<QString("%1$").arg(fileExt),type);
        }
        else
        {
            fileExt.replace("*.","");
            searchStr = QString("%1.*%2$").arg(iSearchStrCopy, fileExt);

            _pAppController->setFiltersOnElemBrowser(QStringList()<<searchStr,type);
        }
    }
    displayHeaderContextMenu(QPoint());
    return;
}

void GTAMainWindow::toScxmlExportTheFile(const QString& fileName, const QString& directory)
{
    QString ProgressMsg = QString("Exporting %1 file(s) to SCXML").arg(QString::number(_exportDocToSCXMLcount));
    showProgressBar(ProgressMsg, this->geometry());
    QString filePath = QDir::cleanPath(QString("%1%2%3").arg(directory, QDir::separator(), fileName));
    if (!filePath.isEmpty() && _pAppController != nullptr)
    {
        TestConfig* testConf = TestConfig::getInstance();
        //bool hasUnsubscibeAtStart = testConf->getInitSubscribeStart();
        //bool hasUnsubscibeAtEnd = testConf->getInitSubscribeEnd();
        _pAppController->setUnsubscribeFileParamOnly(_unsubscribeFileParamOnlyEnable);


        std::string headerState_str = testConf->getDataTVGeometry();
        QByteArray headerStateHex(headerState_str.c_str(), static_cast<int>(headerState_str.length()));
        QByteArray headerState = QByteArray::fromHex(headerStateHex);

        QFutureWatcher<bool>* FutureWatcher = new QFutureWatcher<bool>();
        connect(FutureWatcher, SIGNAL(finished()), this, SLOT(exportDoctoScxmlSlot()));
        QFuture<bool> Future = QtConcurrent::run(_pAppController, &GTAAppController::exportDocToScxml, fileName, directory, _startUnsubscribeEnable, _endUnsubscribeEnable, false);
        FutureWatcher->setFuture(Future);
    }
}

void GTAMainWindow::onExportSCXMLPbClicked()
{
    QModelIndexList selectedIndexs = ui->DataTV->selectionModel()->selectedRows(0);
    if (!selectedIndexs.isEmpty())
    {
        QString defaultPath = QDir::cleanPath(QDir::currentPath() + "/");
        QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"), defaultPath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

        if (!dir.isEmpty())
        {
            _exportDocToSCXMLcount = selectedIndexs.count();
            _exportDocToSCXMLcount_SLOT = 0;

            for (const QModelIndex& index : selectedIndexs)
            {
                if (index.isValid())
                {
                    QString fileName = _pAppController->getFilePathFromTreeModel(index).trimmed();

                    if (fileName.isEmpty())
                        continue;
                    if (!fileName.endsWith(".pro") && !fileName.endsWith(".seq") && !fileName.endsWith(".obj"))
                        continue;

                    toScxmlExportTheFile(fileName, dir);
                }
            }
        }
    }
}

void GTAMainWindow::exportDoctoScxmlSlot()
{
    QFutureWatcher<bool> *pWatcher = dynamic_cast<QFutureWatcher<bool> *>(sender());
    if(pWatcher == nullptr)
        return;
    else
    {
        _exportDocToSCXMLcount_SLOT = _exportDocToSCXMLcount_SLOT + 1;
        hideProgressBar();
        if ((_exportDocToSCXMLcount - _exportDocToSCXMLcount_SLOT) != 0)
        {
            QString ProgressMsg = QString("Exporting %1 file(s) to SCXML").arg(QString::number(_exportDocToSCXMLcount - _exportDocToSCXMLcount_SLOT));
            showProgressBar(ProgressMsg, this->geometry());
        }
        pWatcher->deleteLater();

        if (_exportDocToSCXMLcount_SLOT == _exportDocToSCXMLcount)
        {
            if (_exportDocToSCXMLcount_SLOT > 1)
                QMessageBox::information(this, "Export SCXML", "Export Multiple Doc to SCXML completed");
            else
                QMessageBox::information(this, "Export SCXML", "Export Doc to SCXML completed");
        }
    }
}

void GTAMainWindow::onExportPBClicked()
{
    GTAEditorWindow *pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if (pCurrSubWindow)
    {
        pCurrSubWindow->exportSXCML();
    }
}

void GTAMainWindow::onConvertCsvToProClicked()
{
    QStringList errmsg;
    if (_last_csv_selection_path.isEmpty())
        _last_csv_selection_path = _pAppController->getGTADataDirectory();

    QString fullpath;
    if (!_isConsole)
    {
        _last_csv_selection_file.clear();
        fullpath = QFileDialog::getOpenFileName(this, "Open a csv file", _last_csv_selection_path, "csv file (*.csv)");
        if (fullpath.isEmpty()) {
            qWarning() << "Csv filepath is empty";
            return;
        }
        else {
            _last_csv_selection_path = QFileInfo(fullpath).path();
            _last_csv_selection_file = fullpath;
        }
    }
    else
    {
        fullpath = _last_csv_selection_file;
    }

    _csvConverter.setParent(this);
    _ReportWidget->onClearLog();
    _ReportWidget->setLogLevel(3);
    auto fileData = _csvConverter.setAndCheckCsv(fullpath);
    if (fileData.has_value()) {
        addCsvFile(fileData->fileName, fileData->authorName);
    }
    
    //Csv parser created in constructor
    //parse csv get list of actions
    //these should be a member of mainwindow
    
    return;
}

QString GTAMainWindow::getCsvTableFullPath(const QString& elementName)
{
    QString csvDir = "output";
    QString csvName = elementName + "_" + _dateTestReport + "_Test_Report.csv";
    QString csvFullPath = QDir(csvDir).absolutePath() + "/" + csvName;
    return _TestReportCsvSavePath.isEmpty() ? csvFullPath : _TestReportCsvSavePath;
}

QString GTAMainWindow::getLogFullPath(const QString& elementName)
{
    QString logDir = _pAppController->getGTALogDirectory();
    QString logName = elementName + ".log";
    QString logFullPath = logDir + "/" + logName;
    return logFullPath;
}

void GTAMainWindow::packChecksFromLog(QList<QStringList>& newList)
{
    QList<QStringList> checks = newList;
    newList.clear();
    for (const QStringList& currentCheck : checks)
    {
        if (currentCheck.length() == 5)
            newList.append(currentCheck);
    }

    for (QStringList& currentList : newList)
    {
        for (const QStringList& check : checks)
        {
            if (check.length() == 6 && currentList[0] == check[0])
            {
                currentList.append(check[4]);
                currentList.append(check[5]);
            }
        }
        currentList.removeAt(0);
    }
}

bool GTAMainWindow::testCaseTimeStampIdAreNumeric(QString& testcase, QString& timestamp, QString& row)
{
    bool isIdNumeric = false;
    row.toULongLong(&isIdNumeric);

    bool isTimeStampNumeric = false;
    timestamp.toULongLong(&isTimeStampNumeric);

    bool isTestCaseNumeric = false;
    testcase.toULongLong(&isTestCaseNumeric);

    return isIdNumeric && isTimeStampNumeric && isTestCaseNumeric;
}

void GTAMainWindow::appendChecksFromLine(QString& line, QList<QStringList>& checks)
{
    QStringList splitLine;
    QStringList first;
    QStringList second;

    if (line.contains("#Check Value "))
    {
        splitLine = line.split("#Check Value ");
        if (splitLine.length() == 2)
        {
            first = splitLine.at(0).split("]", QString::SkipEmptyParts);
            second = splitLine.at(1).split(":;:", QString::SkipEmptyParts);

            QString timestamp = first[0].remove("[");
            QString testcase = first[1].split("_").at(0).split("TS").last();
            QString row = first[1].split("[").last();
            QString command = second.at(0);

            if (testCaseTimeStampIdAreNumeric(testcase, timestamp, row))
            {
                //QString dateTime = QDateTime::fromMSecsSinceEpoch(timestamp.toLongLong()).toString("yyyy-MM-ddThh:mm:ss.zzzzzz");
                if (second.at(1) == "OK" || second.at(1) == "KO")
                {
                    QString commandStatus = second.at(1);
                    checks.append({ row, testcase, command, timestamp, commandStatus });
                }
                else
                {
                    QString parameterName = second.at(2);
                    QString parameterValue = second.at(3);
                    checks.append({ row, testcase, command, timestamp, parameterName, parameterValue });
                }
            }
        }
    }
}

QList<QStringList> GTAMainWindow::getCheckValueReportFromLog(const QString& logFullPath)
{
    QFile file(logFullPath);
    QList<QStringList> checks;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        while (!stream.atEnd()) {
            QString line = stream.readLine();
            appendChecksFromLine(line, checks);
        }
    }
    file.close();
    packChecksFromLog(checks);
    return checks;
}

QList<QStringList> GTAMainWindow::getPrintTimeValueReportFromLog(const QString& logFullPath)
{
    QFile file(logFullPath);
    QList<QStringList> times;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        while (!stream.atEnd()) {
            QString line = stream.readLine();
            QStringList lineInfo;
            QRegularExpression regex("\\[(\\d+)\\]\\s+INFO\\s+TS(\\d+)_\\w+");
            QRegularExpressionMatch match = regex.match(line);
            if (match.hasMatch()) {
                if (line.contains("#print time"))
                {
                    lineInfo.append(match.captured(1));
                    lineInfo.append(match.captured(2));
                    times.append(lineInfo);
                }         
            }
        }
    }
    file.close();
    return times;
}

void GTAMainWindow::appendTestCasesFromLine(QString& line, QList<QStringList>& testcases)
{
    QStringList splitLine;
    QStringList first;
    QStringList second;

    if (line.contains("#Test_Case_"))
    {
        splitLine = line.split("#Test_Case_");
        if (splitLine.length() == 2)
        {
            first = splitLine.at(0).split("\t", QString::SkipEmptyParts); // Split by tab
            second = splitLine.at(1).split(":;:", QString::SkipEmptyParts);

            QString ts_value = first[2];
            QString test_case = second.at(0);

            testcases.append({ts_value, test_case });
        }
    }
}

QList<QStringList> GTAMainWindow::getTestCasesReportFromLog(const QString& logFullPath)
{
    QFile file(logFullPath);
    QList<QStringList> testCases;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        while (!stream.atEnd()) {
            QString line = stream.readLine();
            appendTestCasesFromLine(line, testCases);
        }
    }
    file.close();
    return testCases;
}

QString GTAMainWindow::getTestReportFileNameToSave(const QString& elementName, bool silentMode)
{
    QDateTime currentDateTime = QDateTime::currentDateTime().toUTC();
    _dateTestReport = currentDateTime.toString("yyyyMMddhhmmss");
    QString outputDir = "output";
    if (!QDir(outputDir).exists() && _TestReportCsvSavePath.isEmpty()) QDir().mkdir(outputDir);
    QString tableDir = QDir(outputDir).absolutePath();
    QString tablePath = _TestReportCsvSavePath.isEmpty() ? tableDir + "/" + elementName + "_" + _dateTestReport + "_Test_Report.csv" : _TestReportCsvSavePath;
    return silentMode ? tablePath : QFileDialog::getSaveFileName(this, "Save as csv", tablePath, "CSV files (.csv)", 0, 0); // getting the filename (full path)
}

bool GTAMainWindow::writeCheckValueReportToCsv(const QString& elementName, QVector<QVector<QString>>& testReport, bool silentMode)
{
    if (testReport.size() == 0 && !silentMode)
    {
        QMessageBox::warning(this, "Test report generation aborted", "Warning: No data available in the procedure!");
        return false;
    }
    QString filename = getTestReportFileNameToSave(elementName, silentMode);
    QFile data(filename);

    if (data.open(QFile::WriteOnly | QFile::Truncate))
    {
        QTextStream output(&data);
        int numberOfChecks = testReport[0].size();
        if (numberOfChecks > 6)
        {
            for (const auto& row : testReport)
            {
                for (const auto& col : row)
                {
                    output << col << ";";
                }
                output << "\n";
            }
        }
        else
        {
            if(!silentMode)
                QMessageBox::warning(this, "Test report generation aborted", "Warning: No CHECK_VAL available in the procedure!");
            return false;
        }
    }
    else
    {
        /*if (!silentMode)
            QMessageBox::critical(this, "Test report generation error", "Error: " + filename + " file cannot be opened!");
            */
        return false;
    }
    return true;
}

QVector<QVector<QString>> GTAMainWindow::makeTestReport(const QList<QStringList>& fullListOfChecks,const QList<QStringList>& fullListOfTimes, const QList<QStringList>& fullListOfTestCases)
{
    int NumberOfTestCases = 0;
    QVector<QStringList> valueToCheckMap;
    QVector<QStringList> filteredAssociationsCheckCase;
    QStringList allCheckCasesFiltred;
    QStringList uniqueValues;
    QVector<QString> firstRow = { "Input Generation Status" };
    QVector<QString> secondRow = { "" };

    for (const auto& check : fullListOfChecks)
    {
        QString value = check[1];
        int tcIdx = check[0].toInt();
        valueToCheckMap.append({ value, QString::number(tcIdx) });
    }

    for (const auto& testCase : fullListOfTestCases)
    {
        int tcIdx = testCase[0].mid(2).toInt();
        QVector<QStringList> filteredAssociations;
        for (const auto& association : valueToCheckMap)
        {
            int associatedTcIdx = association[1].toInt();
            if (tcIdx == associatedTcIdx)
            {
                filteredAssociations.append(association);
            }
        }
        filteredAssociationsCheckCase.append(filteredAssociations);
        // Iterate over the filtered keys
        for (const auto& association : filteredAssociations)
        {
            QString check = association[0];
            QString valueName = check.split("(").first();
            QString associatedTestCase = association[1];
            if (!uniqueValues.contains(valueName))
            {
                uniqueValues.append(valueName);
            }
            allCheckCasesFiltred.append(check);
        }
    }

    for (const QString& valueName : uniqueValues)
    {
        firstRow.append("Actual_Output");
        secondRow.append(valueName);
    }

    firstRow.append({ "Report_Start_GMT",  "Report_End_GMT",   "Report_GTA",   "Report_Video_Comparison",  "Report_RS422" });
    secondRow.append({ "",                  "",                 "PASS/FAIL",    "PASS/FAIL",                "PASS/FAIL" });

    QVector<QVector<QString>> testReportTable = { firstRow, secondRow };
    for (const auto& testCase : fullListOfTestCases)
    {
        // Create a QMap to store the data for each unique checkName
        QMap<QString, QString> checkData;
        QStringList Status;
        bool containsKO = false;
        bool containsNA = false;
        bool containsOnlyOk = true;
        bool emptyRow = true;
        int tcIdx = testCase[0].mid(2).toInt(); // Extract the Test_Case index
        QVector<QString> row = {""};
        QVector<long long> gmt; 

        for (const auto& checkCase : filteredAssociationsCheckCase)
        {
            QString checkName = checkCase[0].split("(").first();

            // Initialize the data for this checkName if it doesn't exist
            if (!checkData.contains(checkName))
            {
                checkData[checkName] = "";
            }

            for (const auto& check : fullListOfChecks)
            {
                const int checkIdx = check[0].toInt();
                if (checkIdx == tcIdx && check[1] == checkCase[0])
                {
                    emptyRow = false;
                    QString actualValue = check.length() >= 6 ? check[5] : "";
                    QString expectedValue = checkCase[0].split(" = ").at(1).split("(").at(0);
                    bool isNumeric = false;
                    double numericValue = actualValue.toDouble(&isNumeric);
                    if (actualValue.isEmpty())
                    {
                        Status.append("NA");
                    }
                    else if (isNumeric)
                    {
                         if (numericValue != expectedValue.toDouble())
                         {   
                            Status.append("KO");
                         }
                         else if (numericValue == expectedValue.toDouble())
                         {
                            Status.append("OK");
                         }
                        checkData[checkName] = actualValue;
                    }
                    else
                    {
                        QRegularExpression regex("\"Value\":(\\d+(?:\\.\\d+)?)");
                        QRegularExpressionMatch match = regex.match(actualValue);
                        if (actualValue.isEmpty())
                        {
                            Status.append("NA");
                        }
                        else if (match.hasMatch())
                        {
                            QString extractedValue = match.captured(1);
                            double value = extractedValue.toDouble();
                            if (value != expectedValue.toDouble())
                            {
                                Status.append("KO");
                            }

                            else if (value == expectedValue.toDouble())
                            {
                                Status.append("OK");
                            }
                            // Append the extracted value to the corresponding column for this checkName
                            checkData[checkName] = QString::number(value);
                        }
                    }
                }
            }
        }
        for (const auto& time : fullListOfTimes)
        {
            const int timeIdx = time[1].toInt();
            if (timeIdx == tcIdx)
            {
                gmt.append(time[0].toLongLong());
            }
        }
        for (auto status : Status)
        {
            if (status == "KO")
            {
                containsKO = true;
                break;
            }
            else if (status == "NA")
            {
                containsNA = true;
            }
            else if (status != "OK")
            {
                containsOnlyOk = false;
            }
        }
        if (containsKO)
            row[0] = "KO";
        else if (!containsOnlyOk || containsNA || emptyRow)
            row[0] = "NA";
        else
            row[0] = "OK";

        // Append the data for each unique checkName to the row
        for (const QString& columnName : secondRow)
        {
            if (checkData.contains(columnName))
            {
                row.append(checkData[columnName]); // Add the value from checkData for the corresponding column
            }
        }

        QString gmtStart = QDateTime::fromMSecsSinceEpoch(*std::min_element(gmt.constBegin(), gmt.constEnd()), QTimeZone::utc()).toString("yyyy-MM-ddThh:mm:ss.zzzzzz");
        QString gmtEnd = QDateTime::fromMSecsSinceEpoch(*std::max_element(gmt.constBegin(), gmt.constEnd()), QTimeZone::utc()).toString("yyyy-MM-ddThh:mm:ss.zzzzzz");

        if (!gmtStart.startsWith("1970") && !gmtEnd.startsWith("1970"))
        {
            
            row.append({ gmtStart, gmtEnd, row.front(), "", "" });
        }
        else
        {
            row.append({ "0", "0", row.front(), "", "" });
        }
        testReportTable.append(row);
    }

    return testReportTable;
}


void GTAMainWindow::generateReport(const QString& logFullPath, const QString& elementName, bool silentMode)
{
    QList<QStringList> fullListOfchecks = getCheckValueReportFromLog(logFullPath);
    QList<QStringList> fullListOfTimes = getPrintTimeValueReportFromLog(logFullPath);
    QList<QStringList> fullListOfTestCases = getTestCasesReportFromLog(logFullPath);
    QVector<QVector<QString>> testReport = makeTestReport(fullListOfchecks, fullListOfTimes, fullListOfTestCases);
    if(writeCheckValueReportToCsv(elementName, testReport, silentMode) && !silentMode)
        QMessageBox::information(this, "Test report generation finished", "Test report have been generated successfully");
}

QString GTAMainWindow::findLatestLogFile(const QString& folderPath, const QString& baseLogFileName)
{
    QDir folderDir(folderPath);

    // Filter the directory to show files with the specified prefix and .log suffix
    QStringList filters;
    filters << baseLogFileName + ".log"
        << baseLogFileName + "-*.log"
        << baseLogFileName + "_*.log";
    folderDir.setNameFilters(filters);

    QFileInfoList fileList = folderDir.entryInfoList(QDir::Files, QDir::Time);

    if (fileList.isEmpty()) {
        return QString();
    }

    return fileList.first().absoluteFilePath();
}

void GTAMainWindow::onExportTestReportToCsv(bool silentMode)
{
    const QModelIndexList indexList = ui->DataTV->selectionModel()->selectedRows(0);
    const QModelIndex index = indexList.at(0);
    const QString treeModelPath = _pAppController->getFilePathFromTreeModel(index);
    QString elementName = treeModelPath.right(treeModelPath.size() - (treeModelPath.lastIndexOf("/") + 1));
    elementName.remove("." + _pAppController->getExtensionFromTreeModel(index));
    const QString logName = elementName;
    QString logDir = _pAppController->getGTALogDirectory();
    QString latestLogFile = findLatestLogFile(logDir, logName);

    if (QFileInfo::exists(latestLogFile))
    {
        generateReport(latestLogFile, elementName, silentMode);
    }
    return;
}

void GTAMainWindow::onExportTestReportToCsvClicked()
{
    onExportTestReportToCsv();
    return;
}

void GTAMainWindow::onExportTestReportToCsvToolBarClicked(bool silentMode)
{
    if (_pAppController)
    {
        GTAEditorWindow* pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
        if (pCurrSubWindow)
        {

            GTAElement* pElement = pCurrSubWindow->getElement();
            QString logDir = _pAppController->getGTALogDirectory();
            const QString element = pCurrSubWindow->getElementName();
            const QString elementName = element.mid(0, element.lastIndexOf("."));
            const QString logName = elementName ;

            QString latestLogFile = findLatestLogFile(logDir, logName);

            if (QFileInfo::exists(latestLogFile))
            {
                generateReport(latestLogFile, elementName, silentMode);
            }
        }
    }
    return;
}

/**
 * @brief Exports selected files as plain text and saves them in .txt format 
*/
void GTAMainWindow::onExportAsPlainTextXmlClicked() {
    if (ui->DataTV->selectionModel() != nullptr)
    {
        QModelIndexList selectedItems = ui->DataTV->selectionModel()->selectedRows(0);
        if (!selectedItems.isEmpty()) {
            if (_last_plain_xml_export_path.isEmpty()) {
                _last_plain_xml_export_path = QDir::cleanPath(QDir::currentPath() + "/");
            }
            QString outDirPath = QFileDialog::getExistingDirectory(this, tr("Select Directory"), _last_plain_xml_export_path, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

            _last_plain_xml_export_path = outDirPath;

            QStringList exportFails;
            for (const auto& idx : selectedItems) {
                QString fileName = _pAppController->getFilePathFromTreeModel(idx).trimmed();

                if (fileName.isEmpty() || !fileName.endsWith(".pro") && !fileName.endsWith(".seq") && !fileName.endsWith(".obj") && !fileName.endsWith(".fun")) {
                    continue;
                }

                GTAElement* pElement;
                QString filePath = _pAppController->getGTADataDirectory() + fileName;
                _pAppController->getElementFromDocument(filePath, pElement, false, false, true);
                pElement->updateCommandInstanceName(QString());
                auto cmds = pElement->GetAllCommands();
                QStringList cmdsText;
                for (const auto& cmd : cmds) {
                    if (cmd == nullptr) {
                        continue;
                    }
                    QString cmdText = cmd->getInstanceName().split("#").last();
                    if (!cmdText.endsWith("\n")) {
                        cmdText += "\n";
                    }
                    cmdsText.push_back(cmdText);
                }
                QString txtFileName = filePath.split("/").last().split(".").first() + "_export_" + QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss").replace("-", "") + ".txt";
                QString txtFilePath = outDirPath + "/" + txtFileName;
                QFile file(txtFilePath);
                file.open(QIODevice::WriteOnly);
                QTextStream out(&file);
                out << cmdsText.join("");
                file.close();
                
                // Check if file was written successfully
                if (!file.exists()) {
                    exportFails.append(txtFilePath);
                }
            }

            // Inform the user on the export final status
            if (exportFails.isEmpty()) {                                
                QMessageBox::information(this, "Export file in txt", 
                    QString("Succeeded to export all selected file(s) in.txt format at \n%1").arg(outDirPath), QMessageBox::Ok);
            }
            else {             
                QMessageBox::warning(this, "Export file in txt", 
                    QString("Failed to export in .txt format the following file(s) : \n%1").arg(exportFails.join("\n")), QMessageBox::Ok);
            }
        }
    }
}

void GTAMainWindow::unlockDocumentInSvnBeforeDeleting(QModelIndex idx)
{
    SVNInputParam svnInput;
    svnInput.command = "Unlock";
    svnInput.cmdIndex = idx;
    svnInput.userCredential = _usrCredentials;
    _pAppController->executeSVNAction(svnInput);
}

void GTAMainWindow::onDeleteDataPBClicked()
{
    if(ui->DataTV->selectionModel() != nullptr)
    {
        QModelIndexList selectedItems =  ui->DataTV->selectionModel()->selectedIndexes();
        if(!selectedItems.isEmpty())
        {

            QMessageBox msgBox (this);
            QString text = QString("Selected file(s) (and links to favorites) will be removed permanently from the database, do you want to delete them?");
            msgBox.setText(text);
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setWindowTitle("File Delete Warning");
            msgBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::No);
            int ret = msgBox.exec();
            if(ret==QMessageBox::Yes)
            {
                QStringList dirPaths = _pAppController->getFileWatcherList();
                _FileWatcher.removePaths(dirPaths);
                disconnect(&_FileWatcher, SIGNAL(directoryChanged(QString)), this, SLOT(onDirectoryChanged(QString)));

                int previousRow = -1;
                bool isFileDeleted = false;
                for(int i = 0; i < selectedItems.count(); i++)
                {

                    QModelIndex idx = selectedItems.at(i);
                    int row = idx.row();
                    if(row != previousRow)
                    {
                        if(idx.column() == 0 || idx.column() == 1)
                        {
                            QString fileName = _pAppController->getFilePathFromTreeModel(idx);
                            QString absFilePath = QString("%1%2").arg(_pAppController->getGTADataDirectory(),fileName);

                            if(fileName.isEmpty())
                                return;

                            QString sElementName = fileName.mid(0,fileName.indexOf("."));
                            sElementName.trimmed();

                            QString FileName;
                            if(fileName.count("/") == 1)
                            {
                                FileName = fileName;
                                FileName.remove("/");
                            }
                            else if(fileName.count("/") > 1)
                            {
                                QStringList temp = fileName.split("/",QString::SkipEmptyParts);
                                FileName = temp.last();
                            }

                            bool isloaded = isFileLoaded(fileName);

                            if (isloaded)
                            {
                                QMessageBox::StandardButton button = QMessageBox::information(this,"Deletion Error","File is already loaded.\nPress Ok to unload and delete the element",QMessageBox::Ok,QMessageBox::Cancel);
                                if(button == QMessageBox::Cancel)
                                    return;
                                else if(button == QMessageBox::Ok)
                                {
                                    GTAEditorWindow* pWindow = getLoadedWindow(fileName);
                                    TabIndices windowPos = pWindow->getFramePosition();
                                    ui->EditorFrame->closeTab(*windowPos.viewIndex, *windowPos.tabIndex);
                                    QTabWidget* currentTabWidget = ui->EditorFrame->getTabWidget(*windowPos.viewIndex);

                                    if (ui->EditorFrame->getViewsCount() > 1 && currentTabWidget->count() == 0)
                                    {
                                        ui->EditorFrame->closeView(*windowPos.viewIndex);
                                        ui->EditorFrame->updateWindowsPosition(0, -1);
                                    }
                                    else
                                    {
                                        int tabIndex;
                                        if (*windowPos.tabIndex == 0 || ui->EditorFrame->getTabWidget(*windowPos.viewIndex)->count() < 2)
                                            tabIndex = 0;
                                        else
                                            tabIndex = *windowPos.tabIndex;

                                        ui->EditorFrame->updateWindowsPosition(*windowPos.viewIndex, tabIndex);
                                    }
                                    updateMoveUI();
                                }
                            }
                            QStringList listOfDoc;

                            unlockDocumentInSvnBeforeDeleting(idx);
                            bool rc = _pAppController->deleteDocument(fileName,listOfDoc);
                            if(rc == false)
                            {
                                QString errMsg;
                                if(! listOfDoc.isEmpty())
                                {
                                    errMsg = QString("Document %1 can not be deleted as it is being used in following documents").arg(fileName);
                                }
                                else
                                    errMsg = QString("Document %1 can not be deleted as user do not have permission").arg(fileName);

                                GTAMessageBox msgBox(GTAMessageBox::Critical,"Delete Document") /*= GTAMessageBox(GTAMessageBox::Critical,"Delete Document")*/;
                                msgBox.setWindowTitle("Delete Document");
                                msgBox.setMessage(errMsg);
                                msgBox.setAdditionalData(listOfDoc);
                                msgBox.setAdditionalDataLabel("Document used by");
                                msgBox.exec();

                            }

                            isFileDeleted |= rc;

                        }


                    }
                    previousRow = row;
                }

                if(isFileDeleted)
                {
                    ui->DataTypeCB->setCurrentIndex(0);
                    ui->DataSearchLE->clear();
                }

                connectFoldersToWatcher();
                connect(&_FileWatcher, SIGNAL(directoryChanged(QString)), this, SLOT(onDirectoryChanged(QString)));
                updateDataBrowserModel();
            }
        }
    }
}

void GTAMainWindow::onDocumentUsedByClicked()
{
    if(ui->DataTV->selectionModel() != nullptr && _pAppController!= nullptr)
    {
        QModelIndexList selectedItems =  ui->DataTV->selectionModel()->selectedIndexes();
        if(! selectedItems.isEmpty())
        {
            //            QString fileName = ui->DataTV->model()->data(selectedItems.at(0)).toString();

            QString fileName = _pAppController->getFilePathFromTreeModel(selectedItems.at(0));
            if(fileName.isEmpty())
                return;

            QStringList docUsesList = _pAppController->getDocumentUses(fileName);

            GTAMessageBox msgBox;
            QString msg = QString("The %1 is used by following documents\n ").arg(fileName);
            if(docUsesList.isEmpty())
            {
                msg = "Document is not used by any other document";
            }

            msgBox.setMessage(msg);
            msgBox.setAdditionalDataLabel("Document used by");
            msgBox.setAdditionalData(docUsesList);
            msgBox.exec();
        }
    }
}

void GTAMainWindow::onStopSCXMLClicked()
{
    _pAppController->stopSCXML();
}

QString GTAMainWindow::getLastRecordFromLogFile(const QString& logFullPath)
{
    QFile file(logFullPath);
    QString lastRecord;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        QStringList logFile = stream.readAll().split("\n");
        lastRecord = logFile.takeLast();
        while (lastRecord.isEmpty())
            lastRecord = logFile.takeLast();
    }
    file.close();
    return lastRecord;
}

void GTAMainWindow::onLaunchPBClicked()
{
    GTAEditorWindow::ExecutionMode mode = GTAEditorWindow::NORMAL_MODE;
    GTAEditorWindow *pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if (pCurrSubWindow)
    {
        QMessageBox msgBox(this);
        QString text = QString("Run check compatibility report before launch ?");
        msgBox.setText(text);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("Run compatibility");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        QPushButton* abortButton = new QPushButton(QIcon(":/images/forms/img/abort.png"), "Abort");
        msgBox.setWindowFlags(Qt::CustomizeWindowHint | Qt::Dialog | Qt::WindowTitleHint);
        msgBox.addButton(abortButton, QMessageBox::RejectRole);
        msgBox.exec();
        QAbstractButton* buttonClicked = msgBox.clickedButton();
        if (buttonClicked != nullptr)
        {
            QMessageBox::ButtonRole role = msgBox.buttonRole(buttonClicked);
            if (role == QMessageBox::YesRole)
            {
                clearLogWindow();
            }
            pCurrSubWindow->launch(mode, role);
        }
    }
}

/**
* @brief Slot triggered with file right-click action "update validation status". Like the name says, it updates the validation status of the file.
* @param isSwitchingView : boolean to trigger validation popup when you swap from result view to procedure view
*/
void GTAMainWindow::executeValidationWidget(bool isSwitchingView)
{
    GTALogFrame * activeLogFrame = nullptr;
    activeLogFrame = dynamic_cast<GTALogFrame*>(getActiveSubWindow());
    QObject* pSender = sender();
    if (pSender == _pUpdateValidationStatus && _pSettingWidget->getValidationEnabler())
    {
        QModelIndexList selectedIndexs = ui->DataTV->selectionModel()->selectedRows(0);
        if (!selectedIndexs.isEmpty())
        {
            QModelIndex index = selectedIndexs.at(0);
            if (index.isValid())
            {
                QString fileName = _pAppController->getFilePathFromTreeModel(index,true).trimmed();
                QString uuid = fileName.split("::").last();
                fileName = fileName.split("::").first();
                GTAElement* pElem = nullptr;

                _pAppController->getElementFromDocument(uuid,pElem,true);

                LogModelInput input;
                LogModelOutput output;
                input.docFileName = fileName;
                input.logFileName = "";
                output = _pAppController->getLogModel(input,true);
                if (isSwitchingView) 
                    onChangeToEditorView(true);
                if (output.pValidationModel != nullptr)
                {
                    GTAValidationListWidget *pValWgt = new GTAValidationListWidget();
                    connect(pValWgt, SIGNAL(selectedFiles(QString,QStringList)), this, SLOT(onValidationWindowClosed(QString, QStringList)));
                    pValWgt->setModel(output.pValidationModel);
                    pValWgt->setFileName(fileName);
                    pValWgt->show();
                    if (isSwitchingView) 
                        onChangeToEditorView(true);
                }
                else
                {
                    QMessageBox::critical(this,"Updation of Validation Status Failed","Unable to update validation status. Perform a Result analysis and proceed!");
                }

                if (output.pModel != nullptr)
                {
                    delete output.pModel;
                    output.pModel = nullptr;
                }
            }
        }
    }
    else if (nullptr != activeLogFrame && _pSettingWidget->getValidationEnabler())
    {
        LogModelInput input;
        LogModelOutput output;
        QString fileName = activeLogFrame->getLastElementAnalyzedPath();
        input.docFileName = fileName;
        input.logFileName = "";
		output = _pAppController->getLogModel(input,true);
        

        if (output.pValidationModel != nullptr)
        {
            GTAValidationListWidget *pValidationWidget = new GTAValidationListWidget();
            if(pValidationWidget != nullptr)
            {
                connect(pValidationWidget,SIGNAL(selectedFiles(QString,QStringList)),this,SLOT(onValidationWindowClosed(QString,QStringList)));
                connect(pValidationWidget,SIGNAL(changeToEditorView(bool)),this,SLOT(onChangeToEditorView(bool)));
                pValidationWidget->setFileName(fileName);
                pValidationWidget->setModel(output.pValidationModel);
                pValidationWidget->show();
            }
        }
        else
        {
            //move to editor view if the validation model is not available
            onChangeToEditorView(true);
        }
    }
    else
    {
        //move to editor view if the LogFrame is not available
        onChangeToEditorView(true);
    }
}

/**
* @brief Execute the validation status modification on accepting validation widget.
* @param validator
* @param uuidList List of uuid files
*/
void GTAMainWindow::onValidationWindowClosed(QString validator, QStringList uuidList)
{
    showTreeProgressBar();
    onOpenDocument();
    _validationStatusToBeUpdatedOnTreeView = !uuidList.isEmpty();

    QFutureWatcher<void>* FutureWatcher = new QFutureWatcher<void>();
    connect(FutureWatcher, SIGNAL(finished()), this, SLOT(onupdateValidationInfoFinished()));
    QFuture<void> Future = QtConcurrent::run(this, &GTAMainWindow::onUpdateValidationInfo, validator, uuidList);
    FutureWatcher->setFuture(Future);
}

void GTAMainWindow::onSvnActionFinished()
{
    QSet<QString> svnChangedFiles;
    SVNInputParam svnInput;
    QString svnErrorMessage;
    bool ignorePWatcher = false;
    bool noCheckModifications = false;

    if(_svnActionCommand == "Update All Elements" || 
        _svnActionCommand == "Add All Elements" || 
        _svnActionCommand == "Commit All Elements" || 
        _svnActionCommand == "Update Directory")
        ignorePWatcher = true;

    QFutureWatcher<QString > *pWatcher = dynamic_cast<QFutureWatcher<QString> *>(sender());

    if(_svnActionCommand == "Check for modification")
    {
        if(pWatcher != nullptr)
        {
            QStringList output = pWatcher->result().split("\n");
            if(output.contains("<SVN Check For Modifications SUCCESS:>"))
            {
                output.removeOne("<SVN Check For Modifications SUCCESS:>");
                if((output.count() > 1))
                {
                    GTASVNCheckForModificationsWidget *pDisplay = new GTASVNCheckForModificationsWidget(this);
                    pDisplay->displayList(output);

                    hideProgressBar();
                    pDisplay->show();

                    svnInput.command = QString();
                    svnInput.cmdIndex= QModelIndex();
                    svnInput.fileName = QString();
                    svnInput.resolveOption = QString();
                    svnInput.userCredential = QStringList();
                    _svnActionCommand.clear();
                    return;
                }
                else
                {
                    noCheckModifications = true;
                }
            }
        }
    }
    if(pWatcher != nullptr || ignorePWatcher == true)
    {
        hideProgressBar();
        if(ignorePWatcher == true && pWatcher == nullptr && svnErrorMessage.isEmpty())
            svnErrorMessage = "Authentication failed";
        else if(noCheckModifications)
        {
            svnErrorMessage = " No modifications in Commands";
        }
        else if(svnErrorMessage.isEmpty())
            svnErrorMessage = pWatcher->result();
        QString message = "SVN " + _svnActionCommand + " executed successfully";
        bool authenticationfailed = false;
        QString saveSVNAuthStat;

        if(svnErrorMessage.isEmpty())
            QMessageBox::information(this,_svnActionCommand,message);

        else if(svnErrorMessage.contains("Authentication failed"))
        {
            QMessageBox::critical(this,_svnActionCommand,"Please enter valid user credential.\n SVN Authentication failed");
            authenticationfailed = true;
            saveSVNAuthStat = "false";
        }
        else
            QMessageBox::critical(this,_svnActionCommand,svnErrorMessage);

        if(_usrCredentials.isEmpty() == false)
        {
            if(saveSVNAuthStat != "false")
                saveSVNAuthStat = _usrCredentials.last();
            if(saveSVNAuthStat == "false")
            {
                if(authenticationfailed)
                    _usrCredentials.clear();
                else
                {
                    svnInput.command = "Clear SVN Authentication cache";
                    svnInput.cmdIndex = QModelIndex();
                    svnInput.userCredential = _usrCredentials;
                    _pAppController->executeSVNAction(svnInput);
                    _usrCredentials.clear();
                }
            }
        }
        saveSVNAuthStat = QString();

        for (const QString& relFilePath : _pSvnParamsTempContainer->fileList) 
        {
            QString absDirPath = _pAppController->getGTADataDirectory();
            QString absFilePath = QString("%1/%2").arg(absDirPath, relFilePath);
            svnChangedFiles.insert(absFilePath);
        }
    }
    else
    {
        if (_pSvnParamsTempContainer != nullptr) {
            delete _pSvnParamsTempContainer;
            _pSvnParamsTempContainer = nullptr;
        }
        return;
    }

    if (_svnActionCommand == "Delete")
    {
        auto viewCount = ui->EditorFrame->getViewsCount();
        for(int viewIndex = 0; viewIndex <viewCount; viewIndex++)
        {
            auto tabs = ui->EditorFrame->getAllWindows(viewIndex);
            for (int tabIndex = 0; tabIndex < tabs.size(); tabIndex++)
            {
                std::shared_ptr<QString> relFilePath = nullptr;
                while(relFilePath==nullptr) {
                    try {
                        GTAEditorWindow* tab = tabs.at(tabIndex);
                        relFilePath = std::make_shared<QString>(tab == nullptr ? "" : tab->getRelativePath());
                    }
                    catch (...) {
                        relFilePath = nullptr;
                    }
                }
                for (const QString& file : svnChangedFiles)
                {
                    const QString path = QString("%1%2").arg(_pAppController->getGTADataDirectory(), *relFilePath);
                    if (path == file)
                        ui->EditorFrame->closeTab(viewIndex, tabIndex);
                }
            }
        }
    }

    svnInput.command = QString();
    svnInput.cmdIndex= QModelIndex();
    svnInput.fileName = QString();
    svnInput.resolveOption = QString();
    svnInput.userCredential = QStringList();
    _svnActionCommand.clear();
    svnErrorMessage.clear();

    updateTree(false, false);
    if (_pSvnParamsTempContainer != nullptr) {
        delete _pSvnParamsTempContainer;
        _pSvnParamsTempContainer = nullptr;
    }
    connect(&_FileWatcher, SIGNAL(directoryChanged(QString)), this, SLOT(onDirectoryChanged(QString)));
}

void GTAMainWindow::onUpdateValidationInfo(QString validator, QStringList uuidList)
{
    //Document viewer updation and saving of relevant files based on UUID
    foreach(QString uuid, uuidList)
    {
        GTAElement *pElem = nullptr;
        _pAppController->getElementFromDocument(uuid,pElem,true,false);
        QString absoluteFilePath = _pAppController->getAbsoluteFilePathFromUUID(uuid);
        if (pElem != nullptr)
        {
            _pAppController->updatevalidationStatusonDV(uuid,XNODE_VALIDATED,validator,QDateTime::currentDateTime().toString());
            pElem->setValidatorName(validator);
            pElem->setValidationStatus(XNODE_VALIDATED);
            pElem->setValidationTime(QDateTime::currentDateTime().toString());
            _pAppController->saveDocument(pElem->getName(),true,pElem,pElem->getHeader(),absoluteFilePath);
        }
    }
}

void GTAMainWindow::onupdateValidationInfoFinished()
{
    hideTreeProgressBar();
    //changing the view to Editor View
    onChangeToEditorView(true);

    if (_validationStatusToBeUpdatedOnTreeView)
        onUpdateDataDB();
}

/**
 * This slot is called when a play is clicked from the title based execution model
 * @iUuid : uuid of the element to be executed
 * @iVarList : list of all database variables
*/
void GTAMainWindow::onExecuteTitle(const QString &iUuid, GTAElement *pTitleBasedElem,QStringList& iVarList)
{
    if(pTitleBasedElem)
    {

        QString sElementName;

        GTAElement *pElement = nullptr;
        _pAppController->getElementFromDocument(iUuid,pElement,true,false);


        if(pElement)
        {
            sElementName = pElement->getName();
            pTitleBasedElem->setName(sElementName);

        }
        pTitleBasedElem->setElementType(GTAElement::CUSTOM);
        bool rc = _pAppController->launchTitleBasedElement(pTitleBasedElem,sElementName,iVarList);
        if(!rc)
        {
            QStringList errMsgLst = _pAppController->getLastError().split("/n");
            ErrorMessageList msgList;
            foreach(QString errorMessage,errMsgLst)
            {
                ErrorMessage logMessage;
                logMessage.description = errorMessage;
                logMessage.errorType = ErrorMessage::ErrorType::kError;
                logMessage.source = ErrorMessage::MsgSource::kNoSrc;
                msgList.append(logMessage);

            }
            if(!msgList.isEmpty())
                onUpdateErrorLog(msgList);

        }
    }

}

void GTAMainWindow::onKeyF9Pressed()
{
    GTAEditorWindow * pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if (pCurrSubWindow)
    {
        pCurrSubWindow->F9KeyPressed();
    }
}

void GTAMainWindow::onKeyF2Pressed()
{
	GTAEditorWindow * pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if(pCurrSubWindow)
	{
		pCurrSubWindow->F2KeyPressed();
	}
}

void GTAMainWindow::onSettingsActionClick()
{
    _pSettingWidget->loadSettings();
    _pSettingWidget->show();
}

void GTAMainWindow::setUITheme(QColor iColor)
{

    // init the style of the application
    //#ifdef WIN32
    //qApp->setStyle( new QCl );
    //#else
    //qApp->setStyle( new QMotifStyle );
    //#endif

    QFont appFont = QApplication::font();

    QPalette p(iColor);
    qApp->setPalette( p );


}

/**
  * The function show the image, which represent the task is going on
  * Call must delete the return QLabel object once the process is completed
  */
void GTAMainWindow::showWorkInProgress()
{
}

void GTAMainWindow::onThemeActionClick()
{
    GTAUITheme * pThemeWidget = new GTAUITheme();
    pThemeWidget->show();
}

void GTAMainWindow::onAboutClick()
{
    QString welcome = "Welcome " + GTAAppController::getUserName() + " !";
    QString msg = "GTA-Ground Test Automation\n";
    msg += "Generic editor to create object, procedure, function & sequence\n\n";
    msg += "Version      : "+ QString(TOOL_VERSION_STR) +QString("\n"); //-- IKM
    msg += "Database Version : " + QString(GTA_DATABASE_VERSION_STR) + QString("\n");
    msg += "Released On  : " + QString(__DATE__) + " : " + QString(__TIME__) ;

    QDialog *aboutGTA = new QDialog();
    aboutGTA->setWindowTitle(welcome);
    aboutGTA->setAttribute(Qt::WA_DeleteOnClose);
    aboutGTA->setWindowIcon(QIcon(QPixmap(":/images/forms/img/GTAAppIcon_black.png")));
    aboutGTA->deleteLater();

    QGridLayout *gridlayout = new QGridLayout(aboutGTA);

    QLabel *infoHeader = new QLabel(aboutGTA);
    infoHeader->setText("GTA-Generic Test Automation");
    infoHeader->setStyleSheet("font-weight: bold; font: 16pt \"MS Shell Dlg 2\";");

    QLabel *infoFirstLine = new QLabel(aboutGTA);
    infoFirstLine->setText("Generic editor to create object, procedure, function & sequence");

    QLabel *infoVersionLine = new QLabel(aboutGTA);
    infoVersionLine->setText("Version");

    QLabel *infoVersion = new QLabel(aboutGTA);
    infoVersion->setText(": "+QString(TOOL_VERSION_STR));

    QLabel* infoDBVersionLine = new QLabel(aboutGTA);
    infoDBVersionLine->setText("Database Version");

    QLabel* infoDBVersion = new QLabel(aboutGTA);
    infoDBVersion->setText(": " + QString(GTA_DATABASE_VERSION_STR));

    QLabel *infoDateLine = new QLabel(aboutGTA);
    infoDateLine->setText("Released On");

    QLabel *infoDate = new QLabel(aboutGTA);
    infoDate->setText(": "+QString(__DATE__) + " : " + QString(__TIME__));

    QHBoxLayout*developedByLayout = new QHBoxLayout;
    QLabel *infoLastLine = new QLabel(aboutGTA);
    QLabel *enterpriseLogo = new QLabel(aboutGTA);
    infoLastLine->setText("Developed by");
    infoLastLine->setFixedWidth(100);
    enterpriseLogo->setPixmap(QPixmap(":/images/forms/img/logoCompany.png"));
    enterpriseLogo->setContentsMargins(0, 3, 0, 0);
    developedByLayout->addWidget(infoLastLine,1,0);
    developedByLayout->addWidget(enterpriseLogo, 1,0);
    developedByLayout->addStretch(1);

    QLabel *separator = new QLabel(aboutGTA);
    separator->setText("============================================================");
    QLabel *separator1 = new QLabel(aboutGTA);
    separator1->setText("____________________________________");
    QLabel *separator2 = new QLabel(aboutGTA);
    separator2->setText("____________________________________");

    QLabel *icon = new QLabel(aboutGTA);
    icon->setPixmap(QPixmap(":/images/forms/img/GTAAppIcon_black.png"));

    QLabel *infoSoftwares = new QLabel(aboutGTA);
    infoSoftwares->setText("Additional Info:");

    QGridLayout *gridlayoutInfo = new QGridLayout(aboutGTA);

    QPushButton* qtHelp = new QPushButton(aboutGTA);
    qtHelp->setObjectName("qtHelp");
    qtHelp->setIcon(QIcon(QPixmap(":/images/forms/img/Qt_icon.png")));
	qtHelp->setToolTip("QT Information");
    connect(qtHelp,SIGNAL(pressed()),this,SLOT(showQtInfo()));

    QPushButton* pluginHelp = new QPushButton(aboutGTA);
    pluginHelp->setObjectName("plugin");
    pluginHelp->setIcon(QIcon(QPixmap(":/images/forms/img/plugin.png")));
    pluginHelp->setToolTip("Plugin Information");
    connect(pluginHelp,SIGNAL(pressed()),this,SLOT(showQtInfo()));

    QPushButton* extLibsHelp = new QPushButton(aboutGTA);
    extLibsHelp->setObjectName("plugin");
    extLibsHelp->setIcon(QIcon(QPixmap(":/images/forms/img/copyright.png")));
    extLibsHelp->setToolTip("External Libraries Information");
    connect(extLibsHelp, SIGNAL(pressed()), this, SLOT(showExtLibsInfo()));


    gridlayout->addWidget(infoHeader,0,1,1,3);
    gridlayout->addWidget(icon,0,0,5,1);
    gridlayout->addWidget(infoFirstLine,1,1,1,3);
    gridlayout->addWidget(separator1,2,1,1,3);
    gridlayout->addWidget(infoVersionLine,3,1,1,1);
    gridlayout->addWidget(infoVersion,3,2,1,1);
    gridlayout->addWidget(infoDBVersionLine,4,1,1,1);
    gridlayout->addWidget(infoDBVersion,4,2,1,1);
    gridlayout->addWidget(infoDateLine,5,1,1,1);
    gridlayout->addWidget(infoDate,5,2,1,1);
    gridlayout->addWidget(separator2,6,1,1,3);
    gridlayout->addItem(developedByLayout,7,1,1,3);
    gridlayout->addWidget(separator,8,0,1,4);
    gridlayout->addWidget(infoSoftwares,9,0,1,4);

    gridlayoutInfo->addWidget(qtHelp,0,0,1,1);
    gridlayoutInfo->addWidget(pluginHelp,0,1,1,1);
    gridlayoutInfo->addWidget(extLibsHelp, 0, 2, 1, 1);

    gridlayout->addLayout(gridlayoutInfo,10,0,1,3,Qt::AlignLeft);

    aboutGTA->exec();
}

void GTAMainWindow::showQtInfo()
{
    QObject *pObj = sender();
    QWidget* pParent = dynamic_cast<QWidget*>(pObj->parent());
    if (nullptr != pObj && nullptr != pParent)
    {
        if (pObj->objectName() == "qtHelp")
            QMessageBox::aboutQt(pParent,"QT Info");
        else if (pObj->objectName() == "plugin")
        {
            bool rc = false;
            rc = _pAppController->loadPlugin();
            if (rc)
            {
                QString about = _pAppController->getPluginInterface()->aboutPlugin();
                QString ParserTypes = _pAppController->getParserTypes().join("\n");
                QString message = QString("%1\n\nPlugins Available: \n%2").arg(about, ParserTypes);
                QMessageBox::about(pParent, "Plugin Information", message);
                _pAppController->unloadPlugin();
            }
        }
    }
}

void GTAMainWindow::showExtLibsInfo()
{
    QObject* pObj = sender();
    QWidget* pParent = dynamic_cast<QWidget*>(pObj->parent());
    QString text = "GUI features\n"
        "-----------------------------------------\n"
        " > DraggableTabWidget.h/.cpp\n"
        "   Copyright (c) 2019 Akihito Takeuchi\n"
        "   https://github.com/akihito-takeuchi/qt-draggable-tab-widget\n"
        "   Distributed under the MIT License : http://opensource.org/licenses/MIT\n";
    QMessageBox::about(pParent, "External libraries", text);
}

//Dev Comment : new implementation
void GTAMainWindow::onClearPage()
{
    GTAEditorWindow *pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if (pCurrSubWindow)
    {
        pCurrSubWindow->clearEditorPage();
    }
}

//Dev Comment : new implementation
void GTAMainWindow::onShowHeader()
{
    GTAEditorWindow *pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if (pCurrSubWindow)
    {
        _headerChangesDone = false;
        onDisconnectFileWatcher();
        connect(pCurrSubWindow, SIGNAL(headerChangesDone()), this, SLOT(onHeaderChangesDone()));
        pCurrSubWindow->displayHeader();        
    }    
}

/**
 * @brief Show PurposeAndConclusion Dialog
*/
void GTAMainWindow::onShowPurposeAndConclusion()
{
    GTAEditorWindow* pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());

    if (pCurrSubWindow)
    {
        QString element = pCurrSubWindow->getRelativePath();
        if (element.endsWith(".pro"))
        {
            pCurrSubWindow->displayPurposeAndConclusionDialog();
        }
    }
}

void GTAMainWindow::onHighlightHeaderTableRow(int row)
{
    GTAEditorWindow* pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if (pCurrSubWindow)
    {
        pCurrSubWindow->highlightHeaderTableRow(row);
    }
}

void GTAMainWindow::createEditorToolBar()
{
    //Editor Tool bar
    QAction* pSearchReplace = ui->EditorToolBar->addAction(QIcon(":/images/forms/img/SEARCH.png"), "Find/Replace\nCtrl+F", this, SLOT(onSearchReplaceShow()));
    pSearchReplace->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F));

    QAction* pUndo = _pUndoStackGrp->createUndoAction(this, tr("&Undo\nCtrl+Z"));
    pUndo->setIcon(QIcon(":/images/forms/img/Undo.jpg"));
    pUndo->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z));
    ui->EditorToolBar->addAction(pUndo);

    QAction* pRedo = _pUndoStackGrp->createRedoAction(this, tr("&Redo\nCtrl+Y"));
    pRedo->setIcon(QIcon(":/images/forms/img/Redo.png"));
    pRedo->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Y));
    ui->EditorToolBar->addAction(pRedo);

    _pMoveRowUpAction =ui->EditorToolBar->addAction(QIcon(":/images/forms/img/UpArrow_16_16.png"),"Move rows up\nAlt+Up arrow",this,SLOT(onMoveEditorRowUp()));
    _pMoveRowUpAction->setShortcut(QKeySequence(Qt::ALT +  Qt::Key_Up));
    _pMoveDownAction =ui->EditorToolBar->addAction(QIcon(":/images/forms/img/DownArrow_16_16.png"),"Move rows down\nAlt+Down arrow",this,SLOT(onMoveEditorRowDown()));
    _pMoveDownAction->setShortcut(QKeySequence(Qt::ALT +Qt::Key_Down));

    _pCollapseToTitleAction = ui->EditorViewToolBar->addAction(QIcon(":/images/forms/img/Editing_Collapse_icon.png"),"Collapse to title",this,SLOT(onCollapseAllToTitle()));


    _pIndentedView = ui->EditorViewToolBar->addAction(QIcon(":/images/forms/img/Indentation.png"),"Indent/UnIndent view", this, SLOT(indentedView()));
    _pIndentedView->setCheckable(true);


    _pIgnoreOn = ui->EditorToolBar->addAction(QIcon(":/images/forms/img/ignoreOn.png"),"Ignore On\nCtrl+I",this,SLOT(onIgnoreOnSCXML()));
    _pIgnoreOn->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));

    _pIgnoreOff = ui->EditorToolBar->addAction(QIcon(":/images/forms/img/ignoreOff.png"),"Ignore off\nCtrl+Shift+I",this,SLOT(onIgnoreOffSCXML()));
    _pIgnoreOff->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_I));

    _pExportSCXML = ui->ExportToolBar->addAction(QIcon(":/images/forms/img/ExportXml.png"),"Export SCXML\nCtrl+Shift+E",this,SLOT(onExportPBClicked()));
    
    _pConvertCsvToPro = ui->ExportToolBar->addAction(QIcon(":/images/forms/img/ConvertCsvToPro.png"),
                                                    "Convert csv to pro\nAlt+C", this, SLOT(onConvertCsvToProClicked()));

    _pGenerateTestReport = ui->ExportToolBar->addAction(QIcon(":/images/forms/img/ExportToCsv.png"),
        "Generate Test Report", this, SLOT(onExportTestReportToCsvToolBarClicked()));
    _pGenerateTestReport->setEnabled(true);

    _pExportLTRA = ui->ExportToolBar->addAction(QIcon(":/images/forms/img/ExportLTRA.png"),"Export LTR\\LTRA\nCtrl+Shift+L");
    _pExportLTRA->setEnabled(false);

    _pSwitchPerspective = ui->ExportToolBar->addAction(QIcon(":/images/forms/img/Switch.png"),"Switch between Editor and Result View",this,SLOT(onSwitchClicked()));

    /*QAction * pShowDetail = ui->EditorToolBar->addAction(QIcon(":/images/forms/img/Relation.png"),"Show Details",this,SLOT(onShowCommandDetail()));
     pShowDetail->setEnabled(false);*/


    //Editor File Toolbar
    QAction * pNewElement = ui->EditorFileToolBar->addAction(QIcon(":/images/forms/img/NEW.png"),"New\nCreate new element\nCtrl+N",this,SLOT(onAddFile()));
    pNewElement->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));


    _pContextMenuOnDV = new QMenu(this);

    _pOpenDocAction = _pContextMenuOnDV->addAction(QIcon(":/images/forms/img/OpenFile.png"),"Open",this,SLOT(onOpenDocument()));
    _pOpenDocAction->setToolTip("Open\nLoads file in editor" );

    _pAddFolder = _pContextMenuOnDV->addAction(QIcon(":/images/forms/img/NEW.png"), "Add file", this, SLOT(onAddFile()));
    _pAddFolder = _pContextMenuOnDV->addAction(QIcon(":/images/forms/img/addfolder.png"), "Add folder", this, SLOT(onAddFolder()));

    _pAddToFavOnDV = _pContextMenuOnDV->addAction(QIcon(":/images/forms/img/addToFav.png"), "Add to Favorites", this, SLOT(onAddToFavorites()));
    _pAddToFavOnDV->setToolTip("Add to Favorites\nAdds the selected file to favorites");

    _pRemoveFromFav = ui->EditorToolBar->addAction(QIcon(":/images/forms/img/removeFromFav.png"), "Remove from Favorites", this, SLOT(onRemoveFromFavorites()));
    _pRemoveFromFav->setToolTip("Remove from Favorites\nRemoves the selected file to favorite list");
    _pContextMenuOnDV->addAction(_pRemoveFromFav);
    _pRemoveFromFav->setVisible(false);

    _pContextMenuOnDV->addSeparator();

    QAction * pSave = ui->EditorFileToolBar->addAction(QIcon(":/images/forms/img/SAVE.png"),"Save\nSave element in document\nCtrl+S",this,SLOT(onSaveEditorDoc()));
    pSave->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));

    _pSaveAs = ui->EditorFileToolBar->addAction(QIcon(":/images/forms/img/EditFile.png"),"Save as\n",this,SLOT(onSaveAs()));

    _pSaveAllAction = ui->EditorFileToolBar->addAction(QIcon(":/images/forms/img/SaveAll.png"),"Save all",this,SLOT(onSaveAll()));
    _pSaveAllAction->setToolTip("Save All open files");

    _pDocUsedByAction = _pContextMenuOnDV->addAction(QIcon(":/images/forms/img/UsedBy.png"),"Used By",this,SLOT(onDocumentUsedByClicked()));
    _pDocUsedByAction->setToolTip("Used By\nShows dependency of selected file with others");

    _pShowDependencies = _pContextMenuOnDV->addAction(QIcon(":/images/forms/img/Using.png"), "Show Dependencies", this, SLOT(onShowDependencies()));
    _pShowDependencies->setToolTip("Show Dependencies\nShows the elements used by the selected file");

    _pShowDetailOnDV = _pContextMenuOnDV->addAction(QIcon(":/images/forms/img/Relation.png"), "Show Details", this, SLOT(onShowElementDetail()));
    _pShowDetailOnDV->setToolTip("Show Details\nExpands and show details of file");

    _pContextMenuOnDV->addSeparator();

    _pDeleteDocAction =_pContextMenuOnDV->addAction(QIcon(":/images/forms/img/DELETE.png"),"Delete",this,SLOT(onDeleteDataPBClicked()));
    _pDeleteDocAction->setToolTip("Delete\nRemoves file permanently from disk");


    _pRename = _pContextMenuOnDV->addAction(QIcon(":/images/forms/img/Rename.png"),"Rename",this,SLOT(onRename()));

    //TBD BEGIN
   // _pSaveAsAction = _pContextMenuOnDV->addAction(QIcon(":/images/forms/img/EditFile.png"), "Save As", this, SLOT(onSaveAs()));
    _pSaveAsAction = _pContextMenuOnDV->addAction(QIcon(":/images/forms/img/EditFile.png"), "Save As", this, SLOT(onSaveAsItem()));
    //TBD End
    _pRefreshTreeAction = _pContextMenuOnDV->addAction(QIcon(":/images/forms/img/Refresh.png"),"Refresh File System View",this,SLOT(onUpdateDataDB()));
	_pMoveFiles = _pContextMenuOnDV->addAction(QIcon(":/images/forms/img/MoveFile.png"),"Move File(s)",this,SLOT(onMoveFiles()));

    _pContextMenuOnDV->addSeparator();

    _pExportDocAction = _pContextMenuOnDV->addAction(QIcon(":/images/forms/img/export.png"), "Export", this, SLOT(onExportDocsClicked()));
    _pExportDocAction->setToolTip("Export\nExports selected file, its dependent file(s) to user defined folder");


    _pExportSCXMLs = _pContextMenuOnDV->addAction(QIcon(":/images/forms/img/ExportXml.png"), "Generate SCXML", this, SLOT(onExportSCXMLPbClicked()));

    _pExportPlainXmls = _pContextMenuOnDV->addAction(QIcon(":/images/forms/img/ExportLTRA.png"), "Export file in txt", this, SLOT(onExportAsPlainTextXmlClicked()));

    _pExportCsvs = _pContextMenuOnDV->addAction(QIcon(":/images/forms/img/ExportToCsv.png"),
        "Generate Test Report", this, SLOT(onExportTestReportToCsvClicked()));

    _pCreateCallSequence = _pContextMenuOnDV->addAction(QIcon(":/images/forms/img/Add.png"), "create call sequence", this, SLOT(createCallSequence()));
    _pCreateCallSequence->setToolTip("create a sequence of call");
    _pCreateCallSequence->setEnabled(true);


    _pUpdateValidationStatus = _pContextMenuOnDV->addAction(QIcon(":/images/forms/img/Analyze.png"), "Update validation Status", this, SLOT(executeValidationWidget()));

    _pGitInit = ui->GitToolBar->addAction(QIcon(":/images/forms/img/git/git.png"), "Git init", this, SLOT(onGitInitClicked()));
    _pGitInit->setEnabled(true);

    _pGitStatus = ui->GitToolBar->addAction(QIcon(":/images/forms/img/Refresh.png"), "Update Git status", this, SLOT(onGitStatusClicked()));
    _pGitStatus->setEnabled(false);

    _pGitFolder = ui->GitToolBar->addAction(QIcon(":/images/forms/img/git/git-folder.png"), "Open git directory", this, SLOT(onGitDirectoryClicked()));
    _pGitFolder->setEnabled(false);

    _pOpenContainingFolderFromTree = _pContextMenuOnDV->addAction(QIcon(":/images/forms/img/OpenFolder.png"),
        "Open containing folder", this, SLOT(onOpenContainingFolderFromTree()));
    
    _pCommitChangesToGit = _pContextMenuOnDV->addAction(QIcon(":/images/forms/img/git/git.png"),
        "Commit changes to git", this, SLOT(onCommitChangesToGit()));
    
    _pDiffFiles = _pContextMenuOnDV->addAction(QIcon(":/images/forms/img/SEARCH.png"),
        "Diff Files", this, SLOT(onDiffFiles()));
    
    _pAddFileToCommit = _pContextMenuOnDV->addAction(QIcon(":/images/forms/img/AddFileForCommit.png"),
        "Add file for commit", this, SLOT(onAddFileForCommit()));
    
    _pRemoveFileFromGitWorkspace = _pContextMenuOnDV->addAction(QIcon(":/images/forms/img/Remove.png"),
        "Remove from Git workspace", this, SLOT(onRemoveFileFromGitWorkspace()));

    _pAddToExecutionList = _pContextMenuOnDV->addAction(QIcon(":/images/forms/img/AddToDashboardLauncher.png"),
        "Add to Procedure Dashboard Launcher", this, SLOT(onAddToExecutionList()));
}

void GTAMainWindow::onDoubleClickInTreeView(const QModelIndex &)
{
    onOpenDocument();
}

void GTAMainWindow::onOpenDocument(bool , bool , bool , bool )
{
    if(ui->DataTV->selectionModel() != nullptr)
    {
        QModelIndexList selectedIndexes = ui->DataTV->selectionModel()->selectedIndexes();
        if(!selectedIndexes.isEmpty())
        {
            QModelIndex indexToLoad = selectedIndexes.at(0);
            if(_pAppController)
            {
                QString fileName = _pAppController->getFilePathFromTreeModel(indexToLoad);
                openDocument(fileName);

            }
        }
    }
}

/**
 * @brief Searches for an item in the model by its filename.
 *
 * This function recursively searches through a QAbstractItemModel to find an item
 * that matches the specified filename. It looks for the filename in all nodes of the model,
 * including child nodes at all levels.
 *
 * @param model The model to search within. This should be a pointer to an instance
 *        of QAbstractItemModel or its subclass.
 * @param fileName The name of the file to search for in the model.
 * @param parent An optional QModelIndex representing the parent under which to search.
 *        Defaults to an invalid QModelIndex, which signifies the root of the model.
 * @return QModelIndex An index corresponding to the found item. If no item is found,
 *         an invalid QModelIndex is returned.
 */
QModelIndex searchInModel(QAbstractItemModel* model, const QString& fileName, const QModelIndex& parent = QModelIndex()) {
    for (int r = 0; r < model->rowCount(parent); ++r) {
        QModelIndex index = model->index(r, 0, parent); // assuming file names are in column 0
        QVariant data = model->data(index);
        if (data.toString() == fileName) {
            return index;
        }

        // Recursively search in children
        QModelIndex found = searchInModel(model, fileName, index);
        if (found.isValid()) {
            return found;
        }
    }
    return QModelIndex();
}

/**
 * @brief Opens a file by its name.
 *
 * This method searches for a file with the given name in the model associated with
 * the DataTV (a QTreeView) of the main window. If the file is found, it retrieves
 * the file path using the application controller and then opens the document.
 * If the file is not found, it displays an error message box.
 *
 * @param fileName The name of the file to be opened.
 */
void GTAMainWindow::openFileByName(const QString& fileName) {
    QModelIndex index = searchInModel(ui->DataTV->model(), fileName);
    if (index.isValid()) {
        QString filePath = _pAppController->getFilePathFromTreeModel(index);
        openDocument(filePath);
    }
    else {
        QMessageBox::critical(this, "Error", QString("Cannot Open file location"));// Handle file not found
    }
}

void GTAMainWindow::openDocument(const QString &fileName)
{
    if(!fileName.isEmpty() && _pMdiController != nullptr)
    {
        if(isFileLoaded(fileName))
        {
            GTAEditorWindow* pSubWindow = getLoadedWindow(fileName);
            if(pSubWindow)
            {
                TabIndices windowIndices = pSubWindow->getFramePosition();
                ui->EditorFrame->setCurrentTabIndices(windowIndices);
                ui->EditorFrame->getTabWidget(*windowIndices.viewIndex)->setCurrentIndex(*windowIndices.tabIndex);
            }
        }
        else
        {
            QString gtaDataDir = _pAppController->getGTADataDirectory();
            QString absPath = QDir::cleanPath(QString("%1/%2").arg(gtaDataDir,fileName));
            //TODO: I04DAIVYTE-1205
            //      This is called for directories, in which case always returns false (since directory is not a file)
            //      As a result error log is always shown when double clicking directories, another method should be made for dirs

            const fs::path fsAbsPath(absPath.toStdString());
            bool isDocValid = _pMdiController->isDocumentValid(absPath);
            if(isDocValid) {
                QString sTemp = fileName;
                QStringList sTempList = sTemp.split("/",QString::SkipEmptyParts);
                QString sFileName("");
                if(sTempList.count() > 0) {
                    sFileName = sTempList.last();
                }
                GTAEditorWindow *pSubWindow = createNewSubWindow(sFileName);
                if(pSubWindow) {
                    pSubWindow->setFileDetails(sFileName,fileName);
                    pSubWindow->openDocument(fileName);
                }
            }
            else
            {
                if (fs::is_directory(fsAbsPath)) {
                    return;
                }
                QString errorMessage("File may be corrupted or is conflicted");
                ErrorMessage logMessage;
                logMessage.description = errorMessage;
                logMessage.errorType = ErrorMessage::ErrorType::kError;
                logMessage.fileName = fileName;
                logMessage.source = ErrorMessage::MsgSource::kFile;
                ErrorMessageList msgList;
                msgList.append(logMessage);
                onUpdateErrorLog(msgList);
            }
        }
    }
}

//if isUUID = false then the iUUID contains the relative path
//if isUUID = true then iUUID contains the UUID
void GTAMainWindow::onJumpToElement(const QString &iUUID, const int &iLineNumber,GTAAppController::DisplayContext iDisplayContext,bool isUUID)
{
    QString absolutePath;
    QString dataDirPath = _pAppController->getGTADataDirectory();
    if (isUUID)
    {
        absolutePath = _pAppController->getAbsoluteFilePathFromUUID(iUUID);
    }
    else
    {
        absolutePath = QDir::cleanPath(QString("%1/%2").arg(dataDirPath,iUUID));
    }

    QFile file(absolutePath);
    if(file.exists())
    {

        QString RelativeFilePath = absolutePath;
        RelativeFilePath.replace(dataDirPath,"");
        if(!RelativeFilePath.startsWith("/"))
            RelativeFilePath.prepend("/");
        RelativeFilePath.trimmed();
        GTAEditorWindow *pCurrWindow  = nullptr;
        if(isFileLoaded(RelativeFilePath))
        {
            pCurrWindow = getLoadedWindow(RelativeFilePath);
            if(pCurrWindow)
            {
                pCurrWindow->setFocus();
                pCurrWindow->highlightRow(iLineNumber,iDisplayContext);
            }
        }
        else
        {
            QString sTemp = RelativeFilePath;
            QStringList sTempList = sTemp.split("/",QString::SkipEmptyParts);
            QString sFileName("");
            if(sTempList.count() > 0)
            {
                sFileName = sTempList.last();
            }
            GTAEditorWindow *pSubWindow = createNewSubWindow(sFileName);
            if(pSubWindow)
            {
                pSubWindow->setFileDetails(sFileName,RelativeFilePath);
                pSubWindow->openDocument(RelativeFilePath);
                pSubWindow->setJumpContext(iDisplayContext); // setting the jump flag and the jump context is of prime importance
                pSubWindow->setJumpFlag(true);
            }
            _jumpToLine = iLineNumber;
        }
    }
    else
    {
        QMessageBox::critical(this,"Error",QString("Cannot Open file location %1").arg(absolutePath));
    }
}

void GTAMainWindow::onJumpToElementHeader(const QString& iUUID, const int& iLineNumber, GTAAppController::DisplayContext iDisplayContext, bool isUUID)
{
    QString absolutePath;
    QString dataDirPath = _pAppController->getGTADataDirectory();
    if (isUUID)
    {
        absolutePath = _pAppController->getAbsoluteFilePathFromUUID(iUUID);
    }
    else
    {
        absolutePath = QDir::cleanPath(QString("%1/%2").arg(dataDirPath, iUUID));
    }

    QFile file(absolutePath);
    if (file.exists())
    {
        QString RelativeFilePath = absolutePath;
        RelativeFilePath.replace(dataDirPath, "");
        if (!RelativeFilePath.startsWith("/"))
            RelativeFilePath.prepend("/");
        RelativeFilePath.trimmed();
        GTAEditorWindow* pCurrWindow = nullptr;
        if (isFileLoaded(RelativeFilePath))
        {
            pCurrWindow = getLoadedWindow(RelativeFilePath);
            if (pCurrWindow)
            {
                GTAElement* pElement = pCurrWindow->getElement();
                if (pElement)
                {
                    GTAHeader* pHeader = pElement->getHeader();
                    pCurrWindow->setFocus();
                    onShowHeader();
                    onHighlightHeaderTableRow(iLineNumber);
                }
            }
        }
        else
        {
            QString sTemp = RelativeFilePath;
            QStringList sTempList = sTemp.split("/", QString::SkipEmptyParts);
            QString sFileName("");
            if (sTempList.count() > 0)
            {
                sFileName = sTempList.last();
                openFileByName(sFileName);
            }   
        }
    }
    else
    {
        QMessageBox::critical(this, "Error", QString("Cannot Open file location %1").arg(absolutePath));
    }
}

void GTAMainWindow::onElementCreatedForJump(GTAAppController::DisplayContext iDisplayContext)
{
    GTAEditorWindow *pCurrWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if (pCurrWindow)
    {
        pCurrWindow->highlightRow(_jumpToLine,iDisplayContext);
        _jumpToLine = -1;
    }
}

void GTAMainWindow::onOpenDocumentFromFile()
{
    if (_pAppController)
    {
        QString dataDir = _pAppController->getGTADataDirectory();
        QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), dataDir, "*.pro *.seq *.tmpl *.obj *.fun");
        QFileInfo fileInformation(filePath);
        if (!fileInformation.exists())
            return;
        
        int pos = filePath.indexOf(dataDir);
        filePath.remove(pos, dataDir.length());
        openDocument(filePath);
    }
}

void GTAMainWindow::onSaveAsFileDetailsAdded()
{
    if(_pAppController && _pMdiController)
    {
        if(sender() == _pNewFileDialog)
        {
            bool isFileValid = _pNewFileDialog->isFileValid();
            if(isFileValid)
            {

                QString fileName = _pNewFileDialog->getFileName();
                QString filePath = _pNewFileDialog->getFilePath();
                QString fileType = _pNewFileDialog->getFileType();
                QString authorName = _pNewFileDialog->getAuthorName();
                QString maxTimeEstimated = _pNewFileDialog->getMaxTimeEstimated();
                QString dataDir = _pAppController->getGTADataDirectory();
                QString elementName = QString("%1%2").arg(fileName,fileType);
                elementName.trimmed();

                filePath.replace(dataDir,"");
                filePath.trimmed();
                QString relFilePath = QString("%1/%2").arg(filePath, elementName);
                GTAEditorWindow *pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
                if (pCurrSubWindow)
                {
                    GTAElement *pElement = pCurrSubWindow->getElement();
                    if(pElement)
                    {
                        GTAHeader *pHeader = pElement->getHeader();
                        GTAElement *pNewElement = new GTAElement(*pElement);
                        if(pNewElement)
                        {
                            pNewElement->resetUuid();
                            GTAElement::ElemType type =  _pAppController->getElementTypeFromExtension(fileType);
                            pNewElement->setElementType(type);
                            pNewElement->setAuthorName(authorName);
                            pNewElement->setCreationTime(QDateTime::currentDateTime().toString());
                        }

                        GTAHeader *pNewHeader = nullptr;
                        if (nullptr == pHeader)
                        {
                            pNewHeader = new GTAHeader();
                            pNewHeader->setGTAVersion(pNewElement->getGtaVersion(),true);
                        }
                        else
                        {
                            pNewHeader = new GTAHeader(*pHeader);
                        }

                        _pMdiController->saveDocument(fileName,false,pNewElement,pNewHeader,relFilePath);
                         GTAEditorWindow * pSubWindow = createNewSubWindow(elementName);
                         if(pSubWindow)
                         {
                             pSubWindow->setFileDetails(elementName,relFilePath, fileType, authorName, maxTimeEstimated);
                             pSubWindow->openDocument(relFilePath);
                         }
                        

                    }
                }
				onUpdateTreeItem();
            }
        }
    }
}

void GTAMainWindow::onSaveAll()
{
    QSet<QString> fullPathSet;
    for (uint viewIndex = 0; viewIndex < ui->EditorFrame->getViewsCount(); viewIndex++)
    {
        QList<GTAEditorWindow*> windowsList = ui->EditorFrame->getAllWindows(viewIndex);
        for (auto window : windowsList)
        {
            if (window)
            {
                window->onSave();
                fullPathSet.insert(_pAppController->getGTADataDirectory() + window->getRelativePath());
            }
        }
    }
    updateTree(false, false);
}

void GTAMainWindow::saveAll(QList<GTAEditorWindow*> &lstFiles,int &iStatus)
{
    for(int i = 0; i < lstFiles.count(); i++)
    {
        GTAEditorWindow *pCurrEditorWindow = lstFiles.at(i);
        if(pCurrEditorWindow)
        {
            pCurrEditorWindow->setGlobalSaveStatus(iStatus);
            pCurrEditorWindow->onSave();
        }
    }
}

void GTAMainWindow::onSaveAsItem()
{
    if (_pAppController)
    {

        QModelIndexList selectedIndex = ui->DataTV->selectionModel()->selectedIndexes();
        if (!selectedIndex.isEmpty())
        {
            QModelIndex index = selectedIndex.at(0);
            if (index.isValid())
            {
                QString fileName = _pAppController->getFilePathFromTreeModel(index);
                if (fileName.isEmpty())
                    return;
                else {
                   openDocument(fileName);
                   GTAEditorWindow* pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
                   if (pCurrSubWindow)
                   {
                       GTAElement* pElement = pCurrSubWindow->getElement();
                       QString element = pCurrSubWindow->getElementName();
                       QString elementName = element.mid(0, element.lastIndexOf("."));
                       QString extension = element.replace(elementName, "");
                       extension = extension.trimmed();
                       QString uuid = pCurrSubWindow->getUUID();
                       QString path = _pAppController->getAbsoluteFilePathFromUUID(uuid);
                       QStringList lstSeps = path.split("/");
                       lstSeps.removeLast();
                       path = lstSeps.join("/");

                       if (_pNewFileDialog)
                       {
                           _pNewFileDialog->show();
                           _pNewFileDialog->setDataDirPath(path);
                           _pNewFileDialog->setWindowTitle("Save As");
                           _pNewFileDialog->setFileName(elementName);
                           _pNewFileDialog->setElement(pElement);
                           _pNewFileDialog->setMode(GTANewFileCreationDialog::SAVEAS);
                           _pNewFileDialog->setFileType(extension);
                       }                      
                       
                   }

                }
             
            }
        }

    }
    
}

void GTAMainWindow::onSaveAs()
{
    if(_pAppController)
    {

        GTAEditorWindow *pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
        if (pCurrSubWindow)
        {

            GTAElement *pElement = pCurrSubWindow->getElement();
            QString element = pCurrSubWindow->getElementName();
            QString elementName = element.mid(0,element.lastIndexOf("."));
            QString extension = element.replace(elementName,"");
            extension = extension.trimmed();
            QString uuid = pCurrSubWindow->getUUID();
            QString path = _pAppController->getAbsoluteFilePathFromUUID(uuid);
            QStringList lstSeps = path.split("/");
            lstSeps.removeLast();
            path = lstSeps.join("/");

            if(_pNewFileDialog)
            {
				_pNewFileDialog->show();
                _pNewFileDialog->setDataDirPath(path);
                _pNewFileDialog->setWindowTitle("Save As");
                _pNewFileDialog->setFileName(elementName);
                _pNewFileDialog->setElement(pElement);
                _pNewFileDialog->setMode(GTANewFileCreationDialog::SAVEAS);
                _pNewFileDialog->setFileType(extension);
            }
        }
    }
}

void GTAMainWindow::EnableExport(bool iVal)
{
    ui->actionScxml->setEnabled(iVal);
    if(_pLaunchScxmlAction != nullptr)
    {
        _pLaunchScxmlAction->setEnabled(iVal);
    }

    if(_pLaunchSequencerAction != nullptr)
    {
        _pLaunchSequencerAction->setEnabled(false); //iVal);
    }

    if(_pCheckCompatibilityAction != nullptr)
    {
        _pCheckCompatibilityAction->setEnabled(true);

    }
}

void GTAMainWindow::EnableFileEditorAction(bool iVal)
{
    if(_pOpenDocAction != nullptr)
        _pOpenDocAction->setEnabled(iVal);
    //    if(_pNewFromDocAction != nullptr)
    //        _pNewFromDocAction->setEnabled(iVal);
    if(_pDocUsedByAction != nullptr)
        _pDocUsedByAction->setEnabled(iVal);
    if(_pDeleteDocAction != nullptr)
        _pDeleteDocAction->setEnabled(iVal);
    if(_pExportDocAction != nullptr)
        _pExportDocAction->setEnabled(iVal);
}

void GTAMainWindow::startImportingExternalFiles()
{
    QString importPath = _pImportDialog->getImportPath();
    QString pluginType = _pImportDialog->getPluginType();
    importPath = QDir::cleanPath(importPath);

    if (_pAppController)
    {
        GTAPluginInterface *pPlugin = _pAppController->getPluginInterface();
        QObject *pObj = dynamic_cast<QObject*>(pPlugin);
        if (pPlugin && pObj)
        {
            bool hasWidget = false;
            hasWidget = pPlugin->checkForWidget(pluginType,importPath);
            if (hasWidget)
            {
                //show the widgets from the main thread and then continue importing in parallel threads
                connect(pObj,SIGNAL(widgetClosed(bool)),this,SIGNAL(PluginWidgetClosed(bool)));
                connect(this,SIGNAL(PluginWidgetClosed(bool)),this,SLOT(startImportingExternalFilesinParallel(bool)));
            }
            else
            {
                //if there are no widgets to display in the plugin, directly move to importing of files
                startImportingExternalFilesinParallel(true);
            }
        }
    }
}

void GTAMainWindow::startImportingExternalFilesinParallel(bool )
{
    disconnect(&_FileWatcher, SIGNAL(directoryChanged(QString)),this,SLOT(onDirectoryChanged(QString)));
    disconnect(this,SIGNAL(PluginWidgetClosed(bool)),this,SLOT(startImportingExternalFilesinParallel(bool)));

    QString exportPath = _pImportDialog->getExportPath();
    QString importPath = _pImportDialog->getImportPath();
    QString errorPath = _pImportDialog->getErrorPath();
    QString pluginType = _pImportDialog->getPluginType();
    exportPath = QDir::cleanPath(exportPath);
    importPath = QDir::cleanPath(importPath);
    if(errorPath.isEmpty())
        errorPath = importPath;
    else
        errorPath = QDir::cleanPath(errorPath);


    QDateTime time = QDateTime::currentDateTime();
    QDate date = time.date();
    QString sDate = date.toString(QString("dd.MM.yyyy"));
    QTime logTime = time.time();
    QString sTime = logTime.toString(QString("hh:mm:ss"));
    sTime.replace(":","-");
    QString str = pluginType;
    str.trimmed();
    QString sName = QString("%1Log").arg(str);
    QString logFileName = QString("%1%6%2_%3_%4.%5").arg(errorPath,sName,sDate,sTime,QString("txt"),QDir::separator());

    bool _OverWriteFiles = _pImportDialog->getOverWriteFileStatus();

    showProgressBar(QString("Parsing %1 files .. please Wait").arg(pluginType), this->geometry());
    QFutureWatcher<PluginOutput> * FutureWatcher = new QFutureWatcher<PluginOutput>();
    connect(FutureWatcher,SIGNAL(finished()),this,SLOT(onImportExternalFilesFinished()));
    QFuture<PluginOutput> Future = QtConcurrent::run(_pAppController,&GTAAppController::importExternalFile,pluginType,QDir::cleanPath(exportPath),QDir::cleanPath(importPath),QDir::cleanPath(logFileName),_OverWriteFiles);
    FutureWatcher->setFuture(Future);
}

void GTAMainWindow::onImportExternalFilesFinished()
{
    QStringList newAddedFolders;
    PluginOutput output;
    bool rc;
    QFutureWatcher<PluginOutput> *pWatcher = dynamic_cast<QFutureWatcher<PluginOutput> *>(sender());
    if(pWatcher == nullptr)
    {
           disconnect(_pImportDialog,SIGNAL(startImport()),this,SLOT(startImportingExternalFiles()));
           disconnect(_pImportDialog,SIGNAL(widgetclosed()),this,SLOT(onImportExternalFilesFinished()));
           if (_pImportDialog != nullptr)
           {
               delete _pImportDialog;
               _pImportDialog = nullptr;
           }
           _pAppController->unloadPlugin();
           return;
    }
    else
    {
        output = pWatcher->result();
        rc = output.retVal;
        QStringList errors = output.errors;
        if(!rc)
        {
            hideProgressBar();
            QMessageBox::critical(this,"Error","Error while importing files");
            //unloading the plugin to keep the DLL free for next call
            disconnect(_pImportDialog,SIGNAL(startImport()),this,SLOT(startImportingExternalFiles()));
            disconnect(_pImportDialog,SIGNAL(widgetclosed()),this,SLOT(onImportExternalFilesFinished()));
            if (_pImportDialog != nullptr)
            {
                delete _pImportDialog;
                _pImportDialog = nullptr;
            }
            _pAppController->unloadPlugin();
        }
        else
        {
            _pAppController->setImportedFilesCount(output.importedFilesCount);
        }

        pWatcher->deleteLater();
        if (!errors.isEmpty() && (_ReportWidget != nullptr))
        {
            QString contents = errors.join("\n");
            onUpdateErrorLogMsg(contents);
        }
    }


}

void GTAMainWindow::onPluginFileSavingFinished()
{
    hideProgressBar();

    QString title = QString("Success");
    QString message = QString("Files parsed successfully!!!");

    QMessageBox msgBox(QMessageBox::Information,title,message,QMessageBox::Ok,this);
    msgBox.exec();

    //unloading the plugin to keep the DLL free for next call
    disconnect(_pImportDialog,SIGNAL(startImport()),this,SLOT(startImportingExternalFiles()));
    disconnect(_pImportDialog,SIGNAL(widgetclosed()),this,SLOT(onImportExternalFilesFinished()));
    if (_pImportDialog != nullptr)
    {
        delete _pImportDialog;
        _pImportDialog = nullptr;
    }
    _pAppController->unloadPlugin();
     onRefreshTree();

}

void GTAMainWindow::onImport()
{
    //unloading the plugin dialog to load the DLL
    if (_pImportDialog == nullptr)
    {
        _pImportDialog = new GTAImportDialog(this);
        connect(_pImportDialog,SIGNAL(startImport()),this,SLOT(startImportingExternalFiles()));
        connect(_pImportDialog,SIGNAL(widgetclosed()),this,SLOT(onImportExternalFilesFinished()));
        _pImportDialog->hide();
    }
    if(_pImportDialog != nullptr && _pAppController != nullptr)
    {
        bool rc = _pAppController->loadPlugin();
        if(rc)
        {
            QStringList pluginTypes = _pAppController->getParserTypes();
            _pImportDialog->setPluginTypes(pluginTypes);
            _pImportDialog->show();
            _pImportDialog->unLockWidget();
            QString dataDir = _pAppController->getGTADataDirectory();
            _pImportDialog->setDefaultExportPath(QDir::cleanPath(dataDir));

        }
    }
}

void GTAMainWindow::onMoveEditorRowUp()
{
    GTAEditorWindow *pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if (pCurrSubWindow)
    {
        pCurrSubWindow->onMoveEditorRowUp();
    }
}

void GTAMainWindow::onMoveEditorRowDown()
{
    GTAEditorWindow *pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if (pCurrSubWindow)
    {
        pCurrSubWindow->onMoveEditorRowDown();
    }
}

void GTAMainWindow::searchAndHighlightNext(QRegExp iSearchString,bool ibSearchUp,bool ibTextSearch)
{
    GTAEditorWindow *pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if (pCurrSubWindow)
    {
        pCurrSubWindow->searchAndHighlightNext(iSearchString,ibSearchUp,ibTextSearch);
    }
}

void GTAMainWindow::searchAndHighlightAll(QRegExp iSearchString,bool iTextSearch)
{
    GTAEditorWindow *pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if (pCurrSubWindow)
    {
        QHash<int,QString> oFoundRows;
        pCurrSubWindow->searchAndHighlightAll(iSearchString,iTextSearch,oFoundRows);
    }
}

void GTAMainWindow::searchInAllDocuments(QRegExp iSearchString, bool iTextSearch)
{
    QHash<QString, SearchResults> FoundRows;
    for (uint viewIndex = 0; viewIndex < ui->EditorFrame->getViewsCount(); viewIndex++)
    {
        QList<GTAEditorWindow*> windowsList = ui->EditorFrame->getAllWindows(viewIndex);
        for (auto window : windowsList)
        {
            if (window)
            {
                QHash<int, QString> oFoundRows;
                window->searchAndHighlightAll(iSearchString, iTextSearch, oFoundRows, true, true);
                GTAElement* pElement = window->getElement();
                QString uuid;
                QString relativePath;
                if (pElement)
                    uuid = pElement->getUuid();

                relativePath = window->getRelativePath();
                SearchResults objSearchResults;
                objSearchResults.foundRows = oFoundRows;
                objSearchResults.uuid = uuid;
                FoundRows.insert(relativePath, objSearchResults);
            }
        }
    }

    if (FoundRows.size() > 0)
        updateSearchResults(FoundRows, false);
    else
        QMessageBox::information(this, "Search Results", "No results found");
}

void GTAMainWindow::resetEditorDisplayMode()
{
    GTAEditorWindow *pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if (pCurrSubWindow)
    {
        pCurrSubWindow->resetEditorDisplayMode();
    }
}

void GTAMainWindow::replaceAndHighightNext(QRegExp iSearchString,QString iReplaceString,bool ibSearchUp)
{
    GTAEditorWindow *pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if (pCurrSubWindow)
    {
        pCurrSubWindow->replaceAndHighightNext(iSearchString,iReplaceString,ibSearchUp);
    }
}

void GTAMainWindow::replaceAllText(QRegExp iSearchString,QString iReplaceString)
{
    GTAEditorWindow *pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if (pCurrSubWindow)
    {
        bool rc = pCurrSubWindow->replaceAllText(iSearchString,iReplaceString);
        if (!rc)
        {
            QMessageBox::information(this,"Not found","Search string not found");
        }
    }
}

void GTAMainWindow::replaceInAllDocuments(QRegExp iSearchString, QString iReplaceString)
{
    bool rowsFound = false;
    for (uint viewIndex = 0; viewIndex < ui->EditorFrame->getViewsCount(); viewIndex++)
    {
        QList<GTAEditorWindow*> windowsList = ui->EditorFrame->getAllWindows(viewIndex);
        for (auto window : windowsList)
        {
            if (window)
            {
                bool rc = window->replaceAllText(iSearchString, iReplaceString);
                if(rc)
                {
                    if(!rowsFound)
                        rowsFound = true;
                }
                else
                    rowsFound = false;
            }
        }
    }
    if(!rowsFound)
        QMessageBox::information(this, "Not found","Search string not found");
}

void GTAMainWindow::replaceAllEquipment(QRegExp iSearchString, QString iReplaceString)
{
    GTAEditorWindow* pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if (pCurrSubWindow)
    {
        bool rc = pCurrSubWindow->replaceAllEquipment(iSearchString, iReplaceString);
        if (!rc)
        {
            QMessageBox::information(this, "Not found", "Equipment not found");
        }
    }
}

void GTAMainWindow::searchRequirement(QRegExp iSearchString)
{
    QString dataFolderPath = _pAppController->getGTADataDirectory();
    QHash<QString, SearchResults> FoundRows;
    QDir baseDir(dataFolderPath);
    QDirIterator dirIterator(dataFolderPath, QDir::Files, QDirIterator::Subdirectories);

    while (dirIterator.hasNext()) {
        dirIterator.next();
        QString fullFilePath = dirIterator.filePath();

        // Calculate the relative file path
        QString relFilePath = baseDir.relativeFilePath(fullFilePath);

        QFile file(fullFilePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            continue; // Skip files that can't be opened
        }

        QDomDocument doc;
        if (!doc.setContent(&file)) {
            file.close();
            continue; // Skip if XML is not well-formed
        }
        file.close();

        // Extract UUID from the <MODEL> element
        QDomElement modelElement = doc.documentElement(); // Assuming <MODEL> is the root
        QString modelUuid = modelElement.attribute(XNODE_UUID);

        SearchResults results;
        QDomNodeList actionNodes = doc.elementsByTagName(XNODE_ACTION);
        for (int i = 0; i < actionNodes.size(); ++i) {
            QDomNode node = actionNodes.at(i);
            if (node.isElement()) {
                QDomElement actionElement = node.toElement();
                QString order = actionElement.attribute(XNODE_ORDER);

                // Search within <REQUIREMENTS> child node
                QDomNodeList requirementNodes = actionElement.elementsByTagName(XNODE_REQUIREMENT);
                for (int j = 0; j < requirementNodes.size(); ++j) {
                    QDomElement reqElement = requirementNodes.at(j).toElement();
                    QString val = reqElement.attribute(XNODE_VAL);
                    if (iSearchString.indexIn(val) != -1) {
                        bool ok;
                        int orderNumber = order.toInt(&ok);
                        if (ok) {
                            results.foundRows.insert(orderNumber, val);
                            break; // Match found
                        }
                    }
                }
            }
        }

        QDomNodeList checkNodes = doc.elementsByTagName(XNODE_CHECK);
        for (int i = 0; i < checkNodes.size(); ++i) {
            QDomNode node = checkNodes.at(i);
            if (node.isElement()) {
                QDomElement checkElement = node.toElement();
                QString order = checkElement.attribute(XNODE_ORDER);

                // Search within <REQUIREMENTS> child node
                QDomNodeList requirementNodes = checkElement.elementsByTagName(XNODE_REQUIREMENT);
                for (int j = 0; j < requirementNodes.size(); ++j) {
                    QDomElement reqElement = requirementNodes.at(j).toElement();
                    QString val = reqElement.attribute(XNODE_VAL);
                    if (iSearchString.indexIn(val) != -1) {
                        bool ok;
                        int orderNumber = order.toInt(&ok);
                        if (ok) {
                            results.foundRows.insert(orderNumber, val);
                            break; // Match found
                        }
                    }
                }
            }
        }

        if (!results.foundRows.isEmpty()) {
            results.uuid = modelUuid;
            FoundRows.insert(relFilePath, results);
        }
    }

    if (!FoundRows.isEmpty()) {
        updateSearchResults(FoundRows,false);
    }
    else {
        QMessageBox::information(this, "Search Results", "No results found");
    }
}

void GTAMainWindow::searchTag(QRegExp iSearchString)
{
    QString dataFolderPath = _pAppController->getGTADataDirectory();
    QHash<QString, SearchResults> FoundTags;
    QDir baseDir(dataFolderPath);
    QDirIterator dirIterator(dataFolderPath, QDir::Files, QDirIterator::Subdirectories);

    while (dirIterator.hasNext()) {
        dirIterator.next();
        QString fullFilePath = dirIterator.filePath();

        // Calculate the relative file path
        QString relFilePath = baseDir.relativeFilePath(fullFilePath);

        QFile file(fullFilePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            continue; // Skip files that can't be opened
        }

        QDomDocument doc;
        if (!doc.setContent(&file)) {
            file.close();
            continue; // Skip if XML is not well-formed
        }
        file.close();

        // Extract UUID from the <MODEL> element
        QDomElement modelElement = doc.documentElement(); // Assuming <MODEL> is the root
        QString modelUuid = modelElement.attribute(XNODE_UUID);

        SearchResults results;

        QDomElement headerElement = doc.documentElement().firstChildElement(XNODE_HEADER);
        QDomNodeList itemNodes = headerElement.elementsByTagName(XNODE_ITEM);
        for (int i = 0; i < itemNodes.size(); ++i) {
            QDomElement itemElement = itemNodes.at(i).toElement();
            QString value = itemElement.attribute(XNODE_VALUE);
            if (iSearchString.indexIn(value) != -1) {
                results.foundTags.insert(i, value); 
            }
        }

        if (!results.foundTags.isEmpty()) {
            results.uuid = modelUuid;
            FoundTags.insert(relFilePath, results);
        }
    }

    if (!FoundTags.isEmpty()) {
        updateSearchResults(FoundTags, true);
    }
    else {
        QMessageBox::information(this, "Search Results", "No results found");
    }
}

void GTAMainWindow::searchComment(QRegExp iSearchString)
{
    QString dataFolderPath = _pAppController->getGTADataDirectory();
    QHash<QString, SearchResults> FoundRows;
    QDir baseDir(dataFolderPath);
    QDirIterator dirIterator(dataFolderPath, QDir::Files, QDirIterator::Subdirectories);

    while (dirIterator.hasNext()) {
        dirIterator.next();
        QString fullFilePath = dirIterator.filePath();

        // Calculate the relative file path
        QString relFilePath = baseDir.relativeFilePath(fullFilePath);

        QFile file(fullFilePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            continue; // Skip files that can't be opened
        }

        QDomDocument doc;
        if (!doc.setContent(&file)) {
            file.close();
            continue; // Skip if XML is not well-formed
        }
        file.close();

        // Extract UUID from the <MODEL> element
        QDomElement modelElement = doc.documentElement(); // Assuming <MODEL> is the root
        QString modelUuid = modelElement.attribute(XNODE_UUID);

        SearchResults results;
        QDomNodeList actionNodes = doc.elementsByTagName(XNODE_ACTION);
        for (int i = 0; i < actionNodes.size(); ++i) {
            QDomNode node = actionNodes.at(i);
            if (node.isElement()) {
                QDomElement actionElement = node.toElement();
                QString order = actionElement.attribute(XNODE_ORDER);

                // Search within <REQUIREMENTS> child node
                QDomNodeList commentNodes = actionElement.elementsByTagName(XNODE_COMMENT);
                for (int j = 0; j < commentNodes.size(); ++j) {
                    QDomElement commentElement = commentNodes.at(j).toElement();
                    QString commentValue = commentElement.text();
                    if (iSearchString.indexIn(commentValue) != -1) {
                        bool ok;
                        int orderNumber = order.toInt(&ok);
                        if (ok) {
                            results.foundRows.insert(orderNumber, commentValue);
                            break; // Match found
                        }
                    }
                }
            }
        }

        QDomNodeList checkNodes = doc.elementsByTagName(XNODE_CHECK);
        for (int i = 0; i < checkNodes.size(); ++i) {
            QDomNode node = checkNodes.at(i);
            if (node.isElement()) {
                QDomElement checkElement = node.toElement();
                QString order = checkElement.attribute(XNODE_ORDER);

                // Search within <REQUIREMENTS> child node
                QDomNodeList commentNodes = checkElement.elementsByTagName(XNODE_COMMENT);
                for (int j = 0; j < commentNodes.size(); ++j) {
                    QDomElement commentElement = commentNodes.at(j).toElement();
                    QString commentValue = commentElement.text();
                    if (iSearchString.indexIn(commentValue) != -1) {
                        bool ok;
                        int orderNumber = order.toInt(&ok);
                        if (ok) {
                            results.foundRows.insert(orderNumber, commentValue);
                            break; // Match found
                        }
                    }
                }
            }
        }

        if (!results.foundRows.isEmpty()) {
            results.uuid = modelUuid;
            FoundRows.insert(relFilePath, results);
        }
    }

    if (!FoundRows.isEmpty()) {
        updateSearchResults(FoundRows, false);
    }
    else {
        QMessageBox::information(this, "Search Results", "No results found");
    }
}

void GTAMainWindow::onCollapseAllToTitle()
{
    GTAEditorWindow *pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if (pCurrSubWindow)
    {
        pCurrSubWindow->onCollapseAllToTitle(_titleCollapseToggled);
    }
}

void GTAMainWindow::onRemoveEmptyRows()
{
    GTAEditorWindow* pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if (pCurrSubWindow)
    {
        pCurrSubWindow->removeEmptyRows();
    }
}

void GTAMainWindow::toggleTitleCollapseActionIcon(bool iset)
{
    _titleCollapseToggled = iset;
    QIcon iconExpandCollapse;
    QString sToolTip;
    if (!iset)
    {
        iconExpandCollapse.addFile(":/images/forms/img/Editing_Collapse_icon.png");
        sToolTip="Collapse to title";
    }
    else
    {
        iconExpandCollapse.addFile(":/images/forms/img/Editing_Expand_icon.png");
        sToolTip = "Expand all\n Alt++";
    }
    _pCollapseToTitleAction->setIcon(iconExpandCollapse);
    _pCollapseToTitleAction->setToolTip(sToolTip);
}

void GTAMainWindow::showTreeProgressBar()
{
    if(_TreeProgressBar != nullptr)
    {
        _TreeProgressBar->setPosition(this->geometry());
        _TreeProgressBar->setWindowModality(Qt::ApplicationModal);
        _TreeProgressBar->setLabelText("Updating File System View");
        _TreeProgressBar->setVisible(true);
    }
}

void GTAMainWindow::showProgressBar(const QString& iMsg, const QRect& pos)
{
    if(_pProgressBar != nullptr)
    {
        _pProgressBar->setVisible(true);
        _pProgressBar->setPosition(pos);
        _pProgressBar->setWindowModality(Qt::ApplicationModal);
        _pProgressBar->setLabelText(iMsg);
    }
}

void GTAMainWindow::hideTreeProgressBar()
{
    if(_TreeProgressBar != nullptr)
    {
        _TreeProgressBar->setVisible(false);
    }
}

void GTAMainWindow::hideProgressBar()
{
    if(_pProgressBar != nullptr)
    {
        _pProgressBar->setVisible(false);
    }
}

/**
 * This fuction enables/disables launch tool button actions
 * @toggleStatus : true - enables the buttons
 *                 false - disables the buttons
*/
void GTAMainWindow::onToggleLaunchButton(bool toggleStatus)
{
    _pLaunchScxmlAction->setEnabled(toggleStatus);
    _pLaunchSequencerAction->setEnabled(false); //toggleStatus);
    _pTitleBasedExecution->setEnabled(false); //toggleStatus);

}

/**
 * This fuction enables/disables menu bar and tool bar actions as well as editor context menu's options
 * @toggleStatus : true - enables the buttons
 *                 false - disables the buttons
*/
void GTAMainWindow::disableEditingActions(bool bDisable)
{
    ui->EditorToolBar->setDisabled(bDisable);
    QList<QAction*> lstOfActions = ui->EditorToolBar->actions();
    foreach(QAction* currAction, lstOfActions)
    {
        currAction->setDisabled(bDisable);
    }

    GTAEditorWindow *pCurrWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if (pCurrWindow)
    {
        pCurrWindow->disableEditingActions(bDisable);
    }
}

void GTAMainWindow::openUserGuide()
{
    QString userGuidePath = QString("file:///%1%2%3").arg(QApplication::applicationDirPath(), QDir::separator(),"Generic Test Automation Tool User Guide.pdf");
    QDesktopServices::openUrl(QUrl(userGuidePath));
}

/**
 * @brief This function call a script to find and write versions of tools bench in the popup software.
 * 
 */
void GTAMainWindow::onOpenVersionTools() const
{
    if (_pAppController->launchScriptBenchVersionsFile()){
        QString full_infos = "";
        _pAppController->parserBenchVersionsFile(full_infos);
        QObject* pObj = sender();
        QWidget* pParent = dynamic_cast<QWidget*>(pObj->parent());
        QMessageBox::about(pParent, "Version Tools", full_infos);
    }
}

void GTAMainWindow::onExportDocsClicked()
{

    QModelIndexList selectedIndex = ui->DataTV->selectionModel()->selectedIndexes();
    if (!selectedIndex.isEmpty())
    {
        QModelIndex index = selectedIndex.at(0);
        if (index.isValid())
        {
            QString fileName = _pAppController->getFilePathFromTreeModel(index);
            if(fileName.isEmpty())
                return;

            QStringList lstDocs;
            QStringList lstOfErrors;
            if (_pAppController)
                _pAppController->getDocumentsUsed(fileName,lstDocs,lstOfErrors);

            lstDocs.prepend(fileName);
            if (fileName.contains(".pro") || fileName.contains(".seq"))
                _pExportDoctWidget->scxmlExportDisable(false);
            else
                _pExportDoctWidget->scxmlExportDisable(true);

            _pExportDoctWidget->setErrorInformation(lstOfErrors.join("\n").trimmed());
            _pExportDoctWidget->setLstOfFilesToExport(lstDocs);
            _pExportDoctWidget->setExportFileName(fileName);
            int typeSepIndex = fileName.lastIndexOf("/"); 
            _pExportDoctWidget->setSaveFolder(fileName.mid(0,typeSepIndex));

            _pExportDoctWidget->setDefaultLogFolderPath(_pAppController->getSystemService()->getLogDirectory());
            _pExportDoctWidget->show();
            _pExportDoctWidget->setPath(_pAppController->getExportDocDirectory());

        }
    }
}

void GTAMainWindow::onExportStarted(const QString& iSaveFolder,const QString& iFolderName,const QStringList& ilstOfDocuments, bool ibIncludeBDB, bool ibIncludeSCXML,bool ibCompress, const QString &iFileName,const QString &iLogFile)
{
    exportDocumentInput input;
    input.SaveFolder = iSaveFolder;
    input.FolderName = iFolderName;
    input.lstOfDocuments = ilstOfDocuments;
    input.bIncludeBDB = ibIncludeBDB;
    input.bIncludeSCXML = ibIncludeSCXML;
    input.bCompress = ibCompress;
    input.FileName = iFileName;
    input.LogFile = iLogFile;


}

void GTAMainWindow::onExportDocumentsFinished()
{

    QFutureWatcher<QStringList> *pWatcher = dynamic_cast<QFutureWatcher<QStringList> *>(sender());
    if(pWatcher == nullptr)
        return;
    else
    {
        hideProgressBar();

        QStringList lstCopyError = pWatcher->result();
        if(lstCopyError.isEmpty())
        {
            QMessageBox::information(this,"Export successful!!!",QString("Export of document finished"));
        }
        else
        {

            _ReportWidget->setText( QString("Failed to copy following files:\n%1").arg(lstCopyError.join("\n")));
            _ReportWidget->show();
        }
    }
    pWatcher->deleteLater();

}

QStringList GTAMainWindow::exportDocuments(exportDocumentInput iInput,const QString &iDataFolder, const QString& iFolder, const QString &iElemName)
{
    QStringList lstCopyError;
    lstCopyError.clear();

    QString withoutExtension = iElemName;
    QString sFolder = QDir::cleanPath(QString("%1%2%3").arg(iFolder, QDir::separator(), iElemName));
    QString sDataFolder = QDir::cleanPath(QString("%1%2%3").arg(sFolder, QDir::separator(), DATA_DIR));

    QDir dir(sFolder);
    if(!dir.mkpath(sDataFolder))
    {
        sDataFolder = sFolder;
    }

    QStringList ilstOfDocuments = iInput.lstOfDocuments;
    foreach(QString sFile, ilstOfDocuments)
    {
        QString fileToCopy = QDir::cleanPath(_pAppController->getGTADataDirectory() + QDir::separator() + sFile);
        QStringList separatedPath = sFile.split("/",QString::SkipEmptyParts);
        QString sFileName = separatedPath.last();
  
        QString fileNewName = QDir::cleanPath(sDataFolder + QDir::separator() + sFileName);
        QFile sElemFile(fileToCopy);
        QFileInfo fileNewNameInfo(fileNewName);

        if (fileNewNameInfo.exists())
            sElemFile.remove(fileNewName);

        if(!sElemFile.copy(fileToCopy,fileNewName))
            lstCopyError.append(sFile);

    }

    QString iLogFile = iInput.LogFile;
    if(!iLogFile.isEmpty())
    {
        QString sLogFileName =  iLogFile.split("/",QString::SkipEmptyParts).last();

        QFileInfo loginfo(iLogFile);
        if(loginfo.exists())
        {
            QString sLogFolder = QDir::cleanPath(QString("%1%2%3").arg(sFolder,QDir::separator(),"LOG"));
            if(!dir.mkpath(sLogFolder))
            {
                sLogFolder = sFolder;
            }

            QString newfileName =QDir::cleanPath(QString("%1%2%3").arg(sLogFolder,QDir::separator(),sLogFileName));
            QFile newFile(newfileName);
            if(newFile.exists())
            {
                dir.remove(newfileName);
            }
            QFile logFile(iLogFile);
            if(!logFile.copy(iLogFile,newfileName))
                lstCopyError.append(iLogFile);
        }
    }

    bool ibIncludeSCXML = iInput.bIncludeSCXML;
    if (ibIncludeSCXML)
    {
        QString scxmlDir = QDir::cleanPath(_pAppController->getScxmlDirectory());

        QString scxmlFile = QDir::cleanPath(_pAppController->getScxmlDirectory() + QDir::separator() + withoutExtension + QString(".scxml"));

        QString scxmlFolder = QDir::cleanPath(QString("%1%2%3").arg(sFolder,QDir::separator(),"SCXML"));

        if(!dir.mkpath(scxmlFolder))
        {
            scxmlFolder = sFolder;
        }


        QModelIndexList selectedIndex = ui->DataTV->selectionModel()->selectedIndexes();
        if (!selectedIndex.isEmpty())
        {
            QModelIndex index = selectedIndex.at(0);
            if (index.isValid())
            {
                QString fileName = _pAppController->getFilePathFromTreeModel(index);
                if(!fileName.isEmpty())
                    _pAppController->exportDocToScxml(fileName,scxmlFolder,true,true);
            }
        }
    }

    bool ibIncludeBDB = iInput.bIncludeBDB;
    if (ibIncludeBDB)
    {
        QString benchDBFolder = QDir::cleanPath(_pAppController->getGTALibraryPath());
        QFileInfo dbFolderInfo(benchDBFolder);
        QString BDBDirectory = QDir::cleanPath(QString("%1%2%3").arg(sFolder,QDir::separator(),"BDB"));
        //bool rc = false;

        QList <GTAFILEInfo> lstFileInfo;

        for (int k=0;k<iInput.lstOfDocuments.count();k++)
        {
            QList <GTAFILEInfo> lstFiles;
            _pAppController->getFileInfoListUsed(iInput.lstOfDocuments.at(k),lstFiles);
            lstFileInfo.append(lstFiles);
        }

        //_pAppController->getFileInfoListUsed(iInput.FileName,lstFileInfo);

        if(!dir.mkpath(BDBDirectory))
        {
            BDBDirectory = sFolder;
        }

        foreach(GTAFILEInfo copyFile, lstFileInfo)
        {
            QString fileToCopy = copyFile._filePath;
            QString fileNewName = QDir::cleanPath(BDBDirectory + QDir::separator() + copyFile._filename);

            //(story #358311)
            if(copyFile._fileType == TABLES_DIR )
            {
                QFileInfo fileInfo(copyFile._filePath);
                if(!fileInfo.exists())
                {
                    //creating filetocopy string with local repository path, if the file mentioned in procedures doesn't exist
                    QString repoPath = QString::fromStdString(TestConfig::getInstance()->getRepositoryPath());
                    QString tablesDir = QDir::cleanPath(QString("%1%2").arg(QDir::separator(), TABLES_DIR));
                    fileToCopy = QDir::cleanPath((repoPath + tablesDir + copyFile._filePath.split(tablesDir).last()));
                }
                //Creating a seperate TABLES Folder for table files in export folder while exporting.
                QString sTablesFolder = QDir::cleanPath(QString("%1%2%3").arg(sFolder,QDir::separator(),TABLES_DIR));
                if(!dir.mkpath(sTablesFolder))
                {
                    sTablesFolder = sFolder;
                }
                fileNewName = QDir::cleanPath(QString("%1%2%3").arg(sTablesFolder,QDir::separator(),copyFile._filename));
            }

            QFile sElemFile(fileToCopy);
            QFileInfo fileNewNameInfo(fileNewName);
            if (fileNewNameInfo.exists())
                dir.remove(fileNewName);
            if(!sElemFile.copy(fileToCopy,fileNewName))
                lstCopyError.append(copyFile._filename);
        }

    }
    bool ibCompress = iInput.bCompress;
    if(ibCompress)
    {
        QString zipfile = sFolder+".zip";
        QFileInfo zipInfo(sFolder+".zip");
        //deleting previous file
        if(zipInfo.exists())
        {
            QFile oldfile(zipfile);
            oldfile.remove();
        }
        //todo: for zip
        GTAUtil::CompressDirectory(zipfile,sFolder);

        //QFileInfo zipInfo(sFolder+".zip");
        if(zipInfo.exists())
        {
            QDir dir(sFolder);
            QDirIterator it(sFolder, QStringList() << "*.*", QDir::Files, QDirIterator::Subdirectories);
            while (it.hasNext())
            {
                QString fileName = it.next();
                QFile rmFile(fileName);
                rmFile.remove(fileName);
            }
            QDirIterator it1(sFolder, QStringList() << "*.*", QDir::Dirs, QDirIterator::Subdirectories);
            while (it1.hasNext())
            {
                QString dirName = it1.next();
                QDir rmDir(dirName);
                rmDir.rmdir(dirName);
            }
        }
    }
    return lstCopyError;

}

void GTAMainWindow::exportDocuments(const QString& iSaveFolder,const QString& iFolderName,const QStringList& ilstOfDocuments, bool ibIncludeBDB, bool ibIncludeSCXML,bool ibCompress, const QString &iFileName,const QString &iLogFile)
{

    if (!ilstOfDocuments.isEmpty())
    {
        QString sepFileName;


        QString withoutExtension;
        QStringList sepFileNameLst = iFileName.split("/",QString::SkipEmptyParts);
        if(!sepFileNameLst.isEmpty())
        {
            sepFileName = sepFileNameLst.last();
        }
        else
        {
            sepFileName = iFileName;
        }
        QStringList tempList = sepFileName.split(".");
        withoutExtension = tempList.first();

        QString sFolder = QDir::cleanPath (QString("%1%2%3").arg(iSaveFolder,QDir::separator(),iFolderName));
        QFileInfo folderInfo(sFolder);
        QString sFolderZip = sFolder+".zip";
        QFileInfo folderInfoZip(sFolderZip);
        bool ZipExists = false;
        if (folderInfoZip.exists() && folderInfoZip.isFile())
        {
            ZipExists = true;
        }
        if (!folderInfo.exists() && !ZipExists)
        {
            QDir dirForSave(sFolder);
            if (!dirForSave.mkpath(sFolder))
            {
                QMessageBox::critical(this,"Folder creation Failed",QString("Could not create folder \"%1\"").arg(sFolder));
                return;
            }
        }
        else
        {
            QMessageBox msgBox (this);
            if (ZipExists && ibCompress)
                msgBox.setText("The file already exists. Contents will be overwritten, continue?");
            else
                msgBox.setText("The folder already exists. Contents will be overwritten, continue?");


            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setWindowTitle("Folder overwrite warning");
            msgBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::Yes);

            int ret = msgBox.exec();
            if(ret!=QMessageBox::Yes)
            {
                return;
            }
        }

        QString sDataFolder = GTAAppController::getGTAAppController()->getGTADataDirectory();

        exportDocumentInput input;
        input.SaveFolder = iSaveFolder;
        input.FolderName = iFolderName;
        input.lstOfDocuments = ilstOfDocuments;
        input.bIncludeBDB = ibIncludeBDB;
        input.bIncludeSCXML = ibIncludeSCXML;
        input.bCompress = ibCompress;
        input.FileName = iFileName;
        input.LogFile = iLogFile;

        QFutureWatcher<QStringList> * exportWatcher = new QFutureWatcher<QStringList>();
        connect(exportWatcher,SIGNAL(finished()),this,SLOT(onExportDocumentsFinished()));
        QFuture<QStringList> exportFuture = QtConcurrent::run(this,&GTAMainWindow::exportDocuments,input,sDataFolder,sFolder,withoutExtension);
        exportWatcher->setFuture(exportFuture);
        
        
        _pExportDoctWidget->hide();

        showProgressBar("Exporting documents", this->geometry());


    }
    else
        QMessageBox::critical(this,"Empty document list",QString("No documents to copy"));
}

void GTAMainWindow::onAddToFavorites()

{

    QModelIndexList selectedIndex = ui->DataTV->selectionModel()->selectedIndexes();
    if (!selectedIndex.isEmpty())
    {
        foreach(QModelIndex index,selectedIndex)
        {

            if (index.isValid() && (index.column() == 0 || index.column() == 1))
            {
                QString fileName = _pAppController->getFilePathFromTreeModel(index);

                if(fileName.isEmpty())
                    return;

                _pAppController->addToFavorites(fileName,index);

            }
        }
    }
}

void GTAMainWindow::onRemoveFromFavorites()
{
    QModelIndexList selectedIndex = ui->DataTV->selectionModel()->selectedIndexes();
    if (!selectedIndex.isEmpty())
    {
        QModelIndex index = selectedIndex.at(0);
        if (index.isValid() && (index.column() == 0 || index.column() == 1))
        {
            QString fileName = _pAppController->getFilePathFromTreeModel(index);
            if(fileName.isEmpty())
                return;
            _pAppController->removeFromFavorites(index,fileName);
			QItemSelectionModel *model = ui->DataTV->selectionModel();
			ui->DataTV->setModel(nullptr);
			if (nullptr != model)
			{
				delete model;
				model = nullptr;
			}
            ui->DataTV->setModel(_pAppController->getFavoriteBrowserDataModel(true));
        }
    }
}

void GTAMainWindow::populateDataDVContextMenu(QModelIndex )
{

    if (ui->DataTypeCB->currentText() == "Favorites")
    {

        _pDeleteDocAction->setVisible(false);
        _pAddToFavOnDV->setVisible(false);
        _pRemoveFromFav->setVisible(true);

    }
    else
    {
        _pDeleteDocAction->setVisible(true);
        _pAddToFavOnDV->setVisible(true);
        _pRemoveFromFav->setVisible(false);
    }

    _pCreateCallSequence->setEnabled(true);

    _pOpenDocAction->setVisible(true);
    _pDocUsedByAction->setVisible(true);
    _pExportDocAction->setVisible(true);
    _pShowDetailOnDV->setVisible(true);
    _pShowDependencies->setVisible(true);
    _pRename->setVisible(true);
	_pMoveFiles->setVisible(true);
    _pUpdateValidationStatus->setVisible(true);
    _pExportSCXMLs->setVisible(true);
    _pSaveAsAction->setVisible(true);  // TM

}

void GTAMainWindow::onCallActionDropped(int& iRow, QString& fileName)
{
    if (ui->DataTV->selectionModel() != nullptr)
    {
        QModelIndex indexToLoad = ui->DataTV->selectionModel()->currentIndex();
        if (indexToLoad.column() == 0)
        {
            QString dataDirectory = _pAppController->getGTADataDirectory();
            QString FileName_UUID = _pAppController->getFilePathFromTreeModel(indexToLoad, true);
            QString FileName = FileName_UUID.split("::").first();
            QString FileRefUUID = FileName_UUID.split("::").last();
            QFileInfo info(QDir::cleanPath(QString("%1%2%3").arg(dataDirectory, QDir::separator(), FileName)));
            if (info.exists())
            {
                GTAEditorWindow* pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
                if (pCurrSubWindow)
                {
                    pCurrSubWindow->CallActionDropped(iRow, FileName, FileRefUUID);
                }
            }
            else
                QMessageBox::warning(this, "Invalid drop item", QString("Dropped item (%1) not found in data directory").arg(fileName));

        }
    }
}

QStringList GTAMainWindow::getSVNCredential()
{
    QString userName,password,saveAuthState;
    QStringList svnUsrCred;
    GTASVNAuthenticationWidget usrCredObj(nullptr);
    usrCredObj.setWindowTitle("SVN Authentication");
    usrCredObj.exec();

    if(usrCredObj.getSaveAuthenticationState())
        saveAuthState = "true";
    else
        saveAuthState = "false";

    if(!usrCredObj.getUserName().isEmpty() && !usrCredObj.getPassword().isEmpty())
    {
        userName = usrCredObj.getUserName();
        password = usrCredObj.getPassword();
        svnUsrCred << userName<<password<<saveAuthState;
    }
    else
        svnUsrCred.empty();
    return svnUsrCred;

}

QStringList GTAMainWindow::getSVNSelectedFiles(QString svnCommand, QString &isUpdateDir, QString &commitMessage, QString &errMsg)
{
    if(_pAppController)
    {
        isUpdateDir = "false";
        QString widgetTitle;
        QHash<QString,QString> oFileStat;
        QStringList headerItems,selectedFiles;
        QString dataDir = _pAppController->getGTADataDirectory();
        bool rc;
        if (svnCommand == "Add All Elements")
            rc = _pAppController->getAllModifiedUnversionFiles(dataDir, oFileStat, false, _usrCredentials, errMsg, true);
        else
            rc = _pAppController->getAllModifiedUnversionFiles(dataDir, oFileStat, false, _usrCredentials, errMsg);

        if(rc == true && errMsg.isEmpty())
        {
            if(oFileStat.isEmpty())
            {
                if(svnCommand == "Update All Elements")
                {
                    QMessageBox::information(this,"SVN Update All Elements","No Modifications Found.");
                }
                else if(svnCommand == "Commit All Elements")
                {
                    QMessageBox::information(this,"SVN Commit All","No Modifications Found.");
                }
                else if (svnCommand == "Add All Elements")
                {
                    QMessageBox::information(this, "SVN Add All", "No Modifications Found.");
                }
                return QStringList();
            }

            widgetTitle = (svnCommand == "Commit All Elements") ? "Select Files to Commit" : (svnCommand == "Add All Elements") ? "Select Files to Add" : "Select Files to Update";
            GTASVNListAllWidget *pDisplay = new GTASVNListAllWidget(this);
            pDisplay->setTitle(widgetTitle);
            pDisplay->clear();
            headerItems<<"File Name"<<"SVN Status";
            pDisplay->displayList(true,oFileStat);
            pDisplay->setColumnHeaders(headerItems);

            if(svnCommand == "Update All Elements")
                pDisplay->showUpdateDirCB(true);

            if(svnCommand == "Commit All Elements")
                pDisplay->showCommitMessage(true);


            pDisplay->exec();
            if(pDisplay->getStatus() == -1) return QStringList();
            if(svnCommand == "Update All Elements")
            {
                bool isUpdateDirCheck = pDisplay->isUpdateDirchecked();
                if(isUpdateDirCheck)
                    isUpdateDir = "true";
            }
            selectedFiles = pDisplay->getSelectedList();

            if(svnCommand == "Commit All Elements")
                commitMessage = pDisplay->getCommitMessage();

            pDisplay->close();
            delete pDisplay;
        }
        else if(!errMsg.isEmpty())
        {
            return QStringList();
        }
        else
        {
            onSvnActionFinished();
        }

        return selectedFiles;
    }
	return QStringList();
}

void GTAMainWindow::prepareSvnAddAllCommand(const QString& svnActionCommand, SVNInputParam& svnParams)
{
    if (_pAppController)
    {
        QStringList selectedFiles;
        QString isUpdateDir;
        QString commitMessage, svnErrMsg;

        selectedFiles = getSVNSelectedFiles(svnActionCommand, isUpdateDir, commitMessage, svnErrMsg);

        if (!selectedFiles.isEmpty() && svnErrMsg.isEmpty())
        {
            QString infoMsg = "You are going to add All/multiple elements of GTA,This might take some time. Do you really want to proceed?";
            QMessageBox::StandardButton reply = QMessageBox::question(this, "SVN Add All", infoMsg, QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::No)
                return;
        }
        else if (!svnErrMsg.isEmpty())
        {
            QMessageBox::critical(this, svnActionCommand, svnErrMsg);
            svnErrMsg.clear();
            return;
        }
        else
        {
            QMessageBox::information(this, "Info", "Terminating SVN Adding...Since no files were selected");
            return;
        }
        svnParams.command = svnActionCommand;
        svnParams.cmdIndex = QModelIndex();
        svnParams.cmdIndexList = QModelIndexList();
        svnParams.userCredential = _usrCredentials;
        svnParams.fileList = selectedFiles;

        selectedFiles.clear();
        isUpdateDir.clear();
        svnErrMsg.clear();
    }
}

/**
 * This function checks if all files in folder are selected by user in tree view.
 * @files : the list of selected files.
 * @filesInside : the list of all files in directory
 * @return : bool true if all files in folder are selected
*/
bool GTAMainWindow::checkIfAllfilesInsideFolderAreSelected(const QSet<QString>& filesSelected, const QStringList& filesInsideFolder, QStringList &uncheckedFiles)
{
    uncheckedFiles = QStringList(filesInsideFolder);
    bool areAllSelected = true, isSelected;
    for (const QString& fi : filesInsideFolder) {
        isSelected = false;
        for (const QString& f : filesSelected) {
            QString fsel = f.split("/", QString::SkipEmptyParts).last();
            if (fi == fsel) {
                isSelected = true;
                uncheckedFiles.removeOne(fi);
                break;
            }
        }
        areAllSelected = areAllSelected && isSelected;
    }
    return areAllSelected;
};

/**
 * This function makes list of files selected by the user in the tree view.
 * @selectedIdx : the list of selected files indexes.
 * @return : void
*/
void GTAMainWindow::makeListOfSelectedFiles(const QModelIndexList& selectedIdx, const QString& svnCommand)
{
    QSet<QString> files;
    for (const QModelIndex& idx : selectedIdx)
        files.insert(_pAppController->getFilePathFromTreeModel(idx));

    for (QString file : files)
    {
        QString fullDirectory = QString("%1%2").arg(_pAppController->getGTADataDirectory(), file);
        QStringList parts = file.split("/", QString::SkipEmptyParts);
        QString trimmedFile = parts.join("/");

        QDir dir = QDir(fullDirectory);
        QStringList filesInside = dir.exists() ? dir.entryList() : QStringList();
        filesInside.removeOne(".");
        filesInside.removeOne("..");

        if (!filesInside.isEmpty() && svnCommand == "Delete")
        {
            QString folderName = file.split("/", QString::SkipEmptyParts).join("/");
            QStringList uncheckedFiles;
            if (checkIfAllfilesInsideFolderAreSelected(files, filesInside, uncheckedFiles))
                _pSvnParamsTempContainer->fileList.append(trimmedFile);
            else
            {
                QMessageBox::StandardButton reply = QMessageBox::warning(
                    this, "SVN Delete",
                    "One or more selected items are folders which are not empty.\nAll their content will be removed.\nDo you want to continue?",
                    QMessageBox::Yes | QMessageBox::No
                );
                if (reply == QMessageBox::Yes)
                {
                    for (const QString& unchecked : uncheckedFiles)
                        _pSvnParamsTempContainer->fileList.append(QString("%1/%2").arg(folderName, unchecked));
                    _pSvnParamsTempContainer->fileList.append(trimmedFile);
                }
                else
                    return;
            }
        }
        else
            _pSvnParamsTempContainer->fileList.append(trimmedFile);
    }
    qSort(
        _pSvnParamsTempContainer->fileList.begin(),
        _pSvnParamsTempContainer->fileList.end(),
        [](const QString& s1, const QString& s2)->bool { return s1.size() > s2.size(); }
    );
}

void GTAMainWindow::onSVNAction()
{
    if (_pSvnParamsTempContainer != nullptr)
    {
        delete _pSvnParamsTempContainer;
        _pSvnParamsTempContainer = nullptr;
    }
    _pSvnParamsTempContainer = new SVNInputParam();

    QString svnActionCommand;
    QString resolveOption;
    QModelIndex index = QModelIndex();
    bool ignoreIndex = false;


    QObject *pSender = sender();
    QAction *pAction = dynamic_cast<QAction*>(pSender);
    if(pAction != nullptr)
    {
        _svnActionCommand = pAction->text();
        _pSvnParamsTempContainer->command = _svnActionCommand;
        svnActionCommand = _svnActionCommand;
    }

    if(svnActionCommand ==  "Update All Elements" || 
        svnActionCommand == "Commit All Elements" || 
        svnActionCommand == "Add All Elements")
        ignoreIndex = true;

    if(_svnActionCommand == "Clear SVN Authentication cache")
    {
        _pSvnParamsTempContainer->cmdIndex = QModelIndex();
        _pSvnParamsTempContainer->userCredential = QStringList();
        _usrCredentials.clear();
        showProgressBar("Performing SVN Authentication cache cleaning .. please wait", this->geometry());
        QFutureWatcher<QString > * FutureWatcher = new QFutureWatcher<QString>();
        connect(FutureWatcher,SIGNAL(finished()),this,SLOT(onSvnActionFinished()));
        QFuture<QString> Future = QtConcurrent::run(_pAppController, &GTAAppController::executeSVNAction,*_pSvnParamsTempContainer);
        FutureWatcher->setFuture(Future);

    }
    else
    {
        if(_usrCredentials.isEmpty())
            _usrCredentials = getSVNCredential();

        if(!_usrCredentials.empty())
        {
            QModelIndexList selectedIndex = ui->DataTV->selectionModel()->selectedIndexes();
            if(!selectedIndex.isEmpty() || ignoreIndex == true)
            {
                _pSvnParamsTempContainer->cmdIndexList = selectedIndex; //here new field created
                makeListOfSelectedFiles(selectedIndex, svnActionCommand);
                if(ignoreIndex == false)
                {
                    index = selectedIndex.at(0);
                }

                if(svnActionCommand == "Commit" && ignoreIndex == false)
                {
                    QDialog *commitMsgDialog = new QDialog();
                    commitMsgDialog->setWindowTitle("SVN Commit Message");
                    commitMsgDialog->setAttribute(Qt::WA_DeleteOnClose);
                    commitMsgDialog->setWindowIcon(QIcon(QPixmap(":/images/forms/img/GTAAppIcon_black.png")));
                    commitMsgDialog->setWindowFlags(((commitMsgDialog->windowFlags()|Qt::CustomizeWindowHint) & ~Qt::WindowCloseButtonHint));
                    commitMsgDialog->deleteLater();
                    commitMsgDialog->accept();

                    QGridLayout *gridLayout = new QGridLayout(commitMsgDialog);

                    QLabel *messageLabel = new QLabel(commitMsgDialog);
                    messageLabel->setText("Commit Message");

                    QTextEdit *commitMessageTE = new QTextEdit(commitMsgDialog);

                    QPushButton *commitMsgPB = new QPushButton(commitMsgDialog);
                    commitMsgPB->setText("Commit");
                    commitMsgPB->setFocus();
                    commitMsgPB->setAutoDefault(true);

                    connect(commitMsgPB,SIGNAL(clicked()),commitMsgDialog,SLOT(hide()));

                    gridLayout->addWidget(messageLabel,0,0,1,2);
                    gridLayout->addWidget(commitMessageTE,0,2,1,2);
                    gridLayout->addWidget(commitMsgPB, 4,2,1,2);

                    commitMsgDialog->exec();
                    _pSvnParamsTempContainer->commitMessage = commitMessageTE->toPlainText();
                    commitMsgDialog->close();

                }

                if((index.isValid() && !svnActionCommand.isEmpty()) || ignoreIndex == true)
                {
                    QString isUpdateDir;
                    QString commitMessage, svnErrMsg;
                    _pSvnParamsTempContainer->cmdIndex = index;
                    _pSvnParamsTempContainer->userCredential = _usrCredentials;
                    if(svnActionCommand == "Resolve Conflict")
                    {
                        GTASVNResolveOptionDialog* pOptionDlg = new GTASVNResolveOptionDialog(nullptr);
                        pOptionDlg->exec();
                        if(pOptionDlg->startResolving())
                            _pSvnParamsTempContainer->resolveOption = pOptionDlg->getUserSelection();

                        pOptionDlg->hide();
                        pOptionDlg->deleteLater();

                    }

                    else if(svnActionCommand == "Update All Elements")
                    {
                        if(_pAppController)
                        {
                            QStringList selectedFiles = getSVNSelectedFiles(svnActionCommand,isUpdateDir, commitMessage, svnErrMsg);

                            if(svnErrMsg.isEmpty() && (!selectedFiles.isEmpty() || isUpdateDir.compare("true") == 0))
                            {
                                QString infoMsg = "You are going to Update All/multiple elements of GTA,This might take some time. Do you really want to proceed?";
                                QMessageBox::StandardButton reply = QMessageBox::question(this, "SVN Update All", infoMsg,QMessageBox::Yes|QMessageBox::No);
                                if(reply == QMessageBox::No)
                                    return;
                            }
                            else if(selectedFiles.isEmpty() && svnErrMsg.isEmpty() ) //TODO DEV: handle/decide shall we give them one more attempt
                            {
                                QMessageBox::information(this,"Info","Terminating SVN Update...Since no files were selected");
                                return;
                            }
                            else if(!svnErrMsg.isEmpty())
                            {
                                QMessageBox::critical(this, svnActionCommand, svnErrMsg);
                                svnErrMsg.clear();
                                return;
                            }
                            if(isUpdateDir.compare("true") == 0)
                            {
                                _svnActionCommand = "Update Directory";
                                _pSvnParamsTempContainer->command = _svnActionCommand;
                                _pSvnParamsTempContainer->cmdIndex = QModelIndex();
                                _pSvnParamsTempContainer->cmdIndexList = QModelIndexList();
                                _pSvnParamsTempContainer->userCredential = _usrCredentials;
                            }
                            else
                            {
                                _pSvnParamsTempContainer->command = "Update All Elements";
                                _pSvnParamsTempContainer->cmdIndex = QModelIndex();
                                _pSvnParamsTempContainer->cmdIndexList = QModelIndexList();
                                _pSvnParamsTempContainer->userCredential = _usrCredentials;
                                _pSvnParamsTempContainer->fileList = selectedFiles;
                            }
                            selectedFiles.clear();
                            isUpdateDir.clear();
                            svnErrMsg.clear();
                        }
                    }

                    else if(svnActionCommand == "Commit All Elements") //TODO: These should be consts defined in header
                    {
                        if(_pAppController)
                        {
                            QStringList selectedFiles = getSVNSelectedFiles(svnActionCommand,isUpdateDir,commitMessage,svnErrMsg);

                            if(!selectedFiles.isEmpty() && svnErrMsg.isEmpty())
                            {
                                QString infoMsg = "You are going to commit All/multiple elements of GTA,This might take some time. Do you really want to proceed?";
                                QMessageBox::StandardButton reply = QMessageBox::question(this, "SVN Commit All", infoMsg,QMessageBox::Yes|QMessageBox::No);
                                if(reply == QMessageBox::No)
                                    return;
                            }
                            else if(!svnErrMsg.isEmpty())
                            {
                                QMessageBox::critical(this, svnActionCommand, svnErrMsg);
                                svnErrMsg.clear();
                                return;
                            }
                            else
                            {
                                QMessageBox::information(this,"Info","Terminating SVN Commit...Since no files were selected");
                                return;
                            }
                            _pSvnParamsTempContainer->command = svnActionCommand;
                            _pSvnParamsTempContainer->cmdIndex = QModelIndex();
                            _pSvnParamsTempContainer->cmdIndexList = QModelIndexList();
                            _pSvnParamsTempContainer->userCredential = _usrCredentials;
                            _pSvnParamsTempContainer->fileList = selectedFiles;
                            _pSvnParamsTempContainer->commitMessage = commitMessage;

                            selectedFiles.clear();
                            isUpdateDir.clear();
                            commitMessage.clear();
                            svnErrMsg.clear();
                        }
                    }

                    else if (svnActionCommand == "Add All Elements")
                    {
                        prepareSvnAddAllCommand(svnActionCommand, *_pSvnParamsTempContainer);
                    }

                    disconnect(&_FileWatcher, SIGNAL(directoryChanged(QString)), this, SLOT(onDirectoryChanged(QString)));
                    showProgressBar("Performing SVN Action .. please wait", this->geometry());
                    QFutureWatcher<QString > * FutureWatcher = new QFutureWatcher<QString>();
                    connect(FutureWatcher,SIGNAL(finished()),this,SLOT(onSvnActionFinished()));
                    QFuture<QString> Future = QtConcurrent::run(_pAppController, &GTAAppController::executeSVNAction,*_pSvnParamsTempContainer);
                    FutureWatcher->setFuture(Future);
                }
            }
        }
    }
    svnActionCommand.clear();
}

void GTAMainWindow::onHideSelecteRowTitle()
{
    GTAEditorWindow *pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if (pCurrSubWindow)
    {
        pCurrSubWindow->onHideSelecteRowTitle();
    }
}

void GTAMainWindow::showBetweenSelectedRows()
{
    GTAEditorWindow *pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if (pCurrSubWindow)
    {
        pCurrSubWindow->onShowBetweenSelectedRows();
    }
}

void GTAMainWindow::onShowClipBoard()
{

    QAbstractItemModel* pModel = nullptr;
    bool rc = _pMdiController->getClipboardModel(pModel);
    if (!rc)
    {
        QMessageBox::information(this,"Clipboard Error",_pMdiController->getLastError());

    }
    else
    {
        if(nullptr!=pModel)
        {

            _pClipBoardWindow->setWindowTitle("Clip board");
            _pClipBoardWindow->setModel(pModel);
            _pClipBoardWindow->show();

            pModel->setParent(_pClipBoardWindow);
        }
    }
}

void GTAMainWindow::createCallSequence()
{
    if(_pAppController)
    {
        _selectedForSeqList.clear();
        _selectedForSeqList = ui->DataTV->selectionModel()->selectedIndexes();

        if(_selectedForSeqList.count() > 0)
        {
            QString dataDirPath = _pAppController->getGTADataDirectory();

            if(_pNewFileDialog)
            {
                _pNewFileDialog->setDataDirPath(dataDirPath);
                _pNewFileDialog->setWindowTitle("New sequence file");
                _pNewFileDialog->setMode(GTANewFileCreationDialog::SEQUENCE);
                _pNewFileDialog->show();

            }
        }
        else
        {
            QMessageBox::warning(this,"Create Call sequence warning", "Nothing selected. Please make a valid selection!!!");
        }
    }
}

void GTAMainWindow::onSequenceCreated()
{
    onNewFileDetailsAdded();
    if(sender() == _pNewFileDialog)
    {
        bool isFileValid = _pNewFileDialog->isFileValid();
        if(isFileValid)
        {
            GTAEditorWindow *pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
            if (pCurrSubWindow)
            {
                QString fileType = _pNewFileDialog->getFileType();

                QModelIndexList selectedItems = ui->DataTV->selectionModel()->selectedIndexes();
                pCurrSubWindow->createCallSequence(selectedItems,fileType);
            }
        }
    }
}

void GTAMainWindow::onDisplayKeyboarShortCutHelp()
{

    if(nullptr==_pShortcuthelpWindow)
    {
        _pShortcuthelpWindow = new GTAKeyBoardShortcutsWidget(this);

    }

    _pShortcuthelpWindow->show();
}

void GTAMainWindow::closeEvent(QCloseEvent *event)
{
    TestConfig* testConf = TestConfig::getInstance();

    showProgressBar("Please wait while GTA closes", this->geometry());
    if ((_pAppController->isSequencerOpen() == true && _pAppController->CloseSequencer() == false)
            || (updateRecentSessionInfo() ==  GTASavePrompt::Ignored))
    {
        event->ignore();
    }
    else
    {
        bool bSave = testConf->getSVNCommitOnExit();

        SVNInputParam svnInput;
        svnInput.command = "Clear SVN Authentication cache";
        svnInput.cmdIndex= QModelIndex();
        svnInput.fileName = QString();
        svnInput.resolveOption = QString();
        svnInput.userCredential = QStringList();

        _pAppController->executeSVNAction(svnInput);
        
        QByteArray geometry_qbytearray(saveGeometry());
        QByteArray geometry_qbytearrayHex = geometry_qbytearray.toHex();
        std::string geometry_str(geometry_qbytearrayHex.constData(), geometry_qbytearrayHex.length());
        testConf->setGeometry(geometry_str);

        QByteArray windowstate_qbytearray = saveState();
        QByteArray windowstate_qbytearrayHex = geometry_qbytearray.toHex();
        std::string windowstate_str(windowstate_qbytearrayHex.constData(), windowstate_qbytearrayHex.length());
        testConf->setWindowState(windowstate_str);

        QHeaderView *DataTVHeader = ui->DataTV->header();

        QByteArray dataTVHeaderStatus;
        if (DataTVHeader)
        {
            // returns nothing (?)
            dataTVHeaderStatus = DataTVHeader->saveState();
        }

        ui->DataTV->model();

        QByteArray hex_array = dataTVHeaderStatus.toHex();
        std::string dataTVHeaderStatus_str(hex_array.constData(), hex_array.length());
        testConf->setDataTVGeometry(dataTVHeaderStatus_str);

        _pAppController->onToolClosure();
        QMainWindow::closeEvent(event);

        testConf->saveConf();
    }
    hideProgressBar();
}

void GTAMainWindow::resetSCXMLInitSubscribeChecks()
{
    //    ui->actionInit_subsribe_start->setChecked(true);
    //    ui->actionInit_subscribe_end->setChecked(true);
}

void GTAMainWindow::onIgnoreOnSCXML()
{
    GTAEditorWindow *pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if (pCurrSubWindow)
    {
        pCurrSubWindow->ignoreInSCXMLOnOff(true);
    }
}

void GTAMainWindow::onSelectAllRows()
{
    GTAEditorWindow* pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if (pCurrSubWindow)
    {
        pCurrSubWindow->selectAllRows();
    }
}

void GTAMainWindow::onIgnoreOffSCXML()
{
    GTAEditorWindow *pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if (pCurrSubWindow)
    {
        pCurrSubWindow->ignoreInSCXMLOnOff(false);
    }
}

/**
 * @brief Slot on Enable/Disable Manual Actions
*/
void GTAMainWindow::onSelectManActToBeIgnored()
{
    GTAEditorWindow* currentWindow = dynamic_cast<GTAEditorWindow*>(ui->EditorFrame->getCurrentWindow());
    if (currentWindow != nullptr)
    {
        GTAIgnoreManualAction* ignoreManActWidget = new GTAIgnoreManualAction(currentWindow);
        int rc = ignoreManActWidget->exec();
        if (rc == 0)
            return;
        
        QList<int> manAcToBeIgnored = ignoreManActWidget->getSelection();
        currentWindow->ignoreManualActions(manAcToBeIgnored);
    }
}

void GTAMainWindow::onSetReadOnlyStatus()
{
    GTAEditorWindow * pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if (pCurrSubWindow)
    {
        pCurrSubWindow->onSetReadOnlyStatusForCmd();
    }
}

void GTAMainWindow::onReadOnlyDocToggled()
{
    GTAEditorWindow *pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if (pCurrSubWindow)
    {
        pCurrSubWindow->onReadOnlyDocToggled(!_ReadOnlyDocToggled);
    }
}

void GTAMainWindow::setReadOnlyDocStatus(bool readOnlyStatus)
{
    _ReadOnlyDocToggled = readOnlyStatus;
    QIcon iconReadOnly;
    QString sToolTip;
}

void GTAMainWindow::connectFoldersToWatcher()
{
    if(_pAppController)
    {
        QStringList dirPaths = _pAppController->getFileWatcherList();
        _FileWatcher.addPaths(dirPaths);
        connect(&_FileWatcher, SIGNAL(directoryChanged(QString)),this,SLOT(onDirectoryChanged(QString)));
    }

    auto gitCtrl = GtaGitController::getGtaGitController();
    if (gitCtrl->isRepoOpened()) {
        onGitStatusClicked(false);
    }
}

void GTAMainWindow::updateDataBrowserModel(const QString &ipath)
{
    TestConfig* testConf = TestConfig::getInstance();

    if(_pAppController)
    {
        QStringList dirPaths = _pAppController->getFileWatcherList();
        _FileWatcher.removePaths(dirPaths);
        disconnect(&_FileWatcher, SIGNAL(directoryChanged(QString)), this, SLOT(onDirectoryChanged(QString)));

        //Should use thread here ..but causes a crash ... used mutex however still crashes.. think of a solution
        _pAppController->insertIntoTreeStructure(QStringList()<<ipath);
        _pAppController->getViewController()->updateTreeBrowserModel();


        ui->DataTV->setModel(nullptr);
        QAbstractItemModel *pTreeModel = nullptr;
        _pAppController->getViewController()->createTreeBrowserModel();
        pTreeModel = _pAppController->getElemDataBrowserModel(true);
        if(pTreeModel != nullptr)
            ui->DataTV->setModel(pTreeModel);
        ui->DataTV->resizeColumnToContents(0);

        //reset all filters and search preferences
        ui->DataTypeCB->setCurrentIndex(0);
        ui->DataSearchLE->clear();

        std::string headerState_str = testConf->getDataTVGeometry();
        QByteArray headerStateHex(headerState_str.c_str(), static_cast<int>(headerState_str.length()));
        QByteArray headerState = QByteArray::fromHex(headerStateHex);
        
        if(!headerState.isEmpty())
            ui->DataTV->header()->restoreState(headerState);

        connectFoldersToWatcher();
        _pAppController->readFavoritesFromDb();
    }
    displayHeaderContextMenu(QPoint());
}

void GTAMainWindow::updateDataBrowserModel()
{
    TestConfig* testConf = TestConfig::getInstance();
    if(_pAppController)
    {
        QStringList dirPaths = _pAppController->getFileWatcherList();
        _FileWatcher.removePaths(dirPaths);
        disconnect(&_FileWatcher, SIGNAL(directoryChanged(QString)), this, SLOT(onDirectoryChanged(QString)));

        _pAppController->updateTreeStructure(false);
        _pAppController->getViewController()->updateTreeBrowserModel();

        ui->DataTV->setModel(nullptr);
        QAbstractItemModel *pTreeModel = nullptr;
        _pAppController->getViewController()->createTreeBrowserModel();
        pTreeModel = _pAppController->getElemDataBrowserModel(true);
        if(pTreeModel != nullptr)
            ui->DataTV->setModel(pTreeModel);

        ui->DataTV->resizeColumnToContents(0);

        std::string headerState_str = testConf->getDataTVGeometry();
        QByteArray headerStateHex(headerState_str.c_str(), static_cast<int>(headerState_str.length()));
        QByteArray headerState = QByteArray::fromHex(headerStateHex);

        if(!headerState.isEmpty())
            ui->DataTV->header()->restoreState(headerState);

        connectFoldersToWatcher();
        _pAppController->readFavoritesFromDb();
    }
}

/**
* @brief Updates the tree view according to the procedure database
* @param rebuildDb if true rebuilds work_db before updating the tree
* @param isInit true if this is called during initialization
*/
void GTAMainWindow::updateTree(bool rebuildDb, bool isInit)
{
    ui->DataTV->setModel(nullptr);
    _pAppController->getViewController()->updateTreeBrowserModel();

    QFutureWatcher<bool>* TreeWatcher = new QFutureWatcher<bool>();
    connect(TreeWatcher, SIGNAL(finished()), this, SLOT(onTreeUpdateFinished()));

    QFuture<bool> TreeFuture = QtConcurrent::run(_pAppController, &GTAAppController::updateTreeStructure, rebuildDb, isInit);
    TreeWatcher->setFuture(TreeFuture);
}

void GTAMainWindow::onTreeUpdateFinished_(bool rc)
{
    if (rc)
    {
        ui->DataTV->setModel(nullptr);
        QAbstractItemModel* pTreeModel = nullptr;
        _pAppController->getViewController()->createTreeBrowserModel();
        pTreeModel = _pAppController->getElemDataBrowserModel(true);
        if (pTreeModel != nullptr)
            ui->DataTV->setModel(pTreeModel);
        ui->DataTV->resizeColumnToContents(0);

        std::string headerState_str = TestConfig::getInstance()->getDataTVGeometry();
        QByteArray headerStateHex(headerState_str.c_str(), static_cast<int>(headerState_str.length()));
        QByteArray headerState = QByteArray::fromHex(headerStateHex);
        if (!headerState.isEmpty())
            ui->DataTV->header()->restoreState(headerState);

        connectFoldersToWatcher();
        _pAppController->readFavoritesFromDb();

    }
    else
    {
        ui->DataTV->setModel(nullptr);
    }
    hideTreeProgressBar();
    displayHeaderContextMenu(QPoint());
}

void GTAMainWindow::onTreeUpdateFinished()
{
    QFutureWatcher<bool> *pWatcher = dynamic_cast<QFutureWatcher<bool> *>(sender());
    if(pWatcher == nullptr)
        return;
    else
    {
        bool rc = pWatcher->result();
        onTreeUpdateFinished_(rc);
    }
    pWatcher->deleteLater();
}

/**
 * @brief Reading the general settings of the application (geometry).
 * @return void
 */
void GTAMainWindow::readSettings()
{
    TestConfig* testConf = TestConfig::getInstance();

    std::string geometry_str = testConf->getGeometry();
    QByteArray geometry_QByteArrayHex(geometry_str.c_str(), static_cast<int>(geometry_str.length()));
    QByteArray geometry_QByteArray = QByteArray::fromHex(geometry_QByteArrayHex);
    restoreGeometry(geometry_QByteArray);

    std::string windowstate_str = testConf->getWindowState();
    QByteArray windowstate_QByteArrayHex(windowstate_str.c_str(), static_cast<int>(windowstate_str.length()));
    QByteArray windowstate_QByteArray = QByteArray::fromHex(windowstate_QByteArrayHex);
    restoreState(windowstate_QByteArray);

    std::string datatv_geometry_str = TestConfig::getInstance()->getDataTVGeometry();
    QByteArray headerStateHex(datatv_geometry_str.c_str(), static_cast<int>(datatv_geometry_str.length()));
    QByteArray headerState = QByteArray::fromHex(headerStateHex);
    ui->DataTV->header()->restoreState(headerState);
}

/**
* @brief Renames a document.
* 
* This method can be improved. The current strategy is to close and reopen the
* window if it is open in the editor frame, which is time-consuming.
* Direct renaming can be done at the level of the window tab, editorWindow and documents separately.
*/
void GTAMainWindow::onRename()
{
    if(_pRenameWidget)
    {
        QModelIndexList selectedIndexes = ui->DataTV->selectionModel()->selectedIndexes();
        if(!selectedIndexes.isEmpty())
        {
            QModelIndex indexToLoad = selectedIndexes.at(0);
            if(indexToLoad.column() == 0 || indexToLoad.column() == 1)
            {
                QString absRelPath = _pAppController->getFilePathFromTreeModel(indexToLoad);
                bool isLoaded = isFileLoaded(absRelPath);
                QString fileName = absRelPath.split("/").last();
                fileName = fileName.mid(0,fileName.lastIndexOf(".")).trimmed();
                if(isLoaded)
                {
                    QMessageBox::StandardButton button = QMessageBox::information(this,"Rename Element","The element is loaded. Cannot rename the element.\n Press Ok to unload the element and continue with rename",QMessageBox::Ok,QMessageBox::Cancel);
                    if(button == QMessageBox::Cancel)
                        return;
                    else
                    {
                        GTAEditorWindow* pWindow = getLoadedWindow(absRelPath);
                        TabIndices windowPos = pWindow->getFramePosition();
                        ui->EditorFrame->closeTab(*windowPos.viewIndex, *windowPos.tabIndex);
                        QTabWidget* currentTabWidget = ui->EditorFrame->getTabWidget(*windowPos.viewIndex);
                        if (ui->EditorFrame->getViewsCount() > 1 && currentTabWidget->count() == 0)
                        {
                            ui->EditorFrame->closeView(*windowPos.viewIndex);
                            ui->EditorFrame->updateWindowsPosition(0, -1);
                        }
                        else
                            ui->EditorFrame->updateWindowsPosition(*windowPos.viewIndex, *windowPos.tabIndex);
                        updateMoveUI();
                    }
                }
                _pRenameWidget->setFileName(fileName);
                _pRenameWidget->show();

            }
        }
    }
}

void GTAMainWindow::onRenameDone(QString iNewName)
{
    if(!iNewName.isEmpty())
    {
        QModelIndexList selectedIndexes = ui->DataTV->selectionModel()->selectedIndexes();
        if(!selectedIndexes.isEmpty())
        {
            QModelIndex indexToLoad = selectedIndexes.at(0);
            if(indexToLoad.column() == 0 || indexToLoad.column() == 1)
            {
                if(_pAppController)
                {
                    QStringList dirPaths = _pAppController->getFileWatcherList();
                    _FileWatcher.removePaths(dirPaths);
                    disconnect(&_FileWatcher, SIGNAL(directoryChanged(QString)),this,SLOT(onDirectoryChanged(QString)));
                    
                    bool rc = _pAppController->renameFile(indexToLoad,iNewName);
                    if(!rc)
                    {
                        QMessageBox msgBox(this);
                        msgBox.setText("A file with the same name already exists. Please use other name");
                        msgBox.setIcon(QMessageBox::Critical);
                        msgBox.setWindowTitle("Error");
                        msgBox.setStandardButtons(QMessageBox::Ok);
                        msgBox.exec();
                        return;
                    }
                    ui->DataSearchLE->clear();
                    connectFoldersToWatcher();
                    onTreeUpdateFinished_(true);
                    updateDataBrowserModel();

                    auto gitCtrl = GtaGitController::getGtaGitController();
                    if (gitCtrl->isRepoOpened()) {
                        onGitStatusClicked(false);
                    }
                }
            }
        }
    }
}

void GTAMainWindow::onRemoveToolID(const QList<GTASCXMLToolId> &iList, const GTASCXMLToolId &iRemoveObj)
{
    if(_pAppController)
    {
        _pAppController->onRemoveToolID(iList,iRemoveObj);
    }
}

void GTAMainWindow::onNewToolIdsAdded(const QList<GTASCXMLToolId> &iList)
{
    if(_pAppController)
    {
        _pAppController->onNewToolIdsAdded(iList);
    }
}

void GTAMainWindow::onRefreshTree()
{
    if(_pAppController)
    {
        disconnect(&_FileWatcher, SIGNAL(directoryChanged(QString)),this,SLOT(onDirectoryChanged(QString)));

        showTreeProgressBar();
        ui->DataTV->setModel(nullptr);
        _pAppController->getViewController()->updateTreeBrowserModel(); // deletes pTreeViewModel
        QFutureWatcher<bool> * TreeWatcher = new QFutureWatcher<bool>();
        connect(TreeWatcher,SIGNAL(finished()),this,SLOT(onTreeUpdateFinished()));
        QFuture<bool> TreeFuture = QtConcurrent::run(_pAppController, &GTAAppController::updateTreeStructure, false, false);
        TreeWatcher->setFuture(TreeFuture);
    }
}

void GTAMainWindow::onUpdateClipBoard()
{
    if(_pMdiController)
    {
        QAbstractItemModel* pModel = _pClipBoardWindow->model();
        _pClipBoardWindow->setModel(nullptr);
        if(pModel!=nullptr)
            delete pModel;
        pModel=nullptr;
        bool rC = _pMdiController->getClipboardModel(pModel);
        if(pModel!=nullptr)
            _pClipBoardWindow->setModel(pModel);
        if (!rC)
            QMessageBox::warning(this,"Warning","Nothing copied, unbalanced \"end\" statements found");
    }
}

/**
* @brief Returns the active procedure window in the procedure editor
*/
QWidget* GTAMainWindow::getActiveSubWindow()
{
    QWidget *pActiveSubWindow = nullptr;
    int index = ui->MultiPerspectiveWidget->currentIndex();
    if (index == EDITOR_PAGE)
    {
        pActiveSubWindow = ui->EditorFrame->getCurrentWindow();
    }
    else if (index == RESULT_PAGE)
    {
        if (!_pResultMdiArea->hasFocus())
            _pResultMdiArea->setFocus(Qt::ActiveWindowFocusReason);
        QMdiSubWindow *pSubWindow = _pResultMdiArea->activeSubWindow();
        if(pSubWindow)
        {
            pActiveSubWindow = pSubWindow->widget();
        }
        else if (_pResultMdiArea->currentSubWindow())
        {
            pActiveSubWindow = _pResultMdiArea->currentSubWindow()->widget();
        }
        else if (_pResultMdiArea->subWindowList().count() > 0)
        {
            if (nullptr != _pResultMdiArea->subWindowList().first())
            {
                pActiveSubWindow = _pResultMdiArea->subWindowList().first()->widget();
            }
        }        
    }
    return pActiveSubWindow;
}

void GTAMainWindow::onUpdateCollapseToTitle(bool iVal)
{
    _titleCollapseToggled = iVal;
}

void GTAMainWindow::onUpdateRowMoveDownAction(bool iVal)
{
    _pMoveDownAction->setDisabled(iVal);
}

void GTAMainWindow::onUpdateRowMoveUpAction(bool iVal)
{
    _pMoveRowUpAction->setDisabled(iVal);
}

void GTAMainWindow::onUpdateActiveStack(QUndoStack * &undoStack)
{
    if(undoStack != nullptr && _pUndoStackGrp != nullptr)
    {
        QUndoStack *currentActiveStack = _pUndoStackGrp->activeStack();

        // if the stack has already been set as active stack then no need to set it again
        if(currentActiveStack != undoStack)
            _pUndoStackGrp->setActiveStack(undoStack);
    }
}

void GTAMainWindow::onRemoveStack(QUndoStack * &undoStack)
{
    if(undoStack != nullptr && _pUndoStackGrp != nullptr)
    {
        QUndoStack *currentActiveStack = _pUndoStackGrp->activeStack();

        // if the stack has already been set as active stack then no need to set it again
        _pUndoStackGrp->removeStack(undoStack);
        if(currentActiveStack == undoStack)
        {
            _pUndoStackGrp->setActiveStack(nullptr);
        }
    }
}

/**
* @brief Gets the window according to its relative path.
* @param iRelPath Relative path of the file.
* @return Corresponding subwindow.
*/
GTAEditorWindow* GTAMainWindow::getLoadedWindow(const QString &iRelPath)
{
    GTAEditorWindow* pCurrEditorWindow = nullptr;
    for (uint viewIndex = 0; viewIndex < ui->EditorFrame->getViewsCount(); viewIndex++)
    {
        QList<GTAEditorWindow*> windowsList = ui->EditorFrame->getAllWindows(viewIndex);
        for (auto window : windowsList)
        {
            pCurrEditorWindow = window;
            if(pCurrEditorWindow)
            {
                QString windowPath = pCurrEditorWindow->getRelativePath();
                if (iRelPath == windowPath)
                    goto endloop;
            }
        }
    }
    endloop:
    return pCurrEditorWindow;
}

/**
* @brief Assess if the designated file is currently loaded in the editor.
* @param iRelPath relative path or UUID.
  @param isUUID true if we use UUID instead of relative path.
  @return rc true if file is open, false otherwise.
  */
bool GTAMainWindow::isFileLoaded(const QString &iRelPath, bool isUUID)
{
    bool rc = false;
    for (uint viewIndex = 0; viewIndex < ui->EditorFrame->getViewsCount(); viewIndex++)
    {
        QList<GTAEditorWindow*> windowsList = ui->EditorFrame->getAllWindows(viewIndex);
        for (auto window : windowsList)
        {
            if (window != nullptr)
            {
                QString match;
                if (isUUID)
                    match = window->getUUID();
                else
                    match = window->getRelativePath();
                if (iRelPath == match)
                {
                    rc = true;
                    goto endLoop;
                }
            }
        }
    }
    endLoop:
    return rc;
}

//returns true if the inconsistency was detected and User selected reload DB else
//returns false if the inconsistency was detected yet the user did not choose to reload the DB
bool GTAMainWindow::checkDatabaseConsistency()
{
    bool rc = false;
    if(_pAppController)
    {
        QString dbVersion = _pAppController->getDatabaseVersion();
        if (dbVersion == GTA_DATABASE_VERSION_STR)
        {
           rc = true;
        }
        else
        {
           QMessageBox msgBox(this);
           msgBox.setText("Database version is : " + dbVersion + " expected version is : " + GTA_DATABASE_VERSION_STR + "\nDo you want to :\n \t- Rebuild the Database\n \t- Cancel and continue at your own risk");
           msgBox.setIcon(QMessageBox::Critical);
           msgBox.setWindowTitle("Database version mismatch");
           msgBox.setStandardButtons(QMessageBox::Ok);
           msgBox.addButton(QMessageBox::Cancel);

           if (msgBox.exec() == QMessageBox::Ok)
              reloadProgram();

           else
              rc = false;
        }
    }
    return rc;
}

void GTAMainWindow::initialiseResultPage()
{
    if (ui->MultiPerspectiveWidget->count() == 1)
    {
        if (nullptr != _pResultMdiArea)
        {
            ui->MultiPerspectiveWidget->addWidget(_pResultMdiArea);
            _pResultMdiArea->setViewMode(QMdiArea::TabbedView);
            createNewResultPage();
            connect(ui->actionLTR, SIGNAL(triggered()), this, SLOT(onResultToolbarClicked()));
            connect(_pExportLTRA, SIGNAL(triggered()), this, SLOT(onResultToolbarClicked()));
        }
    }

}

/* This function is called when we push SwitchButton (Double Arrows) */
void GTAMainWindow::onSwitchClicked()
{
    int index = ui->MultiPerspectiveWidget->currentIndex();
    if (index == EDITOR_PAGE)
    {        
        ui->actionResultAnalyzerView->trigger();
        ui->actionApplication_Logs->setChecked(false);
        ui->actionLogView->setChecked(false);
    }
    else if (index == RESULT_PAGE)
    {
        executeValidationWidget(true);
        ui->actionApplication_Logs->setChecked(true);
        ui->actionLogView->setChecked(true);
    }
}

/**
 * @brief SLOT on actionEditorView triggered
*/
void GTAMainWindow::onActionEditorViewTriggered(bool)
{
    // Execute the validation widget if applied
    executeValidationWidget(true);
    // Change the view
    onChangeToEditorView(true);
}

/**
 * @brief SLOT on change to EditorView
*/
void GTAMainWindow::onChangeToEditorView(bool)
{
    hideRightClickMenu();
    ui->MultiPerspectiveWidget->setCurrentIndex(EDITOR_PAGE);
    ui->actionEditorView->setChecked(true);
    _pActiveSubwindowViewGrp->setDisabled(false);
    ui->ResultToolBar->setVisible(false);
    ui->ResultToolBar->setEnabled(false);
    ui->DataBrowserDW->setVisible(true);
    ui->EditorFileToolBar->setEnabled(true);
    ui->EditorToolBar->setEnabled(true);
    _pExportSCXML->setEnabled(true);
    _pExportLTRA->setEnabled(false);
    ui->GeneralToolBar->setEnabled(true);
    ui->EditorViewToolBar->setEnabled(true);
}

void GTAMainWindow::onPerspectiveChanged(bool )
{
    hideRightClickMenu();
    if(sender() == ui->actionEditorView)
    {
        onChangeToEditorView(true);
    }
    else if(sender() == ui->actionResultAnalyzerView)
    {
        ui->MultiPerspectiveWidget->setCurrentIndex(RESULT_PAGE);
        ui->actionResultAnalyzerView->setChecked(true);
        ui->ResultToolBar->setVisible(true);
        ui->ResultToolBar->setEnabled(true);
        ui->DataBrowserDW->setVisible(false);
        ui->EditorFileToolBar->setEnabled(false);
        ui->EditorToolBar->setEnabled(false);
        _pExportSCXML->setEnabled(false);
        _pExportLTRA->setEnabled(true);
        ui->GeneralToolBar->setEnabled(false);
        ui->EditorViewToolBar->setEnabled(false);

        // on switching perspective view, updating the menu item highlight for selected subwindow view 
        if(QMdiArea::TabbedView == _pResultMdiArea->viewMode())
            ui->actionTabbedView->setChecked(true);
        else
            ui->actionMultiWindowView->setChecked(true);

        //Set Focus and view in expandded mode when there is single Result page with Tabbed view
        if(1 == _pResultMdiArea->subWindowList().count() && QMdiArea::TabbedView == _pResultMdiArea->viewMode())
        {
            GTALogFrame *wgt = dynamic_cast<GTALogFrame *>(getActiveSubWindow());
            if (wgt)
            {
                wgt->onSetFocus();
                wgt->setWindowState(Qt::WindowMaximized);
            }
        }
    }
    else if (sender() == ui->actionLiveView)
    {
        ui->MultiPerspectiveWidget->setCurrentIndex(LIVE_PALLETE);
        //this->setStyleSheet("QToolTip {border: 1px solid #76797C;background-color: #5A7566;color: white;padding: 0px;opacity: 200;} QWidget {color: #eff0f1;background-color: #31363b;selection-background-color: #3daee9;selection-color: #eff0f1;background-clip: border;border-image: none;border: 0px transparent black;outline: 0;}");
        ui->actionLiveView->setChecked(true);
        ui->DataBrowserDW->setVisible(false);
        ui->ResultToolBar->setVisible(false);
        ui->GitToolBar->setVisible(false);
    }
}

void GTAMainWindow::onMdiStyleChanged()
{
    int index = ui->MultiPerspectiveWidget->currentIndex();
    if (index == RESULT_PAGE)
    {
        if (sender() == ui->actionTabbedView)
        {
            _pResultMdiArea->setViewMode(QMdiArea::TabbedView);
            ui->actionTabbedView->setChecked(true);
            initResultTabbar();
        }
        else if (sender() == ui->actionMultiWindowView)
        {
            _pResultMdiArea->setViewMode(QMdiArea::SubWindowView);
            ui->EditorViewToolBar->setEnabled(false);
            ui->actionMultiWindowView->setChecked(true);
        }
    }
}

bool GTAMainWindow::isDockWidgetPresent(const QString &idockWdgtName)
{
    if(_DockWidgetsMap.size() > 0)
    {
        QStringList dockWidgets = _DockWidgetsMap.keys();
        if(dockWidgets.contains(idockWdgtName))
            return true;
    }
    return false;
}

/**
 * This function adds a new dock widget to the main window based on the name passed in the argument
 * However if a dockwidget with the same name already exists, then returns the already created dockwidget
 * At any given time only one dock widget of a given name will exist.
 * For example : Log Window is the window so only one isntance of it will be open.
 * @idockWdgtName : Name of the dock widget to be created
 * @return : QDockWidget * of the widget which is already created.
*/
QDockWidget * GTAMainWindow::addDockWidgetsToMainWindow(const QString & idockWdgtName)
{
    QDockWidget * pDockWidget = nullptr;
    if(!idockWdgtName.isEmpty())
    {
        if(!isDockWidgetPresent(idockWdgtName))
        {
            pDockWidget = new QDockWidget(idockWdgtName,this);
            addDockWidget(Qt::BottomDockWidgetArea,pDockWidget);
            pDockWidget->setObjectName(idockWdgtName);
            pDockWidget->setHidden(true);
            connect(pDockWidget,SIGNAL(visibilityChanged(bool)),this,SLOT(dockWidgetViewActionUpdate(bool)));

            if(_DockWidgetsMap.count() >= 1)
            {
                QDockWidget* lastAddedDockWdgt = _DockWidgetsMap.values().last();
                if(lastAddedDockWdgt)
                {
                    QList<QDockWidget *> lstDockWdgts = tabifiedDockWidgets(lastAddedDockWdgt);
                    QDockWidget *pLastTabbedDockWdgt = nullptr;
                    if(!lstDockWdgts.isEmpty())
                    {
                        pLastTabbedDockWdgt = lstDockWdgts.last();
                        tabifyDockWidget(pLastTabbedDockWdgt,pDockWidget);
                    }
                    else
                        tabifyDockWidget(lastAddedDockWdgt,pDockWidget);
                }
            }
            _DockWidgetsMap.insert(idockWdgtName,pDockWidget);

        }
        else
        {
            pDockWidget = _DockWidgetsMap.value(idockWdgtName);
        }

    }
    return pDockWidget;
}

void GTAMainWindow::displayDebugContextMenu(QPoint iPos)
{
    QWidget * pSourceWidget = (QWidget*)sender();
    if(pSourceWidget != nullptr && pSourceWidget == _pOutputWindow && _pDebugWindowContextMenu != nullptr)
    {
        _pDebugWindowContextMenu->exec(_pOutputWindow->viewport()->mapToGlobal(iPos));
    }

}

void GTAMainWindow::onClearDebugWindow()
{
    if(_pOutputWindow != nullptr)
    {
        _pOutputWindow->clear();
    }
}

void GTAMainWindow::onCopyDebugWindow()
{
    if(_pOutputWindow != nullptr)
    {
        _pOutputWindow->copy();
    }
}

/**
* @brief : Displays the Procedure Dashboard launcher dialog
*/
void GTAMainWindow::onClickGTAControllerLauncher()
{
    const QString gtaDataDirectory = _pAppController->getGTADataDirectory();
    _pGTAControllerLauncherWidget->setDataDirectoryPath(gtaDataDirectory);
    _pGTAControllerLauncherWidget->InitialiseAdditionalTimeValues();
    _pGTAControllerLauncherWidget->show();
}

void GTAMainWindow::createDebugWindowContextMenu()
{
    if(_pDebugWindowContextMenu == nullptr)
    {
        _pDebugWindowContextMenu = new QMenu();
        _pClearDebugWindow = _pDebugWindowContextMenu->addAction(QIcon(":/images/forms/img/CLEAR_ALL.png"),"Clear",this,SLOT(onClearDebugWindow()));
        _pCopydebugWindowText = _pDebugWindowContextMenu->addAction(QIcon(":/images/forms/img/COPY_ROW.png"),"Copy",this,SLOT(onCopyDebugWindow()));
    }
}

void GTAMainWindow::displayApplicationLogContextMenu(QPoint iPos)
{
    QWidget * pSourceWidget = (QWidget*)sender();
    if(pSourceWidget != nullptr && pSourceWidget == _pApplicationLogs && _pAppLogWindowContextMenu != nullptr)
    {
        _pAppLogWindowContextMenu->exec(_pApplicationLogs->viewport()->mapToGlobal(iPos));
    }

}

void GTAMainWindow::onClearAppLogWindow()
{
    if(_pApplicationLogs != nullptr)
    {
        _pApplicationLogs->clear();
    }
}

void GTAMainWindow::onCopyAppLogWindow()
{
    if(_pApplicationLogs != nullptr)
    {
        _pApplicationLogs->copy();
    }
}
void GTAMainWindow::createAppLogWindowContextMenu()
{
    if(_pAppLogWindowContextMenu == nullptr)
    {
        _pAppLogWindowContextMenu = new QMenu();
        _pClearAppLogWindow = _pAppLogWindowContextMenu->addAction(QIcon(":/images/forms/img/CLEAR_ALL.png"),"Clear",this,SLOT(onClearAppLogWindow()));
        _pCopyAppLogWindowText = _pAppLogWindowContextMenu->addAction(QIcon(":/images/forms/img/COPY_ROW.png"),"Copy",this,SLOT(onCopyAppLogWindow()));
    }
}

void GTAMainWindow::onOutputWindowOpened(bool isChecked)
{
    QDockWidget * pDockWidget = addDockWidgetsToMainWindow(DOCK_WDGT_OUTPUT);
    if(pDockWidget)
    {
        if(isChecked)
        {
            if(_pOutputWindow == nullptr)
            {
                _pOutputWindow = new QTextEdit();
                _pOutputWindow->setReadOnly(true);
                _pOutputWindow->setContextMenuPolicy(Qt::CustomContextMenu);
                createDebugWindowContextMenu();
                connect(_pOutputWindow,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(displayDebugContextMenu(QPoint)));
                pDockWidget->setWidget(_pOutputWindow);
            }
            pDockWidget->show();
            _pOutputWindow->show();
            pDockWidget->raise();
        }
        else
            pDockWidget->hide();
    }
}

void GTAMainWindow::onUpdateOutputWindow(const QStringList iMsgLst)
{
    onOutputWindowOpened(true);
    if(_pOutputWindow != nullptr)
    {
        foreach(QString msg, iMsgLst)
        {
            _pOutputWindow->append(msg);
        }
    }
}

void GTAMainWindow::onApplicationLogOpened(bool isChecked)
{
    QDockWidget * pDockWidget = addDockWidgetsToMainWindow(DOCK_WDGT_APPLICATION_LOGS);

    if(pDockWidget)
    {
        if(isChecked)
        {
            if(_pApplicationLogs == nullptr)
            {

                _pApplicationLogs = new QTextEdit();
                _pApplicationLogs->setReadOnly(true);
                _pApplicationLogs->setContextMenuPolicy(Qt::CustomContextMenu);
                createAppLogWindowContextMenu();
                connect(_pApplicationLogs,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(displayApplicationLogContextMenu(QPoint)));
                pDockWidget->setWidget(_pApplicationLogs);
            }
            pDockWidget->show();
            _pApplicationLogs->show();
        }
        else
            pDockWidget->hide();
    }
}

void GTAMainWindow::onUpdateApplicationLogs(const QStringList iMsgLst)
{   
    if (ui->MultiPerspectiveWidget->currentIndex() != RESULT_PAGE)
    {
        onApplicationLogOpened(true);
    }        

    if(_pApplicationLogs != nullptr)
    {
        foreach(QString msg, iMsgLst)
        {
            _pApplicationLogs->append(msg);
        }
    }
}

void GTAMainWindow::updateSearchResults(const QHash<QString,SearchResults> &SearchResultMap, bool isHeader)
{
    QDockWidget * pDockWidget = addDockWidgetsToMainWindow(DOCK_WDGT_SEARCH_RESULTS);

    if(pDockWidget)
    {
	//	pDockWidget->setAttribute(Qt::WA_DeleteOnClose,true);
	//	connect(pDockWidget,SIGNAL(destroyed()),this,SLOT(onWidgetClosed()));
        //        tabifyDockWidget(ui->LogDW,pDockWidget);
        GTASearchResults *pSearchResultWidget = new GTASearchResults();

        if(pSearchResultWidget)
        {
            GTAWidget *pWdgt = dynamic_cast<GTAWidget*>(pSearchResultWidget);
            if(pWdgt)
            {
                connect(pSearchResultWidget,SIGNAL(widgetClosed()),this,SLOT(onWidgetClosed()));
                pSearchResultWidget->setObjectName(DOCK_WDGT_SEARCH_RESULTS);
                if (!isHeader)
                {
                    connect(pSearchResultWidget, SIGNAL(jumpToElement(QString, int, GTAAppController::DisplayContext, bool)), this, SLOT(onJumpToElement(QString, int, GTAAppController::DisplayContext, bool)));
                }
                else
                {
                    connect(pSearchResultWidget, SIGNAL(jumpToElementHeader(QString, int, GTAAppController::DisplayContext, bool)), this, SLOT(onJumpToElementHeader(QString, int, GTAAppController::DisplayContext, bool)));
                }

                pDockWidget->setWidget(pSearchResultWidget);
                pDockWidget->show();
                pSearchResultWidget->show();
                pSearchResultWidget->setSearchResults(SearchResultMap, isHeader);
                pDockWidget->raise();
            }
        }
    }
}

void GTAMainWindow::onLogWindowOpened(bool isChecked)
{
    QDockWidget * pDockWidget = addDockWidgetsToMainWindow(DOCK_WDGT_LOGS);
    if(pDockWidget)
    {
        if(isChecked)
        {
            if(_ReportWidget != nullptr)
            {
                pDockWidget->setWidget(_ReportWidget);
                _ReportWidget->show();
            }
            pDockWidget->show();
            pDockWidget->raise();
        }
        else
            pDockWidget->hide();
    }
}

void GTAMainWindow::onUpdateErrorLog(const ErrorMessageList &iErrorList)
{

    onLogWindowOpened(true);
    //    ui->LogDW->show();
    //    _ReportWidget->show();
    if(_ReportWidget)
        _ReportWidget->setLog(iErrorList);
}

void GTAMainWindow::onUpdateErrorLogMsg(QString iLogMsg)
{

    //        ui->LogDW->show();
    //        _ReportWidget->show();

    onLogWindowOpened(true);
    if(_ReportWidget)
    {
        _ReportWidget->setText(iLogMsg);

    }
}

// This function allow to clear the Log Window before CheckCompatibility() function
void GTAMainWindow::clearLogWindow()
{
	onLogWindowOpened(true);
	ErrorMessageList ErrorList; 
	if (_ReportWidget != nullptr)
	{
        _ReportWidget->onClearLog();
		_ReportWidget->setLog(ErrorList, true);
	}
}

void GTAMainWindow::onCheckCompatibilityPBClicked()
{
    GTAEditorWindow *pCurrWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if (pCurrWindow)
    {
        QString windowUuid = pCurrWindow->getUUID();
        if (nullptr != _pMdiController->getActiveViewController())
        {
            QString controllerUUID = _pMdiController->getActiveViewController()->getUUID();
            if(windowUuid != controllerUUID)
            {
                QMessageBox::critical(this,"Error","Error: No file selected to check compatibility");
            }
            else
            {
				clearLogWindow();
				pCurrWindow->checkCompatibility();
            }
        }
        else
        {
            QMessageBox::critical(this,"Error","Error: No file selected to check compatibility");
        }
    }
}

void GTAMainWindow::onGitInitClicked() {
    auto gitController = GtaGitController::getGtaGitController();
    auto testConf = TestConfig::getInstance();
    //
    auto gitDir = testConf->getGitDbPath();
    if (gitDir.empty()) {
        QString selectDir = QFileDialog::getExistingDirectory();
        if (selectDir.isEmpty()) {
            QMessageBox::critical(this, "Empty git path", "Please set .git folder location in the config");
            return;
        }
        else {
            testConf->setGitDbPath(selectDir.toStdString());
            gitDir = selectDir.toStdString();
        }
    }
    gitController->setRepoPath(gitDir);
    auto error = gitController->openRepo();
    if (error) {
        QMessageBox::critical(this, "Git Init Repo Error", error->c_str());
        _pGitStatus->setEnabled(false);
        _pGitFolder->setEnabled(false);
    }
    else {
        _pGitStatus->setEnabled(true);
        _pGitFolder->setEnabled(true);
    }
}

void GTAMainWindow::onGitStatusClicked(bool enablePopups) {
    using namespace std::chrono;
    auto t1 = high_resolution_clock::now();
    auto gitController = GtaGitController::getGtaGitController();
    struct gitData {
        GTATreeItem* treeItem = nullptr;
        QString GitPath = "";
        GitStatus fileStatus = GitStatus::Unknown;
    };

    auto* treeRoot = _pAppController->getRootTreeNode();
    auto rootPath = _pAppController->getGTADataDirectory();

    auto children = treeRoot->getAllChildren();

    QStringList filenames;
    QList<gitData> treeGitData;
    for (const auto& child : children) {
        gitData data;
        data.treeItem = child;
        treeGitData.push_back(data);
    }
    QMap<QString, QString> foundFiles;
    QString repoDirectory = QString().fromStdString(gitController->getRepoPath());
    
    QDirIterator repoDirIterator(repoDirectory, QDirIterator::Subdirectories);
    QStringList allPaths;
    size_t matchedFiles = 0;
    size_t checkedFiles = 0;

    while (repoDirIterator.hasNext()) {
        QString filename = repoDirIterator.next();
        QFileInfo file(filename);
        for (auto& targetItem : treeGitData) {
            auto& targetFilename = targetItem.treeItem->getAbsoluteRelPath();
            auto checkedFile = file.filePath();
            checkedFile.replace(rootPath, "");
            if (targetFilename == checkedFile) {
                auto gitRootFilepath = file.filePath();
                gitRootFilepath.replace(repoDirectory + "/", "");
                targetItem.GitPath = gitRootFilepath;
                matchedFiles++;
            }
        }
    }
    for (auto& treeItem : treeGitData) {
        if (!treeItem.GitPath.isEmpty()) {
            auto status = gitController->CheckFileStatus(treeItem.GitPath.toStdString());
            treeItem.treeItem->setGitStatus(status);

        }
    }
    if (enablePopups) {
        if (!matchedFiles) {
            QString msg = QString("Please make sure correct git repository path is selected.\n"
                "Your git path: %1\n"
                "Your workspace: %2").arg(QString().fromStdString(gitController->getRepoPath()), _pAppController->getGTADataDirectory());

            QMessageBox::critical(this, "Could not match workspace files to git repository.", msg);
        }
        else {
            QMessageBox::information(this, "Git status", "Git status updated");
        }
    }
}

void GTAMainWindow::onGitDirectoryClicked() {
    auto gitCtrl = GtaGitController::getGtaGitController();
    QString path = QString().fromStdString(gitCtrl->getRepoPath());
    QFileInfo gitDir(path);
    if (gitDir.exists()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    }
    else {
        QMessageBox::critical(this, "Git repository does not exist", QString("Checked path: ") + path);
    }
}

void GTAMainWindow::onSaveEditorDoc()
{
    _pAppController->storeFavoritesInXml();
    GTAEditorWindow* pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if (pCurrSubWindow)
    {
        pCurrSubWindow->onSave();
        _validationStatusToBeUpdatedOnTreeView = true;
        onupdateValidationInfoFinished();
    }
    //QSet<QString> fullPath = { _pAppController->getGTADataDirectory() + pCurrSubWindow->getRelativePath() };
    //updateTree(false, false, fullPath);
}

/**
* @brief Slot controlling the saving of a file (CTRL+S)
* @param iFilePath Path of the current file
*/
void GTAMainWindow::onSaveEditorDocFinished(const QString &iFilePath)
{
    _pAppController->updateTreeItem(iFilePath, GTAAppController::Save);

    QString dataType = ui->DataTypeCB->currentText();
    onDataTypeValChange(dataType);
    
	GTAEditorWindow *pCurrWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
	if (pCurrWindow)
	{
		QString headerFilename = pCurrWindow->getHeaderName();

        if(headerFilename!=ELEM_TMPL)
        {
            // Set view for header browser
            GTASystemServices* pSystemService = _pAppController->getSystemService();
            QString dataDir = pSystemService->getDataDirectory();

            QString headerFilepath = QString("%1/%2.hdr").arg(dataDir, headerFilename);
            QFile header_file(headerFilepath);

            if (header_file.exists())
            {
                // After saving the current file, its associated header file will be removed #377899
                header_file.remove();
            }
        }
	}
}

void GTAMainWindow::onRestoreFileWatcher()
{
    connectFoldersToWatcher();
}

void GTAMainWindow::onDisconnectFileWatcher()
{
    QStringList dirPaths = _pAppController->getFileWatcherList();
    _FileWatcher.removePaths(dirPaths);
    disconnect(&_FileWatcher, SIGNAL(directoryChanged(QString)), this, SLOT(onDirectoryChanged(QString)));
}

void GTAMainWindow::onOpenInNewWindow(const QString &iFileRelPath,bool isUUID)
{
    if(isUUID)
    {
        QString uuid = iFileRelPath;
        QString path = _pAppController->getAbsoluteFilePathFromUUID(uuid);
        QString dataDir = _pAppController->getGTADataDirectory();
        path.replace(dataDir,"");
        openDocument(path);

    }
    else
    {
        openDocument(iFileRelPath);
    }

}

void GTAMainWindow::onSessionManagerSelected()
{
    if(_pAppController)
    {
        QStringList lstSessions = _pAppController->getSessionNames();
        GTASessionManager *pSessionManager = new GTASessionManager;
        if(pSessionManager)
        {
            GTAWidget *pWdgt = dynamic_cast<GTAWidget*>(pSessionManager);
            if(pWdgt)
            {
                connect(pSessionManager,SIGNAL(widgetClosed()),this,SLOT(onWidgetClosed()));
                connect(pSessionManager,SIGNAL(deleteSession(QStringList)),this,SLOT(onDeleteSession(QStringList)));
                connect(pSessionManager,SIGNAL(saveSession(QString)),this,SLOT(onAddNewSession(QString)));
                connect(pSessionManager,SIGNAL(switchSession(QString)),this,SLOT(onSwitchSession(QString)));
                pSessionManager->setSessions(lstSessions);
                pSessionManager->show();
            }
        }
    }
}

void GTAMainWindow::onPortingWizardClicked()
{

    if(_pAppController)
    {
        //        GTAPortingDialog *portingDlg = new GTAPortingDialog;
        //        if(portingDlg)
        //        {
        //            connect(portingDlg,SIGNAL(startPortingFiles(QString)),this,SLOT(onStartPortingFiles(QString)));
        //            connect(portingDlg,SIGNAL(portingDone()),this,SLOT(onPortingDone()));
        //            portingDlg->show();
        //        }

        //    }

        QMessageBox::StandardButton button = QMessageBox::information(this,"Porting files",
                                                                      "You are about to port old files from DATA folder to new GTA format.\n This might take a while and the tool will be locked during this process.\nDo you want to contnue?",
                                                                      QMessageBox::Yes,QMessageBox::No);
        if(button == QMessageBox::Yes)
        {

            QString gtaDataDir = _pAppController->getGTADataDirectory();
            QString dirPath = QDir::cleanPath(gtaDataDir);
            showProgressBar("Porting in progress...", this->geometry());
            QFutureWatcher<QString> * PortingWatcher = new QFutureWatcher<QString>();
            connect(PortingWatcher,SIGNAL(finished()),this,SLOT(onPortingFinished()));
            QFuture<QString> PortingFuture = QtConcurrent::run(_pAppController,&GTAAppController::portFiles,dirPath,true);
            PortingWatcher->setFuture(PortingFuture);
        }
    }
}

void GTAMainWindow::onPortingFinished()
{
    hideProgressBar();
    QFutureWatcher<QString> *pWatcher = dynamic_cast<QFutureWatcher<QString> *>(sender());
    if(pWatcher == nullptr)
        return;

    QMessageBox::information(this,"Porting files","Porting Completed",QMessageBox::Ok);

    pWatcher->deleteLater();
    showTreeProgressBar();
    updateTree(true);
}

void GTAMainWindow::onDeleteSession(const QStringList &iSelectedItems)
{
    if(!iSelectedItems.isEmpty() && _pAppController != nullptr)
    {
        _pAppController->deleteSession(iSelectedItems);
        removeSessionFromRecents(iSelectedItems);
    }
}

void GTAMainWindow::onSwitchSession(const QString &iSessionName)
{
    if(!iSessionName.isEmpty())
    {
        int retVal;
        if(_recentSessions.count() <= 0)
        {
            retVal = GTASavePrompt::Accepted;
        }
        retVal = saveSession(true);
        if(retVal != GTASavePrompt::Ignored)
        {
            onCloseAllSubwindows();
            loadSession(iSessionName);

            bool isFound = false;
            //bool isDef = false;
            if(_recentSessions.count())
            {
                for(int i = 0; i < _recentSessions.count(); i++)
                {
                    QAction *pAction = _recentSessions.at(i);
                    if(pAction)
                    {
                        if(iSessionName == pAction->text())
                        {
                            _recentSessions.append(_recentSessions.takeAt(i));
                            isFound = true;
                            break;
                        }
                    }
                }

                //    isDef = (iSessionName == "Default");
                //     ui->actionClose_Session->setEnabled(isDef);
                if(isFound)
                {
                    updateRecentMenu(iSessionName);
                }
                else
                    addSessionToRecents(iSessionName);

            }
            else
            {
                addSessionToRecents(iSessionName);

            }
        }

    }
}

void GTAMainWindow::onAddNewSession(const QString &iSessionName)
{
    if(!iSessionName.isEmpty() && _pAppController != nullptr)
    {
        QStringList lstFileNames;
        for (uint viewIndex = 0; viewIndex < ui->EditorFrame->getViewsCount(); viewIndex++)
        {
            QList<GTAEditorWindow*> windowsList = ui->EditorFrame->getAllWindows(viewIndex);
            for (auto window : windowsList)
            {
                if(window)
                {
                    QString uuid = window->getUUID();
                    QString absFilePath = _pAppController->getAbsoluteFilePathFromUUID(uuid);
                    const QString GTADirPath = _pAppController->getGTADataDirectory();
                    if(!absFilePath.isEmpty())
                    {
                        absFilePath.replace(GTADirPath,"");
                        lstFileNames << absFilePath;
                    }
                }
            }
            _pAppController->addNewSession(iSessionName,lstFileNames);
            addSessionToRecents(iSessionName);
        }
    }
}

void GTAMainWindow::onCloseSession()
{
    int retVal = saveSession(true);
    if(retVal != GTASavePrompt::Ignored)
    {
        onCloseAllSubwindows();
        for(int i = 0; i < _recentSessions.count(); i++)
        {
            QAction *pAction = _recentSessions.at(i);
            if(pAction)
            {
                if("Default" == pAction->text())
                {
                    _recentSessions.append(_recentSessions.takeAt(i));
                    break;
                }
            }
        }
        updateRecentMenu("Default");
    }
}

void GTAMainWindow::onSaveSession()
{
    saveSession();
}

//saves current session
int GTAMainWindow::saveSession(bool isGlobalSave)
{
    int rc;
    QAction *pAction = _recentSessions.last();
    if(pAction)
    {
        QString sessionName = pAction->text();
        QList<GTAEditorWindow *> lstModifiedFiles;
        QStringList lstFileNames;
        for(uint viewIndex = 0; viewIndex < ui->EditorFrame->getViewsCount(); viewIndex++)
        {
            QList<GTAEditorWindow*> windowsList = ui->EditorFrame->getAllWindows(viewIndex);
            for (auto window : windowsList)
            {
                if (window)
                {
                    QString uuid = window->getUUID();
                    QString absFilePath = _pAppController->getAbsoluteFilePathFromUUID(uuid);
                    const QString GTADirPath = _pAppController->getGTADataDirectory();
                    if (!absFilePath.isEmpty())
                    {
                        absFilePath.replace(GTADirPath, "");
                        lstFileNames << absFilePath;
                    }

                    if (window->isFileModified())
                    {
                        window->setGlobalSave(isGlobalSave);
                        lstModifiedFiles.append(window);
                    }
                }
            }
        }

        QStringList savedFiles = _pAppController->getSessionFiles(sessionName);
        QSet<QString> dbFiles = QSet<QString>::fromList(savedFiles);
        QSet<QString> newFiles = QSet<QString>::fromList(lstFileNames);
        QSet<QString> tempSet(newFiles);
        bool sessionModified = ((newFiles.subtract(dbFiles).count() > 0) || (dbFiles.subtract(tempSet).count() > 0));

        if(lstModifiedFiles.count())
        {
            rc = savePrompt(lstModifiedFiles);
            if(rc == GTASavePrompt::Accepted)
            {
                _pAppController->saveSession(sessionName,lstFileNames);
            }
            else if(rc == GTASavePrompt::Ignored)
            {
                updateRecentMenu();
            }
        }
        else if(sessionModified)
        {
            _pAppController->saveSession(sessionName,lstFileNames);
        }
    }
    return rc;
}

int GTAMainWindow::savePrompt(QList<GTAEditorWindow *> &iLstModifiedFiles)
{
    int retVal;
    if(iLstModifiedFiles.count())
    {
        QStringList fileNames;
        for(int i = 0; i < iLstModifiedFiles.count(); i++)
        {
            GTAEditorWindow *pWindow = iLstModifiedFiles.at(i);
            QString uuid = pWindow->getUUID();
            QString relPath = _pAppController->getAbsoluteFilePathFromUUID(uuid).replace(_pAppController->getGTADataDirectory(),"");
            fileNames << relPath;
        }

        GTASavePrompt savePrompt;
        savePrompt.setFiles(fileNames);
        retVal = savePrompt.exec();

        if(retVal == GTASavePrompt::Accepted)
        {
            int status = GTASavePrompt::Accepted;
            onSaveAll();

        }
    }
    return retVal;
}

void GTAMainWindow::updateRecentMenu(const QString &iCurrSessionName)
{
    //This entire for loop is only written to clear the MenuBar. Using ui->menuRecent_Sessions->clear()
    //deletes the actions and the _recentSessions then has dangling pointers hence this approach
    //Change the code with caution!!!
    QObjectList objList = ui->menuRecent_Sessions->children();
    for(int j = 0; j < objList.count(); j++)
    {
        QObject *pObj = objList.at(j);
        if(pObj /*&& pObj->objectName() != "Default"*/)
        {
            QAction *pAction = dynamic_cast<QAction*>(pObj);
            if(pAction)
            {
                ui->menuRecent_Sessions->removeAction(pAction);

            }
        }
    }

    //This for loop actually adds actions to the menu bar
    for(int i = _recentSessions.count()-1, count = 0; (i >= 0 && count < RECENT_MAX_COUNT); i--,count++)
    {
        QAction *pAction = _recentSessions.at(i);
        if(pAction != nullptr && !iCurrSessionName.isEmpty() && iCurrSessionName == pAction->text())
        {
            pAction->setChecked(true);
        }
        ui->menuRecent_Sessions->addAction(pAction);
    }
}

void GTAMainWindow::addSessionToRecents(const QString &iSessionName)
{
    if(!iSessionName.isEmpty())
    {
        QAction *action = new QAction(iSessionName,ui->menuRecent_Sessions);
        action->setCheckable(true);
        action->setObjectName(iSessionName);
        _pActionGrp->addAction(action);
        connect(action,SIGNAL(triggered()),this,SLOT(onRecentSessionSelected()));
        _recentSessions.append(action);
        updateRecentMenu(iSessionName);
    }

}

void GTAMainWindow::removeSessionFromRecents(const QStringList &iSessionLst)
{
    if(!iSessionLst.isEmpty())
    {
        bool isFound = false;
        foreach(QString sessionName,iSessionLst)
        {
            QMutableListIterator <QAction*> i(_recentSessions);
            while(i.hasNext())
            {
                QAction *pAction = i.next();
                if(sessionName == pAction->objectName())
                {
                    isFound |= true;
                    i.remove();
                    break;
                }
            }
        }
        if(isFound)
            updateRecentMenu();
    }
}

void GTAMainWindow::onWidgetClosed()
{
    GTAWidget *pWdgt = dynamic_cast<GTAWidget *>(sender());
    if(pWdgt != nullptr)
    {
        delete pWdgt;
        pWdgt = nullptr;
    }
}

void GTAMainWindow::onStartPortingFiles(const QString &iPortDirPath)
{

    QString dirPath = QDir::cleanPath(iPortDirPath);
    showProgressBar("Porting in progress...", this->geometry());
    QFutureWatcher<QString> * PortingWatcher = new QFutureWatcher<QString>();
    connect(PortingWatcher,SIGNAL(finished()),this,SLOT(onPortingFinished()));
    QFuture<QString> PortingFuture = QtConcurrent::run(_pAppController,&GTAAppController::portFiles,dirPath,true);
    PortingWatcher->setFuture(PortingFuture);
}

int GTAMainWindow::updateRecentSessionInfo()
{
    int val = -1;
    if(!_recentSessions.isEmpty() && _pAppController != nullptr)
    {
        //updating recent list
        QStringList recentSessions;
        for(int i = _recentSessions.count()-1, count = 0; (i >= 0 && count < RECENT_MAX_COUNT); i--,count++)
        {
            QAction *pAction = _recentSessions.at(i);
            if(pAction)
            {
                recentSessions.append(pAction->text());
            }
        }		
        val = saveSession(true);
        if (val != GTASavePrompt::Ignored)
            _pAppController->writeSessionInfoFile(recentSessions);

    }
    return val;
}

void GTAMainWindow::onSaveToDocxReady()
{
    if((_pNewFileDialog != nullptr) && (sender() == _pNewFileDialog))
    {
        GTAElement *pElement = _pNewFileDialog->getElement();
        QString path = _pNewFileDialog->getFilePath();
        QString name = _pNewFileDialog->getFileName();
        if(pElement && _pAppController)
        {

            showProgressBar("Saving element in .docx format ...", this->geometry());
            QFutureWatcher<bool> * Watcher = new QFutureWatcher<bool>();
            connect(Watcher,SIGNAL(finished()),this,SLOT(onSaveToDocxFinished()));
            QFuture<bool> Future = QtConcurrent::run(_pAppController,&GTAAppController::convertToDocx,pElement,path,name);
            Watcher->setFuture(Future);


        }
    }
}

void GTAMainWindow::onSaveToDocxFinished()
{
    hideProgressBar();
    QFutureWatcher<bool> *pWatcher = dynamic_cast<QFutureWatcher<bool> *>(sender());
    if(pWatcher == nullptr)
        return;

    bool rc = pWatcher->result();
    if(rc)
        QMessageBox::information(this,"File saved","File saved to .docx format successfully !!!",QMessageBox::Ok);
    else
        QMessageBox::warning(this,"Error",_pAppController->getLastError(),QMessageBox::Ok);

    pWatcher->deleteLater();

}

void GTAMainWindow::onPrintElement()
{
    GTAEditorWindow *pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if (pCurrSubWindow != nullptr)
    {

        GTAElement *pElement = pCurrSubWindow->getElement();
        if(pElement && _pAppController)
        {
            QString elementName = pElement->getName();
            //            QString UUID = pElement->getUuid();
            //            QString absoluteFilePath = _pAppController->getAbsoluteFilePathFromUUID(UUID);
            //            QStringList lstSep = absoluteFilePath.split("/");
            //            lstSep.removeLast();
            //            QString path = lstSep.join("/");

            if(_pNewFileDialog != nullptr)
            {

                _pNewFileDialog->setDataDirPath("");
                _pNewFileDialog->setWindowTitle("Save As");
                _pNewFileDialog->setFileName(elementName);
                _pNewFileDialog->setElement(pElement);
                _pNewFileDialog->setMode(GTANewFileCreationDialog::SAVE_AS_DOCX);
                _pNewFileDialog->show();
            }
        }
    }
}

//false - hide/disable
//true - show/enable
void GTAMainWindow::toggleEditorActions(bool iToggleVal)
{

    disableEditingActions(!iToggleVal);

    QDockWidget *pDockWdgt = addDockWidgetsToMainWindow(DOCK_WDGT_LOGS);
    if(pDockWdgt != nullptr && pDockWdgt->objectName() == DOCK_WDGT_LOGS && !pDockWdgt->isHidden())
    {
        ui->actionLogView->setChecked(iToggleVal);
        ui->actionLogView->setEnabled(iToggleVal);
        ui->actionApplication_Logs->setChecked(iToggleVal);
        ui->actionApplication_Logs->setEnabled(iToggleVal);

    }
    ui->actionData_Browser->setChecked(iToggleVal);
    ui->actionData_Browser->setEnabled(iToggleVal);

    ui->actionSearch_Replace->setEnabled(iToggleVal);

    ui->actionMultiWindowView->setEnabled(iToggleVal);
    ui->actionOpen->setEnabled(iToggleVal);
    ui->menuRecent_Sessions->setEnabled(iToggleVal);
    ui->actionSession_Manager->setEnabled(iToggleVal);

    ui->actionPrint->setEnabled(iToggleVal);


    if(_pMoveRowUpAction)
        _pMoveRowUpAction->setEnabled(iToggleVal);
    if(_pMoveDownAction)
        _pMoveDownAction->setEnabled(iToggleVal);

    ui->menuPerspective->setEnabled(iToggleVal);
    ui->menuSubwindow->setEnabled(iToggleVal);
    ui->actionView_Header->setEnabled(iToggleVal);

    ui->actionConfiguration->setEnabled(iToggleVal);
    ui->actionTheme->setEnabled(iToggleVal);
    ui->menuExport->setEnabled(iToggleVal);
    ui->menuImport->setEnabled(iToggleVal);
    ui->actionLaunch_Scxml->setEnabled(iToggleVal);
    ui->actionLaunch_with_Sequencer->setEnabled(iToggleVal);


    if(_pSave)
        _pSave->setEnabled(iToggleVal);
    if(_pClose)
        _pClose->setEnabled(iToggleVal);

    if(_pCloseAll)
        _pCloseAll->setEnabled(iToggleVal);

    if(_pCloseAllButThis)
        _pCloseAllButThis->setEnabled(iToggleVal);


    if(_pSwitchToEditorMode)
        _pSwitchToEditorMode->setEnabled(iToggleVal);

    if(_pEnableDisableUnsubscribe)
        _pEnableDisableUnsubscribe->setEnabled(iToggleVal);

    if (_pEditTagVariables)
        _pEditTagVariables->setEnabled(iToggleVal);

    if(_pViewResult)
        _pViewResult->setEnabled(iToggleVal);


    QList<QAction*> lstOfActions = ui->EditorViewToolBar->actions();
    foreach(QAction* currAction, lstOfActions)
    {
        if(currAction)
            currAction->setEnabled(iToggleVal);
    }


    if(!_DockWidgetsMap.isEmpty())
    {
        QMap<QString,QDockWidget*>::iterator itr;

        for(itr = _DockWidgetsMap.begin(); itr != _DockWidgetsMap.end(); itr++)
        {
            QDockWidget * pDockWdgt = itr.value();
            QString key = itr.key();
            if((key != DOCK_WDGT_OUTPUT) && (pDockWdgt != nullptr))
            {
                if(!pDockWdgt->isHidden() && iToggleVal == false)
                {
                    pDockWdgt->hide();
                }

            }
        }
    }
    ui->ProgramSelToolBar->setEnabled(iToggleVal);
    ui->GeneralToolBar->setEnabled(iToggleVal);
    ui->EditorFileToolBar->setEnabled(iToggleVal);
    ui->EditorViewToolBar->setEnabled(iToggleVal);
    ui->ExportToolBar->setEnabled(iToggleVal);
    ui->GitToolBar->setEnabled(iToggleVal);

}

QAction* GTAMainWindow::indentedView()
{
    // Commented code because we want to keep the details visible
    //for (uint viewIndex = 0; viewIndex < ui->EditorFrame->getViewsCount(); viewIndex++)
    //{
    //    QList<GTAEditorWindow*> windowsList = ui->EditorFrame->getAllWindows(viewIndex);
    //    for (auto window : windowsList)
    //    {
    //        if (_pIndentedView != nullptr && window != nullptr)
    //        {
    //            if (_pIndentedView->isChecked())
    //                window->hideColumns();
    //            else
    //                window->showColumns();
    //        }
    //    }
    //}

    if (_pIndentedView)
    {
        GTAElementViewModel* pModel = nullptr;
        pModel->setIndentedView(_pIndentedView->isChecked());
        return _pIndentedView;
    }
    
    return nullptr;
}

void GTAMainWindow::onEditTagVariables()
{
    GTAEditorWindow *pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if(pCurrSubWindow != nullptr)
    {
        GTAElement *pElem = pCurrSubWindow->getElement();
        QList<TagVariablesDesc> tagVarList = pElem->getTagList();
        QString filepath = pElem->getRelativeFilePath();
        if(tagVarList.isEmpty())
        {
            QMessageBox::warning(this,"Error","No valid Tag variables exist",QMessageBox::Ok);
        }
        else
        {
            GTATagVariableDescWidget * pWdgt = new GTATagVariableDescWidget(filepath);
            pWdgt->setWindowModality(Qt::ApplicationModal);
            pWdgt->setAttribute(Qt::WA_DeleteOnClose);
            pWdgt->setWindowTitle(QString("Tag Variable Properties (%1)").arg(filepath));
            connect(pWdgt,SIGNAL(sendUserInput(QString,QList<TagVariablesDesc>)),this,SLOT(onUserInputTagVarDesc(QString,QList<TagVariablesDesc>)));
            pWdgt->showWidget(tagVarList);
        }
    }
}

/**
 * @brief Slot function on Maximum Time Estimated edition
*/
void GTAMainWindow::onEditMaxTimeEstimated()
{
    GTAEditorWindow* pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if (pCurrSubWindow != nullptr)
    {
        bool rc;
        QString old_value = pCurrSubWindow->getMaxTimeEstimated();
        QString new_value = QString::number(QInputDialog::getInt(this, tr("Edit value"),
            tr("Maximum Time Estimated (") + XNODE_MAX_TIME_ESTIMATED_UNIT + tr("): "),old_value.toInt(),0,100000000,1, &rc, Qt::MSWindowsFixedSizeDialogHint));

        bool emptyStr = new_value.trimmed().isEmpty();
        bool valueChanged = old_value != new_value;
        if (rc && !emptyStr && valueChanged)
        {
           pCurrSubWindow->setUndoRedoForMTE(old_value, new_value);
           pCurrSubWindow->setMaxTimeEstimated(new_value);
        }
    }
}

void GTAMainWindow::onUserInputTagVarDesc(QString iFileName, QList<TagVariablesDesc> tagList)
{
    GTAEditorWindow *pCurrSubWindow = dynamic_cast<GTAEditorWindow*>(getActiveSubWindow());
    if(pCurrSubWindow != nullptr)
    {
        GTAElement *pElem = pCurrSubWindow->getElement();
        pElem->setTagVarStructList(tagList);
    }
}

/**
  * This function creates context menu for result pages
 */
void GTAMainWindow::initResultTabbar()
{
    QList<QTabBar *> tabBarList = _pResultMdiArea->findChildren<QTabBar*>();
    for (int i = 0; i < tabBarList.count(); i++)
    {
        QTabBar *tabBar = tabBarList.at(i);
        if (tabBar)
        {
            tabBar->setExpanding(false);
            tabBar->setMovable(true);
            tabBar->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(tabBar, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(displayResultTabContextMenu(QPoint)));
        }
    }
}

/**
  * This function populates the context menu in result page
  */
void GTAMainWindow::displayResultTabContextMenu(QPoint iPoint)
{
    QList<QTabBar *> tabBarList = _pResultMdiArea->findChildren<QTabBar*>();

    QTabBar * pTabBar = tabBarList.at(0);
    if (pTabBar)
    {
        int index = pTabBar->tabAt(iPoint);
        if (index > -1)
        {
            int currentTabIndex = pTabBar->currentIndex();
            if (currentTabIndex == index)
            {
                _pResultTabContextMenu->popup(pTabBar->mapToGlobal(iPoint));
            }
        }
    }
}

/**
* This function returns widget object if window title is present in Result Mdi Area othrewise nullptr
* @titleName: Window title of result page i.e. relative path
* @return: QWidget object or nullptr
*/
QWidget *GTAMainWindow::getWidgetFromTitle(QString titleName)
{
    QWidget *pWidget = nullptr;
    QList<QMdiSubWindow *> tabBarList = _pResultMdiArea->subWindowList();
    int tabCount = tabBarList.count();
    for(int i=0; i < tabCount ; i++)
    {
        if(tabBarList.at(i)->windowTitle().compare(titleName) == 0)
        {
            QMdiSubWindow *pSubWindow = tabBarList.at(i);
            if (pSubWindow)
            {
                pWidget = pSubWindow->widget();
            }
            break;
        }
    }
    return pWidget;
}

/**
 * This function creates a new blank result analysis page and attached to Result Mdi Area
 * @return: Subwindow object of new page created
*/
GTALogFrame *GTAMainWindow::createNewResultPage()
{
    GTALogFrame * newLogFrame = new GTALogFrame(_pAppController);
    newLogFrame->setAttribute(Qt::WA_DeleteOnClose);
    setNewWindowTitle(newLogFrame);
    _pResultMdiArea->addSubWindow(newLogFrame);
    initResultTabbar();
    connect(newLogFrame, &GTALogFrame::UpdateErrorLog, this, &GTAMainWindow::onUpdateErrorLog);

    // Hide Log and Application Logs windows
    ui->actionApplication_Logs->setChecked(false);
    ui->actionLogView->setChecked(false);

    return newLogFrame;
}

/**
 * @return: appController object
*/
GTAAppController* GTAMainWindow::getAppController()
{
    return _pAppController;
}

/**
  * This function sets the window title of new created result page similar to notepad++ style
  * @newLogFrame: Input newLogFrame subwindow for which window title is to set
 */
void GTAMainWindow::setNewWindowTitle(GTALogFrame *&newLogFrame)
{
    QList<QMdiSubWindow *> tabBarList = _pResultMdiArea->subWindowList();
    int tabCount = tabBarList.count();
    int i, j;
    bool isFlag = false;
    for(i=1; i < tabCount+1 ; i++)
    {
        if(isFlag == true)
            break;
        for(j = 0; j < tabCount; j++)
        {
            if(QString(QString("New Result %1").arg(QString::number(i))) == tabBarList.at(j)->windowTitle())
                break;
        }
        if(j >= tabCount)
        {
            isFlag = true;
        }
    }
    if(isFlag == true)
        i= i-1;
    newLogFrame->setWindowTitle(QString("New Result " + QString::number(i)));
}

void GTAMainWindow::refreshTreeView()
{
	disconnect(&_FileWatcher, SIGNAL(directoryChanged(QString)),this,SLOT(onDirectoryChanged(QString)));

	showTreeProgressBar();
	ui->DataTV->setModel(nullptr);
	_pAppController->getViewController()->updateTreeBrowserModel();
	QFutureWatcher<bool> * TreeWatcher = new QFutureWatcher<bool>();
	connect(TreeWatcher, SIGNAL(finished()), this, SLOT(onTreeUpdateFinished()));
	QFuture<bool> TreeFuture = QtConcurrent::run(_pAppController, &GTAAppController::updateWorkDB);
	TreeWatcher->setFuture(TreeFuture);
}

/* This SLOT allow to import file in the current TreeView (DATA/)
* @return: none
*/
void GTAMainWindow::onImportFile()
{
	if (_pAppController)
	{
		QMessageBox messageBox;
		messageBox.setWindowTitle("Import files/folder");
		messageBox.setText("If you want to import one or more file click File button. \nIf you want to import a folder click the Folder button.");

		QPushButton *folderButton =  messageBox.addButton("Import folder", QMessageBox::ActionRole);
		QPushButton *fileButton =  messageBox.addButton("Import file(s)", QMessageBox::ActionRole);

		messageBox.setStandardButtons(QMessageBox::Cancel);

		messageBox.exec();
		if (messageBox.clickedButton() == folderButton)
		{
			QFileDialog dialog;
			dialog.setFileMode(QFileDialog::Directory);
			QString folder = dialog.getExistingDirectory(this, tr("Select the folder to import"), "C:/");
			
			if (!folder.isEmpty())
			{
				QMessageBox::warning(this, "Warning", "Folder to import :" + folder, QMessageBox::Ok); 
				disconnect(&_FileWatcher, SIGNAL(directoryChanged(QString)),this,SLOT(onDirectoryChanged(QString)));
				_pAppController->importFolderToWorkDB(folder);
				refreshTreeView();
				
			}
			else
			{
				// FilePath will be sent to the database to add it
				QMessageBox::warning(this, "Warning", "No folder is selected, import canceled!", QMessageBox::Ok);
			}
		}
		else if (messageBox.clickedButton() == fileButton)
		{
			QFileDialog dialog;
			dialog.setFileMode(QFileDialog::AnyFile);
			QStringList files = dialog.getOpenFileNames(this, tr("Select one or more files to import"), "C:/", tr("GTA Files (*.obj *.fun *.pro *.seq)"));
			if (files.size() == 0)
			{
				// FilePath will be sent to the database to add it
				QMessageBox::warning(this, "Warning", "No files selected", QMessageBox::Ok);
			}
			else if (files.size() == 1)
			{
				QFileInfo fileInformation(files[0]);
				if (!fileInformation.exists())
				{
					return;
				}
				else
				{
					// FilePath will be sent to the database to add it
					QMessageBox::warning(this, "Warning", "File to be imported:"+ files[0], QMessageBox::Ok);
					disconnect(&_FileWatcher, SIGNAL(directoryChanged(QString)),this,SLOT(onDirectoryChanged(QString)));
					_pAppController->importFilesToWorkDB(files);
					refreshTreeView();
				}
			}
			else
			{
				// FilePath will be sent to the database to add it
				QMessageBox::warning(this, "Warning", "Files to be imported:" + QString(files.join(",<br>")), QMessageBox::Ok);
				disconnect(&_FileWatcher, SIGNAL(directoryChanged(QString)),this,SLOT(onDirectoryChanged(QString)));
				_pAppController->importFilesToWorkDB(files);
				refreshTreeView();
			}
		}
		// By default, we use the root of the disk
	}
}

/* This SLOT allow to update the DATA database (DATA/)
* @return: none
*/
void GTAMainWindow::onUpdateDataDB()
{
	if (_pAppController)
	{
		disconnect(&_FileWatcher, SIGNAL(directoryChanged(QString)),this,SLOT(onDirectoryChanged(QString)));

		showTreeProgressBar();
		ui->DataTV->setModel(nullptr);
		_pAppController->getViewController()->updateTreeBrowserModel();
		QFutureWatcher<bool> * TreeWatcher = new QFutureWatcher<bool>();
		connect(TreeWatcher, SIGNAL(finished()), this, SLOT(onTreeUpdateFinished()));
        QFuture<bool> TreeFuture = QtConcurrent::run(_pAppController, &GTAAppController::updateTreeStructure, false, false);
		TreeWatcher->setFuture(TreeFuture);
	}
}

/* This SLOT allow to create the DATA database (DATA/)
* @return: none
*/
void GTAMainWindow::onCreateDataDB(bool message)
{
	if (_pAppController)
	{   
        if (message)
        {
            QString infoMsg = "Do you want to rebuild the DATA Database?\nThis operation may take some time.";
            QMessageBox::StandardButton reply = QMessageBox::question(this, "Create DATA Database", infoMsg, QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::No)
                return;
        }
		
		disconnect(&_FileWatcher, SIGNAL(directoryChanged(QString)),this,SLOT(onDirectoryChanged(QString)));

		showTreeProgressBar();
		ui->DataTV->setModel(nullptr);
		_pAppController->getViewController()->updateTreeBrowserModel();
		QFutureWatcher<bool> * TreeWatcher = new QFutureWatcher<bool>();
		connect(TreeWatcher, SIGNAL(finished()), this, SLOT(onTreeUpdateFinished()));
           QFuture<bool> TreeFuture = QtConcurrent::run(_pAppController, &GTAAppController::updateTreeStructure, true, false);
		TreeWatcher->setFuture(TreeFuture);
	}
}

void GTAMainWindow::modifyFilePath(QString dataPath, QString repoPath)
{
	// Update DB and Workspace paths (QLabel)
	ui->DBpathLB->clear();
	if (!dataPath.isEmpty())
	{
		ui->DBpathLB->setVisible(true);
        dataPath.replace(QString("\\"), QString("/"));
        dataPath.remove(0, dataPath.lastIndexOf(QChar('/')) + 1);
        ui->DBpathLB->setText("<b>Bench DB :</b>\t" + dataPath);
	}
	else
	{
		ui->DBpathLB->setVisible(false);
	}
	ui->WKpathLB->clear();
	if (!repoPath.isEmpty())
	{
        ui->WKpathLB->setVisible(true);
        repoPath.replace(QString("\\"), QString("/"));
        repoPath.remove(0, repoPath.lastIndexOf(QChar('/')) + 1);
        ui->WKpathLB->setText("<b>Workspace :</b>\t" + repoPath);
	}
	else
	{
		ui->WKpathLB->setVisible(false);
	}
}

/**
 * @brief Grey out git toolbar icons to force re-init in case of changed path
*/
void GTAMainWindow::onGitDbChanged() {
    _pGitFolder->setEnabled(false);
    _pGitStatus->setEnabled(false);
}

void GTAMainWindow::onImportAlreadyExists(QHash <QString, QString> duplicates)
{
	QHash <QString, QString> duplicatesToReplace;
	if (_pAppController)
	{
		QHash <QString, QString>::iterator i,j;
		j = duplicates.end();
		for (i = duplicates.begin(); i != duplicates.end(); i++)
		{
			QMessageBox msgBox;
			msgBox.setText("The following file already exists in the DATA folder, overwrite it ? \n");
			msgBox.setInformativeText(i.value());
			QAbstractButton *yesButton = msgBox.addButton("Yes", QMessageBox::YesRole);
			msgBox.addButton("No", QMessageBox::NoRole);
			QAbstractButton *yesAllButton = msgBox.addButton("Yes for All", QMessageBox::YesRole);
			QAbstractButton *noAllButton = msgBox.addButton("No for All", QMessageBox::NoRole);
			msgBox.setIcon(QMessageBox::Question);
			msgBox.exec();

			if (msgBox.clickedButton() == yesButton)
			{
				duplicatesToReplace.insert(i.key(), i.value());
			}
			else if (msgBox.clickedButton() == yesAllButton)
			{
				j = i;
				break;
			}
			else if (msgBox.clickedButton() == noAllButton)
			{
				break;
			}
		}
		while (j != duplicates.end())
		{
			duplicatesToReplace.insert(j.key(), j.value());
			j++;
		}
		if (!duplicatesToReplace.isEmpty())
			_pAppController->deleteAndImport(duplicatesToReplace);
	}
	
}

/**
* @brief Detects whether there is a duplicate file ith respect to its UUID
* @param oldPath the old file with the same UUID
* @param newPath the new file with the same UUID
* @return void
*/
void GTAMainWindow::onDuplicateUUIDFound(QString newPath, QString oldPath)
{
	QMessageBox msgBox;
	msgBox.setText("Duplicate UUID detected !\n");
	msgBox.setInformativeText("Old file: " + oldPath + "\n - - - - -  "+ "\n New file: " + newPath + "\n - - - - -  " + "\n Which file do you want to delete ?");
	QAbstractButton *oldButton = msgBox.addButton("Old File", QMessageBox::YesRole);
	QAbstractButton *newButton = msgBox.addButton("New File", QMessageBox::NoRole);
    QAbstractButton *cancelButton = msgBox.addButton("Cancel", QMessageBox::NoRole);
    msgBox.setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
	msgBox.setIcon(QMessageBox::Question);
	msgBox.exec();
	if (msgBox.clickedButton() == oldButton)
	{
		_pAppController->deleteFile(oldPath);
	}
	else if (msgBox.clickedButton() == newButton)
	{
		_pAppController->deleteFile(newPath);
	}
    else if (msgBox.clickedButton() == cancelButton)
    {
        return;
    }
	disconnect(&_FileWatcher, SIGNAL(directoryChanged(QString)),this,SLOT(onDirectoryChanged(QString)));
   
	showTreeProgressBar();
	ui->DataTV->setModel(nullptr);
	_pAppController->getViewController()->updateTreeBrowserModel();
	QFutureWatcher<bool> * TreeWatcher = new QFutureWatcher<bool>();
	connect(TreeWatcher, SIGNAL(finished()), this, SLOT(onTreeUpdateFinished()));
	QFuture<bool> TreeFuture = QtConcurrent::run(_pAppController, &GTAAppController::updateWorkDB);
	TreeWatcher->setFuture(TreeFuture);
    
}

void GTAMainWindow::onUpdateTreeItem()
{
	disconnect(&_FileWatcher, SIGNAL(directoryChanged(QString)),this,SLOT(onDirectoryChanged(QString)));

	showTreeProgressBar();
	ui->DataTV->setModel(nullptr);
	_pAppController->getViewController()->updateTreeBrowserModel();
	QFutureWatcher<bool> * TreeWatcher = new QFutureWatcher<bool>();
	connect(TreeWatcher, SIGNAL(finished()), this, SLOT(onTreeUpdateFinished()));
	QFuture<bool> TreeFuture = QtConcurrent::run(_pAppController, &GTAAppController::updateWorkDB);
	TreeWatcher->setFuture(TreeFuture);
}

void GTAMainWindow::onOpenContainingFolderFromTree()
{
    QModelIndexList selectedIndexs = ui->DataTV->selectionModel()->selectedRows(0);
    if (!selectedIndexs.isEmpty())
    {
        QModelIndex index = selectedIndexs.at(0);
        if (index.isValid())
        {
            const QString fileName = _pAppController->getFilePathFromTreeModel(index, false).trimmed();
            const QString fullFilePath = _pAppController->getGTADataDirectory() + fileName;
            const QString containingFolderPath = fullFilePath.left(fullFilePath.lastIndexOf("/"));
            
            if (QFileInfo(containingFolderPath).exists()) {
                QDesktopServices::openUrl(QUrl::fromLocalFile(containingFolderPath));
            }
            else {
                QMessageBox::critical(this, "Containing folder does not exist", QString("Checked path: ") +
                    containingFolderPath);
            }
        }
    }
}

void GTAMainWindow::onCommitChangesToGit()
{
    auto gitController = GtaGitController::getGtaGitController();
    const QString repoDirectory = QString().fromStdString(gitController->getRepoPath());
    QStringList argument;
    argument << "/command:commit" << "/path:" + repoDirectory;

    QProcess pProcess;
    const bool success = pProcess.startDetached("TortoiseGitProc", argument);
    if (!success)
    {
        QMessageBox::critical(nullptr, "TortoiseGit Error", "Error : This action cannot be performed.\nPlease check if TortoiseGit is installed successfully");
    }
}

void GTAMainWindow::onDiffFiles()
{
    QModelIndexList indexList = ui->DataTV->selectionModel()->selectedRows(0);
    QStringList argument;
    QString ActionCmd;

    if (indexList.count() == 1)
    {
        ActionCmd = "TortoiseGitProc";
        const QModelIndex modelIndex = indexList.at(0);
        const QString treeModelPath = _pAppController->getFilePathFromTreeModel(modelIndex);
        const QString gtaDataDirectory = _pAppController->getGTADataDirectory();
        const QString fullFilePath = gtaDataDirectory + treeModelPath;
        argument << "/command:diff" << "/path:" + fullFilePath;
    }
    else if (indexList.count() == 2)
    {
        ActionCmd = "TortoiseGitMerge";
        for (int index = 0; index < 2; index++)
        {
            const QModelIndex modelIndex = indexList.at(index);
            const QString treeModelPath = _pAppController->getFilePathFromTreeModel(modelIndex);
            const QString gtaDataDirectory = _pAppController->getGTADataDirectory();
            const QString fullFilePath = gtaDataDirectory + treeModelPath;
            argument << fullFilePath;
        }
    }

	if (!ActionCmd.isEmpty())
	{
        QProcess pProcess;
        const bool success = pProcess.startDetached(ActionCmd, argument);
        if (!success)
        {
            QMessageBox::critical(nullptr, "TortoiseGit Error", "Error : This action cannot be performed.\nPlease check if TortoiseGit is installed successfully");
        }
	}
}

void GTAMainWindow::onAddFileForCommit()
{
    QModelIndexList indexList = ui->DataTV->selectionModel()->selectedRows(0);
    const QModelIndex modelIndex = indexList.at(0);
    const QString treeModelPath = _pAppController->getFilePathFromTreeModel(modelIndex);
    const QString gtaDataDirectory = _pAppController->getGTADataDirectory();
    const QString fullFilePath = gtaDataDirectory + treeModelPath;

    QStringList argument;
    argument << "/command:add" << "/path:" + fullFilePath;
  
    QProcess pProcess;
    const bool success = pProcess.startDetached("TortoiseGitProc", argument);
    if (!success)
    {
        QMessageBox::critical(nullptr, "TortoiseGit Error", "Error : This action cannot be performed.\nPlease check if TortoiseGit is installed successfully");
    }
}

void GTAMainWindow::onRemoveFileFromGitWorkspace()
{
    QModelIndexList indexList = ui->DataTV->selectionModel()->selectedRows(0);
    const QModelIndex modelIndex = indexList.at(0);
    const QString treeModelPath = _pAppController->getFilePathFromTreeModel(modelIndex);
    const QString gtaDataDirectory = _pAppController->getGTADataDirectory();
    QString fullFilePath = gtaDataDirectory + treeModelPath;

    auto gitCtrl = GtaGitController::getGtaGitController();

    QString gitPath = QString().fromStdString(gitCtrl->getRepoPath());

    QString gitRelativePath = fullFilePath.replace(gitPath, "");
    if (gitRelativePath[0] == QChar('/')) {
        gitRelativePath.remove(0, 1);
    }

    auto error = gitCtrl->removeFileFromIndex(gitRelativePath.toStdString());
    if (error) {
        QMessageBox::critical(nullptr, "Git Error", QString().fromStdString(error.value()));
    }
}

/**
* @brief : Add the list of selected files in the execution list of Procedure dashboard launcher and 
           displays the dialog if not already open.
*/
void GTAMainWindow::onAddToExecutionList()
{
    const QString gtaDataDirectory = _pAppController->getGTADataDirectory();
    QModelIndexList indexList = ui->DataTV->selectionModel()->selectedRows(0);

    _pGTAControllerLauncherWidget->setDataDirectoryPath(gtaDataDirectory);
    _pGTAControllerLauncherWidget->InitialiseAdditionalTimeValues();

    foreach(QModelIndex modelIndex, indexList)
    {
        const QString treeModelPath = _pAppController->getFilePathFromTreeModel(modelIndex);
        const QString fullFilePath = gtaDataDirectory + treeModelPath;

        _pGTAControllerLauncherWidget->AddNewFileToExecutionList(fullFilePath);
    }

    _pGTAControllerLauncherWidget->show();
}

void GTAMainWindow::UpdateProcedureDashBoardMenuItem(QModelIndexList indexList)
{
    if (_pAddToExecutionList)
    {
        bool listHasOnlyPROFile = true;
        foreach(QModelIndex index, indexList)
        {
            QString extension = _pAppController->getExtensionFromTreeModel(index);

            if (extension != "pro")
            {
                listHasOnlyPROFile = false;
                break;
            }
        }

        QString GTAControllerExe = GTASystemServices::getSystemServices()->getGTAControllerFilePath();
        QString autoLauncherDir = GTASystemServices::getSystemServices()->getAutoLauncherWatchDirectory();
        QString autoLauncherConfig = GTASystemServices::getSystemServices()->getAutoLauncherConfigPath();

        QFile fileGtaController(GTAControllerExe);
        QFile fileAutoLauncher(autoLauncherDir);
        QFile fileAutoLauncherConfig(autoLauncherConfig);

        if (listHasOnlyPROFile && fileGtaController.exists() && fileAutoLauncher.exists() && fileAutoLauncherConfig.exists())
        {
            _pAddToExecutionList->setEnabled(true);
        }
        else
        {
            _pAddToExecutionList->setEnabled(false);
        }
    }
}

void GTAMainWindow::UpdateGitActionMenuItems(QModelIndex index)
{
	if (_pCommitChangesToGit && _pAddFileToCommit && _pDiffFiles && _pRemoveFileFromGitWorkspace)
	{
		_pCommitChangesToGit->setVisible(false);
		_pAddFileToCommit->setVisible(false);
		_pDiffFiles->setVisible(false);
		_pRemoveFileFromGitWorkspace->setVisible(false);

		const QString extension = _pAppController->getExtensionFromTreeModel(index);
		if (extension != "folder")
		{
			const QString fileName = _pAppController->getFilePathFromTreeModel(index, false).trimmed();
			const QString gtaDataDirectory = _pAppController->getGTADataDirectory();
			QString fullFilePath = gtaDataDirectory + fileName;

			auto gitController = GtaGitController::getGtaGitController();
			const QString repoDirectory = QString().fromStdString(gitController->getRepoPath());
			fullFilePath.replace(repoDirectory + "/", "");
			auto status = gitController->CheckFileStatus(fullFilePath.toStdString());

			if (_pGitStatus && _pGitStatus->isEnabled())
			{
				QModelIndexList indexList = ui->DataTV->selectionModel()->selectedRows(0);
				if ((indexList.count() == 1 && (status != GitStatus::IndexNew && status != GitStatus::WtNew))
					|| indexList.count() == 2)
				{
					_pDiffFiles->setVisible(true);
				}

				if (status == GitStatus::WtModified || status == GitStatus::IndexNew)
				{
					_pCommitChangesToGit->setVisible(true);
				}

				if (indexList.count() == 1 && status == GitStatus::WtNew)
				{
					_pAddFileToCommit->setVisible(true);
				}

				if (indexList.count() == 1 && status != GitStatus::WtNew)
				{
					_pRemoveFileFromGitWorkspace->setVisible(true);
				}
			}
		}
	}
}

/**
 * @brief Perform the move of files from both menu selection and drag&drop
 * @param selectedFiles the selected QModelIndex files in the tree view
 * @param dir the target directory
*/
void GTAMainWindow::moveFiles(QModelIndexList selectedFiles, QString dir)
{   
    QString DataDirPath = _pAppController->getGTADataDirectory();

    // inspect selected items
    QList<GTATreeItem*> selectedItems;
    QStringList loadedFiles;
    QList<TabIndices> previousFilePos;
    
    for (const auto& itemIndex : selectedFiles)
    {     
        GTATreeItem* itemNode = _pAppController->getTreeNodeFromIndex(itemIndex);
        QString itemAbsPath = DataDirPath + itemNode->getAbsoluteRelPath();
        if (itemAbsPath == dir) // do not put the target... inside the target!
           continue;

        QString fileName = itemNode->getName();
        QString itemRootPath = DataDirPath + itemNode->getAbsoluteRelPath().replace("/" + fileName, "");
        if (itemRootPath == dir) // no move when same folder
            continue;
        

        // check if the item is opened in Editor View                
        QString fileUUID = itemNode->getUUID();
        if (isFileLoaded(fileUUID, true))
        {
            QMessageBox::StandardButton button = QMessageBox::information(this, "Move Warning", "File " + fileName + " is already loaded.\n\nPress Ok to automatically close the element \nbefore the move.", QMessageBox::Ok, QMessageBox::Cancel);
            if (button == QMessageBox::Cancel)
            {
                continue;
            }                
            else if (button == QMessageBox::Ok)
            {
                QString itemRelativePath = itemNode->getAbsoluteRelPath().replace(DataDirPath, "");
                GTAEditorWindow* pWindow = getLoadedWindow(itemRelativePath);
                TabIndices windowPos = pWindow->getFramePosition();

                // Store uuid of loaded file to be moved
                loadedFiles.append(fileUUID);
                previousFilePos.append(windowPos);
                
                // Close the file              
                ui->EditorFrame->closeTab(*windowPos.viewIndex, *windowPos.tabIndex);
                QTabWidget* currentTabWidget = ui->EditorFrame->getTabWidget(*windowPos.viewIndex);
                if (ui->EditorFrame->getViewsCount() > 1 && currentTabWidget->count() == 0)
                {
                    ui->EditorFrame->closeView(*windowPos.viewIndex);
                    ui->EditorFrame->updateWindowsPosition(0, -1);
                }
                else
                {
                    ui->EditorFrame->updateWindowsPosition(*windowPos.viewIndex, *windowPos.tabIndex);
                }                    
                updateMoveUI();
            }
        }
        selectedItems.append(itemNode);
    }
    if (selectedItems.isEmpty())
    {
        qWarning() << "No items selected.";
        return;
    }
        
    // perform the move
    onDisconnectFileWatcher();
    showTreeProgressBar();
    _pAppController->moveFiles(selectedItems, dir);
    onTreeUpdateFinished_(true);
    updateDataBrowserModel();

    // Reload the files
    for (int i = 0; i < loadedFiles.count(); i++)
    {
        QString uuid = loadedFiles.at(i);
        onOpenInNewWindow(uuid, true);

        // Set to last position 
        TabIndices previousPos = previousFilePos.at(i);
        QString relPath = _pAppController->getAbsoluteFilePathFromUUID(uuid).replace(DataDirPath, "");      
        QString fileName = relPath.split('/').last();
        GTAEditorWindow* pSubWindow = getLoadedWindow(relPath);       
        bool moveToOtherView = previousPos.viewIndex != 0 && ui->EditorFrame->getViewsCount() == 1;
        TabIndices insertPos = moveToOtherView ? TabIndices(0, -1) : previousPos;
        DraggableTabWidget* targetTab = ui->EditorFrame->getTabWidget(*insertPos.viewIndex);
        targetTab->insertTab(*insertPos.tabIndex, pSubWindow, fileName);
        ui->EditorFrame->updateWindowsPosition(*insertPos.viewIndex, *insertPos.tabIndex);
        if (moveToOtherView) {
            _pMoveView->trigger();
        }
    }
}

/**
* @brief Performs the moving files/folders processes
* @param None
* @return void
*/
void GTAMainWindow::onMoveFiles()
{
    onDisconnectFileWatcher();
	QModelIndexList selectedIndexes = ui->DataTV->selectionModel()->selectedRows(0);
	if (!selectedIndexes.isEmpty())
	{
		QList<GTATreeItem*> selectedItems;
		foreach(QModelIndex index, selectedIndexes)
		{
			selectedItems.append(_pAppController->getTreeNodeFromIndex(index));
		}
		QString DataDirPath = _pAppController->getGTADataDirectory();
		QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"), DataDirPath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
		if (dir.isEmpty() || dir.isNull()) {
			return;
		}
        if (!dir.contains(DataDirPath)){            
            QMessageBox::warning(this, "Target warning", "Target path should not be outside DATA directory :  \n" + DataDirPath);
            return;
        }            
        moveFiles(selectedIndexes, dir);
	}
}

/**
* @brief Slot triggered by the dropping of items inside the tree view. Behaves according to the moving of items within folders.
* @param selectedFiles Items that are dragged and dropped within the tree view
* @param target The target item in which we drop the selected items
*/
void GTAMainWindow::onDroppedFiles(QModelIndexList selectedFiles, QModelIndex target)
{
    // inspect target and move items if it's a folder
    QString DataDirPath = _pAppController->getGTADataDirectory();
    GTATreeItem* targetItem = _pAppController->getTreeNodeFromIndex(target);

    // just check if target is the root and if it's a directory
    bool isRoot = target.row() == -1 && target.column() == -1;
    QString targetPath = isRoot ? DataDirPath : DataDirPath + targetItem->getAbsoluteRelPath();
    if (!QDir(targetPath).exists())
    {
        qWarning() << "Target path does not exist.";
        return;
    }
    moveFiles(selectedFiles, targetPath);   
}

/**
* @brief Add a new file (.pro, .seq, .fun, .obj) to the tree view, folders and database
* * @param mode is dialog mode enum type
*/
void GTAMainWindow::onAddFile(GTANewFileCreationDialog::DialogMode mode, const QString& fileName, const QString& authorName, const QString& fileType)
{
    if (_pAppController)
    {
        QString rootPath = "";
        QModelIndexList selectedIndices = ui->DataTV->selectionModel()->selectedRows(0);
        if (selectedIndices.count() == 1)
        {
            QModelIndex index = selectedIndices[0];
            GTATreeItem* treeItem = _pAppController->getTreeNodeFromIndex(index);
            if (treeItem->getExtension() == "folder")
                rootPath = treeItem->getAbsoluteRelPath();
            else
                rootPath = treeItem->getParent()->getAbsoluteRelPath();
        }
        if (rootPath == "root")
            rootPath = "";
        QString dataDirPath = _pAppController->getGTADataDirectory() + (rootPath == "" ? "" : "/" + rootPath);

        if (_pNewFileDialog)
        {
            if (_customPath.isEmpty()) {
                _pNewFileDialog->setDataDirPath(dataDirPath);
            }
            else {
                _pNewFileDialog->setDataDirPath(_customPath);
            }
            _pNewFileDialog->setWindowTitle("Create New File");
            _pNewFileDialog->setMode(mode);
            if(fileName.isEmpty() || authorName.isEmpty())
            {
                _pNewFileDialog->show();
            }
            else if(!fileName.isEmpty() && !authorName.isEmpty())
            {
                if (_customFileName.isEmpty()) {
                    _pNewFileDialog->setFileName(fileName);
                }
                else {
                    _pNewFileDialog->setFileName(_customFileName);
                }
                _pNewFileDialog->setAuthorName(authorName);
                _pNewFileDialog->setFileTypes();
                _pNewFileDialog->setFileType(fileType);
                _pNewFileDialog->setFilePath();
                _pNewFileDialog->clickOk();
            }               
        }
    }

    _customFileName.clear();
    _customPath.clear();
}

/**
* @brief Adds a folder to the tree, db and DATA
*/
void GTAMainWindow::onAddFolder()
{
    bool rc;
    QString newFolderName = QInputDialog::getText(this, tr("New folder"),
        tr("Folder name: "), QLineEdit::Normal,
        "", &rc, Qt::MSWindowsFixedSizeDialogHint);
    
    bool emptyStr = newFolderName.trimmed().isEmpty();
    if (rc && emptyStr)
    {
        QMessageBox::critical(this, "Empty name!", "Folder name cannot be empty.");
        onAddFolder();
        return;
    }
    if (rc && !emptyStr)
    {   
        onDisconnectFileWatcher();

        QString rootPath = "";
        QModelIndexList selectedIndices = ui->DataTV->selectionModel()->selectedRows(0);
        if (selectedIndices.count() == 1)
        {
            QModelIndex index = selectedIndices[0];
            GTATreeItem* treeItem = _pAppController->getTreeNodeFromIndex(index);
            if (treeItem->getExtension() == "folder")
                rootPath = treeItem->getAbsoluteRelPath();
            else
                rootPath = treeItem->getParent()->getAbsoluteRelPath();
        }
        if (rootPath == "root")
            rootPath = "";
        QString dataDirPath = _pAppController->getGTADataDirectory() + "/" + rootPath;
        QDir(dataDirPath).mkdir(newFolderName);

        onRefreshTree();
        connectFoldersToWatcher();
    }
}

/**
* @brief Finds the current directory of a file
* @param path_ path to analyze
* @return QString the directory
*/
inline QString find_directory(QString path_)
{
    std::string path = path_.toStdString();
    std::string delimiter1 = "/";
    std::string delimiter2 = "\\";
    size_t pos = 0;
    std::string token;
    std::string res = "";
    while ((pos = path.find(delimiter1)) != std::string::npos || (pos = path.find(delimiter2)) != std::string::npos)
    {
        token = path.substr(0, pos);
        path.erase(0, pos + 1);
        res.append(token);
        res.append("/");
    }
    return QString::fromStdString(res);
}

/**
* @brief Initializes the ui with respect to the bench configuration file and load the configuration file
* @param None
* @return bool validity true if the loading was performed correctly
*/
bool GTAMainWindow::initializeConfiguration()
{
    // set ui details
    ui->mutable_label_current_ini_folder->setText(_current_bench_config_file_folder);
    QStringList ini_file_list = QDir(_current_bench_config_file_folder).entryList(QStringList() << "*.ini");
    ui->CB_ini_files->clear();
    foreach(QString filename, ini_file_list)
        ui->CB_ini_files->addItem(filename);
    ui->CB_ini_files->setCurrentIndex(0);

    TestConfig* testConf = TestConfig::getInstance();
    QString inifullpath = _current_bench_config_file_folder + _current_bench_config_file_name;
    bool validity = testConf->loadConf(inifullpath);
    
    return validity;
}

/**
* @brief Updates the UI and the TestConfig based on UI modifications (combo box or folder).
* @param None
* @return void
*/
void GTAMainWindow::updateIniFile()
{
    QString new_path = ui->mutable_label_current_ini_folder->text() + ui->CB_ini_files->currentText();

    TestConfig* testConf = TestConfig::getInstance();
    auto oldLibPath = testConf->getLibraryPath();
    auto oldRepoPath = testConf->getRepositoryPath();
    auto oldToolDataPath = testConf->getToolDataPath();
    bool validity = testConf->loadConf(new_path);

    if (!validity)
    {
        GTASystemServices* pSysService = _pAppController->getSystemService();
        if (pSysService != nullptr)
        {
            _pSettingWidget->setDefaultLogDirPath(pSysService->getDefaultLogDirectory());
        }
        _pSettingWidget->show();
        QMessageBox::critical(_pSettingWidget, "Settings error", "Settings are not valid.");
        _pSettingWidget->loadSettings();
        _pSettingWidget->setInitialSettingFlag(false);
    }
    else
    {
        _current_bench_config_file_folder = ui->mutable_label_current_ini_folder->text();
        _current_bench_config_file_name = ui->CB_ini_files->currentText();

        int default_index = ui->CB_ini_files->findText(_current_bench_config_file_name);
        ui->CB_ini_files->setCurrentIndex(default_index);
        ui->mutable_label_current_ini_folder->setText(_current_bench_config_file_folder);

        auto newLibPath      = testConf->getLibraryPath();
        auto newRepoPath     = testConf->getRepositoryPath();
        auto newToolDataPath = testConf->getToolDataPath();

        bool isLibPathChanged      = !(oldLibPath == newLibPath);
        bool isRepoPathChanged     = !(oldRepoPath == newRepoPath);
        bool isToolDataPathChanged = !(oldToolDataPath == newToolDataPath);

        loadAndupdateConfiguration(isLibPathChanged, isRepoPathChanged, isToolDataPathChanged, false);
    }
}

/**
* @brief Refreshes the interface by updating the widgets
* @param None
* @return void
*/
void GTAMainWindow::refresh_ui()
{
    QStringList ini_file_list = QDir(_current_bench_config_file_folder).entryList(QStringList() << "*.ini");
    ui->CB_ini_files->clear();
    foreach(QString filename, ini_file_list)
        ui->CB_ini_files->addItem(filename);
    int default_index = ui->CB_ini_files->findText(_current_bench_config_file_name);
    ui->CB_ini_files->setCurrentIndex(default_index);
    ui->mutable_label_current_ini_folder->setText(_current_bench_config_file_folder);

    modifyFilePath(QString::fromStdString(TestConfig::getInstance()->getLibraryPath()),
        QString::fromStdString(TestConfig::getInstance()->getRepositoryPath()));
}

/**
* @brief Updates the UI and the TestConfig when the cancel button of the settings widget is clicked.
* @param None
* @return void
*/
void GTAMainWindow::on_cancel_settings()
{
    TestConfig* testConf = TestConfig::getInstance();

    QStringList ini_file_list = QDir(_current_bench_config_file_folder).entryList(QStringList() << "*.ini");
    ui->CB_ini_files->clear();
    foreach(QString filename, ini_file_list)
        ui->CB_ini_files->addItem(filename);
    int default_index = ui->CB_ini_files->findText(_current_bench_config_file_name);
    ui->CB_ini_files->setCurrentIndex(default_index);
    ui->mutable_label_current_ini_folder->setText(_current_bench_config_file_folder);

    testConf->loadConf(_current_bench_config_file_folder + _current_bench_config_file_name);
    //TODO Add error handling for loadConf
    modifyFilePath(QString::fromStdString(testConf->getLibraryPath()),
        QString::fromStdString(testConf->getRepositoryPath()));
}
