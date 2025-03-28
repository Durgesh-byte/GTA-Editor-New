#ifndef GTAUNDOREDOCOLLAPSETOTITLE_H
#define GTAUNDOREDOCOLLAPSETOTITLE_H

#include "GTAEditorWindow.h"

#pragma warning (push, 0)
#include <QUndoCommand>
#pragma warning (pop)

class GTAUndoRedoCollapseToTitle : public QUndoCommand
{
public:
    GTAUndoRedoCollapseToTitle(bool collaspeToggle,GTAEditorWindow *mainWindow, QUndoCommand *parent = 0);
    ~GTAUndoRedoCollapseToTitle(){}

    void undo();
    void redo();

private:
    GTAEditorWindow *_pMainWindow;
    bool _CollapseToggle;
};

#endif // GTAUNDOREDOCOLLAPSETOTITLE_H
