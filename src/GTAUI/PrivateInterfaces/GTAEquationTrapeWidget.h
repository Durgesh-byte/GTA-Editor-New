#ifndef GTAEQUATIONTRAPEWIDGET_H
#define GTAEQUATIONTRAPEWIDGET_H

#include <QWidget>
#include <QHash>
#include "GTAEquationUIInterface.h"

class GTAEquationTrepeze;
namespace Ui {
    class GTAEquationTrapeWidget;
}

class GTAEquationTrapeWidget : public GTAEquationUIInterface
{
    Q_OBJECT
    //Q_INTERFACES(GTAEquationUIInterface)

public:
    explicit GTAEquationTrapeWidget(QWidget *parent = 0);
    ~GTAEquationTrapeWidget();

    int getRampCount() const;
    int getLengthCount() const;
    int getLevelCount() const;
    QString getRamp(int Idx) const;
    QString getLevel(int Idx) const;
    QString getLength(int Idx) const;

    void insertLength(int index, const QString & iVal);
    void insertLevel(int index, const QString & iVal);
    void insertRamp(int index, const QString & iVal);
   
    //interfaces from #include "GTAEquationUIInterface.h"
    bool isValid()const;
    void clear() const ;
    bool getEquation(GTAEquationBase*& pEqBase)const ;
    bool setEquation(GTAEquationBase*& pEqBase);

public slots:
     void onSolidCBToggled(bool iVal);

private:
    Ui::GTAEquationTrapeWidget *ui;

    QHash<int, QWidget*> _LevelWgtList;
    QHash<int, QWidget*> _LengthWgtList;
    QHash<int, QWidget*> _RampWgtList;
};

#endif // GTAEQUATIONTRAPEWIDGET_H
