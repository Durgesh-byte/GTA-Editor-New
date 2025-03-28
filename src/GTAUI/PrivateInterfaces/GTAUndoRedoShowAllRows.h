#ifndef GTAUNDOREDOSHOWALLROWS_H
#define GTAUNDOREDOSHOWALLROWS_H

#include "GTAEditorWindow.h"

#pragma warning (push, 0)
#include <QList>
#include <QUndoCommand>
#pragma warning (pop)

class GTAUndoRedoShowAllRows :public QUndoCommand
{
public:
    GTAUndoRedoShowAllRows(int status, QList<int> selectedRows,GTAEditorWindow *mainWindow, QUndoCommand * parent = 0);
    ~GTAUndoRedoShowAllRows(){}

    void undo();
    void redo();

private:
    GTAEditorWindow * _pMainWindow;
    QList<int> _SelectedRows;
    int _Status;
};

#endif // GTAUNDOREDOSHOWALLROWS_H
