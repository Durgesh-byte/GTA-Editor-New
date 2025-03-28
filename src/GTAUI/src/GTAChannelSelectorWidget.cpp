#include "GTAChannelSelectorWidget.h"
#include "ui_GTAChannelSelectorWidget.h"

#include "GTAUtil.h"

GTAChannelSelectorWidget::GTAChannelSelectorWidget(QDialog *parent) :
    QDialog(parent),
    ui(new Ui::GTAChannelSelectorWidget)
{
    ui->setupUi(this);

}

GTAChannelSelectorWidget::~GTAChannelSelectorWidget()
{
    delete ui;
}
//format will be only of these form CHA, CHB, EEC1_A, EEC1_B,EEC2_A or EEC2_B 
QString GTAChannelSelectorWidget::getChannelSelection(const QString& iFormat)
{
    QString channelSelected;
    
    QString sFormat = iFormat;
    if(ui->bothRB->isChecked())
        channelSelected = BOTH_CHANNEL;
    else if (ui->chARB->isChecked())
         channelSelected = sFormat.replace("B","A");
    else if (ui->chBRB->isChecked())
        channelSelected = sFormat.replace("A","B");
    else if (ui->inCtrlRB->isChecked())
        channelSelected = CHANNEL_IN_CONTROL; //channel in control
    else if (ui->notInCtrlRB->isChecked())
        channelSelected = CHANNEL_NOT_IN_CONTROL; //channel not in control
  
    return channelSelected;
    
}
void GTAChannelSelectorWidget::setParameter(const QString& iParam)
{
    ui->paramEdit->setText(iParam);

}