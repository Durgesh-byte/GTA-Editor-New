#ifndef GTAVALIDATIONWIDGETMODEL_H
#define GTAVALIDATIONWIDGETMODEL_H

#include "GTAEditorTreeModel.h"
#include "GTAViewModel.h"
#include "GTACommandBase.h"
#include "GTAActionExpandedCall.h"
#include "GTAActionCall.h"

#pragma warning(push, 0)
#include <QDebug>
#pragma warning(pop)

class GTAViewModel_SHARED_EXPORT GTAValidationWidgetModel : public GTAEditorTreeModel
{
public:
    GTAValidationWidgetModel(GTAElement *pElement, QObject *pParent = 0);
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags ( const QModelIndex & index ) const;
    GTACommandBase * nodeFromIndex(const QModelIndex &index) const;

    void expandAll(bool isExpand);
    void selectAll(bool isSelectAll);

    QStringList getElementUUID()const;
    GTAElement * getElement()const;
    static QString getExtensionForElmntType(GTAElement::ElemType elemType);

    void LayoutAboutToBeChanged();
    void LayoutChanged();

};

#endif // GTAVALIDATIONWIDGETMODEL_H
