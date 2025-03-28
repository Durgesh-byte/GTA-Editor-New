#include "GTAEquationGainValueWidget.h"
#include "ui_GTAEquationGainValueWidget.h"
#include "GTAEquationGain.h"


GTAEquationGainValueWidget::GTAEquationGainValueWidget(QWidget *parent) :GTAEquationUIInterface(parent),
   ui(new Ui::GTAEquationGainValueWidget)
{
    ui->setupUi(this);
}

GTAEquationGainValueWidget::~GTAEquationGainValueWidget()
{
    delete ui;
}

// GET Functions
QString GTAEquationGainValueWidget::getGain() const
{
    return ui->gainLE->text();
}

// SET Functions
void GTAEquationGainValueWidget::setGain(const QString & iGain)
{
     ui->gainLE->setText(iGain);
}

void GTAEquationGainValueWidget::clear() const
{
    ui->gainLE->clear();
}
bool GTAEquationGainValueWidget::isValid()const
{
    if( ui->gainLE->text().isEmpty() )
        return false;
    return true;
}

bool GTAEquationGainValueWidget::getEquation(GTAEquationBase*& pEqBase) const
{
    bool rc = false;
    GTAEquationGain * pEqnGain = NULL;
    if (pEqBase != NULL)
    {
        pEqnGain = dynamic_cast<GTAEquationGain*>(pEqBase);
    }
    else
    {
        pEqnGain = new GTAEquationGain();
    }
    if(pEqnGain != NULL)
    {
        pEqnGain->setGain(getGain());
        rc = true;
    }   
    pEqBase = pEqnGain;
    return rc;
}

bool GTAEquationGainValueWidget::setEquation(GTAEquationBase*& pEqBase)
{
    GTAEquationGain * pEqnGain = NULL;
    if (NULL != pEqBase)
    {
        pEqnGain = dynamic_cast<GTAEquationGain*> (pEqBase);
        if (NULL != pEqnGain)
        {
            setGain(pEqnGain->getGain());
        }
    }
    else
    {
        clear();
    }
    return true;
}

void GTAEquationGainValueWidget::onSolidCBToggled(bool) {}

#include "GTAWgtRegisterDef.h"
REGISTER_EQUATION_WIDGET(EQ_GAIN,GTAEquationGainValueWidget)
