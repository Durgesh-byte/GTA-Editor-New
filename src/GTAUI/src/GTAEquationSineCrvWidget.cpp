#include "GTAEquationSineCrvWidget.h"
#include "ui_GTAEquationSineCrvWidget.h"
#include "GTAEquationSineCrv.h"


GTAEquationSineCrvWidget::GTAEquationSineCrvWidget(QWidget *parent) :
    GTAEquationUIInterface(parent),
    ui(new Ui::GTAEquationSineCrvWidget)
{
    ui->setupUi(this);
}

GTAEquationSineCrvWidget::~GTAEquationSineCrvWidget()
{
    delete ui;
}

// GET Functions
QString GTAEquationSineCrvWidget::getGain() const
{
    return ui->GainLE->text();
}
QString GTAEquationSineCrvWidget::getTrigOperator() const
{
    return ui->TrigometryOpeCB->currentText();
}
QString GTAEquationSineCrvWidget::getPulsation() const
{
    return ui->PulsationLE->text();
}
QString GTAEquationSineCrvWidget::getPhase() const
{
    return ui->PhaseLE->text();
}
QString GTAEquationSineCrvWidget::getOffset() const
{
    return ui->OffsetLE->text();
}

// SET Functions
void GTAEquationSineCrvWidget::setGain(const QString & iGain)
{
    ui->GainLE->setText(iGain);
}
void GTAEquationSineCrvWidget::setTrigOperator(const QString & iOp)
{
    if(ui->TrigometryOpeCB->findText(iOp))
    {
        int itemCount = ui->TrigometryOpeCB->count();
        for(int i = 0; i< itemCount; i++)
        {
            if( ui->TrigometryOpeCB->itemText(i) == iOp)
            {
                ui->TrigometryOpeCB->setCurrentIndex(i);
            }
        }
    }
}

void GTAEquationSineCrvWidget::setPulsation(const QString & iPulse)
{
    ui->PulsationLE->setText(iPulse);
}

void GTAEquationSineCrvWidget::setPhase(const QString & iPhase)
{
    ui->PhaseLE->setText(iPhase);
}

void GTAEquationSineCrvWidget::setOffset(const QString & iOffset)
{
    ui->OffsetLE->setText(iOffset);
}


bool GTAEquationSineCrvWidget::isValid()const
{
    if( ui->GainLE->text().isEmpty() || ui->OffsetLE->text().isEmpty() ||
        ui->PulsationLE->text().isEmpty() || ui->PhaseLE->text().isEmpty() )
    {
        return false;
    }
    else
        return true;
}
void GTAEquationSineCrvWidget::clear() const
{
    ui->GainLE->clear();
    ui->OffsetLE->clear();
    ui->PulsationLE->clear();
    ui->PhaseLE->clear();
    ui->TrigometryOpeCB->setCurrentIndex(0);

}
bool GTAEquationSineCrvWidget::getEquation(GTAEquationBase*& pEqBase)const
{
    bool rc = false;
    GTAEquationSineCrv *pSineCrv = NULL;
    if (NULL != pEqBase)
    {
        pSineCrv = dynamic_cast<GTAEquationSineCrv*> (pEqBase);
    }
    else
    {
        pSineCrv = new GTAEquationSineCrv();
    }
    if (NULL != pSineCrv)
    {
        pSineCrv->setGain(getGain());
        pSineCrv->setOffset(getOffset());
        pSineCrv->setPhase(getPhase());
        pSineCrv->setPulsation(getPulsation());
        pSineCrv->setTrignometryOperator(getTrigOperator());
        rc = true;
    }
    pEqBase = pSineCrv;
    return rc;
}

bool GTAEquationSineCrvWidget::setEquation(GTAEquationBase*& pEqBase)
{
    GTAEquationSineCrv* pEqnSinusCrv = NULL;
    if(pEqBase != NULL)
    {
        pEqnSinusCrv = dynamic_cast<GTAEquationSineCrv*> (pEqBase);

        if(pEqnSinusCrv != NULL)
        {
            setGain(pEqnSinusCrv->getGain());
            setOffset(pEqnSinusCrv->getOffset());
            setPhase(pEqnSinusCrv->getPhase());
            setPulsation(pEqnSinusCrv->getPulsation());
            setTrigOperator(pEqnSinusCrv->getTrignometryOperator());
        }
    }
    else
        clear();
    return true;
}

void GTAEquationSineCrvWidget::onSolidCBToggled(bool )
{

}

#include "GTAWgtRegisterDef.h"
REGISTER_EQUATION_WIDGET(EQ_SINECRV,GTAEquationSineCrvWidget)
