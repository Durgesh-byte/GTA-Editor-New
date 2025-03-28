#ifndef GTASEARCHRESULTS_H
#define GTASEARCHRESULTS_H

#include <QWidget>
#include <QString>
#include <QHash>
#include <QHashIterator>
#include <QTreeWidgetItem>
#include <QModelIndex>
#include "GTAAppController.h"
#include <QCloseEvent>
#include <QShortcut>
#include <QMenu>
#include "GTAWidget.h"

namespace Ui {
    class GTASearchResults;
}

class GTASearchResults : public GTAWidget
{
    Q_OBJECT

public:
    explicit GTASearchResults(GTAWidget *parent = 0);
    ~GTASearchResults();
    void show();
    void setSearchResults(const QHash<QString,SearchResults> &searchResultMap, bool isHeader);

private:
    void createContextMenu();

signals:
    void jumpToElement(const QString &iRelativeFilePath, const int &iLineNumber,GTAAppController::DisplayContext iDisplayContext,bool isUUID = false);
    void jumpToElementHeader(const QString& iRelativeFilePath, const int& iLineNumber, GTAAppController::DisplayContext iDisplayContext, bool isUUID = false);

private slots:
    void onTreeItemDoubleClicked(QTreeWidgetItem* pItem, int );
    void closeEvent(QCloseEvent *event);
    void onClearSearchWindow();
    void onExportSearchWindow();
    void displayContextMenu(const QPoint &iPos);



private:
    Ui::GTASearchResults *ui;
    QAction *_pActionClearWindow;
    QAction* _pActionExportResult;
    QMenu * _pContextMenu;
    bool _isHeader;
};

#endif // GTASEARCHRESULTS_H
