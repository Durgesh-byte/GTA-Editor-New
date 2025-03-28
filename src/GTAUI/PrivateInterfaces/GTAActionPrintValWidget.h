#ifndef GTAACTIONPRINTVALWIDGET_H
#define GTAACTIONPRINTVALWIDGET_H

#include "GTAActionWidgetInterface.h"

#pragma warning(push, 0)
#include <QWidget>
#include <QLineEdit>
#pragma warning(pop)


class GTAActionPrint;
namespace Ui {
    class GTAActionPrintValWidget;
}

class GTAActionPrintValWidget : public GTAActionWidgetInterface
{
    Q_OBJECT

public:
    explicit GTAActionPrintValWidget(QWidget *parent = 0);
    ~GTAActionPrintValWidget();

   
    void setValue(const QString & iVal) ;
    
    

    void setActionCommand(const GTAActionBase * ipActionCmd);
    bool getActionCommand(GTAActionBase*& opCmd)const;
    void clear() ;
    bool isValid()const ;
    bool hasSearchItem(){return true;}

    int getSearchType();
    bool setWidgetFields(const QList<QPair<QString, QString>>& params, ErrorMessageList& errorLogs) override final;

signals:
    void searchObject(QLineEdit *&);

private:
    Ui::GTAActionPrintValWidget *ui;
    QStringList getValues() const;

private slots:
    void onSearchPBClicked();
    void addToParamList();
    void removeFromParamList();
    void editFromParamList();
    void updateParamLEFromLst();
};

#endif // GTAActionPrintValWidget_H

//void setActionCommand(const GTAActionBase * ipActionCmd);
//bool getActionCommand(GTAActionBase* opCmd)const;
//void clear() ;
//bool isValid()const ;
