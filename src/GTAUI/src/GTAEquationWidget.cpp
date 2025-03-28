#include "GTAEquationWidget.h"
#include "ui_GTAEquationWidget.h"
#include "GTACommandBuilder.h"

#include "GTAUtil.h"
#include "GTAFactory.h"
#include <QMessageBox>
#include <QDebug>
GTAEquationWidget::GTAEquationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GTAEquationWidget)
{
    ui->setupUi(this);

    // This list will be displayed to choose a simulation name
	_simulationList << EQ_CRENELS << EQ_GAIN << EQ_RAMP << EQ_SAWTOOTH << EQ_SINUS << EQ_SQUARE << EQ_STEP << EQ_TRAPEZE << EQ_TRIANGLE;
    
    // We MUST use the initial list to have all choices before switching list according to MEDIA
    ui->SimulationCB->addItems(_simulationList);
    this->setWindowFlags(Qt::Dialog|Qt::CustomizeWindowHint|Qt::WindowTitleHint);

    connect(ui->SimulationCB,SIGNAL(currentIndexChanged(QString)),this,SLOT(onSimulationChange(QString)));
    connect(ui->OKPB,SIGNAL(clicked()),this,SLOT(onOKButtonClick()));
    connect(ui->CancePB,SIGNAL(clicked()),this,SLOT(onCancelButtonClick()));

    //following two items are hidden since current index of SimulationCB is set to 0 which is constant option.
    reset();
}

GTAEquationWidget::~GTAEquationWidget()
{
    clear();
    delete ui;
}

bool GTAEquationWidget::getEquation(GTAEquationBase *& opEquation)
{
    bool rc = false;

    GTAEquationUIInterface* pWidgetItf = dynamic_cast<GTAEquationUIInterface*>(ui->EquationStackSW->currentWidget());
    if (NULL!=pWidgetItf)
    {
        rc = pWidgetItf->getEquation(opEquation);

        if (opEquation != nullptr)
        {
            if(ui->isSolidCB->isChecked())
                opEquation->setSolidState("1");
            else
                opEquation->setSolidState("0");

            if(ui->isContinousCB->isChecked())
                opEquation->setContinousState("1");
            else
                opEquation->setContinousState("0");
        }
    }
    return rc;
}

QString GTAEquationWidget::getSimulation() const
{
    return ui->SimulationCB->currentText();
}
void GTAEquationWidget::onSimulationChange(const QString & simulationName)
{
    clear();

    GTAFactory* pFactory = &GTAFactory::getInstance();
    GTAEquationUIInterface * pEqnWgt = NULL;
    pFactory->create(simulationName,pEqnWgt) ;
    if(pEqnWgt!=NULL)
    {
        connect(ui->isSolidCB,SIGNAL(toggled(bool)),pEqnWgt,SLOT(onSolidCBToggled(bool)));
        ui->isSolidCB->setChecked(true);
        if(simulationName == EQ_CONST )
        {
            ui->isContinousCB->setVisible(false);
            ui->isSolidCB->setVisible(false);
        }
        else
        {
            ui->isContinousCB->setVisible(true);
            ui->isSolidCB->setVisible(true);
        }
        ui->EquationStackSW->addWidget(pEqnWgt);
        ui->EquationStackSW->setCurrentWidget(pEqnWgt);
    }
    else
        qDebug()<<QString("Could not add widget for %1(Registration failure)").arg(simulationName);
}

void GTAEquationWidget::onOKButtonClick()
{
    if(! isInvalidEquation())
    {
        hide();
        emit okPressed();
    }
}

void GTAEquationWidget::onCancelButtonClick()
{
    clear();
    hide();
}

void GTAEquationWidget::setEquation(const GTAEquationBase * ipEquation)
{
    if (NULL!=ipEquation)
    {
        GTAEquationBase * pEquationExp = (GTAEquationBase*)ipEquation;
        GTAEquationBase::EquationType eqnType = ipEquation->getEquationType();
        QString simulationName = getSimulationName(eqnType);

        GTAFactory* pFactory = &GTAFactory::getInstance();
        GTAEquationUIInterface * pEqnWgt =NULL;
        pFactory->create(simulationName, pEqnWgt) ;
        if (pEqnWgt!=NULL)
        {
            clear();
            connect(ui->isSolidCB,SIGNAL(toggled(bool)),pEqnWgt,SLOT(onSolidCBToggled(bool)));
            ui->isSolidCB->setChecked(true);
            pEqnWgt->setEquation(pEquationExp);
            ui->EquationStackSW->addWidget(pEqnWgt);
            ui->EquationStackSW->setCurrentWidget(pEqnWgt);
            if(pEquationExp->getSolidState() == "1")
                ui->isSolidCB->setChecked(true);
            else
                ui->isSolidCB->setChecked(false);

            if(pEquationExp->getContinousState() == "1")
                ui->isContinousCB->setChecked(true);
            else
                ui->isContinousCB->setChecked(false);

            int idx = ui->SimulationCB->findText(simulationName);
			
			// For all equations except CONST
			if (simulationName != EQ_CONST)
			{
				if (idx != -1)
				{
					ui->SimulationCB->setCurrentIndex(idx);
				}

			}
		}
    }
    else
        reset();    
}

bool GTAEquationWidget::isInvalidEquation(bool showMessage)
{
    QString oErrorMsg = QString();
    bool compileError = false;
    QString simulationName = ui->SimulationCB->currentText();

    GTAEquationUIInterface* pItf = dynamic_cast<GTAEquationUIInterface *>(ui->EquationStackSW->currentWidget());//(*) pWidget;
    if (pItf)
    {
        if(!pItf->isValid())
            compileError=true;

    }

    if(compileError)
    {
        if(showMessage)
        {
            oErrorMsg="One of more fields are empty";
            QMessageBox::critical(0,"Compilation Error",oErrorMsg,QMessageBox::Ok);
        }
    }
    return compileError;
}

void GTAEquationWidget::clear()
{

    GTAEquationUIInterface* pWidgetItf = dynamic_cast<GTAEquationUIInterface*>(ui->EquationStackSW->currentWidget());
    if (NULL!=pWidgetItf)
    {

        ui->EquationStackSW->removeWidget(pWidgetItf);
        pWidgetItf->setParent(NULL);
        delete pWidgetItf;
        pWidgetItf = NULL;

    }
}
/*
 * This function allow to modify the list of equations according to the simulation name
 */
void GTAEquationWidget::reset()
{
    clear();
	ui->SimulationCB->setCurrentIndex(0);
    //following two items are hidden since current index of SimulationCB is set to 0 which is constant option
    if (!ui->SimulationCB->currentText().isEmpty()) {
        onSimulationChange(ui->SimulationCB->currentText());
    }
    else {
        ui->isContinousCB->setVisible(false);
        ui->isSolidCB->setVisible(false);
    }
    GTAFactory* pFactory = &GTAFactory::getInstance();
    GTAEquationUIInterface * pEqnWgt = NULL;
	
	// We have to get the SimulationName in order to create the right widget
	QString simulationName = getSimulation();
    // Shall be modified to make it more dynamic - Get the the first item into the list
    pFactory->create(simulationName,pEqnWgt);
    if(pEqnWgt)
    {
        connect(ui->isSolidCB,SIGNAL(toggled(bool)),pEqnWgt,SLOT(onSolidCBToggled(bool)));
        ui->isSolidCB->setChecked(true);
        ui->EquationStackSW->addWidget(pEqnWgt);
        ui->EquationStackSW->setCurrentWidget(pEqnWgt);
    }
}

QString GTAEquationWidget:: getSimulationName( GTAEquationBase::EquationType iEquationType)
{
    QString simulationName;
    switch (iEquationType)
    {
	// The CONST case does not exist anymore
	case GTAEquationBase::CONST:
		simulationName = EQ_CONST;
		break;
    case GTAEquationBase::GAIN:
        simulationName = EQ_GAIN;
        break;
    case GTAEquationBase::RAMP:
        simulationName = EQ_RAMP;
        break;
    case GTAEquationBase::TREPEZE:
        simulationName = EQ_TRAPEZE;
        break;
    case GTAEquationBase::CRENELS:
        simulationName = EQ_CRENELS;
        break;
    case GTAEquationBase::SINECRV:
        simulationName = EQ_SINECRV;
        break;
    case GTAEquationBase::SINUS:
        simulationName = EQ_SINUS;
        break;
    case GTAEquationBase::TRIANGLE:
        simulationName = EQ_TRIANGLE;
        break;
    case GTAEquationBase::SQUARE:
        simulationName = EQ_SQUARE;
        break;
    case GTAEquationBase::STEP:
        simulationName = EQ_STEP;
        break;
    case GTAEquationBase::SAWTOOTH:
        simulationName = EQ_SAWTOOTH;
        break;
    default:
        simulationName = QString();
        break;
    }
    return simulationName;
}
void GTAEquationWidget::keyPressEvent ( QKeyEvent * event )
{
    if (event->key()==Qt::Key_Escape )
    {
        this->hide();
    }
    QWidget::keyPressEvent(event);
}
