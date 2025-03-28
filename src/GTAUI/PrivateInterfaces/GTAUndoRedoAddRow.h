#ifndef GTAUNDOREDOADDROW_H
#define GTAUNDOREDOADDROW_H

#include "GTAEditorWindow.h"
#include "GTACommandBase.h"

#pragma warning (push, 0)
#include <QList>
#include <QUndoCommand>
#pragma warning (pop)

class GTAUndoRedoAddRow : public QUndoCommand
{
public:
    GTAUndoRedoAddRow(int selectedRowStatus, QList<int> selectedRows,GTAEditorWindow *mainWindow, QUndoCommand * parent = 0);
    ~GTAUndoRedoAddRow(){}

    void undo();
    void redo();

private:
    GTAEditorWindow * _pMainWindow;
    int _SelectedRowStatus;
    QList<int> _SelectedRows;
    QList<int> _UndoRows;

};

#endif // GTAUNDOREDOADDROW_H
