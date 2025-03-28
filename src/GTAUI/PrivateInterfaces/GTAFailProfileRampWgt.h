
#ifndef GTAFAILPROFILERAMPWGT_H
#define GTAFAILPROFILERAMPWGT_H

#include <QWidget>
#include "GTAFailureProfileUIInterface.h"
#include "GTAFailureProfileBase.h"
namespace Ui {
    class GTAFailProfileRampWgt;
}

class GTAFailProfileRampWgt : public GTAFailureProfileUIInterface
{
    Q_OBJECT
    //Q_INTERFACES(GTAEquationUIInterface)
public:
    explicit GTAFailProfileRampWgt(QWidget *parent = 0);
    ~GTAFailProfileRampWgt();

    QString getStart() const;
    QString getStop() const;
   
   

    void setStart(const QString & iPeriod);
    void setStop(const QString & iAmp);
    

    //interfaces from GTAEquationUIInterface
    bool isValid()const;
    void clear() const ;
    bool getFailureProfile(GTAFailureProfileBase*& pEqBase) const;
    bool setFailureProfile(GTAFailureProfileBase*& pEqBase);

private:
    Ui::GTAFailProfileRampWgt *ui;
};

#endif // GTAFailProfileRampWgt_H


