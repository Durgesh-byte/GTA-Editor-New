#include "GTAUndoRedoAddCmd.h"
#include "GTACommandVariant.h"

GTAUndoRedoAddCmd::GTAUndoRedoAddCmd(int selectedRow, QVariant cmd, QVariant previousCommand,GTAEditorWindow *mainWindow, QUndoCommand *parent)
    :QUndoCommand(parent),_pMainWindow(mainWindow), _SelectedRow(selectedRow), _Cmd(cmd), _PreviousInstance(previousCommand)
{

}

void GTAUndoRedoAddCmd::undo()
{
    QList<int> lstIndex;
    lstIndex.append(_SelectedRow);
    QList<int> list;
    if (_PreviousInstance != NULL)
    {
        GTACommandBase* pCmd = NULL;
        _pMainWindow->addCommand(_SelectedRow, _PreviousInstance);
        GTACommandVariant valCmdVariant = _PreviousInstance.value<GTACommandVariant>();
        pCmd = valCmdVariant.getCommand();
        if (pCmd)
        {
            GTACommandBase* pSaveCmd = pCmd->getClone();
            GTACommandVariant variant;
            variant.setCommand(pSaveCmd);
            _PreviousInstance.setValue(variant);
        }
    }    
    else
        _pMainWindow->clearRow(lstIndex, list);
}

void GTAUndoRedoAddCmd::redo()
{
    GTACommandBase* pCmd = NULL;
    _pMainWindow->addCommand(_SelectedRow,_Cmd);
    GTACommandVariant valCmdVariant = _Cmd.value<GTACommandVariant>();
    pCmd = valCmdVariant.getCommand();
    if(pCmd)
    {
        GTACommandBase* pSaveCmd = pCmd->getClone();
        GTACommandVariant variant;
        variant.setCommand(pSaveCmd);
        _Cmd.setValue(variant);
    }
}
