#ifndef GTAUNDOREDOHIDEROW_H
#define GTAUNDOREDOHIDEROW_H

#include "GTAEditorWindow.h"

#pragma warning (push, 0)
#include <QList>
#include <QUndoCommand>
#pragma warning (pop)

class GTAUndoRedoHideRow : public QUndoCommand
{
public:
    GTAUndoRedoHideRow(QList<int> selectedRows,GTAEditorWindow *mainWindow, QUndoCommand * parent = 0);
    ~GTAUndoRedoHideRow(){}
    void undo();
    void redo();

private:
    GTAEditorWindow * _pMainWindow;
    QList<int> _SelectedRows;
};

#endif // GTAUNDOREDOHIDEROW_H
