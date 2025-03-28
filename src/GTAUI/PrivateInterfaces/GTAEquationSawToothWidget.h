
#ifndef GTAEQUATIONSAWTOOTHWIDGET_H
#define GTAEQUATIONSAWTOOTHWIDGET_H

#include <QWidget>
#include "GTAEquationUIInterface.h"
#include "GTAEquationSawTooth.h"
namespace Ui {
    class GTAEquationSawToothWidget;
}

class GTAEquationSawToothWidget : public GTAEquationUIInterface
{
    Q_OBJECT
    //Q_INTERFACES(GTAEquationUIInterface)

public:
    explicit GTAEquationSawToothWidget(QWidget *parent = 0);
    ~GTAEquationSawToothWidget();

    QString getPeriod() const;
    QString getRampMode() const;
    QString getStartValue() const;
    QString getMinValue() const;
    QString getMaxValue() const;

    void setPeriod(const QString & iPeriod);
    void setRampMode(const QString & iRampMode);
    void setStartValue(const QString & iStartValue);
    void setMinValue(const QString & iMinValue);
    void setMaxValue(const QString & iMaxValue);

    //interfaces from GTAEquationUIInterface
    bool isValid()const;
    void clear() const ;
    bool getEquation(GTAEquationBase*& pEqBase) const;
    bool setEquation(GTAEquationBase*& pEqBase);

public slots:
     void onSolidCBToggled(bool );

private:
    Ui::GTAEquationSawToothWidget *ui;
};

#endif // GTAEquationSawToothWidget_H


