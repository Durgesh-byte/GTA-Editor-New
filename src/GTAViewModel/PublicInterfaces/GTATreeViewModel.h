#ifndef GTATREEVIEWMODEL_H
#define GTATREEVIEWMODEL_H

#include "GTAViewModel.h"
#include "GTATreeItem.h"

#pragma warning(push, 0)
#include <QAbstractItemModel>
#include <QStandardItem>
#include <QVariant>
#include <QSortFilterProxyModel>
#pragma warning(pop)

class GTAViewModel_SHARED_EXPORT GTATreeViewModel : public QAbstractItemModel
{
public:
    GTATreeViewModel(QObject *parent = 0);
     GTATreeViewModel(GTATreeItem *iRootNode,QObject *parent = 0);
    ~GTATreeViewModel();
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    /**
      *This function populates header name in data browser
      #isection: Takes input of which section name need to be updated
      #role: depends on type of role it decided the type of action need to be performed i.e. display etc.
      @return: column name
    */
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QModelIndex index(int row, int column, const QModelIndex &parentIndex = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &iIndex) const;
    int rowCount(const QModelIndex &parentIndex = QModelIndex()) const;
    /**
      *This function returns column count of data browser
      *@return : column count of data browser
    */
    int columnCount(const QModelIndex & = QModelIndex()) const;
    GTATreeItem *nodeFromIndex(const QModelIndex &index) const;
    QString getAbsolutePathForIndex(const QModelIndex &index);


    QString getFileUUIDForIndex(const QModelIndex &index);
//    void setFavoriteFlag(bool iVal);
    Qt::DropActions supportedDragActions() const;

    /**
      * This function provides colums list in data browser
      * @return: Returns column list to be displayed in data browser
      */
    QStringList getColumnList() const;

	/**
	* This function returns the extension corressponding to the item pointed by index.
	* @return: QString containing the extension corressponding to the item pointed by index
	*/
	QString  getExtensionForIndex(const QModelIndex &index);

private:
    QPixmap assignIcon(GitStatus status) const;

//    bool _FavFlag;
    GTATreeItem *_RootNode;
	QStringList _ColumnList; //It contains all columns of document viewer

};

#endif // GTATREEVIEWMODEL_H
