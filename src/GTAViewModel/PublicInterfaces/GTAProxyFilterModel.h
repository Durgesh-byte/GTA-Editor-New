#ifndef GTAPROXYFILTERMODEL_H
#define GTAPROXYFILTERMODEL_H

#pragma warning(push, 0)
#include <QObject>
#include <QString>
#include <QStringList>
#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#pragma warning(pop)


class GTAProxyFilterModel : public QSortFilterProxyModel
{

public:

    GTAProxyFilterModel(QAbstractItemModel *model, QObject *parent = 0);
    bool filterAcceptsRow(int ,const QModelIndex &) const;
    void setSearchcolumns(QList<int> cols);
    void setSearchCriteria(QString critera);
    void setSearchStrings(QStringList texts);
    Qt::ItemFlags flags( const QModelIndex & index ) const;
    Qt::DropActions supportedDragActions() const;
private:
    QList<int> _searchCols;
    QStringList _texts;
    QString _searchCriteria;


};

#endif // GTAPROXYFILTERMODEL_H
