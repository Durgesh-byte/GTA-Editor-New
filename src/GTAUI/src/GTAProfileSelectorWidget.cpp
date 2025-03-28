#include "GTAProfileSelectorWidget.h"
#include "ui_GTAProfileSelectorWidget.h"
#include "GTACommandBuilder.h"
#include "GTAUtil.h"
#include "GTAFactory.h"
#include <QMessageBox>
#include <QDebug>
GTAProfileSelectorWidget::GTAProfileSelectorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GTAProfileSelectorWidget)
{
    ui->setupUi(this);

   QList<QString> simulationList;
    
    simulationList<<EQ_IDLE<<EQ_SAWTOOTH<<EQ_SQUARE<<EQ_SINUS<<EQ_EXP<<EQ_NOISE<<EQ_RAMP<<FEQ_CONST<<FEQ_RAMPSLOPE;//<<EQ_TRAPEZE<<EQ_CRENELS<<EQ_TRIANGLE<<EQ_STEP;
    ui->SimulationCB->clear();
    ui->SimulationCB->addItems(simulationList);


    connect(ui->SimulationCB,SIGNAL(currentIndexChanged(QString)),this,SLOT(onSimulationChange(QString)));
    connect(ui->OKPB,SIGNAL(clicked()),this,SLOT(onOKButtonClick()));
    /*connect(ui->CancePB,SIGNAL(clicked()),this,SLOT(hide()));

    ui->SimulationCB->setCurrentIndex(0);*/
    this->setWindowFlags(Qt::Dialog);

    GTAFactory* pFactory = &GTAFactory::getInstance();
    GTAFailureProfileUIInterface * pEqnWgt =NULL;
    pFactory->create(ui->SimulationCB->currentText(),pEqnWgt) ;
    if(pEqnWgt!=NULL)
    {
        ui->ProfileStackSW->addWidget(pEqnWgt);
        ui->ProfileStackSW->setCurrentWidget(pEqnWgt);
    }

   reset();
    
}

GTAProfileSelectorWidget::~GTAProfileSelectorWidget()
{
   clear();  
   delete ui;
}

bool GTAProfileSelectorWidget::getFailureProfile(GTAFailureProfileBase *& opEquation)
{
    bool rc = false;
   
    GTAFailureProfileUIInterface* pWidgetItf = dynamic_cast<GTAFailureProfileUIInterface*>(ui->ProfileStackSW->currentWidget());
    if (NULL!=pWidgetItf)
    {
        rc = pWidgetItf->getFailureProfile(opEquation);
    }
    return rc;
}

QString GTAProfileSelectorWidget::getSimulation() const
{
    return ui->SimulationCB->currentText();
}
void GTAProfileSelectorWidget::onSimulationChange(const QString & simulationName)
{
    clear();
   
    GTAFactory* pFactory = &GTAFactory::getInstance();
    GTAFailureProfileUIInterface * pEqnWgt =NULL;
    pFactory->create(simulationName,pEqnWgt) ;
    if(pEqnWgt!=NULL)
    {
        ui->ProfileStackSW->addWidget(pEqnWgt);
        ui->ProfileStackSW->setCurrentWidget(pEqnWgt);
    }
    else/**/
        qDebug()<<QString("Could not add widget for %1(Registration failure)").arg(simulationName);
}

void GTAProfileSelectorWidget::onOKButtonClick()
{
    if(isValidFailureProfile())
    {
        hide();
        emit okPressed();
    }
}

void GTAProfileSelectorWidget::setFailureProfile(const GTAFailureProfileBase * )
{
   
   /* if (NULL!=ipEquation)
    {

        GTAFailureProfileBase * pEquationExp = (GTAFailureProfileBase*)ipEquation;
        GTAFailureProfileBase::EquationType eqnType = ipEquation->getEquationType();
        QString simulationName = getSimulationName(eqnType);

        GTAFactory* pFactory = &GTAFactory::getInstance();
        GTAEquationUIInterface * pEqnWgt =NULL;
        pFactory->create(simulationName,pEqnWgt) ;
        if (pEqnWgt!=NULL)
        {
            clear();
            pEqnWgt->setEquation(pEquationExp);
            ui->EquationStackSW->addWidget(pEqnWgt);
            ui->EquationStackSW->setCurrentWidget(pEqnWgt);
        }
    }
    else
        reset();*/
   
    
}

bool GTAProfileSelectorWidget::isValidFailureProfile(bool showMessage)
{
    QString oErrorMsg = QString();
    bool compileError = false;
    QString simulationName = ui->SimulationCB->currentText();

    GTAFailureProfileUIInterface* pItf = dynamic_cast<GTAFailureProfileUIInterface *>(ui->ProfileStackSW->currentWidget());//(*) pWidget;
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
    return !compileError;
}

void GTAProfileSelectorWidget::clear()
{

    //GTAEquationUIInterface* pWidgetItf = dynamic_cast<GTAEquationUIInterface*>(ui->EquationStackSW->currentWidget());
    //if (NULL!=pWidgetItf)
    //{
    //   
    //    ui->EquationStackSW->removeWidget(pWidgetItf);
    //    pWidgetItf->setParent(NULL);
    //    delete pWidgetItf;
    //    pWidgetItf = NULL;

    //}
}
void GTAProfileSelectorWidget::reset()
{
    /*clear();
    ui->SimulationCB->setCurrentIndex(0);
    GTAFactory* pFactory = &GTAFactory::getInstance();
    GTAEquationUIInterface * pEqnWgt =NULL;
    pFactory->create(EQ_CONST,pEqnWgt) ;
    ui->EquationStackSW->addWidget(pEqnWgt);
    ui->EquationStackSW->setCurrentWidget(pEqnWgt);*/
}
QString GTAProfileSelectorWidget:: getSimulationName( GTAFailureProfileBase::EquationType iEquationType)
{
    QString simulationName;
    switch (iEquationType)
    {
    case GTAFailureProfileBase::CONST:
        simulationName = FEQ_CONST;
        break;
    case GTAFailureProfileBase::IDLE:
        simulationName = EQ_IDLE;
        break;
     case GTAFailureProfileBase::RAMP:
         simulationName = EQ_RAMP;
         break;
    case GTAFailureProfileBase::SINUS:
        simulationName = EQ_SINUS;
        break;
     case GTAFailureProfileBase::NOISE:
         simulationName = EQ_NOISE;
         break;
    case GTAFailureProfileBase::PULSE:
        simulationName = EQ_SQUARE;
        break;
    case GTAFailureProfileBase::EXPONENT:
         simulationName = EQ_EXP;
         break;
    case GTAFailureProfileBase::SAWTOOTH:
        simulationName = EQ_SAWTOOTH;
        break;
    case GTAFailureProfileBase::RAMPSLOPE:
        simulationName = FEQ_RAMPSLOPE;
        break;
    default:
        simulationName = QString();
        break;
    }
    return simulationName;
}
void GTAProfileSelectorWidget::keyPressEvent ( QKeyEvent * event )
{
    if (event->key()==Qt::Key_Escape )
    {
        this->hide();
    }
    QWidget::keyPressEvent(event);
}
void GTAProfileSelectorWidget::initialize()
{
   ui->SimulationCB->setCurrentIndex(0);

    //onSimulationChange(EQ_IDLE);
}