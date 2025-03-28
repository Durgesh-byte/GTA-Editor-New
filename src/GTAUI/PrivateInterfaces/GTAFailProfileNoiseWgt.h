
#ifndef GTAFAILPROFILENOISEWGT_H
#define GTAFAILPROFILENOISEWGT_H

#include <QWidget>
#include "GTAFailureProfileUIInterface.h"
#include "GTAFailureProfileBase.h"
namespace Ui {
    class GTAFailProfileNoiseWgt;
}

class GTAFailProfileNoiseWgt : public GTAFailureProfileUIInterface
{
    Q_OBJECT
    //Q_INTERFACES(GTAEquationUIInterface)
public:
    explicit GTAFailProfileNoiseWgt(QWidget *parent = 0);
    ~GTAFailProfileNoiseWgt();

    QString getMean() const;
    QString getSigma() const;
    QString getSeed() const;
   

    void setMean(const QString & iPeriod);
    void setSigma(const QString & iAmp);
    void setSeed(const QString & iOffset);
   

    //interfaces from GTAEquationUIInterface
    bool isValid()const;
    void clear() const ;
    bool getFailureProfile(GTAFailureProfileBase*& pEqBase) const;
    bool setFailureProfile(GTAFailureProfileBase*& pEqBase);

private:
    Ui::GTAFailProfileNoiseWgt *ui;
};

#endif // GTAFailProfileNoiseWgt_H


