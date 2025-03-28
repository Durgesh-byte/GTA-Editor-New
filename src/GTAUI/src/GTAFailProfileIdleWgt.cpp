#include "GTAFailProfileIdleWgt.h"
#include "GTAFailProfileIdle.h"
#include "ui_GTAFailProfileIdleWgt.h"

GTAFailProfileIdleWgt::GTAFailProfileIdleWgt(QWidget *parent) :
GTAFailureProfileUIInterface(parent),
ui(new Ui::GTAFailProfileIdleWgt)
{
    ui->setupUi(this);
}

GTAFailProfileIdleWgt::~GTAFailProfileIdleWgt()
{
    delete ui;
}



bool GTAFailProfileIdleWgt::isValid()const
{
     return true;
}
void GTAFailProfileIdleWgt::clear() const
{
   
}
bool GTAFailProfileIdleWgt::getFailureProfile(GTAFailureProfileBase*& pEqBase)const
{

    
    GTAFailProfileIdle* pEqnIdle= new GTAFailProfileIdle;
    pEqBase = pEqnIdle;
    return true;
}

bool GTAFailProfileIdleWgt::setFailureProfile(GTAFailureProfileBase*& )
{

    return true;
}
#include "GTAWgtRegisterDef.h"
REGISTER_FAILURE_WIDGET(EQ_IDLE,GTAFailProfileIdleWgt)