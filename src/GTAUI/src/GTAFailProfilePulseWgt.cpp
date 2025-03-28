#include "GTAFailProfilePulseWgt.h"
#include "GTAFailProfilePulse.h"
#include "ui_GTAFailProfilePulseWgt.h"

GTAFailProfilePulseWgt::GTAFailProfilePulseWgt(QWidget *parent) :
GTAFailureProfileUIInterface(parent),
ui(new Ui::GTAFailProfilePulseWgt)
{
    ui->setupUi(this);
}

GTAFailProfilePulseWgt::~GTAFailProfilePulseWgt()
{
    delete ui;
}

QString GTAFailProfilePulseWgt::getPeriod() const
{
    return ui->periodLE->text();
}
QString GTAFailProfilePulseWgt::getDutyCycle() const
{
    return ui->dutyCycleLE->text();
}
QString GTAFailProfilePulseWgt::getOffset() const
{
    return ui->offsetLE->text();
}
QString GTAFailProfilePulseWgt::getPhase() const
{
    return ui->phaseLE->text();
}

void GTAFailProfilePulseWgt::setPeriod(const QString & iPeriod)
{
    ui->periodLE->setText(iPeriod);
}
void GTAFailProfilePulseWgt::setDutyCycle(const QString & iAmp)
{
    ui->dutyCycleLE->setText(iAmp);
}
void GTAFailProfilePulseWgt::setOffset(const QString & iOffset)
{
    ui->offsetLE->setText(iOffset);
}
void GTAFailProfilePulseWgt::setPhase(const QString & iPhase)
{
    ui->phaseLE->setText(iPhase);
}

bool GTAFailProfilePulseWgt::isValid()const
{
    if( ui->periodLE->text().isEmpty()     ||
        ui->dutyCycleLE->text().isEmpty()  ||
        ui->phaseLE->text().isEmpty()      ||
        ui->offsetLE->text().isEmpty()
       )
     return false;
    else
        return true;
}
void GTAFailProfilePulseWgt::clear() const
{
    ui->periodLE->clear();
    ui->dutyCycleLE->clear();
    ui->phaseLE->clear();
    ui->offsetLE->clear();
}
bool GTAFailProfilePulseWgt::getFailureProfile(GTAFailureProfileBase*& pEqBase)const
{

    bool rc = false;
    GTAFailProfilePulse* pEqnSinus= NULL;
    if (NULL!=pEqBase)
    {
        pEqnSinus = dynamic_cast<GTAFailProfilePulse*> (pEqBase);

    }
    else
    {
        pEqnSinus = new GTAFailProfilePulse();

    }
    if (NULL!=pEqnSinus)
    {
        pEqnSinus->setDutyCycle(getDutyCycle());
        pEqnSinus->setOffset(getOffset());
        pEqnSinus->setPeriod(getPeriod());
        pEqnSinus->setPhase(getPhase());
        rc = true;
    }
    pEqBase = pEqnSinus;
    return rc;
}

bool GTAFailProfilePulseWgt::setFailureProfile(GTAFailureProfileBase*& pEqBase)
{
    GTAFailProfilePulse* pEqnSinus= NULL;
    if (NULL!=pEqBase)
    {
        pEqnSinus = dynamic_cast<GTAFailProfilePulse*> (pEqBase);
        if (NULL!=pEqnSinus)
        {
            setDutyCycle(pEqnSinus->getDutyCycle());
            setPhase(pEqnSinus->getPhase());
            setOffset(pEqnSinus->getOffset());
            setPeriod(pEqnSinus->getPeriod());

        }

    }
    else
        clear();
    return true;
}
#include "GTAWgtRegisterDef.h"
REGISTER_FAILURE_WIDGET(EQ_SQUARE,GTAFailProfilePulseWgt)