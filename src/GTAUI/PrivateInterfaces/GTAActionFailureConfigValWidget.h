#ifndef GTAACTIONFAILURECONFIGVALWIDGET_H
#define GTAACTIONFAILURECONFIGVALWIDGET_H

#include "GTAActionWidgetInterface.h"
#include "GTAActionPrintTable.h"
#include "GTAProfileSelectorWidget.h"
#include "GTAFailureConfig.h"

#pragma warning(push, 0)
#include <QWidget>
#include <QLineEdit>
#include <QListWidgetItem>
#pragma warning(pop)

class GTAActionPrint;
namespace Ui {
    class GTAActionFailureConfigValWidget;
}

class GTAActionFailureConfigValWidget : public QWidget 
{
    Q_OBJECT

public:
    explicit GTAActionFailureConfigValWidget(QWidget *parent = 0);
    ~GTAActionFailureConfigValWidget();

    
    void clear() ;
    bool isValid()const ;
    bool hasSearchItem(){return true;}

    QStringList getValues() const;
    void setValue(const QString & ) ;
    bool updatePrintTableInElement();
    int getSearchType();
    GTAFailureConfig*    getConfigValue()const {return _pConfigVal;}
    void setConfigValue(GTAFailureConfig*);
    void clearContents();
   void setConfigName(const QString& iName);
private:
    Ui::GTAActionFailureConfigValWidget *ui;
    bool _bUpdatePrintCommand;
    GTAProfileSelectorWidget*  _profileSelectorWidget;
    GTAFailureConfig*  _pConfigVal;
    GTAFailureProfileBase* _pCurrentProfile;

    
signals:

    void okPressed();
private slots:
    void onSearchPBClicked();
    void addToParamList();
    void removeFromParamList();
    void editFromParamList();
    void updateParamLEFromLst( );
    
    void updateTableName(const QString& );
    void displayWarning(const QString& );
    void showProfileSelectorWidget();

    void setCurrentProfile();
    void onOkPBClicked();
    void onCancelPBClicked();
    void onConfigItemClicked(QListWidgetItem *);

};

#endif // GTAActionFailureConfigValWidget_H


