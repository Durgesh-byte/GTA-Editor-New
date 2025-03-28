#include "ui_GTAActionFCTLConfWidget.h"
#include "GTAActionFCTLConfWidget.h"
#include "GTAActionFCTLConf.h"

GTAActionFCTLConfWidget::GTAActionFCTLConfWidget(QWidget *parent) :
    GTAActionWidgetInterface(parent),
    ui(new Ui::GTAActionFCTLConfWidget)
{
    ui->setupUi(this);
}

GTAActionFCTLConfWidget::~GTAActionFCTLConfWidget()
{
    delete ui;
}

bool GTAActionFCTLConfWidget::getActionCommand(GTAActionBase *& pCmd)const
{
    bool rc =false;
    pCmd = NULL;
    if(isValid())
    {
        GTAActionFCTLConf *pConfCmd  = new GTAActionFCTLConf();
        pConfCmd->setAlpha(ui->AlphaLE->text());
        pConfCmd->setAltitude(ui->AltitudeLE->text());
        pConfCmd->setAP1(ui->AP1CB->currentText());
        pConfCmd->setAP2(ui->AP2CB->currentText());
        pConfCmd->setFCPC1(ui->FCPC1CB->currentText());
        pConfCmd->setFCPC2(ui->FCPC2CB->currentText());
        pConfCmd->setFCPC3(ui->FCPC3CB->currentText());
        pConfCmd->setFCSC1(ui->FCSC1CB->currentText());
        pConfCmd->setFCSC2(ui->FCSC2CB->currentText());
        pConfCmd->setFMGEC1(ui->FMGEC1CB->currentText());
        pConfCmd->setFMGEC2(ui->FMGEC2CB->currentText());
        pConfCmd->setBCM(ui->BCMCB->currentText());

        pConfCmd->setBHYD(ui->BHYDLE->text());
        pConfCmd->setYHYD(ui->YHYDLE->text());
        pConfCmd->setGHYD(ui->GHYDLE->text());
        pConfCmd->setGrndFlight(ui->GndFlghtCB->currentText());
        pConfCmd->setGrossWt(ui->GrossWeightLE->text());

        pConfCmd->setCG(ui->CGLE->text());
        pConfCmd->setVCAS(ui->VCASLE->text());
        pConfCmd->setLandingGear(ui->LdgGearCB->currentText());
        pConfCmd->setLAW(ui->LAWCB->currentText());
        pConfCmd->setSFConf(ui->SFConfCB->currentText());
        pConfCmd->setEngines(ui->EnginesCB->currentText());
        pConfCmd->setConfigName(ui->ConfigNameLE->text());
        pCmd = pConfCmd;
        rc = true;
    }
    return rc;
}
void GTAActionFCTLConfWidget::setActionCommand(const GTAActionBase *pCmd)
{
   
    if(pCmd != NULL)
    {
        GTAActionFCTLConf *pConfCmd = dynamic_cast<GTAActionFCTLConf *>((GTAActionBase*)pCmd);
        QString sAlpha=  pConfCmd->getAlpha();
        QString sAlittude = pConfCmd->getAltitude();
        QString sAP1 = pConfCmd->getAP1();
        QString sAp2 = pConfCmd->getAP2();
        QString sFCPC1 = pConfCmd->getFCPC1();
        QString sFCPC2 = pConfCmd->getFCPC2();
        QString sFCPC3 = pConfCmd->getFCPC3();
        QString FCSC1 = pConfCmd->getFCSC1();
        QString FCSC2 = pConfCmd->getFCSC2();
        QString FMGEC1 = pConfCmd->getFMGEC1();
        QString FMGEC2 = pConfCmd->getFMGEC2();
        QString BCM = pConfCmd->getBCM();

        QString BHYD = pConfCmd->getBHYD();
        QString YHYD = pConfCmd->getYHYD();
        QString GHYD = pConfCmd->getGHYD();
        QString GF = pConfCmd->getGrndFlight();
        QString GW = pConfCmd->getGrossWt();

        QString CG = pConfCmd->getCG();
        QString VCAS = pConfCmd->getVCAS();
        QString LDG = pConfCmd->getLandingGear();
        QString LAW = pConfCmd->getLAW();
        QString SFConf = pConfCmd->getSFConf();
        QString Eng = pConfCmd->getEngines();


        ui->AlphaLE->setText(sAlpha);
        ui->AltitudeLE->setText(sAlittude);
        int idx = ui->AP1CB->findText(sAP1);
        ui->AP1CB->setCurrentIndex(idx);
        idx = ui->AP2CB->findText(sAp2);
        ui->AP2CB->setCurrentIndex(idx);
        idx = ui->FCPC1CB->findText(sFCPC1);
        ui->FCPC1CB->setCurrentIndex(idx);
        idx = ui->FCPC2CB->findText(sFCPC2);
        ui->FCPC2CB->setCurrentIndex(idx);
        idx = ui->FCPC3CB->findText(sFCPC3);
        ui->FCPC3CB->setCurrentIndex(idx);
        idx = ui->FCSC1CB->findText(FCSC1);
        ui->FCSC1CB->setCurrentIndex(idx);
        idx = ui->FCSC2CB->findText(FCSC2);
        ui->FCSC2CB->setCurrentIndex(idx);
        idx = ui->FMGEC1CB->findText(FMGEC1);
        ui->FMGEC1CB->setCurrentIndex(idx);
        idx = ui->FMGEC2CB->findText(FMGEC2);
        ui->FMGEC2CB->setCurrentIndex(idx);
        idx = ui->BCMCB->findText(BCM);
        ui->BCMCB->setCurrentIndex(idx);
        idx = ui->GndFlghtCB->findText(GF);
        ui->GndFlghtCB->setCurrentIndex(idx);
        idx = ui->SFConfCB->findText(SFConf);
        ui->SFConfCB->setCurrentIndex(idx);
        idx = ui->EnginesCB->findText(Eng);
        ui->EnginesCB->setCurrentIndex(idx);
        idx = ui->LAWCB->findText(LAW);
        ui->LAWCB->setCurrentIndex(idx);
        idx = ui->LdgGearCB->findText(LDG);
        ui->LdgGearCB->setCurrentIndex(idx);

        ui->BHYDLE->setText(BHYD);
        ui->YHYDLE->setText(YHYD);
        ui->GHYDLE->setText(GHYD);
        ui->GrossWeightLE->setText(GW);
        ui->CGLE->setText(CG);
        ui->VCASLE->setText(VCAS);
        ui->AlphaLE->setText(sAlpha);
        ui->AltitudeLE->setText(sAlittude);
        ui->ConfigNameLE->setText(pConfCmd->getConfigName());
    }

   
}

void GTAActionFCTLConfWidget::clear()
{
    int idx = 0;
    ui->ConfigNameLE->clear();
    ui->BHYDLE->clear();
    ui->YHYDLE->clear();
    ui->GHYDLE->clear();
    ui->GrossWeightLE->clear();
    ui->CGLE->clear();
    ui->VCASLE->clear();
    ui->AlphaLE->clear();
    ui->AltitudeLE->clear();
    ui->AP1CB->setCurrentIndex(idx);
    ui->AP2CB->setCurrentIndex(idx);
    ui->FCPC1CB->setCurrentIndex(idx);
    ui->FCPC2CB->setCurrentIndex(idx);
    ui->FCPC3CB->setCurrentIndex(idx);
    ui->FCSC1CB->setCurrentIndex(idx);
    ui->FCSC2CB->setCurrentIndex(idx);
    ui->FMGEC1CB->setCurrentIndex(idx);
    ui->FMGEC2CB->setCurrentIndex(idx);
    ui->BCMCB->setCurrentIndex(idx);
    ui->GndFlghtCB->setCurrentIndex(idx);
    ui->SFConfCB->setCurrentIndex(idx);
    ui->EnginesCB->setCurrentIndex(idx);
    ui->LAWCB->setCurrentIndex(idx);
    ui->LdgGearCB->setCurrentIndex(idx);
}

bool GTAActionFCTLConfWidget::isValid()const
{
    bool rc =true;

    rc  = rc & !ui->BHYDLE->text().isEmpty();
    rc  = rc & !ui->YHYDLE->text().isEmpty();
    rc  = rc & !ui->GHYDLE->text().isEmpty();
    rc  = rc & !ui->GrossWeightLE->text().isEmpty();
    rc  = rc & ! ui->CGLE->text().isEmpty();
    rc  = rc & !ui->VCASLE->text().isEmpty();
    rc  = rc & !ui->AlphaLE->text().isEmpty();
    rc  = rc & !ui->AltitudeLE->text().isEmpty();

    return rc;
}
#include "GTAWgtRegisterDef.h"
REGISTER_ACTION_WIDGET(QString("%1 %2").arg(ACT_FCTL,COM_FCTL_CONF),GTAActionFCTLConfWidget,obj)