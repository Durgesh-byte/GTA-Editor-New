#include "GTAViewController.h"
#include "GTALocalDbUtil.h"
#include "GTAElementViewModel.h"
#include "GTAHeaderTableModel.h"
#include "GTAAppController.h"
#include "GTAHeader.h"
#include "GTAActionIfEnd.h"
#include "GTAActionIF.h"
#include "GTAComment.h"
#include "GTAActionWhile.h"
#include "GTAActionWhileEnd.h"
#include "GTAActionDoWhile.h"
#include "GTAActionDoWhileEnd.h"
#include "GTAActionElse.h"
#include "GTACommandVariant.h"
#include "GTAEditorTreeModel.h"
#include "GTAActionCall.h"
#include "GTAActionExpandedCall.h"
#include "GTADocumentController.h"
#include "GTAFileSystemModel.h"
#include "GTAAppController.h"
#include "GTAElement.h"
#include "GTAMessage.h"
#include "GTAHeader.h"
#include "GTATreeItem.h"
#include "GTAActionBase.h"
#include "GTAGenericToolCommand.h"

#pragma warning(push, 0)
#include <QHeaderView>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QStack>
#include <QTreeView.h>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QHash>
#include <QSortFilterProxyModel>
#pragma warning(pop)





GTAViewController* GTAViewController::_pViewController = 0;

GTAViewController::GTAViewController(QObject* ipObject) :QObject(ipObject)
{

	_pHeaderModel = NULL;
	_pElement = NULL;
	_pElementViewModel = NULL;
	_pHeader = NULL;
	_lastHighligtedRow = -1;
	_pFavDataModel = NULL;
	_pElemBrowserModel = NULL;
	_pTreeViewModel = NULL;
	_pProxyModel = NULL;
	_pFavTreeRoot = NULL;
	_previousInsertRowIndex = -1;

}
GTAViewController* GTAViewController::getGTAViewController(QObject* ipObject)
{
	if (_pViewController == 0)
		_pViewController = new GTAViewController(ipObject);
	return _pViewController;
}
GTAViewController::~GTAViewController()
{
	//   //DEAD CODE clearEditorRowCopy();
	//  //DEAD CODE  deleteAllModel();
	//    if(_pHeaderModel)
	//    {delete _pHeaderModel;_pHeaderModel = NULL;}

	if (_pTreeViewModel != NULL)
	{
		delete _pTreeViewModel;
		_pTreeViewModel = NULL;
	}
	if (_pFavDataModel != NULL)
	{
		delete _pFavDataModel;
		_pFavDataModel = NULL;
	}

	if (_pProxyModel != NULL)
	{
		delete _pProxyModel;
		_pProxyModel = NULL;
	}
	if (_pFavTreeRoot != NULL)
	{
		delete _pFavTreeRoot;
		_pFavTreeRoot = NULL;
	}
}
bool GTAViewController::setEditorModel(QAbstractTableModel* ipModel)
{
	bool rc = false;
	if (ipModel)
	{
		rc = true;
		_pElementViewModel = dynamic_cast<GTAElementViewModel*> (ipModel);
	}
	return rc;
}
bool GTAViewController::setInitialEditorView(QTableView* ipView)
{
	bool rC = false;
	if (ipView)
	{
		_pElement = new GTAElement(GTAElement::PROCEDURE, 100); //Creating with 30 rows by default
		if (_pHeader)
		{
			_pElement->setHeader(_pHeader);
		}
		_pElementViewModel = new GTAElementViewModel(_pElement, this);
		establishConnectionWithModel();
		ipView->setModel(_pElementViewModel);
		ipView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
		ipView->horizontalHeader()->setStretchLastSection(true);
		ipView->setDragEnabled(true);
		ipView->setAcceptDrops(true);
		ipView->setDragDropMode(QAbstractItemView::DragDrop);
		ipView->viewport()->setAcceptDrops(true);
		ipView->setDropIndicatorShown(true);

		//
		//{
		//	int numberOfCols = _pElementViewModel->columnCount();
		//
		//	int totalWidth = ipView->horizontalHeader()->geometry().width();
		//	int dHeight = ipView->horizontalHeader()->geometry().height();
		//	QSize sizeHeader = ipView->horizontalHeader()->sizeHint();
		//
		//	int totalSizeInPixels = dHeight*totalWidth;
		//	ipView->horizontalHeader()->resizeSection(0,totalSizeInPixels);//,dHeight.height());
		//	//ipView->horizontalHeader()->resizeSection(1,0.12*0.12*totalSizeInPixels);//,dHeight.height());
		//	//ipView->horizontalHeader()->resizeSection(2,0.2*0.2*totalSizeInPixels);//,dHeight.height());
		//	//ipView->horizontalHeader()->resizeSection(3,0.12*0.12*totalSizeInPixels);//,dHeight.height());
		//	//ipView->horizontalHeader()->resizeSection(4,0.12*0.12*totalSizeInPixels);//,dHeight.height());
		//	//ipView->horizontalHeader()->resizeSection(5,0.12*0.12*totalSizeInPixels);//,dHeight.height());
		//}
		//


		ipView->setSelectionMode(QAbstractItemView::ExtendedSelection);

		ipView->show();
		rC = true;

		_pEditorView = ipView;
	}
	return rC;

}
bool GTAViewController::setHeaderView(QTableView* ipView, GTAHeader*& ipHeader)
{
	GTADocumentController docController;
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

bool GTAViewController::headerEdited()const
{
	return _pHeader->inEditMode();
}
QString GTAViewController::getHeaderName()const
{
	if (_pHeader)
		return _pHeader->getName();
	else
		return QString();
}
bool GTAViewController::headerComplete(QStringList& oEmptyManField)const
{
	bool rC = false;
	if (_pHeader)
		rC = _pHeader->mandatoryAttributeFilled(oEmptyManField);

	return rC;
}

//one by one addition of rows changes view model and model.
void GTAViewController::addEditorRows(const int& iSelectedRow)
{
	if (_pElementViewModel)
		_pElementViewModel->insertRows(iSelectedRow, 0);
}
void GTAViewController::deleteEditorRows(const QList<int>& lstSelectedRows)
{
    bool rC=false;
    if (!lstSelectedRows.isEmpty())
    {
        // int startRow = lstSelectedRows[0];
        // int totalRowCount = lstSelectedRows.count();//lstSelectedRows.at(lstSelectedRows.size()-1) -startRow +1;

        QList<int> selectedRows = lstSelectedRows;
        qSort(selectedRows.begin(),selectedRows.end(),qGreater<int>());
        foreach(int row,selectedRows)
        {
            if (_pElementViewModel)
            {
                rC = _pElementViewModel->removeRows(row,1);
            }
        }

    }
}
void GTAViewController::clearAllinEditor()
{
	if (_pElement)
	{
		_pElementViewModel->clearAllinEditor();
	}


}


bool GTAViewController::editAction(const int& iRow, const QVariant& incomingData)
{
	/*int indexToBeRemoved = iRow;*/
	bool rC = false;

	QModelIndex index = _pElementViewModel->index(iRow, 0);
	rC = _pElementViewModel->setData(index, incomingData);
	if (!rC)
		_LastError = _pElementViewModel->getLastError();

	return rC;


}

GTACommandBase* GTAViewController::getCommand(const int& iSelRow)const
{
	GTACommandBase* oCmd = NULL;
	if (iSelRow < _pElement->getAllChildrenCount())
	{
		_pElement->getCommand(iSelRow, oCmd);

	}
	return oCmd;

}
void GTAViewController::updateEditorView()const
{
	_pElementViewModel->updateChanges();
}

//may not need this
QAbstractItemModel* GTAViewController::buildEditorModels(const EditorType& ieType)
{
	deleteAllModel();
	//All model by default has one element at least.
	if (_pElement)
	{
		delete _pElement;
	}

	int noOfDefaultchildren = 100;
	switch (ieType)
	{
	case Object:
		_pElement = new GTAElement(GTAElement::OBJECT, noOfDefaultchildren);


		break;
	case Function:
		_pElement = new GTAElement(GTAElement::FUNCTION, noOfDefaultchildren);


		break;
	case Procedure:
		_pElement = new GTAElement(GTAElement::PROCEDURE, noOfDefaultchildren);


		break;
	case Sequence:
		_pElement = new GTAElement(GTAElement::SEQUENCE, noOfDefaultchildren);

		break;
	case Template:
		_pElement = new GTAElement(GTAElement::TEMPLATE, noOfDefaultchildren);


		break;
	default:
		break;
	};

	if (_pElement)
	{
		_pElement->setHeader(_pHeader);
		_pElementViewModel = new GTAElementViewModel(_pElement);
	}
	//if (_pElement)
	//{
	//
	//	//setEditorModel(_pElementViewModel);
	//}

	establishConnectionWithModel();
	return _pElementViewModel;
}

bool GTAViewController::createHeaderModel(GTAHeader* ipheader, QAbstractItemModel*& iopModel)
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
bool GTAViewController::setHeader(GTAHeader* ipheader)
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
bool GTAViewController::createSetHeaderModel(QAbstractItemModel*& iopModel)
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
bool GTAViewController::createSetElementModel(QAbstractItemModel*& iopModel)
{
	bool rc = false;
	if (_pElement)
	{
		if (_pElementViewModel != NULL)
		{
			delete _pElementViewModel; _pElementViewModel = NULL;
		}

		_pElementViewModel = new GTAElementViewModel(_pElement);
		iopModel = _pElementViewModel;
		rc = true;
	}
	else
	{
		iopModel = NULL;
		_LastError = "Empty header";
	}
	establishConnectionWithModel();
	return rc;

}
void GTAViewController::deleteAllModel()
{
	//if (NULL!=_pHeaderModel)
	//{
	//	delete _pHeaderModel;
	//	_pHeaderModel=NULL;
	//}
	if (NULL != _pElementViewModel)
	{
		delete _pElementViewModel;
		_pElementViewModel = NULL;

	}
	/*if (!_pFavDataModel==NULL))
	{
	delete _pFavDataModel;
	_pFavDataModel = NULL;
	}*/


}
bool GTAViewController::isSelectedCommandsBalanced(QList<int>& ioLstSelectedRows)
{
	bool rc = true;
	QStringList _errMsg;
	if (!ioLstSelectedRows.isEmpty())
	{
		QStack<GTACommandBase*> balanceIfStack;
		QStack<GTACommandBase*> balanceWhileStack;
		QStack<GTACommandBase*> balanceDoWhileStack;
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

				GTAActionDoWhile *pDoWhile = dynamic_cast<GTAActionDoWhile*>(pCmd);
				GTAActionDoWhileEnd *pDoWhileEnd = dynamic_cast<GTAActionDoWhileEnd*>(pCmd);

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

			}

		}
	}

	if (!rc)
	{
		//   QMessageBox::warning(this,"Warning","No selection Nothing copied");
		//QMessageBox::critical(this,"Row Selection Error","End statement cannot be deleted exclusively",QMessageBox::Ok,QMessageBox::Close);
	}
	return rc;

}
bool GTAViewController::areSelectedCommandsNested(const QList<int>& iLstSelectedRows)
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

void GTAViewController::undoClearRow(const QList<int> &iIndexList, QList<GTACommandBase*> &iCmdList)
{
    if(_pElementViewModel)
    {
        for(int i = 0; i < iIndexList.count(); i++)
        {
            int iIndex = iIndexList.at(i);
            GTACommandBase *pCmd = iCmdList.at(i);
            int totalchildren = pCmd->getAllChildrenCount();
            GTACommandVariant cmdVariant;
            cmdVariant.setCommand(pCmd);
            QVariant variant;
            variant.setValue(cmdVariant);
            QModelIndex index = _pElementViewModel->index(iIndex,0);

            _pElementViewModel->setData(index, variant);

            QList<int> lstIndex;

            GTACommandBase* pNextCommand = NULL;
            _pElement->getCommand(iIndex + totalchildren + 1,pNextCommand);

            if(pNextCommand == NULL )
                lstIndex.append(iIndex + totalchildren + 1);

            for(int j = 1; j <= totalchildren; j++)
            {
                GTACommandBase* pSubNextCommand = NULL;
                _pElement->getCommand(iIndex + totalchildren + 1 + j, pSubNextCommand);
                if(pSubNextCommand == NULL)
                    lstIndex.append(iIndex + totalchildren + 1 + j);
            }

            if((iIndex>0) ||  (totalchildren>0))
                deleteEditorRows(lstIndex);

        }
    }
}

bool GTAViewController::pasteEditorCopiedRow(const int& iPasteAtRow)
{
	bool rC = true;

	GTACommandBase* pCurrentCommand = NULL;
	_pElement->getCommand(iPasteAtRow, pCurrentCommand);
	if (pCurrentCommand)
	{
		_LastError = "Paste allowed on empty rows only.";
		return rC;
	}

	if (!_lstCopiedElements.isEmpty())
	{
		int sizeOfCopiedItems = _lstCopiedElements.size();
		if (sizeOfCopiedItems)
		{
			int rowToBePasted = iPasteAtRow;
			for (int i = 0; i<sizeOfCopiedItems; i++)
			{
				bool bAddNewRow = true;
				GTACommandBase* pCurrentCopiedItem = _lstCopiedElements.at(i);
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

					GTACommandBase* nxtCmd = NULL;
					rC = _pElement->getCommand(rowToBePasted, nxtCmd);
					if (!rC && sizeOfCopiedItems - 1 != i)
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

bool GTAViewController::pasteEditorCopiedRow(const int& iPasteAtRow, const QList<GTACommandBase*> CmdsPasted, QString lastCalled)
{
	bool rC = true;

	GTACommandBase* pCurrentCommand = NULL;
	_pElement->getCommand(iPasteAtRow, pCurrentCommand);
	if (pCurrentCommand)
	{
		_LastError = "Paste allowed on empty rows only.";
		rC = false;
		return rC;
	}

	if (!CmdsPasted.isEmpty())
	{
		int sizeOfCopiedItems = CmdsPasted.size();
		if (sizeOfCopiedItems)
		{
			int rowToBePasted = iPasteAtRow;
			for (int i = 0; i<sizeOfCopiedItems; i++)
			{
				bool bAddNewRow = true;
				GTACommandBase* pCurrentCopiedItem = CmdsPasted.at(i);
				if (pCurrentCopiedItem)
				{
					GTACommandVariant cmdVariant;
					cmdVariant.setCommand(pCurrentCopiedItem);
					GTAGenericToolCommand *pGenCmd = dynamic_cast<GTAGenericToolCommand *>(pCurrentCopiedItem);
					if (lastCalled == "copy" && pGenCmd != NULL)
					{
						pGenCmd->setObjId();
						pCurrentCopiedItem = pGenCmd;
					}
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

					GTACommandBase* nxtCmd = NULL;
					rC = _pElement->getCommand(rowToBePasted, nxtCmd);
					if (!rC && sizeOfCopiedItems - 1 != i)
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


void GTAViewController::clearEditorRowCopy()
{
	foreach(GTACommandBase* pCmd, _lstCopiedElements)
	{
		if (pCmd != NULL)
		{
			delete pCmd;
			pCmd = NULL;
		}
	}
	_lstCopiedElements.clear();
	//_lstCopiedComments.clear();
}
bool GTAViewController::setElement(const GTAElement* isElement, EditorType isType)
{
	bool rc = false;
	if (NULL != isElement)
	{
		if (_pElement)
		{
			delete _pElement; _pElement = NULL;
		}

		_pElement = (GTAElement*)isElement;
		rc = true;
		switch (isType)
		{
		case Object:
			_pElement->setElementType(GTAElement::OBJECT);

			break;
		case Function:
			_pElement->setElementType(GTAElement::FUNCTION);


			break;
		case Procedure:
			_pElement->setElementType(GTAElement::PROCEDURE);
			break;

		case Sequence:
			_pElement->setElementType(GTAElement::SEQUENCE);
			break;
		case Template:
			_pElement->setElementType(GTAElement::TEMPLATE);
			break;
		default:
			rc = false;
			break;
		}
	}
	else
	{
		_LastError = "empty element cannot be set";
	}
	if (!rc)
		_LastError = "element type cannot does not match";

	return rc;
}
QAbstractItemModel* GTAViewController::buildEditorModels(const GTAElement* isElement, EditorType isType)
{
	if (isElement)
	{
		if (_pElement)
		{
			delete _pElement; _pElement = NULL;
		}

		_pElement = (GTAElement*)isElement;
		switch (isType)
		{
		case Object:
			_pElement->setElementType(GTAElement::OBJECT);

			break;
		case Function:
			_pElement->setElementType(GTAElement::FUNCTION);


			break;
		case Procedure:
			_pElement->setElementType(GTAElement::PROCEDURE);
			break;

		case Sequence:
			_pElement->setElementType(GTAElement::SEQUENCE);
			break;
		case Template:
			_pElement->setElementType(GTAElement::TEMPLATE);
			break;
		default:
			break;
		};



		if (_pElementViewModel != NULL)
		{
			delete _pElementViewModel; _pElementViewModel = NULL;
		}


		_pElementViewModel = new GTAElementViewModel(_pElement);
		_pElement->setHeader(_pHeader);
		establishConnectionWithModel();
		return _pElementViewModel;
	}
	else
		return NULL;

}

QList<GTACommandBase*> GTAViewController::getCommandsInClipboard()const
{
	return _lstCopiedElements;
}

bool GTAViewController::setCopiedItems(QList<int>& ioLstSelectedRows, const QList<GTACommandBase*> &)
{
	bool rc = false;
	clearEditorRowCopy();
	isSelectedCommandsBalanced(ioLstSelectedRows);

	if (!ioLstSelectedRows.isEmpty())
	{
		int skipRowTill = -1;
		QList<int> lstSelectedRows = ioLstSelectedRows;
		qSort(lstSelectedRows.begin(), lstSelectedRows.end());
		for (int i = 0; i<lstSelectedRows.size(); i++)
		{
			int currentRow = lstSelectedRows.at(i);
			if (currentRow <= skipRowTill)
				continue;
			if (i>lstSelectedRows.size())
			{
				_LastError = "Size exceeds skipped rows";
				return false;
			}
			GTACommandBase* pBase = getCommand(currentRow);

			GTACommandBase* pCopy = NULL;
			if (pBase != NULL)
			{
				pCopy = pBase->getClone();
			}

			_lstCopiedElements.append(pCopy);

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



bool GTAViewController::setCopiedItems(QList<int>& ioLstSelectedRows, bool isNewCommand)
{
	bool rc = false;
	clearEditorRowCopy();
	isSelectedCommandsBalanced(ioLstSelectedRows);

	if (!ioLstSelectedRows.isEmpty())
	{
		int skipRowTill = -1;
		QList<int> lstSelectedRows = ioLstSelectedRows;
		qSort(lstSelectedRows.begin(), lstSelectedRows.end());
		for (int i = 0; i<lstSelectedRows.size(); i++)
		{
			int currentRow = lstSelectedRows.at(i);
			if (currentRow <= skipRowTill)
				continue;
			if (i>lstSelectedRows.size())
			{
				_LastError = "Size exceeds skipped rows";
				return false;
			}
			GTACommandBase* pBase = getCommand(currentRow);

			if (pBase)
			{
				GTACommandBase* pCopy = NULL;
				if (pBase != NULL)
				{
					pCopy = pBase->getClone();
				}
				GTAGenericToolCommand *pGenCmd = dynamic_cast<GTAGenericToolCommand *>(pCopy);
				GTAGenericToolCommand *pBaseGenCmd = dynamic_cast<GTAGenericToolCommand *>(pBase);
				if ((pGenCmd != NULL) && (pBaseGenCmd != NULL))
				{
					if (isNewCommand)
					{
						//pGenCmd->incrementCmdId();
						pGenCmd->setObjId();
						pCopy = pGenCmd;
					}
				}
				_lstCopiedElements.append(pCopy);


				int totalChildrens = 0;
				if (pBase != NULL)
					totalChildrens = pBase->getAllChildrenCount();


				if (totalChildrens != 0)
					skipRowTill = currentRow + totalChildrens; //skip those rows since these will be copied automatically

				rc = true;
			}
		}
	}
	return rc;
}

//int GTAViewController::getSelectedRowInEditor() const
//{
//    int oRow = -1;
//    //    if(_pEditorView != NULL)
//    //    {
//    //        QModelIndex index =  _pEditorView->currentIndex();
//    //        if(index.isValid())
//    //            oRow = index.row();
//    //    }



//    return oRow;
//}

bool GTAViewController::getModelForTreeView( QAbstractItemModel*& pModel , GTAElement* ipElement, const QString&)
{
	bool rc = false;
	if (ipElement != NULL)
	{
		//QStringList checkForRep= QStringList()<<iElemetName;
		QStringList checkForRep;
		//createExpandedTreeElement(ipElement,checkForRep);
		if (ipElement != NULL)
		{
			pModel = new GTAEditorTreeModel(ipElement);
			rc = true;
		}
		else
			_LastError = "could not expand element";
	}

	return rc;

}
//Creates an element with command at given row , attaches to a new tree model.
bool GTAViewController::getModelForTreeView(QAbstractItemModel*& pModel, const int& iRow, GTAElement *ipElement)
{
	bool rc = false;

	if (ipElement)
	{
		pModel = NULL;
		GTACommandBase* pCmd;
		rc = ipElement->getCommand(iRow, pCmd);
		if (rc && pCmd != NULL)
		{
			QString statement = pCmd->getStatement();
			GTAElement* pElement = NULL;
			if (pCmd->hasChildren() || pCmd->hasReference())
			{
				pElement = new GTAElement(GTAElement::CUSTOM);
				GTACommandBase* pCmdForElement = pCmd->getClone();
				pCmdForElement->setParent(NULL);
				pElement->insertCommand(pCmdForElement, 0, false);
				QStringList checkForRep;
				createExpandedTreeElement(pElement, checkForRep);
				pModel = new GTAEditorTreeModel(pElement);
				pModel->setObjectName(statement);
			}
			else
			{
				_LastError = "Current command not compatible for expanded view";
				rc = false;
			}

		}
		else
		{
			_LastError = "Command not found";
		}
	}
	else
		_LastError = "Element not valid";

	return rc;
}

void GTAViewController::createExpandedTreeElement(GTAElement *&iopElement, QStringList &checkForRep, bool)
{
    int cmdCnt = iopElement->getDirectChildrenCount();
    QString uuid = iopElement->getUuid();
	
    GTAElement *oGroupedElem = new GTAElement(iopElement->getElementType());

    // int GloblPosition = 0;
    GTAActionTitle *pTitle = new GTAActionTitle(ACT_TITLE);
    pTitle->setTitle("StartOfTestProcedure");
    GTAActionParentTitle *pTitleCmd = new GTAActionParentTitle(pTitle,"");
    int titleChildCnt = 0;
    for(int i=0;i<cmdCnt;i++)
    {

        GTACommandBase *pCmd1 = NULL;
        iopElement->getDirectChildren(i,pCmd1);
        if(pCmd1 != NULL)
        {
            GTACommandBase *pCmd = pCmd1->getClone();
            if(pCmd == NULL)
                continue;

			if(pCmd->isIgnoredInSCXML())
                continue;

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
            }
            else if(pTitleCmd && pCmd->hasReference())
            {
                GTAActionCall *pCallCmd = dynamic_cast<GTAActionCall *>(pCmd);
                if(pCallCmd != NULL)
                {
                    GTAActionExpandedCall *pExtCall = new GTAActionExpandedCall(*pCallCmd);
                    QString callElemName = pCallCmd->getElement();
                    QString functionNameWithArg = pCallCmd->getFunctionWithArgument();
                    pExtCall->setTagValue(pCallCmd->getTagValue());

                    //Load the call document in the element;
                    GTAElement* pElemOnCall=NULL;
                    GTAAppController* pController = GTAAppController::getGTAAppController();
                    bool rc = pController->getElementFromDocument(pCallCmd->getRefrenceFileUUID(),pElemOnCall,true);

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
                        if (!checkForRep.contains(uuid))
                        {
                            checkForRep.append(uuid);
							pCallCmd->updateTagValue();
                            pElemOnCall->replaceTag(pCallCmd->getTagValue());
                            pElemOnCall->updateCommandInstanceName(pCallCmd->getInstanceName());

                            GTAElement *groupedCallElem = new GTAElement(GTAElement::CUSTOM);

                            createExpandedTreeElement(pElemOnCall,checkForRep);
                            //remove the current element from the list to allow another call
                            checkForRep.removeOne(uuid);

                            groupedCallElem  = pElemOnCall;
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
                                        pExtCall->insertChildren(pCurCmd,callChildCntr++);
                                    }
                                }
                                if(pTitleCmd != NULL)
                                {
                                    pTitleCmd->insertChildren(pExtCall,titleChildCnt++);
                                }
                            }
                        }
                        else
                        {
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
                }

            }
            else if(pTitleCmd)// && pCmd->canBeCopied())
            {

                if(pCmd->canHaveChildren())
                {
                    /*
                      This code scope adds a condition command under a Title.
                      Creation of new if-else commands named pIfCmd2 and pElseCmd2.
                      These 2 commands are filled sequentially with children by using CreateCommandGroupedByTitle function to fetch children under a title(childrenTitle).
                    */
                    GTAActionParentTitle *childrenTitle = NULL;//new GTAActionParentTitle(NULL,"");
                    GTAActionIF *pIfCmd1 = dynamic_cast<GTAActionIF *>(pCmd);
                    GTACommandBase *pElseCmd2 = NULL;
                    GTACommandBase *pIfCmd2 = NULL;
                    if(pIfCmd1 != NULL)
                    {
                        pIfCmd2 = new GTAActionIF(*pIfCmd1);
                        pIfCmd2->clearChildrenList();
                        QList<GTACommandBase *> ifChildren = pIfCmd1->getChildren();

                        for(int j=0;j<ifChildren.count();j++)
                        {
                            GTACommandBase *pBaseCmd = ifChildren.at(j);

                            GTAActionBase *pActBase = dynamic_cast<GTAActionBase *>(pBaseCmd);
                            GTACheckBase *pChkBase = dynamic_cast<GTACheckBase *>(pBaseCmd);
                            if(pActBase != NULL)
                            {
                                if((pActBase->getAction() == ACT_CONDITION) && (pActBase->getComplement() == COM_CONDITION_ELSE))
                                {
                                    pElseCmd2 = pBaseCmd;
                                    break;
                                }
                                else
                                {
                                    pIfCmd2->insertChildren(pBaseCmd,j);
                                }
                            }
                            else if(pChkBase)
                            {
                                pIfCmd2->insertChildren(pBaseCmd,j);
                            }
                        }
                        createCommandGroupedByTitle(pIfCmd2,childrenTitle,checkForRep,checkForRep);

                        //clear any existing children as all children are now grouped under childrenTitle
                        pIfCmd2->clearChildrenList();

                        //add children of childrenTitle to the IfCmd2
                        if(childrenTitle != NULL)
                        {
                            QList<GTACommandBase *> childChildren = childrenTitle->getChildren();
                            if(childChildren.count()> 0)
                            {

                                //pIfCmd2->clearChildrenList();
                                for(int j=0;j<childChildren.count();j++)
                                {
                                    GTACommandBase *pchildTitleCmd =childChildren.at(j);
                                    if(pchildTitleCmd != NULL)
                                    {
                                        pIfCmd2->insertChildren(pchildTitleCmd,j);
                                    }
                                }
                            }
                        }
                        if(pElseCmd2 != NULL)
                        {
                            GTAActionParentTitle *childrenTitle1 = NULL;

                            createCommandGroupedByTitle(pElseCmd2,childrenTitle1,checkForRep,checkForRep);

                            //clear any existing children as all children are now grouped under childrenTitle1
                            pElseCmd2->clearChildrenList();

                            //add children of childrenTitle1 to the pElseCmd2
                            if(childrenTitle1 != NULL)
                            {
                                QList<GTACommandBase *> childChildren = childrenTitle1->getChildren();
                                if(childChildren.count()> 0)
                                {

                                    //pElseCmd2->clearChildrenList();
                                    for(int j=0;j<childChildren.count();j++)
                                    {
                                        GTACommandBase *pchildTitleCmd =childChildren.at(j);
                                        if(pchildTitleCmd != NULL)
                                        {
                                            pElseCmd2->insertChildren(pchildTitleCmd,j);
                                        }
                                    }
                                }
                            }
                            int ifChildrenCnt = (pIfCmd2->getChildren()).count();
                            pIfCmd2->insertChildren(pElseCmd2,ifChildrenCnt);
                        }
                        if(pCmd->canBeCopied())
                        {
                            pTitleCmd->insertChildren(pIfCmd2,titleChildCnt++);
                        }
                    }
                    else
                    {
                        /*
                          This code scope adds end condition commands or Else Commands under a Title.
                          Creation of new if-else commands named pIfCmd2 and pElseCmd2.
                          These 2 commands are filled sequentially with children by using CreateCommandGroupedByTitle function to fetch children under a title(childrenTitle).
                        */
                        createCommandGroupedByTitle(pCmd,childrenTitle,checkForRep,checkForRep);
                        if(childrenTitle != NULL)
                        {
                            QList<GTACommandBase *> childChildren = childrenTitle->getChildren();
                            if(childChildren.count()> 0)
                            {
                                //clear any existing children as all children are now grouped under childrenTitle
                                pCmd->clearChildrenList();

                                for(int j=0;j<childChildren.count();j++)
                                {
                                    GTACommandBase *pchildTitleCmd =childChildren.at(j);
                                    if(pchildTitleCmd != NULL)
                                    {
                                        // GTAActionIfEnd * ifEnd = dynamic_cast<GTAActionIfEnd * >(pchildTitleCmd);
                                        // GTAActionWhileEnd * whileEnd = dynamic_cast<GTAActionWhileEnd * >(pchildTitleCmd);
                                        // GTAActionDoWhileEnd * doWhileEnd = dynamic_cast<GTAActionDoWhileEnd* >(pchildTitleCmd);
                                        //if(!ifEnd && !whileEnd && !doWhileEnd)
                                        pCmd->insertChildren(pchildTitleCmd,j);
                                    }
                                }
                            }
                        }
                        //an isolated else shouldn't be created in the editor itself
                        //snippet added below as a failsafe
                        // GTAActionElse *pElseCmd = dynamic_cast<GTAActionElse *>(pCmd);
                        //if(pCmd->canBeCopied() || pElseCmd)
                        pTitleCmd->insertChildren(pCmd,titleChildCnt++);
                    }

                }
                else
                {
                    //if(pCmd->canBeCopied())
                    pTitleCmd->insertChildren(pCmd,titleChildCnt++);
                }
            }
        }
    }
    if(pTitleCmd != NULL)
    {
        oGroupedElem->appendCommand(pTitleCmd);
        iopElement = oGroupedElem;
    }
}

void GTAViewController::createCommandGroupedByTitle(GTACommandBase*& ipCmd, GTAActionParentTitle *&ipTitleCmd, QStringList &oVariableList, QStringList &icheckForRep)
{
    int GlobalTitleCnt = 0;//ipTitleCmd->getChildrens().size();
    if(ipCmd != NULL)
    {
        QList<GTACommandBase*> children = ipCmd->getChildren();
        GTAActionTitle *pTitle = new GTAActionTitle(ACT_TITLE);
        pTitle->setTitle("StartOfTestProcedure");
        GTAActionParentTitle *pTitleCmd = new GTAActionParentTitle(pTitle,"");
        int titleChildCnt = 0;
        for(int i=0;i<children.count();i++)
        {
            GTACommandBase * pCmd1 = children.at(i);
            if(pCmd1 != NULL)
            {
                GTACommandBase *pCmd = pCmd1->getClone();
                if(pCmd == NULL)
                    continue;

				if( pCmd->isIgnoredInSCXML())
                    continue;

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

                }
                else if(pTitleCmd && pCmd->hasReference())
                {
                    GTAActionCall *pCallCmd = dynamic_cast<GTAActionCall *>(pCmd);
                    if(pCallCmd != NULL)
                    {
                        GTAActionExpandedCall *pExtCall = new GTAActionExpandedCall(*pCallCmd);
                        QString callElemName = pCallCmd->getElement();
                        QString functionNameWithArg = pCallCmd->getFunctionWithArgument();
                        pExtCall->setTagValue(pCallCmd->getTagValue());

                        //Load the call document in the element;
                        GTAElement* pElemOnCall=NULL;
                        GTAAppController* pController = GTAAppController::getGTAAppController();
                        bool rc = pController->getElementFromDocument(pCallCmd->getRefrenceFileUUID(),pElemOnCall,true);


                        if(rc && pElemOnCall != NULL )
                        {
                            pElemOnCall->replaceTag(pCallCmd->getTagValue());
                            pElemOnCall->updateCommandInstanceName(pCallCmd->getInstanceName());
                            GTAElement *groupedCallElem = new GTAElement(GTAElement::CUSTOM);

                            createExpandedTreeElement(pElemOnCall,icheckForRep,groupedCallElem);

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
                                        pExtCall->insertChildren(pCurCmd,callChildCntr++);
                                    }
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
                    }
                }
                else// if(pCmd->canBeCopied())

                {
                    if(pTitleCmd)
                    {
                        if(pCmd->canHaveChildren())
                        {
                            GTAActionParentTitle *childrenTitle = NULL;//new GTAActionParentTitle(NULL,"");
                            GTAActionIF *pIfCmd1 = dynamic_cast<GTAActionIF *>(pCmd);
                            GTACommandBase *pElseCmd2 = NULL;
                            GTACommandBase *pIfCmd2 = NULL;
                            if(pIfCmd1 != NULL)
                            {
                                pIfCmd2 = new GTAActionIF(*pIfCmd1);
                                pIfCmd2->clearChildrenList();
                                QList<GTACommandBase *> ifChildren = pIfCmd1->getChildren();

                                for(int j=0;j<ifChildren.count();j++)
                                {
                                    GTACommandBase *pBaseCmd = ifChildren.at(j);

                                    GTAActionBase *pActBase = dynamic_cast<GTAActionBase *>(pBaseCmd);
                                    GTACheckBase *pChkBase = dynamic_cast<GTACheckBase *>(pBaseCmd);

                                    if(pActBase != NULL)
                                    {
                                        if((pActBase->getAction() == ACT_CONDITION) && (pActBase->getComplement() == COM_CONDITION_ELSE))
                                        {
                                            pElseCmd2 = pBaseCmd;
                                            break;
                                        }
                                        else
                                        {
                                            pIfCmd2->insertChildren(pBaseCmd,j);
                                        }
                                    }
                                    else if(pChkBase)
                                    {
                                        pIfCmd2->insertChildren(pBaseCmd,j);
                                    }
                                }
                                createCommandGroupedByTitle(pIfCmd2,childrenTitle,oVariableList,icheckForRep);
                                if(childrenTitle != NULL)
                                {
                                    QList<GTACommandBase *> childChildren = childrenTitle->getChildren();
                                    if(childChildren.count()> 0)
                                    {

                                        pIfCmd2->clearChildrenList();
                                        for(int j=0;j<childChildren.count();j++)
                                        {
                                            GTACommandBase *pchildTitleCmd =childChildren.at(j);
                                            if(pchildTitleCmd != NULL)
                                            {
                                                pIfCmd2->insertChildren(pchildTitleCmd,j);
                                            }
                                        }
                                    }
                                }
                                if(pElseCmd2 != NULL)
                                {
                                    GTAActionParentTitle *childrenTitle1 = NULL;

                                    createCommandGroupedByTitle(pElseCmd2,childrenTitle1,oVariableList,icheckForRep);
                                    if(childrenTitle != NULL)
                                    {
                                        QList<GTACommandBase *> childChildren = childrenTitle1->getChildren();
                                        if(childChildren.count()> 0)
                                        {

                                            pElseCmd2->clearChildrenList();
                                            for(int j=0;j<childChildren.count();j++)
                                            {
                                                GTACommandBase *pchildTitleCmd =childChildren.at(j);
                                                if(pchildTitleCmd != NULL)
                                                {
                                                    pElseCmd2->insertChildren(pchildTitleCmd,j);
                                                }
                                            }
                                        }
                                    }
                                    int ifChildrenCnt = (pIfCmd2->getChildren()).count();
                                    pIfCmd2->insertChildren(pElseCmd2,ifChildrenCnt);
                                }
                                if(pCmd->canBeCopied())
                                {
                                    pTitleCmd->insertChildren(pIfCmd2,titleChildCnt++);
                                }
                            }
                            else
                            {
                                createCommandGroupedByTitle(pCmd,childrenTitle,oVariableList,icheckForRep);
                                if(childrenTitle != NULL)
                                {
                                    QList<GTACommandBase *> childChildren = childrenTitle->getChildren();
                                    if(childChildren.count()> 0)
                                    {

                                        pCmd->clearChildrenList();
                                        for(int j=0;j<childChildren.count();j++)
                                        {
                                            GTACommandBase *pchildTitleCmd =childChildren.at(j);
                                            if(pchildTitleCmd != NULL)
                                            {
                                                // GTAActionIfEnd * ifEnd = dynamic_cast<GTAActionIfEnd * >(pchildTitleCmd);
                                                // GTAActionWhileEnd * whileEnd = dynamic_cast<GTAActionWhileEnd * >(pchildTitleCmd);
                                                // GTAActionDoWhileEnd * doWhileEnd = dynamic_cast<GTAActionDoWhileEnd* >(pchildTitleCmd);
                                                //if(!ifEnd && !whileEnd && !doWhileEnd)
                                                pCmd->insertChildren(pchildTitleCmd,j);
                                            }
                                        }
                                    }
                                }
                                GTAActionElse *pElseCmd = dynamic_cast<GTAActionElse *>(pCmd);
                                if(pCmd->canBeCopied() || pElseCmd)
                                    pTitleCmd->insertChildren(pCmd,titleChildCnt++);
                            }

                        }
                        else
                        {
                            //if(pCmd->canBeCopied())
                            pTitleCmd->insertChildren(pCmd,titleChildCnt++);

                        }
                    }
                    else
                    {
                        if(pCmd->canHaveChildren())
                        {
							//I Don't think this does anything
                            GTAActionTitle *pSubTitle = new GTAActionTitle(ACT_TITLE);
							pSubTitle->setTitle("");
                            GTAActionParentTitle *childrenTitle = new GTAActionParentTitle(NULL,"");
                            createCommandGroupedByTitle(pCmd,childrenTitle,oVariableList,icheckForRep);
                            //childrenTitle->insertChildren(new GTAActionParentTitleEnd,childrenTitle->getChildrens().size());
                            if(childrenTitle != NULL)
                            {
                                QList<GTACommandBase *> childChildren = childrenTitle->getChildren();
                                if(childChildren.count()> 0)
                                {

                                    pCmd->clearChildrenList();
                                    for(int j=0;j<childChildren.count();j++)
                                    {
                                        GTACommandBase *pchildTitleCmd =childChildren.at(j);

                                        if(pchildTitleCmd != NULL)
                                        {
                                                pCmd->insertChildren(pchildTitleCmd,j);
                                        }
                                    }
                                }
                            }
                        }
                        // GTAActionElse *pElseCmd = dynamic_cast<GTAActionElse *>(pCmd);
                        //if(pCmd->canBeCopied() || pElseCmd)
                        ipTitleCmd->insertChildren(pCmd,ipTitleCmd->getChildren().size());
                    }
                }
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



//Expands a given element by replacing call command by expanded version of call comand.
void GTAViewController::createExpandedTreeElement1(GTAElement*& iopElement, QStringList& checkForRep, bool groupExpandedElementByTitle)
{
    if(iopElement)
    {
        int size =iopElement->getDirectChildrenCount();
        int childCount = iopElement->getAllChildrenCount();

        if(size > 0)
        {

            GTAElement *pElement = new GTAElement(*iopElement);

            if(pElement)
            {
                pElement->ClearAll();
				for (int i = 0;i < size;i++,childCount--)
                {
                    GTACommandBase* pCurCmd = NULL;
                    iopElement->getDirectChildren(i,pCurCmd);
                    if (NULL!=pCurCmd)
                    {
                        if(pCurCmd->isIgnoredInSCXML())
                        {
                            //                            pElement->removeCommand(i);
                            childCount = iopElement->getAllChildrenCount();
                            continue;
                        }

                        int cmdChildCount = pCurCmd->getAllChildrenCount();
                        childCount = childCount-cmdChildCount;

                        GTAActionExpandedCall* pExtCall = NULL;//new GTAActionExpandedCall(*pCall);
                        pExtCall = createExpandedTreeElement1(pCurCmd,NULL,checkForRep,groupExpandedElementByTitle);
                        if (pExtCall)
                        {

                            pElement->insertCommand(pExtCall,childCount,false);
                            pExtCall->setParent(NULL);
                            pElement->removeCommand(childCount-1);
                        }
                    }

                }
            }
            iopElement = pElement;
        }
    }
}
/**
*This function returns expanded call for any command.
If it has children then will return null but all its children containing call command will be replaced by extended call command.
If current command is call then, it will expand the call, replace its children those are call with 'expanded call' command and return Expanded call.

*/
GTAActionExpandedCall* GTAViewController::createExpandedTreeElement1(GTACommandBase*& ipCmd, GTACommandBase* ipParentCmd, QStringList& chkForLev, bool groupExpandedElementByTitle)
{
    if (NULL!=ipCmd)
    {
		if(ipCmd->isIgnoredInSCXML())
        {
            return NULL;
        }

        //For 

        if (ipCmd->hasChildren())
        {
            QList<GTACommandBase*>& lstCmdChildrens = ipCmd->getChildren();

            for (int i=0;i<lstCmdChildrens.size();i++)
            {   
                GTACommandBase* pChildCmd = lstCmdChildrens.at(i);
                GTAActionExpandedCall* pExtCall = createExpandedTreeElement1(pChildCmd,ipCmd,chkForLev,groupExpandedElementByTitle);
                //chkForLev.clear();
                if (NULL!=pExtCall)
                {
                    if (NULL!=pChildCmd)
                    {delete pChildCmd;pChildCmd=NULL;}

                    lstCmdChildrens.removeAt(i);
                    lstCmdChildrens.insert(i,pExtCall);
                    
                }
            }
        }
        else
        {
            GTAActionCall* pCall = dynamic_cast<GTAActionCall*>(ipCmd);
            if (NULL!=pCall)
            {
                GTAActionExpandedCall* pExtCall = new GTAActionExpandedCall(*pCall);
                pExtCall->setParent(ipParentCmd);
                GTACommandBase* pExtCallBase = pExtCall;
                QString documentName = pCall->getElement();
                QString functionNamewithArg = pCall->getFunctionWithArgument();
                pExtCall->setTagValue(pCall->getTagValue());
                QString callTimeout,callTimeoutUint;
                pCall->getTimeOut(callTimeout,callTimeoutUint);
                // double TimeInMS = ((callTimeoutUint == ACT_TIMEOUT_UNIT_MSEC) ? callTimeout.toDouble():callTimeout.toDouble() * 1000);
                
                //QList<double> timeOuts = pCall->getReferenceTimeoutStates();
                //The is code does not give  correct recursion info
                //though it introduce a bug , if the document is used multiple time
                if (!chkForLev.contains(functionNamewithArg))
                {
                    chkForLev.append(functionNamewithArg);

                }
                else
                {
                    pExtCall->setBrokenLink(true);
                    return pExtCall;
                }
                GTAElement* pElemOnCall=NULL;
                GTAAppController* pController = GTAAppController::getGTAAppController();
                bool rc = pController->getElementFromDocument(pCall->getRefrenceFileUUID(),pElemOnCall,true);
                if (rc && pElemOnCall)
                {
                    if(groupExpandedElementByTitle)
                    {
                        GTAElement* pTitleGroupElem = NULL;
                        pElemOnCall->createActionElementGroupedByTitle(pTitleGroupElem);
                        if(NULL!=pTitleGroupElem)
                        {
                            delete pElemOnCall;
                            pElemOnCall = pTitleGroupElem;
                        }
                    }
                    pElemOnCall->replaceTag(pCall->getTagValue());
                    pElemOnCall->updateCommandInstanceName(pCall->getInstanceName());
                    int size =pElemOnCall->getDirectChildrenCount();
                    int callCmdIdx = 0;
                    for (int i=0;i<size;i++)
                    {
                        GTACommandBase* pCurCmd = NULL;
                        pElemOnCall->getDirectChildren(i,pCurCmd);
                        
                        if (pCurCmd != NULL)
                        { 
                            if(pCurCmd->isIgnoredInSCXML())
                            {
                                continue;
                            }

                            if(pCurCmd->hasReference())
                            {
                                GTAActionExpandedCall* pExt = createExpandedTreeElement1(pCurCmd,pExtCallBase,chkForLev,groupExpandedElementByTitle);
                                if (pExt)
                                    pExtCall->insertChildren(pExt,callCmdIdx++);
                            }
                            else
                                pExtCall->insertChildren(pCurCmd,callCmdIdx++);
                        }
                        
                    }
                    chkForLev.removeAll(functionNamewithArg);


                }
                else
                    pExtCall->setBrokenLink(true);

                pExtCall->setReportStatusOfChildrens(GTACommandBase::EXCLUDE);
                pExtCall->setReportStatus(GTACommandBase::PARTIALLY_INCLUDE);
                return pExtCall;
                

            }
        }
    }
    return NULL;
}
//bool GTAViewController::addheaderField(const QString& iName,const QString& iVal,const QString& iMandatory,const QString& iDescription)
//{

//    bool rc = false;
//    if (_pHeaderModel)
//    {
//        int row = _pHeader->itemSize();
//        bool bMandatory=true;

//        if (iMandatory.contains("No"))
//            bMandatory = false;

//        //bool bAddRC = false;
//        //bAddRC=_pHeader->addHeaderItem(iName,bMandatory,iVal,iDescription);
//        //if (bAddRC)
//        //{
//        //  rc = _pHeaderModel->insertRows(row,0,QModelIndex());

//        //}

//        rc = _pHeaderModel->insertRows(row,1,QModelIndex());
//        if (rc)
//        {
//            QModelIndex index= _pHeaderModel->index(row,0);
//            headerNodeInfo hdrInfo;
//            hdrInfo._name = iName;
//            hdrInfo._description = iDescription;
//            hdrInfo._val = iVal;
//            hdrInfo._Mandatory = bMandatory;

//            QVariant setVarData;
//            setVarData.setValue(hdrInfo);
//            rc = _pHeaderModel->setData(index,setVarData);
//            if(!rc)
//                _LastError = "set data failed";
//        }
//        else
//            _LastError = _pHeader->getLastError();
//    }
//    else
//        _LastError = "Model does not exist";

//    return rc;

//}

//bool GTAViewController::showICDDetails(const bool & iShow,QList <GTAFILEInfo> lstFileInfo)
//{
//    bool rc = false;

//    if (!lstFileInfo.isEmpty())
//    {
//        if (iShow)
//            _pHeaderModel->setFileInfo(lstFileInfo);
//        else
//            _pHeaderModel->removeFileInfo();

//        rc=true;
//    }
//    else
//        _LastError = "No information on ICDs found";
//    return rc;

//}

//bool GTAViewController::editHeaderField(const int& iRow, const QString& iName,const QString& iVal,bool iMandatory,const QString& iDescription)
//{

//    bool rc = false;
//    if (_pHeaderModel)
//    {
//        int row = _pHeaderModel->rowCount(QModelIndex());
//        bool bMandatory = true;

//        QModelIndex index= _pHeaderModel->index(iRow,0);
//        headerNodeInfo hdrInfo;
//        hdrInfo._name = iName;
//        hdrInfo._description = iDescription;
//        hdrInfo._val = iVal;
//        hdrInfo._Mandatory = iMandatory;

//        QVariant setVarData;
//        setVarData.setValue(hdrInfo);
//        rc = _pHeaderModel->setData(index,setVarData);

//        //bAddRC=_pHeader->editHeaderItem(iRow,iName,bMandatory,iVal,iDescription);
//        if (!rc)
//            _LastError = _pHeader->getLastError();
//    }
//    else
//        _LastError = "Model does not exist";

//    return rc;

//}
//bool GTAViewController::removeHeaderField(const int& iRow)
//{
//    bool rc = false;
//    if (iRow)
//    {

//        rc = _pHeaderModel->removeRows(iRow,0,QModelIndex());
//    }
//    else
//        _LastError = "Header name is non editable";

//    return rc;
//}
//bool GTAViewController::moveHeaderRowItemDown(const int& iRow)
//{
//    bool rc = false;
//    if (_pHeader!=NULL)
//    {

//        if (iRow>0 && iRow<_pHeader->itemSize()-1)
//        {
//            QString currentRowTagName   = _pHeader->getFieldName(iRow);
//            QString currentRowTagVal    = _pHeader->getFieldValue(iRow);
//            QString currentRowTagDes    = _pHeader->getFieldDescription(iRow);
//            bool    currentRowTagMan    = _pHeader->isNodeMandatory(currentRowTagName);

//            headerNodeInfo currentNodeInfo;
//            currentNodeInfo._description=currentRowTagDes;
//            currentNodeInfo._Mandatory=currentRowTagMan;
//            currentNodeInfo._name=currentRowTagName;
//            currentNodeInfo._val=currentRowTagVal;


//            QString nextRowTagName   = _pHeader->getFieldName(iRow+1);
//            QString nextRowTagVal    = _pHeader->getFieldValue(iRow+1);
//            QString nextRowTagDes    = _pHeader->getFieldDescription(iRow+1);
//            bool    nextRowTagMan    = _pHeader->isNodeMandatory(nextRowTagName);


//            headerNodeInfo nextNodeInfo;
//            nextNodeInfo._description=nextRowTagDes;
//            nextNodeInfo._Mandatory=nextRowTagMan;
//            nextNodeInfo._name=nextRowTagName;
//            nextNodeInfo._val=nextRowTagVal;

//            QModelIndex currentRowIndex = _pHeaderModel->index(iRow,0);


//            headerNodeInfo emptyInfo;
//            emptyInfo._description="";
//            emptyInfo._Mandatory=false;
//            emptyInfo._name="";
//            emptyInfo._val="";

//            rc=_pHeaderModel->insertRows(iRow,1,QModelIndex());
//            if (rc)
//            {
//                rc = _pHeaderModel->removeRows(iRow+2,0,QModelIndex());

//                if (rc)
//                {
//                    QModelIndex prevRowIndex = _pHeaderModel->index(iRow,0);
//                    QVariant currentValVar;
//                    currentValVar.setValue(nextNodeInfo);
//                    rc = _pHeaderModel->setData(prevRowIndex,currentValVar);

//                }
//            }

//        }

//    }
//    return rc;
//}
//bool GTAViewController::moveHeaderRowItemUp(const int& iRow)
//{
//    bool rc = false;
//    if (_pHeader!=NULL)
//    {

//        if (iRow>1)
//        {
//            QString currentRowTagName   = _pHeader->getFieldName(iRow);
//            QString currentRowTagVal    = _pHeader->getFieldValue(iRow);
//            QString currentRowTagDes    = _pHeader->getFieldDescription(iRow);
//            bool    currentRowTagMan    = _pHeader->isNodeMandatory(currentRowTagName);

//            headerNodeInfo currentNodeInfo;
//            currentNodeInfo._description=currentRowTagDes;
//            currentNodeInfo._Mandatory=currentRowTagMan;
//            currentNodeInfo._name=currentRowTagName;
//            currentNodeInfo._val=currentRowTagVal;


//            QString previousRowTagName   = _pHeader->getFieldName(iRow-1);
//            QString previousRowTagVal    = _pHeader->getFieldValue(iRow-1);
//            QString previousRowTagDes    = _pHeader->getFieldDescription(iRow-1);
//            bool    previousRowTagMan    = _pHeader->isNodeMandatory(previousRowTagName);


//            headerNodeInfo previousNodeInfo;
//            previousNodeInfo._description=previousRowTagDes;
//            previousNodeInfo._Mandatory=previousRowTagMan;
//            previousNodeInfo._name=previousRowTagName;
//            previousNodeInfo._val=previousRowTagVal;

//            QModelIndex currentRowIndex = _pHeaderModel->index(iRow,0);


//            headerNodeInfo emptyInfo;
//            emptyInfo._description="";
//            emptyInfo._Mandatory=false;
//            emptyInfo._name="";
//            emptyInfo._val="";

//            rc=_pHeaderModel->insertRows(iRow-1,1,QModelIndex());
//            if (rc)
//            {
//                rc = _pHeaderModel->removeRows(iRow+1,0,QModelIndex());
//                if (rc)
//                {
//                    QModelIndex prevRowIndex = _pHeaderModel->index(iRow-1,0);
//                    QVariant currentValVar;
//                    currentValVar.setValue(currentNodeInfo);
//                    rc = _pHeaderModel->setData(prevRowIndex,currentValVar);
//                }
//            }



//        }

//    }
//    return rc;
//}
//bool GTAViewController::moveEditorRowItemUp(const int& selectedItem)
//{

//    bool rc = true;
//    if (selectedItem<=0)
//        return false;

//    GTACommandBase* pCurrentSelectedCmd = NULL;
//    pCurrentSelectedCmd = getCommand(selectedItem);
//    if (pCurrentSelectedCmd==NULL)
//        return false;

//    GTACommandBase* pCurrentItemsParent = pCurrentSelectedCmd->getParent();
//    pCurrentSelectedCmd = pCurrentSelectedCmd->getClone();

//    QList<GTACommandBase*> currentLstOfCopiedItem = _lstCopiedElements;
//    _lstCopiedElements.clear();

//    GTACommandBase* pItemAboveSelectedItem = getCommand(selectedItem-1);


//    if (!pCurrentSelectedCmd->IsUserEditable())
//    {
//        if (pItemAboveSelectedItem==NULL)
//        {
//            addEditorRows(selectedItem);
//            deleteEditorRows(QList<int>()<<selectedItem-1);
//        }
//        else
//        {
//            if (pItemAboveSelectedItem!=pCurrentItemsParent)
//            {
//                addEditorRows(selectedItem);
//                _lstCopiedElements.append(pItemAboveSelectedItem);
//                rc = pasteEditorCopiedRow(selectedItem+1);
//                deleteEditorRows(QList<int>()<<selectedItem-1);
//                deleteEditorRows(QList<int>()<<selectedItem+1);
//            }

//        }

//    }
//    else if (pItemAboveSelectedItem==NULL)
//    {
//        addEditorRows(selectedItem);
//        deleteEditorRows(QList<int>()<<selectedItem-1);

//    }
//    else /*if (!pItemAboveSelectedItem->hasChildren() && !pCurrentSelectedCmd->hasChildren() )*/
//    {


//        if (pItemAboveSelectedItem == pCurrentItemsParent)
//        {
//            int rowAboveParent = selectedItem -2;
//            if (rowAboveParent>=0)
//            {
//                deleteEditorRows(QList<int>()<<selectedItem);
//                addEditorRows(rowAboveParent);
//                _lstCopiedElements.append(pCurrentSelectedCmd);
//                rc = pasteEditorCopiedRow(++rowAboveParent);
//                deleteEditorRows(QList<int>()<<selectedItem + pCurrentSelectedCmd->getAllChildrenCount());

//            }
//            else
//            {
//                _pElementViewModel->prependRow();
//                _lstCopiedElements.append(pCurrentSelectedCmd);
//                rc=pasteEditorCopiedRow(0);
//                deleteEditorRows(QList<int>()<<1+selectedItem);
//            }

//        }

//        else if (pItemAboveSelectedItem!=NULL && pItemAboveSelectedItem->IsUserEditable()==true && pCurrentSelectedCmd->canHaveChildren())
//        {
//            if (selectedItem-1>=0)
//            {
//                pItemAboveSelectedItem=pItemAboveSelectedItem->getClone();
//                deleteEditorRows(QList<int>()<<selectedItem-1);
//                addEditorRows(selectedItem-1);
//                _lstCopiedElements.append(pItemAboveSelectedItem);
//                pasteEditorCopiedRow(selectedItem);
//                deleteEditorRows(QList<int>()<<selectedItem+1);
//            }

//        }
//        else
//        {

//            if (pItemAboveSelectedItem->IsUserEditable()==false)
//            {
//                _lstCopiedElements.append(pCurrentSelectedCmd->getClone());
//                deleteEditorRows(QList<int>()<<selectedItem);
//                addEditorRows(selectedItem-2);
//                pasteEditorCopiedRow(selectedItem-1);
//                deleteEditorRows(QList<int>()<<selectedItem);

//            }
//            else
//            {
//                pItemAboveSelectedItem = pItemAboveSelectedItem->getClone();

//                _lstCopiedElements.append(pItemAboveSelectedItem);
//                int childrenCount = pCurrentSelectedCmd->getAllChildrenCount();

//                addEditorRows(selectedItem + childrenCount);
//                rc = pasteEditorCopiedRow(selectedItem+childrenCount+1);
//                if (rc)
//                {
//                    deleteEditorRows(QList<int>()<<selectedItem-1);
//                    deleteEditorRows(QList<int>()<<selectedItem + childrenCount+1);

//                }
//            }







//        }



//    }
//    _lstCopiedElements.clear();
//    _lstCopiedElements = currentLstOfCopiedItem;


//    return true;



//}

//bool GTAViewController::isEditorRowMovableUpwards(const int& selectedItem)const
//{
//    bool rc = true;
//    if (selectedItem<=0) //0th row not movable
//        return false;

//    GTACommandBase* pCurrentSelectedCmd = NULL;
//    pCurrentSelectedCmd = getCommand(selectedItem);
//    if (pCurrentSelectedCmd==NULL)
//        return false;

//    GTACommandBase* pItemAboveSelectedItem = getCommand(selectedItem-1);

//    if (pItemAboveSelectedItem==NULL)
//        return true;
//    else if (!pCurrentSelectedCmd->IsUserEditable()) // if current command is an end statement
//    {

//        /***********
//        case covered:
//        if
//            -
//            -
//            if
//                -
//                -
//                end
//            end         <<<
//        ***********/
//        if (pItemAboveSelectedItem->IsUserEditable() == false)
//            return false;
//        /***********
//        case covered:
//        if
//            -
//            -
//            else
//            end         <<<
//        -
//        ***********/
//        else if(dynamic_cast<GTAActionElse*>(pItemAboveSelectedItem) !=NULL )
//            return false;
//        /***********
//        case covered:
//        if
//            end         <<<
//        ***********/
//        else if (pItemAboveSelectedItem == pCurrentSelectedCmd->getParent() && pCurrentSelectedCmd->IsUserEditable()==false)
//            return false;
//    }
//    else if(pCurrentSelectedCmd != NULL && pCurrentSelectedCmd->IsUserEditable())
//    {
//        GTAActionElse* pElse = dynamic_cast<GTAActionElse*>(pCurrentSelectedCmd);


//        if (pElse!= NULL )
//        {
//            /***********
//            case covered:
//            if
//            else        <<<
//            end
//            ***********/
//            if ( pElse->getParent() == pItemAboveSelectedItem)
//                return false;

//            else if ( pItemAboveSelectedItem->IsUserEditable() == false)
//            {
//                /***********
//                case covered:
//                if
//                    -
//                    if
//                        -
//                        -
//                        end
//                    else        <<<
//                    -
//                    -
//                    end
//                ***********/
//                if (pCurrentSelectedCmd->hasChildren()==true)
//                    return false;
//                /***********
//                case covered:
//                if
//                    -
//                    while
//                        -
//                        -
//                        end
//                    else        <<<
//                    end
//                ***********/
//                else if (dynamic_cast<GTAActionIF*> (pItemAboveSelectedItem->getParent()) == NULL )
//                    return false;
//                /***********
//                case covered:
//                if
//                    -
//                    if
//                        -
//                        else
//                        -
//                        end
//                    else        <<<
//                    end
//                ***********/
//                else if ( pItemAboveSelectedItem->getParent()->hasMutuallExclusiveStatement() == true)
//                    return false;
//            }
//        }
//        else
//        {
//            /***********
//            case covered:
//            if/while
//                -
//                -
//                -
//                end
//            if/while        <<<
//                -
//                end
//            ***********/
//            if (pCurrentSelectedCmd->hasChildren() && pItemAboveSelectedItem->IsUserEditable()==false)
//                return false;
//            else
//                if (pCurrentSelectedCmd->getParent() == pItemAboveSelectedItem && pCurrentSelectedCmd->canHaveChildren())
//                    return false;
//        }


//    }

//    return rc;
//}
//bool GTAViewController::moveEditorRowItemDown(const int& selectedItem)
//{
//    bool rc = true;
//    if (selectedItem>=_pElementViewModel->rowCount()-1)
//        return false;

//    GTACommandBase* pCurrentSelectedCmd = NULL;
//    pCurrentSelectedCmd = getCommand(selectedItem);
//    if (pCurrentSelectedCmd==NULL)
//        return false;

//    GTACommandBase* pCurrentItemsParent = pCurrentSelectedCmd->getParent();
//    GTACommandBase* pItemNextToSelectedItem = getCommand(selectedItem+1);

//    QList<GTACommandBase*> currentLstOfCopiedItem = _lstCopiedElements;
//    _lstCopiedElements.clear();



//    if (selectedItem>=0)
//    {
//        if (pItemNextToSelectedItem!=NULL && pItemNextToSelectedItem->canHaveChildren())
//        {
//            _lstCopiedElements.append(pCurrentSelectedCmd->getClone());
//            addEditorRows(selectedItem+1);
//            pasteEditorCopiedRow(selectedItem+2);
//            deleteEditorRows(QList<int>()<<selectedItem);
//            deleteEditorRows(QList<int>()<<selectedItem+2);
//        }
//        else{

//            if (pCurrentSelectedCmd->hasChildren())
//            {
//                if (pItemNextToSelectedItem!=NULL)
//                {
//                    pItemNextToSelectedItem = pItemNextToSelectedItem->getClone();
//                    deleteEditorRows(QList<int>()<<selectedItem+1);
//                    if (selectedItem-1<0)
//                        _pElementViewModel->prependRow();
//                    else
//                        addEditorRows(selectedItem-1);

//                    _lstCopiedElements.append(pItemNextToSelectedItem);
//                    pasteEditorCopiedRow(selectedItem);

//                    if (selectedItem-1>=0)
//                        deleteEditorRows(QList<int>()<<selectedItem+1);



//                }
//                else
//                {
//                    if (selectedItem-1>=0)
//                    {
//                        deleteEditorRows(QList<int>()<<selectedItem+1);
//                        addEditorRows(selectedItem-1);
//                    }
//                    else
//                    {
//                        deleteEditorRows(QList<int>()<<selectedItem+1);
//                        _pElementViewModel->prependRow();

//                    }
//                }

//            }
//            else
//            {
//                if (pItemNextToSelectedItem==NULL)
//                {

//                    if (selectedItem-1>=0)
//                    {
//                        deleteEditorRows(QList<int>()<<selectedItem+1);
//                        addEditorRows(selectedItem-1);
//                    }
//                    else
//                    {
//                        deleteEditorRows(QList<int>()<<selectedItem+1);
//                        _pElementViewModel->prependRow();
//                    }
//                }
//                else
//                {
//                    if (!pCurrentSelectedCmd->IsUserEditable())
//                    {
//                        _lstCopiedElements.append(pItemNextToSelectedItem->getClone());
//                        addEditorRows(selectedItem-1);
//                        pasteEditorCopiedRow(selectedItem);
//                        deleteEditorRows(QList<int>()<<selectedItem+1);
//                        deleteEditorRows(QList<int>()<<selectedItem+2);

//                    }
//                    else
//                    {
//                        _lstCopiedElements.append(pCurrentSelectedCmd->getClone());
//                        deleteEditorRows(QList<int>()<<selectedItem);
//                        addEditorRows(selectedItem);
//                        pasteEditorCopiedRow(selectedItem+1);
//                        deleteEditorRows(QList<int>()<<selectedItem+2);
//                    }
//                }

//            }

//        }

//    }



//    _lstCopiedElements.clear();
//    _lstCopiedElements=currentLstOfCopiedItem;


//    return rc;
//}
//bool GTAViewController::isEditorRowMovableDownwards(const int& selectedItem)const
//{
//    bool rc = true;
//    if (selectedItem>=_pElementViewModel->rowCount()-1)
//        return false;

//    GTACommandBase* pCurrentSelectedCmd = NULL;
//    pCurrentSelectedCmd = getCommand(selectedItem);
//    if (pCurrentSelectedCmd==NULL)
//        return false;

//    GTACommandBase* pCommandBelowSelItem = getCommand(selectedItem+1);
//    if (pCommandBelowSelItem==NULL)
//        return true;
//    else if (!pCurrentSelectedCmd->IsUserEditable())
//    {

//        GTACommandBase* pBelowElse = dynamic_cast<GTAActionElse*>(pCommandBelowSelItem);
//        GTAActionIfEnd* pCurrentIFEnd = dynamic_cast<GTAActionIfEnd*>(pCurrentSelectedCmd);
//        GTAActionIF* pNextIF = dynamic_cast<GTAActionIF*>(pCommandBelowSelItem);
//        GTAActionIF* pCurrentIF = dynamic_cast<GTAActionIF*>(pCurrentSelectedCmd);
//        GTAActionWhileEnd* pCurrentWhileEnd = dynamic_cast<GTAActionWhileEnd*>(pCurrentSelectedCmd);
//        GTAActionWhile* pNextWhile = dynamic_cast<GTAActionWhile*>(pCommandBelowSelItem);

//        if (pCurrentIFEnd!=NULL && pNextWhile!=NULL)
//            return false;
//        else if (pCurrentWhileEnd!=NULL && pNextIF!=NULL)
//            return false;




//        /***********
//        case covered:
//        if
//            -
//            -
//            if
//                -
//                -
//                end         <<<
//            end
//        ***********/
//        if (pCommandBelowSelItem->IsUserEditable() == false)
//            return false;
//        /***********
//        case covered:
//        if
//            if
//                -
//                -
//                end         <<<
//            else
//                -
//            end
//        ***********/
//        else if( pBelowElse!=NULL )
//        {
//            if (pBelowElse->hasChildren() == true)
//                return false;
//        }
//        /***********
//        case covered:
//        if
//            if
//            -
//            -
//            end         <<<
//            if/else/while
//                -
//            end
//        ***********/
//        else if (pCommandBelowSelItem->hasChildren())
//            return false;

//    }
//    else if(pCurrentSelectedCmd != NULL && pCurrentSelectedCmd->IsUserEditable())
//    {
//        GTAActionElse* pElse = dynamic_cast<GTAActionElse*>(pCurrentSelectedCmd);

//        if (pElse!=NULL)
//        {
//            if (pCommandBelowSelItem->canHaveChildren())
//            {
//                /***********
//                case covered:
//                if
//                    if
//                    -
//                    -
//                    else         <<<
//                        while/for
//                            -
//                            end
//                    end
//                    -
//                    end
//                ***********/
//                if (dynamic_cast<GTAActionIF*>(pCommandBelowSelItem)==NULL)
//                    return false;
//                /***********
//                case covered:
//                if
//                    if
//                        -
//                        -
//                        else         <<<
//                        if
//                            -
//                            else
//                                -
//                            end
//                        end
//                    -
//                    end
//                ***********/
//                else
//                {
//                    if(pCommandBelowSelItem->hasMutuallExclusiveStatement())
//                        return false;
//                }
//            }
//            else if ( dynamic_cast<GTAActionIfEnd*> (pCommandBelowSelItem)!=NULL)
//            {
//                /***********
//                case covered:
//                    -
//                    -
//                    if
//                        -
//                        else
//                    end
//               ***********/
//                if (selectedItem +2 >_pElementViewModel->rowCount()-1)
//                    return false;
//                else
//                {
//                    GTACommandBase* pNextNextCmd = getCommand(selectedItem+2);
//                    /***********
//                    case covered:
//                        -
//                        -
//                        if
//                            -
//                            else
//                            end
//                        _
//                        ***********/
//                    if (dynamic_cast<GTAActionIfEnd*> (pNextNextCmd)==NULL)
//                        return false;
//                }
//            }

//        }

//        else
//        {
//            /* if (pCommandBelowSelItem->hasChildren())
//                return false;
//            else*/
//            {
//                GTAActionIF* pNextIF = dynamic_cast<GTAActionIF*>(pCommandBelowSelItem);
//                GTAActionIF* pCurrentIF = dynamic_cast<GTAActionIF*>(pCurrentSelectedCmd);
//                GTAActionWhile* pNextWhile = dynamic_cast<GTAActionWhile*>(pCommandBelowSelItem);
//                GTAActionWhile* pCurrentWhile = dynamic_cast<GTAActionWhile*>(pCurrentSelectedCmd);
//                GTAActionElse* pNextElse = dynamic_cast<GTAActionElse*>(pCommandBelowSelItem);
//                if(pCurrentIF!=NULL && pNextIF!=NULL || pCurrentIF!=NULL && pNextElse!=NULL || pCurrentIF!=NULL && pNextWhile!=NULL || pCurrentWhile!=NULL && pNextIF!=NULL)
//                    return false;

//                GTACommandBase* pBelowElse = dynamic_cast<GTAActionElse*>(pCommandBelowSelItem);


//                if (dynamic_cast<GTAActionIfEnd*> (pCommandBelowSelItem)!=NULL && pCommandBelowSelItem->getParent() == pCurrentSelectedCmd )
//                    return false;
//                if (dynamic_cast<GTAActionWhileEnd*> (pCommandBelowSelItem)!=NULL && pCommandBelowSelItem->getParent() == pCurrentSelectedCmd )
//                    return false;
//            }

//        }


//    }

//    return rc;
//}
//bool GTAViewController::searchHighlight(const QRegExp& searchString,QList<int>& iSelectedRows,bool ibSearchUp,bool ibTextSearch,QModelIndex &oHighlightedRowIdx)
//{
//    bool rc = false;

//    if (_pElement!=NULL)
//    {
//        _pElementViewModel->setDisplayContext(GTAElementViewModel::SEARCH_REPLACE);
//        QList<int> foundRows;
//        _pElementViewModel->findAndHighlight(searchString,iSelectedRows,foundRows,ibSearchUp,ibTextSearch);
//        if (!foundRows.isEmpty())
//        {
//            _lastHighligtedRow = foundRows.takeLast();
//            oHighlightedRowIdx = _pElementViewModel->index(_lastHighligtedRow,0);
//            rc = true;
//        }
//        else
//            _lastHighligtedRow = -1;

//    }
//    return rc;
//}
//void GTAViewController::resetDisplayContext()
//{
//    if (_pElement!=NULL)
//    {
//        _pElementViewModel->setDisplayContext(GTAElementViewModel::EDITING);
//        _lastHighligtedRow=-1;
//    }
//}
//void GTAViewController::setDisplaycontextToCompatibilityCheck()
//{
//    if (_pElement!=NULL)
//    {
//        _pElementViewModel->setDisplayContext(GTAElementViewModel::COMPATIBILITY_CHK);
//    }
//}
//bool GTAViewController::hightlightReplace(const QRegExp& searchString,const QString& iReplaceString,
//                                             QList<int>& iSelectedRows,bool ibSearchUp,QModelIndex &oHighlightedRowIdx )
//{
//    bool rc = false;
//    QRegExp regExpSearch(searchString);
//    if (_pElement!=NULL)
//    {
//        if (_lastHighligtedRow==-1)
//            rc =searchHighlight(regExpSearch,iSelectedRows,ibSearchUp,false,oHighlightedRowIdx);
//        else
//        {
//            rc = _pElementViewModel->replaceString(_lastHighligtedRow,searchString,iReplaceString,ibSearchUp);
//            iSelectedRows.clear();
//            iSelectedRows.append(_lastHighligtedRow+1);
//            searchHighlight(regExpSearch,iSelectedRows,ibSearchUp,false,oHighlightedRowIdx);

//        }

//    }
//    return rc;
//}

//bool GTAViewController::replaceAllString(const QRegExp& searchString,const QString& iReplaceString)
//{

//    bool rc = false;

//    if (_pElement!=NULL && _pElementViewModel!=NULL)

//    {
//        _pElementViewModel->setDisplayContext(GTAElementViewModel::SEARCH_REPLACE);
//        _lastHighligtedRow=-1;
//        rc = _pElementViewModel->replaceAllString(searchString,iReplaceString);


//    }
//    return rc;
//}
//bool GTAViewController::highLightLastChild(const QModelIndex& ipIndex)
//{
//    bool rc = false;
//    if (_pElementViewModel)
//    {

//        rc = _pElementViewModel->highLightLastChild(ipIndex);
//    }
//    return rc;
//}
bool GTAViewController::getExpandedElementFromLoadedElement(GTAElement*& oElement, bool flattenElements)
{
	bool rc = false;
	GTAElement* tempElement = NULL;
	if (_pElement != NULL)
	{
		tempElement = new GTAElement(*_pElement);//_pElement->getClone();
		QStringList checkRec;
		this->createExpandedTreeElement(tempElement, checkRec);
		if (flattenElements)
			tempElement->flattenCallCommands();
		oElement = tempElement;
		rc = true;
	}
	else
		_LastError = "Element not loaded";

	return rc;
}
void GTAViewController::getRowsContainingTitle(QList<int>& oLstOfRows)
{
    // GTAElement* tempElement = NULL;
    if (_pElement!=NULL)
    {
        int allItems = _pElement->getAllChildrenCount();
        for (int i=0;i<allItems;i++)
        {
            GTACommandBase* pCmd = NULL;
            _pElement->getCommand(i,pCmd);

            if (pCmd!=NULL && pCmd->isTitle())
            {
                oLstOfRows.append(i);
            }
        }

    }
    else
        _LastError = "Element not loaded";

}
bool GTAViewController::createElementDataBrowserModel(const QStringList& iFilters, const QString& iDataDir)
{
	bool rc = true;

	if (_pElemBrowserModel == NULL)
		_pElemBrowserModel = new GTAFileSystemModel(this);


	_pElemBrowserModel->setRootPath(iDataDir);
	_pElemBrowserModel->setNameFilters(iFilters);
	_pElemBrowserModel->setNameFilterDisables(false);
	_pElemBrowserModel->setFilter(QDir::Files);//|QDir::AllDirs|QDir::NoDotAndDotDot);
	_pElemBrowserModel->index(iDataDir);
	return rc;
}

/*    Funcition      : GTATreeViewModel::cloneTree
*    Description    : clones  a tree. To clone a tree from root node pass newRoot as NULL. Can also clone part of a tree
*   Parameters      : GTATreeItem* &newRoot - reference of the root pointer of tree to be constructed
*                       const GTATreeItem * originalRoot - root node of already existing tree
*/
void GTAViewController::cloneTree(GTATreeItem* &newRoot, const GTATreeItem * originalRoot)
{
	if (newRoot == NULL)
	{

		newRoot = new GTATreeItem(*originalRoot);
		newRoot->setParent(NULL);
	}
	if (originalRoot->hasChildren())
	{
		QList<GTATreeItem*> lstChildren = originalRoot->getChildren();
		for (int i = 0; i < lstChildren.count(); i++)
		{
			GTATreeItem *originalItem = lstChildren.at(i);
			GTATreeItem *newItem = new GTATreeItem(*originalItem);
			newRoot->addChild(newItem);
			cloneTree(newItem, originalItem);
		}
	}
}
void GTAViewController::updateFavoriteDataModel()
{
	if (_pFavDataModel != NULL)
	{
		delete _pFavDataModel;
		_pFavDataModel = NULL;
	}
}
GTATreeItem *GTAViewController::getFavRootTreeNode()
{
	return _pFavTreeRoot;
}
QSortFilterProxyModel* GTAViewController::getFavoriteBrowserDataModel(bool isFavItemRemoved)
{

	if (isFavItemRemoved && (_pFavDataModel != NULL))
	{
		delete _pFavDataModel;
		_pFavDataModel = NULL;
	}
	if (_pFavDataModel == NULL)
	{
		GTAAppController *appCtrl = GTAAppController::getGTAAppController();
		if (appCtrl)
		{
			//            GTATreeItem *favRootNode = appCtrl->getFavRootTreeNode();
			if (_pFavTreeRoot)
			{
				delete _pFavTreeRoot;
				_pFavTreeRoot = NULL;
			}

			GTATreeItem * rootNode = appCtrl->getRootTreeNode();
			if (rootNode)
			{
				cloneTree(_pFavTreeRoot, rootNode);
				_pFavTreeRoot->removeNonFavorite();
			}

			_pFavDataModel = new GTATreeViewModel(_pFavTreeRoot);

		}
	}
	QAbstractItemModel *model = dynamic_cast<QAbstractItemModel*> (_pFavDataModel);
	_pProxyModel = new GTADataElementFilterModel(model);
	return _pProxyModel;
}

/* This function returns the tree view model to set in the ui->DataTV
* If the treemodel is null tree model is created and then model is returned
* Else the existing model is returned
*
*/
QSortFilterProxyModel* GTAViewController::getTreeBrowserModel()
{
	if (_pTreeViewModel == NULL)
	{
		createTreeBrowserModel();
	}

    // QAbstractItemModel *model = dynamic_cast<QAbstractItemModel*> (_pTreeViewModel);
    _pProxyModel = new GTADataElementFilterModel(_pTreeViewModel);
    return _pProxyModel;

}

QStringList GTAViewController::getColumnList() const
{
	if (_pTreeViewModel != NULL)
	{
		if (_pTreeViewModel->getColumnList().isEmpty())
			return QStringList();
		else
			return _pTreeViewModel->getColumnList();
	}
	else
		return QStringList();
}

bool GTAViewController::createTreeBrowserModel()
{
	bool rc = true;

	if (_pTreeViewModel == NULL)
	{

		GTAAppController *appCtrl = GTAAppController::getGTAAppController();
		GTATreeItem * rootNode = appCtrl->getRootTreeNode();
		_pTreeViewModel = new GTATreeViewModel(rootNode);
	}

	return rc;
}
QStringList GTAViewController::getDocumentByExtension(const GTATreeItem *treeRootNode, const QString &iExtn, QHash<QString, QString> &docMap)
{
	QStringList documents;
	if (treeRootNode)
	{
		QString Extension = iExtn;
		Extension.remove("*.");
		QList<GTATreeItem*> lstChildren = treeRootNode->getChildren();
		for (int i = 0; i < lstChildren.count(); i++)
		{
			GTATreeItem * pItem = lstChildren.at(i);
			if (pItem)
			{
				QString ext = pItem->getExtension();

				if ((ext != "folder") && (ext == Extension))
				{
					QString absRelPath = pItem->getAbsoluteRelPath();
					QString name = pItem->getName();
					QString uuid = pItem->getUUID();
					if (absRelPath.contains("FSII_"))
					{
						QStringList path = absRelPath.split("/");
						path.removeLast();
						QString completeName = QString("%1.%2").arg(name, ext);
						path.append(completeName);
						absRelPath = QDir::cleanPath(path.join("/"));
					}
					docMap.insert(absRelPath, uuid);
					documents.append(absRelPath);
				}
				else
				{
					documents.append(getDocumentByExtension(pItem, Extension, docMap));
				}
			}
		}
	}
	return documents;
}


/* This function is used to update the treeModel
*  This function should be called when files have been saved imported, files deleted etc
*  to update the existing model in order to reflect new file structure
*/
void GTAViewController::updateTreeBrowserModel()
{
	if (_pTreeViewModel != NULL)
	{
		delete _pTreeViewModel;
		_pTreeViewModel = NULL;
	}
}
bool GTAViewController::createFavoriteDataModel(const QStringList&)
{
	//    bool rc = true;
	//    if (_pFavDataModel==NULL)
	//    {
	//        _pFavDataModel = new QStringListModel(this);
	//    }

	//    QStringList stringToSet =  favList;
	//    stringToSet.sort();
	//    _pFavDataModel->setStringList(stringToSet);

	return true;
}
bool GTAViewController::addFavorites(const QStringList&)
{
	//    bool rc = false;
	//    //    QStringList stringList;
	//    if (NULL!=_pFavDataModel)
	//    {
	//        QStringList stringList = _pFavDataModel->stringList();
	//        stringList.append(favList);
	//        stringList.removeDuplicates();
	//        stringList.sort();
	//        _pFavDataModel->setStringList(stringList);
	//        rc = true;
	//    }

	return true;
}
void GTAViewController::setFiltersOnFavorites(const QString&)
{

	//    if (NULL!=_pFavDataModel)
	//    {

	//        QStringList stringList = _pFavDataModel->stringList();

	//        QStringList newList;
	//        foreach(QString currentStr, stringList)
	//        {
	//            if (currentStr.contains(filter,Qt::CaseInsensitive))
	//                newList.append(currentStr);
	//        }
	//        newList.removeDuplicates();
	//        newList.sort();
	//        _pFavDataModel->setStringList(newList);
	//    }
}
void GTAViewController::setFiltersOnElemBrowser(const QStringList& filters, const int &iSearchType)
{

	QString filterString = filters.join("||");
	QRegExp regExp(filterString, Qt::CaseInsensitive, QRegExp::RegExp);

	if (NULL != _pProxyModel)
	{
		_pProxyModel->setSearchcolumn(iSearchType);
		_pProxyModel->setDynamicSortFilter(true);
		_pProxyModel->setFilterRegExp(regExp);
	}
}
bool GTAViewController::removeFavorites(const QStringList&)
{
	//    bool rc = false;
	//    QStringList stringList;
	//    if (NULL!=_pFavDataModel)
	//    {
	//        rc = true;
	//        QStringList stringList = _pFavDataModel->stringList();

	//        QStringList newList;
	//        foreach(QString currentStr, stringList)
	//        {
	//            if (!favList.contains(currentStr))
	//                newList.append(currentStr);
	//        }
	//        newList.removeDuplicates();
	//        newList.sort();
	//        _pFavDataModel->setStringList(newList);
	//    }
	return true;

}
QModelIndex GTAViewController::getElementDataBrowserIndexForDataDir(const QString&)
{
	//    _pTreeViewModel->index()
	return  QModelIndex();
	//    _pElemBrowserModel->index(iDataDir); //--IKM
}
void GTAViewController::establishConnectionWithModel()
{
	if (NULL != _pElementViewModel)
	{
		connect(_pElementViewModel, SIGNAL(callActionInserted(int&, QString&)), this, SLOT(onCallActionInserted(int&, QString&)));

	}


}

//Dev comment moved .. remove it from here
void GTAViewController::onCallActionInserted(int&, QString&)
{
	//    GTAAppController* pCtrl = GTAAppController::getGTAAppController();
	//    if (pCtrl)
	//    {
	//        pCtrl->callActionDropped(iRow,iFileName);
	//    }
	//    //emit callActionInserted(iRow,iFileName);
}
//bool GTAViewController::getClipboardModel(QAbstractItemModel*& pModel)
//{
//    bool rc = false;
//    int noOfCopiedItems = _lstCopiedElements.size();
//    if (noOfCopiedItems)
//    {
//        GTAElement* pElement = new GTAElement(GTAElement::CUSTOM);
//        int cmdCnt=0;
//        for (int i=0;i<noOfCopiedItems;i++)
//        {
//            GTACommandBase* pMasterCmd = _lstCopiedElements.at(i);
//            if (NULL!=pMasterCmd)
//            {
//                pElement->insertCommand(pMasterCmd,cmdCnt,false);
//                int childrenCnt = pMasterCmd->getAllChildrenCount();
//                if (childrenCnt)
//                    cmdCnt = cmdCnt+childrenCnt;

//                cmdCnt++;

//            }
//        }

//        if (pElement)
//        {
//            pModel = new GTAElementViewModel(pElement);
//            rc = true;

//        }
//    }
//    else
//        _LastError = "Noting copied[Clipboard Empty]";
//    return rc;
//}
void GTAViewController::setHiddenRows(const  QStringList& iRows)
{
	if (NULL != _pElement)
	{
		_pElement->setHiddenRows(iRows);
	}
}
QStringList GTAViewController::getHiddenRows()
{
	if (NULL != _pElement)
	{
		return _pElement->getHiddenRows();
	}

	return QStringList();
}



