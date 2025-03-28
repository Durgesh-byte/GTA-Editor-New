#ifndef GTAUNDOREDOPASTE_H
#define GTAUNDOREDOPASTE_H

#include "GTAEditorWindow.h"
#include "GTACommandBase.h"

#pragma warning (push, 0)
#include <QList>
#include <QHash>
#include <QUndoCommand>
#pragma warning (pop)

class GTAUndoRedoPaste :public QUndoCommand
{
public:

    GTAUndoRedoPaste(int selectedRowStatus, QList<int> selectedRows,QList<GTACommandBase*> cmdsPasted,GTAEditorWindow *mainWindow, QUndoCommand * parent = 0);
    ~GTAUndoRedoPaste(){}
    void undo();
    void redo();

private:
    GTAEditorWindow * _pMainWindow;
    int _SelectedRowStatus;
    QList<int> _SelectedRows;
    QList<GTACommandBase*> _CmdsPasted;
    bool _PasteStatus;

};

#endif // GTAUNDOREDOPASTE_H
