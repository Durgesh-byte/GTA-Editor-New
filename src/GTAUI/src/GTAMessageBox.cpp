#include "GTAMessageBox.h"
#include "ui_GTAMessageBox.h"

GTAMessageBox::GTAMessageBox(MessageType iType, const QString &iTitle, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GTAMessageBox)
{
    ui->setupUi(this);

    _Result = None;

    QString wndTitle = iTitle;
    setImage(iType);
    if(wndTitle.isEmpty())
    {
        switch(iType)
        {
        case Information: wndTitle = "Information";break;
        case Warning: wndTitle = "Warning";break;
        case Critical: wndTitle = "Critical";break;
        }
    }
    setWindowTitle(wndTitle);
    connect(ui->OKPB,SIGNAL(clicked()),this,SLOT(onPushButtonClick()));
    connect(ui->CancelPB,SIGNAL(clicked()),this,SLOT(onPushButtonClick()));
    setVisibility(false,CANCELPB);
    setButtonLabel("Close",OKPB);
}

GTAMessageBox::~GTAMessageBox()
{
    delete ui;
}
GTAMessageBox::DialogResult GTAMessageBox::getResult() const
{
    return _Result;
}
void GTAMessageBox::setMessage(const QString & iMessage)
{
    ui->MessageTE->setText(iMessage);
}

void GTAMessageBox::setMessageLabel(const QString & iLabel)
{
    ui->MessageLabel->setText(iLabel);
}

void GTAMessageBox::setAdditionalData(const QStringList & iDataList)
{
    ui->AdditionalDataLW->addItems(iDataList);

}

void GTAMessageBox::setAdditionalDataLabel(const QString & iLabel)
{
    ui->AdditionalDataLabel->setText(iLabel);
}

void GTAMessageBox::onPushButtonClick()
{
    QPushButton * pButton = (QPushButton*)sender();

    if(pButton == ui->OKPB)
        _Result = OK;
    else if(pButton == ui->CancelPB)
        _Result = Cancel;
    else
        _Result = None;

    hide();
}

void GTAMessageBox::setImage(MessageType iType)
{
    QString image;
    switch(iType)
    {
    case Information: image = ":/images/Information"; break;
    case Warning: image = ":/images/Warning"; break;
    case Critical: image = ":/images/Critical"; break;
    }

    QPixmap pixmap(image);
    QIcon icon(pixmap);
    setWindowIcon(icon);
}
void GTAMessageBox::setButtonLabel(const QString & iLabel, Button iButton)
{
    switch (iButton)
    {
    case OKPB:
        ui->OKPB->setText(iLabel);
        break;
    case CANCELPB:
        ui->CancelPB->setText(iLabel);
        break;
    }
}
void GTAMessageBox::setVisibility(bool iVal, Button iButton)
{

    switch (iButton)
    {
    case OKPB:
        ui->OKPB->setVisible(iVal);
        break;
    case CANCELPB:
        ui->CancelPB->setVisible(iVal);
        break;
    }
}