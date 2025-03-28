#ifndef GTAUNDOREDOCLEARALL_H
#define GTAUNDOREDOCLEARALL_H

#include "GTAEditorWindow.h"
#include "GTAElement.h"

#pragma warning (push, 0)
#include <QUndoCommand>
#pragma warning (pop)

class GTAUndoRedoClearAll : public QUndoCommand
{
public:
    GTAUndoRedoClearAll(GTAElement *pElement,GTAEditorWindow *mainWindow, QUndoCommand * parent = 0);
    ~GTAUndoRedoClearAll(){}
    void undo();
    void redo();

private:

    GTAEditorWindow * _pMainWindow;
    GTAElement *_pElement;
    GTAElement *_pNewElement;
};

#endif // GTAUNDOREDOCLEARALL_H
