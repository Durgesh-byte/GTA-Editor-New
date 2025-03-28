#include "GTAEquationConstValueWidget.h"
#include "ui_GTAEquationConstValueWidget.h"
#include "GTAEquationConst.h"
#include "GTAAppController.h"
#include <QCompleter>
GTAEquationConstValueWidget::GTAEquationConstValueWidget(QWidget *parent) :GTAEquationUIInterface(parent),
   ui(new Ui::GTAEquationConstValueWidget)
{
    ui->setupUi(this);
    
    GTAAppController* pController = GTAAppController::getGTAAppController();
    if (NULL!=pController)
    {
       QStringList wordList = pController->getVariablesForAutoComplete();
       QCompleter *completer = new QCompleter(wordList, this);
       completer->setCaseSensitivity(Qt::CaseInsensitive);
       ui->constantLE->setCompleter(completer);
       ui->timeSelectCoB->setVisible(false);
       ui->timeLabel->setVisible(false);

       QStringList comboList;
       comboList<<ACT_CONST_EQN_TIME_TYPE_UTC/*<<ACT_CONST_EQN_TIME_TYPE_FORMATTED*/;
       ui->timeSelectCoB->insertItems(0,comboList);

       connect(ui->timeSelectCB,SIGNAL(toggled(bool)),this,SLOT(ontimeSelected(bool)));
       connect(ui->timeSelectCoB,SIGNAL(currentIndexChanged(int)),this,SLOT(setCurrentIndex(int)));
       ui->constantLE->setPlaceholderText("Enter a const value. Enable checkbox for assigning time!");
    }
}

GTAEquationConstValueWidget::~GTAEquationConstValueWidget()
{
    delete ui;
}

/**
  * This slot is updates the Line Edit and label with the relevant time format selected
  * @iIndex: The index of the item selected in the combo box
*/
void GTAEquationConstValueWidget::setCurrentIndex(int iIndex)
{
    QString text = ui->timeSelectCoB->currentText();
    ui->constantLE->setText(text);

    int index = ui->timeSelectCoB->findText(ACT_CONST_EQN_TIME_TYPE_UTC,Qt::MatchFixedString);
    if (iIndex == index)
    {
        ui->timeLabel->setText("Format: UTC, number of seconds that have elapsed since January 1, 1970 . eg. 1549265623044");
    }
    else
    {
        ui->timeLabel->setText("Format: Regular date time, dddd MMM DD YYYY HH:MM:SS GMT+timezone. eg. Mon Feb 04 2019 13:03:43 GMT+0530");
    }
}

/**
  * This slot shows/hides the time selection option when the checkbox is toggled
  * @iVal: boolean value of checkbox enabled/disabled
*/
void GTAEquationConstValueWidget::ontimeSelected(bool iVal)
{
    ui->timeSelectCoB->setVisible(iVal);
    ui->timeLabel->setVisible(iVal);
    if (iVal)
    {
        ui->constantLE->setText(ui->timeSelectCoB->currentText());
    }
    else
    {
        ui->constantLE->clear();
        ui->constantLE->setPlaceholderText("Enter a const value. Enable checkbox for assigning time");
    }
}

QString GTAEquationConstValueWidget::getValue() const
{
    QString sValue= ui->constantLE->text();
    return sValue;
}
void GTAEquationConstValueWidget::setValue(const QString & iVal)
{
    if ((iVal == ACT_CONST_EQN_TIME_TYPE_UTC) || (iVal == ACT_CONST_EQN_TIME_TYPE_FORMATTED))
    {
        ui->timeSelectCB->setChecked(true);
        ui->timeSelectCoB->setCurrentIndex(iVal == ACT_CONST_EQN_TIME_TYPE_UTC ? 0 : 1);
    }
    else
    {
        ui->constantLE->setText(iVal);
    }
}
void GTAEquationConstValueWidget::clear() const
{
    ui->constantLE->clear();
}
bool GTAEquationConstValueWidget::isValid()const
{
    if( ui->constantLE->text().isEmpty() )
        return false;
    return true;
}

bool GTAEquationConstValueWidget::getEquation(GTAEquationBase*& pEqBase)const
{
    bool rc = false;
    GTAEquationConst * pEqnConst = NULL;
    QString val = this->getValue();

    if (pEqBase != NULL)
    {
        pEqnConst = dynamic_cast<GTAEquationConst*>(pEqBase);
        if(pEqnConst != NULL)
        {
           
            pEqnConst->setConstant(val);
            rc=true;

        }
    }   
    else
    {
        pEqnConst = new GTAEquationConst();
        pEqnConst->setConstant(val);
        rc = true;
    }
    pEqBase=pEqnConst;
    
    return rc;
}

bool GTAEquationConstValueWidget::setEquation(GTAEquationBase*& pEqBase)
{
       
    GTAEquationConst * pEqnConst = dynamic_cast<GTAEquationConst*>(pEqBase);
    if(pEqnConst != NULL)
    {
        QString val = pEqnConst->getConstant();
        this->setValue(val);
        
    }
    else
        this->clear();
    return true;
}




void GTAEquationConstValueWidget::onSolidCBToggled(bool )
{

}

#include "GTAWgtRegisterDef.h"
REGISTER_EQUATION_WIDGET(EQ_CONST,GTAEquationConstValueWidget)

//#include "AINIGTAObjCreator.hpp"
//#include "GTAFactoryCreatorRegistry.hpp"
//#include "GTAUiFactory.h"
//#include "GTAObjCreator.hpp"
//GTAFactoryCreatorRegistry< GTAUiFactory, 
//GTAObjCreator<GTAEquationUIInterface,GTAEquationConstValueWidget>
//> obj(EQ_CONST);

//#include "GTAFactoryCreatorRegistry.hpp"
//#include "GTAUiFactory.h"
//#include "GTAObjCreator.hpp"
//GTAFactoryCreatorRegistry<GTAUiFactory,GTAObjCreator<GTAEquationUIInterface,GTAEquationConstValueWidget>>p(EQ_CONST);
