#include "AINGTASEQUENCERAppMainWindow.h"
#include "AINGTASEQUENCERConfigurationManager.h"
#include "ui_AINGTASEQUENCERAppMainWindow.h"
#include "AINGTASEQUENCERConfigDbManager.h"
#include <QFile>
#include <QFileDialog>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QUrl>
#include <QWidget>
#include <QMessageBox>
#include <QFileInfo>


AINGTASEQUENCERAppMainWindow::AINGTASEQUENCERAppMainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::AINGTASEQUENCERAppMainWindow)
{
    ui->setupUi(this);
#if (MAJOR_TOOL_VERSION == 27 && RELEASE_TOOL_VERSION > 1) || (MAJOR_TOOL_VERSION > 27)
    _pServerThread = NULL;
    _pServer = NULL;
    if (NULL != _pServerThread)
    {
        _pServerThread->exit();
        while (_pServerThread->isRunning()) {}
        delete _pServerThread;
        _pServerThread =NULL;
    }
    if(NULL != _pServer)
    {
        delete _pServer;
        _pServer = NULL;
    }
#endif
    _killScriptsList.clear();
    _killToolsPIDList.clear();

    connect(ui->addInitTB,SIGNAL(clicked()),this,SLOT(onAddInitTBClicked()));
    connect(ui->removeInitTB,SIGNAL(clicked()),this,SLOT(onRemoveInitTBClicked()));

    connect(ui->addToolTB,SIGNAL(clicked()),this,SLOT(onAddToolTBClicked()));
    connect(ui->removeToolsTB,SIGNAL(clicked()),this,SLOT(onRemoveToolsTBClicked()));
    connect(ui->serverStatusRB,SIGNAL(toggled(bool)),this,SLOT(onServerStatusRBClicked(bool)));


    //Menu Bar- Sequencer
    connect(ui->actionClose,SIGNAL(triggered()),this,SLOT(close()));
    connect(ui->actionConfigurations_Manger,SIGNAL(triggered()),this,SLOT(onConfigurationsManagerSelected()));
    connect(ui->actionSave_Configuration,SIGNAL(triggered()),this,SLOT(onSaveConfigurations()));
    connect(ui->actionClose_Conifguration,SIGNAL(triggered()),this,SLOT(onCloseConfiguration()));
    _pconfigdb = new AINGTASEQUENCERConfigDbManager();
    QStringList strlist = _pconfigdb->getConfigNames();
    if (!strlist.isEmpty())
    {
        ui->menuLoad_Configuration->setEnabled(true);
        foreach(QString str,strlist)
        {
            QAction *action = new QAction(str,ui->menuLoad_Configuration);
            connect(action,SIGNAL(triggered()),this,SLOT(onRecentConfigurationSelected()));
            _recentConfigs.append(action);
            updateRecentMenu(str);
        }
    }
    else
    {
        ui->menuLoad_Configuration->setEnabled(false);
    }
    QSize si;
    si.setHeight(30);
    si.setWidth(30);

    ui->addInitTB->setIconSize(si);
    ui->addToolTB->setIconSize(si);
    ui->removeInitTB->setIconSize(si);
    ui->removeToolsTB->setIconSize(si);

    si.setHeight(60);
    si.setWidth(60);
    ui->serverStatusRB->setIconSize(si);

    ui->addInitTB->setAutoRaise(true);
    ui->addToolTB->setAutoRaise(true);
    ui->removeInitTB->setAutoRaise(true);
    ui->removeToolsTB->setAutoRaise(true);

    ui->serverStatusLB->setText("Server is not connected");
    ui->serverStatusLB->setStyleSheet("QLabel {color: #c0392b}");
    ui->serverStatusRB->setToolTip("Start Server");

    ui->serverStatusRB->setStyleSheet("QRadioButton {    spacing: 5px;}QRadioButton::indicator {    width: 60px;    height: 60px;}QRadioButton::indicator:unchecked {    image: url(:images/forms/img/serverOff.png);}QRadioButton::indicator:unchecked:hover {    image: url(:images/forms/img/serverOff.png);}QRadioButton::indicator:unchecked:pressed {    image: url(:images/forms/img/serverOff.png);}QRadioButton::indicator:checked {    image: url(:images/forms/img/serverOn.png);}QRadioButton::indicator:checked:pressed {    image: url(:images/forms/img/serverOn.png);}QRadioButton::indicator:indeterminate:pressed {    image: url(:images/forms/img/checkbox_indeterminate_pressed.png);}");

    AINGTAServer server("127.0.0.1",8082);
    QStringList availableIPs = server.getAvailableAddress();
    ui->connectedIPCB->insertItems(0,availableIPs);
    _lastConfig.clear();
    ui->loadedConfigurationLB->clear();
    ui->loadedConfigurationLB->setVisible(false);
    ui->progressBar->setVisible(false);
    ui->progressBar->hide();
    return;

}

AINGTASEQUENCERAppMainWindow::~AINGTASEQUENCERAppMainWindow()
{
#if (MAJOR_TOOL_VERSION == 27 && RELEASE_TOOL_VERSION > 1) || (MAJOR_TOOL_VERSION > 27)
    if (NULL != _pServerThread)
    {
        _pServerThread->exit();
        while (_pServerThread->isRunning()) {}
        delete _pServerThread;
        _pServerThread =NULL;
    }
    if(NULL != _pServer)
    {
        delete _pServer;
        _pServer = NULL;
    }
#endif
    delete ui;
}

void AINGTASEQUENCERAppMainWindow::close()
{
    this->QMainWindow::close();
}

void AINGTASEQUENCERAppMainWindow::onAddToolTBClicked()
{
    QStringList files = QFileDialog::getOpenFileNames(
            this,
            "Select one or more tools to add",
            "/home",
            "Tools (*.exe)");
    setTools(files);
}

void AINGTASEQUENCERAppMainWindow::onRemoveToolsTBClicked()
{
    //    qDeleteAll(ui->ToolLW->selectedIndexes());

    if(ui->ToolLW->count())
    {
        QList<QListWidgetItem*> list = ui->ToolLW->selectedItems();
        for(int i = 0; i < list.count(); i++)
        {
            QListWidgetItem* selectedItem = list.at(i);
            if(selectedItem)
            {
                delete ui->ToolLW->takeItem(ui->ToolLW->row(selectedItem));
            }
        }
    }
}

void AINGTASEQUENCERAppMainWindow::onAddInitTBClicked()
{
    QStringList files = QFileDialog::getOpenFileNames(
            this,
            "Select one or more files to add",
            "/home",
            "Initialization Scripts (*.py *.bat)");
    setInitialisationScripts(files);
}

void AINGTASEQUENCERAppMainWindow::onRemoveInitTBClicked()
{

    if(ui->InitLW->count())
    {
        QList<QListWidgetItem*> list = ui->InitLW->selectedItems();
        for(int i = 0; i < list.count(); i++)
        {
            QListWidgetItem* selectedItem = list.at(i);
            if(selectedItem)
            {
                delete ui->InitLW->takeItem(ui->InitLW->row(selectedItem));
            }
        }
    }
}

void AINGTASEQUENCERAppMainWindow::onServerStatusRBClicked(bool iVal)
{
    if(iVal)
    {
#if (MAJOR_TOOL_VERSION == 27 && RELEASE_TOOL_VERSION > 1) || (MAJOR_TOOL_VERSION > 27)
        AINGTASEQUENCERServerClientComm::_serverIP = ui->connectedIPCB->currentText();
        AINGTASEQUENCERServerClientComm::_availableIPs = ui->connectedIPCB->count();
        if (NULL != _pServerThread)
        {
            _pServerThread->exit(0);
            while (_pServerThread->isRunning());
            delete _pServerThread;
            _pServerThread =NULL;
        }
        //to run the server in a seperate thread
        _pServerThread = new QThread(this);

        _pServer = new AINGTASEQUENCERServerClientComm();
        connect(_pServerThread,SIGNAL(started()),_pServer,SLOT(checkIfServerIsAlreadyRunning()),Qt::QueuedConnection);


      //  connect(_pServerThread,SIGNAL(started()),_pServer,SLOT(initializeServer()),Qt::QueuedConnection);

        //connect(_pServer,SIGNAL(getIP(QString &)), this, SLOT(getserverIP(QString &)),Qt::DirectConnection);
        connect(_pServer,SIGNAL(addtools(const QStringList &)), this, SLOT(setTools(const QStringList &)),Qt::DirectConnection);
        connect(_pServer,SIGNAL(addscripts(const QStringList &)), this, SLOT(setInitialisationScripts(const QStringList  &) ),Qt::DirectConnection);
        connect(_pServer,SIGNAL(gettools()),this, SLOT(getTools()),Qt::DirectConnection);
        connect(_pServer,SIGNAL(getscripts()),this, SLOT(getScripts()),Qt::DirectConnection);
        connect(_pServer,SIGNAL(startscripts()),this, SLOT(startScripts()),Qt::DirectConnection);
        connect(_pServer,SIGNAL(starttools()),this, SLOT(startTools()),Qt::DirectConnection);
        connect(_pServer,SIGNAL(stopscripts()),this, SLOT(stopScripts()),Qt::DirectConnection);
        connect(_pServer,SIGNAL(stoptools()),this, SLOT(stopTools()),Qt::DirectConnection);
        connect(_pServer,SIGNAL(removescripts(QStringList)),this,SLOT(removeScripts(QStringList)),Qt::DirectConnection );
        connect(_pServer,SIGNAL(removetools(QStringList)),this, SLOT(removeTools(QStringList)),Qt::DirectConnection);
        connect(_pServer,SIGNAL(checktoolsarerunning()),this,SLOT(checkToolsAreRunning()),Qt::DirectConnection);
        connect(_pServer,SIGNAL(serverStarted(bool)),this,SLOT(serverIsRunning(bool)));

        ui->progressBar->setMinimum(0);
        ui->progressBar->setMaximum(0);
        ui->progressBar->setVisible(true);
        ui->progressBar->show();
        this->setEnabled(false);
        ui->serverStatusLB->setText("Server is initializing...");
        connect(_pServer,SIGNAL(multipleInstances(bool)),this ,SLOT(multipleInstancesRunning(bool)));
        _pServer->moveToThread(_pServerThread);
        _pServerThread->start(QThread::TimeCriticalPriority);
#endif

#if MAJOR_TOOL_VERSION > 26 && RELEASE_TOOL_VERSION < 2
        serverIsRunning(true);

#endif

    }

    else
    {
        ui->progressBar->setVisible(false);
        ui->progressBar->hide();
        ui->serverStatusLB->setText("Server is not connected");
        ui->serverStatusLB->setStyleSheet("QLabel {color: #c0392b}");
        ui->serverStatusRB->setToolTip("Start Server");
        if(!_recentConfigs.isEmpty())
            ui->menuLoad_Configuration->setEnabled(true);
        ui->actionClose_Conifguration->setEnabled(true);
        ui->actionSave_Configuration->setEnabled(true);
        ui->addInitTB->setEnabled(true);
        ui->addToolTB->setEnabled(true);
        ui->removeInitTB->setEnabled(true);
        ui->removeToolsTB->setEnabled(true);
        ui->connectedIPCB->setEnabled(true);
        ui->actionConfigurations_Manger->setEnabled(true);
        ui->InitLW->setAcceptDrops(true);
        ui->ToolLW->setAcceptDrops(true);

#if (MAJOR_TOOL_VERSION == 27 && RELEASE_TOOL_VERSION > 1) || (MAJOR_TOOL_VERSION > 27)
        if (NULL != _pServerThread)
        {
            _pServerThread->exit();
            while (_pServerThread->isRunning()) {}
            delete _pServerThread;
            _pServerThread =NULL;
        }
        if(NULL != _pServer)
        {
            delete _pServer;
            _pServer = NULL;
        }
#endif
    }
}

void AINGTASEQUENCERAppMainWindow::multipleInstancesRunning(bool iVal)
{
    if(iVal)
    {
        this->setEnabled(true);
        ui->progressBar->setVisible(false);
        ui->progressBar->hide();
        QMessageBox::warning(nullptr,"Error","Server could not be started, as server is already running in another instance",QMessageBox::Ok);
        close();
    }
}


void AINGTASEQUENCERAppMainWindow::serverIsRunning(bool status)
{
    if(status)
    {
        this->setEnabled(true);
        ui->progressBar->setVisible(false);
        ui->progressBar->hide();
        ui->serverStatusLB->setText("Server is running!!!");
        ui->serverStatusLB->setStyleSheet("QLabel {color: #27ae60}");
        ui->serverStatusRB->setToolTip("Stop Server");
        ui->menuLoad_Configuration->setEnabled(false);
        ui->actionClose_Conifguration->setEnabled(false);
        ui->actionSave_Configuration->setEnabled(false);
        ui->addInitTB->setEnabled(false);
        ui->addToolTB->setEnabled(false);
        ui->removeInitTB->setEnabled(false);
        ui->removeToolsTB->setEnabled(false);
        ui->connectedIPCB->setEnabled(false);
        ui->actionConfigurations_Manger->setEnabled(false);
        ui->InitLW->setAcceptDrops(false);
        ui->ToolLW->setAcceptDrops(false);
    }
    else
    {
#if (MAJOR_TOOL_VERSION == 27 && RELEASE_TOOL_VERSION > 1) || (MAJOR_TOOL_VERSION > 27)
        this->setEnabled(true);
        ui->progressBar->setVisible(false);
        ui->progressBar->hide();
        QMessageBox::warning(this,"Error","Server could not be started",QMessageBox::Ok);
        ui->serverStatusRB->toggle();
        if (NULL != _pServerThread)
        {
            _pServerThread->exit();
            while (_pServerThread->isRunning()) {}
            delete _pServerThread;
            _pServerThread =NULL;
        }
        if(NULL!=_pServer)
        {
            delete _pServer;
            _pServer = NULL;
        }

#endif
    }
}

bool AINGTASEQUENCERAppMainWindow::setInitialisationScripts(const QStringList &iScriptLst)
{
    bool rc = true;
    foreach(QString fileName,iScriptLst)
    {
        //setItemIcon(fileName);
		fileName = QDir::cleanPath(fileName);
        QFile file(fileName);
        if(file.exists())
            setItemIcon(fileName);
        else
            rc &= false;
    }
    return rc;
}

bool AINGTASEQUENCERAppMainWindow::setTools(const QStringList &iToolLst)
{
    bool rc = true;
    foreach(QString tool,iToolLst)
    {
		tool = QDir::cleanPath(tool);
        QFile file(tool);
        if(file.exists())
            setItemIcon(tool);
        else
            rc &= false;
    }
    return rc;
}

void AINGTASEQUENCERAppMainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls() && ui->InitLW->acceptDrops() && ui->ToolLW->acceptDrops())
    {
        event->acceptProposedAction();
    }
    else
    {
        event->ignore();
    }
}

void AINGTASEQUENCERAppMainWindow::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasUrls() && ui->InitLW->acceptDrops() && ui->ToolLW->acceptDrops())
    {
        event->acceptProposedAction();
    }
    else
    {
        event->ignore();
    }
}

void AINGTASEQUENCERAppMainWindow::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasUrls() && ui->InitLW->acceptDrops() && ui->ToolLW->acceptDrops() )
    {
        QList<QUrl> urls = event->mimeData()->urls();
        if (!urls.isEmpty())
        {
            QUrl url;
            foreach (url,urls)
            {
                QString str = url.toLocalFile();
                if(str.contains(".py")|| str.contains(".bat") ||str.contains(".exe"))
                {
                    setItemIcon(str);
                }
                else
                {
                    event->ignore();
                    continue;
                }
            }
        }
        event->acceptProposedAction();
    }
}

void AINGTASEQUENCERAppMainWindow::onConfigurationsManagerSelected()
{
    QStringList lstConfigs;
    for(int i=0;i<_recentConfigs.count();i++)
    {
        lstConfigs.append(_recentConfigs.at(i)->text());
    }
    AINGTASEQUENCERConfigurationManager *pConfigManager = new AINGTASEQUENCERConfigurationManager();
    pConfigManager->setWindowModality(Qt::ApplicationModal);
    if(pConfigManager)
    {
        pConfigManager->setAttribute(Qt::WA_DeleteOnClose);
        connect(pConfigManager,SIGNAL(widgetClosed()),this,SLOT(onWidgetClosed()));
        connect(pConfigManager,SIGNAL(deleteConfig(QStringList)),this,SLOT(onDeleteConfig(QStringList)));
        connect(pConfigManager,SIGNAL(saveConfiguration(QString)),this,SLOT(onAddNewConfiguration(QString)));
        pConfigManager->setConfigurations(lstConfigs);
        pConfigManager->show();
    }
}

void AINGTASEQUENCERAppMainWindow::onSaveConfigurations()
{

    if(_recentConfigs.isEmpty())
    {
        QMessageBox::warning(this,"Error","There are no saved configurations. Please create a new one in Configurations Manager to save the configuration",QMessageBox::Ok);
    }
    else if(_lastConfig.isNull())
        QMessageBox::warning(this,"Error","Please create a new configuration in Configuration manager or load a configuration to modify it",QMessageBox::Ok);
    else
    {
        QAction *pAction = _recentConfigs.last();
        QString configName = pAction->text();

        QStringList lstScripts;
        QStringList lstTools;
        QString IPConnected = ui->connectedIPCB->currentText();

        for(int row =0;row < ui->InitLW->count();row++)
        {
            lstScripts.append(ui->InitLW->item(row)->text());
        }
        for(int row =0;row < ui->ToolLW->count();row++)
        {
            lstTools.append(ui->ToolLW->item(row)->text());
        }
        if(lstScripts.isEmpty() && lstTools.isEmpty())
        {
            QMessageBox::warning(this,"Error","There are no added configurations to be saved",QMessageBox::Ok);
        }
        else
        {
            AINGTASEQUENCERConfigDbManager _pconfigdb;
            _pconfigdb.saveConfiguration(configName,lstScripts,lstTools,IPConnected);
        }
    }
}

void AINGTASEQUENCERAppMainWindow::onAddNewConfiguration(const QString &iConfigurationName)
{

    QStringList lstScripts;
    QStringList lstTools;
    QString IPConnected = ui->connectedIPCB->currentText();

    for(int row =0;row < ui->InitLW->count();row++)
    {
        lstScripts.append(ui->InitLW->item(row)->text());
    }
    for(int row =0;row < ui->ToolLW->count();row++)
    {
        lstTools.append(ui->ToolLW->item(row)->text());
    }
    if(!(lstScripts.isEmpty() && lstTools.isEmpty() && IPConnected.isNull()))
    {
        AINGTASEQUENCERConfigDbManager _pconfigdb;
        _pconfigdb.saveConfiguration(iConfigurationName,lstScripts,lstTools,IPConnected);
        addConfigToRecents(iConfigurationName);
        _lastConfig = iConfigurationName;
    }

}

void AINGTASEQUENCERAppMainWindow::addConfigToRecents(const QString &iConfigurationName)
{
    if(!iConfigurationName.isEmpty())
    {
        QAction *action = new QAction(iConfigurationName,ui->menuLoad_Configuration );
        action->setObjectName(iConfigurationName);
        connect(action,SIGNAL(triggered()),this,SLOT(onRecentConfigurationSelected()));
        _recentConfigs.append(action);
        updateRecentMenu(iConfigurationName);
    }

}

void AINGTASEQUENCERAppMainWindow::onRecentConfigurationSelected()
{
    QAction *pAction = dynamic_cast<QAction*>(sender());
    if(pAction)
    {
        QString ConfigurationName = pAction->text();
        onCloseConfiguration();
        loadConfiguration(ConfigurationName);
        if(_recentConfigs.count())
        {
            for(int i = 0; i < _recentConfigs.count(); i++)
            {
                QAction *pAction = _recentConfigs.at(i);
                if(pAction)
                {
                    if(ConfigurationName == pAction->text())
                    {
                        _recentConfigs.append(_recentConfigs.takeAt(i));
                        break;
                    }
                }
            }
        }
        updateRecentMenu(ConfigurationName);
        _lastConfig= ConfigurationName;
    }
}

void AINGTASEQUENCERAppMainWindow::updateRecentMenu(const QString &iCurrConfigName)
{

    //This entire for loop is only written to clear the MenuBar. Using ui->menuRecent_Sessions->clear()
    //deletes the actions and the _recentSessions then has dangling pointers hence this approach
    //Change the code with caution!!!
    QObjectList objList = ui->menuLoad_Configuration->children();
    if (!objList.isEmpty())
    {
        ui->menuLoad_Configuration->setEnabled(true);
    }

    for(int j = 0; j < objList.count(); j++)
    {
        QObject *pObj = objList.at(j);
        if(pObj /*&& pObj->objectName() != "Default"*/)
        {
            QAction *pAction = dynamic_cast<QAction*>(pObj);
            if(pAction)
            {
                ui->menuLoad_Configuration->removeAction(pAction);

            }
        }
    }

    //This for loop actually adds actions to the menu bar
    if (_recentConfigs.count()-1 > -1)
    {
        for(int i = _recentConfigs.count()-1, count = 0; (i >= 0); i--,count++)
        {
            QAction *pAction = _recentConfigs.at(i);
            ui->menuLoad_Configuration->addAction(pAction);
        }
    }
    else
    {
        ui->menuLoad_Configuration->setDisabled(true);
    }
}

void AINGTASEQUENCERAppMainWindow::onDeleteConfig(const QStringList &iSelectedItems)
{
    if(!iSelectedItems.isEmpty())
    {
        AINGTASEQUENCERConfigDbManager _pconfigdb;
        _pconfigdb.deleteConfigs(iSelectedItems);
        removeConfigFromRecents(iSelectedItems);
        foreach(QString str,iSelectedItems)
        {
            if(_lastConfig.compare(str) == 0)
                onCloseConfiguration();
        }
        _lastConfig.clear();
    }
}

void AINGTASEQUENCERAppMainWindow::removeConfigFromRecents(const QStringList &iConfigLst)
{
    if(!iConfigLst.isEmpty())
    {
        bool isFound = false;
        foreach(QString configName,iConfigLst)
        {
            QMutableListIterator <QAction*> i(_recentConfigs);
            while(i.hasNext())
            {
                QAction *pAction = i.next();
                if(configName == pAction->text())
                {
                    isFound |= true;
                    i.remove();
                    break;
                }
            }
        }
        if(isFound)
            updateRecentMenu();
    }
}
void AINGTASEQUENCERAppMainWindow::loadConfiguration(const QString &iConfigurationName)
{
    AINGTASEQUENCERConfigDbManager _pconfigdb;
    QStringList configlist = _pconfigdb.getConfigFiles(iConfigurationName);

    if(!configlist.isEmpty())
    {
        QString string;
        QStringList lstfiles;
        QString lstfile = configlist.at(0);

        if(!lstfile.isEmpty())
        {
            lstfiles = lstfile.split(",");
            for(int i=0; i<lstfiles.count() ; i++)
            {
                string = lstfiles.at(i);
                setItemIcon(string);
            }
        }

        lstfile = configlist.at(1);
        if(!lstfile.isEmpty())
        {
            lstfiles = lstfile.split(",");
            for(int i=0; i<lstfiles.count() ; i++)
            {
                string = lstfiles.at(i);
                setItemIcon(string);
            }
        }

        string = configlist.at(2);
        if(!string.isEmpty())
        {
            for(int i=0;i<ui->connectedIPCB->count();i++)
            {
                if(string.compare(ui->connectedIPCB->itemText(i))== 0)
                {
                    ui->connectedIPCB->setCurrentIndex(i);
                    break;
                }
            }
        }
         ui->loadedConfigurationLB->clear();
         ui->loadedConfigurationLB->setVisible(true);
         ui->loadedConfigurationLB->setText(QString(" Last Loaded Configuration : \"%1\" ").arg(iConfigurationName));
    }

}

void AINGTASEQUENCERAppMainWindow::onCloseConfiguration()
{
    ui->connectedIPCB->setCurrentIndex(0);
    ui->InitLW->clear();
    ui->ToolLW->clear();
    _lastConfig.clear();
    ui->loadedConfigurationLB->clear();
    ui->loadedConfigurationLB->setVisible(false);
}

void AINGTASEQUENCERAppMainWindow::onWidgetClosed()
{
    AINGTASEQUENCERConfigurationManager *pWdgt = dynamic_cast<AINGTASEQUENCERConfigurationManager *>(sender());
    if(pWdgt != NULL)
    {
        delete pWdgt;
        pWdgt = NULL;
    }
}

void AINGTASEQUENCERAppMainWindow::setItemIcon( const QString &str)
{
    QListWidgetItem *pItem;
    if(str.contains(".py"))
    {
        pItem = new QListWidgetItem(ui->InitLW);
        pItem->setIcon(QIcon(":images/forms/img/PythonFile.png"));
    }
    else if( str.contains(".bat"))
    {
        pItem = new QListWidgetItem(ui->InitLW);
        pItem->setIcon(QIcon(":images/forms/img/BatchFile.png"));
    }
    else if(str.contains(".exe"))
    {
        pItem = new QListWidgetItem(ui->ToolLW);
        pItem->setIcon(QIcon(":images/forms/img/ExeFile.png"));
    }
    pItem->setText(str);
}

#if (MAJOR_TOOL_VERSION == 27 && RELEASE_TOOL_VERSION > 1) || (MAJOR_TOOL_VERSION > 27)
QVariant AINGTASEQUENCERAppMainWindow::startScripts()
{
    QString scriptLog;
    QStringList initScriptList;
    for(int i =0;i<ui->InitLW->count() ; i++)
        initScriptList.append(ui->InitLW->item(i)->text());
    foreach(QString file,initScriptList)
    {
        QFile File(file);
        if(File.exists())
        {

            QStringList arguments;
            QProcess *process = new QProcess();
            QString cmd;
            QDir::cleanPath(file);

            if(file.endsWith(".py"))
            {

                QStringList pathLst = file.split("/");
                QString fileName = pathLst.takeLast();
                QString workingDir = pathLst.join("/");

                cmd = QString("python");
                arguments << fileName;
                qint64 pid;
                process->startDetached(cmd,arguments,workingDir,&pid);
                QString killStmt = QString("taskkill /F /PID %1").arg(QString::number(pid));
                _killScriptsList.append(killStmt);

            }
            else if(file.endsWith(".bat"))
            {
                //cmd = file;
                // process->start(cmd,arguments);
                QStringList pathLst = file.split("/");
                pathLst.removeLast();
                QString workingDir = pathLst.join("/");
                qint64 pid;
                process->startDetached(file,arguments,workingDir,&pid);
                QString killStmt = QString("taskkill /F /T /PID %1").arg(QString::number(pid));
                _killScriptsList.append(killStmt);
            }
            scriptLog.append(QString("Starting script %1 \n").arg(file));
            //emit log(QString("Starting script %1").arg(file));
        }
        else
        {
            scriptLog.append(QString("Could not start the script as the path (%1) was not found.\n").arg(file));
            //  emit log(QString("Could not start the script as the path (%1) was not found.").arg(file));
        }
    }
    return scriptLog;
}

QVariantList AINGTASEQUENCERAppMainWindow::getTools()
{    
    QVariantList filesList;
    for(int i=0;i<ui->ToolLW->count();i++)
        filesList.append(ui->ToolLW->item(i)->text());
    return filesList;

}

QVariantList AINGTASEQUENCERAppMainWindow::getScripts()
{
    QVariantList filesList;
    for(int i=0;i<ui->InitLW->count();i++)
        filesList.append(ui->InitLW->item(i)->text());
    return filesList;

}

QVariant AINGTASEQUENCERAppMainWindow::startTools()
{
    QString toolLog;
    QStringList toolList;
    for(int i =0;i<ui->ToolLW->count() ; i++)
        toolList.append(ui->ToolLW->item(i)->text());
    foreach(QString file,toolList)
    {

        QFile File(file);
        if(File.exists())
        {

            QProcess *process = new QProcess();

            QStringList pathLst = file.split("/");
            pathLst.removeLast();
            QString workingDir = pathLst.join("/");

            qint64 pid;
            QStringList arguments;
            process->startDetached(file,arguments,workingDir,&pid);
            // QString killStmt = QString("taskkill /F /PID %1").arg(QString::number(pid));
            // _killToolsPIDList.append(killStmt);
            _killToolsPIDList.append(QString::number(pid));
            toolLog.append(QString("Starting tool %1 \n").arg(file));
        }
        else
        {
            toolLog.append(QString("Could not start the tool as the path (%1) was not found \n").arg(file));
        }
    }
	return toolLog;
}

void AINGTASEQUENCERAppMainWindow::stopScripts()
{
    foreach(QString killCmd, _killScriptsList)
    {
        QProcess::execute(killCmd);
    }
    //removing all tools from list
    _killScriptsList.clear();
}


void AINGTASEQUENCERAppMainWindow::stopTools()
{
    foreach(QString PID, _killToolsPIDList)
    {
        /*QByteArray ba = killCmd.toLocal8Bit();
        const char *c_str2 = ba.data();
        int ret = system(c_str2); */
        QProcess::execute(QString("taskkill /F /PID %1").arg(PID));

    }
    //removing all tools from list
    _killToolsPIDList.clear();
}

void AINGTASEQUENCERAppMainWindow::removeScripts(const QStringList &iScriptLst)
{
    if(ui->InitLW->count())
    {
        foreach(QString fileName, iScriptLst)
        {
            for(int i=0;i<ui->InitLW->count() ; i++)
            {
                if(fileName.compare(ui->InitLW->item(i)->text()) == 0)
                {
                    delete ui->InitLW->takeItem(i);
                    i--;
                }
            }
        }
    }
}
void AINGTASEQUENCERAppMainWindow::removeTools(const QStringList &iToolLst)
{
    if(ui->ToolLW->count())
    {
        foreach(QString fileName, iToolLst)
        {
            for(int i=0;i<ui->ToolLW->count() ; i++)
            {
                if(fileName.compare(ui->ToolLW->item(i)->text()) == 0)
                {
                    delete ui->ToolLW->takeItem(i);
                    i--;
                }
            }
        }
    }
}

bool AINGTASEQUENCERAppMainWindow::checkToolsAreRunning()
{
    bool found = false;
    foreach(QString PID, _killToolsPIDList)
    {
        QProcess tasklist;
        tasklist.start(
                "tasklist",
                QStringList() << "/NH"
                << "/FO" << "CSV"
                << "/FI" << QString("PID eq %1").arg(PID));
        tasklist.waitForFinished();
        QString output = tasklist.readAllStandardOutput();
        if(output.contains(QString("\"%1\"").arg(PID)))
            found |= true;
    }
    return found;

}
#endif
