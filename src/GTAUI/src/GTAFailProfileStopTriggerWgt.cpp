#include "GTAFailProfileStopTriggerWgt.h"
#include "GTAFailProfileStopTrigger.h"
#include "ui_GTAFailProfileStopTriggerWgt.h"

GTAFailProfileStopTriggerWgt::GTAFailProfileStopTriggerWgt(QWidget *parent) :
GTAFailureProfileUIInterface(parent),
ui(new Ui::GTAFailProfileStopTriggerWgt)
{
    ui->setupUi(this);
}

GTAFailProfileStopTriggerWgt::~GTAFailProfileStopTriggerWgt()
{
    delete ui;
}

QString GTAFailProfileStopTriggerWgt::getTimeOut() const
{
    return ui->timeOutLE->text();
}


void GTAFailProfileStopTriggerWgt::setTimeOut(const QString & iPeriod)
{
    ui->timeOutLE->setText(iPeriod);
}


bool GTAFailProfileStopTriggerWgt::isValid()const
{
    if( ui->timeOutLE->text().isEmpty()    )
     return false;
    else
        return true;
}
void GTAFailProfileStopTriggerWgt::clear() const
{
    ui->timeOutLE->clear();
    
}
bool GTAFailProfileStopTriggerWgt::getFailureProfile(GTAFailureProfileBase*& pEqBase)const
{

    bool rc = false;
    GTAFailProfileStopTrigger* pEqnStopTrigger= NULL;
    if (NULL!=pEqBase)
    {
        pEqnStopTrigger = dynamic_cast<GTAFailProfileStopTrigger*> (pEqBase);

    }
    else
    {
        pEqnStopTrigger = new GTAFailProfileStopTrigger();

    }
    if (NULL!=pEqnStopTrigger)
    {
        pEqnStopTrigger->setTimeout(getTimeOut());
       
       
        rc = true;
    }
    pEqBase = pEqnStopTrigger;
    return rc;
}

bool GTAFailProfileStopTriggerWgt::setFailureProfile(GTAFailureProfileBase*& pEqBase)
{
    GTAFailProfileStopTrigger* pEqnStopTrigger= NULL;
    if (NULL!=pEqBase)
    {
        pEqnStopTrigger = dynamic_cast<GTAFailProfileStopTrigger*> (pEqBase);
        if (NULL!=pEqnStopTrigger)
        {
            setTimeOut(pEqnStopTrigger->getTimeout());
           

        }

    }
    else
        clear();
    return true;
}
#include "GTAWgtRegisterDef.h"
REGISTER_FAILURE_WIDGET(EQ_STOP_TRIGGER,GTAFailProfileStopTriggerWgt)