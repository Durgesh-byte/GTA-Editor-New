#ifndef GTAEQUATIONCONSTVALUEWIDGET_H
#define GTAEQUATIONCONSTVALUEWIDGET_H

#include <QWidget>
#include "GTAEquationUIInterface.h"
namespace Ui {
    class GTAEquationConstValueWidget;
}

class GTAEquationConstValueWidget :  public GTAEquationUIInterface
{
    Q_OBJECT
   // Q_INTERFACES(GTAEquationUIInterface)
public:
    explicit GTAEquationConstValueWidget(QWidget *parent = 0);
    ~GTAEquationConstValueWidget();

    //interfaces from #include "GTAEquationUIInterface.h"
    bool isValid()const;
    void clear() const ;
    bool getEquation(GTAEquationBase*& pEqBase)const ;
    bool setEquation(GTAEquationBase*& pEqBase);

	QString getValue() const;
	void setValue(const QString & iVal);

public slots:
    void onSolidCBToggled(bool );
    void ontimeSelected(bool iVal);
    void setCurrentIndex(int iIndex);

private:
    Ui::GTAEquationConstValueWidget *ui;
};

#endif // GTAEQUATIONCONSTVALUEWIDGET_H
