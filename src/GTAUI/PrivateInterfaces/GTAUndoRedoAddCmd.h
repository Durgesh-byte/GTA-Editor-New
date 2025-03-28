#ifndef GTAUNDOREDOADDCMD_H
#define GTAUNDOREDOADDCMD_H

#include "GTAEditorWindow.h"
#include "GTACommandBase.h"
#include "GTACommandVariant.h"

#pragma warning (push, 0)
#include <QVariant>
#include <QUndoCommand>
#pragma warning (pop)

class GTAUndoRedoAddCmd : public QUndoCommand
{
public:
    GTAUndoRedoAddCmd(int selectedRow, QVariant cmd, QVariant previousCommand, GTAEditorWindow *mainWindow, QUndoCommand *parent = 0);
    void undo();
    void redo();

private:

    GTAEditorWindow * _pMainWindow;
    QVariant _Cmd;
    QVariant _PreviousInstance;
    int _SelectedRow;

};

#endif // GTAUNDOREDOADDCMD_H
