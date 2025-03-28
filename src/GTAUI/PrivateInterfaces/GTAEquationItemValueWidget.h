#ifndef GTAEQUATIONITEMVALUEWIDGET_H
#define GTAEQUATIONITEMVALUEWIDGET_H

#include <QWidget>
#include "GTAEquationUIInterface.h"
namespace Ui {
    class GTAEquationItemValueWidget;
}

class GTAEquationItemValueWidget :  public GTAEquationUIInterface
{
    Q_OBJECT
   // Q_INTERFACES(GTAEquationUIInterface)
public:
    explicit GTAEquationItemValueWidget(QWidget *parent = 0);
    void setLabel(const QString & iLabel);
    QString getValue() const;
    void setValue(const QString & iVal);
    ~GTAEquationItemValueWidget();


    //interfaces from #include "GTAEquationUIInterface.h"
    virtual bool isValid()const;
    virtual void clear() const ;
    virtual bool getEquation(GTAEquationBase*& pEqBase)const ;
    virtual bool setEquation(GTAEquationBase*& pEqBase);

public slots:
     void onSolidCBToggled(bool );

private:
    Ui::GTAEquationItemValueWidget *ui;
};

#endif // GTAEQUATIONITEMVALUEWIDGET_H
