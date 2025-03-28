#ifndef GTAHEADERTABLEMODEL_H_
#define GTAHEADERTABLEMODEL_H_

#include "GTAHeader.h"
#include "GTAViewModel.h"

#pragma warning(push, 0)
#include <QAbstractTableModel>
#pragma warning(pop)


struct headerNodeInfo
{
   QString _name,_val,_description;
   bool _Mandatory, _showInLTRA;
};
 Q_DECLARE_METATYPE(headerNodeInfo)
class GTAViewModel_SHARED_EXPORT GTAHeaderTableModel:public QAbstractTableModel
{
	Q_OBJECT 
public:
    
    explicit GTAHeaderTableModel (GTAHeader* ipHeader, QObject* iParent=0);
	~GTAHeaderTableModel();

    //rowCount(), columnCount(), and data()



    int rowCount(const QModelIndex & = QModelIndex()) const ;
    int columnCount(const QModelIndex & = QModelIndex() )const ;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
	bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
    Qt::ItemFlags flags ( const QModelIndex & index ) const;
    bool insertRows( int row, int count, const QModelIndex & parent);
    bool removeRows( int row, int count, const QModelIndex & parent);
    bool showICD(const bool& );

    void setFileInfo(QList<GTAFILEInfo> iListFileInfo);
    void removeFileInfo();

    void SelectAllForExport();
    void UnSelectAllForExport();
private:
	
    GTAHeader* _pHeader;
    QList<GTAFILEInfo> _pFileInfo;

    bool showLTRAStateChangeAllowed(QString strFieldName) const;
};

#endif
