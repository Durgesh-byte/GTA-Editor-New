
#ifndef GTAFailProfilePulseWgt_H
#define GTAFailProfilePulseWgt_H

#include <QWidget>
#include "GTAFailureProfileUIInterface.h"
#include "GTAFailureProfileBase.h"
namespace Ui {
    class GTAFailProfilePulseWgt;
}

class GTAFailProfilePulseWgt : public GTAFailureProfileUIInterface
{
    Q_OBJECT
    //Q_INTERFACES(GTAEquationUIInterface)
public:
    explicit GTAFailProfilePulseWgt(QWidget *parent = 0);
    ~GTAFailProfilePulseWgt();

    QString getPeriod() const;
    QString getDutyCycle() const;
    QString getOffset() const;
    QString getPhase() const;

    void setPeriod(const QString & iPeriod);
    void setDutyCycle(const QString & iAmp);
    void setOffset(const QString & iOffset);
    void setPhase(const QString & iPhase);

    //interfaces from GTAEquationUIInterface
    bool isValid()const;
    void clear() const ;
    bool getFailureProfile(GTAFailureProfileBase*& pEqBase) const;
    bool setFailureProfile(GTAFailureProfileBase*& pEqBase);

private:
    Ui::GTAFailProfilePulseWgt *ui;
};

#endif // GTAFailProfilePulseWgt_H


