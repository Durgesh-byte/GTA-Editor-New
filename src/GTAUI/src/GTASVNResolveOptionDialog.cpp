#include "GTASVNResolveOptionDialog.h"
#include "ui_GTASVNResolveOptionDialog.h"
#include <QFileDialog>
#include <QDir>
#include "GTAAppController.h"
GTASVNResolveOptionDialog::GTASVNResolveOptionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GTASVNResolveOptionDialog)
{
    ui->setupUi(this);
    connect(ui->okPB,SIGNAL(clicked()),this,SLOT(onOkPBClicked()));
    connect(ui->cancelPB,SIGNAL(clicked()),this,SLOT(onCancelPBClicked()));
    _bResolve=false;
}

GTASVNResolveOptionDialog::~GTASVNResolveOptionDialog()
{
    delete ui;
}

void GTASVNResolveOptionDialog::onOkPBClicked()
{
    _bResolve=true;
    this->hide();
}
void GTASVNResolveOptionDialog::onCancelPBClicked()
{
    _bResolve=false;
    this->hide();
}

 bool GTASVNResolveOptionDialog:: startResolving()
 {
     return _bResolve;
 }

 QString GTASVNResolveOptionDialog:: getUserSelection()
 {
     
         
         

     QString resolveOption="";
     if (ui->resolveBaseRB->isChecked())
     {
        resolveOption="base";
     }
     else if (ui->resolveTheirsRB->isChecked())
     {
        resolveOption="theirs-full";
     }
     else if (ui->resolveMineRB->isChecked())
     {
        resolveOption="mine-full";
     }

     return resolveOption;
 }