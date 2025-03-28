#ifndef GTAEQUATIONRAMPWIDGET_H
#define GTAEQUATIONRAMPWIDGET_H

#include <QWidget>
#include "GTAEquationUIInterface.h"
#include "GTAGenSearchWidget.h"


namespace Ui {
    class GTAEquationRampWidget;
}

class GTAEquationRampWidget : public GTAEquationUIInterface
{
    Q_OBJECT
//    Q_INTERFACES(GTAEquationUIInterface)
public:
    explicit GTAEquationRampWidget(QWidget *parent = 0);
    ~GTAEquationRampWidget();

    QString getRiseTime() const;
    QString getStart() const;
    QString getEnd() const;


    void setRiseTime(const QString & iRiseTime);
    void setStart(const QString & iStart);
    void setEnd(const QString & iEnd);


    //interfaces from GTAEquationUIInterface
    bool isValid()const;
    void clear() const ;
    bool getEquation(GTAEquationBase*& pEqBase) const;
    bool setEquation(GTAEquationBase*& pEqBase);
    bool isEndValueOffset()const;


public slots:
     void onSolidCBToggled(bool iVal);
     void onSearchStartTimeClicked();
     void onSearchEndTimeClicked();
     void onSearchRiseTimeClicked();

     void onSearchWidgetStartOKClicked();
     void onSearchWidgetEndOKClicked();
     void onSearchWidgetRiseOKClicked();

	 void endCBChanged(int index);


private:
    Ui::GTAEquationRampWidget *ui;
    GTAGenSearchWidget*_pSearchWidgetStartTime;
    GTAGenSearchWidget* _pSearchWidgetEndTime;
    GTAGenSearchWidget* _pSearchWidgetRiseTime;
	QPixmap _graphEndValue;
	QPixmap _graphOffset;
};

#endif // GTAEQUATIONRAMPWIDGET_H
