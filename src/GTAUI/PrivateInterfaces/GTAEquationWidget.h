#ifndef GTAEQUATIONWIDGET_H
#define GTAEQUATIONWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QHash>
#include <QKeyEvent>
#include "GTAEquationBase.h"
#include "GTAEquationUIInterface.h"
class GTAEquationCrenelsWidget;
class GTAEquationRampWidget;
class GTAEquationSineCrvWidget;
class GTAEquationTrapeWidget;
class GTAEquationItemValueWidget;
class GTAEquationBase;
class GTAEquationTriangleWidget;
class GTAEquationPulseWidget;
class GTAEquationStepWidget;
class GTAEquationSinusWidget;
class GTAEquationSawToothWidget;
class GTAEquationConstValueWidget;
class GTAEquationGainValueWidget;

namespace Ui {
    class GTAEquationWidget;
}

class GTAEquationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GTAEquationWidget(QWidget *parent = 0);
    ~GTAEquationWidget();

    //QString getEquation() const;

    QString getSimulation() const;

    bool getEquation(GTAEquationBase *& opEquation);
    void setEquation(const GTAEquationBase * ipEquation);

	bool getIsVmac() const { return _vmacElm; }
	void setIsVmac(const bool &iVmacElm) { _vmacElm = iVmacElm; }

    void clear();
    void reset();
    //void reset(const bool &listIndex);

signals:
    void okPressed();

protected:
    void keyPressEvent ( QKeyEvent * event );
private:

    bool isInvalidEquation(bool showMessage = true);


    Ui::GTAEquationWidget *ui;

    GTAEquationConstValueWidget*  _pConstValueWdt;
    GTAEquationBase  *   _pEquation;
    QList<QString>          _simulationList;
	bool					_vmacElm;


    
private slots:
    void onSimulationChange(const QString & simulationName);
    void onOKButtonClick();
    void onCancelButtonClick();
    QString getSimulationName( GTAEquationBase::EquationType iEquationType);

};

#endif // GTAEQUATIONWIDGET_H

class GTAEquationCrenelsWidget;
