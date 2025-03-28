#ifndef GTALOGRESULTTV_H
#define GTALOGRESULTTV_H

#include <QTreeView>
#include "GTALogResultTVRowEditor.h"
#include <QWidget>
#include <QMenu>
#include "GTAAppController.h"

class GTALogResultTV : public QTreeView
{
    Q_OBJECT

public:

    GTALogResultTV(QWidget * parent = 0 );
    virtual ~GTALogResultTV();

    void showDetails(QAbstractItemModel *iModel, const QModelIndex &iIndex, bool &oIsDataChanged);
    void setdefaults();

public slots:
    void displayHeaderContextMenu(QPoint iPos);
    void displayContextMenu(const QPoint &iPos);
    void onRowDataChanged();
    void showHideCol();
    void onDisplayDelta();

signals:
     void UpdateErrorLog(const ErrorMessageList &iErrorList);

private:
    void createHeaderContextmenu();
    void saveSelectionSettings(int index,QString actionText,bool isChecked);
    void populateContextMenu();
    QString getTimeDifferrence(const double &iTime1,const double &iTime2);


private:
    GTALogResultTVRowEditor *_RowEditor;
    QAbstractItemModel *_iModel;
    QModelIndex _iIndex;
    QList<QAction*> actions;
    QStringList columns;
    QMenu *_pContextMenu;
    QAction * Action;
    QAction * Result;
    QAction * CurrentValue;
    QAction * ExpectedValue;
    QAction * TimeOut;
    QAction * ActionOnFail;
    QAction * Precision;
    QAction * Comment;
    QAction * DumpList;
    QAction * Defects;
    QAction * ExecutionTime;
    QAction * UserFeedback;

    QMenu *_pContextMenuTV;
    QAction *_pActionDisplayDelta;


};
#endif
