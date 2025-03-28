#ifndef GTAEQUATIONGAINVALUEWIDGET_H
#define GTAEQUATIONGAINVALUEWIDGET_H

#include <QWidget>
#include "GTAEquationUIInterface.h"
namespace Ui {
    class GTAEquationGainValueWidget;
}

class GTAEquationGainValueWidget :  public GTAEquationUIInterface
{
    Q_OBJECT
   // Q_INTERFACES(GTAEquationUIInterface)
public:
    explicit GTAEquationGainValueWidget(QWidget *parent = 0);
    ~GTAEquationGainValueWidget();

    QString getGain() const;
    void setGain(const QString & iGain);

    //interfaces from #include "GTAEquationUIInterface.h"
    bool isValid() const;
    void clear() const ;
    bool getEquation(GTAEquationBase*& pEqBase) const;
    bool setEquation(GTAEquationBase*& pEqBase);

public slots:
     void onSolidCBToggled(bool);

private:
    Ui::GTAEquationGainValueWidget *ui;
};

#endif // GTAEQUATIONGAINVALUEWIDGET_H
