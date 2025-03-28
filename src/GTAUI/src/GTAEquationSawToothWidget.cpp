#include "GTAEquationSawToothWidget.h"
#include "GTAEquationSawTooth.h"
#include "ui_GTAEquationSawToothWidget.h"

GTAEquationSawToothWidget::GTAEquationSawToothWidget(QWidget *parent) :
GTAEquationUIInterface(parent),
ui(new Ui::GTAEquationSawToothWidget)
{
    ui->setupUi(this);
}

GTAEquationSawToothWidget::~GTAEquationSawToothWidget()
{
    delete ui;
}

// GET Functions
QString GTAEquationSawToothWidget::getPeriod() const
{
    return ui->periodLE->text();
}
QString GTAEquationSawToothWidget::getRampMode() const
{
    QString sRampMode= ui->rampModeCB->currentText();
    return sRampMode;
}
QString GTAEquationSawToothWidget::getStartValue() const
{
    return ui->startValueLE->text();
}
QString GTAEquationSawToothWidget::getMinValue() const
{
    return ui->minValueLE->text();
}
QString GTAEquationSawToothWidget::getMaxValue() const
{
    return ui->maxValueLE->text();
}

// SET Functions
void GTAEquationSawToothWidget::setPeriod(const QString & iPeriod)
{
    ui->periodLE->setText(iPeriod);
}
void GTAEquationSawToothWidget::setRampMode(const QString & iRampMode)
{
   if(ui->rampModeCB->findText(iRampMode))
    {
        int itemCount = ui->rampModeCB->count();
        for (int i=0;i<itemCount;i++)
        {
            if (ui->rampModeCB->itemText(i)==iRampMode)
            {
                ui->rampModeCB->setCurrentIndex(i);
            }
        }
    }
}
void GTAEquationSawToothWidget::setStartValue(const QString & iStartValue)
{
    ui->startValueLE->setText(iStartValue);
}
void GTAEquationSawToothWidget::setMinValue(const QString & iMinValue)
{
    ui->minValueLE->setText(iMinValue);
}
void GTAEquationSawToothWidget::setMaxValue(const QString & iMaxValue)
{
    ui->maxValueLE->setText(iMaxValue);
}

void GTAEquationSawToothWidget::clear() const
{
    ui->periodLE->clear();
    ui->rampModeCB->setCurrentIndex(0);
    ui->startValueLE->clear();
    ui->minValueLE->clear();
    ui->maxValueLE->clear();
}

bool GTAEquationSawToothWidget::isValid()const
{
    if( ui->periodLE->text().isEmpty() || ui->startValueLE->text().isEmpty() ||
        ui->minValueLE->text().isEmpty() || ui->maxValueLE->text().isEmpty() )
    {
        return false;
    }
    else
        return true;
}

bool GTAEquationSawToothWidget::getEquation(GTAEquationBase*& pEqBase)const
{
    bool rc = false;
    GTAEquationSawTooth* pEqnSawTooth= NULL;
    if (NULL!=pEqBase)
    {
        pEqnSawTooth = dynamic_cast<GTAEquationSawTooth*> (pEqBase);
    }
    else
    {
        pEqnSawTooth = new GTAEquationSawTooth();
    }
    if (NULL != pEqnSawTooth)
    {
        pEqnSawTooth->setRampMode(getRampMode());
        pEqnSawTooth->setPeriod(getPeriod());
        pEqnSawTooth->setStartValue(getStartValue());
        pEqnSawTooth->setMinValue(getMinValue());
        pEqnSawTooth->setMaxValue(getMaxValue());
        rc = true;
    }
    pEqBase = pEqnSawTooth;
    return rc;
}

bool GTAEquationSawToothWidget::setEquation(GTAEquationBase*& pEqBase)
{
    GTAEquationSawTooth* pEqnSawTooth= NULL;
    if (NULL!=pEqBase)
    {
        pEqnSawTooth = dynamic_cast<GTAEquationSawTooth*> (pEqBase);
        if (NULL!=pEqnSawTooth)
        {
            setPeriod(pEqnSawTooth->getPeriod());
            setRampMode(pEqnSawTooth->getRampMode());
            setStartValue(pEqnSawTooth->getStartValue());
            setMinValue(pEqnSawTooth->getMinValue());
            setMaxValue(pEqnSawTooth->getMaxValue());
        }
    }
    else
        clear();
    return true;
}

void GTAEquationSawToothWidget::onSolidCBToggled(bool )
{

}

#include "GTAWgtRegisterDef.h"
REGISTER_EQUATION_WIDGET(EQ_SAWTOOTH,GTAEquationSawToothWidget)
