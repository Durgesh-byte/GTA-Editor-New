#include "GTAImportDialog.h"
#include "ui_GTAImportDialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include "GTAUtil.h"

//#define GENESTA_STR "Genesta"
//#define FSII_STR "FlatScript"
//#define AVATAR_STR "AVATAR"
//#define AUTO_TR_STR "AutoTR"


GTAImportDialog::GTAImportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GTAImportDialog)
{
    ui->setupUi(this);

    ui->ExportPathLE->clear();
    ui->ErrorLogPathLE->clear();
    ui->ImportPathLE->clear();
    ui->replaceFilesCB->setChecked(false);

    connect(ui->ExportPB,SIGNAL(clicked()),this, SLOT(onBrowseExportPathClicked()));
    connect(ui->ImportPB,SIGNAL(clicked()),this,SLOT(onBrowseImportPathClicked()));
    connect(ui->ErrorPB,SIGNAL(clicked()),this,SLOT(onBrowseErrorPathClicked()));
    connect(ui->okPB ,SIGNAL(clicked()),this,SLOT(accepted()));
    connect(ui->cancelPB ,SIGNAL(clicked()),this,SIGNAL(widgetclosed()));
    setWindowFlags((this->windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowCloseButtonHint);
    connect(ui->cancelPB ,SIGNAL(clicked()),this,SIGNAL(widgetclosed()));
    setWindowFlags((this->windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowCloseButtonHint);
}

GTAImportDialog::~GTAImportDialog()
{
    delete ui;
}

void GTAImportDialog::show()
{
    ui->ImportTypeCB->setCurrentIndex(0);
    ui->ErrorLogPathLE->clear();
    ui->ImportPathLE->clear();
    ui->replaceFilesCB->setChecked(false);
    this->QDialog::show();
}

void GTAImportDialog::lockWidget()
{
    controlWidget(false);
}

void GTAImportDialog::unLockWidget()
{
    controlWidget(true);
}


//Controls the enabling and disabling of the UI componenets. Pass true to enable and false to disable
void GTAImportDialog::controlWidget(bool iControlVal)
{
    ui->ImportTypeCB->setEnabled(iControlVal);
    ui->ExportPathLE->setEnabled(iControlVal);
    ui->ExportPB->setEnabled(iControlVal);
    ui->ExportPathLE->setEnabled(iControlVal);
    ui->ImportPathLE->setEnabled(iControlVal);
    ui->ImportPB->setEnabled(iControlVal);
    ui->replaceFilesCB->setEnabled(iControlVal);
    ui->okPB->setEnabled(iControlVal);
    ui->cancelPB->setEnabled(iControlVal);
    ui->ErrorLogPathLE->setEnabled(iControlVal);
    ui->ErrorPB->setEnabled(iControlVal);
}


void GTAImportDialog::accepted()
{

    QString ExportPath = ui->ExportPathLE->text();
    QString ImportPath = ui->ImportPathLE->text();
    QString ErrorPath = ui->ErrorLogPathLE->text();

    if(ExportPath.isEmpty())
    {
        this->show();
        QMessageBox::critical(this,"Error","Select Export Path");
    }
    else if(ImportPath.isEmpty())
    {
        this->show();
        QMessageBox::critical(this,"Error","Select Import Path");
    }
    else if(ErrorPath.isEmpty())
    {
        this->show();
        QMessageBox::critical(this,"Error","Select Error Path");
    }

    if(!ExportPath.isEmpty() &&
            !ImportPath.isEmpty() &&
            !ErrorPath.isEmpty())
    {

        QString importType = ui->ImportTypeCB->currentText();

        QString replaceMessge;
        if (!ui->replaceFilesCB->isChecked())
            replaceMessge=" not";

        QString message=QString("You are about to export %2 files, existing files will%1 be replaced.\nThis might take a while and tool will be locked during migration.\nDo you want to continue??").arg(replaceMessge,importType);

        QMessageBox msgBox(this);
        msgBox.setText(message);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle(QString("%1 Import").arg(importType));
        msgBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        int ret = msgBox.exec();
        if(ret==QMessageBox::Yes)
        {
            lockWidget();
            emit startImport();

        }
    }

}

void GTAImportDialog::setDefaultExportPath(const QString &iDirPath)
{
    ui->ExportPathLE->setText(iDirPath);
}

void GTAImportDialog::onBrowseExportPathClicked()
{
    QString pwd;
    QString path = QFileDialog::getExistingDirectory(this,"Select Folder to export",pwd,QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    ui->ExportPathLE->setText(QDir::cleanPath(path));
}

void GTAImportDialog::onBrowseImportPathClicked()
{
    //    QString importType = ui->ImportTypeCB->currentText();

    QString pwd;
    QString path = QFileDialog::getExistingDirectory(this,"Select Import Path",pwd,QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    ui->ImportPathLE->setText(QDir::cleanPath(path));
    ui->ErrorLogPathLE->setText(QDir::cleanPath(path));

}

void GTAImportDialog::onBrowseErrorPathClicked()
{
    QString pwd;
    QString path = QFileDialog::getExistingDirectory(this,"Save Error file at path ...",pwd,QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    ui->ErrorLogPathLE->setText(QDir::cleanPath(path));
}


QString GTAImportDialog::getExportPath()const
{
    return ui->ExportPathLE->text();
}
QString GTAImportDialog::getImportPath()const
{
    return ui->ImportPathLE->text();
}
QString GTAImportDialog::getErrorPath()const
{
    return ui->ErrorLogPathLE->text();
}

bool GTAImportDialog::getOverWriteFileStatus()const
{
    return (ui->replaceFilesCB->isChecked());
}

QString GTAImportDialog::getPluginType()const
{
    return ui->ImportTypeCB->currentText();
}

void GTAImportDialog::setPluginTypes(const QStringList &iList)
{
    ui->ImportTypeCB->clear();
    ui->ImportTypeCB->addItems(iList);
}
