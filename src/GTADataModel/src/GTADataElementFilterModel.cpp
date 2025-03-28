#include "GTADataElementFilterModel.h"

bool GTADataElementFilterModel::filterAcceptsRow(int sourceRow,
         const QModelIndex &sourceParent) const
 {
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    QModelIndex mainindex = sourceModel()->index(sourceRow, SearchColumnIdx, sourceParent);
    if (!index.isValid())
            return false;

    if (mainindex.data().toString().contains(filterRegExp()))
            return true;

    int rows = sourceModel()->rowCount(index);
    for (int row = 0; row < rows; row++)
            if (filterAcceptsRow(row, index))
                    return true;

    return false;


 }

void GTADataElementFilterModel::setSearchcolumn(int col)
{
    SearchColumnIdx = col;
}



GTADataElementFilterModel::GTADataElementFilterModel(QAbstractItemModel *model, QObject *parent)
     : QSortFilterProxyModel(parent)
 {
    setSourceModel(model);
    setDynamicSortFilter(true);
 }
