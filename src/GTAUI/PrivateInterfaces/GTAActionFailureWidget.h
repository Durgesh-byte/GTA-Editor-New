#ifndef GTAACTIONFAILUREWIDGET_H
#define GTAACTIONFAILUREWIDGET_H

#include "GTAActionWidgetInterface.h"
#include "GTAActionPrintTable.h"
#include "GTAActionFailureConfigValWidget.h"
#include "GTAFailureConfig.h"

#pragma warning(push, 0)
#include <QWidget>
#include <QLineEdit>
#pragma warning(pop)

class QListWidgetItem;
class GTAActionPrint;
namespace Ui {
    class GTAActionFailureWidget;
}

class GTAActionFailureWidget : public GTAActionWidgetInterface 
{
    Q_OBJECT

public:
    explicit GTAActionFailureWidget(QWidget *parent = 0);
    ~GTAActionFailureWidget();

    void setActionCommand(const GTAActionBase * ipActionCmd);
    bool getActionCommand(GTAActionBase*& opCmd)const;
    void clear() ;
    bool isValid()const ;
    bool hasSearchItem(){return true;}

    QStringList getValues() const;
    void setValue(const QString & ) ;
    bool updatePrintTableInElement();
    int getSearchType();
   
private:
    Ui::GTAActionFailureWidget *ui;
    bool _bUpdatePrintCommand;

    GTAActionFailureConfigValWidget* _pConfigWidget;
    mutable GTAFailureConfig* _pCurrentConfigVal;

    mutable QList <QPair<QString,GTAFailureConfig* >> _lstParamConfigVal;
    int searchType;

    void clearAll();
private slots:

    void onFailureSelectionChanged(QString);
    void onSearchParameterClicked();
    void onSearchFailureClicked();
    void showConfigWidget();
    void editConfiguration();

    void addToParamList();
    void removeFromParamList();
    void editFromParamList();
    void updateParamLEFromLst( );
    void clearContents(bool);
    void updateTableName(const QString& );
    void displayWarning(const QString& );
    void setConfigValue();
    void onParamValDoubleClicked(QListWidgetItem * ipItem);
    void onParamValClicked(QListWidgetItem * ipItem);
    void updateFailure(const QString&);
     

};

#endif // GTAActionFailureConfigValWidget_H


