#ifndef GTAUNDOREDOMOVEROWSDOWN_H
#define GTAUNDOREDOMOVEROWSDOWN_H

#include "GTAEditorWindow.h"
#include "GTACommandBase.h"

#pragma warning (push, 0)
#include <QList>
#include <QUndoCommand>
#pragma warning (pop)

class GTAUndoRedoMoveRowsDown : public QUndoCommand
{
public:
    GTAUndoRedoMoveRowsDown(int selectedRow, GTAEditorWindow* mainWindow, QUndoCommand *parent = 0);
    void undo();
    void redo();
private:
    int _SelectedRow;
    GTAEditorWindow *_pMainWindow;
};

#endif // GTAUNDOREDOMOVEROWSDOWN_H
