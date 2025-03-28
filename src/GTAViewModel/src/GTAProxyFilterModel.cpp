#include "GTAProxyFilterModel.h"


GTAProxyFilterModel::GTAProxyFilterModel(QAbstractItemModel *model, QObject *parent):QSortFilterProxyModel(parent)
{
    setSourceModel(model);
    setDynamicSortFilter(true);
}

bool GTAProxyFilterModel::filterAcceptsRow(int ,const QModelIndex &) const
{
    return true;
}
void GTAProxyFilterModel::setSearchcolumns(QList<int> cols)
{
    _searchCols = cols;
}
void GTAProxyFilterModel::setSearchCriteria(QString critera)
{
    _searchCriteria = critera;
}
void GTAProxyFilterModel::setSearchStrings(QStringList texts)
{
    _texts = texts;
}

Qt::ItemFlags GTAProxyFilterModel::flags( const QModelIndex & index ) const
{

    Qt::ItemFlags itemFlags;
    itemFlags = QAbstractItemModel::flags(index);
    //itemFlags |=  Qt::ItemIsEditable;

    return Qt::ItemIsTristate|Qt::ItemIsUserCheckable|Qt::ItemIsDragEnabled|itemFlags;
}
Qt::DropActions GTAProxyFilterModel::supportedDragActions() const
{
     return Qt::CopyAction;
}
