#include "GTAUndoRedoDeleteRow.h"

GTAUndoRedoDeleteRow::GTAUndoRedoDeleteRow(int selectedRowStatus, QList<int> selectedRows,QList<GTACommandBase*> cmdsToBeDeleted,GTAEditorWindow *mainWindow, QUndoCommand * parent)
    :_SelectedRowStatus(selectedRowStatus), _SelectedRows(selectedRows),_CmdsDeleted(cmdsToBeDeleted), _pMainWindow(mainWindow), QUndoCommand(parent)

{
//    _UndoRows.clear();
}

void GTAUndoRedoDeleteRow::undo()
{
   _pMainWindow->insertCommands(_SelectedRows,_CmdsDeleted,true);
}

void GTAUndoRedoDeleteRow::redo()
{
    _pMainWindow->deleteRow(_SelectedRowStatus,_SelectedRows);
}
