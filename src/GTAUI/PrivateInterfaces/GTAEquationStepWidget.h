
#ifndef GTAEQUATIONSTEPWIDGET_H
#define GTAEQUATIONSTEPWIDGET_H

#include <QWidget>
#include "GTAEquationUIInterface.h"
#include "GTAGenSearchWidget.h"

namespace Ui {
    class GTAEquationStepWidget;
}

class GTAEquationStepWidget : public GTAEquationUIInterface
{
    Q_OBJECT
    //Q_INTERFACES(GTAEquationUIInterface)
public:
    explicit GTAEquationStepWidget(QWidget *parent = 0);
    ~GTAEquationStepWidget();

    QString getPeriod() const;
    QString getStartValue() const;
    QString getEndValue() const;
   

    void setPeriod(const QString & iPeriod);
    void setStartValue(const QString & iStart);
    void setEndValue(const QString & iEnd);
  

    //interfaces from GTAEquationUIInterface
    bool isValid()const;
    void clear() const ;
    bool getEquation(GTAEquationBase*& pEqBase) const;
    bool setEquation(GTAEquationBase*& pEqBase);

public slots:
     void onSolidCBToggled(bool);

     void onSearchPeriodClicked();
     void onSearchStartValueClicked();
     void onSearchEndValueClicked();

     void onSearchWidgetPeriodOKClicked();
     void onSearchWidgetStartOKClicked();
     void onSearchWidgetEndOKClicked();

private:
    Ui::GTAEquationStepWidget *ui;
    GTAGenSearchWidget* _pSearchWidgetPeriod;
    GTAGenSearchWidget* _pSearchWidgetStartValue;
    GTAGenSearchWidget* _pSearchWidgetEndValue;
};

#endif // GTAEquationStepWidget_H


