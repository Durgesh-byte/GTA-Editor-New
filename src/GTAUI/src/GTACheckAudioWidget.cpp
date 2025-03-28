#include "GTACheckAudioWidget.h"
#include "ui_GTACheckAudioWidget.h"
#include "GTACheckAudioAlarm.h"
#include "GTAAppController.h"



GTACheckAudioWidget::GTACheckAudioWidget(QWidget *parent) : GTACheckWidgetInterface(parent),
    //    QWidget(parent),
    ui(new Ui::GTACheckAudioWidget)
{
    ui->setupUi(this);
    connect(ui->searchPB,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));

    QDoubleValidator* pDoubleValidator = new QDoubleValidator(ui->HeardAfterLE);
    ui->HeardAfterLE->setValidator(pDoubleValidator);
}

GTACheckAudioWidget::~GTACheckAudioWidget()
{
    delete ui;
}

QString GTACheckAudioWidget::getValue() const
{
    return ui->SoundLE->text();
}
QString GTACheckAudioWidget::getTime() const
{
    return ui->HeardAfterLE->text();
}

void GTACheckAudioWidget::setCheckCommand(const GTACheckAudioAlarm * ipCheckCmd)
{
    if(ipCheckCmd != NULL)
    {
        QString value= ipCheckCmd->getAlarmName();
        QString time = ipCheckCmd->getTime();
        ui->SoundLE->setText(value);
        ui->HeardAfterLE->setText(time);
    }
}

GTACheckAudioAlarm * GTACheckAudioWidget::getCheckCommand()
{
    GTACheckAudioAlarm * pCheckCmd = new GTACheckAudioAlarm();
    pCheckCmd->setValue(getValue());
    pCheckCmd->setTime(getTime());
    return pCheckCmd;
}
void GTACheckAudioWidget::clear()
{
    ui->HeardAfterLE->clear();
    ui->SoundLE->clear();
}

bool GTACheckAudioWidget::isValid()
{
    bool rc = false;
    if(! ui->HeardAfterLE->text().isEmpty())
        rc = true;
    return rc;
}

bool GTACheckAudioWidget::getCommand(GTACheckBase *& pCheckCmd)
{
    bool rc = isValid();
    if(rc)
    {
        pCheckCmd = getCheckCommand();
    }
    return rc;
}

bool GTACheckAudioWidget::setCommand(const GTACheckBase * pCheckCmd)
{
    bool rc = false;
    if(pCheckCmd != NULL && pCheckCmd->getCheckType() == GTACheckBase::AUDIO_ALARM)
    {
        GTACheckAudioAlarm * pCmd = dynamic_cast<GTACheckAudioAlarm*>((GTACheckBase*)pCheckCmd);
        if(pCmd != NULL)
        {
            setCheckCommand(pCmd);
            rc = true;
        }
    }
    return rc;
}


void GTACheckAudioWidget::onSearchPBClicked()
{
    emit searchMessage(ui->SoundLE);
}

bool GTACheckAudioWidget::hasSearchItem()const 
{
    return true;
}
bool GTACheckAudioWidget::hasActionOnFail()const 
{
    return true;
}
bool GTACheckAudioWidget::hasPrecision()const
{
return false;
}
bool GTACheckAudioWidget::hasTimeOut()const 
{
return false;
}
bool GTACheckAudioWidget::hasDumpList()const
{   
    return true;

}
bool GTACheckAudioWidget::hasConfigTime() const 
{
return false;
}