#include "GTAFailProfileRampSlopeWgt.h"
#include "GTAFailProfileRampSlope.h"
#include "ui_GTAFailProfileRampSlopeWgt.h"

GTAFailProfileRampSlopeWgt::GTAFailProfileRampSlopeWgt(QWidget *parent) :
GTAFailureProfileUIInterface(parent),
ui(new Ui::GTAFailProfileRampSlopeWgt)
{
    ui->setupUi(this);
}

GTAFailProfileRampSlopeWgt::~GTAFailProfileRampSlopeWgt()
{
    delete ui;
}

QString GTAFailProfileRampSlopeWgt::getOffset() const
{
    return ui->offsetLE->text();
}
QString GTAFailProfileRampSlopeWgt::getSlope() const
{
    return ui->slopeLE->text();
}


void GTAFailProfileRampSlopeWgt::setOffset(const QString & iPeriod)
{
    ui->offsetLE->setText(iPeriod);
}
void GTAFailProfileRampSlopeWgt::setSlope(const QString & iAmp)
{
    ui->slopeLE->setText(iAmp);
}


bool GTAFailProfileRampSlopeWgt::isValid()const
{
    if( ui->offsetLE->text().isEmpty()     ||
        ui->slopeLE->text().isEmpty()  
      
       )
     return false;
    else
        return true;
}
void GTAFailProfileRampSlopeWgt::clear() const
{
    ui->offsetLE->clear();
    ui->slopeLE->clear();
    
}
bool GTAFailProfileRampSlopeWgt::getFailureProfile(GTAFailureProfileBase*& pEqBase)const
{

    bool rc = false;
    GTAFailProfileRampSlope* pEqnRamp= NULL;
    if (NULL!=pEqBase)
    {
        pEqnRamp = dynamic_cast<GTAFailProfileRampSlope*> (pEqBase);

    }
    else
    {
        pEqnRamp = new GTAFailProfileRampSlope();

    }
    if (NULL!=pEqnRamp)
    {
        pEqnRamp->setOffset(getOffset());
        pEqnRamp->setSlope(getSlope());
       
        rc = true;
    }
    pEqBase = pEqnRamp;
    return rc;
}

bool GTAFailProfileRampSlopeWgt::setFailureProfile(GTAFailureProfileBase*& pEqBase)
{
    GTAFailProfileRampSlope* pEqnNoise= NULL;
    if (NULL!=pEqBase)
    {
        pEqnNoise = dynamic_cast<GTAFailProfileRampSlope*> (pEqBase);
        if (NULL!=pEqnNoise)
        {
            setSlope(pEqnNoise->getSlope());
            setOffset(pEqnNoise->getOffset());

        }

    }
    else
        clear();
    return true;
}
#include "GTAWgtRegisterDef.h"
REGISTER_FAILURE_WIDGET(FEQ_RAMPSLOPE,GTAFailProfileRampSlopeWgt)