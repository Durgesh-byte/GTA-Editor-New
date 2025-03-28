#include "GTAImageDisplay.h"
#include "ui_GTAImageDisplay.h"

GTAImageDisplay::GTAImageDisplay(QDialog *parent) : QDialog(parent),
    ui(new Ui::GTAImageDisplay)
{
    ui->setupUi(this);
    setModal(Qt::WindowModal);
    setWindowFlags(Qt::Dialog|Qt::WindowMinMaxButtonsHint);
    setWindowTitle("Image");
    connect(ui->closeButton,SIGNAL(clicked()),this,SLOT(closeWindow()));
}

GTAImageDisplay::~GTAImageDisplay()
{
    delete ui;
} 

void GTAImageDisplay::setImage(QPixmap ipImage)
{
    ui->imageViewLabel->setPixmap(ipImage);
}
void GTAImageDisplay::closeWindow()
{
   close();
}