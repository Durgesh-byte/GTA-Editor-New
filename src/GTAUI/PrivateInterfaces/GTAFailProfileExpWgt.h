
#ifndef GTAFailProfileExpWgt_H
#define GTAFailProfileExpWgt_H

#include <QWidget>
#include "GTAFailureProfileUIInterface.h"
#include "GTAFailureProfileBase.h"
namespace Ui {
    class GTAFailProfileExpWgt;
}

class GTAFailProfileExpWgt : public GTAFailureProfileUIInterface
{
    Q_OBJECT
    //Q_INTERFACES(GTAEquationUIInterface)
public:
    explicit GTAFailProfileExpWgt(QWidget *parent = 0);
    ~GTAFailProfileExpWgt();

    QString getStart() const;
    QString getStop() const;
    QString getTau() const;
    

    void setStart(const QString & iPeriod);
    void setStop(const QString & iAmp);
    void setTau(const QString & iOffset);
    

    //interfaces from GTAEquationUIInterface
    bool isValid()const;
    void clear() const ;
    bool getFailureProfile(GTAFailureProfileBase*& pEqBase) const;
    bool setFailureProfile(GTAFailureProfileBase*& pEqBase);

private:
    Ui::GTAFailProfileExpWgt *ui;
};

#endif // GTAFailProfileExpWgt_H


