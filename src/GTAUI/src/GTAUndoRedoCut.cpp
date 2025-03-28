#include "GTAUndoRedoCut.h"

GTAUndoRedoCut::GTAUndoRedoCut(int selectedRowStatus, QList<int> selectedRows,QHash<int,GTACommandBase*> cmdsCut,GTAEditorWindow *mainWindow, QUndoCommand * parent)
    :_SelectedRowStatus(selectedRowStatus),_SelectedRows(selectedRows),_CmdsCut(cmdsCut) ,_pMainWindow(mainWindow),QUndoCommand(parent)
{
}

void GTAUndoRedoCut::undo()
{
    QList<GTACommandBase*> cmdsCut = _CmdsCut.values();
     _pMainWindow->insertCommands(_SelectedRows,cmdsCut,true);
}

void GTAUndoRedoCut::redo()
{
    _pMainWindow->cutRow(_SelectedRowStatus,_SelectedRows);
    QList<int> keys = _CmdsCut.keys();

    for(int i = 0; i < keys.count(); i++)
    {
        int rowId = keys.at(i);

        if(!_SelectedRows.contains(rowId))
        {
            _CmdsCut.remove(rowId);
        }
    }
}
