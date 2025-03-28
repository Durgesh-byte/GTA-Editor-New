#ifndef GTAEQUATIONPULSEWIDGET_H
#define GTAEQUATIONPULSEWIDGET_H

#include <QWidget>
#include "GTAEquationUIInterface.h"
#include "GTAGenSearchWidget.h"

namespace Ui {
    class GTAEquationPulseWidget;
}

class GTAEquationPulseWidget : public GTAEquationUIInterface
{
    Q_OBJECT
    //Q_INTERFACES(GTAEquationUIInterface)
public:
    explicit GTAEquationPulseWidget(QWidget *parent = 0);
    ~GTAEquationPulseWidget();

    QString getPeriod() const;
    QString getMinValue() const;
    QString getMaxValue() const;
    QString getDirection() const;
    QString getDutyCycle() const;

    void setPeriod(const QString & iPeriod);
    void setMinValue(const QString & iMin);
    void setMaxValue(const QString & iMax);
    void setDirection(const QString & iDirection);
    void setDutyCycle(const QString & iDutyCycle);


    //interfaces from GTAEquationUIInterface
    bool isValid()const;
    void clear() const ;
    bool getEquation(GTAEquationBase*& pEqBase)const ;
    bool setEquation(GTAEquationBase*& pEqBase);

public slots:
     void onSolidCBToggled(bool );
	 void edgeCBChanged(int index);

     void onSearchPeriodClicked();
     void onSearchMaxValueClicked();
     void onSearchMinValueClicked();
     void onSearchDutyCycleClicked();
     void onSearchDirectionClicked();

     void onSearchWidgetPeriodOKClicked();
     void onSearchWidgetMaxOKClicked();
     void onSearchWidgetMinOKClicked();
     void onSearchWidgetDutyOKClicked();
     void onSearchWidgetDirectionOKClicked();

private:
    Ui::GTAEquationPulseWidget *ui;
	QPixmap _graphRisingEdge;
	QPixmap _graphFallingEdge;

    GTAGenSearchWidget* _pSearchWidgetPeriod;
    GTAGenSearchWidget* _pSearchWidgetMaxValue;
    GTAGenSearchWidget* _pSearchWidgetMinValue;
    GTAGenSearchWidget* _pSearchWidgetDutyCycle;
    GTAGenSearchWidget* _pSearchWidgetDirection;

    QList<QString>* _newDirections;
};

#endif // GTAEquationPulseWidget_H


