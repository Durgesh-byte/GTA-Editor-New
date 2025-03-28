#include "GTAUndoRedoShowAllRows.h"

GTAUndoRedoShowAllRows::GTAUndoRedoShowAllRows(int status, QList<int> selectedRows,GTAEditorWindow *mainWindow, QUndoCommand * parent)
    :_Status(status), _SelectedRows(selectedRows),_pMainWindow(mainWindow), QUndoCommand(parent)
{

}

void GTAUndoRedoShowAllRows::undo()
{
    _pMainWindow->HideRows(_SelectedRows);
}

void GTAUndoRedoShowAllRows::redo()
{
    _pMainWindow->showBetweenSelectedRows(_Status,_SelectedRows);
}
