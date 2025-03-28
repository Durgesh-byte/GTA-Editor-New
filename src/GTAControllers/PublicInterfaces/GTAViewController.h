#ifndef GTAVIEWCONTROLLER_H
#define GTAVIEWCONTROLLER_H

#include "GTAControllers.h"
#include "GTASystemServices.h"
#include "GTATreeViewModel.h"
#include "GTATreeItem.h"
#include "GTADataElementFilterModel.h"
#include "GTACheckBase.h"
#include "GTAActionForEach.h"

#pragma warning(push, 0)
#include <QObject>
#include <QList>
#include <QTableView>
#include <QHash>
#include <QVariant>
#include <QStringListModel>
#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#pragma warning(pop)

/*
TODO for refractoring: Make api showICDDetails(const bool & iShow,QList <GTAFILEInfo> lstFileInfo) to
showICDDetails(const bool & iShow,QList <GTAFILEInfo*> lstFileInfo) and implement accordingly;
forward declare GTAFILEInfo and GTAHeader remove dependency of datamodel from pro files (GTAApp.pro)
*/
#include "GTAHeader.h"
class GTACommandBase;
//class GTAHeader;
class GTAElement;
class GTAICDParameter;
class GTABaseModel;
class GTAElementViewModel;
class QDomDocument;
class GTAActionCall;
class GTAActionExpandedCall;
class GTAHeaderTableModel;
class GTAFileSystemModel;
class GTATreeViewModel;
class GTAActionParentTitle;
class GTACheckBase;
class GTAActionIfEnd;


enum GTAControllers_SHARED_EXPORT EditorType{Object,Procedure,Function,Sequence,Template,Invalid};


// Controller for handling header and editor.
// Singleton class
class GTAControllers_SHARED_EXPORT GTAViewController : public QObject
{

    Q_OBJECT

public:
    static GTAViewController* getGTAViewController(QObject* ipObject=0);
    ~GTAViewController();
    bool setInitialEditorView(QTableView* ipView);
    bool setHeaderView(QTableView* ipView,GTAHeader*& ipHeader);
    bool setEditorModel (QAbstractTableModel* ipModel);

    QString getHeaderName()const;
    bool headerComplete(QStringList& oEmptyManField)const;
    bool headerEdited()const;
    
    void addEditorRows(const int& iSelectedRow);
    //	void prependEditorRows();
    bool isSelectedCommandsBalanced(QList<int>& ioLstSelectedRows);
    bool areSelectedCommandsNested(const QList<int>& iLstSelectedRows);
    void deleteEditorRows(const QList<int>& lstSelectedRows);


    void clearAllinEditor();


    bool pasteEditorCopiedRow(const int& iPasteAtRow,const QList<GTACommandBase*> CmdsPasted,QString lastCalled);
    bool pasteEditorCopiedRow(const int& iPasteAtRow);
    void clearEditorRowCopy();
//    int insertCommand(const int &iIndex, GTACommandBase *iCmd, bool isSrcUndoRedo = false);
//    bool insertCommands(const QList<int> &iIndexList, const QList<GTACommandBase*> &iCmdList,bool isSrcUndoRedo = false);
    void undoClearRow(const QList<int> &iIndexList, QList<GTACommandBase*> &iCmdList);

    //bool editAction(const int& iRow,GTACommandBase* ipActionBase);
    bool editAction(const int& iRow,const QVariant& incomingData);
    //void editComment(const int& iRow, GTAComment* ipComment);
    //	void editComment(const int& iRow, const QVariant& ipCommentVariant);


    //bool saveHeader(const QString& isFilePath)const;
    QAbstractItemModel* buildEditorModels(const EditorType& ieType);
    
    /**
      * Create header model for view from header element.
      * @ipHeader: header element to be set into model.
      * @iopModel: Created model;
      */
    bool  createHeaderModel(GTAHeader* ipheader, QAbstractItemModel*& iopModel);
    GTACommandBase* getCommand(const int& iSelRow)const;

    GTAElement* getElement(void)const{return _pElement;}
    inline GTAHeader * getHeader() const{return _pHeader;}
    void updateEditorView()const;
    bool setCopiedItems(QList<int>& ioLstSelectedRows,const QList<GTACommandBase*> &iCmdList);
    bool setCopiedItems(QList<int>& ioLstSelectedRows, bool isNewCommand);
    QAbstractItemModel* buildEditorModels(const GTAElement* isElement,EditorType isType);

    QString getLastError(){return _LastError;}

    /**
      * This function return the selected row in editor, if succeed return valid row
      * otherwise -1
      */
//    int getSelectedRowInEditor() const;
    /**
      * This function creates a tree view model (that is delegated to from app controller) 
      * based on row selection in editor.
      *@parameter opModel: The output model
      *@iRow:row selection.
    */
    bool getModelForTreeView( QAbstractItemModel*& oPModel ,const int& iRow,GTAElement *ipElement);
    
//    bool addheaderField(const QString& iName,const QString& iVal,const QString& iMandatory,const QString& iDescription);
//    bool editHeaderField(const int& iRow, const QString& iName,const QString& iVal,bool iMandatory,const QString& iDescription);
//    bool removeHeaderField(const int& iRow);
//    bool moveHeaderRowItemUp(const int& iRow);
//    bool moveHeaderRowItemDown(const int& iRow);

//    bool showICDDetails(const bool & iShow,QList <GTAFILEInfo> lstFileInfo);

	void createExpandedTreeElement(GTAElement*& iopElement,QStringList& checkForRep, bool groupExpandedElementByTitle = false);
    void createExpandedTreeElement1(GTAElement*& iopElement,QStringList& checkForRep, bool groupExpandedElementByTitle = false);
//    bool isEditorRowMovableUpwards(const int& selectedItem)const;
//    bool isEditorRowMovableDownwards(const int& selectedItem)const;
//    bool moveEditorRowItemUp(const int& selectedItem) ;
//    bool moveEditorRowItemDown(const int& selectedItem) ;
//    bool searchHighlight(const QRegExp& searchString,QList<int>& iSelectedRows,bool ibSearchUp,bool ibTextSearch,QModelIndex &oHighlightedRowIdx);
//    void resetDisplayContext();
//    void setDisplaycontextToCompatibilityCheck();
//    int getLastHighligtedRow(){return _lastHighligtedRow;}
//    bool hightlightReplace(const QRegExp& searchString,const QString& iReplaceString,QList<int>& iSelectedRows,bool ibSearchUp,QModelIndex &oHighlightedRowIdx );
//    bool replaceAllString(const QRegExp& searchString,const QString& sReplace);
//    bool highLightLastChild(const QModelIndex& ipIndex);

    bool getExpandedElementFromLoadedElement(GTAElement*& oElement,bool flattenElements=true);
    void getRowsContainingTitle(QList<int>& oLstOfRows);
    /*
        creates a new header model based on current header, deletes current model.
    */
    bool  createSetHeaderModel( QAbstractItemModel*& iopModel);
    /*
    creates a new element model based on current element, deletes current model.
    */
    bool  createSetElementModel( QAbstractItemModel*& iopModel);
    /*
    sets a new element , deletes current element.
    */
    bool setElement(const GTAElement* isElement,EditorType isType);
    /*
    sets a new element , deletes current element.
    */
    bool setHeader(GTAHeader* ipheader);

    bool getModelForTreeView( QAbstractItemModel*& pModel , GTAElement* ipElement,const QString& iElemetName);
//    bool getModelForTreeView( QAbstractItemModel*& pModel , GTAElement* ipElement,const QString& iElemetName);
    bool createFavoriteDataModel(const QStringList& favList);
    void updateFavoriteDataModel();
    GTATreeItem * getFavRootTreeNode();
    QSortFilterProxyModel*  getFavoriteBrowserDataModel(bool isFavItemRemoved = false);
    bool removeFavorites(const QStringList& favList);
    bool addFavorites(const QStringList& favList);
    bool createElementDataBrowserModel( const QStringList& iFilters ,const QString& iDataDir);
    QAbstractItemModel*  getElemDataBrowserModel() {return (QAbstractItemModel* )_pElemBrowserModel;}
    void cloneTree(GTATreeItem* &newRoot, const GTATreeItem * originalRoot);


    //    void setFavFlagForTreeModel(bool iVal);
    QSortFilterProxyModel* getTreeBrowserModel();
    bool createTreeBrowserModel();
    /**
      *This function gets list of column in data browser
      @return: column list
    */
    QStringList getColumnList() const;
	
    QList<GTATreeItem *> createTreeItemList(GTATreeItem* parent);
    //    void createChildNodes(QList<GTATreeItem *> &rootItems,GTATreeItem *parent = 0);
    QStringList getDocumentByExtension(const GTATreeItem *treeRootNode, const QString &iExtn, QHash<QString, QString> &docMap);
    void updateTreeBrowserModel();
    //    bool addFileInTreeBrowserModel(const QString &iFileName, const QString &iFilePath, const QString &iGTADirPath, const QString &iExtn);
    //    bool deleteFileFromTreeBrowserModel(const QString &iFileName, const QString &iFilePath, const QString &iGTADirPath, const QString &iExtn);

    QModelIndex getElementDataBrowserIndexForDataDir(const QString&);
    void setFiltersOnFavorites(const QString& filter);
    void setFiltersOnElemBrowser(const QStringList& filters,const int &iSearchType);
//    bool getClipboardModel(QAbstractItemModel*& pModel);
    void setHiddenRows( const QStringList& iRows);
    QStringList getHiddenRows();
    QList<GTACommandBase*> getCommandsInClipboard()const;

public slots:
    void onCallActionInserted(int& iRow, QString& iFileName);
    


signals:
    void callActionInserted(int& row, QString& iFileName); 
    
private:

    void deleteAllModel();
    
    void createCommandGroupedByTitle(GTACommandBase*& ipCmd, GTAActionParentTitle *&ipTitleCmd,QStringList &oVariableList, QStringList &icheckForRep);
    GTAActionExpandedCall* createExpandedTreeElement1( GTACommandBase*& ipCallCmd , GTACommandBase* ipParenExtCall , QStringList& chkForLev, bool groupExpandedElementByTitle=false);
    void establishConnectionWithModel();

    GTAViewController(QObject* ipObject);
    static GTAViewController* _pViewController;


    GTAElement* _pElement;
    GTAHeader* _pHeader;
    GTAHeaderTableModel* _pHeaderModel;
    GTAElementViewModel* _pElementViewModel;
    QList<GTACommandBase*> _lstCopiedElements;
    QString _LastError;
    int _lastHighligtedRow;
    int _previousInsertRowIndex;

    GTAFileSystemModel* _pElemBrowserModel;

    QTableView * _pEditorView;
    GTATreeItem *_pFavTreeRoot;
    GTATreeViewModel * _pFavDataModel;
    GTATreeViewModel * _pTreeViewModel;
    GTADataElementFilterModel *_pProxyModel;
};

#endif
