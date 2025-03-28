#ifndef GTAProfileSelectorWidget_H
#define GTAProfileSelectorWidget_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QHash>
#include <QKeyEvent>
#include "GTAFailureProfileBase.h"


namespace Ui {
    class GTAProfileSelectorWidget;
}

class GTAProfileSelectorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GTAProfileSelectorWidget(QWidget *parent = 0);
    ~GTAProfileSelectorWidget();

    //QString getEquation() const;

    QString getSimulation() const;

    bool getFailureProfile(GTAFailureProfileBase *& opEquation);
    void setFailureProfile(const GTAFailureProfileBase * );
    void initialize();

    void clear();
    void reset();
signals:
    void okPressed();
protected:
    void keyPressEvent ( QKeyEvent * event );
private:

    bool isValidFailureProfile(bool showMessage = true);


    Ui::GTAProfileSelectorWidget *ui;

//    GTAEquationConstValueWidget*  _pConstValueWdt;
   // GTAEquationBase  *   _pEquation;


    
private slots:
    void onSimulationChange(const QString & simulationName);
    void onOKButtonClick();
    QString getSimulationName( GTAFailureProfileBase::EquationType iEquationType);

};

#endif // GTAEQUATIONWIDGET_H

class GTAEquationCrenelsWidget;
