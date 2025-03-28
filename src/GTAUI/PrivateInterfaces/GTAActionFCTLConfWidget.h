#ifndef GTAACTIONFCTLCONFWIDGET_H
#define GTAACTIONFCTLCONFWIDGET_H

#include "GTAActionBase.h"
#include "GTAActionWidgetInterface.h"

#pragma warning(push, 0)
#include <QWidget>
#include <QLineEdit>
#pragma warning(pop)

namespace Ui {
    class GTAActionFCTLConfWidget;
}

class GTAActionFCTLConfWidget : public GTAActionWidgetInterface
{
    Q_OBJECT

public:
    explicit GTAActionFCTLConfWidget(QWidget *parent = 0);
    ~GTAActionFCTLConfWidget();


    void clear();
    bool isValid()const;

    bool getActionCommand(GTAActionBase *& pCmd)const;
    void setActionCommand(const GTAActionBase * pCmd);
    

private:
    Ui::GTAActionFCTLConfWidget *ui;

};

#endif // GTAACTIONFCTLCONF_H
