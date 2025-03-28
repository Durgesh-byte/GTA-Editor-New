#ifndef GTAEDITORLOGMODEL_H
#define GTAEDITORLOGMODEL_H
#include "GTAEditorTreeModel.h"
#include "GTAViewModel.h"
class GTAViewModel_SHARED_EXPORT GTAEditorLogModel : public GTAEditorTreeModel
{
public:
    GTAEditorLogModel(GTAElement * ipElement,QObject *parent = 0);


    virtual int columnCount(const QModelIndex &) const;
    virtual QVariant data(const QModelIndex &index, int role =Qt::DisplayRole) const;
    
    QStringList getColumnNameList()const {return _ColumnList;}
    virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;

    bool setData( const QModelIndex & index, const QVariant & value, int role );
    Qt::ItemFlags flags ( const QModelIndex & index ) const;
    GTAElement * getDataModel() const;
    void updateElement(const GTAElement* iElement);
    void updateAllParents(const QModelIndex& );
    void updateAllChildrens(const QModelIndex& );

};

#endif // GTAEDITORLOGMODEL_H
