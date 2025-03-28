#ifndef GTAEQUATIONCRENELSWIDGET_H
#define GTAEQUATIONCRENELSWIDGET_H

#include <QWidget>
#include <QHash>
#include "GTAEquationUIInterface.h"

namespace Ui {
    class GTAEquationCrenelsWidget;
}

class GTAEquationCrenels;
class GTAEquationCrenelsWidget : public GTAEquationUIInterface
{
    Q_OBJECT
//    Q_INTERFACES(GTAEquationUIInterface)

public:
    explicit GTAEquationCrenelsWidget(QWidget *parent = 0);
    ~GTAEquationCrenelsWidget();

    int getLevelCount()const;
    int getLengthCount()const;
    QString getLevel(int iIdx) const;
    QString getLength(int iIdx) const;

    void insertLength(int index, const QString & iVal);
    void insertLevel(int index, const QString & iVal);

    //interfaces from #include "GTAEquationUIInterface.h"
    bool isValid() const;
    void clear() const ;
    bool getEquation(GTAEquationBase*& pEqBase) const ;
    bool setEquation(GTAEquationBase*& pEqBase);

public slots:
     void onSolidCBToggled(bool);

private:
    Ui::GTAEquationCrenelsWidget *ui;

     QHash<int, QWidget*> _LevelWgtList;
     QHash<int, QWidget*> _LengthWgtList;
};

#endif // GTAEQUATIONCRENELSWIDGET_H
