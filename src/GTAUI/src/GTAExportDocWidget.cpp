#include "GTAExportDocWidget.h"
#include "ui_GTAExportDocWidget.h"
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>
#include <QFileDialog>
GTAExportDocWidget::GTAExportDocWidget(QDialog *parent) : QDialog(parent),
    ui(new Ui::GTAExportDocWidget)
{
    ui->setupUi(this);
    connect(ui->exportPB,SIGNAL(clicked()),this,SLOT(onExportAccepted(void)));
    connect(ui->cancelPB,SIGNAL(clicked()),this,SLOT(hide()));
    connect(ui->browseFolderTB,SIGNAL(clicked()),this,SLOT(onBrowseFolder()));
    connect(ui->browseLogPathPB,SIGNAL(clicked()),this,SLOT(onBrowseLogFile()));
    //connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(onSaveAccepted(void)));

    ui->WarningLB->setVisible(false);
}
GTAExportDocWidget::~GTAExportDocWidget()
{
    delete ui;
}


void GTAExportDocWidget::onExportAccepted()
{
    QString logFile = ui->LogFilePathLE->text();

    QString errors = ui->informationLE->toPlainText();
    errors.trimmed();
    QString sFolderPath = ui->saveToFolderLE->text();
    QFileInfo fileInfo(QDir::cleanPath(sFolderPath));
    if (!fileInfo.exists())
    {
        QMessageBox::critical(this,"Folder missing",QString("Folder \"%1\" does not exist").arg(sFolderPath));
        return;
    }
    bool bExportBenchDB     = ui->includeBDCB->isChecked();
    bool bCompress          = ui->compressAllCB->isChecked();
    bool bExportSCXML       = ui->scxmlCB->isChecked();
    if (!errors.isEmpty())
    {
        QMessageBox msgBox (this);
        msgBox.setText("Errors found, continue exporting?");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("Editor Warning");
        msgBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        int ret = msgBox.exec();

        if(_lstOfFiles.contains("PARAMETER.csv"))
        {
            _lstOfFiles.removeAll("PARAMETERS.csv");
        }

        if(ret==QMessageBox::Yes)
        {


            emit  startExporting(sFolderPath,_saveFolderName,_lstOfFiles,bExportBenchDB,bExportSCXML,bCompress,_ExportFileName,_LogFilePath);
        }
    }
    else
        emit  startExporting(sFolderPath,_saveFolderName,_lstOfFiles,bExportBenchDB,bExportSCXML,bCompress,_ExportFileName,_LogFilePath);



}
void GTAExportDocWidget::setErrorInformation(const QString& iError)
{
    if (iError.isEmpty())
    {
        ui->InformationGB->hide();
        ui->informationLE->hide();
        this->resize(this->minimumSize());
        ui->informationLE->setText("");
    }
    else
    {
        ui->informationLE->setText(iError);
        ui->InformationGB->show();
        ui->informationLE->show();
    }

}
void GTAExportDocWidget::setPath(const QString& iPath)
{
    ui->saveToFolderLE->setText(iPath);

}

void GTAExportDocWidget::onBrowseLogFile()
{
    QString defaultFilePath = ui->LogFilePathLE->text();
    QString filePath= QFileDialog::getOpenFileName(this, "Select Log file", (defaultFilePath.isEmpty()) ? "." : defaultFilePath, "Log files(*.log)");

    if(!filePath.isEmpty())
    {
        ui->LogFilePathLE->setText(filePath);
        _LogFilePath = filePath;
    }
}

void GTAExportDocWidget::onBrowseFolder()
{
    QString defaultFolderPath = ui->saveToFolderLE->text();
    QString sFolderPath= QFileDialog::getExistingDirectory(this, ("Open Directory"), (defaultFolderPath.isEmpty()) ? ".": defaultFolderPath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!sFolderPath.isEmpty())
    {
        ui->saveToFolderLE->setText(sFolderPath);
    }
}
void GTAExportDocWidget::setSaveFolder(const QString& iFolder)
{
    _saveFolderName = iFolder;
}
void GTAExportDocWidget::setLstOfFilesToExport(const QStringList& ilstFiles)
{
    _lstOfFiles = ilstFiles ;
}
void GTAExportDocWidget::scxmlExportDisable(bool ibDisable)
{

    ui->scxmlCB->setChecked(!ibDisable);
    ui->scxmlCB->setDisabled(ibDisable);
}
void GTAExportDocWidget::setExportFileName(const QString &iFileName)
{
    _ExportFileName = iFileName;
}

void GTAExportDocWidget::show()
{
    this->QDialog::show();
}

void GTAExportDocWidget::setDefaultLogFolderPath(const QString &iPath)
{
    ui->WarningLB->setVisible(false);
    _LogFolderPath = QDir::cleanPath(iPath);
    QDir dir(_LogFolderPath);
    if(dir.exists())
    {
        QFileInfoList infoList = dir.entryInfoList(QDir::Files);
        bool found = false;
        foreach(QFileInfo info, infoList)
        {
            QString logFileName = info.fileName();
            int lasIndexOfDot = logFileName.lastIndexOf(".");
            QString logName = logFileName.mid(0,lasIndexOfDot).trimmed();

            QString elemName = _ExportFileName;
            elemName = elemName.split("/",QString::SkipEmptyParts).last();
            lasIndexOfDot = elemName.lastIndexOf(".");
            QString element = elemName.mid(0,lasIndexOfDot).trimmed();


            if(logName == element)
            {
                _LogFilePath = info.absoluteFilePath();
                ui->LogFilePathLE->setText(_LogFilePath);
                found = true;
                break;
            }

        }
        if(!found)
        {
            ui->LogFilePathLE->setText(iPath);
            ui->WarningLB->setVisible(true);
            ui->WarningLB->setStyleSheet("QLabel { color : red; }");
        }

    }

}
