#ifndef GTAACTPRINTMSGWIDGET_H
#define GTAACTPRINTMSGWIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include "GTAActionWidgetInterface.h"



class GTAActPrintMsgWidget : public GTAActionWidgetInterface 
{
    Q_OBJECT

public:
    explicit GTAActPrintMsgWidget(QWidget *parent = 0);
    ~GTAActPrintMsgWidget();

    void setActionCommand(const GTAActionBase * ipActionCmd);
    bool getActionCommand(GTAActionBase*& opCmd)const;
    void clear() ;
    bool isValid()const ;

    bool setWidgetFields(const QList<QPair<QString, QString>>& params, ErrorMessageList& errorLogs) override final;
      
private:

    QTextEdit* _pLineEdit;
    QVBoxLayout* _pBoxLayout; 
private slots:
        void validateText();
    
};

#endif // GTAActPrintMsgWidget_H


