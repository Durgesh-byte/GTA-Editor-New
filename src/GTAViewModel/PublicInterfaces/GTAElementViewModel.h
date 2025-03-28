#ifndef GTAELEMENTVIEWMODEL_H
#define GTAELEMENTVIEWMODEL_H

#include "GTAElement.h"
#include "GTAViewModel.h"

#pragma warning(push, 0)
#include <QAbstractItemModel>
#include <QMimeData>
#pragma warning(pop)

class GTAViewModel_SHARED_EXPORT GTAElementViewModel : public QAbstractTableModel
{

    Q_OBJECT

public:

    enum DisplayContext{EDITING,COMPATIBILITY_CHK,SEARCH_REPLACE,DEBUG,LIVE,UNKNOWN};
    enum ColumnIndex{BREAKPOINT_INDEX,ACTION_INDEX,TYPE_INDEX,COMMENT_INDEX,TIMEOUT_INDEX,PRECISION_INDEX,ACTION_ON_FAIL_INDEX,DUMP_LIST_INDEX};
    explicit GTAElementViewModel(GTAElement *& pModel, QObject *parent = 0);
    ~GTAElementViewModel();
    

    int rowCount(const QModelIndex & = QModelIndex()) const ;
    int columnCount(const QModelIndex & = QModelIndex() )const ;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    QModelIndex parent( const QModelIndex & );
    QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    bool setData ( const QModelIndex & index, const QVariant & value, int  = Qt::EditRole );
    Qt::ItemFlags flags ( const QModelIndex & index ) const;
    bool insertRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
    bool removeRows( int row, int count, const QModelIndex & parent = QModelIndex() );
    void getDisplayDataMap(const QModelIndex &iIndex,QMap<int, QString>& ioColumnMapper) const;

    bool updateChanges();
    bool prependRow();


    void clearAllinEditor();
    QString getLastError(){return _lastError;}
    void setDisplayContext(DisplayContext iContext);
    GTAElementViewModel::DisplayContext getDisplayContext()const;
    void findAndHighlight(const QRegExp& searchString,QList<int>& iSelectedRows,QHash<int,QString>& ofoundRows,bool ibSearchUp,bool ibTextSearch,bool isSingleSearch = false,bool isHighlight = true);
    bool replaceString(const int& row,const QRegExp& strToReplace,const QString& sReplaceWith,bool);
    bool replaceAllString(const QRegExp& searchString,const QString& sReplace);
    bool replaceAllEquipment(const QRegExp& searchString, const QString& sReplace);
    bool highLightLastChild(const QModelIndex& ipIndex);
    QStringList GTAElementViewModel::mimeTypes() const;
    Qt::DropActions GTAElementViewModel::supportedDropActions() const;
    bool dropMimeData(const QMimeData *data,Qt::DropAction action, int row, int column, const QModelIndex &parent);
    QMimeData *mimeData(const QModelIndexList &indexes) const;

    void highlightRow(const int &iRowId, QModelIndex &iHighlightedRow, GTAElementViewModel::DisplayContext iDisplayContext);

    //-----------------Indentation changes ----------------------------

//    void getDisplayDataMap(const QModelIndex &iIndex,QMap<int, QString>& ioColumnMapper) const;

    void setIndentedView(bool indentedView);
    bool getIndentedView() const;
    int blockCheck(int *prow)const;

    //-----------------Indentation changes -----------------------------

signals:
    void callActionInserted(int& row, QString& iFileName);

public slots:

private:
    GTAElement * _pElementModel;
    QMap<int,QString> _ColumnNameMap;
    QString _lastError;
    DisplayContext _currentContext;
    QList<int> _EndHighlightsLst;
    int _previousHighLightedRow;
    static int _countParent;

public:
    static bool _statusVar;

private:

    void setElement(GTAElement * _pElementModel);
    /**
        This fuction check if the incoming command is a valid case for inserting/updating data model
        cases:
        if the new command is else, for insertion/ updation
            the current command should be 'IF' and it should not have existing 'ELSE' command
            other wise 
            the current command's parent should be 'IF' command without any 'ELSE' in it.
        in all other case else cannot be inserted.

    */
    bool checkCommandValidity(GTACommandBase* pCurrentCommand, GTACommandBase* pParentCommand, GTACommandBase* pIncomingCmd);
};

#endif // GTAELEMENTVIEWMODEL_H
