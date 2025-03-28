#include "GTAEditorViewController.h"
#include "GTADocumentController.h"
#include "GTACommandBase.h"
#include "GTACommandVariant.h"

#pragma warning(push, 0)
#include <QStack>
#include <QDir>
#pragma warning(pop)

GTAEditorViewController::GTAEditorViewController(QObject* iObject) : QObject(iObject)
{

	_pElement = NULL;
	_pElementViewModel = NULL;

	_pHeader = NULL;
	_pHeaderModel = NULL;
	_GtaDataDirPath = QString("");
	_previousInsertRowIndex = -1;
}

GTAEditorViewController::~GTAEditorViewController()
{
	if (_pElement != NULL)
	{
		delete _pElement;
		_pElement = NULL;
	}

	if (_pElementViewModel != NULL)
	{
		delete _pElementViewModel;
		_pElementViewModel = NULL;
	}

	if (_pHeader != NULL)
	{
		delete _pHeader;
		_pHeader = NULL;
	}

	if (_pHeaderModel != NULL)
	{
		delete _pHeaderModel;
		_pHeaderModel = NULL;
	}
}

/* This function allows to initialize the EditorView after creating a new file
* @input: QTableView
* @return: bool
*/
bool GTAEditorViewController::setInitialEditorView(QTableView* ipView)
{
	bool rC = false;
	if (ipView)
	{
		_pElement = new GTAElement(GTAElement::PROCEDURE, 5); //Creating with 5 rows by default
		if (_pHeader)
		{
			_pElement->setHeader(_pHeader);
		}
		_pElementViewModel = new GTAElementViewModel(_pElement, this);
		ipView->setModel(_pElementViewModel);
		ipView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
		ipView->horizontalHeader()->setStretchLastSection(true);
		ipView->setDragEnabled(true);
		ipView->setAcceptDrops(true);
		ipView->setDragDropMode(QAbstractItemView::DragDrop);
		ipView->viewport()->setAcceptDrops(true);
		ipView->setDropIndicatorShown(true);
		ipView->setSelectionMode(QAbstractItemView::ExtendedSelection);
		ipView->show();
		rC = true;

		_pEditorView = ipView;
	}
	return rC;
}

void GTAEditorViewController::establishConnectionWithModel()
{
	if (NULL != _pElementViewModel)
	{
		connect(_pElementViewModel, SIGNAL(callActionInserted(int&, QString&)), this, SLOT(onCallActionInserted(int&, QString&)));

	}

}

void GTAEditorViewController::onCallActionInserted(int& iRow, QString& iFileName)
{
	emit callActionDropped(iRow, iFileName);
}

bool GTAEditorViewController::setInitialHeaderView(QTableView* ipView)
{
	bool rC = false;


	QString headerTemplateFilePath;

	emit getHeaderTemplateFilePath(headerTemplateFilePath);

	if (!headerTemplateFilePath.isEmpty())
	{
		GTADocumentController docController;
		GTAHeader* pHeader = NULL;
		GTAElement* pElement = NULL;


		rC = docController.openDocument(headerTemplateFilePath, pElement, pHeader, false);

		if (rC)
		{
			rC = setHeaderView(ipView, pHeader);
			if (rC)
			{
				GTAElement* pSubElement = getElement();
				if (NULL != pSubElement && NULL != pHeader)
					pSubElement->setHeader(pHeader);
			}
		}
		else
		{
			ipView->setModel(NULL);
			_LastError = docController.getLastError();
		}

	}
	return rC;
}

bool GTAEditorViewController::setHeaderView(QTableView* ipView, GTAHeader*& ipHeader)
{
	bool rc = false;

	if (ipHeader != NULL)
	{

		GTAHeaderTableModel* pCurrentModel = _pHeaderModel;
		_pHeaderModel = new GTAHeaderTableModel(ipHeader);
		ipView->setModel(_pHeaderModel);
		ipView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
		ipView->horizontalHeader()->setStretchLastSection(true);


		if (pCurrentModel != NULL)
		{
			delete pCurrentModel; pCurrentModel = NULL;
		}
		if (_pHeader != NULL)
		{
			delete _pHeader; _pHeader = NULL;
		}
		_pHeader = ipHeader;

		rc = true;
	}
	else
		ipView->setModel(NULL);

	return rc;

}

bool GTAEditorViewController::createAndSetElementFromDataDirectoryDoc(const QString& iFileName, bool isOutsideDataDir)
{

	bool rc = false;
	GTAElement* pElement = NULL;
	GTAHeader* pHeader = NULL;
	GTAAppController* pAppCtrl = NULL;
	GTADocumentController docController;
	//    emit getGTADataDirectory(sDataDirectory);
	if (!_GtaDataDirPath.isEmpty())
	{

		QString completeFilePath;
		if (isOutsideDataDir)
			completeFilePath = iFileName;
		else
			completeFilePath = QString("%1/%2").arg(_GtaDataDirPath, iFileName);


		if (!isOutsideDataDir)
		{
			pAppCtrl = GTAAppController::getGTAAppController();
			rc = pAppCtrl->getElementFromDocument(iFileName, pElement, false, false);
			if (pElement != NULL)
				pHeader = pElement->getHeader();
		}
		else
		{
			rc = docController.openDocument(completeFilePath, pElement, pHeader, false);
		}
		if (rc && pElement)
		{

			rc = setElement(pElement);
			if (pHeader)
			{
				pElement->setHeader(pHeader);
				rc = setHeader(pHeader);
			}

		}
		else
		{
			if (isOutsideDataDir)
				_LastError = docController.getLastError();
			else
				_LastError = pAppCtrl->getLastError();
			_LastError.append("\nFile may be corrupted or is conflicted");
		}
	}
	return rc;
}

bool GTAEditorViewController::setElement(const GTAElement* isElement)
{
	bool rc = false;
	if (NULL != isElement)
	{
		if (_pElement)
		{
			delete _pElement;
			_pElement = NULL;
		}

		_pElement = (GTAElement*)isElement;
		rc = true;
	}
	else
	{
		_LastError = "empty element cannot be set";
	}
	if (!rc)
		_LastError = "element type cannot does not match";

	return rc;
}

/**
 * @brief Update the Maximum Time Estimated value
 * @param maxTimeEstimated : Maximum Time Estimated value
 * @return bool 
*/
bool GTAEditorViewController::updateElementMaxTimeEstimated(const QString& maxTimeEstimated)
{
	bool rc = false;
	if (!maxTimeEstimated.isEmpty())
	{
		_pElement->setMaxTimeEstimated(maxTimeEstimated);
		rc = true;
	}
	return rc;
}

bool GTAEditorViewController::setHeader(GTAHeader* ipheader)
{
	bool rC = false;
	if (ipheader != NULL)
	{
		if (NULL != _pHeader)
		{
			delete _pHeader;
			_pHeader = NULL;
		}
		_pHeader = ipheader;
		rC = true;
	}
	else
		_LastError = "Header data missing";

	return rC;
}

bool GTAEditorViewController::getElementModel(QAbstractItemModel*& opModel)
{
	bool rc = false;
	if (_pElement)
	{
		if (_pElementViewModel != NULL)
		{
			delete _pElementViewModel;
			_pElementViewModel = NULL;
		}
		_pElementViewModel = new GTAElementViewModel(_pElement);
		opModel = _pElementViewModel;
		rc = true;
	}
	else
	{
		opModel = NULL;
		_LastError = "Empty header";
	}
	establishConnectionWithModel();
	return rc;
}

bool GTAEditorViewController::getHeaderModel(QAbstractItemModel*& iopModel)
{
	bool rc = false;
	if (_pHeader)
	{
		if (_pHeaderModel != NULL)
		{
			delete _pHeaderModel; _pHeaderModel = NULL;
		}

		_pHeaderModel = new GTAHeaderTableModel(_pHeader);
		iopModel = _pHeaderModel;
		rc = true;
	}
	else
	{
		iopModel = NULL;
		_LastError = "Empty header";
	}
	return rc;

}

GTAElement* GTAEditorViewController::getElement(void)const
{
	return _pElement;
}
GTAHeader* GTAEditorViewController::getHeader() const
{
	return _pHeader;
}

void GTAEditorViewController::updateEditorView()const
{
	_pElementViewModel->updateChanges();
}

QStringList GTAEditorViewController::getHiddenRows()
{
	if (NULL != _pElement)
	{
		return _pElement->getHiddenRows();
	}

	return QStringList();
}


//Returns command for the selected row
GTACommandBase* GTAEditorViewController::getCommand(const int& iSelRow)const
{
	GTACommandBase* oCmd = NULL;
	if (iSelRow < _pElement->getAllChildrenCount())
	{
		_pElement->getCommand(iSelRow, oCmd);

	}
	return oCmd;

}


bool GTAEditorViewController::isEditableCommand(GTACommandBase* pCmd)
{
	bool bOK = true;
	if (NULL != pCmd)
		bOK = pCmd->IsUserEditable();
	return bOK;
}

void GTAEditorViewController::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (_pElementViewModel != NULL)
	{
		_pElementViewModel->setData(index, value, role);
	}
}


bool GTAEditorViewController::editAction(const int& iRow, const QVariant& incomingData)
{
	bool rC = false;
	QModelIndex index = _pElementViewModel->index(iRow, GTAElementViewModel::ACTION_INDEX);
	rC = _pElementViewModel->setData(index, incomingData);
	if (!rC)
		_LastError = _pElementViewModel->getLastError();

	return rC;
}


void GTAEditorViewController::undoClearRow(const QList<int>& iIndexList, QList<GTACommandBase*>& iCmdList)
{
	if (_pElementViewModel)
	{
		for (int i = 0; i < iIndexList.count(); i++)
		{
			int iIndex = iIndexList.at(i);
			GTACommandBase* pCmd = iCmdList.at(i);
			int totalchildren = pCmd->getAllChildrenCount();
			GTACommandVariant cmdVariant;
			cmdVariant.setCommand(pCmd);
			QVariant variant;
			variant.setValue(cmdVariant);
			QModelIndex index = _pElementViewModel->index(iIndex, 0);

			// bool rC = 
			_pElementViewModel->setData(index, variant);


			QList<int> lstIndex;

			GTACommandBase* pNextCommand = NULL;
			_pElement->getCommand(iIndex + totalchildren + 1, pNextCommand);

			if (pNextCommand == NULL)
				lstIndex.append(iIndex + totalchildren + 1);

			for (int j = 1; j <= totalchildren; j++)
			{
				GTACommandBase* pSubNextCommand = NULL;
				_pElement->getCommand(iIndex + totalchildren + 1 + j, pSubNextCommand);
				if (pSubNextCommand == NULL)
					lstIndex.append(iIndex + totalchildren + 1 + j);
			}

			if ((iIndex > 0) || (totalchildren > 0))
				deleteEditorRows(lstIndex);

		}
	}

}

void GTAEditorViewController::removeEmptyRows()
{
	if (_pElementViewModel != NULL)
	{
		QList<int> emptyRows;
		int rows = _pElementViewModel->rowCount();
		for (int i = 0; i < rows; i++)
		{
			GTACommandBase* pCommand = getCommand(i);
			if (pCommand)
				continue;
			else
				emptyRows.append(i);
		}
		deleteEditorRows(emptyRows);
	}
}

void GTAEditorViewController::deleteEditorRows(const QList<int>& lstSelectedRows)
{
	bool rC = false;
	if (!lstSelectedRows.isEmpty())
	{
		QList<int> selectedRows = lstSelectedRows;
		qSort(selectedRows.begin(), selectedRows.end(), qGreater<int>());
		foreach(int row, selectedRows)
		{
			if (_pElementViewModel)
			{
				rC = _pElementViewModel->removeRows(row, 1);
			}
		}
	}
}

void GTAEditorViewController::clearAllinEditor()
{
	if (_pElement)
	{
		_pElementViewModel->clearAllinEditor();
	}
}

bool GTAEditorViewController::highLightLastChild(const QModelIndex& ipIndex)
{
	bool rc = false;
	if (_pElementViewModel)
	{

		rc = _pElementViewModel->highLightLastChild(ipIndex);
	}
	return rc;
}

bool GTAEditorViewController::clearRow(int iSelectedRow, QString& oErrMsg, QList<int>& oClearedRowId)
{
	bool rc = true;
	if (iSelectedRow >= 0)
	{
		GTACommandBase* pCommand = getCommand(iSelectedRow);
		if (pCommand != NULL)
		{
			if (pCommand->IsDeletable())
			{
				int numberOfChildren = pCommand->getAllChildrenCount();

				GTAActionElse* pElse = dynamic_cast<GTAActionElse*> (pCommand);

				if (pElse && iSelectedRow > 0)
				{
					if (pCommand->hasChildren())
					{
						oClearedRowId.append(iSelectedRow);
						for (int i = 1; i <= numberOfChildren; i++)
						{
							oClearedRowId.append(iSelectedRow + i);
						}	
					}

					QList<int> deleteRowList; 
					deleteRowList.append(iSelectedRow);
					deleteEditorRows(deleteRowList);

					for (int index = 0; index <= numberOfChildren; index++)
					{
						addEditorRows(iSelectedRow - 1);
					}
				}
				else
				{
					addEditorRows(numberOfChildren + iSelectedRow);
					for (int i = 0; i < numberOfChildren; i++)
					{
						addEditorRows(numberOfChildren + iSelectedRow);
					}

					if (pCommand->hasChildren())
					{
						oClearedRowId.append(iSelectedRow);
						for (int i = 1; i <= numberOfChildren; i++)
							oClearedRowId.append(iSelectedRow + i);

					}
					QList<int> deleteRowList; deleteRowList.append(iSelectedRow);
					deleteEditorRows(deleteRowList);
				}
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

void GTAEditorViewController::addEditorRows(const int& iSelectedRow)
{
	if (_pElementViewModel)
		_pElementViewModel->insertRows(iSelectedRow, 0);
}

QString GTAEditorViewController::getLastError()const
{
	return _LastError;
}

bool GTAEditorViewController::areSelectedCommandsBalanced(QList<int>& ioLstSelectedRows)
{
	bool rc = true;
	QStringList _errMsg;
	if (!ioLstSelectedRows.isEmpty())
	{
		QStack<GTACommandBase*> balanceIfStack;
		QStack<GTACommandBase*> balanceWhileStack;
		QStack<GTACommandBase*> balanceDoWhileStack;
		QStack<GTACommandBase*> balanceForEachStack;
		for (int i = 0; i < ioLstSelectedRows.count(); i++)
		{
			int dRow = ioLstSelectedRows.at(i);
			GTACommandBase* pCmd = NULL;
			bool rC = _pElement->getCommand(dRow, pCmd);
			if (rC)
			{
				GTAActionIF* pIF = dynamic_cast<GTAActionIF*>(pCmd);
				GTAActionIfEnd* pEnd = dynamic_cast<GTAActionIfEnd*>(pCmd);

				GTAActionWhile* pWhile = dynamic_cast<GTAActionWhile*>(pCmd);
				GTAActionWhileEnd* pWhileEnd = dynamic_cast<GTAActionWhileEnd*>(pCmd);

				GTAActionDoWhile* pDoWhile = dynamic_cast<GTAActionDoWhile*>(pCmd);
				GTAActionDoWhileEnd* pDoWhileEnd = dynamic_cast<GTAActionDoWhileEnd*>(pCmd);

				GTAActionForEach* pForEach = dynamic_cast<GTAActionForEach*>(pCmd);
				GTAActionForEachEnd* pForEacheEnd = dynamic_cast<GTAActionForEachEnd*>(pCmd);

				if (pIF)
					balanceIfStack.push(pIF);
				if (pEnd)
				{
					if (balanceIfStack.isEmpty())
					{
						ioLstSelectedRows.removeAt(i);
						rc = false;
					}

					else
					{
						GTACommandBase* pPoppedCmd = balanceIfStack.pop();
						if (pPoppedCmd != pEnd->getParent())
						{
							ioLstSelectedRows.removeAt(i);
							rc = false;
						}
					}
				}


				if (pWhile)
					balanceWhileStack.push(pWhile);
				if (pWhileEnd)
				{
					if (balanceWhileStack.isEmpty())
					{
						ioLstSelectedRows.removeAt(i);
						rc = false;

					}
					else
					{
						GTACommandBase* pPoppedCmd = balanceWhileStack.pop();
						if (pPoppedCmd != pWhileEnd->getParent())
						{
							ioLstSelectedRows.removeAt(i);
							rc = false;
						}
					}
				}

				if (pDoWhile)
					balanceDoWhileStack.push(pDoWhile);
				if (pDoWhileEnd)
				{
					if (balanceDoWhileStack.isEmpty())
					{
						ioLstSelectedRows.removeAt(i);
						rc = false;
					}
					else
					{
						GTACommandBase* pPoppedCmd = balanceDoWhileStack.pop();
						if (pPoppedCmd != pDoWhileEnd->getParent())
						{
							ioLstSelectedRows.removeAt(i);
							rc = false;
						}
					}
				}




				if (pForEach)
					balanceForEachStack.push(pForEach);
				if (pForEacheEnd)
				{
					if (balanceForEachStack.isEmpty())
					{
						ioLstSelectedRows.removeAt(i);
						rc = false;
					}
					else
					{
						GTACommandBase* pPoppedCmd = balanceForEachStack.pop();
						if (pPoppedCmd != pForEacheEnd->getParent())
						{
							ioLstSelectedRows.removeAt(i);
							rc = false;
						}
					}
				}

			}

		}
	}

	if (!rc)
	{
		qDebug() << "Row Selection Error. End statement cannot be deleted exclusively";
	}
	return rc;

}

bool GTAEditorViewController::areSelectedCommandsNested(const QList<int>& iLstSelectedRows)
{
	bool rC = false;
	if (!iLstSelectedRows.isEmpty())
	{
		foreach(int dRow, iLstSelectedRows)
		{
			GTACommandBase* pCmd = NULL;
			_pElement->getCommand(dRow, pCmd);
			if (pCmd != NULL && !pCmd->getChildren().isEmpty())
			{
				rC = true;
				break;
			}
		}
	}
	return rC;

}

bool GTAEditorViewController::insertCommands(const QList<int>& iIndexList, const QList<GTACommandBase*>& iCmdList, bool isSrcUndoRedo)
{
	bool rc = true;
	int rowsAffected = 0;
	if (_pElement)
	{

		for (int i = 0; i < iCmdList.count(); i++)
		{
			GTACommandBase* pCmd = iCmdList.at(i);
			int position = iIndexList.at(i);//+ rowsAffected;
			rowsAffected += insertCommand(position, pCmd, isSrcUndoRedo);

		}
	}

	return rc;
}

int GTAEditorViewController::insertCommand(const int& iIndex, GTACommandBase* iCmd, bool isSrcUndoRedo)
{
	int rowsAffected = 0;
	if (_pElementViewModel)
	{

		if (iCmd != NULL)
		{

			if (iCmd->hasChildren())
			{
				int totalchildren = iCmd->getAllChildrenCount();
				int rCnt;
				if (iIndex == 0)
					rCnt = iIndex;
				else
					rCnt = iIndex - 1;
				addEditorRows(rCnt);

				if (iIndex == 0 && isSrcUndoRedo)
					moveEditorRowItemDown(0);

				GTACommandVariant cmdVariant;
				cmdVariant.setCommand(iCmd);
				QVariant variant;
				variant.setValue(cmdVariant);
				QModelIndex index = _pElementViewModel->index(iIndex, 0);

				//bool rC = 
				_pElementViewModel->setData(index, variant);
				QList<int> lstIndex;
				GTACommandBase* pNextCommand = NULL;
				_pElement->getCommand(iIndex + totalchildren + 1, pNextCommand);
				if (isSrcUndoRedo && iIndex != 0 && pNextCommand == NULL)
				{
					lstIndex.append(iIndex + totalchildren + 1);
					deleteEditorRows(lstIndex);
				}

			}
			else
			{
				int rCnt;
				if (iIndex == 0)
					rCnt = iIndex;
				else
					rCnt = iIndex - 1;
				addEditorRows(rCnt);

				rowsAffected = 1;

				if (iIndex == 0 && isSrcUndoRedo)
					moveEditorRowItemDown(0);

				GTACommandVariant cmdVariant;
				cmdVariant.setCommand(iCmd);
				QVariant variant;
				variant.setValue(cmdVariant);
				QModelIndex index = _pElementViewModel->index(iIndex, 0);
				// bool rC = 
				_pElementViewModel->setData(index, variant);
				QList<int> lstIndex;

				GTACommandBase* pNextCommand = NULL;
				_pElement->getCommand(iIndex + 1, pNextCommand);

				if (isSrcUndoRedo && iIndex != 0  && pNextCommand == NULL)
				{
					lstIndex.append(iIndex + 1);
					deleteEditorRows(lstIndex);
				}
			}
		}
		if (isSrcUndoRedo && iCmd == NULL)
		{
			int rCnt;
			if (iIndex == 0)
				rCnt = iIndex;
			else
				rCnt = iIndex - 1;
			addEditorRows(rCnt);
			rowsAffected = 1;
			GTACommandVariant cmdVariant;
			cmdVariant.setCommand(iCmd);
			QVariant variant;
			variant.setValue(cmdVariant);
			QModelIndex index = _pElementViewModel->index(iIndex, 0);
			// bool rC = 
			_pElementViewModel->setData(index, variant);
			QList<int> lstIndex;
			lstIndex.append(iIndex + 1);
			GTACommandBase* pNextCommand = NULL;
			_pElement->getCommand(iIndex + 1, pNextCommand);
			if (pNextCommand == NULL && _previousInsertRowIndex != iIndex)
			{
				deleteEditorRows(lstIndex);
			}

			_previousInsertRowIndex = iIndex;

		}
	}
	return rowsAffected;
}

bool GTAEditorViewController::pasteEditorCopiedRow(const int& iPasteAtRow)
{
	bool rC = true;

	GTACommandBase* pCurrentCommand = NULL;
	_pElement->getCommand(iPasteAtRow, pCurrentCommand);
	if (pCurrentCommand)
	{
		_LastError = "Paste allowed on empty rows only.";
		return rC;
	}

	QList<GTACommandBase*> lstCopiedElements;
	emit getCopiedItems(lstCopiedElements);
	if (!lstCopiedElements.isEmpty())
	{
		int sizeOfCopiedItems = lstCopiedElements.size();
		if (sizeOfCopiedItems)
		{
			int rowToBePasted = iPasteAtRow;
			for (int i = 0; i < sizeOfCopiedItems; i++)
			{
				bool bAddNewRow = true;
				GTACommandBase* pCurrentCopiedItem = lstCopiedElements.at(i);
				if (pCurrentCopiedItem)
				{
					GTACommandVariant cmdVariant;
					cmdVariant.setCommand(pCurrentCopiedItem);
					QVariant variant; variant.setValue(cmdVariant);
					QModelIndex index = _pElementViewModel->index(rowToBePasted, 0);

					int noOfChildrenForCurCmd = pCurrentCopiedItem->getAllChildrenCount();
					if (noOfChildrenForCurCmd)
						rowToBePasted = rowToBePasted + noOfChildrenForCurCmd + 1;
					else
					{
						rowToBePasted++;
						bAddNewRow = false;
					}

					rC = _pElementViewModel->setData(index, variant);
					//TODO: Shouldn't we check for false rC here?
					GTACommandBase* nxtCmd = NULL;
					auto foundCommand = _pElement->getCommand(rowToBePasted, nxtCmd);
					//TODO: rC = rC && _pElement->getCommand(rowToBePasted, nxtCmd);
					if (!foundCommand && sizeOfCopiedItems - 1 != i)
					{
						addEditorRows(rowToBePasted - 1);
						bAddNewRow = false;
					}
				}
				else
				{
					addEditorRows(rowToBePasted);
				}
				if (bAddNewRow && sizeOfCopiedItems <= i)
					addEditorRows(rowToBePasted);

				if (pCurrentCopiedItem == NULL)
					rowToBePasted++;

			}

		}

	}
	else
	{
		rC = false;
		_LastError = "Nothing to paste";
		return rC;
	}
	return rC;
}


bool GTAEditorViewController::pasteEditorCopiedRow(const int& iPasteAtRow, const QList<GTACommandBase*> CmdsPasted, QString lastCalled)
{
	bool rC = false;

	GTACommandBase* pCurrentCommand = NULL;
	_pElement->getCommand(iPasteAtRow, pCurrentCommand);
	
	// Destination row is not empty
	if (pCurrentCommand)
	{
		_LastError = "Paste allowed on empty rows only.";		
		return rC;
	}
	// Copied row is empty
	if (CmdsPasted.isEmpty())
	{
		_LastError = "Nothing to paste";
		return rC;
	}
	
	// Loop on items to be copied
	int rowToBePasted = iPasteAtRow;
	for (int i = 0; i < CmdsPasted.size(); i++)
	{
		//bool bAddNewRow = true;
		GTACommandBase* pCurrentCopiedItem = CmdsPasted.at(i);

		// If current item to be copied is not empty
		if (pCurrentCopiedItem)
		{
			if (lastCalled == "copy")
			{
				pCurrentCopiedItem->setObjId();
			}

			// Update the element
			GTACommandVariant cmdVariant;
			cmdVariant.setCommand(pCurrentCopiedItem);
			QVariant variant; variant.setValue(cmdVariant);
			QModelIndex index = _pElementViewModel->index(rowToBePasted, 0);
			rC = _pElementViewModel->setData(index, variant);

			// Jump to last child row if the item has children
			rowToBePasted = pCurrentCopiedItem->hasChildren() ? rowToBePasted + pCurrentCopiedItem->getAllChildrenCount() : rowToBePasted;			
		}
		// Else add an empty row
		else
		{
			addEditorRows(rowToBePasted);
		}
		
		// If there are still cmds to be pasted
		if (i < CmdsPasted.size() - 1)
		{	
			GTACommandBase* nxtCmd = NULL;
			rC = _pElement->getCommand(rowToBePasted, nxtCmd);
			// If next row it is not null then add new row or paste in last row
			if (nxtCmd)
			{
				addEditorRows(rowToBePasted);
			}
		}	
		rowToBePasted++;
	}

	rC = true;
	return rC;
}


bool GTAEditorViewController::setEditorRowCopy(QList<int>& lstSelectedRows, bool isNewCommand)
{
	bool rc = false;
	rc = setCopiedItems(lstSelectedRows, isNewCommand);
	return rc;
}

bool GTAEditorViewController::setCopiedItems(QList<int>& ioLstSelectedRows, bool isNewCommand)
{
	bool rc = false;
	clearEditorRowCopy();
	areSelectedCommandsBalanced(ioLstSelectedRows);

	if (!ioLstSelectedRows.isEmpty())
	{
		int skipRowTill = -1;
		QList<int> lstSelectedRows = ioLstSelectedRows;
		qSort(lstSelectedRows.begin(), lstSelectedRows.end());
		for (int i = 0; i < lstSelectedRows.size(); i++)
		{
			int currentRow = lstSelectedRows.at(i);
			if (currentRow <= skipRowTill)
				continue;
			if (i > lstSelectedRows.size())
			{
				_LastError = "Error size exceeds skipped rows";
				return false;
			}
			GTACommandBase* pBase = getCommand(currentRow);

			GTACommandBase* pCopy = NULL;
				if (pBase != NULL)
				{
					pCopy = pBase->getClone();
				}
				GTAGenericToolCommand* pGenCmd = dynamic_cast<GTAGenericToolCommand*>(pCopy);
				GTAGenericToolCommand* pBaseGenCmd = dynamic_cast<GTAGenericToolCommand*>(pBase);

				if ((pGenCmd != NULL) && (pBaseGenCmd != NULL))
				{
					if (isNewCommand)
					{
						//saurav
						pGenCmd->setObjId();
						pCopy = pGenCmd;
					}
				}
				//                _lstCopiedElements.append(pCopy);



				emit setCopiedItem(pCopy);

				int totalChildrens = 0;
				if (pBase != NULL)
					totalChildrens = pBase->getAllChildrenCount();


				if (totalChildrens != 0)
					skipRowTill = currentRow + totalChildrens; //skip those rows since these will be copied automatically

				rc = true;
		}
	}
	return rc;
}


//Dev Comment : check if still needed

bool GTAEditorViewController::setEditorRowCopy(QList<int>&, const QList<GTACommandBase*>&)
{

	//    bool rc = false;

	//    if(_pMDIController)
	//    {
	//        rc = setCopiedItems(lstSelectedRows,iCmdList);
	//        if(!lstSelectedRows.isEmpty())
	//        {
	//            _pMDIController->setCopiedItems(lstSelectedRows);
	//        }
	//    }
	//    return rc;
	return true;
}

//Dev Comment : check if still needed

bool GTAEditorViewController::setCopiedItems(QList<int>&, const QList<GTACommandBase*>&)
{
	//    bool rc = false;
	//    clearEditorRowCopy();
	//    areSelectedCommandsBalanced(ioLstSelectedRows);

	//    if(!ioLstSelectedRows.isEmpty())
	//    {
	//        int skipRowTill = -1;
	//        QList<int> lstSelectedRows = ioLstSelectedRows;
	//        qSort(lstSelectedRows.begin(),lstSelectedRows.end());
	//        for(int i=0;i<lstSelectedRows.size();i++)
	//        {
	//            int currentRow = lstSelectedRows.at(i);
	//            if(currentRow<=skipRowTill)
	//                continue;
	//            if (i>lstSelectedRows.size())
	//            {
	//                _LastError="Error size exceeds skipped rows";
	//                return false;
	//            }
	//            GTACommandBase* pBase = getCommand(currentRow);

	//            GTACommandBase* pCopy=NULL;
	//            if (pBase!=NULL)
	//            {
	//                pCopy = pBase->getClone();
	//            }

	//            //            _lstCopiedElements.append(pCopy);


	//            _pMDIController->setCopiedItem(pCopy);


	//            int totalChildrens = 0;
	//            if (pBase!=NULL)
	//                totalChildrens = pBase->getAllChildrenCount();


	//            if (totalChildrens!=0)
	//                skipRowTill = currentRow + totalChildrens; //skip those rows since these will be copied automatically

	//            rc = true;

	//        }
	//    }
	//    return rc;
	return true;
}


void GTAEditorViewController::clearEditorRowCopy()
{
	emit clearClipBoardItems();
}


int GTAEditorViewController::getSelectedRowInEditor() const
{
	int oRow = -1;
	if (_pEditorView != NULL)
	{
		QModelIndex index = _pEditorView->currentIndex();
		if (index.isValid())
			oRow = index.row();
	}
	return oRow;
}


int GTAEditorViewController::getRowToBeSelected() const
{
	return _rowToBeSelected;
}


bool GTAEditorViewController::moveEditorRowItemUp(int selectedItem)
{
	bool rc = true;
	if (selectedItem <= 0)
		return false;


	_rowToBeSelected = selectedItem - 1;

	GTACommandBase* pCurrentSelectedCmd = NULL;
	pCurrentSelectedCmd = getCommand(selectedItem)->getClone();
	if (pCurrentSelectedCmd == NULL)
		return false;

	GTACommandBase* pItemAboveSelectedItem = getCommand(selectedItem - 1);
	GTACommandBase* pCurrentItemsParent = pCurrentSelectedCmd->getParent();

	QList<GTACommandBase*> currentLstOfCopiedItem;
	emit getCopiedItems(currentLstOfCopiedItem);
	emit clearClipBoardItems();

	if (pItemAboveSelectedItem == NULL) 
	{
		addEditorRows(selectedItem);
		deleteEditorRows(QList<int>() << selectedItem - 1);
	}
	else
	{
		GTACommandBase* pAboveItemsParent = pItemAboveSelectedItem->getParent();

		if (!pCurrentSelectedCmd->IsUserEditable())
		{
			if (pCurrentItemsParent)
			{
				auto parentChildrenCount = pCurrentItemsParent->getAllChildrenCount();
				int itemAboveParentIdx = selectedItem - parentChildrenCount - 1;
				auto itemAboveParent = getCommand(itemAboveParentIdx)->getClone();
				emit setCopiedItem(itemAboveParent);
				addEditorRows(selectedItem);
				pasteEditorCopiedRow(selectedItem + 1);
				deleteEditorRows({ itemAboveParentIdx });
			}
			else
			{
				if (pItemAboveSelectedItem != pCurrentItemsParent)
				{
					pItemAboveSelectedItem = pItemAboveSelectedItem->getClone();
					addEditorRows(selectedItem);
					emit setCopiedItem(pItemAboveSelectedItem);
					rc = pasteEditorCopiedRow(selectedItem + 1);
					deleteEditorRows(QList<int>() << selectedItem - 1);
				}
			}
		}
		else
		{
			if (pItemAboveSelectedItem == pCurrentItemsParent)
			{
				int rowAboveParent = selectedItem - 2;
				if (rowAboveParent >= 0)
				{
					deleteEditorRows(QList<int>() << selectedItem);
					addEditorRows(rowAboveParent);
					emit setCopiedItem(pCurrentSelectedCmd);
					rc = pasteEditorCopiedRow(++rowAboveParent);
				}
				else
				{
					_pElementViewModel->prependRow();
					emit setCopiedItem(pCurrentSelectedCmd);
					rc = pasteEditorCopiedRow(0);
					deleteEditorRows(QList<int>() << 1 + selectedItem);
				}

			}
			else if (pItemAboveSelectedItem != NULL && pItemAboveSelectedItem->IsUserEditable() == true && pCurrentSelectedCmd->canHaveChildren())
			{
				if (pCurrentSelectedCmd->canHaveChildren() && selectedItem > 0)
				{
					// We enter here if we have children
					//First we copy and delete the above line
					pItemAboveSelectedItem = pItemAboveSelectedItem->getClone();
					emit setCopiedItem(pItemAboveSelectedItem);
					deleteEditorRows(QList<int>() << selectedItem - 1);
					const auto& childrenCount = pCurrentSelectedCmd->getAllChildrenCount();
					addEditorRows(selectedItem + childrenCount - 1);
					pasteEditorCopiedRow(selectedItem + childrenCount);
				}
			}
			else if (pItemAboveSelectedItem->canHaveChildren())
			{
				int rowAboveParent = selectedItem - 2;
				if (rowAboveParent >= 0)
				{
					deleteEditorRows(QList<int>() << selectedItem);
					addEditorRows(rowAboveParent);
					emit setCopiedItem(pCurrentSelectedCmd);
					rc = pasteEditorCopiedRow(++rowAboveParent);

				}
				else
				{
					_pElementViewModel->prependRow();
					emit setCopiedItem(pCurrentSelectedCmd);
					rc = pasteEditorCopiedRow(0);
					deleteEditorRows(QList<int>() << 1 + selectedItem);
				}
			}
			else
			{
				if (!pItemAboveSelectedItem->IsUserEditable())
				{					
					if (pCurrentSelectedCmd->isTitle())
					{
						// Titles are not allowed inside a condition
						// so move the title above the beginning of the condition
						int pAboveItemsParentChildrenCount = pAboveItemsParent->getAllChildrenCount();
						int pAboveItemsParentIdx = selectedItem - pAboveItemsParentChildrenCount - 1;
						emit setCopiedItem(pAboveItemsParent->getClone());
						addEditorRows(selectedItem);
						pasteEditorCopiedRow(selectedItem + 1);
						deleteEditorRows(QList<int>() << pAboveItemsParentIdx);
						_rowToBeSelected = pAboveItemsParentIdx;
					}
					else
					{
						emit setCopiedItem(pCurrentSelectedCmd->getClone());
						addEditorRows(selectedItem - 2);
						pasteEditorCopiedRow(selectedItem - 1);
						deleteEditorRows(QList<int>() << (selectedItem + 1));
					}
				}
				else
				{
					emit setCopiedItem(pItemAboveSelectedItem->getClone());
					int childrenCount = pCurrentSelectedCmd->getAllChildrenCount();
					addEditorRows(selectedItem + childrenCount);
					rc = pasteEditorCopiedRow(selectedItem + childrenCount + 1);
					if (rc)
					{
						deleteEditorRows(QList<int>() << selectedItem - 1);
					}
				}
			}
		}
	}
	emit clearClipBoardItems();
	emit setCopiedItems(currentLstOfCopiedItem);

	return true;
}

bool GTAEditorViewController::isEditorRowMovableUpwards(int selectedItem)const
{
	bool rc = true;
	if (selectedItem <= 0) //0th row not movable
		return false;

	GTACommandBase* pCurrentSelectedCmd = NULL;
	pCurrentSelectedCmd = getCommand(selectedItem);
	if (pCurrentSelectedCmd == NULL)
		return false;

	if ((getDisplayContext() == GTAElementViewModel::LIVE) && (pCurrentSelectedCmd->getReadOnlyState() == true))
		return false;

	GTACommandBase* pItemAboveSelectedItem = getCommand(selectedItem - 1);

	if (pItemAboveSelectedItem == NULL) {
		return true;
	}
	else if (pCurrentSelectedCmd->getParent() && !pCurrentSelectedCmd->IsUserEditable() && isCommandAboveParentEmpty(selectedItem)) {
			return false;
	}
	else if (!pCurrentSelectedCmd->IsUserEditable()) // if current command is an end statement
	{

		if (pItemAboveSelectedItem->IsUserEditable() == false)
			return false;

		else if (dynamic_cast<GTAActionElse*>(pItemAboveSelectedItem) != NULL)
			return false;

		else if (pItemAboveSelectedItem == pCurrentSelectedCmd->getParent() && pCurrentSelectedCmd->IsUserEditable() == false)
			return false;
	}
	else if (pCurrentSelectedCmd != NULL && pCurrentSelectedCmd->IsUserEditable())
	{
		GTAActionElse* pElse = dynamic_cast<GTAActionElse*>(pCurrentSelectedCmd);


		if (pElse != NULL)
		{

			if (pElse->getParent() == pItemAboveSelectedItem)
				return false;

			else if (pItemAboveSelectedItem->IsUserEditable() == false)
			{

				if (pCurrentSelectedCmd->hasChildren() == true)
					return false;

				else if (dynamic_cast<GTAActionIF*> (pItemAboveSelectedItem->getParent()) == NULL)
					return false;

				else if (pItemAboveSelectedItem->getParent()->hasMutuallExclusiveStatement() == true)
					return false;
			}
		}
		else if ((getDisplayContext() == GTAElementViewModel::LIVE) && (pItemAboveSelectedItem->getReadOnlyState() == true))
		{
			return false;
		}
		else
		{

			if (pCurrentSelectedCmd->hasChildren() && pItemAboveSelectedItem->IsUserEditable() == false)
				return false;
			else
				if (pCurrentSelectedCmd->getParent() == pItemAboveSelectedItem && pCurrentSelectedCmd->canHaveChildren())
					return false;
		}


	}

	return rc;
}

bool GTAEditorViewController::isCommandAboveParentEmpty(int selectedItem) const
{
	//no check for currentCommand viability as we assume it was checked before calling this method
	auto pCurrentSelectedCmd = getCommand(selectedItem);
	auto parent = pCurrentSelectedCmd->getParent();
	auto childrenCount = parent->getAllChildrenCount();
	int rowAboveParent = selectedItem - childrenCount - 1;
	return rowAboveParent < 0;
}

bool GTAEditorViewController::moveEditorRowItemDown(int selectedItem)
{
	bool rc = true;
	if (selectedItem >= _pElementViewModel->rowCount() - 1)
		return false;

	_rowToBeSelected = selectedItem + 1;

	GTACommandBase* pCurrentSelectedCmd = NULL;
	pCurrentSelectedCmd = getCommand(selectedItem);
	if (pCurrentSelectedCmd == NULL)
		return false;	

	QList<GTACommandBase*> currentLstOfCopiedItem;
	emit getCopiedItems(currentLstOfCopiedItem);
	emit clearClipBoardItems();

    GTACommandBase* pItemNextToSelectedItem = getCommand(selectedItem + 1);

	if (selectedItem >= 0)
	{
		if (pItemNextToSelectedItem == NULL)
		{	
			if (selectedItem - 1 >= 0)
			{
				deleteEditorRows(QList<int>() << selectedItem + 1);
				addEditorRows(selectedItem - 1);
			}
			else
			{
				deleteEditorRows(QList<int>() << selectedItem + 1);
				_pElementViewModel->prependRow();
			}
		}
		else
		{
			GTACommandBase* pNextToItemsParent = pItemNextToSelectedItem->getParent();
			pItemNextToSelectedItem = pItemNextToSelectedItem->getClone();
			if (pCurrentSelectedCmd->hasChildren())
			{
				if (pItemNextToSelectedItem != NULL)
				{
					if (!pCurrentSelectedCmd->getParent())
					{

						pItemNextToSelectedItem = pItemNextToSelectedItem->getClone();
						bool isFirstItem = (selectedItem - 1) < 0;
						if (isFirstItem)
						{
							_pElementViewModel->prependRow();
							selectedItem++;
						}
						// We enter here if we have children
						//First we copy and delete the line
						int childrenCount = pCurrentSelectedCmd->getAllChildrenCount();
						int nextLineIndex = selectedItem + childrenCount + 1;
						GTACommandBase* pItemNextToSelectedItemWithChildren = getCommand(nextLineIndex);
						if (pItemNextToSelectedItemWithChildren)
						{
							pItemNextToSelectedItem = pItemNextToSelectedItemWithChildren->getClone();
							emit setCopiedItem(pItemNextToSelectedItem);
							addEditorRows(selectedItem - 1);
							pasteEditorCopiedRow(selectedItem);
							deleteEditorRows({ nextLineIndex + 1 });
						}
						if (isFirstItem)
						{
							deleteEditorRows({ 0 });
						}
					}
					else // Current Has Children 
					{
						pItemNextToSelectedItem = pItemNextToSelectedItem->getClone();
						addEditorRows(selectedItem - 1);
						emit setCopiedItem(pItemNextToSelectedItem);
						pasteEditorCopiedRow(selectedItem);
						deleteEditorRows({ selectedItem + 2 });
					}
				}
				else
				{
					if (selectedItem - 1 >= 0)
					{
						deleteEditorRows(QList<int>() << selectedItem + 1);
						addEditorRows(selectedItem - 1);
					}
					else
					{
						deleteEditorRows(QList<int>() << selectedItem + 1);
						_pElementViewModel->prependRow();

					}
				}

			}
			else // Current has no Children
			{
				if (pItemNextToSelectedItem == NULL)
				{

					if (selectedItem - 1 >= 0)
					{
						deleteEditorRows(QList<int>() << selectedItem + 1);
						addEditorRows(selectedItem - 1);
					}
					else
					{
						deleteEditorRows(QList<int>() << selectedItem + 1);
						_pElementViewModel->prependRow();
					}
				}
				else
				{
					if (!pCurrentSelectedCmd->IsUserEditable())
					{
						if (!pCurrentSelectedCmd->getParent())
						{
							emit setCopiedItem(pItemNextToSelectedItem->getClone());
							addEditorRows(selectedItem - 1);
							pasteEditorCopiedRow(selectedItem);
							deleteEditorRows({ selectedItem + 2 });
						}
						else
						{
							emit setCopiedItem(pItemNextToSelectedItem->getClone());
							auto parent = pCurrentSelectedCmd->getParent();
							auto parentChildrenCount = parent->getAllChildrenCount();
							int itemAboveParentIdx = selectedItem - parentChildrenCount;
							if (itemAboveParentIdx - 1 < 0) {
								_pElementViewModel->prependRow();
								pasteEditorCopiedRow(0);
							}
							else
							{
								addEditorRows(itemAboveParentIdx - 1);
								pasteEditorCopiedRow(itemAboveParentIdx);
							}
							selectedItem++;
							deleteEditorRows({ selectedItem + 1 });

						}

					}
					else // pCurrentSelectedCmd has no Children
					{

						if (pCurrentSelectedCmd->isTitle() && pItemNextToSelectedItem->canHaveChildren())
						{

							auto parentChildrenCount = pItemNextToSelectedItem->getAllChildrenCount();
							int EndItemNextToIdx = selectedItem + parentChildrenCount;

							emit setCopiedItem(pCurrentSelectedCmd->getClone());
							deleteEditorRows(QList<int>() << selectedItem);

							addEditorRows(EndItemNextToIdx);
							pasteEditorCopiedRow(EndItemNextToIdx + 1);


							_rowToBeSelected = EndItemNextToIdx + 1;
						}
						else
						{

							emit setCopiedItem(pCurrentSelectedCmd->getClone());
							deleteEditorRows(QList<int>() << selectedItem);
							addEditorRows(selectedItem);
							pasteEditorCopiedRow(selectedItem + 1);

						}
					}
				}
			}
		}
	}
	emit clearClipBoardItems();
	emit setCopiedItems(currentLstOfCopiedItem);

	return rc;
}
bool GTAEditorViewController::isEditorRowMovableDownwards(int selectedItem)const
{
	bool rc = true;
	if (selectedItem >= _pElementViewModel->rowCount() - 1)
		return false;

	GTACommandBase* pCurrentSelectedCmd = getCommand(selectedItem);;
	if (pCurrentSelectedCmd == nullptr)
		return false;

	if ((getDisplayContext() == GTAElementViewModel::LIVE) && (pCurrentSelectedCmd->getReadOnlyState() == true))
		return false;

	GTACommandBase* pCommandBelowSelItem = getCommand(selectedItem + 1);
	if (pCommandBelowSelItem == nullptr) {
		return true;
	}
	else if (!pCurrentSelectedCmd->IsUserEditable())
	{

		GTACommandBase* pBelowElse = dynamic_cast<GTAActionElse*>(pCommandBelowSelItem);
		GTAActionIfEnd* pCurrentIFEnd = dynamic_cast<GTAActionIfEnd*>(pCurrentSelectedCmd);
		GTAActionIF* pNextIF = dynamic_cast<GTAActionIF*>(pCommandBelowSelItem);
		//GTAActionIF* pCurrentIF = dynamic_cast<GTAActionIF*>(pCurrentSelectedCmd);
		GTAActionWhileEnd* pCurrentWhileEnd = dynamic_cast<GTAActionWhileEnd*>(pCurrentSelectedCmd);
		GTAActionWhile* pNextWhile = dynamic_cast<GTAActionWhile*>(pCommandBelowSelItem);

		if (pCurrentIFEnd != NULL && pNextWhile != NULL)
			return false;
		else if (pCurrentWhileEnd != NULL && pNextIF != NULL)
			return false;

		if (pCommandBelowSelItem->IsUserEditable() == false)
			return false;

		else if (pBelowElse != NULL)
		{
			if (pBelowElse->hasChildren() == true)
				return false;
		}

		else if (pCommandBelowSelItem->hasChildren())
			return false;

	}
	else if (pCurrentSelectedCmd != NULL && pCurrentSelectedCmd->IsUserEditable())
	{
		GTAActionElse* pElse = dynamic_cast<GTAActionElse*>(pCurrentSelectedCmd);

		if (pElse != NULL)
		{
			if (pCommandBelowSelItem->canHaveChildren())
			{
				if (dynamic_cast<GTAActionIF*>(pCommandBelowSelItem) == NULL)
					return false;

				else
				{
					if (pCommandBelowSelItem->hasMutuallExclusiveStatement())
						return false;
				}
			}
			else if (dynamic_cast<GTAActionIfEnd*> (pCommandBelowSelItem) != NULL)
			{

				if (selectedItem + 2 > _pElementViewModel->rowCount() - 1)
					return false;
				else
				{
					GTACommandBase* pNextNextCmd = getCommand(selectedItem + 2);

					if (dynamic_cast<GTAActionIfEnd*> (pNextNextCmd) == NULL)
						return false;
				}
			}

		}
		else if ((getDisplayContext() == GTAElementViewModel::LIVE) && (pCommandBelowSelItem->getReadOnlyState() == true))
		{
			return false;
		}
		else
		{
			GTAActionIF* pNextIF = dynamic_cast<GTAActionIF*>(pCommandBelowSelItem);
			GTAActionIF* pCurrentIF = dynamic_cast<GTAActionIF*>(pCurrentSelectedCmd);
			GTAActionWhile* pNextWhile = dynamic_cast<GTAActionWhile*>(pCommandBelowSelItem);
			GTAActionWhile* pCurrentWhile = dynamic_cast<GTAActionWhile*>(pCurrentSelectedCmd);
			GTAActionElse* pNextElse = dynamic_cast<GTAActionElse*>(pCommandBelowSelItem);
			if (pCurrentIF != NULL && pNextIF != NULL || pCurrentIF != NULL && pNextElse != NULL || pCurrentIF != NULL && pNextWhile != NULL || pCurrentWhile != NULL && pNextIF != NULL)
				return false;

			if (dynamic_cast<GTAActionIfEnd*> (pCommandBelowSelItem) != NULL && pCommandBelowSelItem->getParent() == pCurrentSelectedCmd)
				return false;
			if (dynamic_cast<GTAActionWhileEnd*> (pCommandBelowSelItem) != NULL && pCommandBelowSelItem->getParent() == pCurrentSelectedCmd)
				return false;
			if (!isSpaceBelowLastChild(selectedItem))
			{
				return false;
			}
		}
	}

	return rc;
}

bool GTAEditorViewController::isSpaceBelowLastChild(int selectedItem) const 
{
	auto pCurrentSelectedCmd = getCommand(selectedItem);
	auto childrenCount = pCurrentSelectedCmd->getAllChildrenCount();
	auto nextLineIndex = selectedItem + childrenCount + 1;
	GTACommandBase* pItemNextToSelectedItemWithChildren = getCommand(nextLineIndex);
	if (pItemNextToSelectedItemWithChildren)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void GTAEditorViewController::getRowsContainingTitle(QList<int>& oLstOfRows)
{

	// GTAElement* tempElement = NULL;
	if (_pElement != NULL)
	{
		int allItems = _pElement->getAllChildrenCount();
		for (int i = 0; i < allItems; i++)
		{
			GTACommandBase* pCmd = NULL;
			_pElement->getCommand(i, pCmd);

			if (pCmd != NULL && pCmd->isTitle())
			{
				oLstOfRows.append(i);
			}
		}

	}
	else
		_LastError = "Element not loaded";


}

bool GTAEditorViewController::isDataEmpty()
{
	bool bOk = false;
	if (_pElement)
	{
		bOk = _pElement->isEmpty();
	}

	return bOk;
}

bool GTAEditorViewController::addheaderField(const QString& iName, const QString& iVal, const QString& iMandatory, const QString& iDescription)
{

	bool rc = false;
	if (_pHeaderModel)
	{
		int row = _pHeader->itemSize();
		bool bMandatory = true;

		if (iMandatory.contains("No"))
			bMandatory = false;

		rc = _pHeaderModel->insertRows(row, 1, QModelIndex());
		if (rc)
		{
			QModelIndex index = _pHeaderModel->index(row, 0);
			headerNodeInfo hdrInfo;
			hdrInfo._name = iName;
			hdrInfo._description = iDescription;
			hdrInfo._val = iVal;
			hdrInfo._Mandatory = bMandatory;
			hdrInfo._showInLTRA = true;

			QVariant setVarData;
			setVarData.setValue(hdrInfo);
			rc = _pHeaderModel->setData(index, setVarData);
			if (!rc)
				_LastError = "set data failed";
		}
		else
			_LastError = _pHeader->getLastError();
	}
	else
		_LastError = "Model does not exist";

	return rc;

}

bool GTAEditorViewController::isHeaderRowEditable(const int& iRow)
{
	bool rC = true;

	GTAHeader* pHeader = getHeader();
	if (NULL == pHeader)
	{
		GTAElement* pElement = getElement();
		if (pElement != NULL)
		{
			pHeader = pElement->getHeader();
		}

	}
	if (NULL != pHeader)
	{

		if (iRow >= pHeader->getFieldNodeSize())
		{
			rC = false;
			_LastError = "ICD parameter cannot be edited";
		}

	}
	else
	{
		rC = false;
		_LastError = "No Header found";
	}

	return rC;
}


bool GTAEditorViewController::isHeaderRowDeletable(const int& iRow)
{
	bool rC = true;

	GTAHeader* pHeader = getHeader();
	if (NULL == pHeader)
	{
		GTAElement* pElement = getElement();
		if (pElement != NULL)
		{
			pHeader = pElement->getHeader();
		}

	}
	if (NULL != pHeader)
	{

		if (iRow >= pHeader->getFieldNodeSize())
		{
			rC = false;
			_LastError = "ICD parameter cannot be edited/deleted";
		}
		else
		{
			QString sFieldName = pHeader->getFieldName(iRow);
			if (pHeader->isNodeMandatory(sFieldName))
			{
				rC = false;
				_LastError = "Header field is Mandatory cannot be delted";
			}
		}

	}
	else
	{
		rC = false;
		_LastError = "No Header found";
	}

	return rC;
}

bool GTAEditorViewController::getHeaderFieldInfo(const int& iRow, QString& iFieldName, QString& iFieldVal, QString& iMandatoryInfo, QString& iFieldDescription, bool& iShowInLTRA)
{
	bool rC = true;

	GTAHeader* pHeader = getHeader();
	if (NULL == pHeader)
	{
		GTAElement* pElement = getElement();
		if (pElement != NULL)
		{
			pHeader = pElement->getHeader();
		}

	}
	if (NULL != pHeader)
	{

		if (iRow >= pHeader->getFieldNodeSize())
		{
			rC = false;
			_LastError = "ICD parameter cannot be edited/deleted";
		}
		else
		{
			iFieldName = pHeader->getFieldName(iRow);
			iFieldVal = pHeader->getFieldValue(iRow);
			iFieldDescription = pHeader->getFieldDescription(iRow);
			iMandatoryInfo = "No";
			if (pHeader->isNodeMandatory(iFieldName))
				iMandatoryInfo = "Yes";
			iShowInLTRA = pHeader->getShowInLTRA(iRow);
		}

	}
	else
	{
		rC = false;
		_LastError = "No Header found";
	}

	return rC;
}

/**
 * @brief Checks If the selected header Field is mandatory field
 * @param iRow: Index for the selected field in header dialog
 * @param isMandatory: Gets the value of the Mandatory field related to the field
 * @return bool
*/
bool GTAEditorViewController::isHeaderFieldMandatory(const int& iRow, bool& isMandatory)
{
	bool rC = true;

	isMandatory = false;

	GTAHeader* pHeader = getHeader();
	if (NULL == pHeader)
	{
		GTAElement* pElement = getElement();
		if (pElement != NULL)
		{
			pHeader = pElement->getHeader();
		}

	}
	if (NULL != pHeader)
	{

		if (iRow >= pHeader->getFieldNodeSize())
		{
			rC = false;
			_LastError = "ICD parameter cannot be edited/deleted";
		}
		else
		{
			QString fieldName = pHeader->getFieldName(iRow);
			isMandatory = pHeader->isNodeMandatory(fieldName);
		}
	}
	else
	{
		rC = false;
		_LastError = "No Header found";
	}

	return rC;
}

/**
 * @brief Checks If the selected header Field is default mandatory field required to print LTRA
 * @param iRow: Index for the selected field in header dialog
 * @return bool : true if field is default mandatory
*/
bool GTAEditorViewController::isDefaultMandatoryHeaderField(const int& iRow)
{
	bool isDefaultMandatoryHeaderField = false;
	GTAHeader* pHeader = getHeader();
	if (NULL == pHeader)
	{
		GTAElement* pElement = getElement();
		if (pElement != NULL)
		{
			pHeader = pElement->getHeader();
		}
	}

	if (NULL != pHeader)
	{
		QString fieldName = pHeader->getFieldName(iRow);
		isDefaultMandatoryHeaderField = pHeader->isDefaultMandatoryField(fieldName);
	}

	return isDefaultMandatoryHeaderField;
}

bool GTAEditorViewController::editHeaderField(const int& iRow, const QString& iName, const QString& iVal, bool iMandatory, const QString& iDescription, const bool& iShowLTRA)
{

	bool rc = false;
	if (_pHeaderModel)
	{
		// int row = _pHeaderModel->rowCount(QModelIndex());
		// bool bMandatory = true;
		QModelIndex index = _pHeaderModel->index(iRow, 0);
		headerNodeInfo hdrInfo;
		hdrInfo._name = iName;
		hdrInfo._description = iDescription;
		hdrInfo._val = iVal;
		hdrInfo._Mandatory = iMandatory;
		hdrInfo._showInLTRA = iShowLTRA;

		if (iMandatory)
		{
			hdrInfo._showInLTRA = true;
		}

		QVariant setVarData;
		setVarData.setValue(hdrInfo);
		rc = _pHeaderModel->setData(index, setVarData);

		if (!rc)
			_LastError = _pHeader->getLastError();
	}
	else
		_LastError = "Model does not exist";

	return rc;

}

bool GTAEditorViewController::removeHeaderItem(const int& iRow)
{
	bool rc = false;
	if (iRow)
	{
		rc = _pHeaderModel->removeRows(iRow, 0, QModelIndex());
	}
	else
		_LastError = "Header name is non editable";

	return rc;

}

bool GTAEditorViewController::moveHeaderRowItemUp(const int& iRow)
{
	bool rc = false;
	if (_pHeader != NULL)
	{

		if (iRow > 1)
		{
			QString currentRowTagName = _pHeader->getFieldName(iRow);
			QString currentRowTagVal = _pHeader->getFieldValue(iRow);
			QString currentRowTagDes = _pHeader->getFieldDescription(iRow);
			bool    currentRowTagMan = _pHeader->isNodeMandatory(currentRowTagName);

			headerNodeInfo currentNodeInfo;
			currentNodeInfo._description = currentRowTagDes;
			currentNodeInfo._Mandatory = currentRowTagMan;
			currentNodeInfo._name = currentRowTagName;
			currentNodeInfo._val = currentRowTagVal;


			QString previousRowTagName = _pHeader->getFieldName(iRow - 1);
			QString previousRowTagVal = _pHeader->getFieldValue(iRow - 1);
			QString previousRowTagDes = _pHeader->getFieldDescription(iRow - 1);
			bool    previousRowTagMan = _pHeader->isNodeMandatory(previousRowTagName);


			headerNodeInfo previousNodeInfo;
			previousNodeInfo._description = previousRowTagDes;
			previousNodeInfo._Mandatory = previousRowTagMan;
			previousNodeInfo._name = previousRowTagName;
			previousNodeInfo._val = previousRowTagVal;

			QModelIndex currentRowIndex = _pHeaderModel->index(iRow, 0);


			headerNodeInfo emptyInfo;
			emptyInfo._description = "";
			emptyInfo._Mandatory = false;
			emptyInfo._name = "";
			emptyInfo._val = "";

			rc = _pHeaderModel->insertRows(iRow - 1, 1, QModelIndex());
			if (rc)
			{
				rc = _pHeaderModel->removeRows(iRow + 1, 0, QModelIndex());
				if (rc)
				{
					QModelIndex prevRowIndex = _pHeaderModel->index(iRow - 1, 0);
					QVariant currentValVar;
					currentValVar.setValue(currentNodeInfo);
					rc = _pHeaderModel->setData(prevRowIndex, currentValVar);
				}
			}

		}

	}
	return rc;
}

bool GTAEditorViewController::moveHeaderRowItemDown(const int& iRow)
{
	bool rc = false;
	if (_pHeader != NULL)
	{

		if (iRow > 0 && iRow < _pHeader->itemSize() - 1)
		{
			QString currentRowTagName = _pHeader->getFieldName(iRow);
			QString currentRowTagVal = _pHeader->getFieldValue(iRow);
			QString currentRowTagDes = _pHeader->getFieldDescription(iRow);
			bool    currentRowTagMan = _pHeader->isNodeMandatory(currentRowTagName);

			headerNodeInfo currentNodeInfo;
			currentNodeInfo._description = currentRowTagDes;
			currentNodeInfo._Mandatory = currentRowTagMan;
			currentNodeInfo._name = currentRowTagName;
			currentNodeInfo._val = currentRowTagVal;


			QString nextRowTagName = _pHeader->getFieldName(iRow + 1);
			QString nextRowTagVal = _pHeader->getFieldValue(iRow + 1);
			QString nextRowTagDes = _pHeader->getFieldDescription(iRow + 1);
			bool    nextRowTagMan = _pHeader->isNodeMandatory(nextRowTagName);


			headerNodeInfo nextNodeInfo;
			nextNodeInfo._description = nextRowTagDes;
			nextNodeInfo._Mandatory = nextRowTagMan;
			nextNodeInfo._name = nextRowTagName;
			nextNodeInfo._val = nextRowTagVal;

			QModelIndex currentRowIndex = _pHeaderModel->index(iRow, 0);


			headerNodeInfo emptyInfo;
			emptyInfo._description = "";
			emptyInfo._Mandatory = false;
			emptyInfo._name = "";
			emptyInfo._val = "";

			rc = _pHeaderModel->insertRows(iRow, 1, QModelIndex());
			if (rc)
			{
				rc = _pHeaderModel->removeRows(iRow + 2, 0, QModelIndex());

				if (rc)
				{
					QModelIndex prevRowIndex = _pHeaderModel->index(iRow, 0);
					QVariant currentValVar;
					currentValVar.setValue(nextNodeInfo);
					rc = _pHeaderModel->setData(prevRowIndex, currentValVar);

				}
			}

		}

	}
	return rc;
}


QString GTAEditorViewController::getHeaderName()const
{
	if (_pHeader)
		return _pHeader->getName();
	else
		return QString();
}
bool GTAEditorViewController::headerComplete(QStringList& oEmptyManField)const
{
	bool rC = false;
	if (_pHeader)
		rC = _pHeader->mandatoryAttributeFilled(oEmptyManField);

	return rC;
}


bool GTAEditorViewController::getHeaderFromDocument(const QString& iFileName, GTAHeader*& opHeader)
{
	bool rc = false;

	GTADocumentController docController;
	GTAElement* pElement = NULL;
	GTAHeader* pHeader = NULL;
	QString sDataDirectory;
	emit getGTADataDirectory(sDataDirectory);
	if (!sDataDirectory.isEmpty())
	{
		QString completeFilePath = QDir::cleanPath(QString("%1/%2").arg(sDataDirectory, iFileName));
		QFileInfo info(completeFilePath);
		if (!info.exists())
			return false;
		rc = docController.openDocument(completeFilePath, pElement, pHeader, false);
		if (!rc)
		{
			_LastError = docController.getLastError();
		}
		else
		{
			if (pHeader)
			{
				opHeader = pHeader;
				rc = true;
			}
			if (NULL != pElement)
			{
				delete pElement; pElement = NULL;
			}

		}
	}
	return rc;
}
bool GTAEditorViewController::createHeaderModelFromDoc(const QString& iFileName, QAbstractItemModel*& opEditorModel)
{
	bool rC = false;
	GTAHeader* pHeader = NULL;
	rC = getHeaderFromDocument(iFileName, pHeader);
	if (rC && pHeader != NULL)
	{
		rC = createHeaderModel(pHeader, opEditorModel);
	}
	return rC;

}

bool GTAEditorViewController::createHeaderModel(GTAHeader* ipheader, QAbstractItemModel*& iopModel)
{

	bool rC = false;
	if (ipheader != NULL)
	{
		if (NULL != _pHeader)
		{
			delete _pHeader;
			_pHeader = NULL;
			delete _pHeaderModel;
			_pHeaderModel = NULL;
		}
		_pHeader = ipheader;
		_pHeaderModel = new GTAHeaderTableModel(ipheader);
		if (NULL != _pElement)
			_pElement->setHeader(ipheader);

		iopModel = _pHeaderModel;
		rC = false;
	}
	else
		_LastError = "Header data missing";

	return rC;

}

bool GTAEditorViewController::deleteHeader(const QString& iName)
{
	bool rc = false;
	QString sDataDirectory;

	emit getGTADataDirectory(sDataDirectory);

	if (!sDataDirectory.isEmpty())
	{
		GTAHeader* pHeader = getHeader();

		if (pHeader != NULL)
		{

			QString HeaderName = iName + ".hdr";
			QString completePath = sDataDirectory + "/" + HeaderName;
			QString headerFilePath = QDir::cleanPath(completePath);
			QFileInfo fi(headerFilePath);

			if (fi.exists() && fi.isFile())
			{
				QDir Dir(sDataDirectory);
				rc = Dir.remove(HeaderName);
			}

			QDir Dir(headerFilePath);
			rc = Dir.removeRecursively();
		}
	}
	return rc;
}

bool GTAEditorViewController::saveHeader(const QString& iName)
{
	bool rC = false;
	QString sDataDirectory;
	emit getGTADataDirectory(sDataDirectory);
	if (!sDataDirectory.isEmpty())
	{

		QString completePath = sDataDirectory + "\\" + iName + ".hdr";
		QString headerFilePath = QDir::cleanPath(completePath);

		GTAHeader* pHeader = getHeader();
		if (pHeader != NULL)
		{
			GTADocumentController DocCtrl;
			rC = DocCtrl.saveDocument(pHeader, headerFilePath, true);
			if (rC)
				pHeader->setEditMode(false);
		}

	}
	return rC;

}


bool GTAEditorViewController::applyChangesHeader(const QString& Name, GTAHeader*& pHeader)
{
	bool rC = false;
	QString headerFileName = QString("%1.hdr").arg(Name);
	rC = getHeaderFromDocument(headerFileName, pHeader);
	return rC;
}

bool GTAEditorViewController::showICDDetails(const bool& iShow, QList <GTAFILEInfo> lstFileInfo)
{
	bool rc = false;

	if (!lstFileInfo.isEmpty())
	{
		if (iShow)
			_pHeaderModel->setFileInfo(lstFileInfo);
		else
			_pHeaderModel->removeFileInfo();

		rc = true;
	}
	else
		_LastError = "No information on ICDs found";
	return rc;

}

void GTAEditorViewController::setGTADirPath(const QString& iPath)
{
	_GtaDataDirPath = iPath;
}

void GTAEditorViewController::resetDisplayContext()
{
	if (_pElement != NULL)
	{
		_pElementViewModel->setDisplayContext(GTAElementViewModel::EDITING);
		_lastHighligtedRow = -1;
	}
}

GTAElementViewModel::DisplayContext GTAEditorViewController::getDisplayContext()const
{
	if (_pElement != NULL && _pElementViewModel != NULL)
	{
		return _pElementViewModel->getDisplayContext();
	}
	return GTAElementViewModel::EDITING;
}

void GTAEditorViewController::setDisplayContext(GTAElementViewModel::DisplayContext context)
{
	if (_pElement != NULL && _pElementViewModel != NULL)
	{
		_pElementViewModel->setDisplayContext(context);
	}
}

void GTAEditorViewController::setDisplaycontextToCompatibilityCheck()
{
	if (_pElement != NULL)
	{
		_pElementViewModel->setDisplayContext(GTAElementViewModel::COMPATIBILITY_CHK);
	}
}

void GTAEditorViewController::highlightRow(const int& iRowId, QModelIndex& oHighlightedRow, GTAElementViewModel::DisplayContext iDisplayContext)
{
	if (_pElement != NULL && _pElementViewModel != NULL)
	{
		_pElementViewModel->highlightRow(iRowId, oHighlightedRow, iDisplayContext);

	}
}

bool GTAEditorViewController::searchHighlight(const QRegExp& searchString, QList<int>& iSelectedRows, bool ibSearchUp, bool ibTextSearch, QModelIndex& oHighlightedRowIdx, bool isSingleSearch, bool isHighlight, QHash<int, QString>& oFoundRows)
{
	bool rc = false;

	if (_pElement != NULL)
	{
		_pElementViewModel->setDisplayContext(GTAElementViewModel::SEARCH_REPLACE);
		QHash<int, QString> foundRows;
		_pElementViewModel->findAndHighlight(searchString, iSelectedRows, foundRows, ibSearchUp, ibTextSearch, isSingleSearch, isHighlight);
		oFoundRows = foundRows;
		if (!foundRows.isEmpty())
		{
			QList<int> keys = foundRows.keys();
			_lastHighligtedRow = keys.last(); //foundRows.takeLast();
			oHighlightedRowIdx = _pElementViewModel->index(_lastHighligtedRow, 0);
			rc = true;
		}
		else
			_lastHighligtedRow = -1;
	}
	return rc;
}



bool GTAEditorViewController::hightlightReplace(const QRegExp& searchString, const QString& iReplaceString, QList<int>& iSelectedRows, bool ibSearchUp, QModelIndex& oHighlightedRowIdx, QList<int>&)
{
	bool rc = false;
	QRegExp regExpSearch(searchString);
	if (_pElement != NULL)
	{
		if (_lastHighligtedRow == -1)
			rc = searchHighlight(regExpSearch, iSelectedRows, ibSearchUp, false, oHighlightedRowIdx);
		else
		{
			rc = _pElementViewModel->replaceString(_lastHighligtedRow, searchString, iReplaceString, ibSearchUp);
			iSelectedRows.clear();
			iSelectedRows.append(_lastHighligtedRow + 1);
			searchHighlight(regExpSearch, iSelectedRows, ibSearchUp, false, oHighlightedRowIdx);

		}

	}
	return rc;
}


bool GTAEditorViewController::replaceAllString(const QRegExp& searchString, const QString& iReplaceString)
{
	bool rc = false;
	if (_pElement != NULL && _pElementViewModel != NULL)
	{
		_pElementViewModel->setDisplayContext(GTAElementViewModel::SEARCH_REPLACE);
		_lastHighligtedRow = -1;
		rc = _pElementViewModel->replaceAllString(searchString, iReplaceString);
	}
	return rc;
}

bool GTAEditorViewController::replaceAllEquipment(const QRegExp& searchString, const QString& iReplaceString)
{
	bool rc = false;
	if (_pElement != NULL && _pElementViewModel != NULL)
	{
		_pElementViewModel->setDisplayContext(GTAElementViewModel::SEARCH_REPLACE);
		_lastHighligtedRow = -1;
		rc = _pElementViewModel->replaceAllEquipment(searchString, iReplaceString);
	}
	return rc;
}

QString GTAEditorViewController::getUUID()const
{
	QString uuid;
	if (_pElement != NULL)
	{
		uuid = _pElement->getUuid();
	}
	return uuid;
}
