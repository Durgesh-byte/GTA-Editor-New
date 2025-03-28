#ifndef GTAACTIONPRINTTIMEWGT_H
#define GTAACTIONPRINTTIMEWGT_H

#include <QWidget>
#include "GTAActionWidgetInterface.h"
#include <QLineEdit>
#include <QDateTime>
#include <QBasicTimer>
class GTAActionPrint;
namespace Ui {
    class GTAActionPrintTimeWgt;
}

class GTAActionPrintTimeWgt : public GTAActionWidgetInterface
{
    Q_OBJECT

public:
    explicit GTAActionPrintTimeWgt(QWidget *parent = 0);
    ~GTAActionPrintTimeWgt();

    void setActionCommand(const GTAActionBase * ipActionCmd);
    bool getActionCommand(GTAActionBase*& opCmd)const;
    void clear() ;
    bool isValid()const ;
    bool hasSearchItem(){return false;}
    bool setWidgetFields(const QList<QPair<QString, QString>>& params, ErrorMessageList& errorLogs) override final;

   

signals:
    

private:
    Ui::GTAActionPrintTimeWgt *ui;

};
#endif