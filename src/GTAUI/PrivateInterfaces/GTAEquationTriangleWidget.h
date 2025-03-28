
#ifndef GTAEQUATIONTRIANGLEWIDGET_H
#define GTAEQUATIONTRIANGLEWIDGET_H

#include <QWidget>
#include "GTAEquationUIInterface.h"
namespace Ui {
    class GTAEquationTriangleWidget;
}

class GTAEquationTriangleWidget :public GTAEquationUIInterface
{
    Q_OBJECT
    //Q_INTERFACES(GTAEquationUIInterface)

public:
    explicit GTAEquationTriangleWidget(QWidget *parent = 0);
    ~GTAEquationTriangleWidget();

    QString getPeriod() const;
    QString getMinValue() const;
    QString getMaxValue() const;
    QString getStartValue() const;
    QString getDirection() const;

    void setPeriod(const QString & iPeriod);
    void setMinValue(const QString & iMin);
    void setMaxValue(const QString & iMax);
    void setStartValue(const QString & iStart);
    void setDirection(const QString & iDirection);

    //interfaces from #include "GTAEquationUIInterface.h"
    bool isValid()const;
    void clear() const ;
    bool getEquation(GTAEquationBase*& pEqBase)const ;
    bool setEquation(GTAEquationBase*& pEqBase);

public slots:
     void onSolidCBToggled(bool);

private:
    Ui::GTAEquationTriangleWidget *ui;
};

#endif // GTAEQUATIONTRIANGLEWIDGET_H


