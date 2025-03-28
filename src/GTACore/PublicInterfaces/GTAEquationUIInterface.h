#ifndef GTAEQUATIONUIINTERFACE_H
#define GTAEQUATIONUIINTERFACE_H
#include "GTAEquationBase.h"
#include "GTACore.h"

#pragma warning(push, 0)
#include <QWidget>
#include <QObject>
#pragma warning(pop)


class GTACore_SHARED_EXPORT GTAEquationUIInterface : public QWidget
{
public:
    GTAEquationUIInterface(QWidget *parent = 0);
    virtual ~GTAEquationUIInterface();

    virtual bool isValid()const = 0;
    virtual void clear() const = 0;
    virtual bool getEquation(GTAEquationBase*& pEqBase) const = 0;
    virtual bool setEquation(GTAEquationBase*& pEqBase) = 0;

public slots:
    virtual void onSolidCBToggled(bool iVal) =  0;
};





//Q_DECLARE_INTERFACE ( GTAEquationUIInterface, "EquationWidgetInterface" )
#endif // GTAEQUATIONUIINTERFACE_H
