#include "GTAFailProfileRampWgt.h"
#include "GTAFailProfileRamp.h"
#include "ui_GTAFailProfileRampWgt.h"

GTAFailProfileRampWgt::GTAFailProfileRampWgt(QWidget *parent) :
GTAFailureProfileUIInterface(parent),
ui(new Ui::GTAFailProfileRampWgt)
{
    ui->setupUi(this);
}

GTAFailProfileRampWgt::~GTAFailProfileRampWgt()
{
    delete ui;
}

QString GTAFailProfileRampWgt::getStart() const
{
    return ui->startLE->text();
}
QString GTAFailProfileRampWgt::getStop() const
{
    return ui->stopLE->text();
}


void GTAFailProfileRampWgt::setStart(const QString & iPeriod)
{
    ui->startLE->setText(iPeriod);
}
void GTAFailProfileRampWgt::setStop(const QString & iAmp)
{
    ui->stopLE->setText(iAmp);
}


bool GTAFailProfileRampWgt::isValid()const
{
    if( ui->startLE->text().isEmpty()     ||
        ui->stopLE->text().isEmpty()  
      
       )
     return false;
    else
        return true;
}
void GTAFailProfileRampWgt::clear() const
{
    ui->startLE->clear();
    ui->stopLE->clear();
    
}
bool GTAFailProfileRampWgt::getFailureProfile(GTAFailureProfileBase*& pEqBase)const
{

    bool rc = false;
    GTAFailProfileRamp* pEqnRamp= NULL;
    if (NULL!=pEqBase)
    {
        pEqnRamp = dynamic_cast<GTAFailProfileRamp*> (pEqBase);

    }
    else
    {
        pEqnRamp = new GTAFailProfileRamp();

    }
    if (NULL!=pEqnRamp)
    {
        pEqnRamp->setStop(getStop());
        pEqnRamp->setStart(getStart());
       
        rc = true;
    }
    pEqBase = pEqnRamp;
    return rc;
}

bool GTAFailProfileRampWgt::setFailureProfile(GTAFailureProfileBase*& pEqBase)
{
    GTAFailProfileRamp* pEqnNoise= NULL;
    if (NULL!=pEqBase)
    {
        pEqnNoise = dynamic_cast<GTAFailProfileRamp*> (pEqBase);
        if (NULL!=pEqnNoise)
        {
            setStop(pEqnNoise->getStop());
            setStart(pEqnNoise->getStart());

        }

    }
    else
        clear();
    return true;
}
#include "GTAWgtRegisterDef.h"
REGISTER_FAILURE_WIDGET(EQ_RAMP,GTAFailProfileRampWgt)