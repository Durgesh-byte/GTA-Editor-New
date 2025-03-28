#include "GTAUndoRedoToggleReadOnly.h"

GTAUndoRedoToggleReadOnly::GTAUndoRedoToggleReadOnly(GTACommandBase *iCommand, GTAEditorWindow *mainWindow, QUndoCommand *parent):
    _pCmd(iCommand), _pMainWindow(mainWindow), QUndoCommand(parent)

{
}

void GTAUndoRedoToggleReadOnly::undo()
{
    if(_pCmd)
    {
        _pCmd->setReadOnlyState(_ToggleState);
        _pMainWindow->updateEditorView();
    }
}

void GTAUndoRedoToggleReadOnly::redo()
{
    if(_pCmd)
    {
        toggleReadOnlyState();
        _pMainWindow->updateEditorView();
    }
}

void GTAUndoRedoToggleReadOnly::toggleReadOnlyState()
{
    _ToggleState = _pCmd->getReadOnlyState();
    _pCmd->setReadOnlyState(!_ToggleState);
}



