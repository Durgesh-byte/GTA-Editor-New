#include "AINGTAICMain.h"
#include "ui_AINGTAICMain.h"
#include "AINGTAServer.h"
#include "AINGTAICUserFeedbackWidget.h"
#include <QMouseEvent>
#include <QMessageBox>
#define CONF "conf"
#define CONF_FILENAME "GTAInteractiveController"


AINGTAICMain::AINGTAICMain(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::AINGTAICMain)
{
    ui->setupUi(this);
    _mousePressed = false;
    _mousePosition = QPoint(); this->setWindowFlags(Qt::FramelessWindowHint);
    ui->showEditorTB->setAutoRaise(true);
    ui->minimizeTB->setAutoRaise(true);
    ui->closeTB->setAutoRaise(true);
    ui->windowIconsLayout->setSpacing(0);
    ui->windowIconsLayout->setContentsMargins(0, 0, 0, 0);

    ui->startStopRB->setStyleSheet("QRadioButton {    spacing: 5px;}QRadioButton::indicator {    width: 150px;    height: 150px;}QRadioButton::indicator:unchecked {    image: url(:images/forms/img/Start.png);}QRadioButton::indicator:unchecked:hover {    image: url(:images/forms/img/Start.png); }QRadioButton::indicator:unchecked:pressed {    image: url(:images/forms/img/Start_Pressed.png); }QRadioButton::indicator:checked {    image: url(:images/forms/img/Stop.png);}QRadioButton::indicator:checked:pressed {    image: url(:images/forms/img/Stop_Pressed.png);}QRadioButton::indicator:indeterminate:pressed {    image: url(:images/forms/img/checkbox_indeterminate_pressed.png);}");
    QStringList availableIPs = AINGTAServer::getAvailableAddress();
    ui->IPCB->clear();
    ui->IPCB->addItems(availableIPs);
    ui->portSB->setMaximum(65535);
    ui->portSB->setMinimum(0);
    QSize si;
    si.setHeight(0);
    si.setWidth(0);
    ui->editorTE->resize(si);
    this->layout()->setAlignment(ui->MainFrame, Qt::AlignLeft);
    ui->editorTE->hide();

    //resize frame to show only the start options
    QSize size;
    size.setHeight(400);
    size.setWidth(250);
    this->resize(size);

    ui->showEditorTB->setCheckable(true);
    connect(ui->showEditorTB,SIGNAL(toggled(bool)),this,SLOT(displayEditor(bool)));
    connect(ui->startStopRB,SIGNAL(toggled(bool)),this,SLOT(startServer(bool)));
    connect(ui->minimizeTB,SIGNAL(clicked()),this,SLOT(minimizeApplication()));
    connect(ui->closeTB,SIGNAL(clicked()),this,SLOT(close()));
    connect(this,SIGNAL(log(QString)),this,SLOT(updateLogWindow(QString)));
    ui->editorTE->clear();
    ui->editorTE->setReadOnly(true);
    _pServerThread = NULL;
    _pServer = NULL;
    readSettings();

}

AINGTAICMain::~AINGTAICMain()
{
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
    delete ui;
}

void AINGTAICMain::readSettings()
{
    QString appPath = QApplication::applicationDirPath();
    QString settingsPath = QDir::cleanPath( QString("%1%2%3%2%4.conf").arg(appPath,QDir::separator(),CONF,CONF_FILENAME));
    QSettings settings(settingsPath,QSettings::IniFormat);
    settings.beginGroup("GTAInteractiveController");
    int index = ui->IPCB->findText(settings.value("Address").toString());
    ui->IPCB->setCurrentIndex(index);
    ui->portSB->setValue(settings.value("Port").toInt());
    if(settings.value("Autostart").toString().compare("1") == 0)
    {
        ui->startStopRB->toggle();
    }
    settings.endGroup();
}

void AINGTAICMain::displayEditor(bool iVal)
{
    //showing the Text Edit with messages
    if(iVal)
    {
        ui->editorTE->setMinimumSize(0,0);
        QSize size;
        size.setHeight(400);
        size.setWidth(227);
        QPropertyAnimation *anim = new QPropertyAnimation(this, "size");
        anim->setDuration(300);
        anim->setStartValue(size);
        size.setWidth(450);
        anim->setKeyValueAt(0.5,size);
        size.setWidth(800);
        anim->setEndValue(size);
        anim->start();
        ui->showEditorTB->setArrowType(Qt::LeftArrow);
        ui->showEditorTB->setToolTip("Minimize log");
        ui->editorTE->setMinimumWidth(0);
        ui->editorTE->show();

    }
    //hiding the Text Edit with messages
    else
    {
        QSize size;
        size.setHeight(400);
        size.setWidth(800);
        QPropertyAnimation *anim = new QPropertyAnimation(this, "size");
        anim->setDuration(100);
        anim->setStartValue(size);
        size.setWidth(450);
        anim->setKeyValueAt(0.5,size);
        size.setWidth(250);
        anim->setEndValue(size);
        anim->start();
        ui->editorTE->hide();
        ui->showEditorTB->setArrowType(Qt::RightArrow);
        ui->showEditorTB->setToolTip("Maximize log");

    }
}

void AINGTAICMain::startServer(bool iVal)
{
    if(iVal)
    {
        AINGTAICServerClientComm::_serverIP = ui->IPCB->currentText();
        AINGTAICServerClientComm::_port = ui->portSB->value();
        if (NULL != _pServerThread)
        {
            _pServerThread->exit(0);
            while (_pServerThread->isRunning());
            delete _pServerThread;
            _pServerThread =NULL;
        }
        //to run the server in a seperate thread
        _pServerThread = new QThread(this);

        _pServer = new AINGTAICServerClientComm();
        connect(_pServerThread,SIGNAL(started()),_pServer,SLOT(initializeServer()),Qt::QueuedConnection);
        connect(_pServer,SIGNAL(serverStarted(bool)),this,SLOT(serverStarted(bool)));
        connect(_pServer,SIGNAL(displayMsg(QString,bool)),this,SLOT(onDisplayMessage(QString,bool)),Qt::BlockingQueuedConnection);
        connect(_pServer,SIGNAL(getUserInput(QString,QString)),this,SLOT(onGetUserInput(QString,QString)),Qt::BlockingQueuedConnection);
        _pServer->moveToThread(_pServerThread);
        _pServerThread->start(QThread::TimeCriticalPriority);
    }
    else
    {
        emit log(QString("[%1] XML-RPC server stopped on %2:%3 ").arg(QTime::currentTime().toString(),ui->IPCB->currentText(),ui->portSB->text()));
        ui->IPCB->setEnabled(true);
        ui->portSB->setEnabled(true);
        ui->startStopRB->setToolTip("Start Server");
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
    }
}

void AINGTAICMain::minimizeApplication()
{
    this->setWindowState(Qt::WindowMinimized);
}

void AINGTAICMain::mousePressEvent(QMouseEvent *event)
{

    if (event->button() == Qt::LeftButton)
    {
        _mousePressed = true;
        _mousePosition = event->pos();
    }
}

void AINGTAICMain::mouseMoveEvent(QMouseEvent *event)
{
    if (_mousePressed)
    {
        move(mapToParent(event->pos() - _mousePosition));
    }
}

void AINGTAICMain::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        _mousePressed = false;
        _mousePosition = QPoint();
    }
}

void AINGTAICMain::serverStarted(bool iVal)
{
    if(!iVal)
    {
        QMessageBox::warning(this,"Error","Server could not be started",QMessageBox::Ok);
        disconnect(ui->startStopRB,SIGNAL(toggled(bool)),this,SLOT(startServer(bool)));
        ui->startStopRB->toggle();
        connect(ui->startStopRB,SIGNAL(toggled(bool)),this,SLOT(startServer(bool)));
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
        emit log(QString("[%1] XML-RPC server could not be started on %2:%3 ").arg(QTime::currentTime().toString(),ui->IPCB->currentText(),ui->portSB->text()));

    }
    else
    {
        ui->IPCB->setEnabled(false);
        ui->portSB->setEnabled(false);
        emit log(QString("[%1] XML-RPC server started on %2:%3 ").arg(QTime::currentTime().toString(),ui->IPCB->currentText(),ui->portSB->text()));
        ui->startStopRB->setToolTip("Stop Server");
    }
}

bool AINGTAICMain::onDisplayMessage(QString msg, bool waitAck)
{
    /*
    QMessageBox *msgBox = new QMessageBox(this);
    msgBox->setWindowFlags( Qt::Dialog | Qt::CustomizeWindowHint);
    // msgBox->setAttribute(Qt::WA_DeleteOnClose);
    msg.replace("$&GTA_DOUBLE_QUOTE&$","\"",Qt::CaseSensitive);
    msgBox->setText(msg);
    if(waitAck)
    {
        emit log(QString("[%1] << displayMessage (blocking): %2").arg(QTime::currentTime().toString(),msg));
        QPushButton *okButton = msgBox->addButton("OK", QMessageBox::YesRole);
        QPushButton *koButton = msgBox->addButton("KO", QMessageBox::NoRole);

        msgBox->exec();
        QAbstractButton *buttonClicked = msgBox->clickedButton();
        if(buttonClicked != NULL)
        {
            QMessageBox::ButtonRole role= msgBox->buttonRole(buttonClicked);
            if(role== QMessageBox::YesRole)
            {
                emit log(QString("[%1] >> displayMessage (User Input): OK").arg(QTime::currentTime().toString()));
                return true;
            }
            else if(role== QMessageBox::NoRole)
            {
                emit log(QString("[%1] >> displayMessage (User Input): KO").arg(QTime::currentTime().toString()));
                return false;
            }
        }
        if(msgBox != NULL)
        {
            delete msgBox;
            msgBox = NULL;
        }
    }
    else
    {
        emit log(QString("[%1] << displayMessage (non-blocking): %2").arg(QTime::currentTime().toString(),msg));

        msgBox->setWindowFlags( Qt::Dialog | Qt::CustomizeWindowHint);
        msgBox->open(this,SLOT(msgBoxClosed()));
    }
*/

    msg.replace("$&GTA_DOUBLE_QUOTE&$","\"",Qt::CaseSensitive);
    if(waitAck)
        emit log(QString("[%1] << displayMessage (blocking): %2").arg(QTime::currentTime().toString(),msg));
    else
        emit log(QString("[%1] << displayMessage (non-blocking): %2").arg(QTime::currentTime().toString(),msg));

    AINGTAICUserFeedbackWidget *wdgt = new AINGTAICUserFeedbackWidget(this);
    wdgt->setWindowModality(Qt::ApplicationModal);

    connect(wdgt,SIGNAL(sendAcknowledgment(QString)),this,SLOT(onSendAcknowledgment(QString)));
    wdgt->setAttribute(Qt::WA_DeleteOnClose);
    wdgt->showPrompt(msg,waitAck);
    return true;
}

void AINGTAICMain::onGetUserInput(QString msg, QString expectedReturnVal)
{
    msg.replace("$&GTA_DOUBLE_QUOTE&$","\"",Qt::CaseSensitive);
    emit log(QString("[%1] << getParamValueSingleSample: wait for feedback from user - %2").arg(QTime::currentTime().toString(),expectedReturnVal));
    AINGTAICUserFeedbackWidget *wdgt = new AINGTAICUserFeedbackWidget(this);
    wdgt->setWindowModality(Qt::ApplicationModal);
    connect(wdgt,SIGNAL(sendUserInput(QString)),this,SLOT(sendUserInput(QString)));
    wdgt->setAttribute(Qt::WA_DeleteOnClose);
    wdgt->showWidget(msg,expectedReturnVal);
}
void AINGTAICMain::sendUserInput(QString reply)
{
    emit log(QString("[%1] >> getParamValueSingleSample (User Input): %2").arg(QTime::currentTime().toString(),reply));

    _pServer->setReturnReply(reply);
}

void AINGTAICMain::onSendAcknowledgment(QString reply)
{
    if(reply.compare("true") == 0)
        emit log(QString("[%1] >> displayMessage (User Input): OK").arg(QTime::currentTime().toString()));

    else
        emit log(QString("[%1] >> displayMessage (User Input): KO").arg(QTime::currentTime().toString()));

    _pServer->setReturnReply(reply);
}

//void AINGTAICMain::msgBoxClosed()
//{
//    QMessageBox * msgBox = dynamic_cast<QMessageBox*>(sender());
//    if(msgBox != NULL)
//    {
//        msgBox->deleteLater();
//    }
//}

void AINGTAICMain::updateLogWindow(QString text)
{
    ui->editorTE->append(text);
}
