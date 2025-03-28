#ifndef GTAActionCallProceduresWgt_H
#define GTAActionCallProceduresWgt_H

#include "GTAActionWidgetInterface.h"
#include "GTAActionCallProcedures.h"
#include "GTAGenSearchWidget.h"

#pragma warning(push, 0)
#include <QWidget>
#include <QLineEdit>
#include <QListWidgetItem>
#pragma warning(pop)

class GTAActionPrint;
namespace Ui {
    class GTAActionCallProceduresWgt;
}

class GTAActionCallProceduresWgt : public GTAActionWidgetInterface
{
    Q_OBJECT

public:
    explicit GTAActionCallProceduresWgt(QWidget *parent = 0);
    ~GTAActionCallProceduresWgt();

   
    void setValue(const QString & ) ;
    
    

    void setActionCommand(const GTAActionBase * ipActionCmd);
    bool getActionCommand(GTAActionBase*& opCmd)const;
    void clear() ;
    bool isValid()const ;
    bool hasSearchItem(){return true;}

    int getSearchType();

signals:
    void searchObject(QLineEdit *&);
    void addCall();
    void editCall();

private:
    Ui::GTAActionCallProceduresWgt *ui;
    QStringList getValues() const;
    //QStringList  _CallProcedures;
    QList<GTACallProcItem> _CallProcedures;

    GTAGenSearchWidget * _pSearchWidget;
    QString _RefFileUUID;

private:

    void addTag( const TagVariablesDesc& iTagVar , const QString& iTagVal=QString());
    void setTagTable(QString isFileName);
    bool tagValuePairsOK();
    void resetTableWidget();
    QList<TagVariablesDesc> getTagListForElement(QString isFileName,bool isUUID);

private slots:
    void onSearchPBClicked();
    void addCallProcedures();
    void removeFromCallList();
    void editFromCallList();
    void updateParamLEFromLst( ); 
    void updateParamLEFromLst(QListWidgetItem* ); 
    void updateParamLEFromLst(int ); 
    void toggleTagTableView( int);
    void callProcedureLEChanged( const QString &);
    void prepareToAddCall();
    void prepareToEditCall();

public slots:
    void onSearchOKPBClick();
    
};

#endif // GTAActionCallProceduresWgt_H

//void setActionCommand(const GTAActionBase * ipActionCmd);
//bool getActionCommand(GTAActionBase* opCmd)const;
//void clear() ;
//bool isValid()const ;
