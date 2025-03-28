#ifndef GTAWAITFORWIDGET_H
#define GTAWAITFORWIDGET_H

#include <QWidget>
#include "GTAActionWait.h"
#include "GTAActionWidgetInterface.h"

namespace Ui {
    class GTAWaitForWidget;
}

class GTAWaitForWidget : public GTAActionWidgetInterface
{
    Q_OBJECT

public:
    explicit GTAWaitForWidget(QWidget *parent = 0);
    ~GTAWaitForWidget();
    QString getValue() const;
    void setValue(const QString &iVal);

    void setActionCommand(const GTAActionBase* ipActionCmd);
    bool getActionCommand(GTAActionBase*& opCmd)const;
    void clear() ;
    bool isValid()const;

    bool setWidgetFields(const QList<QPair<QString, QString>>& params, ErrorMessageList& errorLogs) override final;

private:
    Ui::GTAWaitForWidget *ui;
};

#endif // GTAWAITFORWIDGET_H
