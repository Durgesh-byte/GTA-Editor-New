#include "GTAUndoRedoEditMTE.h"

/**
 * @brief Constructor for the Undo/Redo on Maximum Time Estimation edition
 * @param oValue : old value
 * @param nValue : new value
 * @param mainWindow : current editor window
 * @param parent 
*/
GTAUndoRedoEditMTE::GTAUndoRedoEditMTE(QString oValue, QString nValue, GTAEditorWindow* mainWindow, QUndoCommand* parent) :
   _oldValue(oValue), _newValue(nValue), _pMainWindow(mainWindow), QUndoCommand(parent)
{
}

/**
 * @brief Undo the change on Maximum Time Estimation edition
*/
void GTAUndoRedoEditMTE::undo()
{

    _pMainWindow->setMaxTimeEstimated(_oldValue);
}

/**
 * @brief Redo the change on Maximum Time Estimation edition
*/
void GTAUndoRedoEditMTE::redo()
{
    _pMainWindow->setMaxTimeEstimated(_newValue);
}