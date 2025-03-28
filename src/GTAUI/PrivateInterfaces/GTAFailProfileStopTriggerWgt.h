
#ifndef GTAFAILPROFILESTOPTRIGGERWGT_H
#define GTAFAILPROFILESTOPTRIGGERWGT_H

#include <QWidget>
#include "GTAFailureProfileUIInterface.h"
#include "GTAFailureProfileBase.h"
namespace Ui {
    class GTAFailProfileStopTriggerWgt;
}

class GTAFailProfileStopTriggerWgt : public GTAFailureProfileUIInterface
{
    Q_OBJECT
    //Q_INTERFACES(GTAEquationUIInterface)
public:
    explicit GTAFailProfileStopTriggerWgt(QWidget *parent = 0);
    ~GTAFailProfileStopTriggerWgt();

    QString getTimeOut() const;
   
   

    void setTimeOut(const QString & iPeriod);
    
   

    //interfaces from GTAEquationUIInterface
    bool isValid()const;
    void clear() const ;
    bool getFailureProfile(GTAFailureProfileBase*& pEqBase) const;
    bool setFailureProfile(GTAFailureProfileBase*& pEqBase);

private:
    Ui::GTAFailProfileStopTriggerWgt *ui;
};

#endif // GTAFailProfileStopTriggerWgt_H


