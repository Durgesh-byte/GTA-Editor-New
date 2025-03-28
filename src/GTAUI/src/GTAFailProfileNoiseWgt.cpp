#include "GTAFailProfileNoiseWgt.h"
#include "GTAFailProfileNoise.h"
#include "ui_GTAFailProfileNoiseWgt.h"

GTAFailProfileNoiseWgt::GTAFailProfileNoiseWgt(QWidget *parent) :
GTAFailureProfileUIInterface(parent),
ui(new Ui::GTAFailProfileNoiseWgt)
{
    ui->setupUi(this);
}

GTAFailProfileNoiseWgt::~GTAFailProfileNoiseWgt()
{
    delete ui;
}

QString GTAFailProfileNoiseWgt::getMean() const
{
    return ui->meanLE->text();
}
QString GTAFailProfileNoiseWgt::getSigma() const
{
    return ui->sigmaLE->text();
}
QString GTAFailProfileNoiseWgt::getSeed() const
{
    return ui->seedLE->text();
}

void GTAFailProfileNoiseWgt::setMean(const QString & iPeriod)
{
    ui->meanLE->setText(iPeriod);
}
void GTAFailProfileNoiseWgt::setSigma(const QString & iAmp)
{
    ui->sigmaLE->setText(iAmp);
}
void GTAFailProfileNoiseWgt::setSeed(const QString & iOffset)
{
    ui->seedLE->setText(iOffset);
}

bool GTAFailProfileNoiseWgt::isValid()const
{
    if( ui->meanLE->text().isEmpty()     ||
        ui->sigmaLE->text().isEmpty()  ||
        ui->seedLE->text().isEmpty()
       )
     return false;
    else
        return true;
}
void GTAFailProfileNoiseWgt::clear() const
{
    ui->meanLE->clear();
    ui->sigmaLE->clear();
    ui->seedLE->clear();
}
bool GTAFailProfileNoiseWgt::getFailureProfile(GTAFailureProfileBase*& pEqBase)const
{

    bool rc = false;
    GTAFailProfileNoise* pEqnNoise= NULL;
    if (NULL!=pEqBase)
    {
        pEqnNoise = dynamic_cast<GTAFailProfileNoise*> (pEqBase);

    }
    else
    {
        pEqnNoise = new GTAFailProfileNoise();

    }
    if (NULL!=pEqnNoise)
    {
        pEqnNoise->setSigma(getSigma());
        pEqnNoise->setSeed(getSeed());
        pEqnNoise->setMean(getMean());
       
        rc = true;
    }
    pEqBase = pEqnNoise;
    return rc;
}

bool GTAFailProfileNoiseWgt::setFailureProfile(GTAFailureProfileBase*& pEqBase)
{
    GTAFailProfileNoise* pEqnNoise= NULL;
    if (NULL!=pEqBase)
    {
        pEqnNoise = dynamic_cast<GTAFailProfileNoise*> (pEqBase);
        if (NULL!=pEqnNoise)
        {
            setSigma(pEqnNoise->getSigma());
            setSeed(pEqnNoise->getSeed());
            setMean(pEqnNoise->getMean());

        }

    }
    else
        clear();
    return true;
}
#include "GTAWgtRegisterDef.h"
REGISTER_FAILURE_WIDGET(EQ_NOISE,GTAFailProfileNoiseWgt)