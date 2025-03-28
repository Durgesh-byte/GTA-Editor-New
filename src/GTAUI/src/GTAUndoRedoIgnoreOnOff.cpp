#include "GTAUndoRedoIgnoreOnOff.h"


GTAUndoRedoIgnoreOnOff::GTAUndoRedoIgnoreOnOff(GTACommandBase *iCommand, bool iOnOffVal,GTAEditorWindow *mainWindow,QUndoCommand *parent) :
    _pCmd(iCommand),_ignoreStatus(iOnOffVal),_pMainWindow(mainWindow),QUndoCommand(parent)
{

}

void GTAUndoRedoIgnoreOnOff::undo()
{
    _pCmd->setIgnoreInSCXML(!_ignoreStatus);
    _pMainWindow->updateEditorView();
}

void GTAUndoRedoIgnoreOnOff::redo()
{
     _pCmd->setIgnoreInSCXML(_ignoreStatus);
     _pMainWindow->updateEditorView();
}
