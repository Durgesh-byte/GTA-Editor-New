#include "GTASettingsWidget.h"
#include "ui_GTASettingsWidget.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QSettings>
#include <QTextStream>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <qdebug.h>
#include <QFutureWatcher>
#include <QFuture>
#include <QtConcurrentRun>
#include <QtNetwork>
#include "GTAUtil.h"

//Following macros are for equipment page, TypeCB combo box entires
#define EQUIPMENT "EQUIPNAME"
//#define APPLICATION "APPLICATION"
#define NAME "NAME"
#define MEDIA "MEDIA"
#define FILE_NAME "FILE"
#define TOOL_NAME "Tool Name"
#define TOOL_TYPE "TOOL_TYPE"


GTASettingsWidget::GTASettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GTASettingsWidget)
{
    setWindowFlags(Qt::Window);
    setWindowFlags(((this->windowFlags()|Qt::CustomizeWindowHint) & ~Qt::WindowCloseButtonHint));

    ui->setupUi(this);

    _EquipmentTabCnt = -1;
    ui->LogLevelCB->clear();
    ui->LogLevelCB->insertItems(0,QStringList()<<LOG_INFO<<LOG_DEBUG<<LOG_ALL);
    connect(ui->LibPathPB,SIGNAL(clicked()),this,SLOT(onBrowsePBClick()));
    connect(ui->RepositoryPathPB,SIGNAL(clicked()),this,SLOT(onBrowsePBClick()));
    connect(ui->EmoExeFilePB,SIGNAL(clicked()),this,SLOT(onBrowsePBClick())); 
    connect(ui->configPathPB,SIGNAL(clicked()),this,SLOT(onBrowsePBClick()));
    connect(ui->LogPathFolderPB,SIGNAL(clicked()),this,SLOT(onBrowsePBClick()));
    connect(ui->GitDbPB, SIGNAL(clicked()), this, SLOT(onBrowsePBClick()));
    connect(ui->GTAControllerPB, SIGNAL(clicked()), this, SLOT(onBrowsePBClick()));
    connect(ui->AutoLauncherPB, SIGNAL(clicked()), this, SLOT(onBrowsePBClick()));
    connect(ui->AutoLauncherConfigPB, SIGNAL(clicked()), this, SLOT(onBrowsePBClick()));
    connect(ui->GTAControllerOutputLogPB, SIGNAL(clicked()), this, SLOT(onBrowsePBClick()));

    connect(ui->OKPB, SIGNAL(clicked()), this, SLOT(onOKPBClick()));
    connect(ui->CancelPB,SIGNAL(clicked()),this,SLOT(onCancelPBClick()));
    connect(ui->channelSelectionCB,SIGNAL(toggled(bool)),this,SLOT(onChannelSelectionChanged(bool)));
    connect(ui->settingPathTab,SIGNAL(currentChanged(int)),this,SLOT(onTabChanged(int)));
    //    connect(ui->PIRWaitTimeCB,SIGNAL(toggled(bool)),this,SLOT(onPIRWaitTimeCBToggeled(bool)));
    connect(ui->DecimalPrecisionCB,SIGNAL(currentIndexChanged(QString)),this, SLOT(onDecimalPrecisionSelected(QString)));
    connect(ui->LibPathLE,SIGNAL(editingFinished()),this,SLOT(onLibPathEdited()));
    connect(ui->LibPathLE,SIGNAL(textChanged(const QString &)),this,SLOT(onLibPathTextChanged(const QString &)));
    connect(ui->RepositoryPathLE,SIGNAL(editingFinished()),this,SLOT(onRepoPathEdited));
    connect(ui->RepositoryPathLE,SIGNAL(textChanged(const QString &)),this,SLOT(onRepoPathTextChanged(const QString &)));
    connect(ui->ToolDataPathLE,SIGNAL(editingFinished()),this,SLOT(onToolDataPathEdited()));
    connect(ui->ToolDataPathLE, SIGNAL(textChanged(const QString&)), this, SLOT(onToolDataPathTextChanged(const QString&)));
    connect(ui->GitDbLE, SIGNAL(editingFinished()), this, SLOT(onGitDbPathEdited()));
    connect(ui->GitDbLE, SIGNAL(textChanged(const QString&)), this, SLOT(onGitDbTextChanged(const QString&)));
    connect(ui->GTAControllerLE, SIGNAL(editingFinished()), this, SLOT(onGTAControllerPathEdited()));
    connect(ui->GTAControllerLE, SIGNAL(textChanged(const QString&)), this, SLOT(onGTAControllerTextChanged(const QString&)));
    connect(ui->AutoLauncherLE, SIGNAL(editingFinished()), this, SLOT(onAutoLauncherPathEdited()));
    connect(ui->AutoLauncherLE, SIGNAL(textChanged(const QString&)), this, SLOT(onAutoLauncherTextChanged(const QString&)));
    connect(ui->AutoLauncherConfigLE, SIGNAL(editingFinished()), this, SLOT(onAutoLauncherConfigPathEdited()));
    connect(ui->AutoLauncherConfigLE, SIGNAL(textChanged(const QString&)), this, SLOT(onAutoLauncherConfigTextChanged(const QString&)));

    connect(ui->ManualActionTimeoutCB,SIGNAL(toggled(bool)),this,SLOT(onManualActionTimeoutCBToggled(bool)));
    connect(ui->SubscribeUnsubscribeTimeoutCB,SIGNAL(toggled(bool)),this,SLOT(onSubscribeUnsubscribeTimeoutCBToggled(bool)));
    connect(ui->UscxmlBaseSCXML,SIGNAL(toggled(bool)),this,SLOT(onUSCXMLBasedSCXMLCBSelected(bool)));
    connect(ui->RefreshTB,SIGNAL(clicked()),this,SLOT(onRefreshTBClicked()));

    ui->RefreshTB->hide();
    ui->EquipmentNoteLB->hide();

    loadSettings();
    _TabChanged = -1;
    _configFileUpdateDB=false;
    ui->editPropertyFrame->hide();
    //    ui->PIRWaitTimeLE->setDisabled(true);
    _LibPathChanged = false;
    _RepPathChanged = false;
    _ToolDataPathChanged = false;
    _GTAControllerPathChanged = false;
    _AutoLauncherPathChanged = false;
    _AutoLauncherConfigPathChanged = false;

    _currLibFilePath = ui->LibPathLE->text();
    _currRepoFilePath = ui->RepositoryPathLE->text();
	_currToolDataPath = ui->ToolDataPathLE->text();
    _currGitDbPath = ui->GitDbLE->text();
    _currGTAControllerPath = ui->GTAControllerLE->text();
    _currAutoLaucherPath = ui->AutoLauncherLE->text();
    _currAutoLaucherConfigPath = ui->AutoLauncherConfigLE->text();
    _newToolAdded = false;
    _pEquipmentModel = NULL;
    _pFilterModel = NULL;
    ui->EquipmentSetting->setEnabled(false);
    onManualActionTimeoutCBToggled(ui->ManualActionTimeoutCB->isChecked());
    ui->progressBar->hide();
}


GTASettingsWidget::~GTASettingsWidget()
{
    //the application crashes if a future watcher is running and we try to delete the ui component while the future is running
    //this scenario can be represented when GTA is immediately closed after start.
    //the live tool list is still populating and encounters a crash when the ui component is deleted before stopping the future.
    //this hack is made for populateLiveTools() and you can find the futurewatcher is made a child of the class.
    foreach (QObject* object, this->children())
    {
        QFutureWatcher<void>* fwatch = dynamic_cast<QFutureWatcher<void>*>(object);
        if (nullptr != fwatch)
        {
            fwatch->cancel();
            fwatch->waitForFinished();
        }

    }
    delete ui;
}


void GTASettingsWidget::onRepoPathTextChanged(const QString&)
{
    onRepoPathEdited();
}

void GTASettingsWidget::onToolDataPathTextChanged(const QString&)
{
    onToolDataPathEdited();
}

void GTASettingsWidget::onGitDbTextChanged(const QString&)
{
    onGitDbPathEdited();
}

void GTASettingsWidget::onLibPathTextChanged(const QString&)
{
    onLibPathEdited();
}

void GTASettingsWidget::onGTAControllerTextChanged(const QString& value)
{
    if (ui->settingPathTab->currentIndex() != 0)
        return;

    if (!value.isEmpty() && value != _currGTAControllerPath)
    {
        _GTAControllerPathChanged = true;
    }
    else
    {
        _GTAControllerPathChanged = false;
    }
}

void GTASettingsWidget::onAutoLauncherTextChanged(const QString& value)
{
    if (ui->settingPathTab->currentIndex() != 0)
        return;

    if (!value.isEmpty() && value != _currAutoLaucherPath)
    {
        _AutoLauncherPathChanged = true;
    }
    else
    {
        _AutoLauncherPathChanged = false;
    }   
}

/**
* This function captures the Text changed event for AutoLauncherConfig textbox
* @param1 : value -> updated path of AutoLauncher config file path
*/
void GTASettingsWidget::onAutoLauncherConfigTextChanged(const QString& value)
{
    if (ui->settingPathTab->currentIndex() != 0)
        return;

    if (!value.isEmpty() && value != _currAutoLaucherConfigPath)
    {
        _AutoLauncherConfigPathChanged = true;
    }
    else
    {
        _AutoLauncherConfigPathChanged = false;
    }
}

void GTASettingsWidget::onLibPathEdited()
{
    if(ui->settingPathTab->currentIndex() != 0)
        return;

    QString filePath = ui->LibPathLE->text();
    if (!filePath.isEmpty() && filePath != _currLibFilePath)
    {
        _LibPathChanged = true;
    }
    else
        _LibPathChanged = false;

}

void GTASettingsWidget::onRepoPathEdited()
{
    if(ui->settingPathTab->currentIndex() != 0)
        return;

    QString filePath = ui->RepositoryPathLE->text();
    if (!filePath.isEmpty() && filePath != _currRepoFilePath)
    {
        _RepPathChanged = true;
    }
    else
        _RepPathChanged = false;
}

void GTASettingsWidget::onToolDataPathEdited()
{
    if(ui->settingPathTab->currentIndex() != 0)
        return;

    QString filePath = ui->ToolDataPathLE->text();
    if (!filePath.isEmpty() && filePath != _currToolDataPath)
    {
        _ToolDataPathChanged = true;
    }
    else
        _ToolDataPathChanged = false;
}

void GTASettingsWidget::onGTAControllerPathEdited()
{
    if (ui->settingPathTab->currentIndex() != 0)
        return;

    QString filePath = ui->GTAControllerLE->text();
    if (!filePath.isEmpty() && filePath != _currGTAControllerPath)
    {
        _GTAControllerPathChanged = true;
    }
    else
        _GTAControllerPathChanged = false;
}

void GTASettingsWidget::onAutoLauncherPathEdited()
{
    if (ui->settingPathTab->currentIndex() != 0)
        return;

    QString filePath = ui->AutoLauncherLE->text();
    if (!filePath.isEmpty() && filePath != _currAutoLaucherPath)
    {
        _AutoLauncherPathChanged = true;
    }
    else
        _AutoLauncherPathChanged = false;
}

/**
* This function captures the editing finished event for AutoLauncherConfig textbox
*/
void GTASettingsWidget::onAutoLauncherConfigPathEdited()
{
    if (ui->settingPathTab->currentIndex() != 0)
        return;

    QString filePath = ui->AutoLauncherConfigLE->text();
    if (!filePath.isEmpty() && filePath != _currAutoLaucherConfigPath)
    {
        _AutoLauncherConfigPathChanged = true;
    }
    else
    {
        _AutoLauncherConfigPathChanged = false;
    }
}

void GTASettingsWidget::onGitDbPathEdited()
{
    if (ui->settingPathTab->currentIndex() != 0)
        return;

    QString filePath = ui->GitDbLE->text();
    if (!filePath.isEmpty() && filePath != _currGitDbPath)
    {
        _GitDbPathChanged = true;
    }
    else
        _GitDbPathChanged = false;
}

void GTASettingsWidget::onBrowsePBClick()
{

    if(sender() == ui->LibPathPB)
    {
        QString currPath = QDir::cleanPath(ui->LibPathLE->text());
        QFileInfo info(currPath);
        if(currPath.isEmpty() || !info.exists())
            currPath = ".";

        QString temp = QFileDialog::getExistingDirectory(this,"Library Path",currPath);
        QString dirPath = QDir::cleanPath(temp);


        if (!dirPath.isEmpty() && dirPath != currPath)
        {
            ui->LibPathLE->setText(QDir::cleanPath(dirPath));
            _LibPathChanged = true;
        }
        else
            _LibPathChanged = false;
    }
    else if( sender() == ui->RepositoryPathPB)
    {
        QString currPath = ui->RepositoryPathLE->text();
        QFileInfo info(currPath);
        if(currPath.isEmpty() || !info.exists())
            currPath = ".";

        QString dirPath = QFileDialog::getExistingDirectory(this,"Repository Path",currPath);
        if (!dirPath.isEmpty() && dirPath != currPath)
        {
            ui->RepositoryPathLE->setText(QDir::cleanPath(dirPath));
            _RepPathChanged = true;
        }
        else
            _RepPathChanged = false;
    }
    else if(sender() == ui->LogPathFolderPB)
    {
        QString currPath = ui->LogPathLE->text();
        if(currPath.isEmpty())
        {
            currPath = _defaultLogDirPath;
        }

        QFileInfo info(currPath);
        if(!info.exists())
            currPath = ".";

        QString dirPath = QFileDialog::getExistingDirectory(this,"Log Directory Path",currPath);
        if(!dirPath.isEmpty())
        {
            ui->LogPathLE->setText(QDir::cleanPath(dirPath));
        }
    }
    else if(sender() == ui->EmoExeFilePB)
    {
        QString currPath = ui->ToolDataPathLE->text();
        QFileInfo info(currPath);
        if(currPath.isEmpty() || !info.exists())
            currPath = ".";
        QString filePath = QFileDialog::getExistingDirectory(this, "Tool Data Path Folder", currPath);
        if (!filePath.isEmpty() && filePath != currPath)
        {
            ui->ToolDataPathLE->setText(QDir::cleanPath(filePath));
            _ToolDataPathChanged = true;
        }
        else
            _ToolDataPathChanged = false;
    }
    else if (sender() == ui->GitDbPB)
    {
        QString currPath = ui->GitDbLE->text();
        QFileInfo info(currPath);
        if (currPath.isEmpty() || !info.exists())
            currPath = ".";
        QString filePath = QFileDialog::getExistingDirectory(this, "Git Data Path Folder (folder containing .git)", currPath);
        if (!filePath.isEmpty() && filePath != currPath)
        {
            ui->GitDbLE->setText(QDir::cleanPath(filePath));
            _GitDbPathChanged = true;
        }
        else
            _GitDbPathChanged = false;
    }
    else if(sender() == ui->configPathPB)
    {
        QString currPath = ".";
        QString filePath = QFileDialog::getOpenFileName(this,"Bench database config file",currPath,"*.ini");
        if (!filePath.isEmpty())
        {
            QString previousFilePath = ui->configFilePathLE->text();
            if (previousFilePath == filePath)
            {
                _configFileUpdateDB=false;
            }
            else
            {
                ui->configFilePathLE->setText(QDir::cleanPath(filePath));
                _configFileUpdateDB=true;
            }
        }
    }
    else if (sender() == ui->GitDbPB)
    {
        QString currPath = ".";
        QString filePath = QFileDialog::getExistingDirectory();
        if (!filePath.isEmpty())
        {
            QString previousFilePath = ui->GitDbLE->text();
            if (previousFilePath == filePath)
            {
                _configFileUpdateDB = false;
            }
            else
            {
                ui->configFilePathLE->setText(QDir::cleanPath(filePath));
                _configFileUpdateDB = true;
            }
        }
    }
    else if (sender() == ui->GTAControllerPB)
    {
        QString currPath = ".";
        QString filePath = QFileDialog::getOpenFileName(this, "Select GTAController Exe",
            QDir::currentPath(), "*.exe");
        if (!filePath.isEmpty())
        {
            QString previousFilePath = ui->GTAControllerLE->text();
            if (previousFilePath == filePath)
            {
                _GTAControllerPathChanged = false;
            }
            else
            {
                ui->GTAControllerLE->setText(QDir::cleanPath(filePath));
                _GTAControllerPathChanged = true;
            }
        }
    }
    else if (sender() == ui->AutoLauncherPB)
    {
        QString currPath = ".";
        QString filePath = QFileDialog::getExistingDirectory();
        if (!filePath.isEmpty())
        {
            QString previousFilePath = ui->AutoLauncherLE->text();
            if (previousFilePath == filePath)
            {
                _AutoLauncherPathChanged = false;
            }
            else
            {
                ui->AutoLauncherLE->setText(QDir::cleanPath(filePath));
                _AutoLauncherPathChanged = true;
            }
        }
    }
    else if (sender() == ui->AutoLauncherConfigPB)
    {
        QString currPath = ".";
        QString filePath = QFileDialog::getOpenFileName(this, "Select AutoLauncher config file",
            QDir::currentPath(), "*.ini");
        if (!filePath.isEmpty())
        {
            QString previousFilePath = ui->AutoLauncherConfigLE->text();
            if (previousFilePath == filePath)
            {
                _AutoLauncherConfigPathChanged = false;
            }
            else
            {
                ui->AutoLauncherConfigLE->setText(QDir::cleanPath(filePath));
                _AutoLauncherConfigPathChanged = true;
            }
        }
    }
    else if (sender() == ui->AutoLauncherPB)
    {
        QString currPath = ".";
        QString filePath = QFileDialog::getExistingDirectory();
        if (!filePath.isEmpty())
        {
            QString previousFilePath = ui->AutoLauncherLE->text();
            if (previousFilePath == filePath)
            {
                _AutoLauncherPathChanged = false;
            }
            else
            {
                ui->AutoLauncherLE->setText(QDir::cleanPath(filePath));
                _AutoLauncherPathChanged = true;
            }
        }
    }

    else if (sender() == ui->GTAControllerOutputLogPB)
    {
        QString currPath = ".";
        QString filePath = QFileDialog::getExistingDirectory();
        if (!filePath.isEmpty())
        {
            QString previousFilePath = ui->GTAControllerOutputLogLE->text();
            if (previousFilePath == filePath)
            {
                _GTAControllerOutputLogPathChanged = false;
            }
            else
            {
                ui->GTAControllerOutputLogLE->setText(QDir::cleanPath(filePath));
                _GTAControllerOutputLogPathChanged = true;
            }
        }
    }
}
void GTASettingsWidget:: setBatchConfigDBFiles(const QString& iIniFilePath)
{
    //The ini file is in windows format. That means path will containt "\" which is treated as special
    //char by QSettings so input ini file converts to correct format and then QSettings are used.

    QFile symbolFileObj(iIniFilePath);
    bool frc = symbolFileObj.open(QFile::ReadOnly | QIODevice::Text);
    if( frc)
    {            
        QTextStream reader(&symbolFileObj);
        QString contents = reader.readAll();
        contents.replace(QString("\\"),QString("/"));
        
        GTASystemServices* pServices = GTAAppController::getGTAAppController(0)->getSystemService();
        QString sTempPath = pServices->getTemporaryDirectory();
        QString sFileSavePath = QString("%1%2%3").arg(sTempPath,QDir::separator(),"TemproryBatchConfig.ini");
        QFile tempIniFile(QDir::cleanPath(sFileSavePath));
        tempIniFile.open(QFile::WriteOnly | QIODevice::Text);
        QTextStream writer(&tempIniFile);
        writer<<contents;
        tempIniFile.close();
        

        bool removeOldEntries = updateDBStatusForTestConfig()==UPDATE?true:false;
        GTAAppController* pAppctrl = GTAAppController::getGTAAppController(0);
        QStringList filesNotFount = pAppctrl->getSystemService()->setBenchDBConfigFiles(sFileSavePath,removeOldEntries);
    }
}
void GTASettingsWidget::onAddPBClick()
{


}


void GTASettingsWidget::onDeletePBClick()
{

}

/*
void GTASettingsWidget::onProgramSelectionChange(const QString & iVal)
{

}


void GTASettingsWidget::onEquipmentSelectionChange(const QString & iVal)
{
}

void GTASettingsWidget::onStandardSelectionChange(const QString & iVal)
{


}

*/

void GTASettingsWidget::onOKPBClick()
{
    TestConfig* testConf = TestConfig::getInstance();

    //****************************************************************************
    //save only adding new values for program, need to improve for deleted items
    //****************************************************************************
    QString sLibraryPath = ui->LibPathLE->text();
    QString sRepositoryPath = ui->RepositoryPathLE->text();

    QFileInfo sLibraryInfo(sLibraryPath);
    QFileInfo sRepositorynfo(sRepositoryPath);
    QString sErrorMsg = QString();

    QString previousLogLevel = QString::fromStdString(testConf->getLogMessages());
    QString currentLogLevel = ui->LogLevelCB->currentText();
    bool logEnable = ui->EnableMessageLog->isChecked();

    _liveViewToolList.clear();

    if (logEnable)
    {
        if (previousLogLevel != currentLogLevel)
            emit logLevelUpdated(currentLogLevel);
        else
            emit logLevelUpdated(currentLogLevel);
    }

    if (logEnable == false)
        emit logLevelUpdated("");

    saveSettings();

    bool validity = testConf->checkValidity();
    if (!validity)
        sErrorMsg = "Settings are not valid.";
    else
    {
        _isInitialSettingWindow = false;
        hide();

        if (_configFileUpdateDB)
        {
            QString filePath = ui->configFilePathLE->text();
            _iniFileSavePath = QDir::cleanPath(filePath);
            _isRemoveOldEntries = updateDBStatusForTestConfig() == UPDATE ? true : false;
            _RepPathChanged = true;
        }

        if (_pEquipmentModel != NULL)
        {
            ui->MappingTW->setModel(NULL);
            delete _pEquipmentModel;
            _pEquipmentModel = NULL;
        }

        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Procedure database rebuild", "Do you want to rebuild the procedure database?",
            QMessageBox::Yes | QMessageBox::No);
        bool rebuildDb = reply == QMessageBox::Yes ? true : false;

        emit settingsFinished(_LibPathChanged, _RepPathChanged, _ToolDataPathChanged, rebuildDb);
        emit settingsOKPB(sLibraryPath, sRepositoryPath);
        if (_GitDbPathChanged) {
            emit gitDbChanged();
        }
    }

    if (!sErrorMsg.isEmpty())
        QMessageBox::critical(this, "Settings error", sErrorMsg);
}

bool GTASettingsWidget::isTestConfigDataUpdate()const
{
    return _isRemoveOldEntries;
}
QString GTASettingsWidget::getTestConfigFilePath()const
{
    return _iniFileSavePath;
}

bool GTASettingsWidget::isConfigFileUpdateDB()const
{
    return _configFileUpdateDB;
}

void GTASettingsWidget::onCancelPBClick()
{

    if(_pEquipmentModel != NULL)
    {
        ui->MappingTW->setModel(NULL);
        delete _pEquipmentModel;
        _pEquipmentModel = NULL;
    }

    if (_isInitialSettingWindow)
    {
        QMessageBox msgBox(this);
        msgBox.setText("No configuration is provided, tool will be closed. Continue with closing of GTA?");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("Editor Warning");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        int ret = msgBox.exec();
        if (ret == QMessageBox::Yes || ret == QMessageBox::Close)
        {
            QApplication::quit();
        }
        else
            this->show();
    }
    else
    {
        hide();
        emit cancel_settings();
    }
}   



/*
void GTASettingsWidget::loadProgramList(const QString & iDefaultSelectedProgram)
{
    
}

void GTASettingsWidget::loadEquipmentList(const QString & iProgram, const QString & iDefaultSelectedEquip)
{


}

void GTASettingsWidget::loadStandardList(const QString & iProgram, const QString & iEquipment, const QString & iDefaulSelectedStandard )
{


}
*/

void GTASettingsWidget::saveSettings()
{
    TestConfig* testConf = TestConfig::getInstance();

    // PATHS
    testConf->setLibraryPath(ui->LibPathLE->text().toStdString());
    testConf->setRepositoryPath(ui->RepositoryPathLE->text().toStdString());
    testConf->setLogDirPath(ui->LogPathLE->text().toStdString());
    testConf->setToolDataPath(ui->ToolDataPathLE->text().toStdString());
    testConf->setGitDbPath(ui->GitDbLE->text().toStdString());
    testConf->setGTAControllerPath(ui->GTAControllerLE->text().toStdString());
    testConf->setAutoLauncherPath(ui->AutoLauncherLE->text().toStdString());
    testConf->setAutoLauncherConfigPath(ui->AutoLauncherConfigLE->text().toStdString());
    testConf->setGTAControllerOutputLogPath(ui->GTAControllerOutputLogLE->text().toStdString());

    // USER SETTINGS
    testConf->setDecimalPlaces(ui->DecimalPrecisionCB->currentText().toStdString());
    testConf->setLogMessages(ui->LogLevelCB->currentText().toStdString());
    testConf->setLogEnable(ui->EnableMessageLog->isChecked());
    testConf->setResizeRow(ui->ResizeRowCb->isChecked());
    testConf->setSaveBeforeExport(ui->forceSaveforSCXMLCB->isChecked());
    testConf->setBenchDBStartup(ui->BenchDbStartupCB->isChecked());
    testConf->setProcDBStartup(ui->ProcDbStartupCb->isChecked());
    testConf->setValidationEnable(ui->EnableValidationCB->isChecked());
    testConf->setPrintTableStatus(ui->LTRAPrintTableCB->isChecked());
    testConf->setChannelSelection(ui->channelSelectionCB->isChecked());
    testConf->setEngineSelection(ui->EngineSelectionCB->currentText().toStdString());
    testConf->setSubscribeUnsubscribeTimeoutStatus(ui->SubscribeUnsubscribeTimeoutCB->isChecked());
    testConf->setSubscribeUnsubscribeTimeoutValue(ui->SubscribeUnsubscribeTimeoutLE->text().toStdString());
    testConf->setNewSCXMLStruct(ui->NewSCXMLStruct->isChecked());
    testConf->setStandAloneExecMode(ui->StandaloneExec->isChecked());
    testConf->setNewLogFormat(ui->NewLogFormatCB->isChecked());
    testConf->setUSCXMLBasedSCXMLEN(ui->UscxmlBaseSCXML->isChecked());    
    testConf->setManualActionTimeoutStatus(ui->ManualActionTimeoutCB->isChecked());
    testConf->setManualActionTimeoutValue(ui->ManualActionTimeoutLE->text().toStdString());
    testConf->setGenericSCXML(ui->genericSCXML->isChecked());
    testConf->setENInvokeLocalVar(ui->EnInvokeLocalVar->isChecked());

    //save debug IP and Port settings
    QString toolListStr;
    for (int i = 0; i < _liveViewToolList.count(); i++)
        toolListStr.append(QString("%1,%2,%3:").arg(_liveViewToolList.at(i).toolName, _liveViewToolList.at(i).toolIP, QString::number(_liveViewToolList.at(i).port)));

    testConf->setDebugModeIP(ui->debugIPCB->currentText().toStdString());
    testConf->setDebugModePort(ui->debugIPSP->value());
    testConf->setValidationEnable(ui->EnableValidationCB->isChecked());
    testConf->setToolList(toolListStr.toStdString());

    commitCommandSettings();
}

void GTASettingsWidget::loadSettings()
{
    TestConfig* testConf = TestConfig::getInstance();

    QString libPath = QString::fromStdString(testConf->getLibraryPath());
    QString repositoryPath = QString::fromStdString(testConf->getRepositoryPath());
    QString toolDataPath = QString::fromStdString(testConf->getToolDataPath());
    QString logPath = QString::fromStdString(testConf->getLogDirPath());
    QString gitDbPath = QString::fromStdString(testConf->getGitDbPath());
    QString gtaControllerPath = QString::fromStdString(testConf->getGTAControllerPath());
    QString autoLauncherPath = QString::fromStdString(testConf->getAutoLauncherPath());
    QString autoLauncherConfigPath = QString::fromStdString(testConf->getAutoLauncherConfigPath());
    QString gtaControllerOutputLogPath = QString::fromStdString(testConf->getGTAControllerOutputLogPath());

    ui->LibPathLE->setText(libPath);
    ui->RepositoryPathLE->setText(repositoryPath);
    ui->ToolDataPathLE->setText(toolDataPath);
    ui->GitDbLE->setText(gitDbPath);
    ui->GTAControllerLE->setText(gtaControllerPath);
    ui->AutoLauncherLE->setText(autoLauncherPath);
    ui->AutoLauncherConfigLE->setText(autoLauncherConfigPath);
    ui->GTAControllerOutputLogLE->setText(gtaControllerOutputLogPath);


    if (logPath.isEmpty())
        ui->LogPathLE->setText(_defaultLogDirPath);
    else
        ui->LogPathLE->setText(logPath);

    bool bSaveBeforeSCXMLExport = testConf->getSaveBeforeExport();
    ui->forceSaveforSCXMLCB->setChecked(bSaveBeforeSCXMLExport);
    bool bchannelSelectionEnable = testConf->getChannelSelection();
    ui->channelSelectionCB->setChecked(bchannelSelectionEnable);
    ui->EngineSelectionCB->setEnabled(bchannelSelectionEnable ? true : false);
    ui->EngineSelectionCB->clear();
    ui->EngineSelectionCB->insertItems(0, GTAUtil::getAvailableEngines());

    QString engineSetting = QString::fromStdString(testConf->getEngineSelection());
    if (!engineSetting.isEmpty())
    {
        int index = ui->EngineSelectionCB->findText(engineSetting);
        if (index >= 0)
            ui->EngineSelectionCB->setCurrentIndex(index);
    }

    QString decimalPlace = QString::fromStdString(testConf->getDecimalPlaces());
    if (!decimalPlace.isEmpty())
    {
        int index = ui->DecimalPrecisionCB->findText(decimalPlace);
        if (index >= 0)
            ui->DecimalPrecisionCB->setCurrentIndex(index);

    }

    QString logMessage = QString::fromStdString(testConf->getLogMessages());
    if (!logMessage.isEmpty())
    {
        int index = ui->LogLevelCB->findText(logMessage);
        if (index >= 0)
            ui->LogLevelCB->setCurrentIndex(index);
    }

    bool logMessageEnable = testConf->getLogEnable();
    ui->EnableMessageLog->setChecked(logMessageEnable);

    bool updateOnStartStatus = testConf->getSVNUpdateOnStart();
    bool commitOnExitStatus = testConf->getSVNCommitOnExit();
    bool addOnExitStatus = testConf->getSVNAddOnExit();
    bool genericSCXML = testConf->getGenericSCXML();
    bool newSCXML = testConf->getNewSCXMLStruct();
    bool resizeRow = testConf->getResizeRow();
    bool loopTimeout = testConf->getLoopTimeoutSubsteps();
    bool standaloneModeExec = testConf->getStandAloneExecMode();
    bool benchDbStartUp = testConf->getBenchDBStartup();
    bool procDbStartup = testConf->getProcDBStartup();
    bool printTableStatus = testConf->getPrintTableStatus();
    bool EnInvokeForLocalVar = testConf->getENInvokeLocalVar();
    bool NewLogFormat = testConf->getNewLogFormat();
    bool manualActTimeoutStatus = testConf->getManualActionTimeoutStatus();
    QString manualActTimeoutVal = QString::fromStdString(testConf->getManualActionTimeoutValue());
    bool iUSCXMLBasedSCXML = testConf->getUSCXMLBasedSCXMLEN();
    bool SubscribeUnsubscribeTimeoutStatus = testConf->getSubscribeUnsubscribeTimeoutStatus();
    QString SubscribeUnsubscribeTimeoutVal = QString::fromStdString(testConf->getSubscribeUnsubscribeTimeoutValue());
    bool iValidationEnabled = testConf->getValidationEnable();

    ui->UscxmlBaseSCXML->setChecked(iUSCXMLBasedSCXML);
    ui->genericSCXML->setChecked(genericSCXML);
    ui->NewSCXMLStruct->setChecked(newSCXML);
    ui->ResizeRowCb->setChecked(resizeRow);
    ui->loopTimeoutCB->setChecked(loopTimeout);
    ui->StandaloneExec->setChecked(standaloneModeExec);
    ui->BenchDbStartupCB->setChecked(benchDbStartUp);
    ui->ProcDbStartupCb->setChecked(procDbStartup);
    ui->LTRAPrintTableCB->setChecked(printTableStatus);
    ui->EnInvokeLocalVar->setChecked(EnInvokeForLocalVar);
    ui->NewLogFormatCB->setChecked(NewLogFormat);
    ui->ManualActionTimeoutCB->setChecked(manualActTimeoutStatus);
    ui->ManualActionTimeoutLE->setText(manualActTimeoutVal);
    ui->SubscribeUnsubscribeTimeoutCB->setChecked(SubscribeUnsubscribeTimeoutStatus);
    ui->SubscribeUnsubscribeTimeoutLE->setText(SubscribeUnsubscribeTimeoutVal);
    ui->SubscribeUnsubscribeTimeoutLE->setEnabled(SubscribeUnsubscribeTimeoutStatus);
    ui->EnableValidationCB->setChecked(iValidationEnabled);
    updateCommandSettings();

    //fill the debug IP and Ports
    QString debugIP = QString::fromStdString(testConf->getDebugModeIP());
    int debugPort = testConf->getDebugModePort();

    QStringList addressList;
    foreach(QHostAddress host, QNetworkInterface::allAddresses())
    {
        if (QAbstractSocket::IPv4Protocol == host.protocol())
            addressList.append(host.toString());
    }
    addressList.removeDuplicates();

    if ((!debugIP.isEmpty() && !addressList.contains(debugIP)) || debugIP.isEmpty())
    {
        //if the .INI has a previously selected ip and the network interface doesn't holds that IP
        //OR
        //there is no ip available in the .INI
        //flush everything and assign IP available in the current interface
        ui->debugIPCB->clear();
        ui->debugIPCB->insertItems(0, addressList);
        ui->debugIPSP->setMaximum(65535);
        ui->debugIPSP->setMinimum(0);
        ui->debugIPSP->setValue(65432);
    }
    else if (!debugIP.isEmpty() && addressList.contains(debugIP))
    {
        //if the .INI has a previously selected ip and the network interface holds that IP
        //use the IP and Port avilable in the .INI
        ui->debugIPCB->clear();
        ui->debugIPCB->insertItems(0, addressList);
        ui->debugIPSP->setMaximum(65535);
        ui->debugIPSP->setMinimum(0);
        int index = ui->debugIPCB->findText(debugIP);
        if (index >= 0)
        {
            ui->debugIPCB->setCurrentIndex(index);
            ui->debugIPSP->setValue(debugPort);
        }
    }
}

void GTASettingsWidget::keyPressEvent ( QKeyEvent * event )
{
    if (event->key()==Qt::Key_Escape )
    {
        this->hide();
    }
    QWidget::keyPressEvent(event);
}
void GTASettingsWidget::closeEvent(QCloseEvent *event)
{
    event->accept();
}

bool GTASettingsWidget::getSaveElementForSCXML()
{
    return ui->forceSaveforSCXMLCB->isChecked();

}

bool GTASettingsWidget::getChannelSelectionSetting()
{
    return ui->channelSelectionCB->isChecked();
}
bool GTASettingsWidget::getPIRWaitTimeSetting(QString&)
{
    //    oTime = ui->PIRWaitTimeLE->text();
    //    return ui->PIRWaitTimeCB->isChecked();
    return false;
}

bool GTASettingsWidget::getPirInvokeInSCXML()
{
    //    return ui->pirInvokeCB->isChecked();
    return false;
}
bool GTASettingsWidget::getLoopTimeOutState()
{
    return ui->loopTimeoutCB->isChecked();
}
bool GTASettingsWidget::getStandaloneModeExec()
{
    return ui->StandaloneExec->isChecked();
}
bool GTASettingsWidget::getEnInvokeForLocalVar()
{
    return ui->EnInvokeLocalVar->isChecked();
}
bool GTASettingsWidget::getGenericSCXMLState()
{
    return ui->genericSCXML->isChecked();
}
bool GTASettingsWidget::getNewSCXMLState()
{
    return ui->NewSCXMLStruct->isChecked();
}
QString GTASettingsWidget::getDecimalPlaceSelected()
{
    return ui->DecimalPrecisionCB->currentText();
}
QString GTASettingsWidget::getLogMessageSelected()
{
    return ui->LogLevelCB->currentText();
}
bool GTASettingsWidget::getLogEnabled()
{
    return ui->EnableMessageLog->isChecked();
}
bool GTASettingsWidget::getVirtualPMR()
{
    //    return ui->VirtualPMRCB->isChecked();
    return false;
}

bool GTASettingsWidget::getResizeRow()
{
    return ui->ResizeRowCb->isChecked();
}

bool GTASettingsWidget::getValidationEnabler()
{
    return ui->EnableValidationCB->isChecked();
}

QString GTASettingsWidget::getEngineTypeSelection()
{
    QString EngineSelection;
    if(getChannelSelectionSetting())
        EngineSelection = ui->EngineSelectionCB->currentText();

    return EngineSelection;
}
void GTASettingsWidget::onChannelSelectionChanged(bool bSelection)
{
    ui->EngineSelectionCB->clear();
    if (bSelection)
    {
        //        QStringList engines = QStringList()<<"RR"<<"PW"<<"CFM";
        QStringList engines;
        engines  = GTAUtil::getAvailableEngines();
        ui->EngineSelectionCB->insertItems(0,engines);
        ui->EngineSelectionCB->setDisabled(false);
    }
    else
    {
        ui->EngineSelectionCB->setDisabled(true);
    }
}
GTASettingsWidget::DBUpdateType GTASettingsWidget:: updateDBStatusForTestConfig()
{
    if(_configFileUpdateDB==true)
    {
        if (ui->newDBRB->isChecked())
        {
            return UPDATE;
        }

        if (ui->appendDBRB->isChecked())
        {
            return APPEND;
        }

    }
    return DO_NOTHING;
}
void GTASettingsWidget::onTabChanged(int tabOrder)
{
    ui->RefreshTB->hide();
    ui->EquipmentNoteLB->hide();

    switch(tabOrder)
    {
    case PATH_SETTINGS:
    case GEN_SETTINGS:
    case SCXML_SETTINGS:
    case DB_SETTINGS:
    {
        _TabChanged = tabOrder;
        break;
    }
    case CMD_SETTINGS:
    {
        updateCommandSettings();
        _TabChanged  = CMD_SETTINGS;
        break;
    }
    case EQUIPMENT_SETTINGS:
    {
        ui->RefreshTB->show();
        ui->EquipmentNoteLB->show();
        ui->EquipmentNoteLB->clear();
        ui->EquipmentNoteLB->setText("Click refresh button to load/reload equipment settings");
        ui->EquipmentNoteLB->setStyleSheet("QLabel { color : #ff0000; }");
        ui->EquipmentSetting->setEnabled(true);
        updateEquipmentSettings();
        _TabChanged  = EQUIPMENT_SETTINGS;
        break;
    }
    default:
        _TabChanged = tabOrder;
        break;
    };
}

void GTASettingsWidget::onEditEquipmentSettings(const QModelIndex &index)
{
    if(index.isValid())
    {
        QString toolName;
        QString sTypeVal;
        const int row = index.row();
        toolName = _pEquipmentModel->record(row).value(DB_TOOL_NAME).toString();
        QString Type = getType();           //gets type from the combo box and returns DB column name
        if(!Type.isEmpty())
            sTypeVal = _pEquipmentModel->record(row).value(Type).toString();

        ui->EquipmentLE->setText(sTypeVal);
        int singleCBIndex = _Tools.indexOf(toolName);
        ui->SingleToolCB->clear();
        ui->SingleToolCB->addItems(_Tools);
        if(!_Tools.contains("NA"))
            ui->SingleToolCB->addItem("NA");
        if(singleCBIndex >= 0 && singleCBIndex < ui->SingleToolCB->count())
            ui->SingleToolCB->setCurrentIndex(singleCBIndex);
        else
            ui->SingleToolCB->setCurrentIndex(ui->SingleToolCB->count()-1);

    }
}

QString GTASettingsWidget::getType()
{
    QString dbColName;
    QString type = ui->TypeCB->currentText();
    if(type == EQUIPMENT)
    {
        dbColName = DB_EQUIPNAME;
    }
    else if (type == NAME/*APPLICATION*/)
    {
		dbColName = NAME; // APPLICATION; // DB_APPLICATION;
    }
    else if (type == MEDIA)
    {
		dbColName = DB_SIGNAL_TYPE; // DB_MEDIA;
    }
    else if (type == FILE_NAME)
    {
        dbColName = DB_FILE;
    }
    return dbColName;
}
QString GTASettingsWidget::findToolType(const QString &iStr)
{
    QString type = QString("");

    for(int i = 0; i < _toolIdList.count();i++ )
    {
        GTASCXMLToolId obj = _toolIdList.at(i);
        QString objId = obj.getId();
        if(iStr == objId)
        {
            type = obj.getToolType();
            break;
        }
    }
    return type;
}

void GTASettingsWidget::onEquipmentSave()
{
    QString modifiedToolName = ui->SingleToolCB->currentText().trimmed();
    if(!modifiedToolName.isEmpty())
    {
        QItemSelectionModel *selection = ui->MappingTW->selectionModel();
        QString criteria = ui->TypeCB->currentText();
        if(selection && (selection->selectedIndexes().count() > 0))
        {
            QModelIndex index  = selection->selectedIndexes().at(0);
            if(index.isValid() && _pEquipmentModel != NULL)
            {
                QString typeVal = ui->EquipmentLE->text().trimmed();
				showProgressbar(true);
				QFutureWatcher<void> * FutureWatcher = new QFutureWatcher<void>();
				connect(FutureWatcher,SIGNAL(finished()),this,SLOT(onUpdateModelFinished()));
				QFuture<void> Future = QtConcurrent::run(_pEquipmentModel,&GTASqlTableModel::updateToolName,criteria,typeVal,modifiedToolName);
				FutureWatcher->setFuture(Future);                
            }
        }
    }
}


void GTASettingsWidget::showProgressbar(bool iVal)
{
    this->setEnabled(!iVal);
    if(iVal)
    {
        ui->progressBar->setMaximum(0);
        ui->progressBar->setMinimum(0);
        ui->progressBar->show();
    }
    else
        ui->progressBar->hide();
}

void GTASettingsWidget::onEquipmentApplyAll()
{
    QString modifiedToolName = ui->ToolNameCB->currentText().trimmed();
    if(!modifiedToolName.isEmpty() && _pEquipmentModel != NULL)
    {
		showProgressbar(true);
		QFutureWatcher<void> * FutureWatcher = new QFutureWatcher<void>();
		connect(FutureWatcher,SIGNAL(finished()),this,SLOT(onUpdateModelFinished()));
		QFuture<void> Future = QtConcurrent::run(_pEquipmentModel,&GTASqlTableModel::updateToolName,modifiedToolName);
		FutureWatcher->setFuture(Future);
    }
}

void GTASettingsWidget::onTypeCBChanged(int)
{
    QItemSelectionModel *selection = ui->MappingTW->selectionModel();
    if(selection && (selection->selectedIndexes().count() > 0))
    {
        QModelIndex index  = selection->selectedIndexes().at(0);
        if(index.isValid())
            onEditEquipmentSettings(index);
    }
}

void GTASettingsWidget::onSearchTypeChanged(int)
{
    ui->SearchBoxLE->clear();
}

void GTASettingsWidget::onSearchEquipment()
{
    if(_pEquipmentModel != NULL)
    {
        QString type = ui->SearchTypeCB->currentText();
        QString searchStr = ui->SearchBoxLE->text().trimmed();
        _pEquipmentModel->search(type,searchStr);
    }
}

void GTASettingsWidget::onClearSearchBox()
{
    ui->SearchBoxLE->clear();
    if(_pEquipmentModel != NULL)
    {
        _pEquipmentModel->refresh();
    }
}

void GTASettingsWidget::onAddNewToolName()
{
    QString newToolName = ui->NewToolNameLE->text().trimmed();
    if(!newToolName.isEmpty() && !_Tools.contains(newToolName,Qt::CaseInsensitive))
    {

        ui->ToolNameCB->addItem(newToolName);
        ui->RemoveToolCB->addItem(newToolName);

        _Tools.append(newToolName);

        ui->SingleToolCB->clear();
        ui->SingleToolCB->addItems(_Tools);
        if(!_Tools.contains("NA"))
            ui->SingleToolCB->addItem("NA");


        GTASCXMLToolId obj;
        obj.setId(newToolName);
        _toolIdList.append(obj);
        _newToolAdded = true;
        emit newToolIdsAdded(_toolIdList);

    }

}

void GTASettingsWidget::onRemoveToolName()
{
    QString toolName = ui->RemoveToolCB->currentText().trimmed();

    if(_Tools.contains(toolName))
    {
        int index = _Tools.indexOf(toolName);
        if(index >=0 && index < _Tools.count())
        {
            _Tools.removeAt(index);

            int idx = ui->RemoveToolCB->findText(toolName);
            if(idx >=0 && idx < ui->RemoveToolCB->count())
                ui->RemoveToolCB->removeItem(idx);

            idx = ui->ToolNameCB->findText(toolName);
            if(idx >=0 && idx < ui->ToolNameCB->count())
                ui->ToolNameCB->removeItem(idx);

            idx = ui->SingleToolCB->findText(toolName);
            if(idx >=0 && idx < ui->SingleToolCB->count())
            {
                ui->SingleToolCB->removeItem(idx);
            }
        }
        bool found = false;
        GTASCXMLToolId obj;
        for(int i = 0; i < _toolIdList.count();i++)
        {
            obj = _toolIdList.at(i);
            if(obj.getId() == toolName)
            {
                found = true;
                obj = _toolIdList.takeAt(i);
                break;
            }
        }
        if(found == true)
        {
            emit removeToolID(_toolIdList,obj);
        }

    }

}

void GTASettingsWidget::hideMappingTWCols()
{
    QList<int> hideColIds;
    /*hideColIds << DB_NAME_COL_ID << DB_PARAM_TYPE_COL_ID << DB_SIGNAL_TYPE_COL_ID
               << DB_UNIT_COL_ID << DB_VALUE_TYPE_COL_ID << DB_MNVAL_COL_ID
               << DB_MXVAL_COL_ID << DB_LABEL_COL_ID << DB_IDENTIFIER_COL_ID
               << DB_BIT_POS_COL_ID << DB_FILE_LAST_MODIFIED_COL_ID
               << DB_BUSNAME_COL_ID << DB_SIGNAL_NAME_COL_ID << DB_MESSAGE_COL_ID
               << DB_PARAMETER_LOCAL_NAME_COL_ID << DB_FS_NAME_COL_ID << DB_SUFFIX_COL_ID
               << DB_PRECISON_COL_ID << DB_TEMP_PARAM_NAME_COL_ID << DB_DIRECTION_COL_ID
               << DB_MEDIA_TYPE_COL_ID << DB_TOOL_TYPE_COL_ID << DB_REFRESH_RATE_COL_ID;*/

	hideColIds << DB_NAME_COL_ID << DB_PARAM_TYPE_COL_ID << DB_SIGNAL_TYPE_COL_ID
		<< DB_UNIT_COL_ID << DB_VALUE_TYPE_COL_ID << DB_MNVAL_COL_ID
		<< DB_MXVAL_COL_ID << DB_LABEL_COL_ID << DB_PARAMETER_LOCAL_NAME_COL_ID/*DB_IDENTIFIER_COL_ID*/
		<< DB_BIT_POS_COL_ID << DB_FILE_LAST_MODIFIED_COL_ID
		<< DB_MESSAGE_COL_ID << DB_FS_NAME_COL_ID << DB_SUFFIX_COL_ID
		<< DB_PRECISON_COL_ID << DB_DIRECTION_COL_ID << DB_MEDIA_TYPE_COL_ID 
		<< DB_TOOL_TYPE_COL_ID << DB_REFRESH_RATE_COL_ID;

    for(int i = 0; i < hideColIds.count();i++)
    {
        int col = hideColIds.at(i);
        ui->MappingTW->hideColumn(col);
    }

}


void GTASettingsWidget::createEquipmentModel()
{
    GTAAppController *pAppCtrl = GTAAppController::getGTAAppController();
    if(pAppCtrl)
    {
        QSqlDatabase sqlDB = pAppCtrl->getLocalDB();
        if(_pEquipmentModel != NULL)
        {
            delete _pEquipmentModel;
            _pEquipmentModel = NULL;
        }

        _pEquipmentModel = new GTASqlTableModel(this,sqlDB);
        _pEquipmentModel->createEquipmentTable();

		// Allow to fill in the Tool_name field in the "Equipment settings" tab
		_pEquipmentModel->updateToolName(DB_TOOL_NAME, "NA", _defaultToolVal.getId());

        /*if(_EquipmentTabCnt < 0)		//default toolname must be set only once
        {
            QString defaultToolName = _defaultToolVal.getId();
			_pEquipmentModel->updateToolName(DB_TOOL_NAME, "NA", _defaultToolVal.getId()); // defaultToolName);
            _EquipmentTabCnt++;
        }*/
    }
}

void GTASettingsWidget::updateEquipmentSettings()
{
    GTAAppController *pAppCtrl = GTAAppController::getGTAAppController();
    if(pAppCtrl)
    {

        ui->MappingTW->setModel(NULL);

        //data related connects
        connect(ui->MappingTW,SIGNAL(clicked(QModelIndex)),this,SLOT(onEditEquipmentSettings(QModelIndex)));
        connect(ui->TypeCB,SIGNAL(currentIndexChanged(int)),this,SLOT(onTypeCBChanged(int)));
        connect(ui->EquipmentSavePB,SIGNAL(clicked()),this,SLOT(onEquipmentSave()));
        connect(ui->EquipApplyAllPB,SIGNAL(clicked()),this, SLOT(onEquipmentApplyAll()));
        connect(ui->AddNewToolTB,SIGNAL(clicked()),this,SLOT(onAddNewToolName()));
        connect(ui->RemoveTB,SIGNAL(clicked()),this,SLOT(onRemoveToolName()));
        //search related connects
        connect(ui->SearchBoxLE,SIGNAL(textEdited(QString)),this,SLOT(onSearchEquipment()));
        connect(ui->SearchTypeCB,SIGNAL(currentIndexChanged(int)),this,SLOT(onSearchTypeChanged(int)));

        _newToolAdded = false;
        ui->EquipmentLE->clear();
        ui->NewToolNameLE->clear();
        ui->TypeCB->clear();
        ui->SearchTypeCB->clear();

        QStringList types;
        types << EQUIPMENT << NAME/*APPLICATION*/ << TOOL_TYPE << FILE_NAME;
        ui->TypeCB->addItems(types);
        ui->SearchTypeCB->addItems(types);

        _Tools.clear();
        _toolIdList.clear();
        ui->ToolNameCB->clear();
        ui->RemoveToolCB->clear();

        //        createEquipmentModel();

        ui->MappingTW->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->MappingTW->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->MappingTW->setAlternatingRowColors(true);
        ui->MappingTW->setEditTriggers(QAbstractItemView::NoEditTriggers);

        if(_pEquipmentModel != NULL)
            ui->MappingTW->setModel(_pEquipmentModel);


        //        if(_EquipmentTabCnt >= 0)
        pAppCtrl->getSCXMLToolID(_toolIdList,_defaultToolVal);
        for(int i = 0; i < _toolIdList.count();i++)
        {
            GTASCXMLToolId toolId = _toolIdList.at(i);
            _Tools.append(toolId.getId());
            _Tools.removeDuplicates();
        }
        ui->ToolNameCB->addItems(_Tools);
        ui->RemoveToolCB->addItems(_Tools);
    }
}

bool  GTASettingsWidget::getCommandPropertyTable( QHash<QString, THshPropertyVal> & oPropertyTable)
{
    bool rc = false;

    GTAAppController *pAppCtrl = GTAAppController::getGTAAppController(0);
    if(pAppCtrl)
    {
        GTASystemServices* pServices = pAppCtrl->getSystemService();
        if(pServices)
        {
            QString xmlPath = pServices->getCommandPropertyConfigXMLPath();
            xmlPath = QDir::cleanPath(xmlPath);

            QFileInfo xmlInfo(xmlPath);

            bool retVal = xmlInfo.exists();
            if (! retVal)
            {
                retVal = pAppCtrl->generateCmdPropertyFileIfAbsent();
            }
            if(retVal)
            {
                QFile objFile(xmlPath);
                if (objFile.open(QFile::Text | QFile::ReadOnly))
                {
                    QDomDocument domDoc;
                    domDoc.setContent(&objFile);
                    objFile.close();

                    QDomElement rootElem = domDoc.documentElement();
                    QDomNodeList actNodeLst = rootElem.elementsByTagName(XNODE_ACTION);
                    QDomNodeList chkNodeLst = rootElem.elementsByTagName(XNODE_CHECK);

                    if(actNodeLst.isEmpty()==false || chkNodeLst.isEmpty()==false)
                        rc = true;

                    for (int ai=0;ai<actNodeLst.size();ai++)
                    {
                        QDomElement currentActElem = actNodeLst.at(ai).toElement();
                        QString sCurrentActionName = currentActElem.attribute(XATTR_NAME);
                        QDomNodeList actionChildNodes =currentActElem.childNodes();
                        if (actionChildNodes.isEmpty())
                        {
                            QHash<QString,QString> hshAttrVal;
                            getAttributeNameValForNode(currentActElem,hshAttrVal);
                            //verticalHeaderItems.append(hshAttrVal.value(XATTR_NAME));
                            foreach(QString sattrName,hshAttrVal.keys())
                            {

                                QString sVal = hshAttrVal.value(sattrName);
                                THshPropertyVal propValHsh;
                                if(oPropertyTable.contains(sCurrentActionName))
                                    propValHsh=oPropertyTable.value(sCurrentActionName);

                                if(sattrName==XATTR_NAME)
                                    propValHsh.insert(XNODE_COMPLEMENT,QString());
                                else
                                    propValHsh.insert(sattrName,sVal);

                                oPropertyTable.insert(sCurrentActionName,propValHsh);
                            }

                        }
                        else
                        {

                            for (int ci=0;ci<actionChildNodes.size();ci++)
                            {
                                QDomElement currentComplementNode = actionChildNodes.at(ci).toElement();

                                if (currentComplementNode.nodeName()==XNODE_COMPLEMENT)
                                {

                                    QHash<QString,QString> hshAttrValforCom;
                                    QString compName = currentComplementNode.attribute(XATTR_NAME);
                                    getAttributeNameValForNode(currentComplementNode,hshAttrValforCom);
                                    QString encodedname = QString("%1#%2").arg(sCurrentActionName,compName);
                                    foreach(QString sattrName,hshAttrValforCom.keys())
                                    {
                                        QString sVal = hshAttrValforCom.value(sattrName);
                                        THshPropertyVal propValHsh;
                                        if(oPropertyTable.contains(encodedname))
                                            propValHsh=oPropertyTable.value(encodedname);

                                        if(sattrName==XATTR_NAME)
                                            propValHsh.insert(XNODE_COMPLEMENT,compName);
                                        else
                                            propValHsh.insert(sattrName,sVal);

                                        oPropertyTable.insert(encodedname,propValHsh);


                                    }

                                }
                            }

                        }


                    } //for all action nodes end

                    for (int ai=0;ai<chkNodeLst.size();ai++)
                    {
                        QDomElement currentActElem = chkNodeLst.at(ai).toElement();
                        QDomNodeList actionChildNodes =currentActElem.childNodes();
                        if (actionChildNodes.isEmpty())
                        {
                            QString currentElem = currentActElem.attribute(XATTR_NAME);
                            currentElem = currentElem.simplified();
                            currentElem.replace(" ","");

                            QString sEncodedChkName = QString("%1#%2").arg(XNODE_CHECK,currentElem);
                            QHash<QString,QString> hshAttrVal;
                            getAttributeNameValForNode(currentActElem,hshAttrVal);
                            foreach(QString sattrName,hshAttrVal.keys())
                            {
                                QString sVal = hshAttrVal.value(sattrName);
                                THshPropertyVal propValHsh;
                                if (oPropertyTable.contains(sEncodedChkName))
                                    propValHsh=oPropertyTable.value(sEncodedChkName);

                                if(sattrName==XATTR_NAME)
                                    propValHsh.insert(XNODE_COMPLEMENT,QString());
                                else
                                    propValHsh.insert(sattrName,sVal);
                                oPropertyTable.insert(sEncodedChkName,propValHsh);

                            }
                        }
                    }
                }
            }
        }
    }
    return rc;
}
void GTASettingsWidget::updateCommandSettings()
{
    QHash<QString, THshPropertyVal> commandPropertyTable;
    bool rc = getCommandPropertyTable(commandPropertyTable);
    if (rc && commandPropertyTable.isEmpty() == false)
    {
        QStringList verticalHeaderLst;
        QStringList horizontalHdrLst ;

        horizontalHdrLst.append(XNODE_COMPLEMENT);//complement should be first coloum.
        QStringList encodedNameLst = commandPropertyTable.keys();
        foreach (THshPropertyVal propertyVal, commandPropertyTable)
        {
            QStringList propertyLst = propertyVal.keys();
            foreach(QString sPropertyName,propertyLst)
            {
                if(!horizontalHdrLst.contains(sPropertyName))
                    horizontalHdrLst.append(sPropertyName);
            }

        }


        QStringList caNameLst;// this will hold separated list of checks and action names in sorted order.
        QStringList checkNodeNameLst;
        for (int ei=0;ei<encodedNameLst.size();ei++)
        {
            QString currentEncodedName=encodedNameLst.at(ei);
            if (currentEncodedName.startsWith(XNODE_CHECK))
                checkNodeNameLst.append(currentEncodedName);
            else
                caNameLst.append(currentEncodedName);
            
        }
        qSort(caNameLst.begin(), caNameLst.end(), GTAUtil::caseInsensitiveLessThan);
        qSort(checkNodeNameLst.begin(), checkNodeNameLst.end(), GTAUtil::caseInsensitiveLessThan);
        caNameLst.append(checkNodeNameLst);

        //using sorted items create table widget.

        for (int ci=0;ci<caNameLst.size();ci++)
        {
            QString currentEncodedName=caNameLst.at(ci);
            QStringList nameEncodedLst = currentEncodedName.split("#");
            QString cmdName = nameEncodedLst.at(0);
            if (currentEncodedName.startsWith(XNODE_CHECK))
                verticalHeaderLst.append(nameEncodedLst.join(" "));
            else
                verticalHeaderLst.append(cmdName);

        }

        ui->commandSettingsTableWidget->setColumnCount(horizontalHdrLst.size());
        ui->commandSettingsTableWidget->setRowCount(verticalHeaderLst.size());

        ui->commandSettingsTableWidget->setVerticalHeaderLabels(verticalHeaderLst);
        ui->commandSettingsTableWidget->setHorizontalHeaderLabels(horizontalHdrLst);  
        //set resizing property
        ui->commandSettingsTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        //ui->commandSettingsTableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);



        foreach (QString sEncodedName, caNameLst)
        {

            THshPropertyVal propertyVal = commandPropertyTable.value(sEncodedName);
            int dRow = caNameLst.indexOf(sEncodedName);
            foreach(QString sProperty,propertyVal.keys())
            {
                int dCol = horizontalHdrLst.indexOf(sProperty);
                QTableWidgetItem* pItem = ui->commandSettingsTableWidget->item(dRow,dCol);
                if (!pItem)
                {
                    pItem = new QTableWidgetItem();
                    ui->commandSettingsTableWidget->setItem(dRow,dCol,pItem);
                }
                
                pItem->setText(propertyVal.value(sProperty));

                if (sProperty==XNODE_COMPLEMENT)
                {
                    pItem->setFlags(Qt::ItemIsSelectable);
                    //QColor greyColor  = ui->commandSettingsTableWidget->verticalHeaderItem(0)->backgroundColor();
                    QColor greyColor  =  QColor(237,237,237);//this has bee found by trial the actual code should be color of header item
                    pItem->setBackgroundColor(greyColor);
                }
                
            }

        }


    }
    else
    {
        GTAAppController *pAppCtrl = GTAAppController::getGTAAppController(0);
        if(pAppCtrl)
        {   
			pAppCtrl->generateCmdPropertyFileIfAbsent();
        }
    }
}

void GTASettingsWidget::getAttributeNameValForNode(const QDomElement& iElement, QHash<QString,QString> & oHashOfNameValue)
{
    if (iElement.isNull()==false)
    {
        QDomNamedNodeMap nodeMap = iElement.attributes();
        int mapSize = nodeMap.count();

        for (int ni=0;ni<mapSize;ni++)
        {
            QDomNode currentAttrNode = nodeMap.item(ni);
            QDomAttr actAttr = currentAttrNode.toAttr();
            QString attrName = actAttr.name();
            QString attrVal = actAttr.value();
            oHashOfNameValue.insert(attrName,attrVal);
        }

    }

}


void GTASettingsWidget::commitCommandSettings()
{

    QString commandPropertyFilePath = GTAAppController::getGTAAppController()->getSystemService()->getCommandPropertyConfigXMLPath();
    QFile FileObj(commandPropertyFilePath);

    if(!FileObj.exists(commandPropertyFilePath))
    {
        updateCommandSettings();
    }
    else
    {
        bool rc = FileObj.open(QFile::WriteOnly | QFile::Text);
        if(! rc)
        {
            return;

        }

        QDomDocument oDomDoc;
        QDomProcessingInstruction procInstruct = oDomDoc.createProcessingInstruction("xml","version=\"1.0\"");
        oDomDoc.appendChild(procInstruct);


        QDomElement rootElementOfCPXML = oDomDoc.createElement(XNODE_COMMANDS);
        oDomDoc.appendChild(rootElementOfCPXML);

        int dRow = ui->commandSettingsTableWidget->rowCount();
        int dCol = ui->commandSettingsTableWidget->columnCount();

        int complColPos = 0;

        QHash<QString,QDomElement> hshOfElemWithComplement;

        for (int ri =0 ; ri<dRow;ri++)
        {
            QString actName ;
            QTableWidgetItem* verticalItem = ui->commandSettingsTableWidget->verticalHeaderItem(ri);
            if (verticalItem)
            {


                actName= verticalItem->text();
                QTableWidgetItem* complementItem =  ui->commandSettingsTableWidget->item(ri,complColPos);
                QString complemntName;
                if(complementItem)
                    complemntName =complementItem->text();

                if (complemntName.isEmpty())
                {
                    //only act exists
                    QDomElement actNode ;
                    if(actName.startsWith(XNODE_CHECK,Qt::CaseInsensitive))
                    {
                        actName = actName.simplified();
                        actName.replace(" ","");
                        actNode=oDomDoc.createElement(XNODE_CHECK);
//                        actName.remove(XNODE_CHECK);
                        actName.replace(XNODE_CHECK,"");
                        actName.trimmed();
                    }
                    else
                    {

                        actNode=oDomDoc.createElement(XNODE_ACTION);
                    }

                    actNode.setAttribute(XATTR_NAME,actName);
                    rootElementOfCPXML.appendChild(actNode);
                    for (int ci=0;ci<dCol;ci++)
                    {
                        QTableWidgetItem* pColItem = ui->commandSettingsTableWidget->horizontalHeaderItem(ci);
                        QString colName;
                        if (pColItem)
                        {
                            colName = pColItem->text();
                        }

                        if (colName.isEmpty()==false)
                        {
                            if (colName!=XNODE_COMPLEMENT)
                            {
                                QTableWidgetItem* pItemValue = ui->commandSettingsTableWidget->item(ri,ci);
                                if(pItemValue)
                                {

                                    actNode.setAttribute(colName,pItemValue->text());
                                }
                            }
                        }
                    }

                }
                else
                {
                    QDomElement actNode ;//= oDomDoc.createElement(XNODE_ACTION);
                    if (hshOfElemWithComplement.contains(actName))
                    {
                        actNode = hshOfElemWithComplement.value(actName);
                    }
                    else
                    {
                        actNode=oDomDoc.createElement(XNODE_ACTION);
                        actNode.setAttribute(XATTR_NAME,actName);
                        hshOfElemWithComplement.insert(actName,actNode);
                        rootElementOfCPXML.appendChild(actNode);
                    }

                    QDomElement complementNode = oDomDoc.createElement(XNODE_COMPLEMENT);
                    complementNode.setAttribute(XATTR_NAME,complemntName);
                    actNode.appendChild(complementNode);


                    for (int ci=0;ci<dCol;ci++)
                    {
                        QTableWidgetItem* pColItem = ui->commandSettingsTableWidget->horizontalHeaderItem(ci);
                        QString colName;
                        if (pColItem)
                        {
                            colName = pColItem->text();
                        }

                        if (colName.isEmpty()==false)
                        {
                            if (colName!=XNODE_COMPLEMENT)
                            {
                                QTableWidgetItem* pItemValue = ui->commandSettingsTableWidget->item(ri,ci);
                                if(pItemValue)
                                {

                                    complementNode.setAttribute(colName,pItemValue->text());
                                }
                            }
                        }
                    }
                }
            }
        }

        QString contents = oDomDoc.toString();
        QTextStream stream(&FileObj);
        stream <<contents;

        rc = FileObj.flush();
        FileObj.close();

    }
    updateCommandSettings();
}


void GTASettingsWidget::onDecimalPrecisionSelected(QString iVal)
{

}

void GTASettingsWidget::onPIRWaitTimeCBToggeled(bool)
{
    //    ui->PIRWaitTimeLE->setEnabled(iVal);
}

void GTASettingsWidget::show()
{
    _currLibFilePath = ui->LibPathLE->text();
    _currRepoFilePath = ui->RepositoryPathLE->text();
    _currToolDataPath = ui->ToolDataPathLE->text();
    _currGitDbPath = ui->GitDbLE->text();
    _currGTAControllerPath = ui->GTAControllerLE->text();
    _currAutoLaucherPath = ui->GTAControllerLE->text();
    _LibPathChanged = false;
    _RepPathChanged = false;
    _ToolDataPathChanged = false;
    _GTAControllerPathChanged = false;
    _AutoLauncherPathChanged = false;
    _AutoLauncherConfigPathChanged = false;

    if(ui->LogPathLE->text().isEmpty())
    {
        ui->LogPathLE->setText(_defaultLogDirPath);
    }
    this->QWidget::show();
}

void GTASettingsWidget::setDefaultLogDirPath(const QString &iPath)
{
    _defaultLogDirPath = iPath;
}

bool GTASettingsWidget::isBenchDbStartup() const
{
    return ui->BenchDbStartupCB->isChecked();
}

bool GTASettingsWidget::isProcDbStartup() const
{
    return ui->ProcDbStartupCb->isChecked();
}

bool GTASettingsWidget::getLTRAPrintTableStatus()
{
    return (ui->LTRAPrintTableCB->isChecked());
}

void GTASettingsWidget::enEquipTab(bool iEnEqTab)
{
    ui->EquipmentSetting->setEnabled(iEnEqTab);
    ui->RefreshTB->setEnabled(iEnEqTab);
}

void GTASettingsWidget::refreshEquipTable()
{
    if(_pEquipmentModel != NULL)
    {
        _pEquipmentModel->refresh();
    }
}

bool GTASettingsWidget::getNewLogFormatStatus()
{
    return ui->NewLogFormatCB->isChecked();

}

bool GTASettingsWidget::getInitSubscribeStartStatus()
{
    //    return ui->InitSubscribeStartCB->isChecked();
    return true;
}

bool GTASettingsWidget::getInitSubscribeEndStatus()
{
    //    return ui->InitSubscribeEndCB->isChecked();
    return true;
}

void GTASettingsWidget::onManualActionTimeoutCBToggled(bool iVal)
{
    if(iVal)
    {
        ui->ManualActionTimeoutLE->setEnabled(true);
    }
    else
    {
        ui->ManualActionTimeoutLE->setEnabled(false);
    }
}
void GTASettingsWidget::onSubscribeUnsubscribeTimeoutCBToggled(bool iVal)
{
    if(iVal)
    {
        ui->SubscribeUnsubscribeTimeoutLE->setEnabled(true);
    }
    else
    {
        ui->SubscribeUnsubscribeTimeoutLE->setEnabled(false);
    }
}
void GTASettingsWidget::onUSCXMLBasedSCXMLCBSelected(bool)
{
    //    if(iVal)
    //    {
    //        ui->UscxmlBaseSCXML->setEnabled(true);
    //    }
    //    else
    //    {
    //        ui->UscxmlBaseSCXML->setEnabled(false);
    //    }
}

//void GTASettingsWidget::onCurrentTabChanged(int idx)
//{
//    if (idx == 6)
//    {
//        QList<GTALiveToolParams> gtaLiveToolList;
//        //work load of settings for GTA Live View
//        if (ui->ToolListTW->rowCount() == 0)
//        {
//            //the table is empty
//            //fill values from settings
//            GTAAppSettings appSettings;
//            QString bisgIP_gtaLive = appSettings.getGtaLiveBisgIP();
//            int bisgPort_gtaLive = appSettings.getGtaLiveBisgPort();

//            GTALiveToolParams CGIB;
//            CGIB.toolName = "CGIB";
//            CGIB.toolIP = bisgIP_gtaLive;
//            CGIB.port = bisgPort_gtaLive;

//            gtaLiveToolList = appSettings.getLiveViewToolList();
//            gtaLiveToolList.prepend(CGIB);
//        }
//        else
//        {
//            //Update the status of the tools in the list
//            GTALiveToolParams CGIB;
//            CGIB.toolName = "CGIB";
//            CGIB.toolIP = ui->ToolListTW->item(0,1)->text();
//            CGIB.port = ui->ToolListTW->item(0,2)->text().toInt();

//            //clean the table and fill again
//            ui->ToolListTW->clearContents();
//            ui->ToolListTW->setRowCount(0);

//            gtaLiveToolList = _liveViewToolList;
//            gtaLiveToolList.prepend(CGIB);
//        }
//        populateLiveTools(gtaLiveToolList);
//    }

//}

QList<GTALiveToolParams>& GTASettingsWidget::getLiveViewToolList()
{
    return _liveViewToolList;
}

QString GTASettingsWidget::getGtaLiveBisgIP() const
{
    return "127.0.0.1";
}

int GTASettingsWidget::getGtaLiveBisgPort() const
{
    return 2405;
}

bool GTASettingsWidget::getUSCXMLBasedSCXMLCBStatus()
{
    return ui->UscxmlBaseSCXML->isChecked();
}

bool GTASettingsWidget::getManualActionTimeoutStatus()
{
    return ui->ManualActionTimeoutCB->isChecked();
}
QString GTASettingsWidget::getManualActionTimeoutValue()
{
    return ui->ManualActionTimeoutLE->text();
}

bool GTASettingsWidget::getSubscribeUnsubscribeTimeoutStatus()
{
    return ui->SubscribeUnsubscribeTimeoutCB->isChecked();
}
QString GTASettingsWidget::getSubscribeUnsubscribeTimeoutValue()
{
    return ui->SubscribeUnsubscribeTimeoutLE->text();
}

void GTASettingsWidget::onRefreshTBClicked()
{

    if(_pEquipmentModel != NULL)
    {
        _pEquipmentModel->refresh();
    }
    else
    {
        showProgressbar(true);
        QFutureWatcher<void> * FutureWatcher = new QFutureWatcher<void>();
        FutureWatcher->setObjectName("GTASettingsWidget::onRefreshTBClicked");
        connect(FutureWatcher,SIGNAL(finished()),this,SLOT(onUpdateModelFinished()));
        QFuture<void> Future = QtConcurrent::run(this,&GTASettingsWidget::createEquipmentModel);
        FutureWatcher->setFuture(Future);
    }
}

void GTASettingsWidget::onUpdateModelFinished()
{
    QFutureWatcher<void> *pWatcher = dynamic_cast<QFutureWatcher<void> *>(sender());
    if(pWatcher == NULL)
        return;
    else
    {
        if(_pEquipmentModel != NULL )
        {
            if (!pWatcher->objectName().isEmpty() && (pWatcher->objectName().compare("GTASettingsWidget::onRefreshTBClicked") == 0))
            {
                ui->MappingTW->setModel(_pEquipmentModel);
            }
            _pEquipmentModel->refresh();
        }

        showProgressbar(false);

        pWatcher->deleteLater();
    }
}

/**
 * The function returns the selected debug IP.
 * @return: IP for debug mode as QString
*/
QString GTASettingsWidget::getDebugModeIP()
{
    return ui->debugIPCB->currentText();
}

/**
 * The function returns the selected debug Port.
 * @return: Port for debug mode
*/
int GTASettingsWidget::getDebugModePort()
{
    return ui->debugIPSP->value();
}


void GTASettingsWidget::setisInitialSettingWindow(const bool& isInitsettingWindow)
{
    _isInitialSettingWindow = isInitsettingWindow;
}

/**
 * @brief Set the tool name for some  equipments
 * @param iEquipmentToolName 
*/
void GTASettingsWidget::setEquipmentToolName(const QHash<QString, QStringList> iEquipmentToolName)
{ 
    if (!iEquipmentToolName.isEmpty())
    {
        if (_pEquipmentModel == NULL)
            createEquipmentModel();

        updateEquipmentSettings();

        foreach(QString toolName, iEquipmentToolName.keys())
        {
            foreach(QString eqpName, iEquipmentToolName.value(toolName))
            {
                // Search then select the equipment to be updated (e.g. FWC1)
                ui->SearchBoxLE->setText(eqpName);
                _pEquipmentModel->search(DB_EQUIPNAME, eqpName);
                QModelIndex index = ui->MappingTW->model()->index(0, 0);
                emit ui->MappingTW->clicked(index);

                // Set the toolname 
                ui->TypeCB->setCurrentIndex(ui->TypeCB->findText(EQUIPMENT));
                ui->SingleToolCB->setCurrentText(toolName);

                // Apply then save the change
                QString criteria = ui->TypeCB->currentText();
                QString modifiedToolName = ui->SingleToolCB->currentText();
                QItemSelectionModel* selection = ui->MappingTW->selectionModel();
                if (index.isValid() && _pEquipmentModel != NULL)
                {
                    QString typeVal = ui->EquipmentLE->text().trimmed();
                    _pEquipmentModel->updateToolName(criteria, typeVal, modifiedToolName);
                    onUpdateModelFinished();
                }
            }
        }
        onOKPBClick();
    }
    onRefreshTBClicked();
    _TabChanged = EQUIPMENT_SETTINGS;
    ui->SearchBoxLE->clear();
}