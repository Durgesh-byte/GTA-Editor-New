#ifndef GTACHECKECAMDISPLAYWIDGET_H
#define GTACHECKECAMDISPLAYWIDGET_H

#include <QWidget>
#include "GTACheckWidgetInterface.h"

namespace Ui {
    class GTACheckECAMDisplayWidget;
}
class GTACheckEcamDisplay;
class GTACheckECAMDisplayWidget : public GTACheckWidgetInterface
{
    Q_OBJECT

public:
    explicit GTACheckECAMDisplayWidget(QWidget *parent = 0);
    ~GTACheckECAMDisplayWidget();

    void setCheckCommand(const GTACheckEcamDisplay * ipChkCmd);
    GTACheckEcamDisplay * getCheckCommand();


    virtual void clear();
    virtual bool isValid();
    virtual bool getCommand(GTACheckBase *& pCheckCmd);
    virtual bool setCommand(const GTACheckBase * pCheckCmd);

    bool hasTimeOut()const       ;
    bool hasDumpList()const      ;
    bool hasPrecision()const     ;
    bool hasSearchItem()const    ;
    bool hasConfigTime() const   ;
    bool hasActionOnFail()const  ;
private:
    Ui::GTACheckECAMDisplayWidget *ui;

    QString getDiplayType() const;
    QString getDisplayValue() const;


private slots:
        void onDisplayTypeChange(const QString & );
};

#endif // GTACHECKECAMDISPLAYWIDGET_H
