#include "GTAUndoRedoClearRow.h"

GTAUndoRedoClearRow::GTAUndoRedoClearRow(QList<int> selectedRows,QList<GTACommandBase*> cmdsCleared,GTAEditorWindow *mainWindow, QUndoCommand * parent)
    :_SelectedRows(selectedRows),_CmdsCleared(cmdsCleared), _pMainWindow(mainWindow), QUndoCommand(parent)

{

}
void GTAUndoRedoClearRow::undo()
{
    _pMainWindow->undoClearRow(_SelectedRows,_CmdsCleared);
}

void GTAUndoRedoClearRow::redo()
{
    QList<int> invalidIndexes;
    QMap<int, GTACommandBase*> orderMap;

    for(int i = 0,j = 0; i < _SelectedRows.count() && j < _CmdsCleared.count(); i++,j++)
    {
        orderMap.insert(_SelectedRows.at(i),_CmdsCleared.at(j));
    }

    _SelectedRows.clear();
    _SelectedRows = orderMap.keys();
    _CmdsCleared.clear();
    _CmdsCleared = orderMap.values();

    _pMainWindow->clearRow(_SelectedRows,invalidIndexes);
    for(int i = 0; i < invalidIndexes.count(); i++)
    {
        int invalidIndex = invalidIndexes.at(i);
        if(_SelectedRows.contains(invalidIndex))
        {
            int index = _SelectedRows.indexOf(invalidIndex);
            _SelectedRows.removeAt(index);
            _CmdsCleared.removeAt(index);

        }
    }
}
