#include "GTAGenestaImportWidget.h"
#include "ui_GTAGenestaImportWidget.h"
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QDateTime>
#include <QFile>
#include <QTextStream>


GTAGenestaImportWidget::GTAGenestaImportWidget(GTACompatibilityReport * iReportWidget,QDialog *parent) :
    QDialog(parent),
    ui(new Ui::GTAGenestaImportWidget)

{
    ui->setupUi(this);

    connect(ui->BrowseLibFolderPB,SIGNAL(clicked()),this,SLOT(onBrowseGenestaLibPBClick()));
    connect(ui->BrowseErrorFolderPB,SIGNAL(clicked()),this,SLOT(onBrowseErrorFilePBClick()));
    connect(ui->browseObjectFolderPB,SIGNAL(clicked()),this,SLOT(onBrowseObjectFolderPBClick()));
    connect(ui->browseExportFolderPB,SIGNAL(clicked()),this,SLOT(onBrowseExportFolderPBClick()));
    connect(ui->okPB ,SIGNAL(clicked()),this,SLOT(accepted()));
    connect(ui->cancelPB ,SIGNAL(clicked()),this,SLOT(hide()));
    connect(this ,SIGNAL(onUpdateProgress(int)),this,SLOT(updateProgress(int)));
    ui->progressBar->setMinimum(0);
    ui->progressBar->hide();
    ui->importFolderRB->hide();
    ui->openInGTARB->hide();

    _progress=0;
    _bExporProgress=false;
    _ReportWidget = iReportWidget;

}

GTAGenestaImportWidget::~GTAGenestaImportWidget()
{
    /*foreach(QString program, _ProgramConfig.keys())
    {
        deleteProgram(program);
    }
    _ProgramConfig.clear();*/

    delete ui;
}
void GTAGenestaImportWidget::onBrowseGenestaLibPBClick()
{
    QString sFilePath;

    QString defPath;

    defPath = ui->GenestaFilePathLE->text();
   
    QString objPath = ui->libObjectPathLE->text();
    if(defPath.isEmpty() && !objPath.isEmpty())
    {
        defPath = objPath;
    }
    QFileInfo info(defPath);
    if(defPath.isEmpty() || !info.exists())
        defPath = ".";
    

    if (ui->openInGTARB->isChecked())
    {

        sFilePath = QFileDialog::getOpenFileName(0, ("Open File"),
            defPath,
            ("GENESTAFILES (*.procs *.obj )"));
    }
    else if(ui->importFolderRB->isChecked())
    {
        sFilePath = QFileDialog::getExistingDirectory(0, ("Open Directory"),
            defPath,
            QFileDialog::ShowDirsOnly
            | QFileDialog::DontResolveSymlinks);
    }
    /*  */
    //ui->LibPathLE->setText(QDir::cleanPath(dirPath));

    if (!sFilePath.isEmpty())
        ui->GenestaFilePathLE->setText(QDir::cleanPath(sFilePath));
   

}
void GTAGenestaImportWidget::onBrowseErrorFilePBClick()
{
    QString sFilePath;
    QString defPath = ui->ErrorlLogFilePathLE->text();
    QFileInfo info(defPath);
    defPath = info.absolutePath();
    QFileInfo info1(defPath);
    if(defPath.isEmpty() || !info1.exists())
        defPath = ".";
    sFilePath = QFileDialog::getExistingDirectory(0, ("Open Directory"),
        defPath,
        QFileDialog::ShowDirsOnly
        | QFileDialog::DontResolveSymlinks);

    if (!sFilePath.isEmpty())
    {
        QDateTime dateTime2;

        QString logFile = QString(QString("%1")+QDir::separator()+QString("%2_%3.log")).arg(sFilePath,"GenestaExport",dateTime2.currentDateTime().toString("dd.MM.yyyy.hh.mm.ss"));
        ui->ErrorlLogFilePathLE->setText(QDir::cleanPath(logFile));
    }

   
}
void GTAGenestaImportWidget::accepted()
{
    
    _fileNames.clear();
    

    QString objectFolderPath = ui->libObjectPathLE->text();
    if (!objectFolderPath.isEmpty())
    {
        QFileInfo objectfolderInfo(QDir::cleanPath(objectFolderPath));
        QDir objDirectory(objectFolderPath);
        QStringList objeFileTypes;
        objeFileTypes<<"*.obj";
        QFileInfoList objFileInfolst =objDirectory.entryInfoList ( objeFileTypes, QDir::Files ) ;
        foreach(QFileInfo ffileInfo, objFileInfolst )
        {
            _fileNames.append(ffileInfo.absoluteFilePath());
        }
    }


    QString procfolderPath = ui->GenestaFilePathLE->text();




    //TODO: create function as it is repeated

    if (!procfolderPath.isEmpty())
    {
        QFileInfo procfolderInfo(QDir::cleanPath(procfolderPath));
        QDir procDirectory(procfolderPath);
        QStringList procfileTypes;
        procfileTypes<<"*.procs";
        QFileInfoList procFileInfolst =procDirectory.entryInfoList ( procfileTypes, QDir::Files ) ;
        foreach(QFileInfo ffileInfo, procFileInfolst )
        {
            _fileNames.append(ffileInfo.absoluteFilePath());
        }


    }
  
    updateNoOfFiles(_fileNames.size());

   if ( procfolderPath.isEmpty()  ) 
    {
        this->show();
        QMessageBox::critical(this,"Error","Select Genesta Proc folder for import");
    }
   else if ( objectFolderPath.isEmpty() ) 
   {
       this->show();
       QMessageBox::critical(this,"Error","Select object folder for import");
   }
    else if ( ui->ErrorlLogFilePathLE->text().isEmpty() )
    {
        this->show();
        QMessageBox::critical(this,"Error","Select error log file");
    }
    else
    {
        /*QString sGTAPath = ui->GenestaFilePathLE->text();
        QString sErrorFilePath = */
        
        this->show();
        QString message;
        
        QString noOfFiles = QString::number(_fileNames.size());

        QString replaceMessge;
        if (!ui->replaceFilesCB->isChecked())
            replaceMessge=" not";

        message=QString("You are about to migrate/export %1 files, existing files will%2 be replaced.\nThis might take a while and tool will be locked during migration.\nDo you want to continue??").arg(noOfFiles,replaceMessge);


        QMessageBox msgBox (this);
        msgBox.setText(message);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("File overwrite warning");
        msgBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        int ret = msgBox.exec();
        if(ret==QMessageBox::Yes)
        {
            ui->progressBar->show();
            lockWidget();
            _bExporProgress=true;
            emit startGenestaImport(); 
        }
        
    }            
       
}
void GTAGenestaImportWidget::onBrowseExportFolderPBClick()
{

    QString sFilePath;
    QString defPath = ui->exportFolderLE->text();
    QFileInfo info(defPath);

    if(defPath.isEmpty() || !info.exists())
        defPath = ".";
    if (ui->openInGTARB->isChecked())
    {
        sFilePath = QFileDialog::getOpenFileName(0, ("Open File"),
            defPath,
            ("GENESTAFILES (*.procs *.obj )"));
    }
    else if(ui->importFolderRB->isChecked())
    {
        sFilePath = QFileDialog::getExistingDirectory(0, ("Open Directory"),
            defPath,
            QFileDialog::ShowDirsOnly
            | QFileDialog::DontResolveSymlinks);
    }
    /*  */
    //ui->LibPathLE->setText(QDir::cleanPath(dirPath));

    if (!sFilePath.isEmpty())
        ui->exportFolderLE->setText(QDir::cleanPath(sFilePath));
}
void GTAGenestaImportWidget::onBrowseObjectFolderPBClick()
{

    QString sFilePath;
    QString defPath = ui->libObjectPathLE->text();
    

    QString proPath = ui->GenestaFilePathLE->text();
    if(defPath.isEmpty() && !proPath.isEmpty())
    {
        defPath = proPath;
    }

    QFileInfo info(defPath);
    if(defPath.isEmpty() || !info.exists())
        defPath = ".";



    if (ui->openInGTARB->isChecked())
    {
        sFilePath = QFileDialog::getOpenFileName(0, ("Open File"),
            defPath,
            ("GENESTAFILES (*.procs *.obj )"));
    }
    else if(ui->importFolderRB->isChecked())
    {
        sFilePath = QFileDialog::getExistingDirectory(0, ("Open Directory"),
            defPath,
            QFileDialog::ShowDirsOnly
            | QFileDialog::DontResolveSymlinks);
    }
    /*  */
    //ui->LibPathLE->setText(QDir::cleanPath(dirPath));

    if (!sFilePath.isEmpty())
        ui->libObjectPathLE->setText(QDir::cleanPath(sFilePath));
}
QString GTAGenestaImportWidget::getErrorFilePath()
{
    return ui->ErrorlLogFilePathLE->text();
}
QString GTAGenestaImportWidget::getGenestaLibPath()
{
    return ui->GenestaFilePathLE->text();
}
QString GTAGenestaImportWidget::getObjectPath()
{
    return ui->libObjectPathLE->text();
}
QString GTAGenestaImportWidget:: getExportPath()
{
    return ui->exportFolderLE->text();
}
void GTAGenestaImportWidget::onUpdateProgress( int)
{
   
    ui->progressBar->setValue(++_progress);
    ui->progressBar->show();
    if (_progress >= ui->progressBar->maximum())
    {
        QMessageBox::information(this,"Migration","Export/Migration completed");

        //Display the error message file if it has any. "Error: line" 
        QFile errorFileObj(getErrorFilePath());
        bool frc = errorFileObj.open(QFile::ReadOnly | QIODevice::Text);
        if(frc)
        {
            QString contents;
            QTextStream in(&errorFileObj);
            bool hasErros = false;
            while (!in.atEnd()) 
            {
                contents.append(in.readLine());
                contents.append("\n");
                if(!hasErros)
                {
                    if( contents.contains(QString("Error: line")))
                        hasErros=true;
                }

            }

            if (hasErros)
            {  
				_ReportWidget->setText(contents);
				_ReportWidget->show();
            }

        }

        this->hide();
    }
    

}
void GTAGenestaImportWidget::updateNoOfFiles(const int& iNoOfFiles)
{
    ui->progressBar->setRange(0,iNoOfFiles);
}

void GTAGenestaImportWidget::hideProgress()
{
    ui->progressBar->hide();
}
bool GTAGenestaImportWidget::getOverWriteState()
{
    if (ui->replaceFilesCB->isChecked())
    {
        return true;
    }
    return false;
}
void GTAGenestaImportWidget::lockWidget()
{
    ui->progressBar->show();
    ui->okPB->setDisabled(true);
    ui->cancelPB->setDisabled(true);
    ui->replaceFilesCB->setDisabled(true);

    ui->BrowseLibFolderPB->setDisabled(true);
    ui->BrowseErrorFolderPB->setDisabled(true);
    ui->browseObjectFolderPB->setDisabled(true);
    ui->browseExportFolderPB->setDisabled(true);
    
    ui->GenestaFilePathLE->setDisabled(true);
    ui->ErrorlLogFilePathLE->setDisabled(true);
    ui->exportFolderLE->setDisabled(true);
    ui->libObjectPathLE->setDisabled(true);

}
void GTAGenestaImportWidget::unLockWidget()
{
    ui->progressBar->hide();

    ui->okPB->setDisabled(false);
    ui->cancelPB->setDisabled(false);
    ui->replaceFilesCB->setDisabled(false);

    ui->BrowseLibFolderPB->setDisabled(false);
    ui->BrowseErrorFolderPB->setDisabled(false);
    ui->browseObjectFolderPB->setDisabled(false);
    ui->browseExportFolderPB->setDisabled(false);

    ui->GenestaFilePathLE->setDisabled(false);
    ui->ErrorlLogFilePathLE->setDisabled(false);
    ui->exportFolderLE->setDisabled(false);
    ui->libObjectPathLE->setDisabled(false);

}
void GTAGenestaImportWidget::setDefaultExportPath(const QString & iFilePath)
{
    ui->exportFolderLE->setText(iFilePath);

}
void GTAGenestaImportWidget::show()
{
    _progress=0;
    ui->progressBar->setValue(0);
    this->QDialog::show();
    _bExporProgress=false;
    

}
void GTAGenestaImportWidget::closeEvent ( QCloseEvent * event )
{
    if (_progress < ui->progressBar->maximum() && _bExporProgress)
    {
        event->ignore();
    }
    else
    {
        _bExporProgress=false;
        event->accept();
    }
}
