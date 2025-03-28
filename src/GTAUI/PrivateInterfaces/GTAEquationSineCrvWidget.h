#ifndef GTAEQUATIONSINECRVWIDGET_H
#define GTAEQUATIONSINECRVWIDGET_H

#include <QWidget>
#include "GTAEquationUIInterface.h"

namespace Ui {
    class GTAEquationSineCrvWidget;
}

class GTAEquationSineCrvWidget : public GTAEquationUIInterface
{
    Q_OBJECT
    //Q_INTERFACES(GTAEquationUIInterface)
public:
     explicit GTAEquationSineCrvWidget(QWidget *parent = 0);
    ~GTAEquationSineCrvWidget();

    QString getGain() const;
    QString getTrigOperator() const;
    QString getPulsation() const;
    QString getPhase() const;
    QString getOffset() const;

    void setGain(const QString & iGain);
    void setTrigOperator(const QString & iOp);
    void setPulsation(const QString & iPulse);
    void setPhase(const QString & iPhase);
    void setOffset(const QString & iOffset);

    //interfaces from GTAEquationUIInterface
    bool isValid()const;
    void clear() const ;
    bool getEquation(GTAEquationBase*& pEqBase) const ;
    bool setEquation(GTAEquationBase*& pEqBase);

public slots:
     void onSolidCBToggled(bool);

private:
    Ui::GTAEquationSineCrvWidget *ui;
};

#endif // GTAEQUATIONSINECRVWIDGET_H
