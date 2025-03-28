#include "GTAFailProfileConstantWgt.h"
#include "GTAFailProfileConstant.h"
#include "ui_GTAFailProfileConstantWgt.h"

GTAFailProfileConstantWgt::GTAFailProfileConstantWgt(QWidget *parent) :
GTAFailureProfileUIInterface(parent),
ui(new Ui::GTAFailProfileConstantWgt)
{
    ui->setupUi(this);
}

GTAFailProfileConstantWgt::~GTAFailProfileConstantWgt()
{
    delete ui;
}

QString GTAFailProfileConstantWgt::getConsant() const
{
    return ui->constantLE->text();
}


void GTAFailProfileConstantWgt::setConstant(const QString & iPeriod)
{
    ui->constantLE->setText(iPeriod);
}


bool GTAFailProfileConstantWgt::isValid()const
{
    if( ui->constantLE->text().isEmpty()        )
        return false;
    else
        return true;
}
void GTAFailProfileConstantWgt::clear() const
{
    ui->constantLE->clear();
   
}
bool GTAFailProfileConstantWgt::getFailureProfile(GTAFailureProfileBase*& pEqBase)const
{

    bool rc = false;
    GTAFailProfileConstant* pEqnConst= NULL;
    if (NULL!=pEqBase)
    {
        pEqnConst = dynamic_cast<GTAFailProfileConstant*> (pEqBase);

    }
    else
    {
        pEqnConst = new GTAFailProfileConstant();

    }
    if (NULL!=pEqnConst)
    {
       
        pEqnConst->setConst(getConsant());

        rc = true;
    }
    pEqBase = pEqnConst;
    return rc;
}

bool GTAFailProfileConstantWgt::setFailureProfile(GTAFailureProfileBase*& pEqBase)
{
    GTAFailProfileConstant* pEqnNoise= NULL;
    if (NULL!=pEqBase)
    {
        pEqnNoise = dynamic_cast<GTAFailProfileConstant*> (pEqBase);
        if (NULL!=pEqnNoise)
        {
            setConstant(pEqnNoise->getConst());

        }

    }
    else
        clear();
    return true;
}
#include "GTAWgtRegisterDef.h"
REGISTER_FAILURE_WIDGET(FEQ_CONST,GTAFailProfileConstantWgt)/**/