#ifndef GTAUNDOREDODELETEROW_H
#define GTAUNDOREDODELETEROW_H

#include "GTAEditorWindow.h"
#include "GTACommandBase.h"

#pragma warning (push, 0)
#include <QList>
#include <QUndoCommand>
#pragma warning (pop)


class GTAUndoRedoDeleteRow : public QUndoCommand
{
public:
    GTAUndoRedoDeleteRow(int selectedRowStatus, QList<int> selectedRows,QList<GTACommandBase*> cmdsToBeDeleted,GTAEditorWindow *mainWindow, QUndoCommand * parent = 0);
    ~GTAUndoRedoDeleteRow(){}
    void undo();
    void redo();

private:
    GTAEditorWindow * _pMainWindow;
    int _SelectedRowStatus;
    QList<int> _SelectedRows;
//    QList<int> _UndoRows;
    QList<GTACommandBase*> _CmdsDeleted;
};

#endif // GTAUNDOREDODELETEROW_H
