
#ifndef GTAFAILPROFILERAMPSLOPEWGT_H
#define GTAFAILPROFILERAMPSLOPEWGT_H

#include <QWidget>
#include "GTAFailureProfileUIInterface.h"
#include "GTAFailureProfileBase.h"
namespace Ui {
    class GTAFailProfileRampSlopeWgt;
}

class GTAFailProfileRampSlopeWgt : public GTAFailureProfileUIInterface
{
    Q_OBJECT
    //Q_INTERFACES(GTAEquationUIInterface)
public:
    explicit GTAFailProfileRampSlopeWgt(QWidget *parent = 0);
    ~GTAFailProfileRampSlopeWgt();

    QString getOffset() const;
    QString getSlope() const;
   
   

    void setOffset(const QString & iPeriod);
    void setSlope(const QString & iAmp);
    

    //interfaces from GTAEquationUIInterface
    bool isValid()const;
    void clear() const ;
    bool getFailureProfile(GTAFailureProfileBase*& pEqBase) const;
    bool setFailureProfile(GTAFailureProfileBase*& pEqBase);

private:
    Ui::GTAFailProfileRampSlopeWgt *ui;
};

#endif // GTAFailProfileRampSlopeWgt_H


