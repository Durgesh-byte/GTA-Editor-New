#include "GTACheckECAMDisplayWidget.h"
#include "ui_GTACheckECAMDisplayWidget.h"
#include "GTACheckEcamDisplay.h"

#define DISP_PAGE_CHANGE "page change to"
#define DISP_VISU_INFO "visual information"
#define ECAM_DISP_PG_CHNG_VAL "Engine;APU;BLEED;Air cond;Cab pres;Door/oxy;Elec AC; Elec DC;Fuel;Wheel;Hyd;F/CTL;C/B/Cruise;Status"


GTACheckECAMDisplayWidget::GTACheckECAMDisplayWidget(QWidget *parent) :
    GTACheckWidgetInterface(parent),
    ui(new Ui::GTACheckECAMDisplayWidget)
{
    ui->setupUi(this);
    QStringList displayOptionList;displayOptionList<<DISP_PAGE_CHANGE<<DISP_VISU_INFO;
    QStringList pageChangeValList; pageChangeValList = QString(ECAM_DISP_PG_CHNG_VAL).split(";");
    ui->CheckSubOptCB->addItems(displayOptionList);
    ui->CheckValueCB->addItems(pageChangeValList);
    ui->CheckSubOptCB->setCurrentIndex(0);
    ui->CheckValueTE->hide();
    connect(ui->CheckSubOptCB,SIGNAL(currentIndexChanged(QString)),this,SLOT(onDisplayTypeChange(QString)));
}

GTACheckECAMDisplayWidget::~GTACheckECAMDisplayWidget()
{
    delete ui;
}
QString GTACheckECAMDisplayWidget::getDiplayType() const
{
    return ui->CheckSubOptCB->currentText();
}
QString GTACheckECAMDisplayWidget::getDisplayValue() const
{
    if(ui->CheckSubOptCB->currentText() == DISP_PAGE_CHANGE)
        return ui->CheckValueCB->currentText();
    else
        return ui->CheckValueTE->toPlainText();
}

void GTACheckECAMDisplayWidget::onDisplayTypeChange(const QString &)
{
    if(ui->CheckSubOptCB->currentText() == DISP_PAGE_CHANGE)
    {
        ui->CheckValueCB->show();
        ui->CheckValueTE->hide();
    }
    else
    {
        ui->CheckValueCB->hide();
        ui->CheckValueTE->show();
    }

}
void GTACheckECAMDisplayWidget::setCheckCommand(const GTACheckEcamDisplay * ipChkCmd)
{
    if(ipChkCmd != NULL)
    {
        QString displayType = ipChkCmd->getDisplayType();
        QString value = ipChkCmd->getValue();
        if(displayType == DISP_PAGE_CHANGE)
        {
            int itemCount = ui->CheckValueCB->count();
            for(int i = 0 ; i < itemCount ; i++)
            {
                if(ui->CheckValueCB->itemText(i) == value)
                {
                    ui->CheckValueCB->setCurrentIndex(i);
                    break;
                }
            }
        }
        else
            ui->CheckValueTE->setText(value);
    }
}

GTACheckEcamDisplay * GTACheckECAMDisplayWidget::getCheckCommand()
{
    QString displayType = ui->CheckSubOptCB->currentText();
    QString value = getDisplayValue();

    GTACheckEcamDisplay * pCheckCmd = new GTACheckEcamDisplay(displayType);
    pCheckCmd->setValue(value);
    pCheckCmd->setDisplayType(displayType);
    return pCheckCmd;
}
void GTACheckECAMDisplayWidget::clear()
{
    ui->CheckValueTE->clear();
    if(ui->CheckSubOptCB->count()>0)
        ui->CheckSubOptCB->setCurrentIndex(0);
    if(ui->CheckValueCB->count()>0)
        ui->CheckValueCB->setCurrentIndex(0);
}

bool GTACheckECAMDisplayWidget::isValid()
{
    bool rc = false;
    if(! ui->CheckValueTE->toPlainText().isEmpty() && ! ui->CheckSubOptCB->currentText().isEmpty() && ! ui->CheckValueCB->currentText().isEmpty())
        rc = true;
    return rc;
}

bool GTACheckECAMDisplayWidget::getCommand(GTACheckBase *& pCheckCmd)
{
    bool rc = isValid();
    if(rc)
    {
        pCheckCmd = getCheckCommand();
    }
    return rc;
}

bool GTACheckECAMDisplayWidget::setCommand(const GTACheckBase * pCheckCmd)
{
    bool rc = false;
    if(pCheckCmd != NULL && pCheckCmd->getCheckType() == GTACheckBase::ECAM_DISPLAY)
    {
        GTACheckEcamDisplay * pCmd = dynamic_cast<GTACheckEcamDisplay*>((GTACheckBase*)pCheckCmd);
        if(pCmd != NULL)
        {
            setCheckCommand(pCmd);
            rc = true;
        }
    }
    return rc;
}

bool GTACheckECAMDisplayWidget::hasTimeOut()const       
{
    return false;
}
bool GTACheckECAMDisplayWidget::hasDumpList()const      
{
    return true;
}
bool GTACheckECAMDisplayWidget::hasPrecision()const
{
    return false;
}
bool GTACheckECAMDisplayWidget::hasSearchItem()const
{
    return false;
}
bool GTACheckECAMDisplayWidget::hasConfigTime() const
{
    return false;
}
bool GTACheckECAMDisplayWidget::hasActionOnFail()const
{
    return true;
}