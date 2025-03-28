#ifndef GTAUNDOREDOEDITMTE_H
#define GTAUNDOREDOEDITMTE_H

#include "GTAEditorWindow.h"
#include "GTACommandBase.h"

#pragma warning (push, 0)
#include <QString>
#include <QUndoCommand>
#pragma warning (pop)

class GTAUndoRedoEditMTE : public QUndoCommand
{
public:
    GTAUndoRedoEditMTE(QString oValue, QString nValue, GTAEditorWindow* mainWindow, QUndoCommand* parent = 0);
    ~GTAUndoRedoEditMTE() {}

    void undo();
    void redo();

private:
    GTAEditorWindow* _pMainWindow;
    QString _oldValue;
    QString _newValue;
};

#endif // GTAUNDOREDOEDITMTE_H