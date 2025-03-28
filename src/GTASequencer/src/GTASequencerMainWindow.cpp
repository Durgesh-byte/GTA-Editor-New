#include "GTASequencerMainWindow.h"

#pragma warning(push, 0)
#include "ui_GTASequencerMainWindow.h"
#include <QThread>
#include <QMessageBox>
#include <QProcess>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrentRun>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>
#pragma warning(pop)


GTASequencerMainWindow::GTASequencerMainWindow(const QString &iToolDataPath, GTASequencer *pSequencer,QWidget *parent) :
    _pSequencer(pSequencer),
    QMainWindow(parent),
    ui(new Ui::GTASequencerMainWindow)
{
    ui->setupUi(this);
    initialise();
    initToolBar();

    QString logFilePath;
    QString scxmlPath;
    if(_pSequencer)
    {
        logFilePath = _pSequencer->getLogFilePath();
        scxmlPath = _pSequencer->getScxmlPath();
    }

    setWindowState(Qt::WindowMaximized);
    _pSettings  =  new GTASequencerSettings(iToolDataPath,logFilePath,scxmlPath,this);
    _pSettings->show();
    ui->SettingsDW->setWidget(_pSettings);

//    connect(_pSettings,SIGNAL(settingsApplied(QString,int,QStringList,QStringList,QString,QString,int)),
//            this,SLOT(onSettingsApplied(QString,int,QStringList,QStringList,QString,QString,int)));

    connect(_pSettings,SIGNAL(settingsApplied(SequencerSettings)),this,SLOT(onSettingsApplied(SequencerSettings)));

    ui->LoggerTE->append("Welcome to GTA sequence manager!!!");

    connect(ui->actionSettings,SIGNAL(triggered()),this,SLOT(onSettingsClicked()));
    connect(ui->actionClose,SIGNAL(triggered()),this,SLOT(close()));
    connect(ui->actionUser_Guide,SIGNAL(triggered()),this,SLOT(onOpenUserGuide()));

    ui->action_Start_Server->setVisible(false);
    ui->action_Test_Client->setVisible(false);
    //dev
    //connect(ui->action_Start_Server,SIGNAL(triggered()),this,SLOT(ServerClickSlot()));
    //connect(ui->action_Test_Client,SIGNAL(triggered()),this,SLOT(ClientClickSlot()));
    //

    connect(_pSequencer,SIGNAL(log(QString&)),this,SLOT(onLogReceived(QString&)));
    connect(_pSequencer,SIGNAL(showProgressBar()),this,SLOT(onShowProgressBar()));
    connect(_pSequencer,SIGNAL(hideProgressBar()),this,SLOT(onHideProgressBar()));
    connect(_pSequencer,SIGNAL(fileExecuted(QString,QString)),this,SLOT(onFileExecuted(QString,QString)));
    connect(_pSequencer,SIGNAL(processingComplete()),this,SLOT(onProcessingComplete()));

//    connect(_pSequencer,SIGNAL(sequencingComplete(QDateTime,QMap<QString,QString>)),this,
//            SIGNAL(sequencingComplete(QDateTime,QMap<QString,QString>)));


    _pProgressBar = new QProgressBar(this);
    _pProgressBar->hide();
//    _pProgressBarText = new QLabel("Testing....",this);
//    ui->statusbar->addPermanentWidget(_pProgressBarText);
//    _pProgressBarText->setVisible(true);


}

void GTASequencerMainWindow::ServerClickSlot()
{
    //always ensure there is just one instance of the server running
    if (_server != NULL)
    {
        delete _server;
        _server = NULL;
    }
    _server = new GTAServer("127.0.0.1",8082);
    if (_server->init())
    {
        _server->registerUserFunctions(this,"examples.birne","birne");
        _server->registerUserFunctions(this,"examples.nix","nix");
    }
}

void GTASequencerMainWindow::ClientClickSlot()
{
    if (_client != NULL)
    {
        delete _client;
        _client = NULL;
    }
    _client = new GTAClient("127.0.0.1",8082);
    if (_client->init())
    {
        QVariantList args;
        const char *slot = SLOT(testResponse(QVariant &));
        _client->sendRequest(args,"examples.nix",this,slot);
        args << QVariant(7);
        _client->sendRequest(args,"examples.birne",this,slot);
    }
}

void GTASequencerMainWindow::testResponse(QVariant &arg) {
    qDebug()<<arg;
}

void GTASequencerMainWindow::nix()
{
    qDebug()<<"I am called\n";
}

QVariant GTASequencerMainWindow::birne(int x)
{
    if(x < 0)
    {
        //return QVariant::fromValue(MaiaFault(7, "Birne is doof",this).toString());
        return "Birne is doof";
    }
    else
        return "Tolle Birne";
}

GTASequencerMainWindow::~GTASequencerMainWindow()
{
    if(_pSettings != NULL)
    {
        delete _pSettings;
        _pSettings = NULL;
    }
 
    if(_pProgressBar != NULL)
    {
        delete _pProgressBar;
        _pProgressBar = NULL;
    }
    if (_server != NULL)
    {
        delete _server;
        _server = NULL;
    }
    if (_client != NULL)
    {
        delete _client;
        _client = NULL;
    }

    delete ui;
}


void GTASequencerMainWindow::onCloseSequencer(bool &oRetVal)
 {
     oRetVal = true;
     if(_pSequencer != NULL)
     {
         if(_pSequencer->isSchedulerRunning())
         {
             QMessageBox::StandardButton button = QMessageBox::information(this,"Warning","Sequence execution in progress. Would you like to stop the execution?",QMessageBox::Yes,QMessageBox::No);
             if(button == QMessageBox::Yes)
             {
                 _pSequencer->stop();
             }
             else
             {
                 oRetVal = false;
             }

         }
     }
     if(oRetVal)
         close();
}

 void GTASequencerMainWindow::closeEvent(QCloseEvent *event)
 {
     emit sequencerWindowClosed();
     QMainWindow::closeEvent(event);
 }

 void GTASequencerMainWindow::close()
 {

     this->QMainWindow::close();

 }

void GTASequencerMainWindow::initialise()
{
    _pSettings = NULL;
    _pStart = NULL;
    _pStop = NULL;
    _pReplay = NULL;
    _pReplayAll = NULL;
    _server = NULL;
    _client = NULL;
}

void GTASequencerMainWindow::initToolBar()
{
    _pStart = ui->actionToolBar->addAction(QIcon(":/images/forms/img/start.png"),"Start",this,SLOT(onStartClicked()));
    _pStart->setEnabled(false);
    _pStop = ui->actionToolBar->addAction(QIcon(":/images/forms/img/stop.png"),"Stop",this,SLOT(onStopClicked()));
    _pReplay = ui->actionToolBar->addAction(QIcon(":/images/forms/img/replayOne.png"),"Replay",this,SLOT(onReplayClicked()));
    _pReplayAll = ui->actionToolBar->addAction(QIcon(":/images/forms/img/replayAll.png"),"Replay All",this,SLOT(onReplayAllClicked()));
    enableDisableToolButtons(false);
}

void GTASequencerMainWindow::onSettingsClicked()
{
    if(_pSettings != NULL)
    {
        _pSettings->show();
    }
}

void GTASequencerMainWindow::onSettingsApplied(const SequencerSettings &iSetting)
{
    if(_pSequencer)
    {
        _pStart->setEnabled(true);
        _pSequencer->setSchedulerPath(iSetting.schedulrePath);
        _pSequencer->setInitTime(iSetting.waitTime);
        _pSequencer->setInitScripts(iSetting.initList);
        _pSequencer->setTools(iSetting.toolList);
        _pSequencer->setLogFilePath(iSetting.logFilePath);
        _pSequencer->setScxmlPath(iSetting.scxmlPath);
        _pSequencer->setTimeout(iSetting.timeout);
        _pSequencer->setInitCheck(iSetting.initCheck);
        _pSequencer->setSlaveList(iSetting.slaveList, iSetting.slaveClientlist);
        QMessageBox::information(this,"Success","Settings applied sucessfully. You can proceed with sequence management",QMessageBox::Ok);
    }
}


void GTASequencerMainWindow::onStartClicked()
{
    if(_pSequencer)
    {
        if(_pSettings)
            _pSettings->enableDisableSettings(false);
        startButtonClick();
        QTimer::singleShot(10,this,SLOT(startSequencer()));
    }
}

void GTASequencerMainWindow::startSequencer()
{
    if(_pSequencer != NULL)
    {
        _pSequencer->start();
    }
}

void GTASequencerMainWindow::startButtonClick()
{
    _pStart->setEnabled(false);
    enableDisableToolButtons(true);
}

void GTASequencerMainWindow::onStopClicked()
{
    if(_pSequencer)
    {
        _pStart->setEnabled(true);
        _pStop->setEnabled(false);
        _pReplay->setEnabled(true);
        _pReplayAll->setEnabled(true);
        stopSequencer();
        if(_pSettings)
            _pSettings->enableDisableSettings(true);
    }
}

void GTASequencerMainWindow::stopSequencer()
{
    if(_pSequencer != NULL)
    {
         _pSequencer->stop();
    }
}


void GTASequencerMainWindow::enableDisableToolButtons(bool iVal)
{
    _pStop->setEnabled(iVal);
    _pReplay->setEnabled(iVal);
    _pReplayAll->setEnabled(iVal);
}

void GTASequencerMainWindow::onReplayClicked()
{
    if(_pSequencer)
    {
        if(_pSettings)
            _pSettings->enableDisableSettings(false);
        _pStop->setEnabled(true);
        _pSequencer->replay();

    }
}


void GTASequencerMainWindow::onReplayAllClicked()
{
    if(_pSequencer)
    {
        if(_pSettings)
            _pSettings->enableDisableSettings(false);
        _pStop->setEnabled(true);
        _pSequencer->replayAll();
    }
}


void GTASequencerMainWindow::onLogReceived(QString &iMessage)
{
    ui->LoggerTE->append(iMessage);
}

void GTASequencerMainWindow::onFileExecuted(const QString &iFileName, const QString &iStatus)
{

    QColor textColor;

    if(iStatus == "OK")
    {
        textColor.setRgb(0,128,0);      //green
    }
    else if(iStatus == "KO")
    {
        textColor.setRgb(255,0,0);      //red
    }


    ui->LoggerTE->setTextColor(textColor);
    ui->LoggerTE->append(QString("%1 executed. Execution Status : %2").arg(iFileName,iStatus));
    textColor.setRgb(0,0,0);                                    //back to black font
    ui->LoggerTE->setTextColor(textColor);

}

void GTASequencerMainWindow::onHideProgressBar()
{
    //    _pProgressBar->hide();
    //    _pProgressBar->setVisible(false);
//    _pProgressBarText->setText("Completed...");
}

void GTASequencerMainWindow::onShowProgressBar()
{
    //    _pProgressBar->show();
    //    _pProgressBar->setVisible(true);
//    _pProgressBarText->setText("Processing...");
}

void GTASequencerMainWindow::onProcessingComplete()
{
    _pStart->setEnabled(true);
    _pStop->setEnabled(false);
    _pReplay->setEnabled(true);
    _pReplayAll->setEnabled(true);
    onHideProgressBar();
    ui->LoggerTE->append("Execution completed...");
    if(_pSettings)
        _pSettings->enableDisableSettings(true);
}


void GTASequencerMainWindow::onOpenUserGuide()
{
    QString userGuidePath = QString("file:///%1%2%3").arg(QApplication::applicationDirPath(), QDir::separator(),"Sequence Management User Guide.pdf");
    bool rc = QDesktopServices::openUrl(QUrl(userGuidePath));
    if(!rc)
    {
        QMessageBox::warning(this,"Error","User guide not found",QMessageBox::Ok);
    }
}
