#include "GTAInitConfigWidget.h"
#include "ui_GTAInitConfigWidget.h"
#include "GTACommandBuilder.h"
#include <QStringList>
#include <QMessageBox>
#include "GTAInitConfiguration.h"
#include "GTAUtil.h"

GTAInitConfigWidget::GTAInitConfigWidget(QWidget *parent) :
    QWidget(parent),_pInitConfigCmd(NULL),
    ui(new Ui::GTAInitConfigWidget)
{

    ui->setupUi(this);

    QStringList confList;confList<<INIT_CONF1<<INIT_CONF2<<INIT_CONF3<<INIT_CONF4;
    ui->ConfiguruationCB->addItems(confList);
    ui->ConfiguruationCB->setCurrentIndex(0);
    onConfigurationChange(ui->ConfiguruationCB->currentText());
    connect(ui->ConfiguruationCB,SIGNAL(currentIndexChanged(QString)),this,SLOT(onConfigurationChange(QString)));
    connect(ui->OKPB,SIGNAL(clicked()),this,SLOT(onOKPBClick()));
    connect(ui->CancelPB,SIGNAL(clicked()),this,SLOT(hide()));

    _ParameterList.append(ui->AltoSpeedCB);
    _ParameterList.append(ui->AltGroundCB);
    _ParameterList.append(ui->SlatoFlapLE);
    _ParameterList.append(ui->AltTragetLE);
    _ParameterList.append(ui->SpeedTargetLE);
    _ParameterList.append(ui->ParkBrakeLE);
    _ParameterList.append(ui->AthrLE);
    _ParameterList.append(ui->AutoPilo1LE);
    _ParameterList.append(ui->AutoPilo2LE);
    _ParameterList.append(ui->DisaLE);
    _ParameterList.append(ui->ACPoweredCB);
    _ParameterList.append(ui->Bat1n2LE);
    _ParameterList.append(ui->ExtPowerLE);
    _ParameterList.append(ui->APULE);
    _ParameterList.append(ui->APUGenLE);
    _ParameterList.append(ui->APUBleedLE);
    _ParameterList.append(ui->EngineStateCB);
    _ParameterList.append(ui->RotarySelectorLE);
    _ParameterList.append(ui->MasterLevel1n2LE);
    _ParameterList.append(ui->ThrottleStateCB);
}

GTAInitConfigWidget::~GTAInitConfigWidget()
{
    delete ui;
}

GTAInitConfiguration * GTAInitConfigWidget::getConfiguration() 
{
    if(_pInitConfigCmd == NULL)
    {
        _pInitConfigCmd = new GTAInitConfiguration(ui->ConfiguruationCB->currentText());
    }
    else _pInitConfigCmd->setConfigName(ui->ConfiguruationCB->currentText());

    _pInitConfigCmd->setAltSpeed(ui->AltoSpeedCB->currentText());
    _pInitConfigCmd->setAcPowered(ui->ACPoweredCB->currentText());
    _pInitConfigCmd->setAltGround(ui->AltGroundCB->currentText());
    _pInitConfigCmd->setAltTarget(ui->AltTragetLE->text());
    _pInitConfigCmd->setAPU(ui->APULE->text());
    _pInitConfigCmd->setAPUBleed(ui->APUBleedLE->text());
    _pInitConfigCmd->setAPUGen(ui->APUGenLE->text());
    _pInitConfigCmd->setATHR(ui->AthrLE->text());
    _pInitConfigCmd->setAutoPilot1(ui->AutoPilo1LE->text());
    _pInitConfigCmd->setAutoPilot2(ui->AutoPilo2LE->text());
    _pInitConfigCmd->setBat(ui->Bat1n2LE->text());
    _pInitConfigCmd->setDISA(ui->DisaLE->text());
    _pInitConfigCmd->setEngineSate(ui->EngineStateCB->currentText());
    _pInitConfigCmd->setExternalPower(ui->ExtPowerLE->text());
    _pInitConfigCmd->setMasterLevel(ui->MasterLevel1n2LE->text());
    _pInitConfigCmd->setParkBrakes(ui->ParkBrakeLE->text());
    _pInitConfigCmd->setRotarySelector(ui->RotarySelectorLE->text());
    _pInitConfigCmd->setSlatFlap(ui->SlatoFlapLE->text());
    _pInitConfigCmd->setSpeedTarget(ui->SpeedTargetLE->text());
    _pInitConfigCmd->setThrottleState(ui->ThrottleStateCB->currentText());

    QString currentConf = ui->ConfiguruationCB->currentText();



    return _pInitConfigCmd;
	//return NULL;
}
void GTAInitConfigWidget::setConfiguration(const GTAInitConfiguration * ipConfigCmd)
{
    if(ipConfigCmd != NULL)
    {
        QString configName = ipConfigCmd->getConfigName();
        QString altSpeed = ipConfigCmd->getAltSpeed();
        QString acPowerd = ipConfigCmd->getAcPowered();
        QString altGround= ipConfigCmd->getAltGround();
        QString altTarget= ipConfigCmd->getAltTarget();
        QString apu = ipConfigCmd->getAPU();
        QString apuBleed = ipConfigCmd->getAPUBleed();
        QString apuGen = ipConfigCmd->getAPUGen();
        QString athr = ipConfigCmd->getATHR();
        QString autoPilot1 = ipConfigCmd->getAutoPilot1();
        QString autoPilot2 = ipConfigCmd->getAutoPilot2();
        QString bat = ipConfigCmd->getBat();
        QString disa = ipConfigCmd->getDISA();
        QString engineState = ipConfigCmd->getEngineSate();
        QString extPower = ipConfigCmd->getExternalPower();
        QString masterLevel= ipConfigCmd->getMasterLevel();
        QString parkBrakes = ipConfigCmd->getParkBrakes();
        QString rotarySel = ipConfigCmd->getRotarySelector();
        QString slatFlap = ipConfigCmd->getSlatFlap();
        QString speedTarget = ipConfigCmd->getSpeedTarget();
        QString throttleState = ipConfigCmd->getThrottleState();

        setComboValue(ui->ConfiguruationCB,configName);
        setComboValue(ui->AltoSpeedCB,altSpeed);
        setComboValue(ui->AltGroundCB,altGround);
        setComboValue(ui->ACPoweredCB,acPowerd);
        setComboValue(ui->EngineStateCB,engineState);
        setComboValue(ui->ThrottleStateCB,throttleState);

        ui->AltTragetLE->setText(altTarget);
        ui->APULE->setText(apu);
        ui->APUBleedLE->setText(apuBleed);
        ui->APUGenLE->setText(apuGen);
        ui->AthrLE->setText(athr);
        ui->AutoPilo1LE->setText(autoPilot1);
        ui->AutoPilo2LE->setText(autoPilot2);
        ui->Bat1n2LE->setText(bat);
        ui->DisaLE->setText(disa);
        ui->ExtPowerLE->setText(extPower);
        ui->MasterLevel1n2LE->setText(masterLevel);
        ui->ParkBrakeLE->setText(parkBrakes);
        ui->RotarySelectorLE->setText(rotarySel);
        ui->SlatoFlapLE->setText(slatFlap);
        ui->SpeedTargetLE->setText(speedTarget);

        _pInitConfigCmd = (GTAInitConfiguration*) ipConfigCmd;
    }
}
void GTAInitConfigWidget::setComboValue(QComboBox *&ipCombo, const QString & iValue)
{
    if(ipCombo != NULL)
    {
        int itemCount = ipCombo->count();
        for(int i = 0; i< itemCount; i++)
        {
            if(ipCombo->itemText(i) == iValue)
            {
                ipCombo->setCurrentIndex(i);
                break;
            }
        }
    }
}

void GTAInitConfigWidget::clearConfigParameters()
{
    foreach(QWidget *pWidget,_ParameterList)
    {
        QComboBox * pCombo = dynamic_cast<QComboBox*>(pWidget);
        if(pCombo != NULL)
        {
            pCombo->clear();
        }
        else
        {
            QLineEdit * pLineEdit = dynamic_cast<QLineEdit*>(pWidget);
            if(pLineEdit != NULL)
                pLineEdit->clear();
        }
    }
}
void GTAInitConfigWidget::enableConfigParameters(bool iState)
{
    foreach(QWidget *pWidget,_ParameterList)
    {
        pWidget->setEnabled(iState);
    }
}


void GTAInitConfigWidget::onConfigurationChange(const QString &iConfiguration)
{
    clearConfigParameters();
    enableConfigParameters(false);
    QStringList altSpeedList; altSpeedList.append("Gnd/0 kts");
    QStringList altGroundList; altGroundList.append("Tls/500ft");
    QString slateFlap ="Conf2";
    QString altTarget = "NA";
    QString speedTarget = "NA";
    QString parkBrake= "ON";
    QString athr = "OFF";
    QString autoPilo1 = "OFF";
    QString autoPilo2 = "OFF";
    QString disa = "0";
    QStringList acPoweredList;acPoweredList.append("Not Powered");
    QString bat = "OFF";
    QString extPower = "OFF";
    QString apu = "OFF";
    QString apuGen = "OFF";
    QString apuBleed = "OFF";
    QStringList engineStateList; engineStateList<<"1+2 not running";
    QString rotarySel = "NORM";
    QString masterLever = "OFF";
    QStringList throttleList; throttleList<<"idle";
    if(iConfiguration == INIT_CONF1)
    {
        altSpeedList.clear(); altSpeedList.append("Gnd/0 kts");
        altGroundList.clear(); altGroundList.append("Tls/500ft");
        slateFlap ="Conf2";
        altTarget = "NA";
        speedTarget = "NA";
        parkBrake= "ON";
        athr = "OFF";
        autoPilo1 = "OFF";
        autoPilo2 = "OFF";
        disa = "0";
        acPoweredList.clear();acPoweredList.append("Not Powered");
        bat = "OFF";
        extPower = "OFF";
        apu = "OFF";
        apuGen = "OFF";
        apuBleed = "OFF";
        engineStateList; engineStateList<<"1+2 not running";
        rotarySel = "NORM";
        masterLever = "OFF";
        throttleList.clear(); throttleList<<"idle";

        ui->DisaLE->setEnabled(true);
    }
    else if(iConfiguration == INIT_CONF2)
    {
        altSpeedList.clear(); altSpeedList.append("Gnd/0 kts");
        altGroundList.clear(); altGroundList.append("Tls/500ft");
        slateFlap ="Conf2";
        altTarget = "NA";
        speedTarget = "NA";
        parkBrake= "ON";
        athr = "OFF";
        autoPilo1 = "OFF";
        autoPilo2 = "OFF";
        disa = "0";
        engineStateList.clear(); engineStateList<<"1+2 not running";
        rotarySel = "NORM";
        masterLever = "OFF";
        throttleList.clear(); throttleList<<"idle";
        acPoweredList.clear();acPoweredList<<"Powered";
        bat = "ON";
        extPower = "ON";
        apu = "ON";
        apuGen = "ON";
        apuBleed = "ON";
        ui->DisaLE->setEnabled(true);
    }
    else if(iConfiguration == INIT_CONF3)
    {
        altSpeedList.clear(); altSpeedList.append("Gnd/0 kts");
        altGroundList.clear(); altGroundList.append("Tls/500ft");
        slateFlap ="Conf2";
        altTarget = "NA";
        speedTarget = "NA";
        parkBrake= "ON";
        athr = "OFF";
        autoPilo1 = "OFF";
        autoPilo2 = "OFF";
        disa = "0";
        engineStateList.clear(); engineStateList<<"1+2 running"<<"1+2 not running"<<"1 not running, 2 running";
        rotarySel = "NORM";
        masterLever = "ON";
        throttleList.clear(); throttleList<<"idle";
        acPoweredList.clear();acPoweredList<<"Powered";
        bat = "ON";
        extPower = "OFF";
        apu = "ON";
        apuGen = "ON";
        apuBleed = "OFF";
        ui->DisaLE->setEnabled(true);
        ui->EngineStateCB->setEnabled(true);
    }
    else if(iConfiguration == INIT_CONF4)
    {
        altSpeedList.clear(); altSpeedList<<"approach"<<"cruise low"<<"cruise high";
        altGroundList.clear(); altGroundList.append("Tls/500ft");
        slateFlap ="Conf Clean";
        altTarget = "";
        speedTarget = "";
        parkBrake= "OFF";
        athr = "ON";
        autoPilo1 = "ON";
        autoPilo2 = "ON";
        disa = "0";
        engineStateList.clear(); engineStateList<<"1+2 running";
        rotarySel = "NORM";
        masterLever = "ON";
        throttleList.clear(); throttleList<<"TOGA"<<"MCT"<<"MCL"<<"idle";
        acPoweredList.clear();acPoweredList<<"Powered";
        bat = "ON";
        extPower = "OFF";
        apu = "OFF";
        apuGen = "OFF";
        apuBleed = "OFF";
        ui->AltoSpeedCB->setEnabled(true);
        ui->DisaLE->setEnabled(true);
        ui->AltTragetLE->setEnabled(true);
        ui->SpeedTargetLE->setEnabled(true);
        ui->ThrottleStateCB->setEnabled(true);
    }
    ui->AltoSpeedCB->addItems(altSpeedList);
    ui->AltGroundCB->addItems(altGroundList);
    ui->SlatoFlapLE->setText(slateFlap);
    ui->AltTragetLE->setText(altTarget);
    ui->SpeedTargetLE->setText(speedTarget);
    ui->ParkBrakeLE->setText(parkBrake);
    ui->AthrLE->setText(athr);
    ui->AutoPilo1LE->setText(autoPilo1);
    ui->AutoPilo2LE->setText(autoPilo2);
    ui->DisaLE->setText(disa);
    ui->ACPoweredCB->addItems(acPoweredList);
    ui->Bat1n2LE->setText(bat);
    ui->ExtPowerLE->setText(extPower);
    ui->APULE->setText(apu);
    ui->APUGenLE->setText(apuGen);
    ui->APUBleedLE->setText(apuBleed);
    ui->EngineStateCB->addItems(engineStateList);
    ui->RotarySelectorLE->setText(rotarySel);
    ui->MasterLevel1n2LE->setText(masterLever);
    ui->ThrottleStateCB->addItems(throttleList);
}
QString GTAInitConfigWidget::getInitConfiguration() const
{
    QString currentConf = ui->ConfiguruationCB->currentText();
    QString configCommand;
    if(currentConf == INIT_CONF1 || currentConf == INIT_CONF2)
    {
        configCommand = GTACommandBuilder::getInitConfiguration(currentConf,ui->AltGroundCB->currentText(),ui->DisaLE->text());
    }
    else if(currentConf == INIT_CONF3)
    {
        configCommand = GTACommandBuilder::getInitConfiguration(currentConf,ui->AltGroundCB->currentText(),
                                                                   ui->DisaLE->text(),ui->EngineStateCB->currentText());
    }
    else if(currentConf == INIT_CONF4)
    {
        configCommand = GTACommandBuilder::getInitConfiguration(currentConf,ui->AltGroundCB->currentText(),
                                                                   ui->DisaLE->text(),ui->AltoSpeedCB->currentText(),ui->ThrottleStateCB->currentText());
    }
    return configCommand;
}
QString GTAInitConfigWidget::getEndConfiguration() const
{
    return QString("Put the aircraft back in 'dark cockpit' position");
}
void GTAInitConfigWidget::onOKPBClick()
{
    bool compileError = false;
    foreach(QWidget *pWidget,_ParameterList)
    {
        QComboBox * pCombo = dynamic_cast<QComboBox*>(pWidget);
        if(pCombo != NULL)
        {
            if( pCombo->currentText().isEmpty())
            {
                compileError = true;
            }
        }
        else
        {
            QLineEdit * pLineEdit = dynamic_cast<QLineEdit*>(pWidget);
            if(pLineEdit != NULL)
            {
                if(pLineEdit->text().isEmpty())
                    compileError = true;
            }
        }
        if(compileError)
            break;
    }
    if(compileError)
    {
        QString errMsg = "Provide values for all the fields";
        if (this->isVisible())
            QMessageBox::critical(0,"Compilation Error",errMsg,QMessageBox::Ok);
    }
    else
    {   hide();
        emit okPressed();
    }
}
void GTAInitConfigWidget::keyPressEvent ( QKeyEvent * event )
{
    if (event->key()==Qt::Key_Escape )
    {
        this->hide();
    }
    QWidget::keyPressEvent(event);
}