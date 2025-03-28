#include "GTAFailProfileExpWgt.h"
#include "GTAFailProfileExp.h"
#include "ui_GTAFailProfileExpWgt.h"

GTAFailProfileExpWgt::GTAFailProfileExpWgt(QWidget *parent) :
GTAFailureProfileUIInterface(parent),
ui(new Ui::GTAFailProfileExpWgt)
{
    ui->setupUi(this);
}

GTAFailProfileExpWgt::~GTAFailProfileExpWgt()
{
    delete ui;
}

QString GTAFailProfileExpWgt::getStart() const
{
    return ui->startLE->text();
}
QString GTAFailProfileExpWgt::getStop() const
{
    return ui->stopLE->text();
}
QString GTAFailProfileExpWgt::getTau() const
{
    return ui->tauLE->text();
}


void GTAFailProfileExpWgt::setStart(const QString & iPeriod)
{
    ui->startLE->setText(iPeriod);
}
void GTAFailProfileExpWgt::setStop(const QString & iAmp)
{
    ui->stopLE->setText(iAmp);
}
void GTAFailProfileExpWgt::setTau(const QString & iOffset)
{
    ui->tauLE->setText(iOffset);
}

bool GTAFailProfileExpWgt::isValid()const
{
    if( ui->startLE->text().isEmpty()     ||
        ui->stopLE->text().isEmpty()  ||
        ui->tauLE->text().isEmpty()
       )
     return false;
    else
        return true;
}
void GTAFailProfileExpWgt::clear() const
{
    ui->startLE->clear();
    ui->stopLE->clear();
    ui->tauLE->clear();
}
bool GTAFailProfileExpWgt::getFailureProfile(GTAFailureProfileBase*& pEqBase)const
{

    bool rc = false;
    GTAFailProfileExp* pEqn= NULL;
    if (NULL!=pEqBase)
    {
        pEqn = dynamic_cast<GTAFailProfileExp*> (pEqBase);

    }
    else
    {
        pEqn = new GTAFailProfileExp();

    }
    if (NULL!=pEqn)
    {
        pEqn->setStop(getStop());
        pEqn->setTau(getTau());
        pEqn->setStart(getStart());

        rc = true;
    }
    pEqBase = pEqn;
    return rc;
}

bool GTAFailProfileExpWgt::setFailureProfile(GTAFailureProfileBase*& pEqBase)
{
    GTAFailProfileExp* pEqn= NULL;
    if (NULL!=pEqBase)
    {
        pEqn = dynamic_cast<GTAFailProfileExp*> (pEqBase);
        if (NULL!=pEqn)
        {
            setStop(pEqn->getStop());            
            setTau(pEqn->getTau());
            setStart(pEqn->getStart());

        }

    }
    else
        clear();
    return true;
}
#include "GTAWgtRegisterDef.h"
REGISTER_FAILURE_WIDGET(EQ_EXP,GTAFailProfileExpWgt)