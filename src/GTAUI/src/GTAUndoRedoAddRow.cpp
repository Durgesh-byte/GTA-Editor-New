#include "GTAUndoRedoAddRow.h"


GTAUndoRedoAddRow::GTAUndoRedoAddRow(int selectedRowStatus, QList<int> selectedRows,GTAEditorWindow *mainWindow, QUndoCommand * parent):
    _SelectedRowStatus(selectedRowStatus), _SelectedRows(selectedRows),_pMainWindow(mainWindow),QUndoCommand(parent)
{
    _UndoRows.clear();
}


void GTAUndoRedoAddRow::undo()
{

      _pMainWindow->deleteRow(_SelectedRowStatus,_UndoRows);
}

void GTAUndoRedoAddRow::redo()
{

    _pMainWindow->addRow(_SelectedRowStatus,_SelectedRows);
    _UndoRows.clear();
    for(int i = 0; i < _SelectedRows.count();i++)
    {
        int rowId = _SelectedRows.at(i);
        int newRowId = rowId + i + 1;
        _UndoRows.insert(i,newRowId);
    }

}
