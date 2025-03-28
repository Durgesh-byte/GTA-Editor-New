#include "GTAUndoRedoCollapseToTitle.h"

GTAUndoRedoCollapseToTitle::GTAUndoRedoCollapseToTitle(bool collapseToggle, GTAEditorWindow *mainWindow, QUndoCommand *parent)
    :_CollapseToggle(collapseToggle),_pMainWindow(mainWindow), QUndoCommand(parent)
{
  //  _pMainWindow->collapseAllToTitle(!_CollapseToggle);
}

void GTAUndoRedoCollapseToTitle::undo()
{
    _pMainWindow->collapseAllToTitle(_CollapseToggle);
}

void GTAUndoRedoCollapseToTitle::redo()
{
     _pMainWindow->collapseAllToTitle(!_CollapseToggle);
}

