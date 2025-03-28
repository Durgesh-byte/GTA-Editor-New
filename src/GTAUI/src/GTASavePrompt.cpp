#include "GTASavePrompt.h"
#include "ui_GTASavePrompt.h"

GTASavePrompt::GTASavePrompt(QWidget *parent) :
QDialog(parent),
    ui(new Ui::GTASavePrompt)
{
    ui->setupUi(this);
    connect(ui->savePB,SIGNAL(clicked()),this,SLOT(onSaveAllClicked()));
    connect(ui->NoPB,SIGNAL(clicked()),this,SLOT(onNoPBClicked()));
    connect(ui->CancelPB,SIGNAL(clicked()),this,SLOT(onCancelPBclicked()));
}

GTASavePrompt::~GTASavePrompt()
{
    delete ui;
}

void GTASavePrompt::setFiles(QStringList &iFiles)
{
    ui->FileLW->clear();
    foreach(QString file,iFiles)
    {
        QListWidgetItem *pItem = new QListWidgetItem;
        pItem->setText(file);
        ui->FileLW->addItem(pItem);
    }
}

void GTASavePrompt::onSaveAllClicked()
{
   this->QDialog::done(Accepted);
}

void GTASavePrompt::onNoPBClicked()
{
    this->QDialog::done(Rejected);
}

void GTASavePrompt::onCancelPBclicked()
{
    this->QDialog::done(Ignored);
}
