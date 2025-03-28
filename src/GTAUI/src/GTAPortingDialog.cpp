#include "GTAPortingDialog.h"
#include "ui_GTAPortingDialog.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>

GTAPortingDialog::GTAPortingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GTAPortingDialog)
{
    ui->setupUi(this);
    connect(ui->browsePathTB,SIGNAL(clicked()),this,SLOT(onBrowsePortingPathClicked()));
    connect(ui->okPB ,SIGNAL(clicked()),this,SLOT(accepted()));
    connect(ui->cancelPB ,SIGNAL(clicked()),this,SLOT(close()));
}

GTAPortingDialog::~GTAPortingDialog()
{
    delete ui;
}


void GTAPortingDialog::lockWidget()
{
    this->QDialog::setEnabled(false);
}

void GTAPortingDialog::unLockWidget()
{
    this->QDialog::setEnabled(true);
}

void GTAPortingDialog::show()
{
    ui->DirPathLE->clear();
    unLockWidget();
    this->QDialog::show();
}

QString GTAPortingDialog::getPortingPath()const
{
    return ui->DirPathLE->text();
}

void GTAPortingDialog::accepted()
{
    if(!ui->DirPathLE->text().isEmpty())
    {
        QMessageBox::StandardButton button = QMessageBox::information(this,"Porting files",
                                                                      "You are about to port old files to new GTA format.\n This might take a while and the tool will be locked during this process.\nDo you want to continue?",
                                                                      QMessageBox::Yes,QMessageBox::No);
        if(button == QMessageBox::Yes)
        {
            lockWidget();
            emit startPortingFiles(ui->DirPathLE->text());
//            this->QDialog::accepted();
            close();

        }

    }
    else
    {
        QMessageBox::critical(this,"Error","Select Path");
    }
}

void GTAPortingDialog::onBrowsePortingPathClicked()
{
    QString pwd;
    QString path = QFileDialog::getExistingDirectory(this,"Select Porting Directory Path",pwd,QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    ui->DirPathLE->setText(QDir::cleanPath(path));
}

void GTAPortingDialog::close()
{
    this->QDialog::close();
    emit portingDone();
}
