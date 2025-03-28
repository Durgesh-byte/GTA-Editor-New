#ifndef GTACheckFWCProcedureWidget_H
#define GTACheckFWCProcedureWidget_H

#include <QWidget>
#include <QLineEdit>
#include <QHash>
#include <QPushButton>
#include "GTACheckWidgetInterface.h"
namespace Ui {
    class GTACheckFWCProcedureWidget;
}

class GTACheckFwcProcedure;
class GTACheckFWCProcedureWidget : public GTACheckWidgetInterface
{
    Q_OBJECT

public:
    explicit GTACheckFWCProcedureWidget(QWidget *parent = 0);
    ~GTACheckFWCProcedureWidget();


    void setCheckCommand(const GTACheckFwcProcedure * ipCheckCmd);
    GTACheckFwcProcedure * getCheckCommand();

    virtual void clear();
    virtual bool isValid();
    virtual bool getCommand(GTACheckBase *& pCheckCmd);
    virtual bool setCommand(const GTACheckBase * pCheckCmd);

    bool hasTimeOut()const       ;
    bool hasDumpList()const      ;
    bool hasPrecision()const     ;
    bool hasSearchItem()const    ;
    bool hasConfigTime() const   ;
    bool hasActionOnFail()const  ;

signals:
    void searchProcedure(QLineEdit *&);

private:
    Ui::GTACheckFWCProcedureWidget *ui;

    QHash<int, QLineEdit*> _ProcedureTable;
    QHash<QPushButton * , QLineEdit *> _SearchButtonEditorMap;


    int getProcedureCount() const;
    QString getProcedure(int index) const;
    QString getCondition() const;

    void insertProcedure(int index, const QString & iVal);

private slots:
    void onSearchPBClicked();
};

#endif // GTACheckFWCProcedureWidget_H
