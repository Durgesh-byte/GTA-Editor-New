#ifndef GTAFILTERMODEL_H
#define GTAFILTERMODEL_H

#pragma warning (push, 0)
#include <QSortFilterProxyModel>
#include "QStringList"
#pragma warning (pop)

class GTAFilterModel : public QSortFilterProxyModel
{
public:
    GTAFilterModel(QAbstractItemModel *model, QObject *parent = 0);
    bool filterAcceptsRow(int sourceRow,const QModelIndex &sourceParent) const;
    void setSearchcolumns(QList<int> cols);
    void setSearchCriteria(QString critera);
    void setSearchStrings(QStringList texts);
	Qt::ItemFlags flags( const QModelIndex & index ) const;
private:
    QList<int> _searchCols;
    QStringList _texts;
    QString _searchCriteria;

};

#endif // GTAFILTERMODEL_H
