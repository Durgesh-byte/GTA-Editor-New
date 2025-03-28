
#ifndef GTAFailProfileIdleWgt_H
#define GTAFailProfileIdleWgt_H

#include <QWidget>
#include "GTAFailureProfileUIInterface.h"
#include "GTAFailureProfileBase.h"
namespace Ui {
    class GTAFailProfileIdleWgt;
}

class GTAFailProfileIdleWgt : public GTAFailureProfileUIInterface
{
    Q_OBJECT
    //Q_INTERFACES(GTAEquationUIInterface)
public:
    explicit GTAFailProfileIdleWgt(QWidget *parent = 0);
    ~GTAFailProfileIdleWgt();


    //interfaces from GTAEquationUIInterface
    bool isValid()const;
    void clear() const ;
    bool getFailureProfile(GTAFailureProfileBase*& pEqBase) const;
    bool setFailureProfile(GTAFailureProfileBase*& );

private:
    Ui::GTAFailProfileIdleWgt *ui;
};

#endif // GTAFailProfileIdleWgt_H


