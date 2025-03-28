#include "GTARenameDialog.h"
#include "ui_GTARenameDialog.h"
#include<QMessageBox>

GTARenameDialog::GTARenameDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GTARenameDialog)
{
    ui->setupUi(this);
    connect(ui->okPB,SIGNAL(clicked()),this,SLOT(accepted()));
    connect(ui->cancelPB,SIGNAL(clicked()),this,SLOT(hide()));
    //connect(ui->NameLE,SIGNAL(textEdited(QString)),this,SLOT(ValidateName(QString)));
}

QString GTARenameDialog::getName()const
{
    QString name;
    name = ui->NameLE->text();
    return name;
}


GTARenameDialog::~GTARenameDialog()
{
    delete ui;
}

void GTARenameDialog::accepted()
{
    QString name = getName();
    if(name.isEmpty())
    {
        QMessageBox msgBox(this);
        msgBox.setText("The name field cannot be empty. Please enter name");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle("Error");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
    }
    else
    {
        this->hide();
        emit renameDone(name);
    }
}

void GTARenameDialog::ValidateName(QString iName)
{
    /*QRegExp rx("([\.?/'*:<>\"\\\\]+)");
    if(iName.contains(rx))
    {
        iName.remove(rx);
        ui->NameLE->setText(iName);
    }*/
}

void GTARenameDialog::show()
{
//    ui->NameLE->clear();
    QDialog::show();
}

void GTARenameDialog::setFileName(const QString &iFileName)
{
    ui->NameLE->setText(iFileName);
}
