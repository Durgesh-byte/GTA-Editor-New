
#ifndef GTAFailProfileSawToothWgt_H
#define GTAFailProfileSawToothWgt_H

#include <QWidget>
#include "GTAFailureProfileUIInterface.h"
#include "GTAFailureProfileBase.h"
namespace Ui {
    class GTAFailProfileSawToothWgt;
}

class GTAFailProfileSawToothWgt : public GTAFailureProfileUIInterface
{
    Q_OBJECT
    //Q_INTERFACES(GTAEquationUIInterface)
public:
    explicit GTAFailProfileSawToothWgt(QWidget *parent = 0);
    ~GTAFailProfileSawToothWgt();

    QString getOffset() const;
    QString getPhase() const;
    QString getPeriod() const;
    

    void setOffset(const QString & iPeriod);
    void setPhase(const QString & iAmp);
    void setPeriod(const QString & iOffset);
    

    //interfaces from GTAEquationUIInterface
    bool isValid()const;
    void clear() const ;
    bool getFailureProfile(GTAFailureProfileBase*& pEqBase) const;
    bool setFailureProfile(GTAFailureProfileBase*& pEqBase);

private:
    Ui::GTAFailProfileSawToothWgt *ui;
};

#endif // GTAFailProfileSawToothWgt_H


