#include "GTAFSIIImportWidget.h"
#include "ui_GTAFSIIImportWidget.h"
#include <QFileDialog>
#include <QMessageBox>

GTAFSIIImportWidget::GTAFSIIImportWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GTAFSIIImportWidget)
{
    ui->setupUi(this);
    ui->progressBar->hide();

    connect(ui->BrowseExportPathPB,SIGNAL(clicked()),this, SLOT(onBrowseExportPathClicked()));
    connect(ui->BrowseImportPathPB,SIGNAL(clicked()),this,SLOT(onBrowseImportPathClicked()));
    connect(ui->BrowseErrorFolderPB,SIGNAL(clicked()),this,SLOT(onBrowseErrorPathClicked()));
    connect(ui->okPB ,SIGNAL(clicked()),this,SLOT(accepted()));
    connect(ui->cancelPB, SIGNAL(clicked()), this, SLOT(hide()));
}

GTAFSIIImportWidget::~GTAFSIIImportWidget()
{
    delete ui;
}

void GTAFSIIImportWidget::accepted()
{
     QString ExportPath = ui->ExportFilePathLE->text();
    QString ImportPath = ui->ImportPathLE->text();
    QString ErrorPath = ui->ErrorlLogFilePathLE->text();

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

    if(!ExportPath.isEmpty() && !ImportPath.isEmpty() && !ErrorPath.isEmpty())
    {
        QString replaceMessge;
        if (!ui->replaceFilesCB->isChecked())
            replaceMessge=" not";

        QString message=QString("You are about to export flat script files, existing files will%1 be replaced.\nThis might take a while and tool will be locked during migration.\nDo you want to continue??").arg(replaceMessge);


        QMessageBox msgBox(this);
        msgBox.setText(message);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("Flat Script Import");
        msgBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        int ret = msgBox.exec();
        if(ret==QMessageBox::Yes)
        {
            //           ui->progressBar->show();
            lockWidget();
            //  _bExporProgress=true;

            emit startFSIIImport();
        }
    }
}

void GTAFSIIImportWidget::onBrowseExportPathClicked()
{
    QString pwd;
    QString path = QFileDialog::getExistingDirectory(this,"Select Folder to export Flat Script files",pwd,QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    ui->ExportFilePathLE->setText(path);

}

void GTAFSIIImportWidget::onBrowseImportPathClicked()
{
    QString pwd;
    QString path = QFileDialog::getExistingDirectory(this,"Select Import Path",pwd,QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    ui->ImportPathLE->setText(path);
    ui->ErrorlLogFilePathLE->setText(path);
}


void GTAFSIIImportWidget::onBrowseErrorPathClicked()
{
    QString pwd;
    QString path = QFileDialog::getExistingDirectory(this,"Save Error file at path ...",pwd,QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    ui->ErrorlLogFilePathLE->setText(path);

}

QString GTAFSIIImportWidget::getExportPath()const
{
    return ui->ExportFilePathLE->text();
}
QString GTAFSIIImportWidget::getImportPath()const
{
    return ui->ImportPathLE->text();
}
QString GTAFSIIImportWidget::getErrorPath()const
{
    return ui->ErrorlLogFilePathLE->text();
}
bool GTAFSIIImportWidget::getOverWriteFileStatus()
{
    return (ui->replaceFilesCB->isChecked());
}

void GTAFSIIImportWidget::lockWidget()
{
    //   ui->progressBar->show();
    ui->okPB->setDisabled(true);
    ui->cancelPB->setDisabled(true);
    ui->replaceFilesCB->setDisabled(true);

    ui->BrowseExportPathPB->setDisabled(true);
    ui->BrowseImportPathPB->setDisabled(true);
    ui->BrowseErrorFolderPB->setDisabled(true);

    ui->ExportFilePathLE->setDisabled(true);
    ui->ImportPathLE->setDisabled(true);
    ui->ErrorlLogFilePathLE->setDisabled(true);

}
void GTAFSIIImportWidget::unLockWidget()
{
    //ui->progressBar->hide();

    //ui->okPB->setDisabled(false);
    ui->cancelPB->setDisabled(false);
    ui->replaceFilesCB->setDisabled(false);

    ui->BrowseExportPathPB->setDisabled(false);
    ui->BrowseImportPathPB->setDisabled(false);
    ui->BrowseErrorFolderPB->setDisabled(false);

    ui->ExportFilePathLE->setDisabled(false);
    ui->ImportPathLE->setDisabled(false);
    ui->ErrorlLogFilePathLE->setDisabled(false);

}


void GTAFSIIImportWidget::show()
{
    ui->ExportFilePathLE->clear();
    ui->ImportPathLE->clear();
    ui->ErrorlLogFilePathLE->clear();
    this->QDialog::show();
}


void GTAFSIIImportWidget::setDefaultExportPath(const QString &iDefPath)
{
     ui->ExportFilePathLE->setText(iDefPath);
}
