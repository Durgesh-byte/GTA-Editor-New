#include "GTASequencerSettings.h"

#pragma warning(push, 0)
#include "ui_GTASequencerSettings.h"
#include <QListWidgetItem>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QtXml\QDomDocument>
#include <QTextStream>
#include <QDoubleValidator>
#include <QHostAddress>
#include <QNetworkReply>
#include <QSortFilterProxyModel>
#include <QProgressBar>
#include <QMessageBox>
#pragma warning(pop)

#define TRUE "True"
#define FALSE "False"
#define COL_COUNT 3

GTAClient * GTASequencerSettings::_client = NULL;
bool GTASequencerSettings::_IsServerConnected = false;
QList<GTAClient*> GTASequencerSettings::_clientLists = QList<GTAClient*>();
QList<GTAClient*> GTASequencerSettings::_ConnectedclientLists = QList<GTAClient*>();


GTASequencerSettings::GTASequencerSettings(const QString &iToolDataPath, const QString &iLogFilePath, const QString &iScxmlPath, QWidget *parent) :
        QDialog(parent),
        ui(new Ui::GTASequencerSettings)
{
    ui->setupUi(this);

    //    _logFilePath = iLogFilePath;
    //    _scxmlFilePath = iScxmlPath;

    setPaths(iLogFilePath,iScxmlPath,iToolDataPath);
    connect(ui->AddInitTB,SIGNAL(clicked()),this,SLOT(onAddInitTBClicked()));
    connect(ui->RemoveInitTB,SIGNAL(clicked()),this,SLOT(onRemoveInitTBClicked()));

    connect(ui->AddToolsTB,SIGNAL(clicked()),this,SLOT(onAddToolTBClicked()));
    connect(ui->RemoveToolTB,SIGNAL(clicked()),this,SLOT(onRemoveToolsTBClicked()));

    connect(ui->ApplyPB,SIGNAL(clicked()),this,SLOT(onApplyClicked()));
    connect(ui->ClearPB,SIGNAL(clicked()),this,SLOT(onClearClicked()));

    connect(ui->SearchSchedulerTB,SIGNAL(clicked()),this,SLOT(onSchedulerPBClicked()));
    connect(ui->UserSettingsLW,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(onUserSettingSelected(QModelIndex)));

    connect(ui->ChkSaveUserSetting,SIGNAL(toggled(bool)),this,SLOT(onSaveUserSettingToggled(bool)));
    ui->ChkSaveUserSetting->setChecked(false);

    onSaveUserSettingToggled(false);

    connect(ui->SaveTB,SIGNAL(clicked()),this,SLOT(onSaveUserSettingClicked()));

    //    connect(ui->ChkInitialization,SIGNAL(toggled(bool)),this,SLOT(onInitializationCheckClicked(bool)));
    ui->ChkInitialization->setChecked(true);

    connect(ui->RemoveSettingTB, SIGNAL(clicked()),this,SLOT(onRemoveSettingTBClicked()));

    QDoubleValidator *validator = new QDoubleValidator;
    validator->setRange(0,999,2);

    ui->WaitTimeLE->setValidator(validator);
    ui->TimeoutLE->setValidator(validator);

    readSettingsFromPath();
    connect(ui->addSlaveTB,SIGNAL(clicked()),this,SLOT(onAddSlaveTBClicked()));
    connect(ui->removeSlaveTB,SIGNAL(clicked()),this,SLOT(onRemoveSlaveTBClicked()));
    _rowCount = 0;
    ui->SeqSlavesListTW->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    ui->SeqSlavesListTW->horizontalHeader()->setStretchLastSection(true);
    ui->SeqSlavesListTW->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->SeqSlavesListTW->setSelectionBehavior(QAbstractItemView::SelectRows);
    _ports<<14934<<29890<<33658<<42508<<38798;
    _serverConnectionFailure.clear();
    _IsServerConnected=false;
    _progressDialog = nullptr;

    ui->ClearPB->setAutoDefault(false);
    ui->ApplyPB->setAutoDefault(false);

    // Bellow is incorrect, change it. clear the list while destructing
    qDeleteAll(_ConnectedclientLists.begin(),_ConnectedclientLists.end());
    _ConnectedclientLists.clear();
}

GTASequencerSettings::~GTASequencerSettings()
{

    if(_progressDialog != nullptr)
    {
        delete _progressDialog;
        _progressDialog = nullptr;
    }

    foreach(GTAClient * client, _ConnectedclientLists)
    {
        QVariantList args;
        const char *slot = SLOT(onDeleteServers(QVariant &, QNetworkReply *));
        const char *faultSlot = SLOT(onDeleteServersFailed(int, const QString &, QNetworkReply *));

        client->sendRequest(args,"disconnectToServer",this,slot,this,faultSlot);
    }
    qDeleteAll(_clientLists.begin(),_clientLists.end());
    _clientLists.clear();
    delete ui;

}

void GTASequencerSettings::setInitialisationScripts(const QStringList &iScriptLst)
{

    foreach(QString fileName,iScriptLst)
    {
        QListWidgetItem *pItem = new QListWidgetItem(ui->InitLW);
        pItem->setText(fileName);
    }
}

void GTASequencerSettings::setTools(const QStringList &iToolLst)
{
    //
    foreach(QString tool,iToolLst)
    {
        QListWidgetItem *pItem = new QListWidgetItem(ui->ToolLW);
        pItem->setText(tool);
    }
}


void GTASequencerSettings::onAddInitTBClicked()
{
    QStringList files = QFileDialog::getOpenFileNames(
            this,
            "Select one or more files to add",
            "/home",
            "Initialization Scripts (*.py *.bat)");
    setInitialisationScripts(files);
}

void GTASequencerSettings::onRemoveInitTBClicked()
{
    //    qDeleteAll(ui->InitLW->selectedIndexes());
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

void GTASequencerSettings::onAddToolTBClicked()
{
    QStringList files = QFileDialog::getOpenFileNames(
            this,
            "Select one or more tools to add",
            "/home",
            "Tools (*.exe)");
    setTools(files);
}
void GTASequencerSettings::onRemoveToolsTBClicked()
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

void GTASequencerSettings::onRemoveSettingTBClicked()
{
    if(ui->UserSettingsLW->count())
    {
        QList<QListWidgetItem*> list = ui->UserSettingsLW->selectedItems();
        for(int i = 0; i < list.count(); i++)
        {
            QListWidgetItem* selectedItem = list.at(i);
            if(selectedItem)
            {
                QString settingName = selectedItem->text();
                QString settingsDirPath = QDir::cleanPath(QString("%1%2%3").arg(_ToolDataPath,QDir::separator(),SEQUENCER_SETTINGS_DIR));
                QString settingFilePath = settingsDirPath.append(QString("%1%2.xml").arg(QDir::separator(),settingName));
                settingFilePath = QDir::cleanPath(settingFilePath);
                QFile file(settingFilePath);
                if(file.exists())
                {
                    file.remove();
                    delete ui->UserSettingsLW->takeItem(ui->UserSettingsLW->row(selectedItem));
                }
            }
        }
    }
}


void GTASequencerSettings::onUserSettingSelected(const QModelIndex &index)
{
    if(index.isValid() && ui->UserSettingsLW->count() > 0)
    {
        int row = index.row();
        QListWidgetItem *pItem = ui->UserSettingsLW->item(row);
        if(pItem)
        {
            QString settingName = pItem->text();
            QString settingsDirPath = QDir::cleanPath(QString("%1%2%3").arg(_ToolDataPath,QDir::separator(),SEQUENCER_SETTINGS_DIR));
            QString settingFilePath = settingsDirPath.append(QString("%1%2.xml").arg(QDir::separator(),settingName));
            settingFilePath = QDir::cleanPath(settingFilePath);
            QFile file(settingFilePath);
            if(file.exists())
            {
                if(file.open(QIODevice::ReadOnly | QFile::Text))
                {
                    QDomDocument *pDomDoc = new QDomDocument();
                    if(pDomDoc != NULL)
                    {
                        QString errMsg;
                        int errLine;
                        int errCol;
                        if(pDomDoc->setContent(&file,false,&errMsg,&errLine,&errCol))
                        {
                            QDomElement rootElement = pDomDoc->documentElement();
                            QString rootName = rootElement.nodeName();

                            if(rootName == "Setting" && rootElement.hasChildNodes())
                            {

                                QDomNamedNodeMap attributes = rootElement.attributes();

                                QString initWaitTime = attributes.namedItem("InitWaitTime").nodeValue();
                                ui->WaitTimeLE->setText(initWaitTime);

                                QString timeOut = attributes.namedItem("Timeout").nodeValue();
                                ui->TimeoutLE->setText(timeOut);

                                QString timeOutUnit = attributes.namedItem("TimeoutUnit").nodeValue();

                                int idx = ui->TimeoutCB->findText(timeOutUnit);
                                if(idx >= 0 && idx < ui->TimeoutCB->count())
                                {
                                    ui->TimeoutCB->setCurrentIndex(idx);
                                }

                                idx = -1;
                                QString initWaitTimeUnit = attributes.namedItem("InitWaitTimeUnit").nodeValue();
                                idx = ui->WaitTimeCB->findText(initWaitTimeUnit);
                                if(idx >= 0 && idx < ui->WaitTimeCB->count())
                                {
                                    ui->WaitTimeCB->setCurrentIndex(idx);
                                }



                                //                                QString logFilePath = attributes.namedItem("LogFilePath").nodeValue();
                                //                                ui->LogFilePathLE->setText(logFilePath);
                                //                                _logFilePath = logFilePath;

                                //                                QString scxmlFilePath = attributes.namedItem("ScxmlFilePath").nodeValue();
                                //                                ui->scxmlPathLE->setText(scxmlFilePath);
                                //                                _scxmlFilePath = scxmlFilePath;

                                QString schedulerPath = attributes.namedItem("SchedulerPath").nodeValue();
                                ui->SchedulerPathLE->setText(schedulerPath);



                                _schedulerPath = schedulerPath;

                                QString subSettingName = attributes.namedItem("Name").nodeValue();
                                ui->UserSettingNameLE->setText(subSettingName);

                                QString sInitCheck = attributes.namedItem("InitCheck").nodeValue();
                                bool initCheck = (sInitCheck == TRUE)? true : false;
                                ui->ChkInitialization->setChecked(initCheck);

                                QStringList scripts;
                                QStringList tools;
                                QDomNodeList childNodes = rootElement.childNodes();
                                QDomNode scriptNode;
                                QDomNode toolNode;
                                QDomNode SlaveNode;
                                int j;
                                int count = childNodes.count();

                                if(count >= 2)
                                {
                                    scriptNode = childNodes.at(0);
                                    toolNode = childNodes.at(1);
                                    if(count == 3)
                                        SlaveNode = childNodes.at(2);
                                }
                                QDomNodeList scriptList = scriptNode.childNodes();
                                for(j = 0; j < scriptList.count(); j++)
                                {
                                    QString script = scriptList.at(j).attributes().namedItem("Path").nodeValue();
                                    scripts << script;
                                }
                                ui->InitLW->clear();
                                setInitialisationScripts(scripts);

                                QDomNodeList toolList = toolNode.childNodes();
                                for(j = 0; j < toolList.count(); j++)
                                {
                                    QString tool = toolList.at(j).attributes().namedItem("Path").nodeValue();
                                    tools << tool;
                                }
                                ui->ToolLW->clear();
                                setTools(tools);
                                // For updating sequencer slaves list on UI, removing the existing slaves list
                                QDomNodeList slavesList = SlaveNode.childNodes();
                                for(int i = _rowCount-1 ; i>=0 ;i--)
                                {
                                    ui->SeqSlavesListTW->removeRow(i);
                                    _rowCount--;
                                }
                                for(j = 0; j < slavesList.count(); j++)
                                {
                                    QString IP = slavesList.at(j).attributes().namedItem("IP").nodeValue();
                                    QString Name = slavesList.at(j).attributes().namedItem("AliasName").nodeValue();
                                    setSequencerSlaves(IP,Name);
                                }
                                qDeleteAll(_clientLists.begin(),_clientLists.end());
                                _clientLists.clear();
                                foreach(GTAClient * client, _ConnectedclientLists)
                                {
                                    QVariantList args;
                                    const char *slot = SLOT(onDeleteServers(QVariant &, QNetworkReply *));
                                    const char *faultSlot = SLOT(onDeleteServersFailed(int, const QString &, QNetworkReply *));

                                    client->sendRequest(args,"disconnectToServer",this,slot,this,faultSlot);
                                }
                            }
                        }
                    }
                }
                else
                {
                    QMessageBox::warning(this,"Error","Setting does not exist",QMessageBox::Ok);

                }
                file.close();

            }
        }
    }
}

void GTASequencerSettings::onSaveUserSettingToggled(bool iVal)
{
    ui->UserSettingNameLE->clear();
    ui->UserSettingNameLE->setVisible(iVal);
    ui->SaveTB->setVisible(iVal);
}

void GTASequencerSettings::onInitializationCheckClicked(bool )
{
    //    ui->InitWaitTimeLbl->setEnabled(iVal);
    //    ui->WaitTimeLE->setEnabled(iVal);
    //    ui->WaitTimeCB->setEnabled(iVal);
}


void GTASequencerSettings::onSaveUserSettingClicked()
{

    QString settingName = ui->UserSettingNameLE->text();
    if(!settingName.isEmpty())
    {
        QString settingsDirPath = QDir::cleanPath(QString("%1%2%3").arg(_ToolDataPath,QDir::separator(),SEQUENCER_SETTINGS_DIR));

        QString settingFilePath = settingsDirPath;
        settingFilePath.append(QDir::cleanPath(QString("%1%2.xml").arg(QDir::separator(),settingName)));

        QDir dir(settingsDirPath);
        if(!dir.exists())
        {
            dir.mkpath(settingsDirPath) ;
        }

        QStringList filters;
        filters << "*.xml";
        QStringList files = dir.entryList(filters);
        if(files.contains(settingName + ".xml"))
        {
            QMessageBox::warning(this,"Error","User Setting with the given name already exists. \nPlease save with another name",QMessageBox::Ok);
        }
        else
        {
            bool ret = writeSetting(settingName,settingFilePath);
            if(ret)
            {
                QMessageBox::information(this,"Success","User setting saved successfully",QMessageBox::Ok);
                QListWidgetItem *pItem = new QListWidgetItem(ui->UserSettingsLW);
                pItem->setText(settingName);
            }
            else
            {
                QMessageBox::warning(this,"Error","Error while saving user setting",QMessageBox::Ok);
            }

        }
    }
    else
    {
        QMessageBox::warning(this,"Error","Setting name cannot be empty.",QMessageBox::Ok);
    }
}

void GTASequencerSettings::readSettingsFromPath()
{
    QString settingsDirPath = QDir::cleanPath(QString("%1%2%3").arg(_ToolDataPath,QDir::separator(),SEQUENCER_SETTINGS_DIR));

    QDir dir(settingsDirPath);
    if(!dir.exists())
    {
        dir.mkpath(settingsDirPath) ;
    }
    QStringList filters;
    filters << "*.xml";
    QStringList files = dir.entryList(filters);

    foreach(QString file,files)
    {
        file.replace(settingsDirPath,"");
        file.replace(QDir::separator(),"");
        file = file.mid(0,file.lastIndexOf(".")).trimmed();
        QListWidgetItem *pItem = new QListWidgetItem(ui->UserSettingsLW);
        pItem->setText(file);
    }

}


bool GTASequencerSettings::writeSetting(const QString &iSettingName, const QString &iFilePath)
{
    bool rc = false;
    QFile settingFile(iFilePath);
    if(settingFile.open(QIODevice::WriteOnly))
    {
        QStringList initList = getInitList();
        QStringList toolList = getToolList();
        QString initWaitTime = ui->WaitTimeLE->text();
        QString timeOut = ui->TimeoutLE->text();
        //        QString logFilePath = ui->LogFilePathLE->text();
        //        QString scxmlFilePath = ui->scxmlPathLE->text();
        QString schedulerPath = ui->SchedulerPathLE->text();
        bool initCheck = ui->ChkInitialization->isChecked();
        QString sInitCheck = (initCheck ? TRUE : FALSE);

        QDomDocument *domDoc = new QDomDocument();
        QDomProcessingInstruction procInstruct = domDoc->createProcessingInstruction("xml","version=\"1.0\"");
        domDoc->appendChild(procInstruct);
        QDomElement domElement = domDoc->createElement("Setting");
        domDoc->appendChild(domElement);

        domElement.setAttribute("Name",iSettingName);
        //        domElement.setAttribute("LogFilePath",logFilePath);
        //        domElement.setAttribute("ScxmlFilePath",scxmlFilePath);
        domElement.setAttribute("SchedulerPath",schedulerPath);
        domElement.setAttribute("Timeout",timeOut);
        domElement.setAttribute("TimeoutUnit",ui->TimeoutCB->currentText());
        domElement.setAttribute("InitWaitTime",initWaitTime);
        domElement.setAttribute("InitWaitTimeUnit",ui->WaitTimeCB->currentText());
        domElement.setAttribute("InitCheck",sInitCheck);

        QDomElement scriptElement = domDoc->createElement("Scripts");
        domElement.appendChild(scriptElement);

        foreach(QString script, initList)
        {
            QDomElement element = domDoc->createElement("Script");
            script.replace("\\","/");
            element.setAttribute("Path",script);
            scriptElement.appendChild(element);
        }

        QDomElement toolElement = domDoc->createElement("Tools");
        domElement.appendChild(toolElement);

        foreach(QString tool, toolList)
        {
            QDomElement element = domDoc->createElement("Tool");
            tool.replace("\\","/");
            element.setAttribute("Path",tool);
            toolElement.appendChild(element);
        }

        //Adding each slave element with IP and Name as attribute in DOM Doc
        QDomElement slaveElement = domDoc->createElement("Slaves");
        domElement.appendChild(slaveElement);

        for(int i=0; i < _rowCount ;i++)
        {
            QDomElement element = domDoc->createElement("Slave");

            element.setAttribute("IP", ui->SeqSlavesListTW->item(i,0)->text());
            element.setAttribute("AliasName", ui->SeqSlavesListTW->item(i,1)->text());
            slaveElement.appendChild(element);
        }

        QTextStream stream(&settingFile);
        stream << domDoc->toString();
        settingFile.close();

        rc = true;

        delete domDoc;
        domDoc = NULL;

    }
    return rc;
}

void GTASequencerSettings::setPaths(const QString &iLogFilePath, const QString &iScxmlPath, const QString &iToolDataPath)
{
    ui->LogFilePathLE->clear();
    ui->LogFilePathLE->setText(iLogFilePath);
    _logFilePath = iLogFilePath;
    ui->scxmlPathLE->clear();
    ui->scxmlPathLE->setText(iScxmlPath);
    _scxmlFilePath = iScxmlPath;
    _ToolDataPath =iToolDataPath;

}

void GTASequencerSettings::show()
{
    this->QDialog::show();
}

void GTASequencerSettings::onSchedulerPBClicked()
{
    _schedulerPath = QFileDialog::getOpenFileName(this,
                                                  "Select Scheduler Path",
                                                  "/home",
                                                  "Scheduler Path (*.exe)");
    ui->SchedulerPathLE->setText(_schedulerPath);
}


void GTASequencerSettings::onApplyClicked()
{
    QString schedPath = _schedulerPath = ui->SchedulerPathLE->text();
    if(ui->SchedulerPathLE->text().isEmpty())//_schedulerPath.isEmpty())
    {
        QMessageBox::warning(this,"Incomplete information","The Scheduler Path cannot be empty",QMessageBox::Ok);
    }
    else
    {
        QFile file(schedPath);
        if(!file.exists())
        {
            QMessageBox::warning(this,"Error","The Scheduler Path does not exist",QMessageBox::Ok);
        }
        else
        {
            QString sWaitTime = ui->WaitTimeLE->text();

            int waitTime = sWaitTime.toInt();
            if(ui->WaitTimeCB->currentText() == "min")
            {
                waitTime = waitTime * 60;
            }

            QString sTimeout = ui->TimeoutLE->text();
            int timeout = sTimeout.toInt();
            if(ui->TimeoutCB->currentText() == "min")
            {
                timeout = timeout * 60;
            }
            else if(ui->TimeoutCB->currentText() == "hr")
            {
                timeout = timeout * 3600;
            }

            SequencerSettings settings;
            settings.schedulrePath = schedPath;
            settings.waitTime = waitTime;
            settings.initList = getInitList();
            settings.toolList = getToolList();
            settings.logFilePath = ui->LogFilePathLE->text();
            settings.scxmlPath = ui->scxmlPathLE->text();
            settings.timeout = timeout;
            settings.initCheck = ui->ChkInitialization->isChecked();
            settings.slaveList = getSlaveList();
            settings.slaveClientlist = _ConnectedclientLists;

            emit settingsApplied(settings);
            //        emit settingsApplied(_schedulerPath,waitTime,initList,toolList,logFilePath,scxmlFilePath,timeout);
        }
    }
}


QMap<QString,QString> GTASequencerSettings::getSlaveList()
{
    QMap<QString,QString> slaveList;
    for(int i = 0; i < _rowCount; i++)
    {
        QPushButton* pPushButton  = dynamic_cast<QPushButton*>(ui->SeqSlavesListTW->cellWidget(i,2));
        if(pPushButton)
        {
            if(pPushButton->text().compare("Disconnect",Qt::CaseInsensitive) == 0)
            {
                slaveList.insert(ui->SeqSlavesListTW->item(i,0)->text(),ui->SeqSlavesListTW->item(i,1)->text());
            }
        }

    }
    return slaveList;
}

QStringList GTASequencerSettings::getInitList()
{
    QStringList initList;
    for(int i = 0; i < ui->InitLW->count(); i++)
    {
        QListWidgetItem *pItem = ui->InitLW->item(i);
        if(pItem)
        {
            initList.append(pItem->text());
        }
    }
    return initList;
}

QStringList GTASequencerSettings::getToolList()
{
    QStringList toolList;
    for(int i = 0; i < ui->ToolLW->count(); i++)
    {
        QListWidgetItem *pItem = ui->ToolLW->item(i);
        if(pItem)
        {
            toolList.append(pItem->text());
        }
    }
    return toolList;
}

void GTASequencerSettings::onClearClicked()
{
    ui->ToolLW->clear();
    ui->InitLW->clear();
    ui->SchedulerPathLE->clear();
    ui->WaitTimeLE->clear();
    ui->WaitTimeCB->setCurrentIndex(0);
    ui->TimeoutLE->clear();
    ui->TimeoutCB->setCurrentIndex(0);
    for(int i = _rowCount-1 ; i>=0 ;i--)
    {
        ui->SeqSlavesListTW->removeRow(i);
        _rowCount--;
    }
    foreach(GTAClient * client, _ConnectedclientLists)
    {
        QVariantList args;
        const char *slot = SLOT(onDeleteServers(QVariant &, QNetworkReply *));
        const char *faultSlot = SLOT(onDeleteServersFailed(int, const QString &, QNetworkReply *));

        client->sendRequest(args,"disconnectToServer",this,slot,this,faultSlot);
    }
    qDeleteAll(_clientLists.begin(),_clientLists.end());
    _clientLists.clear();
}


//iVal = true buttons will be enabled
//iVal = false buttons will be disabled
void GTASequencerSettings::enableDisableSettings(bool iVal)
{
//    ui->ApplyPB->setEnabled(iVal);
//    ui->ClearPB->setEnabled(iVal);
    this->setEnabled(iVal);
}


void GTASequencerSettings::onAddSlaveTBClicked()
{    
    if(ui->slaveIPLE->text().isEmpty() && ui->slaveNameLE->text().isEmpty())
    {
        //QMessageBox::warning(nullptr,"Error"," Please enter Sequencer Slave IP address and Alias name",QMessageBox::Ok);
        return;
    }
    if(!ui->slaveIPLE->text().isEmpty() && ui->slaveNameLE->text().isEmpty())
    {
        QMessageBox::warning(this,"Error"," Please enter Sequencer Alias name, cannot be empty",QMessageBox::Ok);
        return;
    }
    QHostAddress address(ui->slaveIPLE->text());
    if(address.isNull())
        QMessageBox::warning(this,"Error"," Please enter a valid IP address",QMessageBox::Ok);
    else
    {
        bool rc = false;
        for(int i = 0; i < _rowCount ; i++)
        {

            QHostAddress addr(ui->SeqSlavesListTW->item(i,0)->text());
            if(address == addr)
            {
                rc = true;
                QMessageBox::warning(this,"Error"," Sequencer Slave with same IP is added already. Please enter a distinct IP ",QMessageBox::Ok);
                break;
            }

        }
        if(!rc)
            setSequencerSlaves(ui->slaveIPLE->text(),ui->slaveNameLE->text());

    }

}

void GTASequencerSettings::setSequencerSlaves(QString IP, QString Name)
{
    ui->SeqSlavesListTW->setRowCount(++_rowCount);   

    QTableWidgetItem *item = new QTableWidgetItem(IP);
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    ui->SeqSlavesListTW->setItem(_rowCount-1,0,item);


    QTableWidgetItem *itemName = new QTableWidgetItem(Name);
    itemName->setFlags(itemName->flags() ^ Qt::ItemIsEditable);
    ui->SeqSlavesListTW->setItem(_rowCount-1,1,itemName);

    QPushButton *pPushButton = new QPushButton();
    pPushButton->setText("Connect");

    QIcon icon;

    icon.addPixmap(QPixmap(":images/forms/img/Connection.png"));
    icon.addPixmap(QPixmap(":images/forms/img/Connection.png"), QIcon::Disabled);
    pPushButton->setIcon(icon);
    pPushButton->setProperty("Row",_rowCount-1);
    ui->SeqSlavesListTW->setCellWidget(_rowCount-1,2,pPushButton);
    connect(pPushButton,SIGNAL(clicked()),this, SLOT(onPushButtonClicked()));
    ui->slaveIPLE->clear();
    ui->slaveNameLE->clear();
}

void GTASequencerSettings::onRemoveSlaveTBClicked()
{
    QModelIndexList selectedIndexes = ui->SeqSlavesListTW->selectionModel()->selectedRows();
    QList<int> rows;
    foreach(QModelIndex idx , selectedIndexes)
    {
        rows.append(idx.row());
    }
    qSort(rows);
    for(int i = rows.count()-1 ; i >=0 ; i--)
    {
        QString ip = ui->SeqSlavesListTW->item(i,0)->text();
        for(int j = 0 ; j < _ConnectedclientLists.count() ; j++)
        {
            //TODO: Is this supposed to be always at(0)? And not i or j?
            GTAClient * client = _ConnectedclientLists.at(0);
            if(client->getUrl().contains(ip))
            {
                QVariantList args;
                const char *slot = SLOT(onDeleteServers(QVariant &, QNetworkReply *));
                const char *faultSlot = SLOT(onDeleteServersFailed(int, const QString &, QNetworkReply *));

                client->sendRequest(args,"disconnectToServer",this,slot,this,faultSlot);

                break;
            }
        }
        ui->SeqSlavesListTW->removeRow(rows.at(i));
        _rowCount--;

    }
}

void GTASequencerSettings::onPushButtonClicked()
{
    QPushButton *pPushButton = dynamic_cast<QPushButton*>(sender());
    bool rc = true;
    if(pPushButton)
    {
        QModelIndex idx = ui->SeqSlavesListTW->indexAt(pPushButton->pos());
        if(idx.isValid())
        {
            QVariant ConnectionStatus = pPushButton->property("ConnectionStatus");
            if(ConnectionStatus.isValid())
            {
                QString status = ConnectionStatus.toString();
                if(status.compare("TRUE",Qt::CaseInsensitive) == 0)
                {
                    rc = false;
                    showProgressDialog("Disconnecting...");
                    int Row = idx.row();
                    QString IP = ui->SeqSlavesListTW->item(Row,0)->text();
                    for(int i=0 ; i < _ConnectedclientLists.count() ; i++)
                    {
                        GTAClient * client = _ConnectedclientLists.at(i);
                        if(client->getUrl().contains(IP))
                        {
                            QVariantList args;
                            const char *slot = SLOT(onServerDisconnection(QVariant &, QNetworkReply *));
                            const char *faultSlot = SLOT(onServerDisconnectionFailed(int, const QString &, QNetworkReply *));

                            client->sendRequest(args,"disconnectToServer",this,slot,this,faultSlot);
                            pPushButton->setText("Connect");
                            pPushButton->setProperty("ConnectionStatus","FALSE");
                            break;
                        }
                    }

                }
            }

            if(rc)
            {
                showProgressDialog("Connecting...");

                int Row = idx.row();
                QString IP = ui->SeqSlavesListTW->item(Row,0)->text();
                _serverConnectionFailure.insert(IP,0);
                foreach(int port,_ports)
                {
                    _client = new GTAClient(IP,port);
                    _clientLists.append(_client);
                    if(_client->init())
                    {
                        QVariantList args;
                        const char *slot = SLOT(onServerClientConnection(QVariant &, QNetworkReply *));
                        const char *faultSlot = SLOT(onServerClientConnectionFailed(int, const QString &, QNetworkReply *));

                        _client->sendRequest(args,"connectToServer",this,slot,this,faultSlot);

                    }
                }
            }
        }
    }

}

void GTASequencerSettings::showProgressDialog(QString imsg)
{
	if (_progressDialog == nullptr)
	{
		_progressDialog = new QProgressDialog(this);
	}
        _progressDialog->setCancelButton(0);
        _progressDialog->setMinimum(0);
        _progressDialog->setWindowTitle(imsg);
        _progressDialog->setMaximum(0);
        //work on below to fix the size of QprogressDialog
        _progressDialog->setFixedSize(_progressDialog->sizeHint());
        _progressDialog->setWindowModality(Qt::ApplicationModal);
        _progressDialog->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
        _progressDialog->show();
}

void GTASequencerSettings::hideProgressDialog()
{
    if(_progressDialog != nullptr)
    {
        _progressDialog->hide();
    }
}

void GTASequencerSettings::onServerClientConnection(QVariant &iVal,  QNetworkReply * ireply)
{

    hideProgressDialog();
    QString str = iVal.toString();
    QString ip = ireply->url().host();
    for(int row=0;row<_rowCount;row++)
    {
        if(ip.compare(ui->SeqSlavesListTW->item(row,0)->text()) == 0)
        {
            if(str.simplified().trimmed().compare("true",Qt::CaseInsensitive) == 0)
            {
                QPushButton *pPushButton = dynamic_cast<QPushButton*>(ui->SeqSlavesListTW->cellWidget(row,2));
                if(pPushButton!=NULL)
                {
                    pPushButton->setText("Disconnect");
                    // pPushButton->setStyleSheet("color : #079992; background-color: #DAF3F2; font: bold 14px;");
                    pPushButton->setProperty("ConnectionStatus","TRUE");

                    // pPushButton->setEnabled(false);
                    _IsServerConnected = true;
                    foreach (GTAClient* client,_clientLists)
                    {
                        QString clienUrl = client->getUrl();
                        if (clienUrl.compare(ireply->url().toString(),Qt::CaseInsensitive) == 0)
                        {
                            GTAClient  *tempClient = new GTAClient(*client);
                            _ConnectedclientLists.append(tempClient);
                            break;
                        }
                    }
                }


            }
            else
            {
                QMessageBox::warning(this,"Error","Server could not be connected",QMessageBox::Ok);
            }
            break;

        }
    }
    _serverConnectionFailure.remove(ireply->url().host());
}

void GTASequencerSettings::onServerClientConnectionFailed(int , const QString &,  QNetworkReply * ireply)
{
    if(_serverConnectionFailure.contains(ireply->url().host()))
    {
        _serverConnectionFailure[ireply->url().host()]++;
        if(_serverConnectionFailure[ireply->url().host()] == _ports.count())
        {
            hideProgressDialog();
            QMessageBox::warning(this,"Error","Server could not be connected, as there is no active Sequencer Slave",QMessageBox::Ok);
            _serverConnectionFailure.remove(ireply->url().host());

        }
    }
}

void GTASequencerSettings::onServerDisconnection(QVariant &, QNetworkReply *ireply)
{
    removeConnectedClient(ireply->url().host());
    hideProgressDialog();
}


void GTASequencerSettings::onServerDisconnectionFailed(int , const QString &,  QNetworkReply * ireply)
{
    hideProgressDialog();
    QMessageBox::warning(this,"Error","Server could not be disconnected. Please check Sequencer Slave",QMessageBox::Ok);

    removeConnectedClient(ireply->url().host());
}

void GTASequencerSettings::onDeleteServers(QVariant &, QNetworkReply *ireply)
{
    removeConnectedClient(ireply->url().host());
}


void GTASequencerSettings::onDeleteServersFailed(int , const QString &,  QNetworkReply * ireply)
{
    removeConnectedClient(ireply->url().host());
}

void GTASequencerSettings::removeConnectedClient(QString IP)
{
    for(int i=0 ; i < _ConnectedclientLists.count() ; i++)
    {
        GTAClient * client = _ConnectedclientLists.at(i);
        if(client->getUrl().contains(IP))
        {
            _ConnectedclientLists.removeOne(client);
            client->deleteLater();
            break;
        }
    }
}


