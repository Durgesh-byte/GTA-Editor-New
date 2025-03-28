
#ifndef GTAFAILPROFILESINEWGT_H
#define GTAFAILPROFILESINEWGT_H

#include <QWidget>
#include "GTAFailureProfileUIInterface.h"
#include "GTAFailureProfileBase.h"
namespace Ui {
    class GTAFailProfileSineWgt;
}

class GTAFailProfileSineWgt : public GTAFailureProfileUIInterface
{
    Q_OBJECT
    //Q_INTERFACES(GTAEquationUIInterface)
public:
    explicit GTAFailProfileSineWgt(QWidget *parent = 0);
    ~GTAFailProfileSineWgt();

    QString getPeriod() const;
    QString getAmplitude() const;
    QString getOffset() const;
    QString getPhase() const;

    void setPeriod(const QString & iPeriod);
    void setAmplitude(const QString & iAmp);
    void setOffset(const QString & iOffset);
    void setPhase(const QString & iPhase);

    //interfaces from GTAEquationUIInterface
    bool isValid()const;
    void clear() const ;
    bool getFailureProfile(GTAFailureProfileBase*& pEqBase) const;
    bool setFailureProfile(GTAFailureProfileBase*& pEqBase);

private:
    Ui::GTAFailProfileSineWgt *ui;
};

#endif // GTAFailProfileSineWgt_H


