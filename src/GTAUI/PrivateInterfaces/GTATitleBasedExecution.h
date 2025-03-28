#ifndef GTATITLEBASEDEXECUTION_H
#define GTATITLEBASEDEXECUTION_H

#include <QWidget>
#include <QAbstractItemModel>
#include "GTACommandBase.h"
#include "GTATitleBasedExecutionModel.h"
#include "GTAWidget.h"
#include <QPointer>

namespace Ui {
class GTATitleBasedExecution;
}

class GTATitleBasedExecution : public GTAWidget
{
    Q_OBJECT

private:

    explicit GTATitleBasedExecution(GTAWidget *parent = 0);
public:
    ~GTATitleBasedExecution();

    void setModel(QAbstractItemModel *pModel);
    void setFileName(const QString &iFileName);
    void setUUID(const QString &iUUID){_uuid = iUUID;}
    QString getUUID()const {return _uuid;}
    void setVariableList(const QStringList &iVarList);
    QStringList& getVariableList();

    static QPointer<GTATitleBasedExecution> getTittleBasedWidget(GTAWidget *parent = 0);

signals:
    void executeTitle(const QString &iUuid, GTAElement *pTitleBasedElem,QStringList& iVarList);

public slots:
    void onExpandAll();
    void onCollapseAll();
    void onSelectAll();
    void onDeSelectAll();
    void onExecute();

private:
    void selectAll(bool isSelectAll);
    void onUpdateModel();
private:
    QString _uuid;
    Ui::GTATitleBasedExecution *ui;
    QAbstractItemModel *_pModel;
    QStringList _varList;
    static QPointer <GTATitleBasedExecution> _pTitleBasedWidget;


    static int count;
};

#endif // GTATITLEBASEDEXECUTION_H
