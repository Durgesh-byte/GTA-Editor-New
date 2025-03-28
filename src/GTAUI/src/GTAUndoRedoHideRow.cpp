#include "GTAUndoRedoHideRow.h"



GTAUndoRedoHideRow::GTAUndoRedoHideRow(QList<int> selectedRows,GTAEditorWindow *mainWindow, QUndoCommand * parent)
    :_SelectedRows(selectedRows), _pMainWindow(mainWindow),QUndoCommand(parent)
{
}

void GTAUndoRedoHideRow::undo()
{
    _pMainWindow->showRows(_SelectedRows);
}

void GTAUndoRedoHideRow::redo()
{
    _pMainWindow->HideRows(_SelectedRows);

}
