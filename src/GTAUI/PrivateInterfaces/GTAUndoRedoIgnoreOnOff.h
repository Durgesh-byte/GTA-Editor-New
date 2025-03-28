#ifndef GTAUNDOREDOIGNOREONOFF_H
#define GTAUNDOREDOIGNOREONOFF_H

#include "GTACommandBase.h"
#include "GTAEditorWindow.h"

#pragma warning (push, 0)
#include <QUndoCommand>
#pragma warning (pop)

class GTAUndoRedoIgnoreOnOff : public QUndoCommand
{
 //     Q_OBJECT
public:
    GTAUndoRedoIgnoreOnOff(GTACommandBase *iCommand, bool iOnOffVal,GTAEditorWindow *mainWindow,QUndoCommand *parent = 0);
    ~GTAUndoRedoIgnoreOnOff(){}

    void undo();
    void redo();

private:
    GTACommandBase *_pCmd;
    bool _ignoreStatus;
    GTAEditorWindow *_pMainWindow;

};

#endif // GTAUNDOREDOIGNOREONOFF_H
