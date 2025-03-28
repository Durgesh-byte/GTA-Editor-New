#ifndef GTATITLEBASEDEXECUTIONMODEL_H
#define GTATITLEBASEDEXECUTIONMODEL_H
#include "GTAEditorTreeModel.h"
#include "GTAViewModel.h"
#include "GTACommandBase.h"

class GTAViewModel_SHARED_EXPORT GTATitleBasedExecutionModel : public GTAEditorTreeModel
{
public:
    GTATitleBasedExecutionModel(GTAElement *pElement, QObject *pParent = 0);

    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags ( const QModelIndex & index ) const;
    GTACommandBase * nodeFromIndex(const QModelIndex &index) const;

    void expandAll(bool isExpand);
    void selectAll(bool isSelectAll);

    GTAElement * getElement()const;


    void LayoutAboutToBeChanged();
    void LayoutChanged();

};

#endif // GTATITLEBASEDEXECUTIONMODEL_H
