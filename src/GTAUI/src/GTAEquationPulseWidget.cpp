#include "GTAEquationPulseWidget.h"
#include "GTAEquationPulse.h"
#include "ui_GTAEquationPulseWidget.h"

GTAEquationPulseWidget::GTAEquationPulseWidget(QWidget *parent) :GTAEquationUIInterface(parent),
ui(new Ui::GTAEquationPulseWidget)
{
    ui->setupUi(this);
    _newDirections = new QList<QString>();

    connect(ui->SearchPB, SIGNAL(clicked()), this, SLOT(onSearchPeriodClicked()));
    connect(ui->SearchPB2, SIGNAL(clicked()), this, SLOT(onSearchMaxValueClicked()));
    connect(ui->SearchPB3, SIGNAL(clicked()), this, SLOT(onSearchMinValueClicked()));
    connect(ui->SearchPB4, SIGNAL(clicked()), this, SLOT(onSearchDutyCycleClicked()));
    connect(ui->SearchPB5, SIGNAL(clicked()), this, SLOT(onSearchDirectionClicked()));

    _pSearchWidgetPeriod = new GTAGenSearchWidget();
    _pSearchWidgetMaxValue = new GTAGenSearchWidget();
    _pSearchWidgetMinValue = new GTAGenSearchWidget();
    _pSearchWidgetDutyCycle = new GTAGenSearchWidget();
    _pSearchWidgetDirection = new GTAGenSearchWidget();
    
    if (_pSearchWidgetPeriod)
    {
        _pSearchWidgetPeriod->hide();
        connect(_pSearchWidgetPeriod, SIGNAL(okPressed()), this, SLOT(onSearchWidgetPeriodOKClicked()));

    }

    if (_pSearchWidgetMaxValue)
    {
        _pSearchWidgetMaxValue->hide();
        connect(_pSearchWidgetMaxValue, SIGNAL(okPressed()), this, SLOT(onSearchWidgetMaxOKClicked()));

    }

    if (_pSearchWidgetMinValue)
    {
        _pSearchWidgetMinValue->hide();
        connect(_pSearchWidgetMinValue, SIGNAL(okPressed()), this, SLOT(onSearchWidgetMinOKClicked()));

    }

    if (_pSearchWidgetDutyCycle)
    {
        _pSearchWidgetDutyCycle->hide();
        connect(_pSearchWidgetDutyCycle, SIGNAL(okPressed()), this, SLOT(onSearchWidgetDutyOKClicked()));

    }
    if (_pSearchWidgetDirection)
    {
        _pSearchWidgetDirection->hide();
        connect(_pSearchWidgetDirection, SIGNAL(okPressed()), this, SLOT(onSearchWidgetDirectionOKClicked()));

    }

	_graphRisingEdge.load(":/images/forms/img/Square_RisingEdge.png");
	_graphFallingEdge.load(":/images/forms/img/Square_FallingEdge.png");
	connect(ui->directionCB, SIGNAL(currentIndexChanged(int)), this, SLOT(edgeCBChanged(int)));
}

GTAEquationPulseWidget::~GTAEquationPulseWidget()
{
    if (_pSearchWidgetPeriod != NULL)
    {
        delete _pSearchWidgetPeriod;
        _pSearchWidgetPeriod = NULL;
    }
    if (_pSearchWidgetMaxValue != NULL)
    {
        delete _pSearchWidgetMaxValue;
        _pSearchWidgetMaxValue = NULL;
    }
    if (_pSearchWidgetMinValue != NULL)
    {
        delete _pSearchWidgetMinValue;
        _pSearchWidgetMinValue = NULL;
    }
    if (_pSearchWidgetDutyCycle != NULL)
    {
        delete _pSearchWidgetDutyCycle;
        _pSearchWidgetDutyCycle = NULL;
    }
    if (_pSearchWidgetDirection != NULL)
    {
        delete _pSearchWidgetDirection;
        _pSearchWidgetDirection = NULL;
    }
    delete ui;
    delete _newDirections;
}

// GET Functions
QString GTAEquationPulseWidget::getPeriod() const
{
    return ui->periodLE->text();
}
QString GTAEquationPulseWidget::getMinValue() const
{
    return ui->minValueLE->text();
}
QString GTAEquationPulseWidget::getMaxValue() const
{
    return ui->maxValueLE->text();
}
QString GTAEquationPulseWidget::getDirection() const
{
    QString sDirection = ui->directionCB->currentText();
    return sDirection;
}
QString GTAEquationPulseWidget::getDutyCycle() const
{
    return ui->dutyCycleLE->text();
}

// SET Functions
void GTAEquationPulseWidget::setPeriod(const QString & iPeriod)
{
     ui->periodLE->setText(iPeriod);
}
void GTAEquationPulseWidget::setMinValue(const QString & iMin)
{
    ui->minValueLE->setText(iMin);
}
void GTAEquationPulseWidget::setMaxValue(const QString & iMax)
{
    ui->maxValueLE->setText(iMax);
}
void GTAEquationPulseWidget::setDirection(const QString & iDirection)
{
    int index = ui->directionCB->findText(iDirection);

    if (index != -1) // Item found
    {
        ui->directionCB->setCurrentIndex(index);
    }
    else // Item not found
    {
        ui->directionCB->addItem(iDirection);
        ui->directionCB->setCurrentText(iDirection);
    }
}
void GTAEquationPulseWidget::setDutyCycle(const QString & iDutyCycle)
{
    ui->dutyCycleLE->setText(iDutyCycle);
}

 bool GTAEquationPulseWidget::isValid()const
 {
     if( ui->periodLE->text().isEmpty()   || ui->minValueLE->text().isEmpty() ||
         ui->maxValueLE->text().isEmpty() || ui->dutyCycleLE->text().isEmpty() )
     {
         return false;
     }
     else
         return true;
 }
 void GTAEquationPulseWidget::clear() const
 {
     ui->periodLE->clear();
     ui->minValueLE->clear(); 
     ui->maxValueLE->clear();
     if (_newDirections->isEmpty())
     {
         ui->directionCB->setCurrentIndex(0);
     }
     else
     {
         // Iterate through _newDirections and remove each item from the combo box
         foreach(const QString & direction, *_newDirections) {
             int index = ui->directionCB->findText(direction);
             if (index != -1) { // -1 means item not found
                 ui->directionCB->removeItem(index);
             }
         }
         _newDirections->clear();
     }
     ui->dutyCycleLE->clear();
 }

 bool GTAEquationPulseWidget::getEquation(GTAEquationBase*& pEqBase)const
 {
     bool rc = false;
     GTAEquationPulse* pEqnPulse = NULL;
     if (NULL != pEqBase)
     {
         pEqnPulse = dynamic_cast<GTAEquationPulse*> (pEqBase);
     }
     else
     {
         pEqnPulse = new GTAEquationPulse();
     }
     if (NULL != pEqnPulse)
     {
         pEqnPulse->setMaxValue(getMaxValue());
         pEqnPulse->setMinValue(getMinValue());
         pEqnPulse->setDirection(getDirection());
         pEqnPulse->setDutyCycle(getDutyCycle());
         pEqnPulse->setPeriod(getPeriod());
         rc = true;
     }
     pEqBase = pEqnPulse;
     return rc;
 }

 bool GTAEquationPulseWidget::setEquation(GTAEquationBase*& pEqBase)
 {
     GTAEquationPulse* pEqnPulse = NULL;
     if (NULL != pEqBase)
     {
         pEqnPulse = dynamic_cast<GTAEquationPulse*> (pEqBase);
         if (NULL != pEqnPulse)
         {
             setMaxValue(pEqnPulse->getMaxValue());
             setMinValue(pEqnPulse->getMinValue());
             setDirection(pEqnPulse->getDirection());
             setPeriod(pEqnPulse->getPeriod());
             setDutyCycle(pEqnPulse->getDutyCycle());
         }
     }
     else
     {
         clear();
     }
     return true;
 }

 void GTAEquationPulseWidget::edgeCBChanged(int index)
 {
	 if (index == 0)
	 {
		 ui->label->setPixmap(_graphRisingEdge);
	 }
	 else
	 {
		 ui->label->setPixmap(_graphFallingEdge);
	 }
 }

 void GTAEquationPulseWidget::onSolidCBToggled(bool) {}

 void GTAEquationPulseWidget::onSearchPeriodClicked()
 {
     if (_pSearchWidgetPeriod != NULL)
     {
         _pSearchWidgetPeriod->setSearchType(GTAGenSearchWidget::PARAMETER);
         _pSearchWidgetPeriod->show();
     }
 }

 void GTAEquationPulseWidget::onSearchMaxValueClicked()
 {
     if (_pSearchWidgetMaxValue != NULL)
     {
         _pSearchWidgetMaxValue->setSearchType(GTAGenSearchWidget::PARAMETER);
         _pSearchWidgetMaxValue->show();
     }
 }

 void GTAEquationPulseWidget::onSearchMinValueClicked()
 {
     if (_pSearchWidgetMinValue != NULL)
     {
         _pSearchWidgetMinValue->setSearchType(GTAGenSearchWidget::PARAMETER);
         _pSearchWidgetMinValue->show();
     }
 }

 void GTAEquationPulseWidget::onSearchDutyCycleClicked()
 {
     if (_pSearchWidgetDutyCycle != NULL)
     {
         _pSearchWidgetDutyCycle->setSearchType(GTAGenSearchWidget::PARAMETER);
         _pSearchWidgetDutyCycle->show();
     }
 }

 void GTAEquationPulseWidget::onSearchDirectionClicked()
 {
     if (_pSearchWidgetDirection != NULL)
     {
         _pSearchWidgetDirection->setSearchType(GTAGenSearchWidget::PARAMETER);
         _pSearchWidgetDirection->show();
     }
 }

 void GTAEquationPulseWidget::onSearchWidgetPeriodOKClicked()
 {
     GTAICDParameter selectedItem = _pSearchWidgetPeriod->getSelectedItems();
     if (!selectedItem.getName().isEmpty())
     {
         ui->periodLE->setText(selectedItem.getName());
     }
     _pSearchWidgetPeriod->hide();
 }

 void GTAEquationPulseWidget::onSearchWidgetMaxOKClicked()
 {
     GTAICDParameter selectedItem = _pSearchWidgetMaxValue->getSelectedItems();
     if (!selectedItem.getName().isEmpty())
     {
         ui->maxValueLE->setText(selectedItem.getName());
     }
     _pSearchWidgetMaxValue->hide();
 }

 void GTAEquationPulseWidget::onSearchWidgetMinOKClicked()
 {
     GTAICDParameter selectedItem = _pSearchWidgetMinValue->getSelectedItems();
     if (!selectedItem.getName().isEmpty())
     {
         ui->minValueLE->setText(selectedItem.getName());
     }
     _pSearchWidgetMinValue->hide();
 }

 void GTAEquationPulseWidget::onSearchWidgetDutyOKClicked()
 {
     GTAICDParameter selectedItem = _pSearchWidgetDutyCycle->getSelectedItems();
     if (!selectedItem.getName().isEmpty())
     {
         ui->dutyCycleLE->setText(selectedItem.getName());
     }
     _pSearchWidgetDutyCycle->hide();
 }

 void GTAEquationPulseWidget::onSearchWidgetDirectionOKClicked()
 {
     GTAICDParameter selectedItem = _pSearchWidgetDirection->getSelectedItems();
     QString parametername = selectedItem.getName();
     if (!parametername.isEmpty())
     {
         if (!_newDirections->contains(parametername))
         {
             _newDirections->append(parametername);
         }
         ui->directionCB->addItem(parametername);
         setDirection(parametername);
         _pSearchWidgetDirection->hide();
     }
     
 }

#include "GTAWgtRegisterDef.h"
 REGISTER_EQUATION_WIDGET(EQ_SQUARE,GTAEquationPulseWidget)
