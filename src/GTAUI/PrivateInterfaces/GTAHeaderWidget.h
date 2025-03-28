#ifndef GTAHEADERWIDGET_H
#define GTAHEADERWIDGET_H

#include "GTAHeaderFieldEdit.h"
#include "GTAEditorWindow.h"

#pragma warning (push, 0)
#include <QWidget>
#include <QTableView>
#include <QDialog>
#include <QFileSystemModel>
#pragma warning (pop)

namespace Ui {
class GTAHeaderWidget;
}

class GTAHeaderWidget : public QDialog
{
    Q_OBJECT

public:
    explicit GTAHeaderWidget(GTAEditorWindow *parent, QDialog *dParent = 0);
    ~GTAHeaderWidget();
    QTableView* getHeaderTableView();
    void setWindowTitle(const QString &iTitle);
    void setModel(QAbstractItemModel * iModel);
    void silentDisableICDChkButton();
    void setModelForCurrentDoc(QAbstractItemModel* pHeaderModel);
    void highlightHeaderTableRow(int row);

private slots:
    void startAddingHeaderItem();
    void startEditingHeaderItem();
    void onDeleteHeaderField();
    void onMoveHdrRowUp();
    void onMoveHdrRowDown();
    void onEditHeaderField();
    void onApplyChangesHeader();
    bool onSaveHeaderDoc();
    void onNewHeaderDoc();
    void onShowICDDetails(bool status);
    void onHeaderItemSelectionChanged(const QItemSelection&, const QItemSelection&);

public slots:
    void onHeaderFieldUpdatedInModel(bool val, const QString &iErrorMsg);
    bool onLoadHeaderDoc(const QString& isHdrName);
    void onAddHeaderItem();
    void onCustomMenuRequested(const QPoint&);
    void onSelectAllToExtract();
    void onUnSelectAllToExtract();

signals:
    void headerFieldAdded(const QString& iName,const QString& iVal,const QString& iMandatory,const QString& iDescription);
    void headerChanged();

private:
    Ui::GTAHeaderWidget *ui;
    GTAHeaderFieldEdit *  _pHdrFieldEditDlg;
    GTAEditorWindow *_parent;
    QFileSystemModel * _pFileSystemHeader;
};

#endif // GTAHEADERWIDGET_H
