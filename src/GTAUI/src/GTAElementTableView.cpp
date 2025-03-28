#include "GTAElementTableView.h"

GTAElementTableView::GTAElementTableView(QWidget * parent  ):QTableView(parent)
{

}

GTAElementTableView::~GTAElementTableView()
{

}
void GTAElementTableView::	selectionChanged ( const QItemSelection & selected, const QItemSelection & deselected )
{
    emit selectionChangedForEdior();
    QTableView::selectionChanged (  selected,  deselected );

}
