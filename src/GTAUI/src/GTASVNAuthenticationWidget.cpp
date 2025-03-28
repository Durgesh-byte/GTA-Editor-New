#include "GTASVNAuthenticationWidget.h"
#include "ui_GTASVNAuthenticationWidget.h"
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>

GTASVNAuthenticationWidget::GTASVNAuthenticationWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GTASVNAuthenticationWidget)
{
    ui->setupUi(this);

    QPixmap acceptIcon(":/images/forms/img/accept_svn.png");
    QPixmap cancelIcon(":/images/forms/img/cancel_svn.png");
    ui->loginTB->setIcon(QIcon(acceptIcon));
    ui->loginTB->setAutoRaise(true);
    ui->loginTB->setFocus();
    ui->canceTB->setIcon(QIcon(cancelIcon));
    ui->canceTB->setAutoRaise(true);

    connect(ui->loginTB,SIGNAL(clicked()),this,SLOT(onLoginTBClicked()));
    connect(ui->canceTB,SIGNAL(clicked()),this,SLOT(onCancelTBClicked()));

}

GTASVNAuthenticationWidget::~GTASVNAuthenticationWidget()
{
    delete ui;
}

QString GTASVNAuthenticationWidget::getUserName()
{
    return ui->userNameLE->text();
}

QString GTASVNAuthenticationWidget::getPassword()
{
    return ui->passwordLE->text();
}

bool GTASVNAuthenticationWidget::getSaveAuthenticationState()
{
    if(ui->saveAuthenticationCB->isChecked())
        return true;
    else
        return false;
}

void GTASVNAuthenticationWidget::clearCredentials()
{
    ui->userNameLE->clear();
    ui->passwordLE->clear();
}

void GTASVNAuthenticationWidget::onCancelTBClicked()
{
    clearCredentials();
    this->close();
}

void GTASVNAuthenticationWidget::onLoginTBClicked()
{
    if(getUserName().isEmpty() || getPassword().isEmpty())
    {
        QMessageBox::warning(this,"SVN Authentication","Username or password can't be empty");
    }
    else
        this->close();
}


