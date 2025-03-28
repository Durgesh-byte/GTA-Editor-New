#ifndef GTAFAILUREPROFILEUIINTERFACE_H
#define GTAFAILUREPROFILEUIINTERFACE_H
#include "GTAFailureProfileBase.h"
#include "GTACore.h"

#pragma warning(push, 0)
#include <QWidget>
#include <QObject>
#pragma warning(pop)

class GTACore_SHARED_EXPORT GTAFailureProfileUIInterface : public QWidget
{
public:
    GTAFailureProfileUIInterface(QWidget *parent = 0);
    virtual ~GTAFailureProfileUIInterface();

    virtual bool isValid()const = 0;
    virtual void clear() const =0;
    virtual bool getFailureProfile(GTAFailureProfileBase*& pEqBase) const =0;
    virtual bool setFailureProfile(GTAFailureProfileBase*& pEqBase) =0;
};





//Q_DECLARE_INTERFACE ( GTAFailureProfileUIInterface, "EquationWidgetInterface" )
#endif // GTAFailureProfileUIInterface_H
