#include "GTAUndoRedoClearAll.h"

GTAUndoRedoClearAll::GTAUndoRedoClearAll(GTAElement *pElement,GTAEditorWindow *mainWindow, QUndoCommand * parent)
    : _pElement(pElement),_pMainWindow(mainWindow),QUndoCommand(parent)
{
    _pNewElement = NULL;
}

void GTAUndoRedoClearAll::undo()
{
    
    if(_pMainWindow != NULL && _pNewElement != NULL)
    {
        _pElement = _pNewElement;
        _pMainWindow->undoClearAll(_pNewElement);
    }
}

void GTAUndoRedoClearAll::redo()
{

    if(_pMainWindow != NULL && _pElement != NULL)
    {

        GTAHeader *pElemHeader = _pElement->getHeader();
        GTAHeader *pHeader = NULL;
        if(pElemHeader != NULL)
            pHeader = new GTAHeader(*pElemHeader);
   

        _pNewElement = new GTAElement(*_pElement);
        _pNewElement->setHeader(pHeader);
        _pElement = NULL;

        _pMainWindow->clearAll();
    }
}
