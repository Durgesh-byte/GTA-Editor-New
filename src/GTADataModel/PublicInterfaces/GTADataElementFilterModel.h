#ifndef GTADataElementFilterModel_H
#define GTADataElementFilterModel_H
#include "GTADataModel.h"

#pragma warning(push, 0)
#include <QSortFilterProxyModel>
#pragma warning(pop)

class GTADataModel_SHARED_EXPORT GTADataElementFilterModel : public QSortFilterProxyModel
{
public:
    GTADataElementFilterModel(QAbstractItemModel *model, QObject *parent = 0);
    bool filterAcceptsRow(int sourceRow,const QModelIndex &sourceParent) const;
    void setSearchcolumn(int col);
private:
    int SearchColumnIdx;
};

#endif // GTADataElementFilterModel_H
