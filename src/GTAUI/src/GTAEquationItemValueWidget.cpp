#include "GTAEquationItemValueWidget.h"
#include "ui_GTAEquationItemValueWidget.h"
#include "GTAEquationGain.h"
#include "GTAEquationConst.h"

GTAEquationItemValueWidget::GTAEquationItemValueWidget(QWidget *parent) :GTAEquationUIInterface(parent),
   ui(new Ui::GTAEquationItemValueWidget)
{
    ui->setupUi(this);
}

GTAEquationItemValueWidget::~GTAEquationItemValueWidget()
{
    delete ui;
}
void GTAEquationItemValueWidget::setLabel(const QString & iLabel)
{
    ui->ItemLabel->setText(iLabel);
}
QString GTAEquationItemValueWidget::getValue() const
{
    QString sValue= ui->ValueLE->text();
    return sValue;
}
void GTAEquationItemValueWidget::setValue(const QString & iVal)
{
     ui->ValueLE->setText(iVal);
}
void GTAEquationItemValueWidget::clear() const
{
    ui->ValueLE->clear();
}
bool GTAEquationItemValueWidget::isValid()const
{
    if( ui->ValueLE->text().isEmpty() )
        return false;
    else
        return true;
}

bool GTAEquationItemValueWidget::getEquation(GTAEquationBase*& pEqBase)const
{
    GTAEquationConst * pEqnConst = dynamic_cast<GTAEquationConst*>(pEqBase);
    GTAEquationGain * pEqnGain = dynamic_cast<GTAEquationGain*>(pEqBase);

    if(pEqnConst != NULL)
    {
        QString val = this->getValue();
        pEqnConst->setConstant(val);
       
    }else if(pEqnGain != NULL)
    {
        QString val = this->getValue();
        pEqnGain->setGain(val);
        
    }
    else
        this->clear();
    return false;
}

bool GTAEquationItemValueWidget::setEquation(GTAEquationBase*& pEqBase)
{
    GTAEquationConst * pEqnConst = dynamic_cast<GTAEquationConst*>(pEqBase);
    GTAEquationGain * pEqnGain = dynamic_cast<GTAEquationGain*>(pEqBase);
 
    if(pEqnConst != NULL)
    {
        QString val = pEqnConst->getConstant();
        this->setValue(val);
    }else if(pEqnGain != NULL)
    {
        QString val = pEqnGain->getGain();
        this->setValue(val);
    }
    else
        this->clear();
    return false;
}

void GTAEquationItemValueWidget::onSolidCBToggled(bool )
{

}
