#include "GTAEditorWindow.h"
#include "ui_GTAEditorWindow.h"
#include "GTAUtil.h"
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrentRun>
#include <QMessageBox>
#include <QShortcut>
#include "GTAAppController.h"
#include "GTAMdiController.h"
#include "GTAUndoRedoAddCmd.h"
#include "GTAUndoRedoAddRow.h"
#include "GTAUndoRedoClearRow.h"
#include "GTAUndoRedoCollapseToTitle.h"
#include "GTAUndoRedoCut.h"
#include "GTAUndoRedoDeleteRow.h"
#include "GTAUndoRedoHideRow.h"
#include "GTAUndoRedoIgnoreOnOff.h"
#include "GTAUndoRedoMoveRowsDown.h"
#include "GTAUndoRedoMoveRowsUp.h"
#include "GTAUndoRedoPaste.h"
#include "GTAUndoRedoShowAllRows.h"
#include "GTAUndoRedoToggleReadOnly.h"
#include "GTAUndoRedoClearAll.h"
#include "GTAUndoRedoEditMTE.h"
#include "GTAElementTreeViewWidget.h"
#include "CustomizedCommandInterface.h"
#include "GTAPurposeAndConclusionDialog.h"
#include <QTimer>
#include <QDir>
#include <GTAAppController.h>
#include "GTAHeaderWidget.h"
#include <QFileDialog>
#include <QMutableListIterator>
#include "GTADebugger.h"
#include <QThread>
#include <QSpinBox>
#include <QWidgetAction>


#define BREAKPOINT_COL_WIDTH 25


GTAEditorWindow::GTAEditorWindow(QWidget* parent, const QString& iFileName) :
	QWidget(parent),
	ui(new Ui::GTAEditorWindow)
{
	ui->setupUi(this);

	_isJump = false;
	_isGlobalSave = false;
	_globalSaveStatus = 0;
	_contextOnJump = GTAAppController::EDITING;
	_CurrentExecutionLine = -1;

	_pProgressBar = new GTAProgress(nullptr);


	setFocusPolicy(Qt::StrongFocus);
	ui->EditorTV->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->EditorTV->setDragEnabled(true);
	ui->EditorTV->setAcceptDrops(true);
	ui->EditorTV->setDropIndicatorShown(true);

	initMembers();
	lastCalled = "copy";
	_lastindex = 0;

	//debugger pointer nullptr assignment
	_pDebugger = nullptr;
	_pDebuggerThread = nullptr;
	_action = GTADebugger::kNone;
	_pExecutingCmd = nullptr;
	_enableNextPreviousContinue = false;

	_pEditorController = new GTAEditorViewController();
	_pMDIController = GTAMdiController::getMDIController();

	QString path;
	_pMDIController->onGetGTADataDirectory(path);
	_pEditorController->setGTADirPath(path);

	connect(this, SIGNAL(rowCopiedToClipBoard()), _pMDIController, SIGNAL(updateClipBoard()));
	connect(this, SIGNAL(disableEditingActionsInMain(bool)), _pMDIController, SIGNAL(updateEditingActions(bool)));
	connect(this, SIGNAL(toggleEditorActionsinMain(bool)), _pMDIController, SIGNAL(toggleEditorActions(bool)));
	connect(this, SIGNAL(toggleTitleCollapseActionIcon(bool)), _pMDIController, SIGNAL(updateTitleCollapseActionIcon(bool)));
	connect(this, SIGNAL(collapseToTitle(bool)), _pMDIController, SIGNAL(updateCollapseToTitle(bool)));
	connect(this, SIGNAL(updateMoveDownAction(bool)), _pMDIController, SIGNAL(updateMoveDownAction(bool)));
	connect(this, SIGNAL(updateMoveUpAction(bool)), _pMDIController, SIGNAL(updateMoveUpAction(bool)));
	connect(this, SIGNAL(updateReadOnlyDocStatus(bool)), _pMDIController, SIGNAL(refreshReadOnlyDocStatus(bool)));
	connect(this, SIGNAL(updateActiveStack(QUndoStack*&)), _pMDIController, SIGNAL(updateActiveStack(QUndoStack*&)));
	connect(this, SIGNAL(removeStack(QUndoStack*&)), _pMDIController, SIGNAL(removeStack(QUndoStack*&)));
	connect(this, SIGNAL(updateErrorLog(ErrorMessageList)), _pMDIController, SIGNAL(updateErrorLog(ErrorMessageList)));
	connect(this, SIGNAL(updateErrorLogMsg(QString)), _pMDIController, SIGNAL(updateErrorLogMsg(QString)));
	connect(this, SIGNAL(elementCreatedForJump(GTAAppController::DisplayContext)), _pMDIController, SIGNAL(elementCreatedForJump(GTAAppController::DisplayContext)));
	connect(this, SIGNAL(removeWindow()), _pMDIController, SIGNAL(removeWindow()));
	connect(this, SIGNAL(updateClearAllIcon(bool)), _pMDIController, SIGNAL(updateClearAllIcon(bool)));
	connect(this, SIGNAL(saveEditorDocFinished(QString)), _pMDIController, SIGNAL(saveEditorDocFinished(QString)));
	connect(this, SIGNAL(openInNewWindow(QString, bool)), _pMDIController, SIGNAL(openInNewWindow(QString, bool)));
	connect(this, SIGNAL(ShowProgressBar(QString)), _pMDIController, SIGNAL(ShowProgressBar(QString)));
	connect(this, SIGNAL(HideProgressBar()), _pMDIController, SIGNAL(HideProgressBar()));
	connect(this, SIGNAL(toggleLaunchButton(bool)), _pMDIController, SIGNAL(toggleLaunchButton(bool)));
	connect(this, SIGNAL(updateOutputWindow(QStringList)), _pMDIController, SIGNAL(updateOutputWindow(QStringList)));

	//connections between mdi controller and editor view controller
	connect(_pEditorController, SIGNAL(clearClipBoardItems()), _pMDIController, SLOT(onClearClipBoardItems()));
	connect(_pEditorController, SIGNAL(callActionDropped(int&, QString&)), _pMDIController, SIGNAL(sigCallActionDropped(int&, QString&)));
	connect(_pEditorController, SIGNAL(getHeaderTemplateFilePath(QString&)), _pMDIController, SLOT(onGetHeaderTemplateFilePath(QString&)));
	connect(_pEditorController, SIGNAL(getGTADataDirectory(QString&)), _pMDIController, SLOT(onGetGTADataDirectory(QString&)));
	connect(_pEditorController, SIGNAL(getCopiedItems(QList<GTACommandBase*>&)), _pMDIController, SLOT(onGetCopiedItems(QList<GTACommandBase*>&)));
	connect(_pEditorController, SIGNAL(setCopiedItem(GTACommandBase*)), _pMDIController, SLOT(onSetCopiedItem(GTACommandBase*)));
	connect(_pEditorController, SIGNAL(setCopiedItems(QList<GTACommandBase*>)), _pMDIController, SLOT(onSetCopiedItems(QList<GTACommandBase*>)));
	connect(this, SIGNAL(updateActiveViewController(GTAEditorViewController*)), _pMDIController, SLOT(onUpdateActiveViewController(GTAEditorViewController*)));

	QShortcut* escShortCut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
	escShortCut->setContext(Qt::WidgetWithChildrenShortcut);
	connect(escShortCut, SIGNAL(activated()), this, SLOT(onEscapeKeyTriggered()));

	_pUndoStack = new QUndoStack(this);

	QStringList fileTypes;
	fileTypes << ELEM_SEQ << ELEM_PROC << ELEM_FUNC << ELEM_OBJ;
	//    _CurrentElementType = ELEM_PROC;
	_pActionWindow = new GTAActionSelectorWidget(this);
	connect(_pActionWindow, SIGNAL(setParentFocus()), this, SLOT(onSetFocus()));
	_pActionWindow->hide();
	_pCheckWindow = new GTACheckWidget();
	connect(_pCheckWindow, SIGNAL(setParentFocus()), this, SLOT(onSetFocus()));
	_pCheckWindow->hide();

	ui->EditorTV->setModel(nullptr);

	_pEditorController->setInitialEditorView(ui->EditorTV);
	_pEditorView = ui->EditorTV;


	_pHeaderWidget = new GTAHeaderWidget(this);
	QTableView* headerTableView = _pHeaderWidget->getHeaderTableView();
	_pEditorController->setInitialHeaderView(headerTableView);
	_pHeaderWidget->hide();

	_pSummaryAndConclusionDlg = new GTAPurposeAndConclusionDialog(this);
	_pSummaryAndConclusionDlg->hide();

	connect(_pHeaderWidget, SIGNAL(headerFieldAdded(QString, QString, QString, QString)), this, SLOT(onHeaderFieldAdded(QString, QString, QString, QString)));
	connect(this, SIGNAL(headerFieldUpdatedInModel(bool, QString)), _pHeaderWidget, SLOT(onHeaderFieldUpdatedInModel(bool, QString)));

	createContextMenu(iFileName);
	connect(_pActionWindow, SIGNAL(okPBClicked()), this, SLOT(onStartEditAction()));
	connect(_pActionWindow, SIGNAL(reopenWindowScheduled()), this, SLOT(onReopenWindow()));
	connect(_pCheckWindow, SIGNAL(okPressed()), this, SLOT(onStartEditCheck()));
	connect(_pCheckWindow, SIGNAL(openActionWindowScheduled()), this, SLOT(onReopenWindow()));

	//Editor Double Click Implementation
	connect(ui->EditorTV, SIGNAL(clicked(QModelIndex)), this, SLOT(onSingleClickOfRow(QModelIndex)));
	connect(ui->EditorTV, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onDoubleClickOfRow(QModelIndex)));
	connect(ui->EditorTV, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(displayContextMenu(const QPoint&)));
	connect(ui->EditorTV, SIGNAL(pressed(QModelIndex)), this, SLOT(highLightLastChild(QModelIndex)));

	onToggleEditorConnections(true);

	_titleCollapseToggled = false;
	_ReadOnlyDocToggled = false;

	_windowTitle = iFileName;
	setWindowTitle(iFileName);
	ui->EditorTV->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	ui->EditorTV->show();
	ui->EditorTV->scrollToTop();
	_isLaunchSequencer = false;

	//Connecting undo stack so that file always knows if it has been modified
	connect(this->_pUndoStack, SIGNAL(indexChanged(int)), this, SLOT(checkIsFileModified()));
}

GTAEditorWindow::~GTAEditorWindow()
{
	emit removeStack(_pUndoStack);
	if (_pActionWindow != nullptr)
	{
		delete _pActionWindow;
		_pActionWindow = nullptr;
	}
	if (_pCheckWindow != nullptr)
	{
		delete _pCheckWindow;
		_pCheckWindow = nullptr;
	}
	if (_pElement != nullptr)
	{
		delete _pElement;
		_pElement = nullptr;
	}
	//removing all QActions and QMenu because all added actions and sub-menu are children of _pContextMenu
	//do not remove the deletion of _pContextMenu
	if (_pContextMenu != nullptr)
	{
		delete _pContextMenu;
		_pContextMenu = nullptr;
	}

	if (_pUndoStack != nullptr)
	{
		delete _pUndoStack;
		_pUndoStack = nullptr;
	}
	if (_pHeaderWidget != nullptr)
	{
		delete _pHeaderWidget;
		_pHeaderWidget = nullptr;
	}
	if (_pElementViewModel != nullptr)
	{
		delete _pElementViewModel;
		_pElementViewModel = nullptr;
	}
	if (_pEditorController != nullptr)
	{
		delete _pEditorController;
		_pEditorController = nullptr;
	}
	if (_pEditorView != nullptr)
	{
		delete _pEditorView;
		_pEditorView = nullptr;
	}
	if (nullptr != _pDebuggerThread)
	{
		_pDebuggerThread->exit();
		while (_pDebuggerThread->isRunning()) {}
		delete _pDebuggerThread;
		_pDebuggerThread = nullptr;
	}
	if (nullptr != _pDebugger)
	{
		delete _pDebugger;
		_pDebugger = nullptr;
	}
	if (_pLiveEditorContextMenu != nullptr)
	{
		delete _pLiveEditorContextMenu;
		_pLiveEditorContextMenu = nullptr;
	}

	
	if (nullptr != _pProgressBar)
	{
		delete _pProgressBar;
		_pProgressBar = nullptr;
	}
	delete ui;
}

void GTAEditorWindow::showProgressBar(const QString& iMsg)
{
	if (_pProgressBar != nullptr)
	{
		_pProgressBar->setVisible(true);
		_pProgressBar->setWindowModality(Qt::ApplicationModal);
		_pProgressBar->setLabelText(iMsg);
	}
}

void GTAEditorWindow::hideProgressBar()
{
	if (_pProgressBar != nullptr)
	{
		_pProgressBar->setVisible(false);
	}
}
void GTAEditorWindow::initMembers()
{
	_pActionWindow = nullptr;
	_pCheckWindow = nullptr;
	_pElement = nullptr;
	_pContextMenu = nullptr;

	_pUndoStack = nullptr;
	//    _pUndoAction = nullptr;
	//    _pRedoAction = nullptr;
	_pElementViewModel = nullptr;
	_pEditorController = nullptr;
	_pEditorView = nullptr;
	_pHeaderWidget = nullptr;
	_pLiveEditorContextMenu = nullptr;

}

void GTAEditorWindow::setFileType(const QString& iFileType)
{

	if (_pEditorController)
	{

		if (!iFileType.isEmpty())
		{
			GTAElement* pElement = _pEditorController->getElement();
			if (pElement != nullptr)
			{
				GTAElement::ElemType elementType;
				if (iFileType == ".pro")
					elementType = GTAElement::PROCEDURE;
				else if (iFileType == ".seq")
					elementType = GTAElement::SEQUENCE;
				else if (iFileType == ".fun")
					elementType = GTAElement::FUNCTION;
				else if (iFileType == ".obj")
					elementType = GTAElement::OBJECT;
				else if (iFileType == ".tmpl")
					elementType = GTAElement::TEMPLATE;

				pElement->setElementType(elementType);
			}
		}
	}
}

// V26 editor view author column enhancement #322480 --->
//DEV: Setting author name is only done for new file creation
//If author name needs to be changed, creation date must be managed accordingly
void GTAEditorWindow::setAuthorName(const QString& iAuthor)
{
	if (_pEditorController)
	{
		if (!iAuthor.isEmpty())
		{
			GTAElement* pElement = _pEditorController->getElement();
			if (pElement != nullptr)
			{
				pElement->setAuthorName(iAuthor);
				pElement->setCreationTime(QDateTime::currentDateTime().toString());
			}
		}
	}
}

/**
 * @brief Set the Maximum Time Estimated value
 * @param iMaxTimeEstimated 
*/
void GTAEditorWindow::setMaxTimeEstimated(const QString& iMaxTimeEstimated)
{
	if (_pEditorController)
	{
		if (!iMaxTimeEstimated.isEmpty())
		{
			GTAElement* pElement = _pEditorController->getElement();
			if (pElement != nullptr)
			{
				_pEditorController->updateElementMaxTimeEstimated(iMaxTimeEstimated);
			}
		}
	}
}

/**
 * @brief Set the Undo/Redo for Maximum Time Estimation edition
 * @param old_value 
 * @param new_value 
*/
void GTAEditorWindow::setUndoRedoForMTE(const QString& old_value, const QString& new_value)
{
	_pUndoStack->push(new GTAUndoRedoEditMTE(old_value, new_value, this));
}

/**
 * @brief Get the Maximum Time Estimated value
 * @return QString
*/
QString GTAEditorWindow::getMaxTimeEstimated()
{
	GTAElement* pElement = _pEditorController->getElement();
	_maxTimeEstimated = pElement->getMaxTimeEstimated();
	return _maxTimeEstimated;
}

//<----V26 editor view column enhancement #322480
void GTAEditorWindow::setWindowTitle(const QString& iFileName)
{

	this->QWidget::setWindowTitle(iFileName);
}


void GTAEditorWindow::displayContextMenu(const QPoint& iPos)
{
	QWidget* pSourceWidget = (QWidget*)sender();
	if (GTAElementViewModel::LIVE == _pEditorController->getDisplayContext())
	{
		displayLiveEditorContextMenu(iPos, pSourceWidget);
		return;
	}

	if (pSourceWidget != nullptr && pSourceWidget == ui->EditorTV && _pContextMenu != nullptr)
	{

		QList<int> lstSelectedRows;
		/*int selectionStatus =*/
		getSelectedEditorRows(lstSelectedRows);
		if (!lstSelectedRows.isEmpty())
		{
			GTACommandBase* pCmd = _pEditorController->getCommand(lstSelectedRows[0]);
			if (_pContextMenu != nullptr)
			{
				QList<QAction*> lst = _pContextMenu->actions();
				for (int i = 0; i < lst.size(); i++)
				{
					QAction* pWdgt = lst.at(i);
					if (nullptr != pWdgt)
					{
						QString sName = pWdgt->text();
						if (sName == "Show Details")
						{
							if (detailViewExists(lstSelectedRows.at(0)))
								pWdgt->setEnabled(true);
							else
								pWdgt->setEnabled(false);
						}
						else if (sName == "Editor")
						{
							if (pCmd != nullptr)
								pWdgt->setEnabled(false);
							else
								pWdgt->setEnabled(true);
						}
						else if (sName == "Change/Edit Command\nF2")
						{
							if (pCmd != nullptr)
								pWdgt->setEnabled(true);
							else
								pWdgt->setEnabled(false);
						}
					}
				}
			}

			bool found = true;
			foreach(int row, lstSelectedRows)
			{
				GTACommandBase* pCmd = getCommandForSelectedRow(row);
				if (pCmd != nullptr && pCmd->hasReference())
				{
					found &= true;
				}
				else
				{
					found &= false;
				}
			}

			GTACommandBase* pCommand = _pEditorController->getCommand(lstSelectedRows[0]);
			GTAActionCallProcedures* pCallProc = dynamic_cast<GTAActionCallProcedures*> (pCommand);

			if (!found && (lstSelectedRows.count() == 1) && pCallProc != nullptr)
			{
				//clear the existing menu
				_pOpenCallInNewWindowMenu->clear();

				//populate the menu with procedure/functions in the parallel call command
				QStringList ProcedureList = pCallProc->getProcedureNames();
				foreach(QString name, ProcedureList)
				{
					QAction* pOpenCallInNewWindow = new QAction(QIcon(":/images/forms/img/NewWindow.png"), name, _pOpenCallInNewWindowMenu);
					connect(pOpenCallInNewWindow, SIGNAL(triggered()), this, SLOT(onOpenCallInNewWindow()));
					_pOpenCallInNewWindowMenu->addAction(pOpenCallInNewWindow);
				}
				_pOpenCallInNewWindowMenu->menuAction()->setVisible(true);
				_pOpenCallInNewWindow->setVisible(false);
			}
			else if (found && (lstSelectedRows.count() == 1))
			{
				_pOpenCallInNewWindowMenu->menuAction()->setVisible(false);
				_pOpenCallInNewWindow->setVisible(true);
			}
		}

		_pContextMenu->exec(ui->EditorTV->viewport()->mapToGlobal(iPos));
	}
}

bool GTAEditorWindow::detailViewExists(const int& iSelectedRow)
{
	bool rC = false;
	GTACommandBase* pSelectedCmd = getCommandForSelectedRow(iSelectedRow);
	if (pSelectedCmd)
	{
		if (pSelectedCmd->canHaveChildren() || pSelectedCmd->hasReference())
			rC = true;
	}

	return rC;
}



void GTAEditorWindow::validateName(QString iName)
{
	QRegExp rx("([?/'*:<>\"\\\\]+)");
	if (iName.contains(rx))
	{
		iName.remove(rx);
		//Dev comment:
		//  ui->ElemNameLE->setText(iName);
	}

}

bool GTAEditorWindow::isValidLogAvailable(QString& oLogPath)
{

	bool isValidLog = false;
	if (_pMDIController)
	{
		QString logName = _elementName;
		logName = logName.mid(0, logName.lastIndexOf("."));
		logName.trimmed();
		logName.append(".log");

		isValidLog = _pMDIController->isValidLogPresent(logName, oLogPath);
	}
	return isValidLog;

}


void GTAEditorWindow::onSave()
{
	if (_pEditorController != nullptr && _pMDIController != nullptr)
	{
		GTAElement* pElement = _pEditorController->getElement();
		GTAHeader* pHeader = _pEditorController->getHeader();

		// This condition must be checked when creating the file
		if (pElement->getName() != ELEM_TMPL)
		{		
			if (this->getHeaderName() == ELEM_TMPL)
			{
				bool rC = _pEditorController->getHeaderFromDocument(_elementName.replace(".pro", ".hdr"), pHeader);
				if (rC)
				{
					pElement->setHeader(pHeader);
					_pEditorController->setHeader(pHeader);
				}
			}


			// Save the file 
			if (pElement != nullptr)
			{
				QString timeModified = pElement->getModifiedTime();
				QString timeValidated = pElement->getValidationTime();
				QDateTime modTime = QDateTime::fromString(timeModified);
				QDateTime valTime = QDateTime::fromString(timeValidated);

				if (valTime.isNull())
				{
					pElement->setValidationTime(timeModified);
					valTime = modTime;
				}

				modTime.toMSecsSinceEpoch();
				valTime.toMSecsSinceEpoch();

				if ((!modTime.isNull() && !valTime.isNull()) && (modTime >= valTime))
				{
					GTAAppController::getGTAAppController()->updatevalidationStatusonDV(pElement->getUuid(), XNODE_NOT_VALIDATED, pElement->getValidatorName(), timeValidated);
					pElement->setValidationStatus(XNODE_NOT_VALIDATED);
				}
			}

			//save last index of UndoStack Class
			_lastindex = _pUndoStack->index();
			isFileModified(true);

			if (!_elementName.isEmpty() && !_relativePath.isEmpty())
			{
				QString fileName = _elementName.mid(0, _elementName.indexOf("."));
				bool overWrite = true;
				_pMDIController->saveDocument(fileName, overWrite, pElement, pHeader, _relativePath);
			}
			else
			{
				QMessageBox::StandardButton button = QMessageBox::information(this, "Save", "File will be saved in Data folder", QMessageBox::Ok, QMessageBox::Cancel);
				if (button == QMessageBox::Ok)
				{
					QString path = _windowTitle;
					QFile file(path);
					if (file.exists())
					{
						QString relPath = path.mid(path.lastIndexOf("/"), path.count() - 1);
						QString fileName = relPath.mid(0, relPath.indexOf("."));
						fileName.replace("/", "");
						fileName.trimmed();
						if (pElement)
							pElement->resetUuid();
						showProgressBar("Saving file");
						bool overWrite = false;
						QFutureWatcher<bool>* FutureWatcher = new QFutureWatcher<bool>();
						connect(FutureWatcher, SIGNAL(finished()), this, SLOT(onSaveEditorDocFinished()));
						QFuture<bool> Future = QtConcurrent::run(_pMDIController, &GTAMdiController::saveDocument, fileName, overWrite, pElement, pHeader, relPath);
						FutureWatcher->setFuture(Future);
					}
					else
					{
						QMessageBox::critical(this, "Error", QString("File '%1' does not exist").arg(path));
						return;
					}
				}
			}
		}
		else
		{
			QMessageBox::warning(this, tr("WARNING"),
				tr("The file name cannot be 'Template' or empty.\nPlease modify the file name."),
				QMessageBox::Ok, QMessageBox::Ok);
			return;
		}
	}
}

void GTAEditorWindow::onSaveEditorDocFinished()
{
	QFutureWatcher<bool>* pWatcher = dynamic_cast<QFutureWatcher<bool> *>(sender());
	if (pWatcher == nullptr)
		return;
	else
	{
		hideProgressBar();
		if (!pWatcher->result())
		{
			QMessageBox::warning(this, "Document Save", _pMDIController->getLastError());
		}
		else
		{
			QString filePath = getAbsolutePath();
			emit saveEditorDocFinished(filePath);
		}
		pWatcher->deleteLater();
	}
}

QString GTAEditorWindow::getAbsolutePath()const
{
	QString GtaDir;
	_pMDIController->onGetGTADataDirectory(GtaDir);
	QString relPath = getRelativePath();
	QString filePath = QString("%1%2").arg(GtaDir, relPath);
	return filePath;
}


void GTAEditorWindow::setJumpFlag(bool iVal)
{
	_isJump = iVal;
}

void GTAEditorWindow::setJumpContext(GTAAppController::DisplayContext iDisplayContext)
{
	_contextOnJump = iDisplayContext;
}

void GTAEditorWindow::openDocument(const QString& iFileName, bool isOutsideDataDir)
{
	loadEditor(iFileName, isOutsideDataDir);
}

void GTAEditorWindow::onCreationOfElementCompleted()
{

	QFutureWatcher<bool>* pWatcher = dynamic_cast<QFutureWatcher<bool> *>(sender());

	if (pWatcher == nullptr)
	{
		return;
	}

	if (_pEditorController)
	{
		bool result = pWatcher->result();
		if (result)
		{

			pWatcher->deleteLater();

			//            bool isGTAVersionValid = false;
			//            GTAElement *pElement = _pEditorController->getElement();
			//            if(pElement != nullptr)
			//            {
			//                QString gtaVer = pElement->getGtaVersion();
			//                if(!gtaVer.isEmpty())
			//                {
			//                    QStringList gtaVersion = gtaVer.split(".");
			//                    if(gtaVersion.count())
			//                    {
			//                        QString temp = gtaVersion.first();
			//                        bool ok;
			//                        int verNo = temp.toInt(&ok);
			//                        if(ok && verNo >= 23)
			//                            isGTAVersionValid = true;
			//                    }
			//                }
			//            }

			//            if(isGTAVersionValid == false)
			//            {
			//                portElement();
			//            }
			//            else
			//            {
			elementCreationSuccessful();
			//            }
		}
		else
		{
			QMessageBox::information(this, "Error in loading", _pEditorController->getLastError());
		}

	}
}

void GTAEditorWindow::elementCreationSuccessful()
{
	if (_pEditorController)
	{
		QAbstractItemModel* pEditorModel = nullptr;
		QAbstractItemModel* pHeaderModel = nullptr;

		_pEditorController->getElementModel(pEditorModel);
		_pEditorController->createHeaderModelFromDoc(_relativePath, pHeaderModel);
		_pEditorController->getHeaderModel(pHeaderModel);

		if (_pHeaderWidget != nullptr && pHeaderModel != nullptr)
		{
			_pHeaderWidget->setModelForCurrentDoc(pHeaderModel);
		}

		if (nullptr != pEditorModel)
		{
			ui->EditorTV->setModel(pEditorModel);
			for (int i = 0; i < _ColWidths.size(); i++) {
				ui->EditorTV->setColumnWidth(i, _ColWidths.at(i));
			}

			ui->EditorTV->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
			ui->EditorTV->horizontalHeader()->setSectionResizeMode(GTAElementViewModel::BREAKPOINT_INDEX, QHeaderView::Fixed);
			ui->EditorTV->horizontalHeader()->setStretchLastSection(true);
			ui->EditorTV->setSelectionMode(QAbstractItemView::ExtendedSelection);
			ui->EditorTV->setColumnWidth(GTAElementViewModel::ACTION_INDEX, 800);

			GTAElement* pElement = nullptr;

			if (_pEditorController != nullptr)
				pElement = _pEditorController->getElement();

			if (pElement)
				emit updateReadOnlyDocStatus(pElement->isReadOnly());

			showAllRows();

			QStringList hiddenRows = _pEditorController->getHiddenRows();
			if (hiddenRows.isEmpty() == false)
			{
				QList<int> rows;
				for (int i = 0; i < hiddenRows.size(); i++)
				{
					QString rowItem = hiddenRows.at(i);
					if (rowItem.isEmpty() == false)
					{
						bool isNum = false;
						int dNum = rowItem.toInt(&isNum);
						if (isNum == true)
							rows.append(dNum);

					}
				}
				if (rows.isEmpty() == false)
				{
					HideRows(rows);
				}
			}

			if (pElement != nullptr && pElement->isReadOnly() == true)
				emit disableEditingActionsInMain(true);
			else
				emit disableEditingActionsInMain(false);

		}
		else
		{
			ui->EditorTV->setModel(nullptr);
		}


		_pUndoStack->clear();
		ui->EditorTV->show();
		ui->EditorTV->scrollToTop();
		emit toggleTitleCollapseActionIcon(_titleCollapseToggled);

		show();

		if (_isJump)
		{
			emit elementCreatedForJump(_contextOnJump);
			_isJump = false;
			_contextOnJump = GTAAppController::EDITING;
		}
	}
}

//void GTAEditorWindow::setWithToColumn()
//{
//    QTableView *pTableView = dynamic_cast<QTableView*>(ui->EditorTV->model());
//    if(pTableView)
//    {
//        QHeaderView *pHeader = pTableView->horizontalHeader();
//        if(pHeader)
//        {
//            pHeader->in
//        }
//    }
//}


void GTAEditorWindow::loadEditor(const QString& iFileName, bool isOutsideDataDir)
{
	if (_pEditorController)
	{
		storeColoumWidths();
		ui->EditorTV->setModel(nullptr);

		QString path;
		_pMDIController->onGetGTADataDirectory(path);
		_pEditorController->setGTADirPath(path);

		QFutureWatcher<bool>* FutureWatcher = new QFutureWatcher<bool>();
		connect(FutureWatcher, SIGNAL(finished()), this, SLOT(onCreationOfElementCompleted()));
		QFuture<bool> Future = QtConcurrent::run(_pEditorController, &GTAEditorViewController::createAndSetElementFromDataDirectoryDoc, iFileName, isOutsideDataDir);
		FutureWatcher->setFuture(Future);
	}
}

void GTAEditorWindow::storeColoumWidths()
{

	QAbstractItemModel* pCurrentEditorModel = ui->EditorTV->model();
	if (nullptr != pCurrentEditorModel)
	{
		_ColWidths.clear();
		int noOfCols = pCurrentEditorModel->columnCount();
		for (int i = 0; i < noOfCols; i++)
		{
			if (i == 0)
			{
				_ColWidths.append(BREAKPOINT_COL_WIDTH);
			}
			else
				_ColWidths.append(ui->EditorTV->columnWidth(i));
		}
	}

}

void GTAEditorWindow::showAllRows()

{
	int totalRows = ui->EditorTV->model() != nullptr ? ui->EditorTV->model()->rowCount() : 0;
	for (int i = 0; i < totalRows; i++)
		ui->EditorTV->showRow(i);
}

void GTAEditorWindow::showRows(QList<int> iLstRows)
{

	if (iLstRows.isEmpty() == false && _pEditorController != nullptr)
	{
		qSort(iLstRows.begin(), iLstRows.end(), qLess<int>());
		if (!iLstRows.isEmpty())
		{
			int count = iLstRows.size();
			for (int i = count - 1; i >= 0; i--)
			{
				int selectedRow = iLstRows.at(i);
				ui->EditorTV->showRow(selectedRow);

				//                _titleCollapseToggled = false;
				//                emit collapseToTitle(_titleCollapseToggled);
				//                emit toggleTitleCollapseActionIcon(_titleCollapseToggled);
				GTACommandBase* pCmd = _pEditorController->getCommand(selectedRow);
				if (nullptr != pCmd)
				{
					int noOfChildrens = pCmd->getAllChildrenCount();
					if (noOfChildrens)
					{
						for (int j = 1; j <= noOfChildrens; j++)
						{
							ui->EditorTV->showRow(j + selectedRow);
						}
					}
				}
			}

		}
	}

}

void GTAEditorWindow::HideRows(QList<int> iLstRows)
{
	if (iLstRows.isEmpty() == false)
	{
		qSort(iLstRows.begin(), iLstRows.end(), qLess<int>());
		if (!iLstRows.isEmpty())
		{
			int count = iLstRows.size();
			for (int i = count - 1; i >= 0; i--)
			{
				int selectedRow = iLstRows.at(i);
				ui->EditorTV->hideRow(selectedRow);

				//                _titleCollapseToggled = true;
				//                emit collapseToTitle(_titleCollapseToggled);
				//                emit toggleTitleCollapseActionIcon(_titleCollapseToggled);

				GTACommandBase* pCmd = getCommandForSelectedRow(selectedRow);
				if (nullptr != pCmd)
				{
					int noOfChildrens = pCmd->getAllChildrenCount();
					if (noOfChildrens)
					{
						for (int j = 1; j <= noOfChildrens; j++)
						{
							ui->EditorTV->hideRow(j + selectedRow);
						}
					}
				}
			}

		}
	}

}

GTACommandBase* GTAEditorWindow::getCommandForSelectedRow(const int& iSelRow)const
{
	if (_pEditorController)
	{
		return _pEditorController->getCommand(iSelRow);
	}
	else
		return nullptr;
}


// returns -1 if model is absent.
// returns 0 if no rows are selected.
// returns 1 if valid rows are selected.
int  GTAEditorWindow::getSelectedEditorRow(int& oSelectedRow)
{
	if (ui->EditorTV->model())
	{
		QModelIndexList selectedRows = ui->EditorTV->selectionModel()->selectedRows();
		int lastRow = 0;
		if (selectedRows.count() == 0)
		{
			lastRow = ui->EditorTV->model()->rowCount() - 1;
			oSelectedRow = lastRow;
			return 0;
		}
		else
		{
			foreach(QModelIndex idx, selectedRows)
			{
				int selRow = idx.row();
				if (lastRow < selRow)
					lastRow = selRow;
			}
			oSelectedRow = lastRow;
			return 1;
		}
	}
	else
		return -1;

}

int  GTAEditorWindow::getSelectedEditorRows(QList<int>& oSelectedRows)
{
	if (ui->EditorTV->model())
	{
		QModelIndexList selectedRows = ui->EditorTV->selectionModel()->selectedRows();
		if (selectedRows.isEmpty())
			return 0;
		else
		{
			foreach(QModelIndex idx, selectedRows)
			{
				int selRow = idx.row();
				oSelectedRows.push_back(selRow);
			}
			return 1;
		}
	}
	else
		return -1;

}

void GTAEditorWindow::F2KeyPressed()
{
	if (ui->EditorTV->model() != nullptr)
	{
		QModelIndex idx = ui->EditorTV->currentIndex();
		if (idx.isValid())
		{
			onDoubleClickOfRow(idx);
		}
	}
}

/**
 * This function stimulates action on F9 key press during debugging
 * Enables/Disables breakpoint
*/
void GTAEditorWindow::F9KeyPressed()
{
	if (ui->EditorTV->model() != nullptr)
	{
		QModelIndex idx = ui->EditorTV->currentIndex();
		if (idx.isValid())
		{
			GTACommandBase* pCmd = nullptr;
			pCmd = _pEditorController->getCommand(idx.row());
			if (pCmd && !pCmd->isIgnoredInSCXML())
			{
				if (pCmd->hasBreakpoint())
					pCmd->setBreakpoint(false);
				else
					pCmd->setBreakpoint(true);
			}
		}
	}
}

/**
 * This function stimulates action on F10 key press during debugging
*/
void GTAEditorWindow::F10KeyPressed()
{
	if ((_pEditorController->getDisplayContext() == GTAElementViewModel::DEBUG) && _enableNextPreviousContinue)
	{
		_pExecutingCmd = getCommandForSelectedRow(_CurrentExecutionLine);

		if (_pExecutingCmd != nullptr)
		{
			_pExecutingCmd->setExecutionControl(false);
			emit userActionRegistered(GTADebugger::NEXT);

			//this action tag helps identify last executed action
			//it is updated here and used when the next command is presented for execution
			_action = GTADebugger::NEXT;

			//this bool tag prevents any unwanted action from MainWindow
			//the tag is only enabled if there is a breakpoint or the last command was a next/previous
			_enableNextPreviousContinue = false;
		}
	}
}

/**
 * This function stimulates action on Shift+F10 key press during debugging
*/
void GTAEditorWindow::ShiftF10KeyPressed()
{
	if ((_pEditorController->getDisplayContext() == GTAElementViewModel::DEBUG) && _enableNextPreviousContinue)
	{
		_pExecutingCmd = getCommandForSelectedRow(_CurrentExecutionLine);

		if (_pExecutingCmd != nullptr)
		{
			_pExecutingCmd->setExecutionControl(false);
			emit userActionRegistered(GTADebugger::PREVIOUS);

			//this action tag helps identify last executed action
			//it is updated here and used when the next command is presented for execution
			_action = GTADebugger::PREVIOUS;

			//this bool tag prevents any unwanted action from MainWindow
			//the tag is only enabled if there is a breakpoint or the last command was a next/previous
			_enableNextPreviousContinue = false;
		}

	}
}

/**
 * This function stimulates action on F6 key press during debugging
*/
void GTAEditorWindow::F6KeyPressed()
{
	if ((_pEditorController->getDisplayContext() == GTAElementViewModel::DEBUG) && _enableNextPreviousContinue)
	{
		_pExecutingCmd = getCommandForSelectedRow(_CurrentExecutionLine);
		if (_pExecutingCmd)
		{
			_pExecutingCmd->setExecutionControl(false);
			emit userActionRegistered(GTADebugger::CONTINUE);

			//this action tag helps identify last executed action
			//it is updated here and used when the next command is presented for execution
			_action = GTADebugger::CONTINUE;

			//this bool tag prevents any unwanted action from MainWindow
			//the tag is only enabled if there is a breakpoint or the last command was a next/previous
			_enableNextPreviousContinue = false;
		}
	}
}

/**
 * This function stimulates action on Shift+F6 key press during debugging
*/
void GTAEditorWindow::ShiftF6KeyPressed()
{
	if (_pEditorController->getDisplayContext() == GTAElementViewModel::DEBUG)
	{
		_pExecutingCmd = getCommandForSelectedRow(_CurrentExecutionLine);
		if (_pExecutingCmd)
		{
			_pExecutingCmd->setExecutionControl(false);
			emit userActionRegistered(GTADebugger::STOP);

			//this action tag helps identify last executed action
			//it is updated here and used when the next command is presented for execution
			_action = GTADebugger::STOP;

		}
		emit updateOutputWindow(QStringList() << "Please Wait! Ending the debug session safely!");
		onDebuggingCompleted();
	}
}

void GTAEditorWindow::onSingleClickOfRow(const QModelIndex& iIndex)
{
	if ((iIndex.isValid()) && (iIndex.column() == GTAElementViewModel::BREAKPOINT_INDEX))
	{

		GTACommandBase* pCmd = nullptr;
		pCmd = _pEditorController->getCommand(iIndex.row());
		if (pCmd)
		{
			if (pCmd->hasBreakpoint())
				pCmd->setBreakpoint(false);
			else
				pCmd->setBreakpoint(true);
		}
	}
}

void GTAEditorWindow::onDoubleClickOfRow(const QModelIndex& iIndex)
{
	//Dev Comment : Change the code in if condition. Move it to other function and keep only the else art in this code
	Qt::KeyboardModifiers modifier = QApplication::keyboardModifiers();
	if (modifier == Qt::ControlModifier)
	{

		QList<int> lstSelectedRows;
		int selectionStatus = getSelectedEditorRows(lstSelectedRows);
		if (selectionStatus > 0)
		{
			GTACommandBase* pCmd = getCommandForSelectedRow(lstSelectedRows.at(0));
			GTAActionCallProcedures* pCallProcedures = dynamic_cast<GTAActionCallProcedures*>(pCmd);
			if (pCallProcedures != nullptr)
			{
				_pOpenCallInNewWindowMenu->clear();
				QStringList ProcedureList = pCallProcedures->getProcedureNames();
				foreach(QString name, ProcedureList)
				{
					QAction* pOpenCallInNewWindow = new QAction(QIcon(":/images/forms/img/NewWindow.png"), name, _pOpenCallInNewWindowMenu);
					connect(pOpenCallInNewWindow, SIGNAL(triggered()), this, SLOT(onOpenCallInNewWindow()));
					_pOpenCallInNewWindowMenu->addAction(pOpenCallInNewWindow);
				}
				_pOpenCallInNewWindowMenu->setVisible(true);
				_pOpenCallInNewWindowMenu->exec(QCursor::pos());
			}
			else
			{
				onOpenCallInNewWindow();
			}
		}
	}
	else
	{
		if (iIndex.isValid() && ui->EditorTV->model() != nullptr)
		{
			int column = iIndex.column();
			int row = iIndex.row();

			QModelIndex typeIndex = ui->EditorTV->model()->index(row, GTAElementViewModel::TYPE_INDEX);
			QString typeValue = ui->EditorTV->model()->data(typeIndex).toString();

			if (column == GTAElementViewModel::ACTION_INDEX)
			{
				if (typeValue.contains("Check"))
				{
					onInsertCheck();
				}
				else
					onInsertAction();
			}
			else if (column == GTAElementViewModel::TYPE_INDEX)
			{
				if (typeValue.contains("Action"))
					onInsertAction();
				else
					onInsertCheck();

			}

		}
	}

}

void GTAEditorWindow::onEditCommand()
{
	//mimicing F2 functionality to just open the relevant command
	F2KeyPressed();
}

void GTAEditorWindow::onInsertCheck()
{
	QAction* pActionSender = dynamic_cast<QAction*>(sender());
	int selectionStatus;
	QList<int> selectedRow;
	selectionStatus = getSelectedEditorRows(selectedRow);
	bool bEditOK = false;

	if (selectionStatus == 0)
	{
		QMessageBox::critical(this, "Error", "Select Row to perform action editing .");
	}
	else if (selectedRow.size() > 1)
	{
		QMessageBox msgBox(this);
		QString text("Multiple selection detected, Edit first selection?");
		msgBox.setText(text);
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.setWindowTitle("Delete Row Warning!!!");
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msgBox.setDefaultButton(QMessageBox::No);
		int ret = msgBox.exec();
		if (ret == QMessageBox::Yes)
		{
			bEditOK = true;
		}

	}
	else
		bEditOK = true;

	if (_pEditorController != nullptr && _pCheckWindow != nullptr && _pMDIController != nullptr && bEditOK == true)
	{
		QList<QString> checkCommandList;

		//        Dev Comment : Decide how to handle which actions to load
		GTAElement* pElement = _pEditorController->getElement();
		if (pElement != nullptr)
		{
			GTAElement::ElemType elemtType = pElement->getElementType();
			if (elemtType == GTAElement::OBJECT)
				checkCommandList = _pMDIController->getCheckCommandList(GTAUtil::OBJECT);
			else if (elemtType == GTAElement::FUNCTION)
				checkCommandList = _pMDIController->getCheckCommandList(GTAUtil::FUNCTION);
			else if (elemtType == GTAElement::PROCEDURE)
				checkCommandList = _pMDIController->getCheckCommandList(GTAUtil::PROCEDURE);
			else if (elemtType == GTAElement::SEQUENCE)
				checkCommandList = _pMDIController->getCheckCommandList(GTAUtil::SEQUENCE);
			else if (elemtType == GTAElement::TEMPLATE)
				checkCommandList = _pMDIController->getCheckCommandList(GTAUtil::TEMPLATE);

		}
		_pCheckWindow->setChecksList(checkCommandList);

		int dSelectionStatus, dselectedRow;
		dSelectionStatus = getSelectedEditorRow(dselectedRow);
		GTACommandBase* pCmd = _pEditorController->getCommand(dselectedRow);
		GTACommandVariant cmdVariant;
		cmdVariant.setCommand(pCmd);
		_actualCmd.setValue(cmdVariant);

		if (_ReadOnlyDocToggled == true)
			_pCheckWindow->setEditableAction(false);
		else if (pCmd != nullptr && pCmd->getReadOnlyState())
			_pCheckWindow->setEditableAction(false);
		else
			_pCheckWindow->setEditableAction(true);

		if (_pEditorController->isEditableCommand(pCmd))
		{
			GTACheckBase* pCheck = dynamic_cast<GTACheckBase*> (pCmd);
			GTAActionBase* pAction = dynamic_cast<GTAActionBase*> (pCmd);

			QString actionsender = "";
			QString menusender = "";
			QString check = "";
			if (nullptr != pActionSender && nullptr != pActionSender->parent())
			{
				actionsender = pActionSender->text();

				QMenu* pMenuSender = dynamic_cast<QMenu*>(pActionSender->parent());
				if (nullptr != pMenuSender)
				{
					menusender = pMenuSender->title();
				}
			}

			if (pAction)
			{
				QMessageBox::critical(this, "Error", "Current row is an Action command, Check editing not possible");

			}
			else if (nullptr != pCheck)
			{
				check = pCheck->getCheckName();
				bool wrongaction = true;


				//check if action sender is equal to check
				if (actionsender == check)
					wrongaction = false;

				//if call comes from context menu Change/Edit
				//if call comes directly from F2 key press
				if (actionsender.isEmpty() || actionsender.contains("F2", Qt::CaseInsensitive))
				{
					wrongaction = false;
				}
				if (!wrongaction)
				{
					_pCheckWindow->setCheckCommand(pCheck, check);
					if (check == CHK_VALUE)
					{
						_pCheckWindow->selectLastEditedRow();
					}						
					_pCheckWindow->show();
				}
				else
				{
					QMessageBox::critical(this, "Error", "Current row has a different command, Command editing not possible.\nUse a blank line to add a new command");
				}
			}
			else
			{
				check = actionsender;
				_pCheckWindow->setCheckCommand(pCheck, check);
				_pCheckWindow->show();
			}
		}
		else
			QMessageBox::critical(this, "Error", "Action/Check Edit cannot be performed on end/invalid statements.");

	}
}

void GTAEditorWindow::onInsertAction()
{
	QAction* pActionSender = dynamic_cast<QAction*>(sender());
	GTACommandVariant variant;
	int selectionStatus;
	QList<int> selectedRow;
	selectionStatus = getSelectedEditorRows(selectedRow);
	bool bEditOK = false;

	if (selectionStatus == 0)
	{
		QMessageBox::critical(this, "Error", "Select Row to perform action editing.");
	}
	else if (selectedRow.size() > 1)
	{
		QMessageBox msgBox(this);
		QString text("Multiple selection detected. Edit first selection?");
		msgBox.setText(text);
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.setWindowTitle("Delete Row Warning!!!");
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msgBox.setDefaultButton(QMessageBox::No);
		int ret = msgBox.exec();
		if (ret == QMessageBox::Yes)
		{
			bEditOK = true;
		}

	}
	else
		bEditOK = true;

	if (_pActionWindow != nullptr && _pEditorController != nullptr && _pMDIController != nullptr && bEditOK == true)
	{
		QHash<QString, QStringList> actionCmdList;
		QStringList orderLst;


		GTAElement* pElement = _pEditorController->getElement();
		if (pElement != nullptr)
		{

			GTAElement::ElemType elemtType = pElement->getElementType();
			//        Dev Comment : Decide how to handle which actions to load
			if (elemtType == GTAElement::OBJECT)
				actionCmdList = _pMDIController->getActionCommandList(GTAUtil::OBJECT, orderLst);
			else if (elemtType == GTAElement::FUNCTION)
				actionCmdList = _pMDIController->getActionCommandList(GTAUtil::FUNCTION, orderLst);
			else if (elemtType == GTAElement::PROCEDURE)
				actionCmdList = _pMDIController->getActionCommandList(GTAUtil::PROCEDURE, orderLst);
			else if (elemtType == GTAElement::SEQUENCE)
				actionCmdList = _pMDIController->getActionCommandList(GTAUtil::SEQUENCE, orderLst);
			else if (elemtType == GTAElement::TEMPLATE)
				actionCmdList = _pMDIController->getActionCommandList(GTAUtil::TEMPLATE, orderLst);
		}

		_pActionWindow->setCommandList(actionCmdList, orderLst);

		GTACommandBase* pCmd = _pEditorController->getCommand(selectedRow[0]);
		
		if (_pEditorController->isEditableCommand(pCmd))
		{

			if (_ReadOnlyDocToggled == true)
				_pActionWindow->setEditableAction(false);
			else if (pCmd != nullptr && pCmd->getReadOnlyState())
				_pActionWindow->setEditableAction(false);
			else
				_pActionWindow->setEditableAction(true);

			GTAActionBase* pAction = dynamic_cast<GTAActionBase*> (pCmd);
			GTACheckBase* pCheck = dynamic_cast<GTACheckBase*> (pCmd);

			QString actionsender = "";
			QString menusender = "";
			QString action = "";
			QString complement = "";
			if (nullptr != pActionSender && nullptr != pActionSender->parent())
			{
				actionsender = pActionSender->text();

				QMenu* pMenuSender = dynamic_cast<QMenu*>(pActionSender->parent());
				if (nullptr != pMenuSender)
				{
					menusender = pMenuSender->title();
				}
			}

			if (pCheck)
			{
				QMessageBox::critical(this, "Error", "Current row is a Check command, Action editing not possible");
				return;
			}
			else if (nullptr != pAction)
			{
				action = pAction->getAction();
				complement = pAction->getComplement();
				bool wrongaction = true;

				if (complement.isEmpty())
				{
					//check if action sender is equal to action
					if (actionsender == action)
						wrongaction = false;
				}
				else
				{
					//check actionsender is equal to complement and menusender is equal to action
					if ((actionsender == complement) && (menusender == action))
						wrongaction = false;
				}

				//if call comes from context menu Change/Edit
				//if call comes directly from F2 key press
				if (actionsender.isEmpty() || actionsender.contains("F2", Qt::CaseInsensitive))
				{
					wrongaction = false;
				}
				if (!wrongaction)
				{
					_pActionWindow->setActionCommand(pAction);
					_pActionWindow->show();
					variant.setCommand(pCmd);
					_actualCmd.setValue(variant);

				}
				else
				{
					QMessageBox::critical(this, "Error", "Current row has a different command, Action editing not possible.\nUse a blank line to add a new command");
				}
			}
			else
			{
				if ((menusender != "Editor") && (menusender != "Action") && (menusender != "Check"))
				{
					action = menusender;
					complement = actionsender;
				}
				else
				{
					action = actionsender;
				}
				_pActionWindow->setActionCommand(pAction, action, complement);
				_pActionWindow->show();
				variant.setCommand(pCmd);
				_actualCmd.setValue(variant);
			}
		}
		else
			QMessageBox::critical(this, "Error", "Action/Check Edit cannot be performed on end/invalid statements.");

	}
}

/**
* @brief Editing an action.
*/
void GTAEditorWindow::onStartEditAction()
{
	if (_pActionWindow != nullptr)
	{
		int dSelectionStatus;
		QList<int> selectedRows;
		dSelectionStatus = getSelectedEditorRows(selectedRows);

		if (selectedRows.empty())
		{
			return;
		}
		else
		{
			QVariant pAction = _pActionWindow->getActionVariant();
			_pUndoStack->push(new GTAUndoRedoAddCmd(selectedRows[0], pAction, _actualCmd, this));
		}

		GTAActionIF* pActionIf = dynamic_cast<GTAActionIF*>(_pActionWindow->getActionCommand());
		if (pActionIf) {
			if (pActionIf->autoAddElse()) {
				dSelectionStatus = getSelectedEditorRows(selectedRows);
				autoAddElse(selectedRows[0]);
			}
			if (pActionIf->isAutoExternalAction()) {
				autoAddExternalAction("if", pActionIf->isExternalActionOnOk());
			}

		}
		
		auto* action = _pActionWindow->getActionCommand();
		GTAActionSetList* pActionSet = dynamic_cast<GTAActionSetList*>(action);
		if (pActionSet && pActionSet->isAutoExternalAction()) {
			autoAddExternalAction("set", pActionSet->isExternalActionOnOk());
		}
	}
}

void GTAEditorWindow::onReopenWindow() {
	ui->EditorTV->selectRow(_reopenWindowRow);
	QModelIndex index = ui->EditorTV->model()->index(_reopenWindowRow, GTAElementViewModel::ACTION_INDEX);
	onDoubleClickOfRow(index);
	_pActionWindow->setActionCB(14); // 14 = external action
}

/**
 * @brief Automatically add an else condition if requested on the if condition command creation
 * @param row is the row number of the if command
*/
void GTAEditorWindow::autoAddElse(int& row)
{	
	// Add a row for the Else 
	ui->EditorTV->selectRow(row);
	onAddRow();

	// Select the empty row
	row++;
	QModelIndex index = ui->EditorTV->model()->index(row, GTAElementViewModel::ACTION_INDEX);
	ui->EditorTV->setCurrentIndex(index);

	// Add the Else command
	GTAActionElse* pElse = new GTAActionElse(ACT_CONDITION, COM_CONDITION_ELSE);
	GTAActionBase* pBase = dynamic_cast<GTAActionBase*>(pElse);
	GTACommandVariant cmdVariant;
	cmdVariant.setCommand(pBase);
	QVariant variant;
	variant.setValue(cmdVariant);
	addCommand(row, variant);
}

void GTAEditorWindow::autoAddExternalAction(const QString& cmdType, bool isOnOk)
{
	
	QList<int> selectedRows;
	auto dSelectionStatus = getSelectedEditorRows(selectedRows);
	
	auto row = selectedRows[0];
	auto* action = _pActionWindow->getActionCommand();
	auto* check = _pCheckWindow->getCheckCommand();
	if (cmdType == "set" || cmdType == "check") {
		auto conditionArg = findReturnStatusConditionArg(row, cmdType);
		if (conditionArg && !(conditionArg->isEmpty())) {
			ui->EditorTV->selectRow(row);
			onAddRow();
			// Select the empty row
			row++;
			QModelIndex index = ui->EditorTV->model()->index(row, GTAElementViewModel::ACTION_INDEX);
			ui->EditorTV->setCurrentIndex(index);

			// Add the If command
			GTAActionIF* pCond = new GTAActionIF(ACT_CONDITION, COM_CONDITION_IF);
			pCond->setCondition(ARITH_EQ);
			pCond->setParameter(*conditionArg);
			pCond->setIsFsOnly(false);
			pCond->setIsValueOnly(true);
			pCond->setChkAudioAlarm(false);
			isOnOk ? pCond->setValue("'OK'") : pCond->setValue("'KO'");

			GTAActionBase* pBase = dynamic_cast<GTAActionBase*>(pCond);
			if (pBase) {
				GTACommandVariant cmdVariant;
				cmdVariant.setCommand(pBase);
				QVariant variant;
				variant.setValue(cmdVariant);
				addCommand(row, variant);
			}

			onAddRow(); //empty row for the external action
			row++;
			if (cmdType == "set" && action) {
				if (action->isAutoOpenWindow()) {
					_reopenWindowRow = row;
					_pActionWindow->scheduleReopenWindow();
				}

				if (action->isAutoAddElse()) {
					autoAddElse(row);
					onAddRow();
					row++;
				}
			}
			else if (cmdType == "check" && check) {
				if (check->isAutoOpenWindow()) {
					_reopenWindowRow = row;
					_pCheckWindow->scheduleOpenActionWindow();
				}

				if (check->isAutoAddElse()) {
					autoAddElse(row);
					onAddRow();
					row++;
				}
			}

			onAddRow(); //row for end if
			row++;
			GTAActionIfEnd* pEndIf = new GTAActionIfEnd();
			pEndIf->setParent(pBase);

			GTAActionBase* pBaseEnd = dynamic_cast<GTAActionBase*>(pEndIf);
			if (pBaseEnd) {
				GTACommandVariant cmdVariant;
				cmdVariant.setCommand(pBaseEnd);
				QVariant variant;
				variant.setValue(cmdVariant);
				addCommand(row, variant);
			}
		}
	}
	else if (cmdType == "if") {
		row--;
		QModelIndex index = ui->EditorTV->model()->index(row, GTAElementViewModel::ACTION_INDEX);
		ui->EditorTV->setCurrentIndex(index);
		onAddRow(); //empty row for the external action
		row++;
		if (action->isAutoOpenWindow()) {
			_reopenWindowRow = row;
			_pActionWindow->scheduleReopenWindow();
		}
		row++;
		index = ui->EditorTV->model()->index(row, GTAElementViewModel::ACTION_INDEX);
		ui->EditorTV->setCurrentIndex(index);
	}
}

std::optional<QString> GTAEditorWindow::findReturnStatusConditionArg(int& row, const QString& cmdName) {
	QString uuid;
	auto pAppCtrl = GTAAppController::getGTAAppController();
	QList<CommandInternalsReturnInfo> retInfoList;
	
	QString actionName;
	if (cmdName == "set") {
		auto* action = _pActionWindow->getActionCommand();
		if (!action) { std::nullopt; }
		pAppCtrl->getCommandList(retInfoList, cmdName);
		uuid = action->getObjId();
		actionName = action->getInstanceName();
	}
	else if (cmdName == "check") {
		auto* check = _pCheckWindow->getCheckCommand();
		if (!check) { std::nullopt; }
		pAppCtrl->getCommandList(retInfoList, "check_Value"); //inconsistent naming, check is sometimes called check, sometimes check_Value
		uuid = check->getObjId();
		actionName = check->getInstanceName();
	}
	else {
		qWarning() << "Command type " << cmdName << " is not supported for findReturnStatusConditionArg";
		std::nullopt;
	}

	 
	QString conditionArg = "";
	for (const auto& info : retInfoList) {
		auto infoIdxStr = info.refLoc;
		int close = infoIdxStr.lastIndexOf("]");
		int open = infoIdxStr.lastIndexOf("[");
		int length = close - open;
		infoIdxStr = infoIdxStr.mid(open + 1, length - 1);
		int infoIdx = infoIdxStr.toInt();
		if (infoIdx == row && info.retName == "Return.Status") {
			conditionArg = "Return.Status_uuid[";
			conditionArg += info.location;
			conditionArg += "]__" + cmdName;
			return conditionArg;
		}
	}

	return conditionArg;
}

/**
* @brief Editing a check.
*/
void GTAEditorWindow::onStartEditCheck()
{
	if (_pActionWindow != nullptr)
	{
		int dSelectionStatus;
		QList<int> selectedRows;
		dSelectionStatus = getSelectedEditorRows(selectedRows);

		if (selectedRows.empty())
		{
			return;
		}
		else
		{
			QVariant check = _pCheckWindow->getCheckVariant();
			_pUndoStack->push(new GTAUndoRedoAddCmd(selectedRows[0], check, _actualCmd, this));
		}

		auto* check = _pCheckWindow->getCheckCommand();
		GTACheckValue* checkVal = dynamic_cast<GTACheckValue*>(check);
		if (checkVal && checkVal->isAutoExternalAction()) {
			autoAddExternalAction("check", checkVal->isExternalActionOnOk());
		}
	}
}

void GTAEditorWindow::undoClearRow(const QList<int>& iIndexList, QList<GTACommandBase*>& iCmdList)
{
	if (_pEditorController)
	{
		_pEditorController->undoClearRow(iIndexList, iCmdList);
		_pEditorController->updateEditorView();

	}
}

void GTAEditorWindow::highLightLastChild(QModelIndex index)
{
	if (_pEditorController)
	{
		_pEditorController->highLightLastChild(index);
	}
}

void GTAEditorWindow::checkIsFileModified()
{
	isFileModified(true);
}

void GTAEditorWindow::clearRow(QList<int> lstSelectedRows, QList<int>& oInvalidCmdList)
{
	if (_pEditorController)
	{
		//avoid any highlight if it exists
		highLightLastChild(QModelIndex());
		QString errMsg;

		for (int i = 0; i < lstSelectedRows.size(); i++)
		{
			QString curerrMsg;
			int currRow = lstSelectedRows.at(i);
			QList<int> RemovedRowIds;

			if (currRow != -1)
			{

				bool rc = _pEditorController->clearRow(currRow, curerrMsg, RemovedRowIds);
				for (int j = 0; j < lstSelectedRows.count(); j++)
				{
					if (RemovedRowIds.contains(lstSelectedRows.at(j)))
					{
						lstSelectedRows[j] = -1;
					}
				}
				RemovedRowIds.clear();
				if (!rc)
				{
					errMsg.append(QString("[selection %1]:%2%3").arg(QString::number(currRow), curerrMsg, "\n"));
					oInvalidCmdList.append(currRow);
				}
			}
		}
		if (!errMsg.isEmpty())
		{
			//Dev Comment implement later
			//QMessageBox::critical(this,"Clear Row Error",errMsg);
			//            _ReportWidget->setText(errMsg);
			//            _ReportWidget->show();
			//            _ReportWidget->setMode(GTACompatibilityReport::GENERIC_ERROR);
			//            _ReportWidget->setDialogName("Clear action errors");

		}

		this->setMoveUpDownButtonEnabled(QModelIndex());
	}
}


void GTAEditorWindow::onClearRow()
{

	highLightLastChild(QModelIndex());
	bool readOnlyCmdStatus = getReadOnlyCmdStatus();

	if (GTAElementViewModel::LIVE == _pEditorController->getDisplayContext() && readOnlyCmdStatus)
	{
		// ignore, do not perform this action on Live Editor's read only cmds
	}
	else if (nullptr != _pEditorController)
	{
		QList<int> lstSelectedRows;
		getSelectedEditorRows(lstSelectedRows);
		if (!lstSelectedRows.isEmpty())
		{

			QList<GTACommandBase*> cmdsCleared;
			QMutableListIterator<int> i(lstSelectedRows);
			while (i.hasNext())
			{
				int rowID = i.next();
				GTACommandBase* pCmd = getCommandForSelectedRow(rowID);
				if (pCmd)
				{
					if (pCmd->IsDeletable())
						cmdsCleared.append(pCmd->getClone());
					else
					{
						i.remove();
						QString errMsg = "Non user editable commands cannot be cleared ,only parent of the command can be cleared";
						QMessageBox::critical(this, "Clear Row Error", errMsg);
					}
				}
				else
					i.remove();
			}
			if (!cmdsCleared.isEmpty())
			{
				_pUndoStack->push(new GTAUndoRedoClearRow(lstSelectedRows, cmdsCleared, this));
			}
		}
		else
		{
			QMessageBox::critical(this, "Clear Row Error", "Select a row to clear");
		}
	}
}

void GTAEditorWindow::addCommand(int selectedRow, QVariant cmd)
{
	if (_pEditorController)
	{
		bool rC = editAction(selectedRow, cmd);
		if (!rC)
		{
			QMessageBox::warning(this, "Edit Action Fail", QString("Cannot insert element:\n[%1]").arg(_pEditorController->getLastError()));

		}
		else
		{
			_pActionWindow->hide();
			bool resizeRow = TestConfig::getInstance()->getResizeRow();
			if (resizeRow)
			{
				ui->EditorTV->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
				resizeRowsToContents();
				ui->EditorTV->verticalHeader()->setSectionResizeMode(QHeaderView::Interactive);
			}
			ui->EditorTV->show();
		}
	}
}

bool GTAEditorWindow::editAction(const int& iRow, const QVariant& value)
{
	bool rC = false;
	if (_pEditorController)
	{
		rC = _pEditorController->editAction(iRow, value);
	}
	return rC;

}
bool GTAEditorWindow::editCheck(const int& iRow, const QVariant& value)
{

	bool rC = false;
	if (_pEditorController)
	{
		rC = _pEditorController->editAction(iRow, value);
	}
	return rC;

}

void GTAEditorWindow::resizeRowsToContents()
{
	ui->EditorTV->setWordWrap(true);
	ui->EditorTV->setWordWrap(true);
	if (ui->EditorTV != nullptr)
	{

		if (ui->EditorTV->model() != nullptr)
		{
			int rowCnt = ui->EditorTV->model()->rowCount();
			for (int i = 0; i < rowCnt; i++)
			{
				ui->EditorTV->resizeRowToContents(i);
			}
		}
	}
}


void GTAEditorWindow::setMoveUpDownButtonEnabled()
{
	setMoveUpDownButtonEnabled(QModelIndex());
}
void GTAEditorWindow::setMoveUpDownButtonEnabled(QModelIndex index)
{
	if (_pMoveRowUpAction != nullptr && _pMoveDownAction != nullptr)
	{
		if (_titleCollapseToggled == true)
		{
			_pMoveDownAction->setDisabled(true);
			_pMoveRowUpAction->setDisabled(true);

			emit updateMoveDownAction(true);
			emit updateMoveUpAction(true);
			return;

		}
		else  if (_pEditorController)
		{
			bool rc = false;
			int row = -1;


			if (index.isValid())
				row = index.row();
			else
			{
				QList<int> lstOfRows;
				getSelectedEditorRows(lstOfRows);
				if (lstOfRows.size() == 1)
					row = lstOfRows.at(0);
			}

			if (row != -1) {
				rc = _pEditorController->isEditorRowMovableUpwards(row);
			}

			if (!rc)
			{
				_pMoveRowUpAction->setDisabled(true);
				emit updateMoveUpAction(true);
			}
			else
			{
				_pMoveRowUpAction->setDisabled(false);
				emit updateMoveUpAction(false);

			}

			if (row != -1) {
				rc = _pEditorController->isEditorRowMovableDownwards(row);
			}

			if (!rc)
			{
				_pMoveDownAction->setDisabled(true);
				emit updateMoveDownAction(true);
			}
			else
			{
				_pMoveDownAction->setDisabled(false);
				emit updateMoveDownAction(false);
			}
		}
	}
}


void GTAEditorWindow::onAddRow()
{
	bool readOnlyCmdStatus = getReadOnlyCmdStatus();
	QList<int> selectedRows;
	int selectedRowStatus;
	selectedRowStatus = getSelectedEditorRows(selectedRows);

	if (selectedRows.count() == 1 && readOnlyCmdStatus && selectedRows.at(0) == getElement()->getAllChildrenCount() - 1)
	{
		_pUndoStack->push(new GTAUndoRedoAddRow(selectedRowStatus, selectedRows, this));
	}
	else if (GTAElementViewModel::LIVE == _pEditorController->getDisplayContext() && readOnlyCmdStatus)
	{
		// ignore, do not perform action
	}
	else
	{
		_pUndoStack->push(new GTAUndoRedoAddRow(selectedRowStatus, selectedRows, this));
	}
}


void GTAEditorWindow::addRow(int selectedRowStatus, QList<int>& selectedRows)
{
	if (_pEditorController)
	{
		if (selectedRowStatus > 0)
		{
			qSort(selectedRows.begin(), selectedRows.end(), qLess<int>());
			for (int i = selectedRows.size() - 1; i >= 0; i--)
			{
				_pEditorController->addEditorRows(selectedRows.at(i));
			}
		}
		else if (ui->EditorTV->model() != nullptr && ui->EditorTV->model()->rowCount() == 0)
		{
			_pEditorController->addEditorRows(0);
		}
		else if (selectedRowStatus == -1)
		{
			QMessageBox::warning(this, "Row Selection Error", "Non existent document Add row failed");
		}
	}
	this->setMoveUpDownButtonEnabled(QModelIndex());
}

void GTAEditorWindow::onDeleteRow()
{
	//avoid any highlight if it exists
	highLightLastChild(QModelIndex());

	bool readOnlyCmdStatus = getReadOnlyCmdStatus();
	if (GTAElementViewModel::LIVE == _pEditorController->getDisplayContext() && readOnlyCmdStatus)
	{
		// ignore, do not perform action
	}
	else if (nullptr != _pEditorController)
	{
		QList<int> lstSelectedRows;
		int selectionStatus = getSelectedEditorRows(lstSelectedRows);
		bool rC;
		rC = _pEditorController->areSelectedCommandsBalanced(lstSelectedRows);

		if (!lstSelectedRows.isEmpty())
		{

			//deleteRow(selectionStatus,lstSelectedRows);  original behavior following code is for undo redo
			GTAElement* pElement = _pEditorController->getElement();
			if (pElement)
			{
				QList<GTACommandBase*> cmdsToBeDeleted;
				for (int i = 0; i < lstSelectedRows.count(); i++)
				{
					int rowID = lstSelectedRows.at(i);
					GTACommandBase* pCmd = _pEditorController->getCommand(rowID);
					if (pCmd)
					{
						if (pCmd->IsDeletable())
						{
							cmdsToBeDeleted.append(pCmd->getClone());
							if (pCmd->hasChildren())
							{
								int totalRowCount = rowID + pCmd->getAllChildrenCount();
								for (int j = i + 1; j < lstSelectedRows.count(); j++)
								{
									int nextIndex = lstSelectedRows.at(j);
									if (nextIndex <= totalRowCount)
									{
										lstSelectedRows.removeAt(j);
									}
								}
							}
						}
						else
						{
							QString errMsg = "Non user editable commands cannot be deleted ,only parent of the command can be deleted";
							QMessageBox::critical(this, "Delete Row Error", errMsg);
						}
					}
					else
					{
						pCmd = nullptr;
						cmdsToBeDeleted.append(pCmd);
					}
				}
				_pUndoStack->push(new GTAUndoRedoDeleteRow(selectionStatus, lstSelectedRows, cmdsToBeDeleted, this));
			}
		}
	}
}

void GTAEditorWindow::deleteRow(int selectionStatus, QList<int>& lstSelectedRows)
{
	if (_pEditorController)
	{
		if (selectionStatus)
		{
			bool bDeletWarnigReqd = _pEditorController->areSelectedCommandsNested(lstSelectedRows);

			if (!bDeletWarnigReqd)
				deleteMultipleRowSelection(lstSelectedRows);
			else
			{
				QMessageBox msgBox(this);
				QString text("Child nodes will be removed if any! continue?");
				msgBox.setText(text);
				msgBox.setIcon(QMessageBox::Warning);
				msgBox.setWindowTitle("Delete Row Warning!!!");
				msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
				msgBox.setDefaultButton(QMessageBox::No);
				int ret = msgBox.exec();
				if (ret == QMessageBox::Yes)
				{
					deleteMultipleRowSelection(lstSelectedRows);
				}
				else
					return;

			}

			if (!lstSelectedRows.isEmpty())
			{
				ui->EditorTV->setSelectionMode(QAbstractItemView::SingleSelection);
				ui->EditorTV->clearSelection();


				int rowToBeSelected = lstSelectedRows.at(0);
				//if row to be selected does not exist its height is 0, since row count cannot be directly accessed from qtableview
				int rowHeight = ui->EditorTV->rowHeight(rowToBeSelected);
				if (!rowHeight)
					rowToBeSelected--;

				ui->EditorTV->selectRow(rowToBeSelected);
				ui->EditorTV->setSelectionMode(QAbstractItemView::ExtendedSelection);

			}
		}
		else
			QMessageBox::warning(this, "Row Selection Error", "Select row(s) to delete ");

		this->setMoveUpDownButtonEnabled(QModelIndex());
	}
}

void GTAEditorWindow::deleteMultipleRowSelection(const QList<int>& lstSelectedRows)
{
	if (_pEditorController != nullptr)
	{
		_pEditorController->deleteEditorRows(lstSelectedRows);

	}
}

void GTAEditorWindow::insertCommands(const QList<int>& iIndexList, const QList<GTACommandBase*>& iCmdList, bool isSrcUndoRedo)
{
	if (_pEditorController)
	{
		_pEditorController->insertCommands(iIndexList, iCmdList, isSrcUndoRedo);
		_pEditorController->updateEditorView();
	}
}
void GTAEditorWindow::cutRow(const int& selectionStatus, QList<int>& ioLstSelectedRows)
{

	if (_pEditorController)
	{

		_pEditorController->setEditorRowCopy(ioLstSelectedRows, false);
		if (!ioLstSelectedRows.isEmpty())
		{
			emit rowCopiedToClipBoard();
			deleteRow(selectionStatus, ioLstSelectedRows);
		}

	}
}


void GTAEditorWindow::onEditorCopyRow()
{
	bool readOnlyCmdStatus = getReadOnlyCmdStatus();
	if (GTAElementViewModel::LIVE == _pEditorController->getDisplayContext() && readOnlyCmdStatus)
	{
		// ignore, do not perform this action on Live Editor's read only cmds
	}
	else
	{
		QList<int> lstSelectedRows;
		int selectionStatus = getSelectedEditorRows(lstSelectedRows);
		if (selectionStatus)
		{
			_pEditorController->setEditorRowCopy(lstSelectedRows, true);
			emit rowCopiedToClipBoard();
			//saurav
			lastCalled = "copy";
		}
		else
			QMessageBox::warning(this, "Warning", "No selection Nothing copied");
	}
}

void GTAEditorWindow::pasteRow(const QList<int> lstSelectedRows, bool& oPastStatus, const QList<GTACommandBase*> CmdsPasted)
{
	//saurav
	PasteCallPrev = PasteCallNow;
	if (PasteCallPrev == "cut")
		lastCalled = "copy";
	PasteCallNow = lastCalled;

	if (_pEditorController)
	{
		//saurav
		oPastStatus = _pEditorController->pasteEditorCopiedRow(lstSelectedRows[0], CmdsPasted, lastCalled);

		if (!oPastStatus)
		{
			QString error = _pEditorController->getLastError();
			if (error.isEmpty())
				error = "paste error";
			QMessageBox::warning(this, "Warning", error);
		}

		//avoid any highlight if it exists
		highLightLastChild(QModelIndex());
		setMoveUpDownButtonEnabled(QModelIndex());
	}
}


void GTAEditorWindow::pasteRow(const QList<int> lstSelectedRows, bool& oPastStatus)
{

	if (_pEditorController)
	{

		oPastStatus = _pEditorController->pasteEditorCopiedRow(lstSelectedRows[0]);

		if (!oPastStatus)
		{
			QString error = _pEditorController->getLastError();
			if (error.isEmpty())
				error = "paste error";
			QMessageBox::warning(this, "Warning", error);
		}

		//avoid any highlight if it exists
		highLightLastChild(QModelIndex());
		setMoveUpDownButtonEnabled(QModelIndex());
	}
}

void GTAEditorWindow::onEditorPasteRow()
{

	if (nullptr != _pEditorController && nullptr != _pMDIController)
	{
		QList<int> lstSelectedRows;
		int selectionStatus = getSelectedEditorRows(lstSelectedRows);
		if (selectionStatus > 0)
		{
			QList<GTACommandBase*> cmdPasted;
			_pMDIController->onGetCopiedItems(cmdPasted);
			_pUndoStack->push(new GTAUndoRedoPaste(selectionStatus, lstSelectedRows, cmdPasted, this));
		}
		else
			QMessageBox::warning(this, "Warning", "Select a row to paste");

	}
}

void GTAEditorWindow::onOpenCallInNewWindow()
{
	if (nullptr != _pEditorController && nullptr != _pMDIController)
	{
		QList<int> lstSelectedRows;
		int selectionStatus = getSelectedEditorRows(lstSelectedRows);
		if (selectionStatus > 0)
		{
			foreach(int rowId, lstSelectedRows)
			{
				GTACommandBase* pCmd = getCommandForSelectedRow(rowId);
				GTAActionCallProcedures* pCallProcedures = dynamic_cast<GTAActionCallProcedures*>(pCmd);
				if (pCmd != nullptr && pCmd->hasReference())
				{
					GTAActionCall* pCall = dynamic_cast<GTAActionCall*>(pCmd);
					if (pCall != nullptr)
					{
						QString uuid = pCall->getRefrenceFileUUID();
						QString fileName;
						if (uuid.isEmpty())
						{
							fileName = pCall->getElement();
							emit openInNewWindow(fileName, false);

						}
						else
						{
							emit openInNewWindow(uuid, true);
						}
					}
				}
				//only for call procedures (parallel)
				else if (pCallProcedures != nullptr)
				{
					QObject* pObj = sender();
					QAction* pAction = dynamic_cast<QAction*>(pObj);
					if (pAction != nullptr)
					{
						QString procName = pAction->text();
						foreach(GTACallProcItem proc, pCallProcedures->getCallProcedures())
						{
							if (procName.compare(proc._elementName) == 0)
							{
								if (proc._UUID.isEmpty())
								{
									emit openInNewWindow(procName, false);

								}
								else
								{
									emit openInNewWindow(proc._UUID, true);
								}
							}
						}
					}
				}
				else
				{
					QMessageBox::warning(this, "Warning", "Please select a call command");
				}
			}
		}
		else
			QMessageBox::warning(this, "Warning", "Select a non empty row");

	}
}

void GTAEditorWindow::showBetweenSelectedRows(int status, QList<int> selectedRows)
{
	if (selectedRows.size() == 2)
	{
		qSort(selectedRows.begin(), selectedRows.end(), qLess<int>());
		for (int i = selectedRows.at(0) + 1; i < selectedRows.at(1); i++)
		{
			ui->EditorTV->showRow(i);
		}
	}
	else if (status == 0 || status == 1)
	{
		int totoalRows = ui->EditorTV->model()->rowCount();
		for (int i = 0; i < totoalRows; i++)
		{
			ui->EditorTV->showRow(i);
		}

	}
}
void GTAEditorWindow::onShowBetweenSelectedRows()
{
	QList <int> selectedRows;

	int status = getSelectedEditorRows(selectedRows);

	_pUndoStack->push(new GTAUndoRedoShowAllRows(status, selectedRows, this));
}

void GTAEditorWindow::onEditorCutRow()
{
	bool readOnlyCmdStatus = getReadOnlyCmdStatus();
	if (GTAElementViewModel::LIVE == _pEditorController->getDisplayContext() && readOnlyCmdStatus)
	{
		// ignore, do not perform this action on Live Editor's read only cmds
	}
	else
	{
		QList<int> lstSelectedRows;
		int selectionStatus = getSelectedEditorRows(lstSelectedRows);
		if (selectionStatus)
		{
			QHash<int, GTACommandBase*> cmdsCut;
			for (int i = 0; i < lstSelectedRows.count(); i++)
			{
				int rowID = lstSelectedRows.at(i);
				GTACommandBase* pCmd = _pEditorController->getCommand(rowID);
				if (pCmd != nullptr)
				{
					cmdsCut.insert(rowID, pCmd->getClone());
					if (pCmd->hasChildren())
					{
						int totalRowCount = rowID + pCmd->getAllChildrenCount();
						for (int j = i + 1; j < lstSelectedRows.count(); j++)
						{
							int nextIndex = lstSelectedRows.at(j);
							if (nextIndex <= totalRowCount)
							{
								lstSelectedRows.removeAt(j);
							}

						}
					}
				}
				else
				{
					pCmd = nullptr;
					cmdsCut.insert(rowID, pCmd);
				}
			}
			_pUndoStack->push(new GTAUndoRedoCut(selectionStatus, lstSelectedRows, cmdsCut, this));
			//saurav
			lastCalled = "cut";
		}
		else
		{
			QMessageBox::warning(this, "Warning", "No selection Nothing copied");
		}
	}
}

void GTAEditorWindow::onHideSelecteRowTitle()
{
	QList <int> selectedRows;
	getSelectedEditorRows(selectedRows);
	_pUndoStack->push(new GTAUndoRedoHideRow(selectedRows, this));
}

void GTAEditorWindow::createContextMenu(const QString iFileName)
{
	//Here we use the automatic deletion of QObject derived classes!
	//The only thing you need to do is: pass parent pointer to the child constructor.
	//Then, if parent is deleted, the child will be deleted as well.
	//It is explained here: http://doc.qt.io/qt-5/objecttrees.html
	//In our case, we delete _pContextMenu in destructor
	//Making _pContextMenu as parent for all QAction and QMenu will handle their deletion automatically
	//The parent concept is tried and tested. Just connect each QMenu and QAction signal destroyed() to a slot
	//the slot is hit the same number of times as many number of times the object is created

	if (nullptr != _pLiveEditorContextMenu)
	{
		delete _pLiveEditorContextMenu;
		_pLiveEditorContextMenu = nullptr;
	}

	_pContextMenu = new QMenu(this);
	QMenu* pEditorMenu = new QMenu("Editor", _pContextMenu);
	QMenu* pActionMenu = new QMenu("Action", pEditorMenu);
	QMenu* pCheckMenu = new QMenu("Check", pEditorMenu);

	//we can't use the editor controller to get the element because we have a dummy element loaded initially during the creation of the context menu
	//this causes improper population of the context menu
	GTAElement::ElemType elemtType = GTAAppController::getGTAAppController()->getElementTypeFromExtension(iFileName.split(".").last());

	QHash<QString, QStringList> actionList;
	QList<QString> checkList;
	QStringList ordrLst;
	if (elemtType == GTAElement::OBJECT)
	{
		actionList = _pMDIController->getActionCommandList(GTAUtil::OBJECT, ordrLst);
		checkList = _pMDIController->getCheckCommandList(GTAUtil::OBJECT);
	}
	else if (elemtType == GTAElement::FUNCTION)
	{
		actionList = _pMDIController->getActionCommandList(GTAUtil::FUNCTION, ordrLst);
		checkList = _pMDIController->getCheckCommandList(GTAUtil::FUNCTION);
	}
	else if (elemtType == GTAElement::PROCEDURE)
	{
		actionList = _pMDIController->getActionCommandList(GTAUtil::PROCEDURE, ordrLst);
		checkList = _pMDIController->getCheckCommandList(GTAUtil::PROCEDURE);
	}
	else if (elemtType == GTAElement::SEQUENCE)
	{
		actionList = _pMDIController->getActionCommandList(GTAUtil::SEQUENCE, ordrLst);
		checkList = _pMDIController->getCheckCommandList(GTAUtil::SEQUENCE);
	}
	else if (elemtType == GTAElement::TEMPLATE)
	{
		actionList = _pMDIController->getActionCommandList(GTAUtil::TEMPLATE, ordrLst);
		checkList = _pMDIController->getCheckCommandList(GTAUtil::TEMPLATE);
	}

	foreach(QString key, actionList.keys())
	{
		QStringList values = actionList.value(key);
		QMenu* intermediateMenu = nullptr;
		QAction* intermediateAction = nullptr;

		if (values.count() > 0)
		{
			intermediateMenu = new QMenu(key, pActionMenu);
		}
		foreach(QString value, values)
		{
			if (nullptr != intermediateMenu)
			{
				intermediateAction = new QAction(QIcon(":/images/ACTION"), value, intermediateMenu);
				connect(intermediateAction, SIGNAL(triggered()), this, SLOT(onInsertAction()));
				intermediateMenu->addAction(intermediateAction);
			}


		}
		if (nullptr != intermediateMenu)
			pActionMenu->addMenu(intermediateMenu);
		else
		{
			intermediateAction = new QAction(QIcon(":/images/ACTION"), key, pActionMenu);
			connect(intermediateAction, SIGNAL(triggered()), this, SLOT(onInsertAction()));
			pActionMenu->addAction(intermediateAction);
		}
	}
	foreach(QString check, checkList)
	{
		QAction* intermediateCheck = new QAction(QIcon(":/images/CHECK"), check, pCheckMenu);
		connect(intermediateCheck, SIGNAL(triggered()), this, SLOT(onInsertCheck()));
		pCheckMenu->addAction(intermediateCheck);
	}

	pEditorMenu->addMenu(pActionMenu);
	pEditorMenu->addMenu(pCheckMenu);

	_pContextMenu->addMenu(pEditorMenu);
	_pContextMenu->addAction(QIcon(":/images/forms/img/EDIT.png"), "Change/Edit Command", this, SLOT(onEditCommand()), QKeySequence(Qt::Key_F2));
	_pContextMenu->addSeparator();

	_pAddRow = _pContextMenu->addAction(QIcon(":/images/forms/img/ADD_ROW.png"), "Add Row", this, SLOT(onAddRow()), QKeySequence(Qt::CTRL + Qt::Key_Plus));
	_pAddRow->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Plus));
	_pEditorView->addAction(_pAddRow);

	_pDeleteRow = _pContextMenu->addAction(QIcon(":/images/forms/img/DELETE_ROW.png"), "Delete Row", this, SLOT(onDeleteRow()), QKeySequence(Qt::CTRL + Qt::Key_Minus));
	_pDeleteRow->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Minus));
	_pEditorView->addAction(_pDeleteRow);

	_pClearRow = _pContextMenu->addAction(QIcon(":/images/forms/img/CLEAR.jpg"), "Clear Row", this, SLOT(onClearRow()), QKeySequence(Qt::Key_Delete));
	_pClearRow->setShortcut(QKeySequence(Qt::Key_Delete));
	_pEditorView->addAction(_pClearRow);

	_pCutRow = _pContextMenu->addAction(QIcon(":/images/forms/img/CUT_ROW.png"), "Cut", this, SLOT(onEditorCutRow()), QKeySequence(Qt::CTRL + Qt::Key_X));
	_pCutRow->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_X));
	_pEditorView->addAction(_pCutRow);

	_pCopyRow = _pContextMenu->addAction(QIcon(":/images/forms/img/COPY_ROW.png"), "Copy", this, SLOT(onEditorCopyRow()), QKeySequence(Qt::CTRL + Qt::Key_C));
	_pCopyRow->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
	_pEditorView->addAction(_pCopyRow);

	_pPasteRow = _pContextMenu->addAction(QIcon(":/images/forms/img/PASTE_ROW.png"), "Paste", this, SLOT(onEditorPasteRow()), QKeySequence(Qt::CTRL + Qt::Key_V));
	_pPasteRow->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_V));
	_pEditorView->addAction(_pPasteRow);

	_pSelectAllRows = _pContextMenu->addAction(QIcon(":/images/forms/img/SelectAllRows.png"), "Select all rows", this, SLOT(selectAllRows()), QKeySequence(Qt::CTRL + Qt::Key_A));
	_pSelectAllRows->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_A));
	_pEditorView->addAction(_pSelectAllRows);

	_pContextMenu->addSeparator();
	_pOpenCallInNewWindow = _pContextMenu->addAction(QIcon(":/images/forms/img/NewWindow.png"), "Open in new window", this, SLOT(onOpenCallInNewWindow()));
	_pOpenCallInNewWindowMenu = new QMenu("Open in new window", _pContextMenu);
	_pContextMenu->addMenu(_pOpenCallInNewWindowMenu);

	_pContextMenu->addSeparator();
	_pMoveRowUpAction = _pContextMenu->addAction(QIcon(":/images/forms/img/UpArrow_16_16.png"), "Move rows up", this, SLOT(onMoveEditorRowUp()), QKeySequence(Qt::ALT + Qt::Key_Up));
	_pMoveDownAction = _pContextMenu->addAction(QIcon(":/images/forms/img/DownArrow_16_16.png"), "Move rows down", this, SLOT(onMoveEditorRowDown()), QKeySequence(Qt::ALT + Qt::Key_Down));

	_pContextMenu->addAction(QIcon(":/images/forms/img/ignoreOn.png"), "Ignore On", this, SLOT(onIgnoreOnSCXML()), QKeySequence(Qt::CTRL + Qt::Key_I));
	_pContextMenu->addAction(QIcon(":/images/forms/img/ignoreOff.png"), "Ignore off", this, SLOT(onIgnoreOffSCXML()), QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_I));
	
	_pContextMenu->addAction(QIcon(":/images/forms/img/readOnly.png"), "Toggle ReadOnly", this, SLOT(onSetReadOnlyStatusForCmd()), QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_U));

	_pEsc = _pContextMenu->addAction("Switch to Edit Mode", this, SLOT(onEscapeKeyTriggered()), QKeySequence(Qt::Key_Escape));
	_pEditorView->addAction(_pEsc);

	QAction* pShowDetail = _pContextMenu->addAction(QIcon(":/images/forms/img/Relation.png"),
		"Show Details",
		this, SLOT(onShowCommandDetail()));
	pShowDetail->setToolTip("Shows expanded view on applicable action");
	pShowDetail->setEnabled(false);

}

void GTAEditorWindow::onShowCommandDetail()
{
	if (_pMDIController)
	{

		QAbstractItemModel* pModel = nullptr;
		QList<int> lstSelectedRows;
		getSelectedEditorRows(lstSelectedRows);

		if (!lstSelectedRows.isEmpty())
		{
			GTAElement* pElement = _pEditorController->getElement();
			pElement->updateCallCommands();
			if (_pMDIController->getModelForTreeView(pModel, lstSelectedRows.at(0), pElement))
			{
				if (nullptr != pModel)
				{
					GTAElementTreeViewWidget* pWidget = new GTAElementTreeViewWidget(this);
					pModel->setParent(pWidget);
					pWidget->setModel(pModel);
					pWidget->setMinimumWidth(1000);


					pWidget->setName(pModel->objectName());
					pWidget->show();
				}
			}
			else
			{
				QString lastError = _pMDIController->getLastError();
				QMessageBox::critical(this, "TreeView error", lastError);
			}
		}

	}
}

void GTAEditorWindow::onSetReadOnlyStatusForCmd()
{
	QList <int> selectedRows;
	getSelectedEditorRows(selectedRows);
	qSort(selectedRows.begin(), selectedRows.end(), qLess<int>());
	if (!selectedRows.isEmpty())
	{
		int count = selectedRows.size();
		for (int i = count - 1; i >= 0; i--)
		{
			int selectedRow = selectedRows.at(i);
			GTACommandBase* pCurrentCommand = getCommandForSelectedRow(selectedRow);
			if (nullptr != pCurrentCommand)
			{
				_pUndoStack->push(new GTAUndoRedoToggleReadOnly(pCurrentCommand, this));
			}
		}
		updateEditorView();
	}
}

void GTAEditorWindow::onIgnoreOnSCXML()
{
	ignoreInSCXMLOnOff(true);
}

void GTAEditorWindow::onIgnoreOffSCXML()
{
	ignoreInSCXMLOnOff(false);
}

void GTAEditorWindow::selectAllRows()
{
	ui->EditorTV->selectAll();
}

std::tuple<QList<int>, bool> GTAEditorWindow::selectRow(int row)
{
	bool bEditOK = false;
	QList<int> selectedRow;

	ui->EditorTV->selectRow(row);
	int selectionStatus = getSelectedEditorRows(selectedRow);

	if (selectionStatus == 0)
	{
		QMessageBox::critical(this, "Error", "Select Row to perform action editing.");
	}
	else if (selectedRow.size() > 1)
	{
		QMessageBox msgBox(this);
		QString text("Multiple selection detected. Edit first selection?");
		msgBox.setText(text);
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.setWindowTitle("Delete Row Warning!!!");
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msgBox.setDefaultButton(QMessageBox::No);
		int ret = msgBox.exec();
		if (ret == QMessageBox::Yes)
		{
			bEditOK = true;
		}
	}
	else
		bEditOK = true;

	return { selectedRow, bEditOK };
}

QHash<QString, QStringList> GTAEditorWindow::getActionCommandList()
{
	QHash<QString, QStringList> actionCmdList;
	QStringList orderLst;

	GTAElement* pElement = _pEditorController->getElement();
	if (pElement != nullptr)
	{
		GTAElement::ElemType elemtType = pElement->getElementType();
		if (elemtType == GTAElement::OBJECT)
			actionCmdList = _pMDIController->getActionCommandList(GTAUtil::OBJECT, orderLst);
		else if (elemtType == GTAElement::FUNCTION)
			actionCmdList = _pMDIController->getActionCommandList(GTAUtil::FUNCTION, orderLst);
		else if (elemtType == GTAElement::PROCEDURE)
			actionCmdList = _pMDIController->getActionCommandList(GTAUtil::PROCEDURE, orderLst);
		else if (elemtType == GTAElement::SEQUENCE)
			actionCmdList = _pMDIController->getActionCommandList(GTAUtil::SEQUENCE, orderLst);
		else if (elemtType == GTAElement::TEMPLATE)
			actionCmdList = _pMDIController->getActionCommandList(GTAUtil::TEMPLATE, orderLst);
	}

	return actionCmdList;
}

void GTAEditorWindow::setCustomizedActionCommandInWidget(const QString& actionName)
{
	QHash<QString, QStringList> actionCmd;
	QStringList orderCmd;
	QHash<QString, QStringList> actionCmdList = getActionCommandList();

	if (actionCmdList.keys().contains(actionName)) {
		actionCmd[actionName] = actionCmdList[actionName];
		orderCmd.append(actionName);
	}
	else {
		//TODO: error handling
	}

	_pActionWindow->setCommandList(actionCmd, orderCmd);
}

std::tuple<QString, QString> GTAEditorWindow::setActionMenuSenders(QAction* pActionSender)
{
	QString actionsender = "";
	QString menusender = "";

	if (nullptr != pActionSender && nullptr != pActionSender->parent())
	{
		actionsender = pActionSender->text();

		QMenu* pMenuSender = dynamic_cast<QMenu*>(pActionSender->parent());
		if (nullptr != pMenuSender)
		{
			menusender = pMenuSender->title();
		}
	}
	
	return { actionsender, menusender };
}

bool GTAEditorWindow::isActionComplementWrong(GTAActionBase* pAction, const QString& actionsender, const QString& menusender, QString& action, QString& complement)
{
	action = pAction->getAction();
	complement = pAction->getComplement();
	bool wrongaction = true;

	if (complement.isEmpty())
	{
		//check if action sender is equal to action
		if (actionsender == action)
			wrongaction = false;
	}
	else
	{
		//check actionsender is equal to complement and menusender is equal to action
		if ((actionsender == complement) && (menusender == action))
			wrongaction = false;
	}

	//if call comes from context menu Change/Edit
	//if call comes directly from F2 key press
	if (actionsender.isEmpty() || actionsender.contains("F2", Qt::CaseInsensitive))
	{
		wrongaction = false;
	}

	return wrongaction;
}

void GTAEditorWindow::setActionEditableDependingOnParam()
{
	if (_ReadOnlyDocToggled == true)
		_pActionWindow->setEditableAction(false);
	else
		_pActionWindow->setEditableAction(true);
}

static int rowIdx = 0;
void GTAEditorWindow::setRowIndex(int idx)
{
	rowIdx = idx;
}

void GTAEditorWindow::appendCustomizedAction(std::shared_ptr<CustomizedAction> action, ErrorMessageList& errorLogs)
{
	auto [actionName, actionComplement, childWgtParams, wgtParams] = action->split();
	//static int rowIdx = 0;
	onAddRow();
	auto [selectedRow, bEditOK] = selectRow(rowIdx);

	if (_pActionWindow != nullptr && _pEditorController != nullptr && _pMDIController != nullptr && bEditOK == true)
	{
		setCustomizedActionCommandInWidget(actionName);
		GTAActionBase* pAction = nullptr;
		_pActionWindow->setActionCommand(pAction, actionName, actionComplement);
		setActionEditableDependingOnParam();

		bool rc = _pActionWindow->getCurrentWidget()->setWidgetFields(childWgtParams, errorLogs);
		rc = rc && _pActionWindow->setActionSelectorWidgetFields(wgtParams);
		if (rc)
		{
			_pActionWindow->setShowErrorBoxes(false);
			_pActionWindow->clickOk();
			_pActionWindow->setShowErrorBoxes(true);
			auto errorLog = _pActionWindow->getLastErrorAndResetIt();
			if (!errorLog.isEmpty()) {
				errorLogs.append(errorLog);
			}
			rowIdx++;
		}
		else
			_pActionWindow->clickCancel();
	}
}

QList<QString> GTAEditorWindow::getCheckCommandList()
{
	QList<QString> checkCommandList;
	//        Dev Comment : Decide how to handle which actions to load
	GTAElement* pElement = _pEditorController->getElement();
	if (pElement != nullptr)
	{
		GTAElement::ElemType elemtType = pElement->getElementType();
		if (elemtType == GTAElement::OBJECT)
			checkCommandList = _pMDIController->getCheckCommandList(GTAUtil::OBJECT);
		else if (elemtType == GTAElement::FUNCTION)
			checkCommandList = _pMDIController->getCheckCommandList(GTAUtil::FUNCTION);
		else if (elemtType == GTAElement::PROCEDURE)
			checkCommandList = _pMDIController->getCheckCommandList(GTAUtil::PROCEDURE);
		else if (elemtType == GTAElement::SEQUENCE)
			checkCommandList = _pMDIController->getCheckCommandList(GTAUtil::SEQUENCE);
		else if (elemtType == GTAElement::TEMPLATE)
			checkCommandList = _pMDIController->getCheckCommandList(GTAUtil::TEMPLATE);

	}
	return checkCommandList;
}

void GTAEditorWindow::setCheckEditableDependingOnParam()
{
	if (_ReadOnlyDocToggled == true)
		_pCheckWindow->setEditableAction(false);
	else
		_pCheckWindow->setEditableAction(true);
}

void GTAEditorWindow::appendCustomizedCheck(std::shared_ptr<CustomizedCheck> check, ErrorMessageList& errorLogs)
{
	auto [checkName, childWgtParams, wgtParams] = check->split();

	onAddRow();
	auto [selectedRow, bEditOK] = selectRow(rowIdx);
	
	if (_pEditorController != nullptr && _pCheckWindow != nullptr && _pMDIController != nullptr && bEditOK == true)
	{
		if (getCheckCommandList().contains(checkName))
			_pCheckWindow->setChecksList({ checkName });
		else {
			errorLogs.append(QString("Command list does not contain %1").arg(checkName));
		}

		setCheckEditableDependingOnParam();
		GTACheckBase* pCheck = nullptr;
		_pCheckWindow->setCheckCommand(pCheck, checkName);
		bool rc = _pCheckWindow->getCurrentWidget()->setWidgetFields(childWgtParams, errorLogs);
		rc = _pCheckWindow->setCheckControlWidgetFields(wgtParams);
		if (rc)
		{
			_pCheckWindow->clickOk();
			rowIdx++;
		}
		else
			_pCheckWindow->clickCancel();
	}
}

void GTAEditorWindow::getManualActionRows(QList<int>& oManualActionRows)
{
	selectAllRows();
	QList <int> selectedRows;
	getSelectedEditorRows(selectedRows);
	qSort(selectedRows.begin(), selectedRows.end(), qLess<int>());
	if (!selectedRows.isEmpty())
	{
		int count = selectedRows.size();
		for (int i = count - 1; i >= 0; i--)
		{

			int selectedRow = selectedRows.at(i);
			GTACommandBase* pCurrentCommand = getCommandForSelectedRow(selectedRow);
			GTAActionBase* action = dynamic_cast<GTAActionBase*>(pCurrentCommand);
			if (action == nullptr)
			{
				continue;
			}
			if (action->getAction().contains(ACT_MANUAL))
			{
				oManualActionRows.append(i);
			}
		}  
	}
	qSort(oManualActionRows.begin(), oManualActionRows.end());
	clearSelectedRows();
}

/**
 * @brief Get all dependencies of a given manual action
 * @param iRow the index of the manual action
 * @param oManualActionDep list of indexes of its dependencies (UserFeedBack check value)
*/
void GTAEditorWindow::getManualActionDependencies(const int& iRow, QList<int>& oManualActionDep, QList<int>& oManualActionDepWhichAreManAct)
{
	QList<int> allRows;
	getAllRows(allRows);
	if (iRow + 1 == allRows.count())
		return;

	GTAActionManual* manAct = dynamic_cast<GTAActionManual*>(getCommandForSelectedRow(iRow));
	QString param = manAct->getParameter();
	if (param.isEmpty() || manAct->getAcknowledgetment())
		return; // ManAct waits for acknowledgment so no dependencies

	// Search possible dependencies on every rows below the manual action
	for (int i = iRow + 1; i < allRows.count(); i++)
	{
		int currentRow = allRows.at(i);
		GTACommandBase* pCurrentCommand = getCommandForSelectedRow(currentRow);

		// Row of CHECK
		if (pCurrentCommand->getCommandType() == GTACommandBase::CHECK)
		{
			GTACheckValue* check = dynamic_cast<GTACheckValue*>(pCurrentCommand);
			QStringList listParam = check->getListParameters();
			if (listParam.contains(param))
			{
				oManualActionDep.append(i); // CheckValue parameters contain ManAct parameter
			}
		}

		// Row of ACTION
		if (pCurrentCommand->getCommandType() == GTACommandBase::ACTION)
		{
			GTAActionBase* action = dynamic_cast<GTAActionBase*>(pCurrentCommand);

			// Row of another ManAct
			if (action->getAction().contains(ACT_MANUAL))
			{
				GTAActionManual* nextManAct = dynamic_cast<GTAActionManual*>(action);

				// if parent in oManuAlctionDep then append current manAct in oManualActionDep
				bool parentIsDep = false;				
				auto parent = pCurrentCommand->getParent();
				if (parent)
				{
					foreach(auto iRow, oManualActionDep)
					{
						GTACommandBase* depCmd = _pEditorController->getCommand(iRow);						
						if (depCmd->getRefrenceFileUUID() == parent->getRefrenceFileUUID())
						{
							oManualActionDepWhichAreManAct.append(i);
							parentIsDep = true;
							break;
						}
					}
				}

				if (nextManAct->getAcknowledgetment())
				{																		
					if (!parentIsDep)
						continue; // next ManAct waits for acknowledgment
				}
				if (param == nextManAct->getParameter() && !nextManAct->isIgnoredInSCXML())
				{
					if (!parentIsDep)
						break; // next not ignored ManAct reuses the same parameter
				}
			}
			else
			{
				QString complement = action->getComplement();
				if (!complement.isEmpty())
				{
					// Row of neither ManAct nor SET action
					QStringList listParam = action->getVariableList();
					if (listParam.contains(param))
					{
						oManualActionDep.append(i); // current Action parameters contain ManAct parameter
					}
				}
				else
				{
					// Row of SET action
					if (action->getAction().contains(ACT_SET))
					{
						GTAActionSetList* setList = dynamic_cast<GTAActionSetList*>(action);
						QStringList setParam = setList->getVariableList();						
						if (setParam.contains(param))
						{
							auto parent = action->getParent();
							if (parent != NULL && parent->getVariableList().contains(param))
							{
								continue; // the parent should already be ignored thus this SET action also
							}
							else
							{
								if (!setList->isIgnoredInSCXML())
								{
									break; // next not ignored SET action reuses the same parameter
								}								
							}
						}
					}
				}
			}
		}
	}
}

/**
 * @brief Get the list of all the rows
 * @param oAllRows list of all the rows
*/
void GTAEditorWindow::getAllRows(QList<int>& oAllRows)
{
	selectAllRows();
	getSelectedEditorRows(oAllRows);
	clearSelectedRows();
}

void GTAEditorWindow::ignoreInSCXMLOnOff(bool iIgnoreStatus)
{
	if (_pEditorController)
	{
		QList <int> selectedRows;
		getSelectedEditorRows(selectedRows);
		qSort(selectedRows.begin(), selectedRows.end(), qLess<int>());
		if (!selectedRows.isEmpty())
		{
			int count = selectedRows.size();
			bool hasTitle = false;
			for (int i = count - 1; i >= 0; i--)
			{
				int selectedRow = selectedRows.at(i);
				GTACommandBase* pCurrentCommand = getCommandForSelectedRow(selectedRow);
				if (nullptr != pCurrentCommand)
				{
					if (!pCurrentCommand->isTitle())
					{
						if (pCurrentCommand->IsEndCommand())
						{
							GTACommandBase* pParent = pCurrentCommand->getParent();
							if (pParent != nullptr)
							{
								_pUndoStack->push(new GTAUndoRedoIgnoreOnOff(pParent, iIgnoreStatus, this));
							}
						}
						toggleIgnore(pCurrentCommand, iIgnoreStatus);
						setEndCmdIgnore(pCurrentCommand, iIgnoreStatus);
					}
					else
					{
						hasTitle = true;
					}
				}
			}
			if (hasTitle && iIgnoreStatus)
			{
				QMessageBox::information(this, "Info", "Can not ignore a title.");
			}

		}

		_pEditorController->updateEditorView();
		ui->EditorTV->update();
		clearSelectedRows();
	}
}

/**
 * @brief Ignore selected manual actions and their "dependencies" (UserFeedBack check value)
 * @param manAcToBeIgnored list of manual actions to be ignored 
*/
void GTAEditorWindow::ignoreManualActions(const QList<int> manAcToBeIgnored)
{
	QList<int> allManAct;
	getManualActionRows(allManAct);
	QList<int> allManAcToBeIgnored = manAcToBeIgnored;
	foreach(int manActRow, allManAct)
	{
		// Ignore OnOff current manual action
		bool status = allManAcToBeIgnored.contains(manActRow);
		selectRow(manActRow);
		ignoreInSCXMLOnOff(status);

		// Ignore OnOff its dependencies
		QList<int> dependencies, dependenciesManAct;
		getManualActionDependencies(manActRow, dependencies, dependenciesManAct);

		// Update manActToBeIgnored with nested manAct
		foreach(int iRow, dependenciesManAct)
		{
			if (!allManAcToBeIgnored.contains(iRow) && status)
				allManAcToBeIgnored.append(iRow);
		}
		
		foreach(int dependencyRow, dependencies)
		{
			selectRow(dependencyRow);
			ignoreInSCXMLOnOff(status);
		}
	}
	clearSelectedRows();
}

void GTAEditorWindow::setEndCmdIgnore(GTACommandBase* pCmd, bool iIgnoreStatus)
{
	GTACommandBase* pParent = pCmd->getParent();
	if (pParent != nullptr)
	{
		QList<GTACommandBase*> chn = pParent->getChildren();
		GTACommandBase* pEndChild = chn.last();
		if (pEndChild != nullptr && pEndChild->IsEndCommand())
		{
			_pUndoStack->push(new GTAUndoRedoIgnoreOnOff(pEndChild, iIgnoreStatus, this));
		}
		setEndCmdIgnore(pParent, iIgnoreStatus);
	}
}

void GTAEditorWindow::toggleIgnore(GTACommandBase* pCurrentCommand, bool iIgnoreStatus)
{
	GTACommandBase* pParent = pCurrentCommand->getParent();
	//for all loop commands that can have children but are not title
	if (pParent != nullptr && pParent->canHaveChildren() && !pParent->isTitle())
	{
		int chnCnt = pParent->getAbsoluteChildrenCount();
		QList<GTACommandBase*> chn = pParent->getChildren();

		foreach(GTACommandBase * child, chn)
		{
			if (child == nullptr)
			{
				chn.removeOne(child);
			}
		}
		// int chnCnt = chn.count();
		GTAActionElse* pElse = dynamic_cast<GTAActionElse*> (pParent);
		if (pElse == nullptr)
			chnCnt = chnCnt - 1;
		if (pCurrentCommand->hasChildren())
		{
			chnCnt = chnCnt - pCurrentCommand->getAbsoluteChildrenCount() - 1;
		}
		else
		{
			chnCnt = chnCnt - 1;
		}
		checkIgnoreStatusOfChildren(pParent, chnCnt, iIgnoreStatus);

		if (chnCnt == 0)
		{

			toggleIgnore(pParent, iIgnoreStatus);
			_pUndoStack->push(new GTAUndoRedoIgnoreOnOff(pCurrentCommand, iIgnoreStatus, this));
			if (pCurrentCommand->hasChildren())
			{
				QList<GTACommandBase*> children = pCurrentCommand->getChildren();
				GTACommandBase* pEndChild = children.last();
				if (pEndChild != nullptr && pEndChild->IsEndCommand())
				{
					_pUndoStack->push(new GTAUndoRedoIgnoreOnOff(pEndChild, iIgnoreStatus, this));
				}

			}
		}
		else
		{
			_pUndoStack->push(new GTAUndoRedoIgnoreOnOff(pCurrentCommand, iIgnoreStatus, this));
			if (pCurrentCommand->hasChildren())
			{
				QList<GTACommandBase*> children = pCurrentCommand->getChildren();
				GTACommandBase* pEndChild = children.last();
				if (pEndChild != nullptr && pEndChild->IsEndCommand())
				{
					_pUndoStack->push(new GTAUndoRedoIgnoreOnOff(pEndChild, iIgnoreStatus, this));
				}

			}
			if (pParent->isIgnoredInSCXML() == iIgnoreStatus && pElse == nullptr)
			{
				QList<GTACommandBase*> children = pParent->getChildren();
				GTACommandBase* pEndChild = children.last();
				if (pEndChild != nullptr && pEndChild->IsEndCommand())
				{
					_pUndoStack->push(new GTAUndoRedoIgnoreOnOff(pEndChild, iIgnoreStatus, this));
				}
			}

		}
	}
	else if (nullptr == pParent)
	{
		_pUndoStack->push(new GTAUndoRedoIgnoreOnOff(pCurrentCommand, iIgnoreStatus, this));
		if (pCurrentCommand->hasChildren())
		{
			QList<GTACommandBase*> children = pCurrentCommand->getChildren();
			GTACommandBase* pEndChild = children.last();
			if (pEndChild != nullptr && pEndChild->IsEndCommand())
			{
				_pUndoStack->push(new GTAUndoRedoIgnoreOnOff(pEndChild, iIgnoreStatus, this));
			}

		}
	}
}

void GTAEditorWindow::checkIgnoreStatusOfChildren(GTACommandBase* pCurrentCommand, int& count, bool iIgnoreStatus)
{
	QList<GTACommandBase*> chn = pCurrentCommand->getChildren();
	foreach(GTACommandBase * child, chn)
	{
		if (child != nullptr && child->isIgnoredInSCXML() == iIgnoreStatus)
		{
			if (child->hasChildren())
				checkIgnoreStatusOfChildren(child, count, iIgnoreStatus);
			count--;
		}
	}
}


void GTAEditorWindow::traverseChildren(GTACommandBase* pParent, int& counterCommands, bool iIgnoreStatus, GTACommandBase* pEndChild)
{
	QList<GTACommandBase*> chnCnt = pParent->getChildren();
	int childrenCnt = chnCnt.count();
	for (int i = 0; i < childrenCnt; i++)
	{
		GTACommandBase* pChild = pParent->getChildren().at(i);
		if (iIgnoreStatus)
		{
			if (pChild != nullptr && !pChild->IsEndCommand() && !pChild->isIgnoredInSCXML())
			{
				if (pChild->hasChildren())
					traverseChildren(pChild, counterCommands, iIgnoreStatus, nullptr);

				++counterCommands;
			}
		}
		else
		{
			if (pChild == nullptr)
			{
				++counterCommands;
			}
			else if (pChild != nullptr && !pChild->IsEndCommand() && pChild->isIgnoredInSCXML())
			{
				if (pChild->hasChildren())
					traverseChildren(pChild, counterCommands, iIgnoreStatus, nullptr);

				++counterCommands;
			}

			if (pChild != nullptr && pChild->IsEndCommand())
				pEndChild = pChild;
		}
	}
}

void GTAEditorWindow::updateEditorView()
{
	if (_pEditorController)
	{
		_pEditorController->updateEditorView();
	}
}
void GTAEditorWindow::moveEditorRowUp(const QList<int> selectedRows)
{
	if (_pEditorController)
	{

		int selectedRow = 0;
		selectedRow = selectedRows.at(0);

		if (_pEditorController->moveEditorRowItemUp(selectedRow))
		{
			_rowToBeSelected = _pEditorController->getRowToBeSelected();
			ui->EditorTV->selectRow(_rowToBeSelected);
		}

		//avoid any highlight if it exists
		highLightLastChild(QModelIndex());
		this->setMoveUpDownButtonEnabled(QModelIndex());
	}


}
void GTAEditorWindow::onMoveEditorRowUp()
{
	QList<int> selectedRows;
	getSelectedEditorRows(selectedRows);
	if (selectedRows.size() == 1)
	{
		_pUndoStack->push(new GTAUndoRedoMoveRowsUp(selectedRows.at(0), this));
	}
}
void GTAEditorWindow::moveEditorRowDown(const QList<int> selectedRows)
{
	if (_pEditorController)
	{

		if (_pEditorController->moveEditorRowItemDown(selectedRows.at(0)))
		{
			_rowToBeSelected = _pEditorController->getRowToBeSelected();
			ui->EditorTV->selectRow(_rowToBeSelected);
			setMoveUpDownButtonEnabled(QModelIndex());
		}

		//avoid any highlight if it exists
		highLightLastChild(QModelIndex());
		setMoveUpDownButtonEnabled(QModelIndex());
	}
}
void GTAEditorWindow::onMoveEditorRowDown()
{

	//bool rc = false;
	QList<int> selectedRows;
	getSelectedEditorRows(selectedRows);
	if (selectedRows.size() == 1)
	{
		_pUndoStack->push(new GTAUndoRedoMoveRowsDown(selectedRows.at(0), this));
	}
}

void GTAEditorWindow::onCollapseAllToTitle(bool iCollapseTitle)
{
	_pUndoStack->push(new GTAUndoRedoCollapseToTitle(iCollapseTitle, this));
}


void GTAEditorWindow::collapseAllToTitle(const bool& iCollapseTitle)
{
	if (_pEditorController)
	{
		QList<int> rows;
		_pEditorController->getRowsContainingTitle(rows);
		if (rows.size())
		{

			_titleCollapseToggled = iCollapseTitle;
			emit collapseToTitle(iCollapseTitle);
			int totalRows = ui->EditorTV->model()->rowCount();
			for (int i = 0; i < totalRows; i++)
			{
				if (!rows.contains(i) && iCollapseTitle == true)
					ui->EditorTV->hideRow(i);
				else
				{
					if (!rows.contains(i))
					{
						ui->EditorTV->showRow(i);
					}
				}
			}
		}
		else
		{
			return;
		}
		if (iCollapseTitle == false)
		{
			QTimer::singleShot(0, this, SLOT(ScrollEditorPage()));
			disableEditingActions(false);
			emit disableEditingActionsInMain(false);

		}
		else
		{
			disableEditingActions(true);
			emit disableEditingActionsInMain(true);
		}

		emit toggleTitleCollapseActionIcon(iCollapseTitle);
	}
}

void GTAEditorWindow::ScrollEditorPage()
{
	ui->EditorTV->scrollTo(ui->EditorTV->currentIndex());
}

void GTAEditorWindow::resizeEditorToContents()
{
	ui->EditorTV->setWordWrap(true);


	if (ui->EditorTV != nullptr)
	{
		if (ui->EditorTV->model() != nullptr)
		{
			int colCnt = ui->EditorTV->model()->columnCount();
			for (int i = 0; i < colCnt; i++)
			{
				ui->EditorTV->resizeColumnToContents(i);
			}

		}
	}

	ui->EditorTV->resizeRowsToContents();
}

void GTAEditorWindow::disableEditingActions(bool bDisable)
{
	if (_pContextMenu)
		_pContextMenu->setDisabled(bDisable);
}

void GTAEditorWindow::onClearPage()
{

}

void GTAEditorWindow::onReadOnlyDocToggled(bool isReadOnlyDocToggled)
{
	if (_pEditorController)
	{
		GTAElement* pCurrentElem = _pEditorController->getElement();
		if (pCurrentElem)
		{
			_ReadOnlyDocToggled = isReadOnlyDocToggled;
			emit updateReadOnlyDocStatus(isReadOnlyDocToggled);
			pCurrentElem->setSaveAsReadOnly(_ReadOnlyDocToggled);

		}
	}
}


void GTAEditorWindow::onSetFocus()
{
	this->QWidget::setFocus();
}

void GTAEditorWindow::focusInEvent(QFocusEvent* event)
{
	emit toggleTitleCollapseActionIcon(_titleCollapseToggled);
	emit updateReadOnlyDocStatus(_ReadOnlyDocToggled);
	emit updateActiveViewController(_pEditorController);
	emit updateActiveStack(_pUndoStack);
	emit updateClearAllIcon(isDataEmpty());
	this->QWidget::focusInEvent(event);
}

void GTAEditorWindow::closeEvent(QCloseEvent* event)
{
	//QundoStack maintains a QList<QUndoCommand*> and always appends to this list
	//the list is never popped. The only change that is made is on the index pointing to that list
	//the index is incremented or decremented based on undo/redo
	//the index can be externally set too. But we don't do that in our case
	//checking the index while saving and closing will give us a clear idea for any changes after last save

	if (!_isGlobalSave)
	{
		int usrResponse = QMessageBox::No;
		if (_lastindex != _pUndoStack->index())
		{
			usrResponse = saveUnsavedFile();
		}


		if (usrResponse != QMessageBox::Cancel)
		{
			emit removeStack(_pUndoStack);
			event->accept();
			this->QWidget::closeEvent(event);
		}
		else
		{
			event->ignore();
		}

	}
	else
	{

		if (_globalSaveStatus == 1)
			saveFile();
		emit removeStack(_pUndoStack);
		event->accept();
		this->QWidget::closeEvent(event);
		_isGlobalSave = false;
		_globalSaveStatus = 0;

	}

	//closing the server
	ShiftF6KeyPressed();
}


void GTAEditorWindow::setGlobalSave(bool val)
{
	_isGlobalSave = val;
}

void GTAEditorWindow::setGlobalSaveStatus(int& iStatus)
{
	_globalSaveStatus = iStatus;
}

bool GTAEditorWindow::isFileModified(bool emitSignal)
{
	bool isModified = true;
	if (_pUndoStack != nullptr) 
	{
		QString newWindowTitle;
		
		if (_lastindex != _pUndoStack->index())
			newWindowTitle = "* " + _windowTitle;
		else
		{
			newWindowTitle = _windowTitle;
			isModified = false;
		}
		setWindowTitle(newWindowTitle);
		if (emitSignal)
			emit fileHasChanged(_framePosition, newWindowTitle);
	}
	return isModified;
}

bool GTAEditorWindow::saveFile()
{
	bool rc = false;
	if (_pEditorController != nullptr && _pMDIController != nullptr)
	{

		GTAElement* pElement = _pEditorController->getElement();
		GTAHeader* pHeader = _pEditorController->getHeader();

		if (!_elementName.isEmpty() && !_relativePath.isEmpty())
		{

			QString fileName = _elementName.mid(0, _elementName.indexOf("."));

			bool overWrite = true;

			rc = _pMDIController->saveDocument(fileName, overWrite, pElement, pHeader, _relativePath);

			if (rc)
			{
				QString filePath = getAbsolutePath();
				emit saveEditorDocFinished(filePath);
			}
		}
	}
	return rc;
}


int GTAEditorWindow::saveUnsavedFile()
{

	int retVal = QMessageBox::warning(this, QString("Save"), QString("File Modified. Save changes to %1?").arg(_elementName), QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
	if (retVal == QMessageBox::Yes)
	{
		bool rc = saveFile();
		if (!rc)
		{
			QMessageBox::warning(this, "Document Save", _pMDIController->getLastError());
		}
	}
	return retVal;
}


void GTAEditorWindow::show()
{
	// Following fixes the issue of the 0th column i.e the breakpoint column to resize when a new element is created
	QHeaderView* header = ui->EditorTV->horizontalHeader();
	if (header)
	{
		header->setSectionResizeMode(QHeaderView::Interactive);
		ui->EditorTV->setColumnWidth(GTAElementViewModel::BREAKPOINT_INDEX, BREAKPOINT_COL_WIDTH);
		header->setSectionResizeMode(GTAElementViewModel::BREAKPOINT_INDEX, QHeaderView::Fixed);

		header->setStretchLastSection(true);
	}

	ui->EditorTV->setSelectionMode(QAbstractItemView::ExtendedSelection);
	ui->EditorTV->setColumnWidth(GTAElementViewModel::ACTION_INDEX, 800);

	this->QWidget::show();
}

void GTAEditorWindow::setFileDetails(const QString& iElementName, const QString& iRelativePath, const QString& iFileType, const QString& iAuthorName, const QString& iMaxTimeEstimated)
{
	_elementName = iElementName;
	_relativePath = iRelativePath;
	setFileType(iFileType);
	setAuthorName(iAuthorName); //V26 editor view author column enhancement #322480
	setMaxTimeEstimated(iMaxTimeEstimated);
	if (_pEditorController != nullptr)
	{
		_pEditorController->establishConnectionWithModel();
	}
}

/**
 * @brief Set the input csv file in the current element
 * @param iInputCsvFilePath 
*/
void GTAEditorWindow::setInputCsvFilePath(const QString& iInputCsvFilePath)
{
	if (_pEditorController)
	{
		GTAElement* pElement = _pEditorController->getElement();
		pElement->setInputCsvFilePath(iInputCsvFilePath);
	}
}

/**
 * @brief Set the output csv file in the current element
 * @param iOutputCsvFilePath
*/
void GTAEditorWindow::setOutputCsvFilePath(const QString& iOutputCsvFilePath)
{
	if (_pEditorController)
	{
		GTAElement* pElement = _pEditorController->getElement();
		pElement->setOutputCsvFilePath(iOutputCsvFilePath);
	}
}

QString GTAEditorWindow::getExtension(const QString& iElementName)
{
	QString extension = iElementName;
	QString fileName = extension.mid(0, extension.indexOf("."));
	extension.replace(fileName, "");
	extension.trimmed();
	return extension;

}


void GTAEditorWindow::undoClearAll(GTAElement* pElement)
{
	if (_pEditorController)
	{

		_pEditorController->setElement(pElement);

		QAbstractItemModel* pEditorModel = nullptr;
		QAbstractItemModel* pHeaderModel = nullptr;
		_pEditorController->getElementModel(pEditorModel);
		_pEditorController->getHeaderModel(pHeaderModel);

		if (nullptr != pEditorModel)
		{

			ui->EditorTV->setModel(pEditorModel);
			for (int i = 0; i < _ColWidths.size(); i++)
				ui->EditorTV->setColumnWidth(i, _ColWidths.at(i));
			ui->EditorTV->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
			ui->EditorTV->horizontalHeader()->setSectionResizeMode(GTAElementViewModel::BREAKPOINT_INDEX, QHeaderView::Fixed);

			ui->EditorTV->horizontalHeader()->setStretchLastSection(true);
			ui->EditorTV->setSelectionMode(QAbstractItemView::ExtendedSelection);
			ui->EditorTV->setColumnWidth(GTAElementViewModel::ACTION_INDEX, 800);

			GTAElement* pElement = nullptr;

			showAllRows();

			QStringList hiddenRows = _pEditorController->getHiddenRows();
			if (hiddenRows.isEmpty() == false)
			{
				QList<int> rows;
				for (int i = 0; i < hiddenRows.size(); i++)
				{
					QString rowItem = hiddenRows.at(i);
					if (rowItem.isEmpty() == false)
					{
						bool isNum = false;
						int dNum = rowItem.toInt(&isNum);
						if (isNum == true)
							rows.append(dNum);

					}
				}
				if (rows.isEmpty() == false)
				{
					HideRows(rows);
				}
			}

			if (pElement != nullptr && pElement->isReadOnly() == true)
				emit disableEditingActionsInMain(true);
			else
				emit disableEditingActionsInMain(false);

		}
		else
		{
			ui->EditorTV->setModel(nullptr);
		}


		if (_pHeaderWidget != nullptr)
		{
			_pHeaderWidget->setModel(pHeaderModel);
		}

		ui->EditorTV->show();
		ui->EditorTV->scrollToTop();
		emit toggleTitleCollapseActionIcon(_titleCollapseToggled);
		emit updateClearAllIcon(isDataEmpty());
	}
}

void GTAEditorWindow::clearEditorPage()
{
	if (_pUndoStack != nullptr && _pEditorController != nullptr)
	{
		if (!isDataEmpty())
		{
			GTAElement* pElement = _pEditorController->getElement();

			_pUndoStack->push(new GTAUndoRedoClearAll(pElement, this));
		}
	}

}

void GTAEditorWindow::clearAll()
{
	if (_pEditorController)
	{
		_pEditorController->clearAllinEditor();
		_pEditorController->updateEditorView();
		emit updateClearAllIcon(isDataEmpty());
	}
}

void GTAEditorWindow::displayHeader()
{	
	QString headerName = _elementName.split(".")[0];
	elementCreationSuccessful();
	if (_pHeaderWidget != nullptr)
	{
		_pHeaderWidget->hide();
		_pHeaderWidget->setWindowTitle(QString("Header - %2").arg(_elementName));		
		_pHeaderWidget->show();
	}
	//onSave();
}

/**
 * @brief this function shows the summary and conclusion dialog on menu item click
 */
void GTAEditorWindow::displayPurposeAndConclusionDialog()
{
	GTAElement* element = nullptr;
	GTAAppController::getGTAAppController()->getElementFromDocument(_relativePath, element, false);	
	if (element == nullptr)
		element = getElement();

	_pSummaryAndConclusionDlg->setWindowTitle(QString("Purpose and Conclusion - %2").arg(_elementName));
	_pSummaryAndConclusionDlg->setPurpose(element->getPurposeForProcedure());
	_pSummaryAndConclusionDlg->setConclusion(element->getConclusionForProcedure());
	_pSummaryAndConclusionDlg->show();
}

/**
 * @brief Set the summary and conclusion to document element
 * @param purpose - value of purpose from PurposeAndConclusion Dialog
 * @param conclusion - value of conclusion from SummarAndConclusion Dialog
*/
void GTAEditorWindow::setPurposeAndConclusionToElement(const QString& purpose, const QString& conclusion)
{
	GTAElement* element =  getElement();
	element->setPurposeForProcedure(purpose);
	element->setConclusionForProcedure(conclusion);
}

void GTAEditorWindow::highlightHeaderTableRow(int row)
{
	if (_pHeaderWidget != nullptr)
	{
		_pHeaderWidget->highlightHeaderTableRow(row);
	}
}

void GTAEditorWindow::onHeaderFieldAdded(const QString& iName, const QString& iVal, const QString& iMandatory, const QString& iDescription)
{
	if (_pEditorController)
	{
		bool status = _pEditorController->addheaderField(iName, iVal, iMandatory, iDescription);
		emit headerFieldUpdatedInModel(status, _pEditorController->getLastError());
	}
}

bool GTAEditorWindow::isHeaderRowEditable(const int& iRow)
{
	bool val = false;
	if (_pEditorController)
	{
		val = _pEditorController->isHeaderRowEditable(iRow);
	}
	return val;
}

bool GTAEditorWindow::getHeaderFieldInfo(const int& iRow, QString& iFieldName, QString& iFieldVal, QString& iMandatoryInfo, QString& iFieldDescription, bool& iShowInLTRA)
{
	bool rc = false;
	if (_pEditorController)
	{
		rc = _pEditorController->getHeaderFieldInfo(iRow, iFieldName, iFieldVal, iMandatoryInfo, iFieldDescription, iShowInLTRA);
	}
	return rc;
}

/**
 * @brief Checks If the selected header Field is mandatory field
 * @param iRow: Index for the selected field in header dialog
 * @param isMandatory: Gets the value of the Mandatory field related to the field
 * @return bool
*/
bool GTAEditorWindow::isHeaderFieldMandatory(const int& iRow, bool& isMandatory)
{
	bool rc = false;
	if (_pEditorController)
	{
		rc = _pEditorController->isHeaderFieldMandatory(iRow, isMandatory);
	}
	return rc;
}

bool GTAEditorWindow::isHeaderFieldDefaultMandatory(const int& iRow)
{
	bool isHeaderFieldDefaultMandatory = false;
	if (_pEditorController)
	{
		isHeaderFieldDefaultMandatory = _pEditorController->isDefaultMandatoryHeaderField(iRow);
	}

	return isHeaderFieldDefaultMandatory;
}

QString GTAEditorWindow::getLastError()
{
	if (_pEditorController)
	{
		return _pEditorController->getLastError();
	}
	return QString();
}

bool GTAEditorWindow::editHeaderField(const int& iRow, const QString& iName, const QString& iVal, const QString& iMandatory, const QString& iDescription, const bool& iShowLTRA)
{
	bool rc = false;
	if (_pEditorController)
	{
		bool bMandatory = true;
		if (iMandatory.contains("No", Qt::CaseInsensitive))
			bMandatory = false;
		rc = _pEditorController->editHeaderField(iRow, iName, iVal, bMandatory, iDescription, iShowLTRA);

	}

	return rc;

}


bool GTAEditorWindow::isHeaderRowDeletable(const int& iRow)
{
	bool rc = false;
	if (_pEditorController)
	{
		rc = _pEditorController->isHeaderRowDeletable(iRow);
	}
	return rc;
}

bool GTAEditorWindow::removeHeaderItem(const int& iRow)
{
	bool rc = false;
	if (_pEditorController)
	{
		rc = _pEditorController->removeHeaderItem(iRow);

	}
	return rc;
}

bool GTAEditorWindow::moveHeaderRowItemUp(const int& iRow)
{
	bool rc = false;
	if (_pEditorController)
	{
		rc = _pEditorController->moveHeaderRowItemUp(iRow);
	}
	return rc;
}

bool GTAEditorWindow::moveHeaderRowItemDown(const int& iRow)
{

	bool rc = false;
	if (_pEditorController)
	{
		rc = _pEditorController->moveHeaderRowItemDown(iRow);
	}
	return rc;

}

bool GTAEditorWindow::headerExists(const QString& iName)const
{
	bool rc = false;
	if (_pMDIController)
	{
		rc = _pMDIController->headerExists(iName);
	}
	return rc;
}

QString GTAEditorWindow::getHeaderName()const
{
	if (_pEditorController)
		return _pEditorController->getHeaderName();
	else
		return QString();
}
bool GTAEditorWindow::headerComplete(QStringList& oEmptyManField)
{
	bool rC = false;
	if (_pEditorController)
		rC = _pEditorController->headerComplete(oEmptyManField);
	return rC;
}

bool GTAEditorWindow::createHeaderModelFromDoc(const QString& iFileName, QAbstractItemModel*& opEditorModel)
{
	bool rc = false;
	if (_pEditorController)
	{
		rc = _pEditorController->createHeaderModelFromDoc(iFileName, opEditorModel);
	}
	return rc;
}

bool GTAEditorWindow::saveHeader(const QString& iName)const
{
	bool rc = false;
	if (_pEditorController)
	{
		rc = _pEditorController->saveHeader(iName);
	}
	return rc;
}

bool GTAEditorWindow::deleteHeader(const QString& iName)const
{
	bool rc = false;
	if (_pEditorController)
	{
		rc = _pEditorController->deleteHeader(iName);
	}
	return rc;
}

bool GTAEditorWindow::setInitialHeaderView(QTableView* ipView)
{
	bool rc = false;
	if (_pEditorController)
	{
		rc = _pEditorController->setInitialHeaderView(ipView);
	}
	return rc;
}

QString GTAEditorWindow::getRelativePath()const
{
	return _relativePath;
}

QString GTAEditorWindow::getElementName()const
{
	return _elementName;
}

bool GTAEditorWindow::isDataEmpty()
{
	bool rc = false;
	if (_pEditorController)
	{
		rc = _pEditorController->isDataEmpty();
	}
	return rc;
}

GTAElement* GTAEditorWindow::getElement()const
{
	if (_pEditorController)
	{
		return _pEditorController->getElement();
	}
	else
		return nullptr;
}

/**
 * \brief Triggers the export of the SCXML of the select document
 */
void GTAEditorWindow::exportSXCML()
{
	if (_pEditorController)
	{
		GTAUtil* _pathExportScxml = new GTAUtil();
		QString FileName = _relativePath;
		QString fileName = _elementName;
		GTAElement* pElement = _pEditorController->getElement();

		if (FileName.isEmpty() || fileName.isEmpty())
		{
			QMessageBox::critical(this, "Error", "Provide element name in editor, it can't be empty");
			return;
		}
		else if (isDataEmpty())
		{
			QMessageBox::warning(this, "Empty data", "Editor is empty nothing to export");
			return;
		}

		fileName = fileName.mid(0, fileName.indexOf("."));
		QString saveFileName = fileName;
		fileName += ".scxml";
		QString filePath;
		QString tempFilePath;
		QString fullFilePath;

		QString DefaultPath = QDir::cleanPath(QDir::currentPath() + "/" + fileName);
		if (GTAUtil::getGlobalPathExportScxml().isEmpty())
		{
			fullFilePath = QFileDialog::getSaveFileName(this, tr("Save File"), DefaultPath, "*.scxml");
			tempFilePath = fullFilePath;
			filePath = tempFilePath.remove("/" + fileName);
			GTAUtil::setGlobalPathExportScxml(filePath);
		}
		else
		{
			fullFilePath = QFileDialog::getSaveFileName(this, tr("Save File"), GTAUtil::getGlobalPathExportScxml() + "/" + fileName, "*.scxml");
		}

		if (!fullFilePath.isEmpty() && _pMDIController != nullptr)
		{

			//bool hasUnsubscribeAtStart = TestConfig::getInstance()->getInitSubscribeStart();
			//bool hasUnsubscribeAtEnd = TestConfig::getInstance()->getInitSubscribeEnd();	
			bool hasUnsubscribeAtStart = getUnsubscribeAtStartResults();
			bool hasUnsubscribeAtEnd = getUnsubscribeAtEndResults();
			_pMDIController->setUnsubscribeFileParamOnly(getUnsubscribeFileParamOnly());

			QFutureWatcher<bool>* FutureWatcher = new QFutureWatcher<bool>();
			connect(FutureWatcher, SIGNAL(finished()), this, SLOT(exportScxmlSlot()));
			showProgressBar("Exporting SCXML");
			QFuture<bool> Future = QtConcurrent::run(_pMDIController, &GTAMdiController::exportToScxml, pElement, saveFileName, fullFilePath, hasUnsubscribeAtStart, hasUnsubscribeAtEnd);
			FutureWatcher->setFuture(Future);
		}
	}
}

/**
 * \brief Slot called when the export is complete. Turns off progress bar, display info.
 */
void GTAEditorWindow::exportScxmlSlot()
{
	QFutureWatcher<bool>* pWatcher = dynamic_cast<QFutureWatcher<bool> *>(sender());
	if (pWatcher == nullptr)
		return;
	else
	{
		hideProgressBar();
		bool result = pWatcher->result();
		if (!result && _pMDIController != nullptr)
		{
			QMessageBox::critical(this, "Export SCXML", _pMDIController->getLastError());
		}
		else
		{
			QMessageBox::information(this, "Export SCXML", "Export to SCXML completed");
		}
		pWatcher->deleteLater();

	}
}

bool GTAEditorWindow::showICDDetails(bool iStatus, QString& oErrMsg)
{

	bool rc = false;
	if (_pMDIController != nullptr && _pEditorController != nullptr)
	{
		GTAElement* pElement = getElement();
		if (pElement)
		{
			if (pElement->getSaveStatus() != GTAElement::SaveOK)
			{
				oErrMsg = "Not applicable for unsaved document";
				rc = false;
			}
			else
			{
				QList <GTAFILEInfo> lstFileInfo;
				rc = _pMDIController->getFileInfoListUsed(_relativePath, lstFileInfo);
				if (rc)
				{
					rc = _pEditorController->showICDDetails(iStatus, lstFileInfo);
					if (!rc)
					{
						oErrMsg = _pEditorController->getLastError();
					}
				}
				else
				{
					oErrMsg = _pMDIController->getLastError();
				}
			}
		}
	}

	return rc;
}

void GTAEditorWindow::CallActionDropped(int& iRow, QString& fileName, QString& fileRefUUID)
{
	if (_pEditorController != nullptr && _pMDIController != nullptr)
	{
		GTACommandBase* pCmd = _pEditorController->getCommand(iRow);
		if (pCmd == nullptr)
		{

			QHash<QString, QStringList> actionCmdList;
			QStringList orderLst;
			GTAElement* pElement = _pEditorController->getElement();//pElement->getUuid();
			if (pElement != nullptr)
			{

				GTAElement::ElemType elemtType = pElement->getElementType();
				if (elemtType == GTAElement::OBJECT)
					actionCmdList = _pMDIController->getActionCommandList(GTAUtil::OBJECT, orderLst);
				else if (elemtType == GTAElement::FUNCTION)
					actionCmdList = _pMDIController->getActionCommandList(GTAUtil::FUNCTION, orderLst);
				else if (elemtType == GTAElement::PROCEDURE)
					actionCmdList = _pMDIController->getActionCommandList(GTAUtil::PROCEDURE, orderLst);
				else if (elemtType == GTAElement::SEQUENCE)
					actionCmdList = _pMDIController->getActionCommandList(GTAUtil::SEQUENCE, orderLst);
				else if (elemtType == GTAElement::TEMPLATE)
					actionCmdList = _pMDIController->getActionCommandList(GTAUtil::TEMPLATE, orderLst);
			}
			_pActionWindow->setCommandList(actionCmdList);
			_pActionWindow->setActionCommand(nullptr);

			ui->EditorTV->selectRow(iRow);
			_pActionWindow->setCallCommandonDropEvent(fileName, fileRefUUID);

		}
	}
}

void GTAEditorWindow::checkCompatibility()
{
	//bool rc = false;
	if (_pMDIController != nullptr)
	{

		bool bEditorEmptyStatus = isDataEmpty();
		if (bEditorEmptyStatus == true)
		{
			QMessageBox::warning(this, "Warning", "The file is Empty, nothing to Check!");

		}
		else
		{
			showProgressBar("Checking Compatibility. Please Wait...");
			QFutureWatcher<ErrorMessageList>* FutureWatcher = new QFutureWatcher<ErrorMessageList>();
			connect(FutureWatcher, SIGNAL(finished()), this, SLOT(displayCompatibilityReport()));
			QFuture<ErrorMessageList> Future = QtConcurrent::run(_pMDIController, &GTAMdiController::CheckCompatibility);
			FutureWatcher->setFuture(Future);
		}

	}
}


void GTAEditorWindow::displayCompatibilityReport()
{

	QFutureWatcher<ErrorMessageList>* pWatcher = dynamic_cast<QFutureWatcher<ErrorMessageList> *>(sender());
	if (pWatcher == nullptr)
		return;
	if (_pEditorController)
	{
		hideProgressBar();
		ErrorMessageList errorList = pWatcher->result();

		if (!errorList.isEmpty())
		{
			_pEditorController->setDisplaycontextToCompatibilityCheck();
			emit updateErrorLog(errorList);
		}
		else
		{
			_pEditorController->resetDisplayContext();

			GTAElement* pElement = _pEditorController->getElement();
			if (pElement != nullptr)
			{

				GTAElement::ElemType elemtType = pElement->getElementType();

				if (elemtType == GTAElement::PROCEDURE)
					QMessageBox::information(this, "Check Compatibility Result", "This procedure is compatible with actual standard!");
				else if (elemtType == GTAElement::SEQUENCE || elemtType == GTAElement::OBJECT)
					QMessageBox::information(this, "Check Compatibility Result", "This sequence is compatible with actual standard!");
				else
				{
					QMessageBox::information(this, "Check Compatibility Result", "Compatible with actual standard!");
				}
			}
		}
	}
	pWatcher->deleteLater();

}

//converts the display context from AppController to GTAElementViewModel
GTAElementViewModel::DisplayContext GTAEditorWindow::getConvertedDisplayContext(GTAAppController::DisplayContext iDisplayContext)
{
	GTAElementViewModel::DisplayContext ret;
	switch (iDisplayContext)
	{
	case GTAAppController::EDITING:
		ret = GTAElementViewModel::EDITING;
		break;

	case GTAAppController::COMPATIBILITY_CHK:
		ret = GTAElementViewModel::COMPATIBILITY_CHK;
		break;

	case GTAAppController::SEARCH_REPLACE:
		ret = GTAElementViewModel::SEARCH_REPLACE;
		break;
	}
	return ret;
}

void GTAEditorWindow::highlightRow(const int& iRowId, GTAAppController::DisplayContext iDisplayContext)
{
	if (_pEditorController)
	{
		QModelIndex oHighlightedRow;

		GTAElementViewModel::DisplayContext displayContext = getConvertedDisplayContext(iDisplayContext);
		_pEditorController->highlightRow(iRowId, oHighlightedRow, displayContext);//GTAElementViewModel::COMPATIBILITY_CHK);
		ui->EditorTV->clearSelection();

		if (oHighlightedRow.isValid())
			ui->EditorTV->scrollTo(oHighlightedRow);

	}

}

void GTAEditorWindow::onEscapeKeyTriggered()
{
	if (_pEditorController->getDisplayContext() != GTAElementViewModel::DEBUG)
	{
		_pEditorController->resetDisplayContext();

		_pEditorController->updateEditorView();
	}
}

void GTAEditorWindow::createCallSequence(const QModelIndexList& iSelectedItems, const QString& iType)
{

	if (_pMDIController != nullptr && _pEditorController != nullptr)
	{
		QList<int> rowLst;
		int row = 0;
		QHash<int, GTAActionCall*> hshCallItems;

		QStringList orderLst;
		EditorType editorType;
		QHash<QString, QStringList> actionCmdList;


		QStringList invalidFiles;
		foreach(QModelIndex index, iSelectedItems)
		{
			GTAActionCall* pCall = new GTAActionCall;

			if (index.column() != 0)
				continue;

			QString fileName = _pMDIController->getFilePathFromTreeModel(index);
			if (fileName.isEmpty())
				continue;


			pCall->setElement(fileName);
			pCall->setAction(ACT_CALL);

			QString complement = COM_CALL_FUNC;

			QStringList allowedComplements;
			if (iType == ".seq")
			{
				actionCmdList = _pMDIController->getActionCommandList(GTAUtil::SEQUENCE, orderLst);
				editorType = EditorType::Sequence;
				allowedComplements << COM_CALL_FUNC << COM_CALL_OBJ << COM_CALL_PROC;
			}
			else if (iType == ".pro")
			{
				actionCmdList = _pMDIController->getActionCommandList(GTAUtil::PROCEDURE, orderLst);
				editorType = EditorType::Procedure;
				allowedComplements << COM_CALL_FUNC << COM_CALL_OBJ;
			}

			_pActionWindow->setCommandList(actionCmdList, orderLst);

			if (fileName.endsWith(QString(".fun")))
			{
				complement = COM_CALL_FUNC;
			}
			else if (fileName.endsWith(QString(".obj")))
			{
				complement = COM_CALL_OBJ;
			}
			else if (fileName.endsWith(QString(".pro")))
			{
				complement = COM_CALL_PROC;
			}
			else
			{
				invalidFiles.append(fileName);
				continue;
			}

			if (!allowedComplements.contains(complement))
			{
				invalidFiles.append(fileName);
				continue;
			}

			pCall->setComplement(complement);

			hshCallItems.insert(row, pCall);

			GTACommandVariant variant;
			variant.setCommand(pCall);
			QVariant actionVariant;
			actionVariant.setValue(variant);


			editAction(row, actionVariant);
			GTAElement* pElement = nullptr;
			_pMDIController->getElementFromDocument(fileName, pElement);
			if (nullptr != pElement)
			{
				QList<TagVariablesDesc> taglst = pElement->getTagList();
				if (!taglst.isEmpty())
				{
					rowLst.append(row);
				}
				delete pElement;
			}

			row++;

		}


		for (int i = 0; i < row; i++)
		{

			GTAActionCall* pCall = hshCallItems.value(i);
			ui->EditorTV->selectRow(i);
			_pActionWindow->setActionCommand(pCall);

			if (rowLst.contains(i))
			{
				connect(_pActionWindow, SIGNAL(okPBClicked()), _pActionWindow, SLOT(hide()));
				int status = _pActionWindow->exec();
				if (status == 0)
				{
					QString errorMsg;
					QList<int> clearedRowId;
					_pEditorController->clearRow(i, errorMsg, clearedRowId);
				}
				disconnect(_pActionWindow, SIGNAL(okPBClicked()), _pActionWindow, SLOT(hide()));
			}

		}

		if (!invalidFiles.isEmpty())
		{
			ErrorMessageList msgList;
			ErrorMessage logMessage;
			logMessage.description = QString("Following files are invalid for this call sequence:-");
			logMessage.errorType = ErrorMessage::ErrorType::kError;
			logMessage.source = ErrorMessage::MsgSource::kNoSrc;

			msgList.append(logMessage);

			foreach(QString file, invalidFiles)
			{
				ErrorMessage logMessage;
				logMessage.description = file;
				logMessage.errorType = ErrorMessage::ErrorType::kError;
				logMessage.source = ErrorMessage::MsgSource::kNoSrc;
				msgList.append(logMessage);
			}


			emit updateErrorLog(msgList);
		}


	}
}

void GTAEditorWindow::debug()
{

	if (_pEditorController != nullptr)
	{
		// to avoid memory leaks
		if (nullptr != _pDebuggerThread)
		{
			_pDebuggerThread->exit(0);
			while (_pDebuggerThread->isRunning());
			delete _pDebuggerThread;
			_pDebuggerThread = nullptr;
		}

		if (nullptr != _pDebugger)
		{
			delete _pDebugger;
			_pDebugger = nullptr;
		}

		// this will start the debugger in a separate thread
		_pDebuggerThread = new QThread(this);

		TestConfig* testConf = TestConfig::getInstance();
		QString debugIP = QString::fromStdString(testConf->getDebugModeIP());
		int debugport = testConf->getDebugModePort();

		_pDebugger = new GTADebugger(debugIP, debugport);


		connect(_pDebuggerThread, SIGNAL(started()), _pDebugger, SLOT(startDebugger()), Qt::QueuedConnection);

		//this slot helps identify if the server could be started
		connect(_pDebugger, SIGNAL(serverStartStatus(bool)), this, SIGNAL(launchDebugMode(bool)), Qt::DirectConnection);
		connect(this, SIGNAL(launchDebugMode(bool)), this, SLOT(onLaunchDebugMode(bool)));

		connect(_pDebugger, SIGNAL(doesLineHaveBreakpoint(int, bool&)), this, SLOT(onCheckLineHasBreakpoint(int, bool&)), Qt::DirectConnection);
		connect(_pDebugger, SIGNAL(debuggerHitBreakpoint(int)), this, SLOT(onDebuggerHitBreakpoint(int)), Qt::DirectConnection);

		connect(this, SIGNAL(userActionRegistered(GTADebugger::UserAction)), _pDebugger,
			SLOT(onUpdateUserAction(GTADebugger::UserAction)), Qt::DirectConnection);

		connect(_pDebugger, SIGNAL(getUserInputStatus()), this, SLOT(userInputUpdated()), Qt::DirectConnection);

		connect(_pDebugger, SIGNAL(debuggingCompleted()), this, SLOT(onDebuggingCompleted()), Qt::QueuedConnection);

		connect(_pDebugger, SIGNAL(updateOutputWindow(QStringList)), this, SLOT(onUpdateOutputWindow(QStringList)), Qt::DirectConnection);

		_pDebugger->moveToThread(_pDebuggerThread);
		_pDebuggerThread->start(QThread::TimeCriticalPriority);

		_pEditorController->removeEmptyRows();

		//launch process will continue in the main thread
		//launch(GTAEditorWindow::DEBUG_MODE);
	}
}

/**
 * This slot manages the setting of the DEBUG/EDITING mode of the Editor Window
 * @iVal: true/false mentions server-started/server-notStarted
*/
void GTAEditorWindow::onLaunchDebugMode(bool iVal)
{
	if (iVal)
	{
		if (_pEditorController)
			_pEditorController->setDisplayContext(GTAElementViewModel::DEBUG);
		emit toggleEditorActionsinMain(false);
		onReadOnlyDocToggled(true);
		_action = GTADebugger::kNone;

		//launch(GTAEditorWindow::DEBUG_MODE);
		onToggleEditorConnections(false);
	}
	else
	{
		if (_pEditorController)
			_pEditorController->setDisplayContext(GTAElementViewModel::EDITING);
		emit toggleEditorActionsinMain(true);
		onReadOnlyDocToggled(false);
		emit updateOutputWindow(QStringList() << "Unable to start debugging. Check and close any instance of Scheduler already running");

		onToggleEditorConnections(true);
	}
	//disconnecting the slot so that the thread doesn't stay linked until next call
	disconnect(_pDebugger, SIGNAL(serverStartStatus(bool)), this, SIGNAL(launchDebugMode(bool)));
	disconnect(this, SIGNAL(launchDebugMode(bool)), this, SLOT(onLaunchDebugMode(bool)));
}

/**
 * This function manages the editor TV and element table view signals/slots
 * @iVal: true/false mentions connect/disconnect
*/
void GTAEditorWindow::onToggleEditorConnections(bool iVal)
{
	if (iVal)
	{
		connect(ui->EditorTV, SIGNAL(clicked(QModelIndex)), this, SLOT(setMoveUpDownButtonEnabled(QModelIndex)));
		GTAElementTableView* pTV = dynamic_cast<GTAElementTableView*>(ui->EditorTV);
		if (nullptr != pTV)
		{
			connect(pTV, SIGNAL(selectionChangedForEdior()), this, SLOT(setMoveUpDownButtonEnabled()));
			connect(pTV, &GTAElementTableView::selectionChangedForEdior, this, &GTAEditorWindow::onSetFocus);
		}
	}
	else
	{
		disconnect(ui->EditorTV, SIGNAL(clicked(QModelIndex)), this, SLOT(setMoveUpDownButtonEnabled(QModelIndex)));
		GTAElementTableView* pTV = dynamic_cast<GTAElementTableView*>(ui->EditorTV);
		if (nullptr != pTV)
		{
			disconnect(pTV, SIGNAL(selectionChangedForEdior()), this, SLOT(setMoveUpDownButtonEnabled()));
			disconnect(pTV, &GTAElementTableView::selectionChangedForEdior, this, &GTAEditorWindow::onSetFocus);
		}
	}
}

bool GTAEditorWindow::userInputUpdated()
{
	//input hasn't changed
	bool rc = false;
	if (_action == GTADebugger::kNone)
	{
		rc = true;
	}
	return rc;
}

void GTAEditorWindow::onCheckLineHasBreakpoint(int iLineNumber, bool& oHasBreakpoint)
{
	GTAElementViewModel* pModel = dynamic_cast<GTAElementViewModel*>(ui->EditorTV->model());
	QModelIndex idx = pModel->index(iLineNumber, GTAElementViewModel::ACTION_INDEX);
	if (idx.isValid() && _pEditorController != nullptr)
	{

		GTACommandBase* pCmd = getCommandForSelectedRow(idx.row());
		if (pCmd)
		{
			_CurrentExecutionLine = iLineNumber;
			//tells me if the current command at the present line number has a breakpoint or the last executed command was next/previous
			if (pCmd->hasBreakpoint() || (_action == GTADebugger::NEXT || _action == GTADebugger::PREVIOUS))
			{
				oHasBreakpoint = true;
			}
			else
			{
				oHasBreakpoint = false;

				QString msg = QString("[%1] : Entering Command State").arg(QString::number((_CurrentExecutionLine)));
				emit updateOutputWindow(QStringList() << msg);
			}
		}
	}

}

void GTAEditorWindow::onDebuggerHitBreakpoint(int iLineNumber)
{
	GTAElementViewModel* pModel = dynamic_cast<GTAElementViewModel*>(ui->EditorTV->model());
	QModelIndex idx = pModel->index(iLineNumber, GTAElementViewModel::ACTION_INDEX);
	if (idx.isValid() && _pEditorController != nullptr)
	{
		_pExecutingCmd = getCommandForSelectedRow(idx.row());
		if (_pExecutingCmd && _action != GTADebugger::STOP) //added to ensure _action is not changed when a stop is pressed)
		{
			_pExecutingCmd->setExecutionControl(true);
			_CurrentExecutionLine = iLineNumber;
			_action = GTADebugger::kNone;
			_enableNextPreviousContinue = true;
		}
	}
}

void GTAEditorWindow::setExecutionMode(ExecutionMode mode)
{
	_executionMode = mode;
}

void GTAEditorWindow::launch(ExecutionMode mode, QMessageBox::ButtonRole role)
{
	setExecutionMode(mode);

	if (_pMDIController != nullptr && _pEditorController != nullptr)
	{
		GTAElement* pElement = _pEditorController->getElement();
		if (pElement)
		{
			GTAElement::ElemType type = pElement->getElementType();


			if (type == GTAElement::PROCEDURE || type == GTAElement::SEQUENCE)
			{

				bool bEditorEmptyStatus = _pEditorController->isDataEmpty();
				QString errorMsg;
				if (bEditorEmptyStatus == true)
				{
					QMessageBox::critical(this, "Error", "Unable to launch scxml scenario, since the editor is empty !!!");
				}
				else
				{				
					if (role == QMessageBox::YesRole)
					{
						showProgressBar("Checking Compatibility. Please Wait...");
						//showProgressBar("Checking Compatibility. Please Wait...");

						QFutureWatcher<ErrorMessageList>* FutureWatcher = new QFutureWatcher<ErrorMessageList>();
						connect(FutureWatcher, SIGNAL(finished()), this, SLOT(onCompatibilityCheckForLaunchFinished()));
						QFuture<ErrorMessageList> Future = QtConcurrent::run(_pMDIController, &GTAMdiController::CheckCompatibility);
						FutureWatcher->setFuture(Future);
					}

					else if (role == QMessageBox::NoRole)
					{

						QString  fileName = _elementName;
						fileName = fileName.mid(0, fileName.indexOf("."));
						QString sElementName = fileName;
						QStringList varList;

						if (((type == GTAElement::SEQUENCE || (type == GTAElement::PROCEDURE)) && (_executionMode != SEQUENCER_MODE)))
						{
							showProgressBar("Launch SCXML in progress...");

							LaunchParameters parameters;
							parameters.elementName = sElementName;
							parameters.errorMsg = errorMsg;
							parameters.hasUnsubscribeStart = getUnsubscribeAtStartResults();
							parameters.hasUnsubscribeEnd = getUnsubscribeAtEndResults();
							parameters.hasUnsubscribeFileParamOnly = getUnsubscribeFileParamOnly();
							parameters.varList = varList;
							parameters.mode = _executionMode;
							parameters.uuid = getUUID();
							//                                emit toggleLaunchButton(false);
							QFutureWatcher<bool>* FutureWatcher = new QFutureWatcher<bool>();
							connect(FutureWatcher, SIGNAL(finished()), this, SLOT(onLauchSCXMLCompleted()));
							QFuture<bool> Future = QtConcurrent::run(_pMDIController, &GTAMdiController::launch, parameters);
							FutureWatcher->setFuture(Future);
						}
						else
							{
								return;
							}

					}
					else
					{
						QMessageBox::information(this, "Info", "Launch Aborted!");
						if (_pEditorController)
							_pEditorController->setDisplayContext(GTAElementViewModel::EDITING);
						emit toggleEditorActionsinMain(true);
						onReadOnlyDocToggled(false);
						onToggleEditorConnections(true);
						}
					}
				}
			}
			else
			{
				QMessageBox::information(this, "Info", "An object or a function cannot be launched");
			}
	}
}

//to be changed
void GTAEditorWindow::onCompatibilityCheckForLaunchFinished()
{

	bool bLaunch = false;
	GTAElement::ElemType elemtType;
	QFutureWatcher<ErrorMessageList>* pWatcher = dynamic_cast<QFutureWatcher<ErrorMessageList> *>(sender());
	if (pWatcher == nullptr)
		return;
	if (_pEditorController != nullptr && _pMDIController != nullptr)
	{
		hideProgressBar();

		ErrorMessageList errorList = pWatcher->result();


		GTAElement* pElement = _pEditorController->getElement();
		QString message;
		if (pElement != nullptr)
		{

			elemtType = pElement->getElementType();


			if (elemtType == GTAElement::PROCEDURE)
				message = QString("Check Compatibility Result. \n This procedure is compatible with actual standard!");
			else if (elemtType == GTAElement::SEQUENCE || elemtType == GTAElement::OBJECT)
				message = QString("Check Compatibility Result. \n This sequence is compatible with actual standard!");
			else
			{

			}
		}

		if (!errorList.isEmpty())
		{
			_pEditorController->setDisplaycontextToCompatibilityCheck();
			emit updateErrorLog(errorList);
			QMessageBox msgBox(this);
			QString text = QString("Errors found in compatiblity report, continue with launch ?");
			msgBox.setText(text);
			msgBox.setIcon(QMessageBox::Critical);
			msgBox.setWindowTitle("Compatiliby report warning");
			msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			msgBox.setDefaultButton(QMessageBox::No);
			int ret = msgBox.exec();
			if (ret == QMessageBox::Yes)
			{
				bLaunch = true;
			}
		}
		else
		{
			_pEditorController->resetDisplayContext();

			QMessageBox msgBox(this);
			QString text = QString(message.append("\n Continue with launch?"));
			msgBox.setText(text);
			msgBox.setWindowModality(Qt::ApplicationModal);
			msgBox.setIcon(QMessageBox::Question);
			msgBox.setWindowTitle("Compatiliby report warning");
			msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			msgBox.setDefaultButton(QMessageBox::No);
			int ret = msgBox.exec();
			if (ret == QMessageBox::Yes)
			{
				bLaunch = true;
			}

		}

		pWatcher->deleteLater();

		if (bLaunch)

		{

			QList<GTAActionCall*> callProcLst;
			QString sElementName = _elementName;
			sElementName = sElementName.mid(0, sElementName.indexOf(".")).trimmed();
			QString errorMsg;
			QStringList varList;


			if (((elemtType == GTAElement::SEQUENCE || (elemtType == GTAElement::PROCEDURE)) && (_executionMode != SEQUENCER_MODE)))
			{

				if (_executionMode == DEBUG_MODE)
					_pEditorController->setDisplayContext(GTAElementViewModel::DEBUG);

				showProgressBar("Launch SCXML in progress...");
				QFutureWatcher<bool>* FutureWatcher = new QFutureWatcher<bool>();
				connect(FutureWatcher, SIGNAL(finished()), this, SLOT(onLauchSCXMLCompleted()));

				LaunchParameters parameters;
				parameters.elementName = sElementName;
				parameters.errorMsg = errorMsg;
				parameters.hasUnsubscribeStart = getUnsubscribeAtStartResults();
				parameters.hasUnsubscribeEnd = getUnsubscribeAtEndResults();
				parameters.hasUnsubscribeFileParamOnly = getUnsubscribeFileParamOnly();
				parameters.varList = varList;
				parameters.mode = _executionMode;
				parameters.uuid = getUUID();

				QFuture<bool> Future = QtConcurrent::run(_pMDIController, &GTAMdiController::launch, parameters);
				FutureWatcher->setFuture(Future);
			}
			else if ((elemtType == GTAElement::SEQUENCE) && (_executionMode == SEQUENCER_MODE))
			{
				bool isMultipleLogs = false;
				bool execute = true;
				QPushButton* buttonSingle = new QPushButton();
				buttonSingle->setText("Single Log");

				QPushButton* buttonMultiple = new QPushButton();
				buttonMultiple->setText("Multiple Logs");

				QPushButton* buttonCancel = new QPushButton;
				buttonCancel->setText("Cancel");

				QMessageBox msg;
				msg.setIcon(QMessageBox::Information);
				msg.setWindowTitle("Sequence management");
				msg.setText("Please select the type of logging required");
				msg.addButton(buttonSingle, QMessageBox::AcceptRole);
				msg.addButton(buttonMultiple, QMessageBox::AcceptRole);
				msg.addButton(buttonCancel, QMessageBox::RejectRole);
				msg.exec();

				QAbstractButton* clickedButton = msg.clickedButton();
				if (clickedButton)
				{
					if (clickedButton == buttonCancel)
					{
						return;
					}
					else if (clickedButton == buttonMultiple)
					{

						isMultipleLogs = isSequenceValidForMultipleLogs(callProcLst);
						if (isMultipleLogs == false)
						{
							QMessageBox::StandardButton button = QMessageBox::information(this, "Mutiple logs for Sequence",
								"The sequence contains commands other than call, hence the logging considered will be single.\nDo you want to continue?",
								QMessageBox::Yes, QMessageBox::No);
							if (button != QMessageBox::Yes)
							{
								execute = false;
							}
						}

					}
					if (execute)
					{
						emit toggleLaunchButton(false);

						QStringList failedList;
						//                        QHash<QString,QString> generatedScxmlLst;
						QStringList scxmlFilePathList;
						QStringList logNameList;
						QStringList procList;

						_pMDIController->generateScxml(isMultipleLogs, getElement(), _relativePath, callProcLst,
							scxmlFilePathList, logNameList, failedList, procList);
						procList.append(_elementName);
						_pMDIController->executeSequence(isMultipleLogs, scxmlFilePathList, logNameList, failedList, procList);
					}
				}
				else
				{
					return;
				}
			}

		}
	}
}

bool GTAEditorWindow::isSequenceValidForMultipleLogs(QList<GTAActionCall*>& ocallProcLst)
{
	bool rc = true;
	GTAElement* pElement = getElement();
	if (pElement)
	{

		int childCnt = pElement->getDirectChildrenCount();
		for (int i = 0; i < childCnt; i++)

		{
			GTACommandBase* pCmd = nullptr;
			pElement->getCommand(i, pCmd);

			if (pCmd != nullptr && !pCmd->isIgnoredInSCXML())
			{
				if (!pCmd->hasReference())
				{
					rc = false;
					ocallProcLst.clear();
					break;
				}
				else
				{

					//                    if(!pCmd->isIgnoredInSCXML())
					//                    {
					GTAActionCall* pCall = dynamic_cast<GTAActionCall*>(pCmd);
					if (pCall)
					{
						QString elemName = pCall->getElement();
						QString ext = elemName.mid(elemName.lastIndexOf("."), (elemName.count() - 1));

						if (ext != ".pro")
						{
							rc = false;
							ocallProcLst.clear();
							break;
						}
						ocallProcLst.append(pCall);

					}
					//                    }
				}

			}
		}

	}
	return rc;
}

void GTAEditorWindow::onSequenceExecutionCompleted()
{
	emit toggleLaunchButton(true);
}

void GTAEditorWindow::onLauchSCXMLCompleted()
{
	QFutureWatcher<bool>* pWatcher = dynamic_cast<QFutureWatcher<bool> *>(sender());
	if (pWatcher == nullptr)
		return;

	bool status = pWatcher->result();
	if (!status)
		QMessageBox::critical(this, "Error", _pMDIController->getLastError());

	pWatcher->deleteLater();
	hideProgressBar();
}

bool GTAEditorWindow::searchHighlight(const QRegExp& searchString, QList<int>& iSelectedRows, bool ibSearchUp, bool singleSearch, bool ibTextSearch, QModelIndex& oHighlightedRowIdx, bool isHighlight, QHash<int, QString>& oFoundRows)
{
	bool rc = false;
	if (_pEditorController)
	{
		int lastSelectedRow = -1;
		if (singleSearch == true)
		{
			lastSelectedRow = _pEditorController->getLastHighligtedRow();
			if (ibSearchUp)
				lastSelectedRow = (lastSelectedRow == 0 || lastSelectedRow == -1) ? _pEditorController->getElement()->getAllChildrenCount() : lastSelectedRow--;
			else
				lastSelectedRow++;

			if (lastSelectedRow > -1 && iSelectedRows.isEmpty())
				iSelectedRows.append(lastSelectedRow);
		}

		rc = _pEditorController->searchHighlight(searchString, iSelectedRows, ibSearchUp, ibTextSearch, oHighlightedRowIdx, singleSearch, isHighlight, oFoundRows);
	}
	return rc;
}

void GTAEditorWindow::searchAndHighlightNext(QRegExp iSearchString, bool ibSearchUp, bool ibTextSearch)
{
	if (!iSearchString.isEmpty())
	{
		QList<int> selectedRows;
		getSelectedEditorRows(selectedRows);
		QModelIndex highlightedRowIdx;
		QHash<int, QString> oFoundRows;
		bool rc = searchHighlight(iSearchString, selectedRows, ibSearchUp, true, ibTextSearch, highlightedRowIdx, true, oFoundRows);
		if (!rc)
		{
			QMessageBox::information(this, "Not found", "Search string not found");
		}
		ui->EditorTV->clearSelection();
		//        QApplication::setActiveWindow(_pSearchReplaceDialog);

		if (highlightedRowIdx.isValid())
			ui->EditorTV->scrollTo(highlightedRowIdx);
	}
}

void GTAEditorWindow::searchAndHighlightAll(QRegExp iSearchString, bool iTextSearch, QHash<int, QString>& oFoundRows, bool isHighlight, bool isMultiSearch)
{

	if (_pEditorController != nullptr && !iSearchString.isEmpty())
	{
		_pEditorController->resetDisplayContext();
		QList<int> selectedRows;
		QModelIndex modelIdx;
		selectedRows.clear();
		bool rc = searchHighlight(iSearchString, selectedRows, false, false, iTextSearch, modelIdx, isHighlight, oFoundRows);
		if (!rc && isHighlight && !isMultiSearch)
		{
			QMessageBox::information(this, "Not found", "Search string not found");
		}
		ui->EditorTV->clearSelection();
		//        QApplication::setActiveWindow(_pSearchReplaceDialog);

	}
}

bool GTAEditorWindow::hightlightReplace(const int& iSelectedRow, const QRegExp& searchString, const QString& sReplace, bool ibSearchUp, QModelIndex& oHighlightedRowIdx)
{
	bool rc = false;
	if (_pEditorController)

	{

		QList<int> selectedRows;
		selectedRows.append(iSelectedRow);
		rc = _pEditorController->hightlightReplace(searchString, sReplace, selectedRows, ibSearchUp, oHighlightedRowIdx);
	}
	return rc;
}

void GTAEditorWindow::replaceAndHighightNext(QRegExp iSearchString, QString iReplaceString, bool ibSearchUp)
{
	if (!iSearchString.isEmpty())
	{
		int selectedRow = 0;
		QList<int> selectedRows;
		getSelectedEditorRows(selectedRows);
		if (!selectedRows.isEmpty())
			selectedRow = selectedRows.at(0);

		QModelIndex highlightedRowIdx;
		bool rc = hightlightReplace(selectedRow, iSearchString, iReplaceString, ibSearchUp, highlightedRowIdx);
		if (!rc)
		{
			QMessageBox::information(this, "Not found", "Search string not found");
		}
		ui->EditorTV->clearSelection();
		//        QApplication::setActiveWindow(_pSearchReplaceDialog);
		if (highlightedRowIdx.isValid())
			ui->EditorTV->scrollTo(highlightedRowIdx);
	}
}

bool GTAEditorWindow::replaceAllText(QRegExp iSearchString, QString iReplaceString)
{
	bool rc = false;
	if (_pEditorController != nullptr && !iSearchString.isEmpty())
	{
		rc = _pEditorController->replaceAllString(iSearchString, iReplaceString);

		//        if (!rc)
		//        {
		//            QMessageBox::information(this,"Not found","Search string not found");
		//        }

		ui->EditorTV->clearSelection();
		//        QApplication::setActiveWindow(_pSearchReplaceDialog);
	}
	return rc;
}

bool GTAEditorWindow::replaceAllEquipment(QRegExp iSearchString, QString iReplaceString)
{
	bool rc = false;
	if (_pEditorController != nullptr && !iSearchString.isEmpty())
	{
		rc = _pEditorController->replaceAllEquipment(iSearchString, iReplaceString);
		ui->EditorTV->clearSelection();
	}
	return rc;
}

void GTAEditorWindow::resetEditorDisplayMode()
{
	if (_pEditorController != nullptr)
	{
		_pEditorController->resetDisplayContext();
	}
}

QString GTAEditorWindow::getUUID()const
{
	QString uuid;
	if (_pEditorController != nullptr)
	{
		uuid = _pEditorController->getUUID();
	}
	return uuid;
}

void GTAEditorWindow::onUpdateOutputWindow(const QStringList iMsgList)
{
	QStringList lst;
	foreach(QString message, iMsgList)
	{
		QString msg = QString("[%1] : %2").arg(QString::number((_CurrentExecutionLine)), message);
		lst.append(msg);
	}

	emit updateOutputWindow(lst);
}


void GTAEditorWindow::onDebuggingCompleted()
{
	emit updateOutputWindow(QStringList() << "disconnecting signals");
	disconnect(_pDebugger, SIGNAL(serverStartStatus(bool)), this, SIGNAL(launchDebugMode(bool)));
	disconnect(this, SIGNAL(launchDebugMode(bool)), this, SLOT(onLaunchDebugMode(bool)));
	if (nullptr != _pDebuggerThread)
	{
		_pDebuggerThread->exit(0);
		QTime t;
		t.start();
		while (_pDebuggerThread->isRunning())
		{
			emit updateOutputWindow(QStringList() << QString("Stopping Server - Time (ms): %1").arg(QString::number(t.elapsed())));
		}
		delete _pDebuggerThread;
		_pDebuggerThread = nullptr;
	}
	if (nullptr != _pDebugger)
	{
		delete _pDebugger;
		_pDebugger = nullptr;
	}

	if (_pEditorController)
		_pEditorController->setDisplayContext(GTAElementViewModel::EDITING);

	emit updateOutputWindow(QStringList() << "Debug session ended!");
	emit toggleEditorActionsinMain(true);
	onToggleEditorConnections(true);
	onReadOnlyDocToggled(false);
}


//---------- Indentation implementation --------------------------
void GTAEditorWindow::hideColumns()
{
	QAbstractItemModel* pCurrentEditorModel = ui->EditorTV->model();
	if (pCurrentEditorModel)
	{

		int noOfCols = pCurrentEditorModel->columnCount();
		if (noOfCols > 0)
		{
			for (int i = (GTAElementViewModel::ACTION_INDEX + 1); i < noOfCols; i++)
			{
				ui->EditorTV->setColumnHidden(i, true);

			}
		}
	}
}
void GTAEditorWindow::showColumns()
{
	QAbstractItemModel* pCurrentEditorModel = ui->EditorTV->model();
	if (pCurrentEditorModel)
	{
		int noOfCols = pCurrentEditorModel->columnCount();
		if (noOfCols > 0)
		{
			for (int i = (GTAElementViewModel::ACTION_INDEX + 1); i < noOfCols; i++)
			{
				ui->EditorTV->setColumnHidden(i, false);
				ui->EditorTV->showColumn(i);

			}
			ui->EditorTV->horizontalHeader()->setSectionResizeMode(GTAElementViewModel::BREAKPOINT_INDEX, QHeaderView::Fixed);

			ui->EditorTV->setColumnWidth(GTAElementViewModel::ACTION_INDEX, 800);
		}
	}
}

//---------- Indentation implementation --------------------------


void GTAEditorWindow::setResetLiveMode(const QString iFileName, bool iVal)
{
	if (_pEditorController)
	{
		if (iVal)
		{
			_pEditorController->setDisplayContext(GTAElementViewModel::LIVE);
			setFileDetails("Live.pro", GTAAppController::getGTAAppController()->getSystemService()->getDataDirectory(), ".pro", GTAAppController::getUserName());
			createLiveEditorContextMenu(iFileName);
			resizeLiveEditorHeader();
		}
		else
		{
			_pEditorController->setDisplayContext(GTAElementViewModel::EDITING);
			createContextMenu(_windowTitle);
		}
	}
	clearEditorPage();
	getElement()->ClearAll();
	GTACommandBase* pCmd = nullptr;
	getElement()->insertCommand(pCmd, 0, false);
}

void GTAEditorWindow::clearSelectedRows()
{
	ui->EditorTV->selectionModel()->clearSelection();
}

int GTAEditorWindow::getRowToBeSelected() const
{
	return _rowToBeSelected;
}

TabIndices GTAEditorWindow::getFramePosition() const
{
	return _framePosition;
}

void GTAEditorWindow::setFramePosition(const TabIndices& position)
{
	_framePosition = position;
}

void GTAEditorWindow::setUnsubscribeAtStartResults(bool choice)
{
	_unsubscribeAtStartResults = choice;
}

void GTAEditorWindow::setUnsubscribeAtEndResults(bool choice)
{
	_unsubscribeAtEndResults = choice;
}

bool GTAEditorWindow::getUnsubscribeAtStartResults() const
{
	return _unsubscribeAtStartResults;
}

bool GTAEditorWindow::getUnsubscribeAtEndResults() const
{
	return _unsubscribeAtEndResults;
}

/**
 * @brief: This method sets the unsubscribeFileParamOnly value which will be used to create scxml
 * @param: unsubscribeFileParamOnly : if true then only File params will be unsubscribed
 */
void GTAEditorWindow::setUnsubscribeFileParamOnly(bool choice)
{
	_unsubscribeFileParamOnly = choice;
}

/**
 * @brief: This method gets the unsubscribeFileParamOnly value which will be used to create scxml
 * @param: unsubscribeFileParamOnly : if true then only File params will be unsubscribed
 */
bool GTAEditorWindow::getUnsubscribeFileParamOnly() const
{
	return _unsubscribeFileParamOnly;
}

/**
  * @brief: This method called on right click on selected row and displays the context menu in Live Editor.
  *         This method decides the enabling and disabling of action items in context menu.
  * @iPos: specifies the point where right click has been done
  * @pSourceWidget: specifies the sender object of the signal
  */
void GTAEditorWindow::displayLiveEditorContextMenu(const QPoint& iPos, QWidget* pSourceWidget)
{
	if (pSourceWidget != nullptr && pSourceWidget == ui->EditorTV && _pLiveEditorContextMenu != nullptr)
	{
		QList<int> lstSelectedRows;
		getSelectedEditorRows(lstSelectedRows);

		//Dev: both variables are created to enable disable rows on different combination of conditions
		bool readOnlyCmdStatus = false;
		bool editableCmdStatus = false;
		foreach(int row, lstSelectedRows)
		{
			GTACommandBase* pCmd = getCommandForSelectedRow(row);
			if (pCmd != nullptr)
			{
				if (pCmd->getReadOnlyState())
				{
					readOnlyCmdStatus |= true;
				}
				else
				{
					editableCmdStatus |= true;
				}
			}
		}

		if (!lstSelectedRows.isEmpty())
		{
			GTACommandBase* pCmd = getCommandForSelectedRow(lstSelectedRows[0]);
			if (_pLiveEditorContextMenu != nullptr)
			{
				QList<QAction*> lst = _pLiveEditorContextMenu->actions();
				for (int i = 0; i < lst.size(); i++)
				{
					bool statusReadOnly = readOnlyCmdStatus;

					QAction* pWdgt = lst.at(i);
					if (nullptr != pWdgt)
					{
						QString sName = pWdgt->text();

						if (sName == "Change/Edit Command\nF2")
						{
							if (nullptr == pCmd || true == statusReadOnly)
								pWdgt->setEnabled(false);
							else
								pWdgt->setEnabled(true);
						}
						else if (sName == "Add Row\nCtrl++")
						{
							//selected row is single row, the last row of Editor with readOnly status
							if (lstSelectedRows.count() == 1 && nullptr != pCmd && true == pCmd->getReadOnlyState()
								&& lstSelectedRows.at(0) == getElement()->getAllChildrenCount() - 1)
								pWdgt->setEnabled(true);
							else if (true == statusReadOnly)
								pWdgt->setEnabled(false);
							else
								pWdgt->setEnabled(true);
						}
						else if (sName == "Delete Row\nCtrl--")
						{
							if (true == statusReadOnly)
								pWdgt->setEnabled(false);
							else
								pWdgt->setEnabled(true);
						}
						else if (sName == "Clear Row\nDelete")
						{
							if (true == statusReadOnly)
								pWdgt->setEnabled(false);
							else
								pWdgt->setEnabled(true);
						}
						else if (sName == "Cut\nCtrl+X")
						{
							if (true == statusReadOnly)
								pWdgt->setEnabled(false);
							else
								pWdgt->setEnabled(true);
						}
						else if (sName == "Copy\nCtrl+C")
						{
							if (true == statusReadOnly)
								pWdgt->setEnabled(false);
							else
								pWdgt->setEnabled(true);
						}
						else if (sName == "Paste\nCtrl+V")
						{
							if (true == statusReadOnly)
								pWdgt->setEnabled(false);
							else
								pWdgt->setEnabled(true);
						}
						else if (sName == "Show Details")
						{
							if (true == statusReadOnly)
								pWdgt->setEnabled(false);
							else if (detailViewExists(lstSelectedRows.at(0)))
								pWdgt->setEnabled(true);
							else
								pWdgt->setEnabled(false);
						}
					}
				}
			}
		}

		_pLiveEditorContextMenu->exec(ui->EditorTV->viewport()->mapToGlobal(iPos));
	}
}

/**
  * @brief: This method creates context menu for Live Editor
  * @iFileName: Arg passed for file name and deduce file type
  */
void GTAEditorWindow::createLiveEditorContextMenu(const QString iFileName)
{
	//Here we use the automatic deletion of QObject derived classes!
	//The only thing you need to do is: pass parent pointer to the child constructor.
	//Then, if parent is deleted, the child will be deleted as well.
	//It is explained here: http://doc.qt.io/qt-5/objecttrees.html
	//In our case, we delete _pContextMenu in destructor
	//Making _pContextMenu as parent for all QAction and QMenu will handle their deletion automatically
	//The parent concept is tried and tested. Just connect each QMenu and QAction signal destroyed() to a slot
	//the slot is hit the same number of times as many number of times the object is created

	if (nullptr != _pContextMenu)
	{
		delete _pContextMenu;
		_pContextMenu = nullptr;
	}

	_pLiveEditorContextMenu = new QMenu(this);
	QMenu* pEditorMenu = new QMenu("Editor", _pLiveEditorContextMenu);
	QMenu* pActionMenu = new QMenu("Action", pEditorMenu);
	QMenu* pCheckMenu = new QMenu("Check", pEditorMenu);

	//we can't use the editor controller to get the element because we have a dummy element loaded initially during the creation of the context menu
	//this causes improper population of the context menu
	GTAElement::ElemType elemtType = GTAAppController::getGTAAppController()->getElementTypeFromExtension(iFileName.split(".").last());

	QHash<QString, QStringList> actionList;
	QList<QString> checkList;
	QStringList ordrLst;
	if (elemtType == GTAElement::OBJECT)
	{
		actionList = _pMDIController->getActionCommandList(GTAUtil::OBJECT, ordrLst);
		checkList = _pMDIController->getCheckCommandList(GTAUtil::OBJECT);
	}
	else if (elemtType == GTAElement::FUNCTION)
	{
		actionList = _pMDIController->getActionCommandList(GTAUtil::FUNCTION, ordrLst);
		checkList = _pMDIController->getCheckCommandList(GTAUtil::FUNCTION);
	}
	else if (elemtType == GTAElement::PROCEDURE)
	{
		actionList = _pMDIController->getActionCommandList(GTAUtil::PROCEDURE, ordrLst);
		checkList = _pMDIController->getCheckCommandList(GTAUtil::PROCEDURE);
	}
	else if (elemtType == GTAElement::SEQUENCE)
	{
		actionList = _pMDIController->getActionCommandList(GTAUtil::SEQUENCE, ordrLst);
		checkList = _pMDIController->getCheckCommandList(GTAUtil::SEQUENCE);
	}
	else if (elemtType == GTAElement::TEMPLATE)
	{
		actionList = _pMDIController->getActionCommandList(GTAUtil::TEMPLATE, ordrLst);
		checkList = _pMDIController->getCheckCommandList(GTAUtil::TEMPLATE);
	}

	foreach(QString key, actionList.keys())
	{
		QStringList values = actionList.value(key);
		QMenu* intermediateMenu = nullptr;
		QAction* intermediateAction = nullptr;

		if (values.count() > 0)
		{
			intermediateMenu = new QMenu(key, pActionMenu);
		}
		foreach(QString value, values)
		{
			if (nullptr != intermediateMenu)
			{
				intermediateAction = new QAction(QIcon(":/images/ACTION"), value, intermediateMenu);
				connect(intermediateAction, SIGNAL(triggered()), this, SLOT(onInsertAction()));
				intermediateMenu->addAction(intermediateAction);
			}


		}
		if (nullptr != intermediateMenu)
			pActionMenu->addMenu(intermediateMenu);
		else
		{
			intermediateAction = new QAction(QIcon(":/images/ACTION"), key, pActionMenu);
			connect(intermediateAction, SIGNAL(triggered()), this, SLOT(onInsertAction()));
			pActionMenu->addAction(intermediateAction);
		}
	}
	foreach(QString check, checkList)
	{
		QAction* intermediateCheck = new QAction(QIcon(":/images/CHECK"), check, pCheckMenu);
		connect(intermediateCheck, SIGNAL(triggered()), this, SLOT(onInsertCheck()));
		pCheckMenu->addAction(intermediateCheck);
	}
	pEditorMenu->addMenu(pActionMenu);
	pEditorMenu->addMenu(pCheckMenu);
	_pLiveEditorContextMenu->addMenu(pEditorMenu);

	//        QAction * pAction = pEditorMenu->addAction(QIcon(":/images/ACTION"),"Action\nF2",this,SLOT(onInsertAction()));
	//        QAction * pCheck = pEditorMenu->addAction(QIcon(":/images/CHECK"),"Check\nF2",this,SLOT(onInsertCheck()));

	//using the below mentioned addAction(overloaded) function automatically adds the menu as parent. Tested!
	_pLiveEditorContextMenu->addAction(QIcon(":/images/forms/img/EDIT.png"), "Change/Edit Command\nF2", this, SLOT(onEditCommand()));
	_pLiveEditorContextMenu->addSeparator();

	QAction* pAddRow = _pLiveEditorContextMenu->addAction(QIcon(":/images/forms/img/ADD_ROW.png"), "Add Row\nCtrl++", this, SLOT(onAddRow()));
	pAddRow->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Plus));
	_pLiveEditorContextMenu->addAction(QIcon(":/images/forms/img/DELETE_ROW.png"), "Delete Row\nCtrl--", this, SLOT(onDeleteRow()));
	_pLiveEditorContextMenu->addAction(QIcon(":/images/forms/img/CLEAR.jpg"), "Clear Row\nDelete", this, SLOT(onClearRow()));
	_pLiveEditorContextMenu->addAction(QIcon(":/images/forms/img/CUT_ROW.png"), "Cut\nCtrl+X", this, SLOT(onEditorCutRow()));
	_pLiveEditorContextMenu->addAction(QIcon(":/images/forms/img/COPY_ROW.png"), "Copy\nCtrl+C", this, SLOT(onEditorCopyRow()));
	_pLiveEditorContextMenu->addAction(QIcon(":/images/forms/img/PASTE_ROW.png"), "Paste\nCtrl+V", this, SLOT(onEditorPasteRow()));

	_pLiveEditorContextMenu->addSeparator();

	_pMoveRowUpAction = new QAction(QIcon(":/images/forms/img/UpArrow_16_16.png"), "Move rows up\nAlt+Up arrow", _pLiveEditorContextMenu);
	connect(_pMoveRowUpAction, SIGNAL(triggered()), this, SLOT(onMoveEditorRowUp()));

	_pMoveDownAction = new QAction(QIcon(":/images/forms/img/DownArrow_16_16.png"), "Move rows down\nAlt+Down arrow", _pLiveEditorContextMenu);
	connect(_pMoveDownAction, SIGNAL(triggered()), this, SLOT(onMoveEditorRowDown()));

	_pLiveEditorContextMenu->addAction(_pMoveRowUpAction);
	_pLiveEditorContextMenu->addAction(_pMoveDownAction);

	QAction* pShowDetail = _pLiveEditorContextMenu->addAction(QIcon(":/images/forms/img/Relation.png"),
		"Show Details",
		this, SLOT(onShowCommandDetail()));
	pShowDetail->setToolTip("Shows expanded view on applicable action");
	pShowDetail->setEnabled(false);

	QMenu* pSubMenu = new QMenu("Repetitions", _pLiveEditorContextMenu);
	QSpinBox* spinB = new QSpinBox();
	spinB->setMaximum(10);
	spinB->setMinimum(1);
	QWidgetAction* pWgtAction = new QWidgetAction(_pLiveEditorContextMenu);

	pWgtAction->setDefaultWidget(spinB);
	pSubMenu->addAction(pWgtAction);
	QAction* pExecute = new QAction("Execute", pSubMenu);
	pSubMenu->addAction(pExecute);
	connect(pExecute, SIGNAL(triggered()), this, SLOT(onRepeatSnippet()));

	_pLiveEditorContextMenu->addSeparator();
	_pLiveEditorContextMenu->addMenu(pSubMenu);

}

/**
  * @brief: This slot invoked on repitions execute context menu trigger.
  *         This method copy the selected row and paste n-times at the end in Live Edidor.
  */
void GTAEditorWindow::onRepeatSnippet()
{
	int repeatTimes = 0;
	QAction* act = dynamic_cast<QAction*>(sender());
	if (act->text() == "Execute")
	{
		QMenu* pMenu = dynamic_cast<QMenu*>(act->parent());
		if (pMenu)
		{
			QWidgetAction* wgtAction = dynamic_cast<QWidgetAction*>(pMenu->actions().first());
			if (wgtAction)
			{
				QSpinBox* mySpinBox = dynamic_cast<QSpinBox*>(wgtAction->defaultWidget());
				if (mySpinBox)
				{
					repeatTimes = mySpinBox->value();
					mySpinBox->setValue(mySpinBox->minimum());
				}
			}
		}
	}

	onEditorCopyRow();

	QList<int> indexList;
	for (int j = 1; j <= repeatTimes; j++)
	{
		int indexRow = -1;
		int editorRowCount = getElement()->getAllChildrenCount();
		for (int i = 0; i < editorRowCount; i++)
		{
			if (getCommandForSelectedRow(i))
			{
				indexRow = i;
				continue;
			}
		}
		indexList.append(indexRow + 1);

		// Add a new blank row at (indexRow) if there is no empty row at last
		QList<int> addRowAtLocList;
		addRowAtLocList.append(indexRow);
		if (indexRow + 1 == editorRowCount)
		{
			_pUndoStack->push(new GTAUndoRedoAddRow(1, addRowAtLocList, this));
		}

		if (nullptr != _pEditorController && nullptr != _pMDIController)
		{
			QList<GTACommandBase*> cmdPasted;
			_pMDIController->onGetCopiedItems(cmdPasted);
			foreach(GTACommandBase * cmd, cmdPasted)
			{
				if (nullptr != cmd && cmd->getReadOnlyState() == true)
					cmd->setReadOnlyState(false);
			}
			_pUndoStack->push(new GTAUndoRedoPaste(1, indexList, cmdPasted, this));
		}
		indexList.clear();
		addRowAtLocList.clear();
	}
}

/**
  * @brief: function call to set live editor column width
  */
void GTAEditorWindow::resizeLiveEditorHeader()
{
	storeColoumWidths();
	if (ui->EditorTV && ui->EditorTV->model() && ui->EditorTV->horizontalHeader())
	{
		for (int i = 0; i < _ColWidths.size(); i++)
			ui->EditorTV->setColumnWidth(i, _ColWidths.at(i));

		ui->EditorTV->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
		ui->EditorTV->horizontalHeader()->setSectionResizeMode(GTAElementViewModel::BREAKPOINT_INDEX, QHeaderView::Fixed);

		ui->EditorTV->horizontalHeader()->setStretchLastSection(true);
		ui->EditorTV->setSelectionMode(QAbstractItemView::ExtendedSelection);
		ui->EditorTV->setColumnWidth(GTAElementViewModel::ACTION_INDEX, 800);
	}
}

/**
  * @brief: function call to check whether selected rows contains read only commands
  *         return true if selected rows has read only commands otherwise false
  */
bool GTAEditorWindow::getReadOnlyCmdStatus()
{
	QList<int> lstSelectedRows;
	getSelectedEditorRows(lstSelectedRows);
	bool readOnlyCmdStatus = false;

	foreach(int row, lstSelectedRows)
	{
		GTACommandBase* pCmd = getCommandForSelectedRow(row);
		if (pCmd != nullptr)
		{
			if (pCmd->getReadOnlyState())
			{
				readOnlyCmdStatus = true;
				break;
			}
		}
	}
	return readOnlyCmdStatus;
}

void GTAEditorWindow::removeEmptyRows()
{
	if (nullptr != _pEditorController)
	{
		_pEditorController->removeEmptyRows();
	}
}