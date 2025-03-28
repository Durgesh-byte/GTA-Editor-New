#ifndef GTAACTIONONECLICKPPCWIDGET_H
#define GTAACTIONONECLICKPPCWIDGET_H

#include <QWidget>
#include "GTAActionOneClickPPC.h"
#include "GTAActionWidgetInterface.h"
namespace Ui {
    class GTAActionOneClickPPCWidget;
}

class GTAActionOneClickPPCWidget : public GTAActionWidgetInterface
{
    Q_OBJECT

public:
    explicit GTAActionOneClickPPCWidget(QWidget *parent = 0);
    ~GTAActionOneClickPPCWidget();

    void setActionCommand(const GTAActionBase * ipActionCmd);
    bool getActionCommand(GTAActionBase*& opCmd)const;
    void clear() ;
    bool isValid() const ;
    
    bool hasActionOnFail(){return true;}
private:
    Ui::GTAActionOneClickPPCWidget *ui;
    void clearWidget();
private slots:
    void onBrowsePBClicked();
    void onStartRBClicked(bool iVal);
    void onStopRBClicked(bool iVal);


};

#endif // GTAACTIONONECLICKPPCWIDGET_H
