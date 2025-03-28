#include "GTATreeView.h"

/**
* @brief Constructor. Directly sets the appropriate parametrization for the tree view.
* @param parent parent widget
*/
GTATreeView::GTATreeView(QWidget* parent)
{
    setAcceptDrops(true);
    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::InternalMove);
    setSortingEnabled(true);
}

/**
* @brief Takes the selected items and the pointed item and forwards them through a signal.
* @param event Current event (here it's the dropping over a certain item in the tree view).
*/
void GTATreeView::dropEvent(QDropEvent* event)
{
    QModelIndex target = this->indexAt(event->pos());
    emit itemsDropped(_selectedIndexes, target);
}

/**
* @brief Begins the drag and drop procedure; stores the current selection.
* @param event Current event
*/
void GTATreeView::dragEnterEvent(QDragEnterEvent* event)
{
    _selectedIndexes = this->selectionModel()->selectedRows(0);
    event->acceptProposedAction();
}

/**
* @brief Highlights the item currently hovered over.
* @param event Current event
*/
void GTATreeView::dragMoveEvent(QDragMoveEvent* event)
{
    QModelIndex current = this->indexAt(event->pos());
    this->selectionModel()->select(QItemSelection(current, current), QItemSelectionModel::ClearAndSelect);
    event->acceptProposedAction();
}