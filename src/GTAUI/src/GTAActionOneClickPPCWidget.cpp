#include "GTAActionOneClickPPCWidget.h"
#include "ui_GTAActionOneClickPPCWidget.h"
#include <QFileDialog>
#include <QDir>

GTAActionOneClickPPCWidget::GTAActionOneClickPPCWidget(QWidget *parent) :
        GTAActionWidgetInterface(parent),
        ui(new Ui::GTAActionOneClickPPCWidget)
{
    ui->setupUi(this);
    connect(ui->BrowsePB,SIGNAL(clicked()),this,SLOT(onBrowsePBClicked()));
    connect(ui->StartPinProgRB,SIGNAL(toggled(bool)),this,SLOT(onStartRBClicked(bool)));
    connect(ui->StopPinProgRB,SIGNAL(toggled(bool)),this,SLOT(onStopRBClicked(bool)));
    ui->StartPinProgRB->setChecked(true);
    ui->PPCFileLE->setReadOnly(true);
}

GTAActionOneClickPPCWidget::~GTAActionOneClickPPCWidget()
{
    delete ui;
}

bool GTAActionOneClickPPCWidget::getActionCommand(GTAActionBase*& opCmd)const
{
    if(!isValid())
    {
        opCmd= NULL;
        return false;
    }
    GTAActionOneClickPPC *pActionCmd  = NULL;
    bool isStart = ui->StartPinProgRB->isChecked();
    QString ppcPath = ui->PPCFileLE->text();
    pActionCmd = new GTAActionOneClickPPC(QString(),QString());
    if(!ppcPath.isEmpty())
    {
        pActionCmd->setPPCPath(ppcPath);
    }
    pActionCmd->setIsStart(isStart);

    opCmd= pActionCmd;
    return true;
}

void GTAActionOneClickPPCWidget::setActionCommand(const  GTAActionBase* ipActionCmd)
{
    GTAActionOneClickPPC* pOneClickCmd = dynamic_cast<GTAActionOneClickPPC*>((GTAActionBase*)ipActionCmd);
    bool isStart = false;

    if(pOneClickCmd != NULL)
    {
        isStart = pOneClickCmd->getIsStart();
        ui->StartPinProgRB->setChecked(isStart);
        ui->StopPinProgRB->setChecked(!isStart);
        ui->PinProgFrame->setVisible(isStart);
        if(isStart)
            ui->PPCFileLE->setText(pOneClickCmd->getPPCPath());
    }
}

void GTAActionOneClickPPCWidget::onBrowsePBClicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,"Pin Prog Config File",QDir::currentPath(),"*.ppc");

    if (!filePath.isEmpty())
        ui->PPCFileLE->setText(QDir::cleanPath(filePath));
}

void GTAActionOneClickPPCWidget::clear()
{
    ui->PPCFileLE->clear();
    ui->StartPinProgRB->setChecked(true);
}

void GTAActionOneClickPPCWidget::onStartRBClicked(bool iVal)
{
    ui->StopPinProgRB->setChecked(!iVal);
    ui->PinProgFrame->setVisible(true);
}

void GTAActionOneClickPPCWidget::onStopRBClicked(bool iVal)
{
    ui->StartPinProgRB->setChecked(!iVal);
    ui->PinProgFrame->setVisible(false);
    ui->PPCFileLE->clear();
}

bool GTAActionOneClickPPCWidget::isValid()const
{
    bool rc = true;
    if(ui->StartPinProgRB->isChecked())
    {
        if(ui->PPCFileLE->text().isEmpty())
        {
            rc = false;
        }
    }
    return rc;
}
#include "GTAWgtRegisterDef.h"
REGISTER_ACTION_WIDGET(QString("%1 %2").arg(ACT_ONECLICK,COM_ONECLICK_PINPROG),GTAActionOneClickPPCWidget,obj)
