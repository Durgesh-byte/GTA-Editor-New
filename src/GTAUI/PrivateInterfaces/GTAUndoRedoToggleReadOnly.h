#ifndef GTAUNDOREDOTOGGLEREADONLY_H
#define GTAUNDOREDOTOGGLEREADONLY_H

#pragma warning (push, 0)
#include "GTACommandBase.h"
#include "GTAEditorWindow.h"
#include <QUndoCommand>
#pragma warning (pop)


class GTAUndoRedoToggleReadOnly:public QUndoCommand
{

public:
    GTAUndoRedoToggleReadOnly(GTACommandBase *iCommand, GTAEditorWindow *mainWindow, QUndoCommand *parent = 0);
    ~GTAUndoRedoToggleReadOnly(){}
    void undo();
    void redo();

private:
    GTACommandBase *_pCmd;
    GTAEditorWindow *_pMainWindow;
    bool _ToggleState;
    void toggleReadOnlyState();

};

#endif // GTAUNDOREDOTOGGLEREADONLY_H
