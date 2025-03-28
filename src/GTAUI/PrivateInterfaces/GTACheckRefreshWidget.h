#ifndef GTACheckRefreshWidget_H
#define GTACheckRefreshWidget_H

#include <QWidget>
#include <QHash>
#include <QLineEdit>
#include "GTACheckWidgetInterface.h"
#include "GTACheckRefresh.h"
#include "GTAICDParameter.h"

namespace Ui {
class GTACheckRefreshWidget;
}
class GTACheckRefresh;
class GTACheckRefreshWidget :/* public QWidget, */public GTACheckWidgetInterface
{
    Q_OBJECT

public:
    explicit GTACheckRefreshWidget(QWidget *parent = 0);
    ~GTACheckRefreshWidget();

    void setCheckCommand(const GTACheckRefresh * ipCheckCmd);
    GTACheckRefresh * getCheckCommand();

    //Derived method
    void clear();
    bool isValid();
    bool getCommand(GTACheckBase *& pCheckCmd);
    bool setCommand(const GTACheckBase * pCheckCmd);

    void processSearchInput(const GTAICDParameter& iResult);
   
    bool hasTimeOut()const       ;
    bool hasDumpList()const      ;
    bool hasPrecision()const     ;
    bool hasSearchItem()const    ;
    bool hasConfigTime() const   ;
    bool hasActionOnFail()const  ;

signals:
    void searchParameter(QLineEdit *&);


private:
    Ui::GTACheckRefreshWidget *ui;


private slots:
    //void fillRefreshRateInfo(const QString & text);
    void onSearchPBClicked();
};

#endif // GTACheckRefreshWidget_H
