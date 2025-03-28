#include "GTAUndoRedoMoveRowsUp.h"

GTAUndoRedoMoveRowsUp::GTAUndoRedoMoveRowsUp(int selectedRow, GTAEditorWindow* mainWindow, QUndoCommand *parent)
    :_SelectedRow(selectedRow), _pMainWindow(mainWindow),QUndoCommand(parent)

{

}

void GTAUndoRedoMoveRowsUp::undo()
{
    QList<int> indexList;
    indexList.append(_pMainWindow->getRowToBeSelected());
    _pMainWindow->moveEditorRowDown(indexList);
}

void GTAUndoRedoMoveRowsUp::redo()
{
    QList<int> indexList;
    indexList.append(_SelectedRow);
    _pMainWindow->moveEditorRowUp(indexList);
}
