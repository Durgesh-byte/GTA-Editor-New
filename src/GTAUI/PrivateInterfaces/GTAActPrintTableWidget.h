#ifndef GTAACTPRINTTABLEWIDGET_H
#define GTAACTPRINTTABLEWIDGET_H
#include "GTAActionWidgetInterface.h"
#include "GTAActionPrintTable.h"

#pragma warning(push, 0)
#include <QWidget>
#include <QLineEdit>
#pragma warning(pop)

class GTAActionPrint;
namespace Ui {
    class GTAActPrintTableWidget;
}

class GTAActPrintTableWidget : public GTAActionWidgetInterface 
{
    Q_OBJECT

public:
    explicit GTAActPrintTableWidget(QWidget *parent = 0);
    ~GTAActPrintTableWidget();

    void setActionCommand(const GTAActionBase * ipActionCmd);
    bool getActionCommand(GTAActionBase*& opCmd)const;
    void clear() ;
    bool isValid()const ;
    bool hasSearchItem(){return true;}

    QStringList getValues() const;
    void setValue(const QString & ) ;
    bool updatePrintTableInElement();
    int getSearchType();
    bool setWidgetFields(const QList<QPair<QString, QString>>& params, ErrorMessageList& errorLogs) override final;
   
private:
    Ui::GTAActPrintTableWidget *ui;
    bool _bUpdatePrintCommand;
    int searchType;
private slots:
    void onSearchPBClicked();
    void addToParamList();
    void removeFromParamList();
    void editFromParamList();
    void updateParamLEFromLst( );
    void clearContents(bool);
    void updateTableName(const QString& );
    void onTableNameEdited(const QString& iTableName);
    void onSearchCBToggled(bool);
};

#endif // GTAActPrintTableWidget_H


