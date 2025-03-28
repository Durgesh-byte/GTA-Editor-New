#include "GTAUndoRedoPaste.h"


GTAUndoRedoPaste::GTAUndoRedoPaste(int selectedRowStatus, QList<int> selectedRows, QList<GTACommandBase*> cmdsPasted, GTAEditorWindow *mainWindow, QUndoCommand * parent)
	:_SelectedRowStatus(selectedRowStatus), _SelectedRows(selectedRows), _CmdsPasted(cmdsPasted), _pMainWindow(mainWindow), QUndoCommand(parent)
{
	_PasteStatus = true;
}

void GTAUndoRedoPaste::undo()
{
	if (_PasteStatus)
	{

		int selectedRow = _SelectedRows.at(0);
		int numOfCmdsPasted = _CmdsPasted.count();
		QList <int> undoRows;
		for (int i = selectedRow; i < (numOfCmdsPasted + selectedRow); i++)
		{
			undoRows.append(i);
		}
		_pMainWindow->clearRow(undoRows, _SelectedRows);
		
	}

}

void GTAUndoRedoPaste::redo()
{
	_pMainWindow->pasteRow(_SelectedRows, _PasteStatus, _CmdsPasted);
	QList<GTACommandBase*> tempList;
	for (int i = 0; i < _CmdsPasted.count(); i++)
	{
		GTACommandBase * pCmd = NULL;
		GTACommandBase * pCmdPasted = _CmdsPasted.at(i);
		if (pCmdPasted != NULL)
		{
			pCmd = pCmdPasted->getClone();
		}
		tempList.append(pCmd);

	}
	_CmdsPasted.clear();
	_CmdsPasted.append(tempList);
}

