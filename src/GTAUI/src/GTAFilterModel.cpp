#include "GTAFilterModel.h"

bool GTAFilterModel::filterAcceptsRow(int sourceRow,
                                         const QModelIndex &sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    if (!index.isValid())
        return false;

    if(_texts.count() == 2)
    {
        bool rc = false;
        if(_searchCriteria == "NOT")
        {
            for(int i=0;i<_searchCols.count();i++)
            {
                QString value = sourceModel()->index(sourceRow, _searchCols.at(i), sourceParent).data().toString();
                if(!value.isEmpty() && value.contains(_texts.at(0),Qt::CaseInsensitive)  && (!(value.contains(_texts.at(1),Qt::CaseInsensitive))))
                {
                    rc= true;
                }
                else
                {
                    int rows = sourceModel()->rowCount(index);
                    for (int row = 0; row < rows; row++)
                        if (filterAcceptsRow(row, index))
                            return true;
                }

            }
        }
        else if(_searchCriteria == "AND")
        {
            for(int i=0;i<_searchCols.count();i++)
            {
                QString value = sourceModel()->index(sourceRow, _searchCols.at(i), sourceParent).data().toString();
                if(!value.isEmpty() && value.contains(_texts.at(0),Qt::CaseInsensitive)  && (value.contains(_texts.at(1),Qt::CaseInsensitive)))
                {
                    rc=  true;
                }
                else
                {
                    int rows = sourceModel()->rowCount(index);
                    for (int row = 0; row < rows; row++)
                        if (filterAcceptsRow(row, index))
                            return true;
                }
            }
        }
        else if(_searchCriteria == "OR")
        {
            for(int i=0;i<_searchCols.count();i++)
            {
                QString value = sourceModel()->index(sourceRow, _searchCols.at(i), sourceParent).data().toString();
                if(!value.isEmpty() && (value.contains(_texts.at(0),Qt::CaseInsensitive)  || (value.contains(_texts.at(1),Qt::CaseInsensitive))))
                {
                    rc =  true;
                }
                else
                {
                    int rows = sourceModel()->rowCount(index);
                    for (int row = 0; row < rows; row++)
                        if (filterAcceptsRow(row, index))
                            return true;
                }
            }
        }
        return rc;
    }
    else if(_texts.count() == 1)
    {
        bool rc = false;
        for(int i=0;i<_searchCols.count();i++)
        {
            QString value = (sourceModel()->index(sourceRow, _searchCols.at(i), sourceParent)).data().toString();
            if(!value.isNull() && value.contains(_texts.at(0),Qt::CaseInsensitive))
            {
                rc  = true;
            }
            else
            {
                int rows = sourceModel()->rowCount(index);
                for (int row = 0; row < rows; row++)
                    if (filterAcceptsRow(row, index))
                        return true;
            }
        }
        return rc;
    }
    else
    {
        QString value = (sourceModel()->index(sourceRow, 0, sourceParent)).data().toString();
        if(!value.isEmpty() && value.contains(filterRegExp()))
        {
            return true;
        }
        else
        {
            int rows = sourceModel()->rowCount(index);
            for (int row = 0; row < rows; row++)
                if (filterAcceptsRow(row, index))
                    return true;
        }
    }
    int rows = sourceModel()->rowCount(index);
    for (int row = 0; row < rows; row++)
        if (filterAcceptsRow(row, index))
            return true;

    return false;
}

void GTAFilterModel::setSearchcolumns(QList<int> cols)
{
    _searchCols = cols;
}

void GTAFilterModel::setSearchCriteria(QString critera)
{
    _searchCriteria = critera;
}

void GTAFilterModel::setSearchStrings(QStringList texts)
{
    _texts = texts;
}

GTAFilterModel::GTAFilterModel(QAbstractItemModel *model, QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setSourceModel(model);
    setDynamicSortFilter(true);
}

Qt::ItemFlags GTAFilterModel::flags( const QModelIndex & index ) const
{

    Qt::ItemFlags itemFlags;
    itemFlags = QAbstractItemModel::flags(index);
    //itemFlags |=  Qt::ItemIsEditable;

    return Qt::ItemIsTristate|Qt::ItemIsUserCheckable|itemFlags;
}

