#ifndef GTAEDITORTREEMODEL_H
#define GTAEDITORTREEMODEL_H

#include "GTACommandBase.h"
#include "GTAElement.h"
#include "GTAViewModel.h"

#pragma warning(push, 0)
#include <QAbstractItemModel>
#include <QList>
#include <QModelIndex>
#include <QVariant>
#pragma warning(pop)

class GTAViewModel_SHARED_EXPORT GTAEditorTreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    
    /** This constructor creates custom element based on Action command used for tree view of
      * a command.The life cycle of created element will be controlled by this model.
      *@ parameter ipCmd: input command whose tree view is to be constructed using element model
      */
    explicit GTAEditorTreeModel(const GTACommandBase * ipCmd, QObject *parent = NULL);

    /** Constructor.
    * if element is custom ,life cycle of created element will be controlled by this model.
    * other wise does not delete.
    */
    explicit GTAEditorTreeModel(GTAElement * ipElement,QObject *parent = 0);


    virtual ~GTAEditorTreeModel();

    virtual int rowCount(const QModelIndex &parent) const;
    virtual int columnCount(const QModelIndex &parent) const;
    virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;
    virtual QModelIndex parent(const QModelIndex & index) const;
    virtual QVariant data(const QModelIndex &index, int role =Qt::DisplayRole) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    
signals:

public slots:
protected:

    QString getActionOnFail(GTACommandBase * pCmd) const;
    QString getTimeOut(GTACommandBase * pCmd) const;
    QString getPrecision(GTACommandBase * pCmd) const ;
    QString getComment(GTACommandBase * pCmd) const;
    QString getDumpList(GTACommandBase * pCmd) const;
    QStringList _ColumnList;
    GTAElement * _pElement;


};

#endif // GTAEDITORTREEMODEL_H
