
#ifndef GTAFAILPROFILECONSTANTWGT_H
#define GTAFAILPROFILECONSTANTWGT_H

#include <QWidget>
#include "GTAFailureProfileUIInterface.h"
#include "GTAFailureProfileBase.h"
namespace Ui {
    class GTAFailProfileConstantWgt;
}

class GTAFailProfileConstantWgt : public GTAFailureProfileUIInterface
{
    Q_OBJECT
    //Q_INTERFACES(GTAEquationUIInterface)
public:
    explicit GTAFailProfileConstantWgt(QWidget *parent = 0);
    ~GTAFailProfileConstantWgt();

    QString getConsant() const;
        

    void setConstant(const QString & iPeriod);
    

    //interfaces from GTAEquationUIInterface
    bool isValid()const;
    void clear() const ;
    bool getFailureProfile(GTAFailureProfileBase*& pEqBase) const;
    bool setFailureProfile(GTAFailureProfileBase*& pEqBase);

private:
    Ui::GTAFailProfileConstantWgt *ui;
};

#endif // GTAFailProfileConstantWgt_H


