#ifndef GTAUNDOREDOCLEARROW_H
#define GTAUNDOREDOCLEARROW_H

#include "GTAEditorWindow.h"
#include "GTACommandBase.h"

#pragma warning (push, 0)
#include <QList>
#include <QUndoCommand>
#pragma warning (pop)

class GTAUndoRedoClearRow:public QUndoCommand
{
public:
    GTAUndoRedoClearRow(QList<int> selectedRows,QList<GTACommandBase*> cmdsCleared,GTAEditorWindow *mainWindow, QUndoCommand * parent = 0);
    ~GTAUndoRedoClearRow(){}
    void undo();
    void redo();
private:

    GTAEditorWindow * _pMainWindow;

    QList<int> _SelectedRows;
    QList<int> _UndoRows;
    QList<GTACommandBase*> _CmdsCleared;
};

#endif // GTAUNDOREDOCLEARROW_H
