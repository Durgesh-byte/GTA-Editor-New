#pragma once

#include <qtreeview.h>
#include <QDropEvent>
#include <qpainter.h>


/**
* Custom class derived from QTreeView, which allows the definition of key methods, for instance for drag and drop.
*/
class GTATreeView : public QTreeView
{
    Q_OBJECT

public:
    GTATreeView(QWidget* parent = nullptr);

private:
    QModelIndexList _selectedIndexes;

protected:
    void dropEvent(QDropEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;

signals:
    void itemsDropped(QModelIndexList, QModelIndex);
};
