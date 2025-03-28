#ifndef GTAEDITORVIEWCONTROLLER_H
#define GTAEDITORVIEWCONTROLLER_H

#include "GTAControllers.h"
#include "GTAElement.h"
#include "GTAElementViewModel.h"
#include "GTAHeaderTableModel.h"
#include "GTAUtil.h"
#include "GTACommandBase.h"
#include "GTACommandList.h"
#include "GTAAppController.h"
#define PORTING_ERROR "PORTING ERROR"

#pragma warning(push, 0)
#include <QTableView>
#include <QHeaderView>
#include <QObject>
#include <QHash>
#pragma warning(pop)


//TODO: This is a warning against assigning Rvalues to references, which is done in default arguments for many methods in this file,
//      This will be fixed in the future, as this refactor will take some time (there are a lot od default arguments which will be broken by removing default argument from some of the refs
#pragma warning (disable : 4239)

class GTAControllers_SHARED_EXPORT GTAEditorViewController :public QObject
{
    Q_OBJECT
public:
    GTAEditorViewController(QObject* iObject = 0);
    ~GTAEditorViewController();
    bool setInitialEditorView(QTableView* ipView);
    void establishConnectionWithModel();
    bool setInitialHeaderView(QTableView* ipView);
    bool createAndSetElementFromDataDirectoryDoc(const QString& iFileName, bool isOutsideDataDir = false);
    bool setElement(const GTAElement* isElement);
    bool updateElementMaxTimeEstimated(const QString& maxTimeEstimated);
    bool setHeader(GTAHeader* ipheader);
    bool getElementModel(QAbstractItemModel*& opModel);
    bool getHeaderModel(QAbstractItemModel*& iopModel);
    GTAElement* getElement(void)const;
    GTAHeader* getHeader() const;
    void updateEditorView()const;
    QStringList getHiddenRows();
    GTACommandBase* getCommand(const int& iSelRow)const;
    QHash<QString, QStringList> getActionCommandList(GTAUtil::ElementType iType, QStringList& orderList);
    QList<QString> getCheckCommandList(GTAUtil::ElementType iType);
    bool isEditableCommand(GTACommandBase* pCmd);
    bool editAction(const int& iRow, const QVariant& incomingData);
    void deleteEditorRows(const QList<int>& lstSelectedRows);
    void removeEmptyRows();
    void undoClearRow(const QList<int>& iIndexList, QList<GTACommandBase*>& iCmdList);
    void clearAllinEditor();
    bool highLightLastChild(const QModelIndex& ipIndex);
    void addEditorRows(const int& iSelectedRow);
    bool clearRow(int iSelectedRow, QString& oErrMsg, QList<int>& oClearedRowId);
    QString getLastError() const;
    bool areSelectedCommandsBalanced(QList<int>& ioLstSelectedRows);
    bool areSelectedCommandsNested(const QList<int>& iLstSelectedRows);
    bool insertCommands(const QList<int>& iIndexList, const QList<GTACommandBase*>& iCmdList, bool isSrcUndoRedo);
    int insertCommand(const int& iIndex, GTACommandBase* iCmd, bool isSrcUndoRedo);
    //saurav
    bool pasteEditorCopiedRow(const int& iPasteAtRow, const QList<GTACommandBase*> CmdsPasted, QString lastCalled);
    bool pasteEditorCopiedRow(const int& iPasteAtRow);
    bool setEditorRowCopy(QList<int>& lstSelectedRows, const QList<GTACommandBase*>& iCmdList);
    bool setEditorRowCopy(QList<int>& lstSelectedRows, bool isNewCommand);
    bool setCopiedItems(QList<int>& ioLstSelectedRows, const QList<GTACommandBase*>& iCmdList);
    bool setCopiedItems(QList<int>& ioLstSelectedRows, bool isNewCommand);
    void clearEditorRowCopy();
    int getSelectedRowInEditor() const;
    int getRowToBeSelected() const;
    bool moveEditorRowItemUp(int selectedItem);
    bool isEditorRowMovableUpwards(int selectedItem)const;
    bool isCommandAboveParentEmpty(int selectedItem) const;
    bool moveEditorRowItemDown(int selectedItem);
    bool isEditorRowMovableDownwards(int selectedItem)const;
    bool isSpaceBelowLastChild(int selectedItem) const;
    void getRowsContainingTitle(QList<int>& oLstOfRows);
    bool isDataEmpty();
    bool setHeaderView(QTableView* ipView, GTAHeader*& ipHeader);
    bool addheaderField(const QString& iName, const QString& iVal, const QString& iMandatory, const QString& iDescription);
    bool getHeaderFieldInfo(const int& iRow, QString& iFieldName, QString& iFieldVal, QString& iMandatoryInfo, QString& iFieldDescription, bool& iShowInLTRA);
    bool isHeaderFieldMandatory(const int& iRow, bool& isMandatory);
    bool isDefaultMandatoryHeaderField(const int& iRow);
    bool editHeaderField(const int& iRow, const QString& iName, const QString& iVal, bool iMandatory, const QString& iDescription, const bool& iShowLTRA);
    bool removeHeaderItem(const int& iRow);
    bool isHeaderRowEditable(const int& iRow);
    bool isHeaderRowDeletable(const int& iRow);
    bool moveHeaderRowItemUp(const int& iRow);
    bool moveHeaderRowItemDown(const int& iRow);
    QString getHeaderName()const;
    bool headerComplete(QStringList& oEmptyManField)const;
    bool getHeaderFromDocument(const QString& iFileName, GTAHeader*& opHeader);
    bool createHeaderModelFromDoc(const QString& iFileName, QAbstractItemModel*& opEditorModel);
    bool createHeaderModel(GTAHeader* ipheader, QAbstractItemModel*& iopModel);
    bool deleteHeader(const QString& iName);
    bool saveHeader(const QString& iName);    
    bool applyChangesHeader(const QString& Name, GTAHeader*& pHeader);
    bool showICDDetails(const bool& iShow, QList <GTAFILEInfo> lstFileInfo);
    void setGTADirPath(const QString& iPath);
    void resetDisplayContext();
    void setDisplaycontextToCompatibilityCheck();


    GTAElementViewModel::DisplayContext getDisplayContext()const;
    void setDisplayContext(GTAElementViewModel::DisplayContext context);


    void highlightRow(const int& iRowId, QModelIndex& oHighlightedRow, GTAElementViewModel::DisplayContext iDisplayContext);
    //    ErrorMessageList portElement();
    int getLastHighligtedRow() { return _lastHighligtedRow; }
    bool searchHighlight(const QRegExp& searchString, QList<int>& iSelectedRows, bool ibSearchUp, bool ibTextSearch, QModelIndex& oHighlightedRowIdx, bool isSingleSearch = false, bool isHighlight = true, QHash<int, QString>& oFoundRows = QHash<int, QString>());
    bool hightlightReplace(const QRegExp& searchString, const QString& iReplaceString, QList<int>& iSelectedRows, bool ibSearchUp, QModelIndex& oHighlightedRowIdx, QList<int>& oFoundRows = QList<int>());
    bool replaceAllString(const QRegExp& searchString, const QString& iReplaceString);
    bool replaceAllEquipment(const QRegExp& searchString, const QString& iReplaceString);
    QString getUUID()const;

    void setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

private slots:
    void onCallActionInserted(int& iRow, QString& iFileName);

signals:
    void clearClipBoardItems();
    void callActionDropped(int& iRow, QString& iFileName);
    void getHeaderTemplateFilePath(QString& templFilePath);
    void getGTADataDirectory(QString& oDataDirPath);
    void getCopiedItems(QList<GTACommandBase*>& oCopiedItems);
    void setCopiedItem(GTACommandBase* selectedItem);
    void setCopiedItems(QList<GTACommandBase*> lstSelectedRows);

private:
    GTAElement* _pElement;
    GTAElementViewModel* _pElementViewModel;

    GTAHeader* _pHeader;
    GTAHeaderTableModel* _pHeaderModel;

    QString _LastError;
    int _lastHighligtedRow;
    QTableView* _pEditorView;
    QString _GtaDataDirPath;
    int _previousInsertRowIndex;
    int _rowToBeSelected;
};

#pragma warning (default : 4239)
#endif // GTAEDITORVIEWCONTROLLER_H
