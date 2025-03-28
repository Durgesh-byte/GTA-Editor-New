#include "GTAFailProfileSawToothWgt.h"
#include "GTAFailProfileSawTooth.h"
#include "ui_GTAFailProfileSawToothWgt.h"

GTAFailProfileSawToothWgt::GTAFailProfileSawToothWgt(QWidget *parent) :
GTAFailureProfileUIInterface(parent),
ui(new Ui::GTAFailProfileSawToothWgt)
{
    ui->setupUi(this);
}

GTAFailProfileSawToothWgt::~GTAFailProfileSawToothWgt()
{
    delete ui;
}

QString GTAFailProfileSawToothWgt::getOffset() const
{
    return ui->offsetLE->text();
}
QString GTAFailProfileSawToothWgt::getPhase() const
{
    return ui->phaseLE->text();
}
QString GTAFailProfileSawToothWgt::getPeriod() const
{
    return ui->periodLE->text();
}


void GTAFailProfileSawToothWgt::setOffset(const QString & iPeriod)
{
    ui->offsetLE->setText(iPeriod);
}
void GTAFailProfileSawToothWgt::setPhase(const QString & iAmp)
{
    ui->phaseLE->setText(iAmp);
}
void GTAFailProfileSawToothWgt::setPeriod(const QString & iOffset)
{
    ui->periodLE->setText(iOffset);
}

bool GTAFailProfileSawToothWgt::isValid()const
{
    if( ui->periodLE->text().isEmpty()     ||
        ui->offsetLE->text().isEmpty()  ||
        ui->phaseLE->text().isEmpty()      
       )
     return false;
    else
        return true;
}
void GTAFailProfileSawToothWgt::clear() const
{
    ui->periodLE->clear();
    ui->phaseLE->clear();
    ui->offsetLE->clear();
}
bool GTAFailProfileSawToothWgt::getFailureProfile(GTAFailureProfileBase*& pEqBase)const
{

    bool rc = false;
    GTAFailProfileSawTooth* pEqnSinus= NULL;
    if (NULL!=pEqBase)
    {
        pEqnSinus = dynamic_cast<GTAFailProfileSawTooth*> (pEqBase);

    }
    else
    {
        pEqnSinus = new GTAFailProfileSawTooth();

    }
    if (NULL!=pEqnSinus)
    {
        pEqnSinus->setPhase(getPhase());
        pEqnSinus->setOffset(getPeriod());
        pEqnSinus->setPeriod(getOffset());

        rc = true;
    }
    pEqBase = pEqnSinus;
    return rc;
}

bool GTAFailProfileSawToothWgt::setFailureProfile(GTAFailureProfileBase*& pEqBase)
{
    GTAFailProfileSawTooth* pEqnSinus= NULL;
    if (NULL!=pEqBase)
    {
        pEqnSinus = dynamic_cast<GTAFailProfileSawTooth*> (pEqBase);
        if (NULL!=pEqnSinus)
        {
            setPhase(pEqnSinus->getPhase());            
            setPeriod(pEqnSinus->getPeriod());
            setOffset(pEqnSinus->getOffset());

        }

    }
    else
        clear();
    return true;
}
#include "GTAWgtRegisterDef.h"
REGISTER_FAILURE_WIDGET(EQ_SAWTOOTH,GTAFailProfileSawToothWgt)