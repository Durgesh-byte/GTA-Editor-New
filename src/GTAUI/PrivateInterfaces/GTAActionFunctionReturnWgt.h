#ifndef GTAACTIONFUNCTIONRETURNWGT_H
#define GTAACTIONFUNCTIONRETURNWGT_H
#include "GTAActionWidgetInterface.h"

#pragma warning(push, 0)
#include <QWidget>
#include <QLineEdit>
#include <QDateTime>
#include <QBasicTimer>
#pragma warning(pop)

class GTAActionPrint;
namespace Ui {
    class GTAActionFunctionReturnWgt;
}

class GTAActionFunctionReturnWgt : public GTAActionWidgetInterface
{
    Q_OBJECT

public:
    explicit GTAActionFunctionReturnWgt(QWidget *parent = 0);
    ~GTAActionFunctionReturnWgt();

    void setActionCommand(const GTAActionBase * ipActionCmd);
    bool getActionCommand(GTAActionBase*& opCmd)const;
    void clear() ;
    bool isValid()const ;
    bool hasSearchItem(){return true;}
    int getSearchType();
    

   

signals:
    private slots:
         void onSearchPBClicked();

private:
    Ui::GTAActionFunctionReturnWgt *ui;

};
#endif