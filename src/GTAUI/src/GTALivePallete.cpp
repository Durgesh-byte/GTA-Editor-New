#include "AINGTALivePallete.h"
#include "AINGTAICDParameter.h"
#include "AINGTAGenSearchWidget.h"
#include "ui_AINGTALivePallete.h"
#include "AINGTAClient.h"
#include "AINGTALivePalleteModel.h"

#include "AINGTADebugger.h"
#include "AINGTAAppController.h"
#include <QTableWidget>
#include <QMessageBox>
#include <QDebug>
#include <QDockWidget>
#include <QMessageBox>
#include <QFutureWatcher>
#include <QFuture>
#include <QtConcurrentRun>
#include <QComboBox>
#include <QWidgetAction>
#include <QDoubleSpinBox>

AINGTALivePallete::AINGTALivePallete(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AINGTALivePallete)
{
    ui->setupUi(this);
	_pLivePalleteModel = nullptr;
	_pRPCCallThread = nullptr;
	_clientCallHandler = nullptr;
    _pContextMenu = nullptr;
    _pCommandsCallThread = nullptr;
    _blockingResponse.clear();
    _pEditorCommands = nullptr;
    _pAcquisitionContextMenu = nullptr;

    createContextMenu();
     ui->parameterTW->setContextMenuPolicy(Qt::CustomContextMenu);
     setFocusPolicy(Qt::StrongFocus);
     ui->getParameterTV->setContextMenuPolicy(Qt::CustomContextMenu);

    _pGenSearchWdgt = new AINGTAGenSearchWidget();
    _pGenSearchWdgt->setSearchType(AINGTAGenSearchWidget::PARAMETER, AINGTAGenSearchWidget::LIVEMODE_PARAMETER_SEARCH);
    _pGenSearchWdgt->hide();
    ui->setPB->setToolTip("opens the parameter search to select an acquisition/generation parameter.");
    ui->powerCB->setToolTip("Starts/Stops the gta live mode");
    ui->progressBar->hide();

    QStringList columnValues = QStringList()<<"Parameter"<<"Forced Value"<<"FS / SSM"<<"Last Forced Value"<<"Last Forced Time";
    ui->parameterTW->setColumnCount(columnValues.count());
    ui->parameterTW->setHorizontalHeaderLabels(columnValues);
    ui->parameterTW->verticalHeader()->setVisible(false);
    ui->parameterTW->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->parameterTW->horizontalHeader()->setResizeContentsPrecision(10);
    ui->parameterTW->horizontalHeader()->setStretchLastSection(true);
    ui->parameterTW->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->parameterTW->setSelectionMode(QAbstractItemView::ExtendedSelection);

    _pEditorWindow = new AINGTAEditorWindow(parent,"Live.pro");

    //ownership of UI components are taken over by QSplitter
    //be careful about the deletion sequence
    QSplitter *parameterViews = new QSplitter(Qt::Horizontal);
    parameterViews->addWidget(ui->parameterTW);
    parameterViews->addWidget(ui->getParameterTV);

    QSplitter* buttonViewGroup = new QSplitter(Qt::Horizontal);
    buttonViewGroup->addWidget(parameterViews);
    buttonViewGroup->addWidget(ui->controlGroup);

    _pViewEditorGroup = new QSplitter(Qt::Vertical,this);
    _pViewEditorGroup->addWidget(buttonViewGroup);
    _pViewEditorGroup->addWidget(_pEditorWindow);
    ui->buttonGrid->addWidget(_pViewEditorGroup,ui->buttonGrid->rowCount(),0,1,ui->buttonGrid->columnCount());

    _pGenSearchWdgt = new AINGTAGenSearchWidget();
    _pGenSearchWdgt->setSearchType(AINGTAGenSearchWidget::PARAMETER, AINGTAGenSearchWidget::LIVEMODE_PARAMETER_SEARCH);
    _pGenSearchWdgt->hide();

    //connect ui components
    connect(ui->parameterTW, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(displayContextMenu(const QPoint&)));
    connect(ui->setPB,SIGNAL(clicked()),_pGenSearchWdgt,SLOT(show()));
    connect(_pGenSearchWdgt,SIGNAL(okPressed()),this,SLOT(onSearchWidgetOKClicked()));
    connect(ui->powerCB,&QCheckBox::clicked,this,&AINGTALivePallete::init);
    connect(ui->editorEnabler,&QSlider::valueChanged,this,&AINGTALivePallete::manageInterfaceForCalls);
//	connect(ui->complexPB, &QPushButton::clicked,this,&AINGTALivePallete::onUpdateEditorSignal);
    ui->complexPB->hide();

    _SetParamList.clear(); 
    _SSMList<<ACT_FSSSM_NO<<ACT_FSSSM_ND<<ACT_FSSSM_FW<<ACT_FSSSM_NCD<<ACT_FSSSM_FT<<ACT_FSSSM_MINUS<<ACT_FSSSM_PLUS<<ACT_FSSSM_LOST<<ACT_FSSSM_NOCHANGE;

    ui->getParameterTV->setModel(nullptr);
    ui->getParameterTV->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->getParameterTV->horizontalHeader()->setStretchLastSection(true);
    ui->getParameterTV->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->getParameterTV->verticalHeader()->setVisible(false);
    ui->getParameterTV->show();
    connect(ui->getParameterTV,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(displayAcquisitionContextMenu(const QPoint&)));

    manageInterfaceForCalls(0);
}

AINGTALivePallete::~AINGTALivePallete()
{
    ui->getParameterTV->setModel(nullptr);
    if (nullptr != _pGenSearchWdgt)
    {
        delete _pGenSearchWdgt;
        _pGenSearchWdgt = nullptr;
    }
    if (nullptr != _pLivePalleteModel)
    {
        delete _pLivePalleteModel;
        _pLivePalleteModel = nullptr;
    }
    if (nullptr != _pCommandsCallThread)
    {
        _pCommandsCallThread->exit(0);
        while (_pCommandsCallThread->isRunning());
        delete _pCommandsCallThread;
        _pCommandsCallThread = nullptr;
    }
    if (nullptr != _pEditorCommands)
    {
        delete _pEditorCommands;
        _pEditorCommands = nullptr;
    }
    if (nullptr != _pViewEditorGroup)
    {
        delete _pViewEditorGroup;
        _pViewEditorGroup = nullptr;
    }
    //do not need to clean _pEditorWindow as _pViewEditorGroup owned it and has freed the memory
    if (nullptr != _pEditorWindow)
    {
        delete _pEditorWindow;
        _pEditorWindow = nullptr;
    }
    if (nullptr != _pRPCCallThread)
    {
        _pRPCCallThread->exit(0);
        while (_pRPCCallThread->isRunning());
        delete _pRPCCallThread;
        _pRPCCallThread =nullptr;
    }
    if (nullptr != _clientCallHandler)
    {
        delete _clientCallHandler;
        _clientCallHandler = nullptr;
    }
    if(nullptr != _pContextMenu)
    {
        delete _pContextMenu;
        _pContextMenu = nullptr;
    }
    if(nullptr != _pAcquisitionContextMenu)
    {
        delete _pAcquisitionContextMenu;
        _pAcquisitionContextMenu = nullptr;
    }
    delete ui;
}

/**
  * Author: Saurav
  * @brief: function call to start the client call thread for calls to bisg server.
  *         The function is called by a switch (signal on to)signal: toggle)
  * @iVal: true/false decides whether to start/stop the client call thread
  */
void AINGTALivePallete::init(bool iVal)
{
    QVariantList args;
    _SetParamList.clear();
    if (iVal)
    {
        //start client calls thread
        _pRPCCallThread = new QThread(this);

        TestConfig* testConf = TestConfig::getInstance();
        QString bisgip = QString::fromStdString(testConf->getBISGIP());
        int port = testConf->getBISGPort();

        _clientCallHandler = new AINGTAClientCalls(bisgip,port);
        _pLivePalleteModel = new AINGTALivePalleteModel();

        connect(_pRPCCallThread,&QThread::started,_clientCallHandler,&AINGTAClientCalls::startClient,Qt::QueuedConnection);

        //sending the requests

        //sending a bisg request for set/get should not be blocking and must be handled in respective threads
        //thus the connection type is queued
        connect(this,&AINGTALivePallete::sendBISGRequest,_clientCallHandler,&AINGTAClientCalls::sendBISGRequest,Qt::QueuedConnection);
        connect(_pLivePalleteModel,&AINGTALivePalleteModel::sendBenchRequest,this,&AINGTALivePallete::sendBISGRequest,Qt::QueuedConnection);
        connect(this, &AINGTALivePallete::updateSignalRange,_pLivePalleteModel,&AINGTALivePalleteModel::onUpdateParamRangeReceived,Qt::QueuedConnection);

        //sending an external tool request should be blocking and must be handled in respective threads
        //but the signaling thread must wait for the operation on receiveing thread to finish
        //thus the connection type is blocking queued
        //there are instances where signals need to be generated/acquired while executing command
        //then a blocking queued connection is needed to ensure the signaling thread waits for the status response
        connect(this,&AINGTALivePallete::sendExtToolRequest,_clientCallHandler,&AINGTAClientCalls::sendExtToolRequest,Qt::BlockingQueuedConnection);
        connect(this,&AINGTALivePallete::sendBlockingBISGRequest,_clientCallHandler,&AINGTAClientCalls::sendBlockingBISGRequest,Qt::BlockingQueuedConnection);


        //handling the responses

        //responses must be sent out to objects immediately. thus a direct connection to ensure call is made through signaling thread
        //once the call is received to this class, the call can be accordingly handled by relevant connecting classes
        //Queued connection is managed for updating of the output window and intended to block UI to show output calls to ensure connection is working
        //Queued connection is managed for model because this does not block the main thread UI
        connect(_clientCallHandler,&AINGTAClientCalls::returnResponse,this,&AINGTALivePallete::sendResponse,Qt::DirectConnection);
        connect(_clientCallHandler,&AINGTAClientCalls::returnBlockedResponse,this,&AINGTALivePallete::sendResponse,Qt::DirectConnection);
        connect(this,&AINGTALivePallete::sendResponse,_pLivePalleteModel,&AINGTALivePalleteModel::onGetMultiParamFinished,Qt::QueuedConnection);
        connect(this,&AINGTALivePallete::sendResponse,this,&AINGTALivePallete::onResponseReceived,Qt::QueuedConnection);
        connect(_clientCallHandler,&AINGTAClientCalls::returnBlockedResponse,this,&AINGTALivePallete::onBlockingResponseReceived,Qt::DirectConnection);


        _clientCallHandler->moveToThread(_pRPCCallThread);
        ui->getParameterTV->setModel(_pLivePalleteModel);

        _pRPCCallThread->start(QThread::TimeCriticalPriority);
        MyThread::sleep(1);

        emit sendBISGRequest(args,UNSUBSCRIBE,nullptr,nullptr);

        //start commands handling thread
        _pEditorWindow->clearSelectedRows();
        onUpdateEditorSignal();

        if (nullptr != _pCommandsCallThread)
        {
            _pCommandsCallThread->exit(0);
            while (_pCommandsCallThread->isRunning());
            delete _pCommandsCallThread;
            _pCommandsCallThread = nullptr;
        }
        if (nullptr != _pEditorCommands)
        {
            disconnect(_pEditorCommands,&AINGTALivePalleteCommands::getParameterValue,this,&AINGTALivePallete::getParameterValue);
            disconnect(_pEditorCommands,&AINGTALivePalleteCommands::setParameterValue,this,&AINGTALivePallete::setParameterValue);
            disconnect(_pEditorCommands,&AINGTALivePalleteCommands::finished,this,&AINGTALivePallete::onExecutingEditedCommandsFinished);
            disconnect(_pEditorCommands,&AINGTALivePalleteCommands::sendBlockingExtToolRequest,this,&AINGTALivePallete::sendBlockingExtToolRequest);
			disconnect(_pEditorCommands,&AINGTALivePalleteCommands::getICDParameterList,this,&AINGTALivePallete::getICDParameterList);
            disconnect(this,&AINGTALivePallete::executeAndAnalyzeElement,_pEditorCommands,&AINGTALivePalleteCommands::executeAndAnalyzeElement);
            delete _pEditorCommands;
            _pEditorCommands = nullptr;
        }

        _pCommandsCallThread = new QThread(this);
        _pEditorCommands = new AINGTALivePalleteCommands(_pEditorWindow->getElement()/*,this*/);

        connect(_pCommandsCallThread,&QThread::started,_pEditorCommands,&AINGTALivePalleteCommands::init,Qt::QueuedConnection);
        //the calls of a generation are blocking calls
        //direct connections are made to ensure the UI does not freeze during blocking calls
        //the slots with direct connections ensure no UI operation as thread based UI calls would crash the application.
        //the calls with UI operation are made on Queued connection. Calls in queued connection also ensure UI not freezing
        connect(_pEditorCommands,&AINGTALivePalleteCommands::getParameterValue,this,&AINGTALivePallete::getParameterValue,Qt::DirectConnection);
        connect(_pEditorCommands,&AINGTALivePalleteCommands::setParameterValue,this,&AINGTALivePallete::setParameterValue,Qt::DirectConnection);
        connect(_pEditorCommands,&AINGTALivePalleteCommands::finished,this,&AINGTALivePallete::onExecutingEditedCommandsFinished,Qt::QueuedConnection);
        connect(_pEditorCommands,&AINGTALivePalleteCommands::sendBlockingExtToolRequest,this,&AINGTALivePallete::sendBlockingExtToolRequest,Qt::DirectConnection);
        connect(_pEditorCommands,&AINGTALivePalleteCommands::getICDParameterList,this,&AINGTALivePallete::getICDParameterList,Qt::DirectConnection);
        connect(this,&AINGTALivePallete::executeAndAnalyzeElement,_pEditorCommands,&AINGTALivePalleteCommands::executeAndAnalyzeElement,Qt::QueuedConnection);


        _pEditorCommands->moveToThread(_pCommandsCallThread);
        _pCommandsCallThread->start(QThread::TimeCriticalPriority);
        MyThread::sleep(1);

        QList<GTALiveToolParams> gtaLiveToolList;
        QList<QString> toolList, tempList;
        GTALiveToolParams toolParam;
        toolList = QString::fromStdString(TestConfig::getInstance()->getToolList()).split(":");
        toolList.removeLast();

        for (int i = 0; i < toolList.size(); i++)
        {
            tempList = toolList.at(i).split(',');
            toolParam.toolName = tempList.first();
            toolParam.toolIP = tempList.at(1);
            toolParam.port = tempList.last().toInt();
            gtaLiveToolList.insert(i, toolParam);
        }

        foreach (GTALiveToolParams param, gtaLiveToolList)
        {
            _pEditorCommands->populateLiveTools(param.toolName,param.toolIP,param.port);
        }

        emit toggleEditorActions(false);
        manageInterfaceForCalls(1);
    }
    else
    {
        QMessageBox msgBox (this);
        msgBox.setText("Changing the view will stop the live execution. All changes will be lost. Continue?");
        msgBox.setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("The Live Execution will be stopped!");
        msgBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        int reply = msgBox.exec();
        if (reply != QMessageBox::Yes)
        {
            ui->powerCB->setChecked(true);
            return;
        }

        //reset the slider to enable generation and acquisition panels on start
        disconnect(ui->editorEnabler,&QSlider::valueChanged,this,&AINGTALivePallete::manageInterfaceForCalls);
        ui->editorEnabler->setValue(0);
        connect(ui->editorEnabler,&QSlider::valueChanged,this,&AINGTALivePallete::manageInterfaceForCalls);


        _pLivePalleteModel->stopForcing();
        _pLivePalleteModel->clearParamList();

        emit sendBISGRequest(args,UNSUBSCRIBE,nullptr,nullptr);

        MyThread::sleep(1);
        _SetParamList.clear();

        //disconnect all client call connections
        disconnect(_pRPCCallThread,&QThread::started,_clientCallHandler,&AINGTAClientCalls::startClient);
        disconnect(this,&AINGTALivePallete::sendBISGRequest,_clientCallHandler,&AINGTAClientCalls::sendBISGRequest);
        disconnect(_pLivePalleteModel,&AINGTALivePalleteModel::sendBenchRequest,this,&AINGTALivePallete::sendBISGRequest);
        disconnect(this, &AINGTALivePallete::updateSignalRange,_pLivePalleteModel,&AINGTALivePalleteModel::onUpdateParamRangeReceived);
        disconnect(this,&AINGTALivePallete::sendExtToolRequest,_clientCallHandler,&AINGTAClientCalls::sendExtToolRequest);
        disconnect(this,&AINGTALivePallete::sendBlockingBISGRequest,_clientCallHandler,&AINGTAClientCalls::sendBlockingBISGRequest);
        disconnect(_clientCallHandler,&AINGTAClientCalls::returnResponse,this,&AINGTALivePallete::sendResponse);
        disconnect(_clientCallHandler,&AINGTAClientCalls::returnBlockedResponse,this,&AINGTALivePallete::sendResponse);
        disconnect(this,&AINGTALivePallete::sendResponse,this,&AINGTALivePallete::onResponseReceived);
        disconnect(this,&AINGTALivePallete::sendResponse,_pLivePalleteModel,&AINGTALivePalleteModel::onGetMultiParamFinished);
        disconnect(_clientCallHandler,&AINGTAClientCalls::returnBlockedResponse,this,&AINGTALivePallete::onBlockingResponseReceived);

        //disconnect all command connections
        disconnect(_pCommandsCallThread,&QThread::started,_pEditorCommands,&AINGTALivePalleteCommands::init);
        disconnect(_pEditorCommands,&AINGTALivePalleteCommands::getParameterValue,this,&AINGTALivePallete::getParameterValue);
        disconnect(_pEditorCommands,&AINGTALivePalleteCommands::setParameterValue,this,&AINGTALivePallete::setParameterValue);
        disconnect(_pEditorCommands,&AINGTALivePalleteCommands::finished,this,&AINGTALivePallete::onExecutingEditedCommandsFinished);
        disconnect(_pEditorCommands,&AINGTALivePalleteCommands::sendBlockingExtToolRequest,this,&AINGTALivePallete::sendBlockingExtToolRequest);
        disconnect(_pEditorCommands,&AINGTALivePalleteCommands::getICDParameterList,this,&AINGTALivePallete::getICDParameterList);
        disconnect(this,&AINGTALivePallete::executeAndAnalyzeElement,_pEditorCommands,&AINGTALivePalleteCommands::executeAndAnalyzeElement);

        if (nullptr != _pRPCCallThread)
        {
            _pRPCCallThread->exit(0);
            while (_pRPCCallThread->isRunning());
            delete _pRPCCallThread;
            _pRPCCallThread =nullptr;
        }
        if (nullptr != _clientCallHandler)
        {
            delete _clientCallHandler;
            _clientCallHandler = nullptr;
        }
        if (nullptr != _pLivePalleteModel)
        {
            delete _pLivePalleteModel;
            _pLivePalleteModel = nullptr;
        }
        if (nullptr != _pCommandsCallThread)
        {
            _pCommandsCallThread->exit(0);
            while (_pCommandsCallThread->isRunning());
            delete _pCommandsCallThread;
            _pCommandsCallThread = nullptr;
        }
        if (nullptr != _pEditorCommands)
        {
            delete _pEditorCommands;
            _pEditorCommands = nullptr;
        }

        ui->parameterTW->clearContents();

        //prompt for save only if the editor has something to save
        if(!_pEditorWindow->isDataEmpty())
        {
            int msgReply = QMessageBox::information(this,"Save GTA Live Execution Editor File","Do you want to save GTA live editor executed commands",QMessageBox::Yes,QMessageBox::No);
            //the mechanism proceeds with a file dialog and after file dialog operation, the mode is automatically reset
            //if the answer is no, we reset it ourselves
            if(msgReply == QMessageBox::Yes)
            {
                emit saveTheEditor();
            }
            else
            {
                emit toggleEditorActions(true);
            }
        }
        else
        {
            emit toggleEditorActions(true);
        }
        manageInterfaceForCalls(0);
    }
}

void AINGTALivePallete::onExecutingEditedCommandsFinished()
{
    showProgressBar(false);
}

void AINGTALivePallete::onSearchWidgetOKClicked()
{
    subscribeAndAddParameters(_pGenSearchWdgt->getSelectedItemList());
    _pGenSearchWdgt->hide();
}

void AINGTALivePallete::subscribeAndAddParameters(QList<AINGTAICDParameter> &parameterList, QMap<QString,QString> &iParamInfo, QMap<QString, QString> &iGetParamInfo)
{
    QMap<QString,QVariantMap> args = getSubscribeParamsVariantList(parameterList);

    // Subscribe request is sent individually for each application type in the parameterlist.
    foreach(QVariantMap arg, args.values())
    {
        QVariantList arguments;
        arguments<<QVariant(arg);
        emit sendBISGRequest(arguments,SUBSCRIBE_PARAM_LIST,nullptr,nullptr);
    }

    foreach(AINGTAICDParameter selectedItem, parameterList)
    {
        if(selectedItem.getDirection().compare("INPUT",Qt::CaseInsensitive) == 0)
        {
            if (!_SetParamList.contains(selectedItem))
            {
                selectedItem.setValue(selectedItem.getMinValue().isEmpty() ? "0" : selectedItem.getMinValue());

                QTableWidgetItem* param = new QTableWidgetItem(selectedItem.getTempParamName());
                QTableWidgetItem* fv = new QTableWidgetItem("-");
                QComboBox* fs_ssm = new QComboBox();
                fs_ssm->insertItems(0,_SSMList);
                QTableWidgetItem* lstFrcdValue = new QTableWidgetItem(QString());
                QTableWidgetItem* frcdTime = new QTableWidgetItem(QString());

                if(!iParamInfo.isEmpty())
                {
                    //Updating the table with imported values
                    if(iParamInfo.contains(selectedItem.getTempParamName()))
                    {
                        QStringList itemInfo = iParamInfo.value(selectedItem.getTempParamName()).split("·");
                        fv->setText(itemInfo.first());
                        selectedItem.setValue(itemInfo.first());
                        fs_ssm->setCurrentIndex(itemInfo.last().isEmpty() ? 0 : _SSMList.indexOf(itemInfo.last()));

                    }
                }
                selectedItem.setSSMValue(fs_ssm->currentText());

                fv->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                param->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                lstFrcdValue->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                frcdTime->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled | Qt::ItemIsSelectable);


                _SetParamList.append(selectedItem);
                int rowCount = _SetParamList.count();
                ui->parameterTW->setRowCount(rowCount);
                ui->parameterTW->setItem(rowCount -1,0,param);
                ui->parameterTW->setItem(rowCount -1,1,fv);
                ui->parameterTW->setCellWidget(rowCount -1,2,fs_ssm);
                ui->parameterTW->setItem(rowCount -1,3,lstFrcdValue);
                ui->parameterTW->setItem(rowCount -1,4,frcdTime);
            }
            else if(!iParamInfo.isEmpty() && _SetParamList.contains(selectedItem))
            {
                //case: If the parameter is present already and iParamInfo exist(while importing), update the row with imported param values
                for(int i=0; i < ui->parameterTW->rowCount() ; i++)
                {
                    if(selectedItem.getName().compare(ui->parameterTW->item(i,0)->text()) == 0)
                    {
                        QStringList itemInfo = iParamInfo.value(selectedItem.getTempParamName()).split("·");
                        ui->parameterTW->item(i,1)->setText(itemInfo.first());
                        QComboBox* ssm =dynamic_cast<QComboBox*>(ui->parameterTW->cellWidget(i,2));
                        if (nullptr != ssm)
                        {
                            ssm->setCurrentIndex(itemInfo.last().isEmpty() ? 0 : _SSMList.indexOf(itemInfo.last()));
                        }
                        int index = _SetParamList.indexOf(selectedItem);
                        if(index != -1)
                        {
                            _SetParamList[index].setValue(itemInfo.first());
                            _SetParamList[index].setSSMValue(ssm->currentText());
                        }
                        break;
                    }
                }
            }

            parameterList.removeOne(selectedItem);
        }
    }
    if(parameterList.count() > 0)
    {
        if(_pLivePalleteModel->appendParamToList(parameterList,iGetParamInfo))
        {
            ui->getParameterTV->setModel(nullptr);
            ui->getParameterTV->setModel(_pLivePalleteModel);
        }
    }
}

bool AINGTALivePallete::getICDParameterList(QStringList &iVariableList, QList<AINGTAICDParameter> & oParameterList)
{
    bool rc = false;
    AINGTAAppController* pAppCtrlr= AINGTAAppController::getGTAAppController();
    if(pAppCtrlr)
    {
        rc  = pAppCtrlr->getICDDetails(iVariableList,oParameterList);
    }
    return rc;
}

void AINGTALivePallete::onResponseReceived(QVariant iretVal)
{
    QStringList output;
    if (iretVal.canConvert<QVariantMap>())
    {
        QVariantMap ret = iretVal.toMap();
        QString status = "";
        if (ret.contains("Status") && ret.value("Status").canConvert<QString>())
        {
            //return from subscribe, set param, unsubscribe
            //get the status of the call and manage accordingly
            status = ret.value("Status").toString();
            output<<QString("Status : %1").arg(status);

            if (status.compare("OK",Qt::CaseInsensitive) != 0)
            {
                //the call failed in the bisg server
                ret.remove("Status");

                foreach (QString key, ret.keys())
                {
                    if (key == "Unnamed application")
                    {
                        //call is a subscribe, unsubscribe param list call
                        output<<"Parameter subscrition/unsubscription failed:"<<QString("No 'Name' member in application definition")<<"";
                        break;
                    }
                    else if (key.contains("Failed parameters"))
                    {
                        QString failedParams = ret.value("Failed parameters").toString();
                        output<<"Subscribtion Failed for the following Parameters:"<<failedParams.split(",")<<"";
                        break;
                    }
                    else if (key.contains("Message") && ret.value(key).canConvert<QString>())
                    {
                        //set failed
                        QString SetFailedMessage = ret.value(key).toString();

                        QVariantMap map;
                        QString value;
                        AINGTADebugger::traverseMap(ret.value("Argument").toMap(),map,value);

                        output.clear();
                        output<<"Setting the Parameter Failed"<<QString("Status : %1").arg(status)<<QString("Message : %1").arg(SetFailedMessage)<<QString("Argument : %1").arg(value.replace("\n",""))<<"";
						map.clear(); value.clear();
                        break;
                    }
                    else if (ret.value(key).canConvert<QVariantMap>())
                    {
                        QVariantMap map;
                        QString value;
                        AINGTADebugger::traverseMap(ret.value(key).toMap(),map,value);
                        output<<value;
                        map.clear();value.clear();
                    }
                }
                emit updateOutputWindow(output);
                return;
            }
            else if (ret.contains("NbParameters") && ret.value("NbParameters").canConvert<int>())
            {
                //call is an unsubscribe call
                //unsubcribe is always OK by default
                int paramsUnsubscribed = ret.value("NbParameters").toInt();
                output.clear();
                output<<"Parameters Unsusbcribed"<<QString("Status : %1 \nNumber of Paramters : %2").arg(status,QString::number(paramsUnsubscribed))<<"";
            }
        }

        if (status.isEmpty())
        {
            //the return is for a get param call
            //call is managed with ui->getParameterTV and its loaded model
        }
    }
    else if (iretVal.canConvert<QString>() && iretVal.toString().contains("EEE:"))
    {
        //the request had an error
        output.append(iretVal.toString());
    }
    else
    {
        QVariantMap input;
        input.insert("Result",iretVal);
        QVariantMap map;
        QString value;
        QStringList output;
        AINGTADebugger::traverseMap(input,map,value);
        output<<value;
        emit updateOutputWindow(output);
    }
    emit updateOutputWindow(output);
}

/**
  * Author: Saurav
  * @brief: Creates a bisg subscribe call
  * @ioParamList: list of params to subscribe; parameters are removed if doesn't contain valid Tool Name
  * @return : QMap of keys as application name and values as prepared bisg subscribe call in below format:
  * 	- "Name" = (QString) <OSLApplicationName>
        - <OSLBusName1> = (QVariantMap)
            - "Media" = (QString)
            - <OSLSignalName1>  = (QVariantMap)
                - "ParameterType" = (QString) <OSLSignal1Type>
            - ... other signals ...
        - ... other bus ....
  */
QMap<QString,QVariantMap> AINGTALivePallete::getSubscribeParamsVariantList(QList<AINGTAICDParameter> &ioParamList)
{
    QMap<QString,QVariantMap> oArgs;
    QStringList output;

    foreach (AINGTAICDParameter iParam, ioParamList)
    {
        if ((iParam.getToolName().isEmpty() || iParam.getToolName().compare("NA",Qt::CaseInsensitive) == 0))
        {
            output<<QString("Subscribe of %1 is failed. Tool ID is missing").arg(iParam.getName());
            ioParamList.removeOne(iParam);
            continue;
        }
        QString appName = iParam.getName().split(".").first();
        QVariantMap variantSubs;

        // Check if appliction name is already present in oArgs, if Yes, updated it with new signals else add new entry
        if(oArgs.keys().contains(appName))
        {
            variantSubs = oArgs.value(appName);
        }
        else
        {
            variantSubs.insert("Name",QVariant(QString("%1").arg(iParam.getName().split(".").first())));
        }
        QString busName = iParam.getName().split(".").at(1);
        QVariantMap variantSubsParamType;
        variantSubsParamType.insert("ParameterType",QVariant(QString("%1").arg(iParam.getValueType())));

        QVariantMap variantSubsBusPair;
        // Check if bus name is already present in application name qvariantmap entry, if Yes, updated it with new signals else add new entry
        if(variantSubs.keys().contains(busName))
        {
            variantSubsBusPair = variantSubs.value(busName).toMap();
        }
            variantSubsBusPair.insert("Media",QVariant(QString("%1").arg(iParam.getMedia())));
            variantSubsBusPair.insert(QString("%1").arg(iParam.getName().split(".").last()),variantSubsParamType);
        

        variantSubs.insert(QString("%1").arg(iParam.getName().split(".").at(1)),variantSubsBusPair);

        oArgs.insert(appName,variantSubs);
    }

    //Update on debug window for any errors related to missing Tool ID
    if(!output.isEmpty())
        emit updateOutputWindow(output);

    return oArgs;
}


/**
  * Author: Saurav
  * @brief: Creates a set call
  * @iParamList: AINGTAICDParameter object list signal to set
  * @oArgs: prepared bisg set call passed as output
  */
void AINGTALivePallete::getSetParamVariantList(QList<AINGTAICDParameter> iParamList,QVariantList &oArgs)
{
    QVariantList ags;
    foreach(AINGTAICDParameter iParam, iParamList)
    {
        QVariantMap variant;
        variant.insert("Parameter",QVariant(QString("%1").arg(iParam.getName())));
        variant.insert("FunctionType",QVariant(QString("%1").arg(iParam.getValueType())));
        variant.insert("Status",iParam.getSSMValue());

        //depending on function type, the map needs to be created
        QString type = iParam.getValueType();
        QVariantMap variantvalue;

        QString eqnvalue = iParam.getValue();
        eqnvalue = eqnvalue.split("{").last().split("}").first();
        foreach (QString part,eqnvalue.split(","))
        {
            QString key = part.split(":").first().trimmed();
            QString value = part.split(":").last().trimmed();
            variantvalue.insert(key,value);
        }

        variant.insert("FunctionValue",variantvalue);
        ags<<QVariant(variant);
    }
    oArgs<<QVariant(ags);
}

/**
  * Author: Saurav
  * @brief: function enables/disables the Live Pallete UI components.
  * @iVal: true/false decides whether to enable/disabe the UI components
  */
void AINGTALivePallete::manageInterfaceForCalls(int iVal)
{
    bool val = (iVal == 0) ? false : ((iVal == 1)? true : false);

    if (sender() == ui->editorEnabler)
    {
        ui->parameterTW->selectionModel()->clearSelection();
        ui->getParameterTV->selectionModel()->clearSelection();
        _pEditorWindow->clearSelectedRows();
        val = (iVal == 0) ? true : ((iVal == 1)? false : true);
        if (val)
        {
            int ret = QMessageBox::warning(this,"Editor Modification!","Changes made in the editor will be sent to relevant tools.\n Changes cannot be reverted.\n The interface will be blocked during the operation.\n Continue?",QMessageBox::Yes,QMessageBox::No);
            if (ret == QMessageBox::Yes)
            {
                disconnect(ui->editorEnabler,&QSlider::valueChanged,this,&AINGTALivePallete::manageInterfaceForCalls);
                ui->editorEnabler->setValue(0);
                connect(ui->editorEnabler,&QSlider::valueChanged,this,&AINGTALivePallete::manageInterfaceForCalls);

                showProgressBar(true);
                _pEditorWindow->removeEmptyRows();
                _pEditorWindow->clearSelectedRows();
                onUpdateEditorSignal();
                _pEditorWindow->getElement()->updateCommandInstanceName(QString());
                emit executeAndAnalyzeElement();
                _pEditorWindow->setEnabled(!val);
                return;
            }
            else
            {
                disconnect(ui->editorEnabler,&QSlider::valueChanged,this,&AINGTALivePallete::manageInterfaceForCalls);
                ui->editorEnabler->setValue(1);
                connect(ui->editorEnabler,&QSlider::valueChanged,this,&AINGTALivePallete::manageInterfaceForCalls);
				val = false;
            }
        }
        _pEditorWindow->setEnabled(!val);

    }
    else
    {
        ui->editorEnabler->setEnabled(val);
        _pEditorWindow->setEnabled(val);
        if (ui->editorEnabler->value() == 0)
        {
            _pEditorWindow->setEnabled(false);
        }
    }

    ui->setPB->setEnabled(val);
    ui->parameterTW->setEnabled(val);
    ui->getParameterTV->setEnabled(val);
//    ui->complexPB->setEnabled(val);
}

/**
  * Author: Swarup
  * @brief: slot that receives to display context menu on right click
  * @iPos: specifies the point where right click has been done
  */
 void AINGTALivePallete::displayContextMenu(const QPoint &iPos)
 {
	 QWidget * pSourceWidget = (QWidget*)sender();

     if(ui->parameterTW->rowCount() > 0)
     {
         QModelIndexList indexList = ui->parameterTW->selectionModel()->selectedRows();
         if(pSourceWidget != NULL && pSourceWidget == ui->parameterTW && _pContextMenu != NULL && indexList.count() > 0)
         {
             //this can be put as 3 actions because they are added the same way in createContextmenu
             QAction* pActionForceValue = _pContextMenu->actions().at(0);
             QAction* pActionForceMultipleValues = _pContextMenu->actions().at(1);
             QAction* pActionForceAllValues = _pContextMenu->actions().at(2);
             if(indexList.count() > 0  && indexList.count() <= 1)
             {
                 pActionForceValue->setEnabled(true);
                 pActionForceMultipleValues->setEnabled(false);
                 pActionForceAllValues->setEnabled(true);
             }
             else if(indexList.count() > 1)
             {
                 pActionForceValue->setEnabled(false);
                 pActionForceMultipleValues->setEnabled(true);
                 pActionForceAllValues->setEnabled(true);
             }
             else
             {
                 pActionForceValue->setEnabled(false);
                 pActionForceMultipleValues->setEnabled(false);
                 pActionForceAllValues->setEnabled(true);
             }
             _pContextMenu->exec(ui->parameterTW->viewport()->mapToGlobal(iPos));

         }
     }
 }

 void AINGTALivePallete::createContextMenu()
 {
     _pContextMenu = new QMenu(this);
     _pAcquisitionContextMenu = new QMenu(this);

     QAction* pActionForceValue = new QAction("Force value", this);
     _pContextMenu->addAction(pActionForceValue);

     QAction* pActionForceMultipleValues = new QAction("Force Selected", this);
     _pContextMenu->addAction(pActionForceMultipleValues);

     QAction* pActionForceAllValues = new QAction("Force All", this);
     _pContextMenu->addAction(pActionForceAllValues);

     QMenu* pUpdateSignalRangeMenu = new QMenu("Update Signal Min/Max Range", _pAcquisitionContextMenu);
     QAction* pUpdate = new QAction("Update Parameter Range",pUpdateSignalRangeMenu);

     QLabel *minValueLB = new QLabel("Enter min value:",pUpdateSignalRangeMenu);
     QFont fnt("Times", 10);
     minValueLB->setFont(fnt);
     QWidgetAction* pLblMinActn = new QWidgetAction(_pAcquisitionContextMenu);
     pLblMinActn->setDefaultWidget(minValueLB);

     QDoubleSpinBox* minValueSB = new QDoubleSpinBox();
     minValueSB->setMaximum(9999);
     minValueSB->setMinimum(-9999);
     QWidgetAction* pWdgtMinActn = new QWidgetAction(_pAcquisitionContextMenu);
     pWdgtMinActn->setDefaultWidget(minValueSB);

     QLabel *maxValueLB = new QLabel("Enter max value:",pUpdateSignalRangeMenu);
     maxValueLB->setFont(fnt);
     QWidgetAction* pLblMaxActn = new QWidgetAction(_pAcquisitionContextMenu);
     pLblMaxActn->setDefaultWidget(maxValueLB);

     QDoubleSpinBox* maxValueSB = new QDoubleSpinBox();
     maxValueSB->setMaximum(9999);
     maxValueSB->setMinimum(-9999);
     QWidgetAction* pWdgtMaxActn = new QWidgetAction(this);
     pWdgtMaxActn->setDefaultWidget(maxValueSB);

     pUpdateSignalRangeMenu->addAction(pLblMaxActn);
     pUpdateSignalRangeMenu->addAction(pWdgtMaxActn);

     pUpdateSignalRangeMenu->addAction(pLblMinActn);
     pUpdateSignalRangeMenu->addAction(pWdgtMinActn);

     pUpdateSignalRangeMenu->addSeparator();
     pUpdateSignalRangeMenu->addAction(pUpdate);

     _pAcquisitionContextMenu->addMenu(pUpdateSignalRangeMenu);


     connect(pActionForceMultipleValues,SIGNAL(triggered()),this,SLOT(onForceValueTriggered()));
     connect(pActionForceValue,SIGNAL(triggered()),this,SLOT(onForceValueTriggered()));
     connect(pActionForceAllValues,SIGNAL(triggered()),this,SLOT(onForceValueTriggered()));
     connect(pUpdate,SIGNAL(triggered()),this,SLOT(onUpdateSignalRangeTriggered()));
 }

 /**
   * Author: Swarup
   * @brief: function that manages forcing value on selecting the appropriate action
   */
 void AINGTALivePallete::onForceValueTriggered()
 {
     QAction *pAction = dynamic_cast<QAction*>(sender());

     QVariantList args;
     QList<AINGTAICDParameter> selectedItems;

     //force all the parameters in the table for a force all option
     if (nullptr != pAction && pAction->text() == "Force All")
     {
         ui->parameterTW->selectAll();
     }

     QTableWidgetItem* param;
     QModelIndexList selectedRowList = ui->parameterTW->selectionModel()->selectedRows();
     foreach(QModelIndex selRow, selectedRowList)
     {
         param = ui->parameterTW->item(selRow.row(),0);
         for(int i=0; i<_SetParamList.count() ; i++)
         {
             if(_SetParamList[i].getTempParamName() == param->text())
             {
                 _SetParamList[i].setValue(ui->parameterTW->item(selRow.row(),1)->text());
                 QComboBox* ssm =dynamic_cast<QComboBox*>(ui->parameterTW->cellWidget(selRow.row(),2));
                 if (nullptr != ssm)
                 {
                     ui->parameterTW->item(selRow.row(),3)->setText(QString("%1·%2").arg(_SetParamList[i].getValue(),ssm->currentText()));
                     _SetParamList[i].setSSMValue(ssm->currentText());
                 }
                 else
                 {
                    ui->parameterTW->item(selRow.row(),3)->setText(_SetParamList[i].getValue());
                    _SetParamList[i].setSSMValue(ACT_FSSSM_NO);
                 }
                 ui->parameterTW->item(selRow.row(),4)->setText(QDateTime::currentDateTime().toString());
                 selectedItems.append(_SetParamList[i]);
             }
         }
     }
	 
     QList<AINGTACommandBase*> cmdList;
     cmdList.append(AINGTALivePalleteCommands::manageSetListfromGenerationWindow(selectedItems));

     QList<int> indexList;
	 int indexRow = -1;
	 int editorRowCount = _pEditorWindow->getElement()->getAllChildrenCount();
	 for (int i = 0; i < editorRowCount; i++)
	 {
		 if (_pEditorWindow->getCommandForSelectedRow(i))
		 {
			 indexRow = i;
			 continue;
		 }
	 }
     indexList.append(indexRow + 1);
     _pEditorWindow->insertCommands(indexList,cmdList,true);

     showProgressBar(true);
     emit executeAndAnalyzeElement();
 }

void AINGTALivePallete::keyPressEvent ( QKeyEvent * event )
{
    if (event->key()==Qt::Key_Delete  && (ui->getParameterTV->hasFocus() || ui->parameterTW->hasFocus()))
    {
        if(ui->getParameterTV->hasFocus() && ui->getParameterTV->model())
        {
            QModelIndexList selectedRows = ui->getParameterTV->selectionModel()->selectedRows();
            if(selectedRows.isEmpty() )
                return ;
            else
            {
                QStringList selectedParams;
                foreach (QModelIndex index, selectedRows)
                {
                     selectedParams.append(ui->getParameterTV->model()->data(index).toString());
                }

				//DEV: this call manages an unsubscrition of 1 parameter at a time. to implement for multi-select unsubscrition together
                 QList<AINGTAICDParameter> items;
                foreach (QString param, selectedParams)
                {
                    foreach(AINGTAICDParameter parameter, _pLivePalleteModel->getParamList())
                    {
                        if(parameter.getTempParamName() == param)
                        {
                            items.append(parameter);
                        }
                    }
                }
                QMap<QString,QVariantMap> args =  getSubscribeParamsVariantList(items);
                //un-subscribe the parameter for further calls
                foreach(QVariantMap arg, args.values())
                {
                    QVariantList arguments;
                    arguments<<QVariant(arg);
                    emit sendBISGRequest(arguments,UNSUBSCRIBE_PARAMLIST,nullptr,nullptr);
                }

                ui->getParameterTV->setModel(nullptr);
                _pLivePalleteModel->clearSelectedRows(selectedParams);
                 ui->getParameterTV->clearSelection();
                 ui->getParameterTV->setModel(_pLivePalleteModel);
            }
        }
        else if (ui->parameterTW->hasFocus() && ui->parameterTW->selectionModel())
        {
            QModelIndexList selectedRows = ui->parameterTW->selectionModel()->selectedRows();
            ui->parameterTW->selectionModel()->clearSelection();
            if(selectedRows.isEmpty() )
                return ;
            else
            {

                QList<AINGTAICDParameter> items;
                foreach (QModelIndex index, selectedRows)
                {
                    for(int p=0;p< _SetParamList.count();p++)
                    {
                        AINGTAICDParameter parameter = _SetParamList.at(p);
                        if(parameter.getTempParamName() == ui->parameterTW->item(index.row(),0)->text())
                        {
                            items.append(parameter);
                            _SetParamList.removeAll(parameter);
                        }
                    }
                }
                QMap<QString,QVariantMap> args = getSubscribeParamsVariantList(items);
                //un-subscribe the parameter for further calls
                foreach(QVariantMap arg, args.values())
                {
                    QVariantList arguments;
                    arguments<<QVariant(arg);
                    emit sendBISGRequest(arguments,UNSUBSCRIBE_PARAMLIST,nullptr,nullptr);
                }
                qSort(selectedRows);
                for(int i=selectedRows.count() -1 ; i>=0 ; i--)
                {
                    ui->parameterTW->removeRow(selectedRows.at(i).row());
                }
            }
        }
    }

    QWidget::keyPressEvent(event);
}

void AINGTALivePallete::onUpdateEditorSignal()
{
    AINGTAElement *pElement = _pEditorWindow->getElement();

    QStringList variableList, checkUuidRep;
    //checkUuidRep.append(pElement->getUuid());
    getVariableList(pElement, variableList, checkUuidRep);

    QList<AINGTAICDParameter> paramList;
    getICDParameterList(variableList, paramList);
    subscribeAndAddParameters(paramList);
}

void AINGTALivePallete::getVariableList(const AINGTAElement *pElement, QStringList &oVariableList, QStringList &iCheckUuidRep)
{
    QString elemUuid = pElement->getUuid();

    if(!iCheckUuidRep.contains(elemUuid))
    {
        int cmdCnt = pElement->getDirectChildrenCount();
        for(int i =0; i < cmdCnt; i++)
        {
            AINGTACommandBase* pCmd = NULL;
            bool rc = pElement->getDirectChildren(i, pCmd);

            if(rc && pCmd != NULL && !pCmd->getReadOnlyState())
            {
                //Can check for type of command and take necesary actions

                //Check for function call
                if(pCmd->hasReference())
                {
                    AINGTAActionCall *pCallCmd = dynamic_cast<AINGTAActionCall*>(pCmd);
                    if(pCallCmd != NULL)
                    {
                        AINGTAAppController* pAppCtrl = AINGTAAppController::getGTAAppController();
                        AINGTAElement *pCallElement = NULL;
                        if(pAppCtrl)
                        {
                            bool rc = pAppCtrl->getElementFromDocument(pCallCmd->getRefrenceFileUUID(),pCallElement,true);
                            if(rc && pCallElement != NULL)
                            {
                                if(!iCheckUuidRep.contains(elemUuid))
                                {
                                    iCheckUuidRep.append(elemUuid);
                                    getVariableList(pCallElement,oVariableList,iCheckUuidRep);

                                    iCheckUuidRep.removeOne(elemUuid);
                                }
                            }
                        }
                    }
                }
                //for set command or other commands
                else
                {
                    oVariableList.append(pCmd->getVariableList());
                }
            }
        }
    }
}

QVariant AINGTALivePallete::setParameterValue(const QList<AINGTAICDParameter> &iparamList,QTimer* ipTimer)
{
    bool rc = false;
    _mutex.lock();
	foreach(AINGTAICDParameter param, iparamList)
	{
		for (int i = 0; i < ui->parameterTW->rowCount(); i++)
		{
			if (param.getName().compare(ui->parameterTW->item(i, 0)->text()) == 0)
			{
                //value is received in structured format
                QString value = param.getValue();
                value.replace("FunctionValue:{Value:","").replace("}","");

                QString ssmvalue = param.getSSMValue();
                ui->parameterTW->item(i, 1)->setText(value);
				QComboBox* ssm = dynamic_cast<QComboBox*>(ui->parameterTW->cellWidget(i, 2));
				if (nullptr != ssm)
				{
                    ssm->setCurrentIndex(ssmvalue.isEmpty() ? 0 : _SSMList.indexOf(ssmvalue));
				}
                ui->parameterTW->item(i, 3)->setText(value);
				ui->parameterTW->item(i, 4)->setText(QDateTime::currentDateTime().toString());

                param.setTempParamName(param.getName());
                int index = _SetParamList.indexOf(param);
				if (index != -1)
				{
                    _SetParamList[index].setValue(value);
					_SetParamList[index].setSSMValue(ssm->currentText());
				}
				break;
			}
        }
	}
    QVariantList args;
    getSetParamVariantList(iparamList,args);
    QNetworkReply* reply = nullptr;
    _blockingResponse = QString(BLOCKED_CALL);
    reply = (emit sendBlockingBISGRequest(args,SET_PARAM));
    if (nullptr != reply)
    {
        if (nullptr != ipTimer)
        {
            ipTimer->stop();
            ipTimer->start(ipTimer->interval());

            while (reply->isFinished() && ipTimer->remainingTime() > 0);
            while (_blockingResponse == BLOCKED_CALL && ipTimer->remainingTime() > 0);
            if (ipTimer->remainingTime() == 0)
            {
                //stop any further processing or uploading of this request
                reply->abort();
            }
        }
        else
        {
            while (reply->isFinished());
            while (_blockingResponse == BLOCKED_CALL);
        }
        rc = true;
    }
    _mutex.unlock();
    return _blockingResponse;
}

bool AINGTALivePallete::getParameterValue(AINGTAICDParameter &oParam)
{
    bool rc = false;
    _mutex.lock();
    QList<AINGTAICDParameter> paramList = _pLivePalleteModel->getParamList();
    for(int i=0 ; i<paramList.count() ; i++)
    {
        if(oParam.getName().compare(paramList.at(i).getName()) == 0)
        {
            rc = true;
            oParam.setValue(paramList.at(i).getValue());
            oParam.setSSMValue(paramList.at(i).getSSMValue());
            oParam.setValueType(paramList.at(i).getValueType());
            break;
        }
    }
    //check in set list also if value is not available in get list
    if (!rc)
    {
        for(int i=0 ; i<_SetParamList.count() ; i++)
        {
            if(oParam.getName().compare(_SetParamList.at(i).getName()) == 0)
            {
                rc = true;
                oParam.setValue(_SetParamList.at(i).getValue());
                oParam.setSSMValue(_SetParamList.at(i).getSSMValue());
                oParam.setValueType(_SetParamList.at(i).getValueType());
                break;
            }
        }
    }
    _mutex.unlock();
    return rc;
}

void AINGTALivePallete::showProgressBar(bool iVal)
{
    ui->progressBar->setMaximum(0);
    ui->progressBar->setMinimum(0);
    if (iVal)
    {
        ui->progressBar->show();
        ui->editorEnabler->setEnabled(!iVal);
    }
    else
    {
        ui->progressBar->hide();

        if (ui->editorEnabler->value() == 0)
        {
            ui->editorEnabler->setEnabled(!iVal);
        }
    }
    ui->setPB->setEnabled(!iVal);
//    ui->complexPB->setEnabled(!iVal);
    ui->parameterTW->setEnabled(!iVal);
    ui->getParameterTV->setEnabled(!iVal);
}

QList<AINGTAICDParameter> AINGTALivePallete::getParameterList()
{
    QList<AINGTAICDParameter> parameterList;
    parameterList.append(_SetParamList);
    if(_pLivePalleteModel)
        parameterList.append(_pLivePalleteModel->getParamList());
    return parameterList;
}
void AINGTALivePallete::setParameterList(QMap<QString,QString> &isetVarList, QMap<QString,QString> &igetVarList)
{
    QList<AINGTAICDParameter> paramterList;
    QStringList variableList;
    variableList.append(isetVarList.keys());
    variableList.append(igetVarList.keys());
    if(!getICDParameterList(variableList,paramterList))
    {
        emit updateOutputWindow(QStringList("Few Paramters could not be imported"));
    }
    subscribeAndAddParameters(paramterList, isetVarList, igetVarList);
}

QVariant AINGTALivePallete::sendBlockingExtToolRequest(QString iIP, int iPort,QVariantList iArgs,QString iMethodname, const QTimer* ipTimer)
{
    _mutex.lock();
    QNetworkReply* reply = nullptr;
    _blockingResponse = QString(BLOCKED_CALL);
    reply = emit sendExtToolRequest(iIP,iPort,iArgs,iMethodname,nullptr,nullptr);
    if (nullptr != reply)
    {
        if (nullptr != ipTimer)
        {
            while (reply->isFinished() && ipTimer->remainingTime() > 0);
            while (_blockingResponse == BLOCKED_CALL && ipTimer->remainingTime() > 0);
        }
        else
        {
            while (reply->isFinished());
            while (_blockingResponse == BLOCKED_CALL);
        }
    }
    _mutex.unlock();
    return _blockingResponse;
}

void AINGTALivePallete::onBlockingResponseReceived(QVariant iretVal)
{
    _blockingResponse = iretVal;
}

QPointer<AINGTAEditorWindow> AINGTALivePallete::getLiveEditorWindow() const
{
    return _pEditorWindow;
}

void AINGTALivePallete::displayAcquisitionContextMenu(const QPoint &iPos)
{
    QWidget * pSourceWidget = (QWidget*)sender();

    if(ui->getParameterTV->model()->rowCount() > 0)// has focus
    {
        QModelIndexList selectedRows = ui->getParameterTV->selectionModel()->selectedRows();
        if(pSourceWidget != NULL && pSourceWidget == ui->getParameterTV && _pAcquisitionContextMenu != nullptr && selectedRows.count() == 1)
        {
            _pAcquisitionContextMenu->exec(ui->getParameterTV->viewport()->mapToGlobal(iPos));
        }
    }
}

void AINGTALivePallete::onUpdateSignalRangeTriggered()
{
	QStringList varList;
    QList<AINGTAICDParameter> paramList;
    if (ui->getParameterTV->selectionModel())
    {
        if (!ui->getParameterTV->selectionModel()->selectedRows().isEmpty())
        {
            QString parameter = ui->getParameterTV->selectionModel()->selectedRows().first().data().toString();
            varList.append(parameter);
        }
    }
	getICDParameterList(varList, paramList);

	double minRange, maxRange;
	QAction* pAction = dynamic_cast<QAction*>(sender());

	if (pAction->text() == "Update Parameter Range")
	{
		QMenu* pMenu = dynamic_cast<QMenu*>(pAction->parent());
		if (pMenu)
		{
			QWidgetAction* maxWgtAction = dynamic_cast<QWidgetAction*>(pMenu->actions().at(1));
			QWidgetAction* minWgtAction = dynamic_cast<QWidgetAction*>(pMenu->actions().at(3));
			if (maxWgtAction)
			{
				QDoubleSpinBox* maxSB = dynamic_cast<QDoubleSpinBox*>(maxWgtAction->defaultWidget());
				if (maxSB)
				{
					maxRange = maxSB->value();
					maxSB->setValue(paramList.first().getMaxValue().toDouble());
				}
			}

			if (minWgtAction)
			{
				QDoubleSpinBox* minSB = dynamic_cast<QDoubleSpinBox*>(minWgtAction->defaultWidget());
				if (minSB)
				{
					minRange = minSB->value();
					minSB->setValue(paramList.first().getMinValue().toDouble());
				}
			}
		}
	}

	ui->getParameterTV->selectionModel()->clearSelection();
    foreach (AINGTAICDParameter parameter,paramList)
        emit updateSignalRange(parameter.getName(), QString::number(minRange), QString::number(maxRange));
}
