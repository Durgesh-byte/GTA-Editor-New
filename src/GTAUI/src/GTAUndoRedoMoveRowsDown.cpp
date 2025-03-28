#include "GTAUndoRedoMoveRowsDown.h"

GTAUndoRedoMoveRowsDown::GTAUndoRedoMoveRowsDown(int selectedRow, GTAEditorWindow* mainWindow, QUndoCommand *parent)
    :_SelectedRow(selectedRow), _pMainWindow(mainWindow), QUndoCommand(parent)
{
}

void GTAUndoRedoMoveRowsDown::undo()
{
    QList<int> indexList;
    indexList.append(_pMainWindow->getRowToBeSelected());
    _pMainWindow->moveEditorRowUp(indexList);
}

void GTAUndoRedoMoveRowsDown::redo()
{

    QList<int> indexList;
    indexList.append(_SelectedRow);
    _pMainWindow->moveEditorRowDown(indexList);
}

