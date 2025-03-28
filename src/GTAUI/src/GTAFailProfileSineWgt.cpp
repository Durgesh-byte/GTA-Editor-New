#include "GTAFailProfileSineWgt.h"
#include "GTAFailProfileSine.h"
#include "ui_GTAFailProfileSineWgt.h"

GTAFailProfileSineWgt::GTAFailProfileSineWgt(QWidget *parent) :
GTAFailureProfileUIInterface(parent),
ui(new Ui::GTAFailProfileSineWgt)
{
    ui->setupUi(this);
}

GTAFailProfileSineWgt::~GTAFailProfileSineWgt()
{
    delete ui;
}

QString GTAFailProfileSineWgt::getPeriod() const
{
    return ui->periodLE->text();
}
QString GTAFailProfileSineWgt::getAmplitude() const
{
    return ui->amplitudeLE->text();
}
QString GTAFailProfileSineWgt::getOffset() const
{
    return ui->offsetLE->text();
}
QString GTAFailProfileSineWgt::getPhase() const
{
    return ui->phaseLE->text();
}

void GTAFailProfileSineWgt::setPeriod(const QString & iPeriod)
{
    ui->periodLE->setText(iPeriod);
}
void GTAFailProfileSineWgt::setAmplitude(const QString & iAmp)
{
    ui->amplitudeLE->setText(iAmp);
}
void GTAFailProfileSineWgt::setOffset(const QString & iOffset)
{
    ui->offsetLE->setText(iOffset);
}
void GTAFailProfileSineWgt::setPhase(const QString & iPhase)
{
    ui->phaseLE->setText(iPhase);
}

bool GTAFailProfileSineWgt::isValid()const
{
    if( ui->periodLE->text().isEmpty()     ||
        ui->amplitudeLE->text().isEmpty()  ||
        ui->phaseLE->text().isEmpty()      ||
        ui->offsetLE->text().isEmpty()
       )
     return false;
    else
        return true;
}
void GTAFailProfileSineWgt::clear() const
{
    ui->periodLE->clear();
    ui->amplitudeLE->clear();
    ui->phaseLE->clear();
    ui->offsetLE->clear();
}
bool GTAFailProfileSineWgt::getFailureProfile(GTAFailureProfileBase*& pEqBase)const
{

    bool rc = false;
    GTAFailProfileSine* pEqnSinus= NULL;
    if (NULL!=pEqBase)
    {
        pEqnSinus = dynamic_cast<GTAFailProfileSine*> (pEqBase);

    }
    else
    {
        pEqnSinus = new GTAFailProfileSine();

    }
    if (NULL!=pEqnSinus)
    {
        pEqnSinus->setAmplitude(getAmplitude());
        pEqnSinus->setOffset(getOffset());
        pEqnSinus->setPeriod(getPeriod());
        pEqnSinus->setPhase(getPhase());
        rc = true;
    }
    pEqBase = pEqnSinus;
    return rc;
}

bool GTAFailProfileSineWgt::setFailureProfile(GTAFailureProfileBase*& pEqBase)
{
    GTAFailProfileSine* pEqnSinus= NULL;
    if (NULL!=pEqBase)
    {
        pEqnSinus = dynamic_cast<GTAFailProfileSine*> (pEqBase);
        if (NULL!=pEqnSinus)
        {
            setAmplitude(pEqnSinus->getAmplitude());
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
REGISTER_FAILURE_WIDGET(EQ_SINUS,GTAFailProfileSineWgt)