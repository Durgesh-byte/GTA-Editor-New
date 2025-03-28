#ifndef GTAVALIDATIONLISTWIDGET_H
#define GTAVALIDATIONLISTWIDGET_H

#include <QWidget>
#include <QAbstractItemModel>
#include "GTAUI.h"
#include "GTACommandBase.h"
#include "GTAWidget.h"
#include "GTAEditorLogModel.h"
#include "GTAActionExpandedCall.h"
#include "GTAActionCall.h"
#include <QPointer>
#include "GTAValidationWidgetModel.h"

namespace Ui {
    class GTAValidationListWidget;
}

class GTAUI_SHARED_EXPORT GTAValidationListWidget : public GTAWidget
{
    Q_OBJECT


private:
    Ui::GTAValidationListWidget *ui;
    QAbstractItemModel * _pModel;

private:
    void onUpdateModel();
    void selectAll(bool isSelectAll);


public:
    ~GTAValidationListWidget();
    explicit GTAValidationListWidget(GTAWidget *parent = 0);
    void setModel(QAbstractItemModel *pModel);
    void setFileName(const QString &iFileName);
    bool getRootValidated();


signals:
    void selectedFiles(QString author, QStringList uuids);
    void changeToEditorView(bool);
public slots:
    void onExpandAll();
    void onCollapseAll();
    void onSelectAll();
    void onDeSelectAll();
    void onValidate_Skip_Cancel_PBClicked();

private slots:
    void enableDisableValidate(QString text);


};

#endif // GTAVALIDATIONLISTWIDGET_H
