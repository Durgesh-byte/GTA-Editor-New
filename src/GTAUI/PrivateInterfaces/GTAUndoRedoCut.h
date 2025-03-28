#ifndef GTAUNDOREDOCUT_H
#define GTAUNDOREDOCUT_H

#include "GTAEditorWindow.h"
#include "GTACommandBase.h"

#pragma warning (push, 0)
#include <QList>
#include <QUndoCommand>
#pragma warning (pop)


class GTAUndoRedoCut :public QUndoCommand
{
public:
    GTAUndoRedoCut(int selectedRowStatus, QList<int> selectedRows,QHash<int,GTACommandBase*> cmdsCut,GTAEditorWindow *mainWindow, QUndoCommand * parent = 0);
    ~GTAUndoRedoCut(){}
    void undo();
    void redo();

private:
    GTAEditorWindow * _pMainWindow;
    int _SelectedRowStatus;
    QList<int> _SelectedRows;
//    QList<int> _UndoRows;
    QHash<int,GTACommandBase*> _CmdsCut;

};

#endif // GTAUNDOREDOCUT_H
