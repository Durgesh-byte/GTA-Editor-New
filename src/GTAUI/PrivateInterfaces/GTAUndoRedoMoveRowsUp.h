#ifndef GTAUNDOREDOMOVEROWSUP_H
#define GTAUNDOREDOMOVEROWSUP_H

#include "GTAEditorWindow.h"
#include "GTACommandBase.h"

#pragma warning (push, 0)
#include <QList>
#include <QUndoCommand>
#pragma warning (pop)

class GTAUndoRedoMoveRowsUp : public QUndoCommand
{
public:
    GTAUndoRedoMoveRowsUp(int selectedRow, GTAEditorWindow* mainWindow, QUndoCommand *parent = 0);
    void undo();
    void redo();

private:
    int _SelectedRow;
    GTAEditorWindow *_pMainWindow;
};

#endif // GTAUNDOREDOMOVEROWSUP_H
