#include "GTAEquationStepWidget.h"
#include "GTAEquationStep.h"
#include "ui_GTAEquationStepWidget.h"

GTAEquationStepWidget::GTAEquationStepWidget(QWidget *parent) :
GTAEquationUIInterface(parent),
ui(new Ui::GTAEquationStepWidget)
{
    ui->setupUi(this);
    connect(ui->SearchTB, SIGNAL(clicked()), this, SLOT(onSearchPeriodClicked()));
    connect(ui->SearchTB2, SIGNAL(clicked()), this, SLOT(onSearchStartValueClicked()));
    connect(ui->SearchTB3, SIGNAL(clicked()), this, SLOT(onSearchEndValueClicked()));

    _pSearchWidgetPeriod = new GTAGenSearchWidget();
    _pSearchWidgetStartValue= new GTAGenSearchWidget();
    _pSearchWidgetEndValue= new GTAGenSearchWidget();

    if(_pSearchWidgetPeriod)
    {
        _pSearchWidgetPeriod->hide();
        connect(_pSearchWidgetPeriod, SIGNAL(okPressed()), this, SLOT(onSearchWidgetPeriodOKClicked()));

    }
    if (_pSearchWidgetStartValue)
    {
        _pSearchWidgetStartValue->hide();
        connect(_pSearchWidgetStartValue, SIGNAL(okPressed()), this, SLOT(onSearchWidgetStartOKClicked()));
    }
    if (_pSearchWidgetEndValue)
    {
        _pSearchWidgetEndValue->hide();
        connect(_pSearchWidgetEndValue, SIGNAL(okPressed()), this, SLOT(onSearchWidgetEndOKClicked()));
    }
}

GTAEquationStepWidget::~GTAEquationStepWidget()
{
    if (_pSearchWidgetPeriod != NULL)
    {
        delete _pSearchWidgetPeriod;
        _pSearchWidgetPeriod = NULL;
    }
    if (_pSearchWidgetStartValue != NULL)
    {
        delete _pSearchWidgetStartValue;
        _pSearchWidgetStartValue = NULL;
    }
    if (_pSearchWidgetEndValue != NULL)
    {
        delete _pSearchWidgetEndValue;
        _pSearchWidgetEndValue = NULL;
    }
    delete ui;
}

// GET Functions
QString GTAEquationStepWidget::getPeriod() const
{
    return ui->periodLE->text();
}
QString GTAEquationStepWidget::getStartValue() const
{
   return ui->startValueLE->text();
}
QString GTAEquationStepWidget::getEndValue() const
{
   return ui->endValueLE->text();
}

// SET Functions
void GTAEquationStepWidget::setPeriod(const QString & iPeriod)
{
    ui->periodLE->setText(iPeriod);
}
void GTAEquationStepWidget::setStartValue(const QString & iStart)
{
    ui->startValueLE->setText(iStart);
}
void GTAEquationStepWidget::setEndValue(const QString & iEnd)
{
    ui->endValueLE->setText(iEnd);
}

bool GTAEquationStepWidget::isValid() const
{
    if( ui->periodLE->text().isEmpty()     ||
        ui->startValueLE->text().isEmpty() ||
        ui->endValueLE->text().isEmpty()
       )
     return false;
    else
        return true;
}
void GTAEquationStepWidget::clear() const
{
    ui->periodLE->clear();
    ui->startValueLE->clear();
    ui->endValueLE->clear();
}

bool GTAEquationStepWidget::getEquation(GTAEquationBase*& pEqBase) const
{
    bool rc = false;
    GTAEquationStep* pEqnStep = NULL;
    if (NULL != pEqBase)
    {
        pEqnStep = dynamic_cast<GTAEquationStep*> (pEqBase);
    }
    else
    {
        pEqnStep = new GTAEquationStep();
    }
    if (NULL != pEqnStep)
    {
        pEqnStep->setPeriod(getPeriod());
        pEqnStep->setStartValue(getStartValue());
        pEqnStep->setEndValue(getEndValue());
        rc = true;
    }
    pEqBase = pEqnStep;
    return rc;
}

bool GTAEquationStepWidget::setEquation(GTAEquationBase*& pEqBase)
{
    GTAEquationStep* pEqnStep = NULL;
    if (NULL != pEqBase)
    {
        pEqnStep = dynamic_cast<GTAEquationStep*> (pEqBase);
        if (NULL != pEqnStep)
        {
            setPeriod(pEqnStep->getPeriod());
            setStartValue(pEqnStep->getStartValue());
            setEndValue(pEqnStep->getEndValue());
        }
    }
    else
    {
        clear();
    }
    return true;
}

 void GTAEquationStepWidget::onSolidCBToggled(bool) {}

 void GTAEquationStepWidget::onSearchPeriodClicked()
 {
     if (_pSearchWidgetPeriod != NULL)
     {
         _pSearchWidgetPeriod->setSearchType(GTAGenSearchWidget::PARAMETER);
         _pSearchWidgetPeriod->show();
     }
 }

 void GTAEquationStepWidget::onSearchStartValueClicked()
 {
     if (_pSearchWidgetStartValue != NULL)
     {
         _pSearchWidgetStartValue->setSearchType(GTAGenSearchWidget::PARAMETER);
         _pSearchWidgetStartValue->show();
     }
 }

 void GTAEquationStepWidget::onSearchEndValueClicked()
 {
     if (_pSearchWidgetEndValue != NULL)
     {
         _pSearchWidgetEndValue->setSearchType(GTAGenSearchWidget::PARAMETER);
         _pSearchWidgetEndValue->show();
     }
 }

 void GTAEquationStepWidget::onSearchWidgetPeriodOKClicked()
 {
     GTAICDParameter selectedItem = _pSearchWidgetPeriod->getSelectedItems();
     if (!selectedItem.getName().isEmpty())
     {
         ui->periodLE->setText(selectedItem.getName());
     }
     _pSearchWidgetPeriod->hide();
 }

 void GTAEquationStepWidget::onSearchWidgetStartOKClicked()
 {
     GTAICDParameter selectedItem = _pSearchWidgetStartValue->getSelectedItems();
     if (!selectedItem.getName().isEmpty())
     {
         ui->startValueLE->setText(selectedItem.getName());
     }
     _pSearchWidgetStartValue->hide();
 }

 void GTAEquationStepWidget::onSearchWidgetEndOKClicked()
 {
     GTAICDParameter selectedItem = _pSearchWidgetEndValue->getSelectedItems();
     if (!selectedItem.getName().isEmpty())
     {
         ui->endValueLE->setText(selectedItem.getName());
     }
     _pSearchWidgetEndValue->hide();
 }

#include "GTAWgtRegisterDef.h"
REGISTER_EQUATION_WIDGET(EQ_STEP,GTAEquationStepWidget)
