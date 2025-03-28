#include "GTACheckWidget.h"
#include "ui_GTACheckWidget.h"
#include "GTACheckAudioWidget.h"
#include "GTACheckECAMDisplayWidget.h"
#include "GTACheckFWCWarningWidget.h"
#include "GTACheckFWCProcedureWidget.h"
#include "GTACheckValueWidget.h"
#include "GTACheckBiteMessageWidget.h"
#include "GTACommandBuilder.h"
#include "GTAGenSearchWidget.h"
#include <QMessageBox>
#include "GTACheckFwcWarning.h"
#include "GTAICDParameter.h"
#include "GTACheckFwcProcedure.h"
#include "GTACheckFwcDisplay.h"
#include "GTACheckVisualDisplay.h"
#include "GTACheckManualCheck.h"
#include "GTACheckBase.h"
#include "GTACheckValue.h"
#include "GTACheckBiteMessage.h"
#include "GTACheckAudioAlarm.h"
#include "GTACheckEcamDisplay.h"
#include "GTACommandVariant.h"
#include "GTACommandVariant.h"
#include "GTAUtil.h"
#include "GTAActionCmdCommentWidget.h"
#include "GTAParamValidator.h"
#include "GTADumpListWidget.h"
#include "GTACheckRefreshWidget.h"
#include "GTAParamValidator.h"
#include "GTAParamValueValidator.h"

GTACheckWidget::GTACheckWidget(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::GTACheckWidget)
{
    ui->setupUi(this);

    _pAudioWgt = new GTACheckAudioWidget();
    _pEcamDispWgt = new GTACheckECAMDisplayWidget();
    _pFwcWarnWgt = new GTACheckFWCWarningWidget();
    _pFwcProcWgt = new GTACheckFWCProcedureWidget();
    _pValueWgt = new GTACheckValueWidget();
    _pBiteMsgWgt = new GTACheckBiteMessageWidget();
    _pDisplayTE = new QTextEdit();
    _pCommentWgt = new GTAActionCmdCommentWidget();
    _pCommentWgt->showImagePanel(false);

    _pRequirementWdgt = new GTACmdRequirementWidget();
    _pRequirementWdgt->hide();
    _pDumpLstWdgt = new GTADumpListWidget();
    _pRefreshWdgt = new GTACheckRefreshWidget();

    ui->CheckStackSW->addWidget(_pAudioWgt);
    ui->CheckStackSW->addWidget(_pEcamDispWgt);
    ui->CheckStackSW->addWidget(_pFwcWarnWgt);
    ui->CheckStackSW->addWidget(_pFwcProcWgt);
    ui->CheckStackSW->addWidget(_pValueWgt);
    ui->CheckStackSW->addWidget(_pBiteMsgWgt);
    ui->CheckStackSW->addWidget(_pDisplayTE);
    ui->CheckStackSW->addWidget(_pRefreshWdgt);


    connect(_pBiteMsgWgt,SIGNAL(searchMessage(QLineEdit*&)),this,SLOT(onSearchPBClick(QLineEdit*&)));
    connect(_pFwcProcWgt,SIGNAL(searchProcedure(QLineEdit*&)),this,SLOT(onSearchPBClick(QLineEdit*&)));
    connect(_pDumpLstWdgt,SIGNAL(searchObject(QLineEdit*&)),this,SLOT(onSearchPBClick(QLineEdit*&)));
    
    connect(_pFwcWarnWgt,SIGNAL(searchMessage(QLineEdit*&)),this,SLOT(onSearchPBClick(QLineEdit*&)));
    connect(_pFwcWarnWgt,SIGNAL(searchParameter(QLineEdit*&)),this,SLOT(onSearchPBClick(QLineEdit*&)));

    connect(_pValueWgt,SIGNAL(searchParameter(QLineEdit*&)),this,SLOT(onSearchPBClick(QLineEdit*&)));
    connect(_pRefreshWdgt,SIGNAL(searchParameter(QLineEdit*&)),this,SLOT(onSearchPBClick(QLineEdit*&)));
    
    connect(_pAudioWgt,SIGNAL(searchMessage(QLineEdit*&)),this,SLOT(onSearchPBClick(QLineEdit*&)));
    //connect(_pCommentWgt,SIGNAL(searchImage(QLineEdit*&)),this,SLOT(onSearchPBClick(QLineEdit*&)));
    connect(ui->CommentPB,SIGNAL(clicked()),_pCommentWgt,SLOT(show()));

    connect(ui->RequirementsPB,SIGNAL(clicked()),_pRequirementWdgt,SLOT(show()));

    connect( ui->OnFailCB,SIGNAL(currentIndexChanged(QString)),this,SLOT(setDumpPBStatus(QString)));
    connect(ui->DumpListPB,SIGNAL(clicked()),_pDumpLstWdgt,SLOT(show()));


	// Manage Confirmation Time and TimeOut Value
	connect(ui->ConfTimeLE, SIGNAL(textChanged(const QString &)), this, SLOT(evaluateConfTime(const QString &)));
	connect(ui->TimeOutLE, SIGNAL(textChanged(const QString &)), this, SLOT(evaluateConfTime(const QString &)));
	connect(ui->ConfTimeUnitCB, SIGNAL(currentTextChanged(const QString &)), this, SLOT(evaluateConfTime(const QString &)));
	connect(ui->onTimeUnitCB, SIGNAL(currentTextChanged(const QString &)), this, SLOT(evaluateConfTime(const QString &)));

    ui->DumpListPB->setDisabled(true);

    //   QList<QString> checkList; checkList<<CHK_VALUE<<CHK_FWC_WARN<<CHK_FWC_PROC<<CHK_FWC_DISP<<CHK_BITE_MSG<<CHK_ECAM_DISP<<CHK_AUDIO_ALRM<<CHK_VISU_DISP<<CHK_MANUAL_CHECK<<CHK_REFRESH;

    _CheckWidgetMap.insert(CHK_VALUE,_pValueWgt);
    _CheckWidgetMap.insert(CHK_FWC_WARN,_pFwcWarnWgt);
    _CheckWidgetMap.insert(CHK_FWC_PROC,_pFwcProcWgt);
    _CheckWidgetMap.insert(CHK_FWC_DISP,_pEcamDispWgt);
    _CheckWidgetMap.insert(CHK_BITE_MSG,_pBiteMsgWgt);
    _CheckWidgetMap.insert(CHK_ECAM_DISP,_pEcamDispWgt);
    _CheckWidgetMap.insert(CHK_AUDIO_ALRM,_pAudioWgt); 
    _CheckWidgetMap.insert(CHK_REFRESH,_pRefreshWdgt);

    _CheckEnumToStringMap.insert(GTACheckBase::VALUE,CHK_VALUE);
    _CheckEnumToStringMap.insert(GTACheckBase::FWC_WARNING,CHK_FWC_WARN);
    _CheckEnumToStringMap.insert(GTACheckBase::FWC_PROCEDURE,CHK_FWC_PROC);
    _CheckEnumToStringMap.insert(GTACheckBase::FWC_DISPLAY,CHK_FWC_DISP);
    _CheckEnumToStringMap.insert(GTACheckBase::BITE_MESSAGE,CHK_BITE_MSG);
    _CheckEnumToStringMap.insert(GTACheckBase::ECAM_DISPLAY,CHK_ECAM_DISP);
    _CheckEnumToStringMap.insert(GTACheckBase::AUDIO_ALARM,CHK_AUDIO_ALRM);
    _CheckEnumToStringMap.insert(GTACheckBase::VISUAL_DISPLAY,CHK_VISU_DISP);
    _CheckEnumToStringMap.insert(GTACheckBase::MANUAL_CHECK,CHK_MANUAL_CHECK);
    _CheckEnumToStringMap.insert(GTACheckBase::REFRESH,CHK_REFRESH);

    _SearchButtonEditorMap.insert(ui->SearchConfirmationPB, ui->ConfTimeLE);
    _SearchButtonEditorMap.insert(ui->SearchTimeoutPB, ui->TimeOutLE);

    
    foreach(QPushButton * pSearchPB, _SearchButtonEditorMap.keys())
    {
       connect(pSearchPB, SIGNAL(clicked()), this, SLOT(onSearchTimePBClick()));
    } 
        
    QStringList lstTimeUnits;
    lstTimeUnits<<ACT_TIMEOUT_UNIT_SEC<<ACT_TIMEOUT_UNIT_MSEC;
    ui->onTimeUnitCB->clear();
    ui->onTimeUnitCB->addItems(lstTimeUnits);

    ui->ConfTimeUnitCB->clear();
    ui->ConfTimeUnitCB->addItems(lstTimeUnits);
    

    QStringList lstPrecUnits;
    ui->onPrecisonUnitCB->clear();
    lstPrecUnits<<ACT_PRECISION_UNIT_PERCENT<<ACT_PRECISION_UNIT_VALUE;
    ui->onPrecisonUnitCB->addItems(lstPrecUnits);

    ui->CheckStackSW->setCurrentWidget(_pValueWgt);
    connect(ui->CheckTypeCB,SIGNAL(currentIndexChanged(QString)),this,SLOT(onCheckSelection(QString)));
    connect(ui->OKPB,SIGNAL(clicked()),this,SLOT(onOKPBClicked()));
    connect(ui->CancelPB,SIGNAL(clicked()),this,SLOT(hide()));
    connect(ui->CancelPB,SIGNAL(clicked()),this,SIGNAL(setParentFocus()));

    _pSearchWidget = new GTAGenSearchWidget();
    _pSearchWidget->hide();
      
    connect(_pSearchWidget,SIGNAL(okPressed()),this,SLOT(onSearchOKPBClick()));
    connect(_pValueWgt,SIGNAL(waitUntilCheckStatus(bool)),this,SLOT(setTimeOutStatus(bool)));

    //TODO: Once confirmed for removal this should be removed (precision being a part of check value only)
    ui->PrecisionLabel->hide();
    ui->PrecisionLE->hide();
    ui->onPrecisonUnitCB->hide();

    ui->PrecisionLE->setValidator( new QDoubleValidator(0, 100, 4, this));
    ui->TimeOutLE->setValidator( new QDoubleValidator(0, 100, 4, this));

    /*ui->ConfTimeLE->setEnabled(false);
    ui->ConfTimeLB->setEnabled(false);
    ui->ConfTimeUnitCB->setEnabled(false);*/

    connect(ui->ExternalActionCheckBox, SIGNAL(stateChanged(int)), this, SLOT(externalActionCheckboxClicked()));

    ui->ExternalActionComboBox->hide();
    ui->AutoOpenActionCB->hide();
    ui->autoAddIfElse->hide();
}

GTACheckWidget::~GTACheckWidget()
{
    if(_pAudioWgt != NULL)
        delete _pAudioWgt;

    if(_pEcamDispWgt != NULL)
        delete _pEcamDispWgt;

    if(_pFwcWarnWgt != NULL)
        delete _pFwcWarnWgt;

    if(_pFwcProcWgt != NULL)
        delete _pFwcProcWgt;

    if(_pValueWgt != NULL)
        delete _pValueWgt;

    if(_pBiteMsgWgt != NULL)
        delete _pBiteMsgWgt;

    if(_pDisplayTE != NULL)
        delete _pDisplayTE;

    if(_pCommentWgt != NULL)
    {
        delete _pCommentWgt;
        _pCommentWgt = NULL;
    }

    if(_pRequirementWdgt != NULL)
    {
        delete _pRequirementWdgt;
        _pRequirementWdgt = NULL;
    }
    if(_pDumpLstWdgt != NULL)
    {
        delete _pDumpLstWdgt;
        _pDumpLstWdgt =NULL;
    }
    if(_pRefreshWdgt != NULL)
    {
        delete _pRefreshWdgt;
        _pRefreshWdgt =NULL;
    }
        
    if (_pSearchWidget != NULL)
    {
        delete _pSearchWidget;
        _pSearchWidget = NULL;
    }

    delete ui;
}



void GTACheckWidget::setChecksList(const QList<QString> &iCheckList)
{

    ui->CheckTypeCB->clear();
    ui->CheckTypeCB->addItems(iCheckList);
    ui->CheckTypeCB->setCurrentIndex(0);

}

void GTACheckWidget::clickOk() 
{ 
    onOKPBClicked(); 
}

void GTACheckWidget::clickCancel() 
{
    ui->CancelPB->clicked(true);
}

GTACheckWidgetInterface* GTACheckWidget::getCurrentWidget() 
{ 
    return dynamic_cast<GTACheckWidgetInterface*>(ui->CheckStackSW->currentWidget()); 
}

bool GTACheckWidget::setCheckControlWidgetFields(const QList<QPair<QString, QString>>& checkParams)
{
    // default setup
    ui->OnFailCB->setCurrentIndex(0);
    ui->TimeOutLE->setText("3");
    ui->PrecisionLE->setText("0");
    ui->ConfTimeLE->setText("0");

    for (auto param : checkParams)
    {
        if (param.first == "OnFailCB")
        {
            int currentIdx = ui->OnFailCB->findText(param.second);
            if (currentIdx >= 0)
                ui->OnFailCB->setCurrentIndex(currentIdx);
            else
                ui->OnFailCB->setCurrentIndex(0);
        }
        else if (param.first == "TimeOutLE")
        {
            bool paramSecondIsNumber = false;
            param.second.toDouble(&paramSecondIsNumber);
            if (paramSecondIsNumber)
                ui->TimeOutLE->setText(param.second);
            else
                ui->TimeOutLE->setText("3");
        }
        else if (param.first == "PrecisionLE")
        {
            bool paramSecondIsNumber = false;
            param.second.toDouble(&paramSecondIsNumber);
            if (paramSecondIsNumber)
                ui->PrecisionLE->setText(param.second);
            else
                ui->PrecisionLE->setText("0");
        }
        else if (param.first == "ConfTimeLE")
        {
            bool paramSecondIsNumber = false;
            param.second.toDouble(&paramSecondIsNumber);
            if (paramSecondIsNumber)
                ui->ConfTimeLE->setText(param.second);
            else
                ui->ConfTimeLE->setText("3");
        }
        else
        {
            //TODO error handling but probably unecessary because of the deafault setup
        }
    }
    return true;
}

void GTACheckWidget::onCheckSelection(const QString &iSelectedCheck)
{
    if (_pCommentWgt != NULL)
        _pCommentWgt->clear();

    if(_pRequirementWdgt != NULL)
        _pRequirementWdgt->clear();

    showCommonItem();
    _pDumpLstWdgt->clearDumpList();
    ui->TimeOutLabel->setText("With Timeout");


    GTAAppController* pController = GTAAppController::getGTAAppController();
    QStringList autoCompleteWordList = pController->getVariablesForAutoComplete();
    if (iSelectedCheck == CHK_VALUE)
    {
        bool isConfigTime = false;
        isConfigTime = !pController->isGenericSCXMLMode();
        showCommonItem(true,false,true,true,isConfigTime);
    }
    else if (iSelectedCheck == CHK_FWC_WARN)
    {
        showCommonItem(true,false,false,false,false);

    }
    else if (iSelectedCheck == CHK_FWC_PROC)
    {
        showCommonItem(true);
    }
    else if (iSelectedCheck == CHK_FWC_DISP)
    {
        showCommonItem(true,false,true,true);
        ui->TimeOutLabel->setText("After (in Sec)");
    }
    else if (iSelectedCheck == CHK_BITE_MSG)
    {
        showCommonItem(true,false,false,false);
    }
    else if (iSelectedCheck == CHK_ECAM_DISP)
    {
        showCommonItem(true);
    }
    else if (iSelectedCheck == CHK_AUDIO_ALRM)
    {
        showCommonItem(true);
    }
    else if (iSelectedCheck == CHK_VISU_DISP)
    {
        _pDisplayTE->clear();
        ui->CheckStackSW->setCurrentWidget(_pDisplayTE);
        showCommonItem(true);
    }
    else if (iSelectedCheck == CHK_MANUAL_CHECK)
    {
        _pDisplayTE->clear();
        ui->CheckStackSW->setCurrentWidget(_pDisplayTE);
        showCommonItem(true);
    }
    else if (iSelectedCheck == CHK_REFRESH)
    {
        _pRefreshWdgt->clear();
        ui->CheckStackSW->setCurrentWidget(_pRefreshWdgt);
        showCommonItem(true,true,true,false,false);
    }
    if(_CheckWidgetMap.contains(iSelectedCheck))
    {
        GTACheckWidgetInterface * pCheckWidgtIntf = _CheckWidgetMap.value(iSelectedCheck);
        if(pCheckWidgtIntf != NULL)
        {
            //pCheckWidgtIntf->clear();
            ui->CheckStackSW->setCurrentWidget(pCheckWidgtIntf);
            pCheckWidgtIntf->setAutoCompleteItems(autoCompleteWordList);
        }
    }
}
void GTACheckWidget::onOKPBClicked()
{
    bool compileError = false;
    bool wasOpenActionScheduled = false;
    QString iSelectedCheck;
    QString sWidgetErrorMsg="";
    if(ui->CheckTypeCB->currentText() == "")
        compileError = true;
    else iSelectedCheck = ui->CheckTypeCB->currentText();


    _pCheckCommand = NULL;

    QString timeOut,timeUnit,precision, precisionUnit,onFail;
    QString confTime;
    QString confTimeUnit;


    if(! ui->TimeOutLE->isHidden())
        timeOut = ui->TimeOutLE->text();
    if(! ui->PrecisionLE->isHidden())
        precision = ui->PrecisionLE->text();
    if(! ui->OnFailCB->isHidden())
        onFail = ui->OnFailCB->currentText();
    /*if(! ui->OnDisplayCB->isHidden())
        onDisplay = ui->OnDisplayCB->currentText();*/
    if(! ui->onTimeUnitCB->isHidden())
        timeUnit = ui->onTimeUnitCB->currentText();
    if(! ui->onPrecisonUnitCB->isHidden())
        precisionUnit = ui->onPrecisonUnitCB->currentText();

    if (!ui->ConfTimeLE->isHidden())
        confTime = ui->ConfTimeLE->text();
    if(! ui->ConfTimeUnitCB->isHidden())
    {
        confTimeUnit = ui->ConfTimeUnitCB->currentText();
    }

    if(_CheckWidgetMap.contains(iSelectedCheck))
    {
        GTACheckWidgetInterface * pCheckWidgetIntf = _CheckWidgetMap.value(iSelectedCheck);
        if(pCheckWidgetIntf != NULL)
        {
            if(pCheckWidgetIntf->getCommand(_pCheckCommand) && _pCheckCommand != NULL)
            {
                compileError = false;

                if (iSelectedCheck == CHK_FWC_WARN)
                {
                    GTACheckFwcWarning * pCheckCmd = dynamic_cast<GTACheckFwcWarning*>(_pCheckCommand);
                    pCheckCmd->getTimeOut(timeOut, timeUnit);
                    /* if(pCheckCmd != NULL)
                        pCheckCmd->setOnDisplay(onDisplay);*/
                }
                if(iSelectedCheck ==  CHK_BITE_MSG)
                {
                    GTACheckBiteMessage * pCheckCmd = dynamic_cast<GTACheckBiteMessage*>(_pCheckCommand);
                    pCheckCmd->getTimeOut(timeOut, timeUnit);
                }
                else if (iSelectedCheck == CHK_FWC_PROC)
                {
                   dynamic_cast<GTACheckFwcProcedure*>(_pCheckCommand);
                    /* if(pCheckCmd != NULL)
                        pCheckCmd->setOnDisplay(onDisplay);*/
                }
                else if (iSelectedCheck == CHK_FWC_DISP)
                {
                    dynamic_cast<GTACheckFwcDisplay*>(_pCheckCommand);
                    /*if(pCheckCmd != NULL)
                        pCheckCmd->setOnDisplay(onDisplay);*/
                }
                else if (iSelectedCheck == CHK_VALUE)
                {
                    GTACheckValue * pCheckCmd = dynamic_cast<GTACheckValue*>(_pCheckCommand);
                    if(pCheckCmd!= NULL && !pCheckCmd->getWaitUntil())
                    {
                        timeOut = "0";
                    }
                    if(pCheckCmd != NULL && pCheckCmd->getWaitUntil() && !ui->TimeOutLE->isHidden() && !timeOut.startsWith("line"))
                    {
                        
                        double timeOutVal = timeOut.toDouble();
                        if (timeOutVal<=0)
                        {
                            delete pCheckCmd;
                            pCheckCmd = NULL;
                            _pCheckCommand=NULL;
                            sWidgetErrorMsg = "Invalid Wait until, wait until cannot have zero time out.";
                            compileError = true;
                        }
                    }
                    setExternalActionProperties(pCheckCmd);
                }
            }
            else compileError = true;
        }
    }
    else if (iSelectedCheck == CHK_VISU_DISP )
    {

        QString displayValue = _pDisplayTE->toPlainText();
        int txtSize = displayValue.size();
        GTAParamValidator validatorObj;
        if(! displayValue.isEmpty() &&  validatorObj.validate(displayValue,txtSize)==QValidator::Acceptable )
        {
            GTACheckVisualDisplay * pCheckCmd = new GTACheckVisualDisplay();
            pCheckCmd->setValue(displayValue);
            _pCheckCommand = pCheckCmd;
        }
        else
        {
            compileError = true;
            if(!displayValue.isEmpty())
                sWidgetErrorMsg = "Forbidden characters in value field";
        }
    }
    else if (iSelectedCheck == CHK_MANUAL_CHECK)
    {

        QString displayValue = _pDisplayTE->toPlainText();
        int txtSize = displayValue.size();
        GTAParamValidator validatorObj;
        if(! displayValue.isEmpty() &&  validatorObj.validate(displayValue,txtSize)==QValidator::Acceptable)
        {
            GTACheckManualCheck * pCheckCmd = new GTACheckManualCheck();
            pCheckCmd->setValue(displayValue);
            _pCheckCommand = pCheckCmd;
        }
        else
        {
            compileError = true;
            if(!displayValue.isEmpty())
                sWidgetErrorMsg = "Forbidden characters in value field";
            
        }
    }

    if(_pCheckCommand != NULL)
    {
		if (precision == NULL /*&& _pCheckCommand->getAllPrecisionValueFE().size() > 0*/)
		{	
			GTACheckValue * pCheckCmd = dynamic_cast<GTACheckValue*>(_pCheckCommand);
            if(pCheckCmd!=nullptr)
            {
                QStringList precisionsListFE = pCheckCmd->getAllPrecisionValueFE();
                if (precisionsListFE.size() > 0)
                    precision = pCheckCmd->getAllPrecisionValueFE().first();
            }
		}
        _pCheckCommand->setPrecision(precision,precisionUnit);
        _pCheckCommand->setTimeOut(timeOut,timeUnit);
        _pCheckCommand->setConfCheckTime(confTime,confTimeUnit);
        _pCheckCommand->setActionOnFail(onFail);
        if (onFail == ACT_FAIL_STOP)
        {
            QStringList sDumplIst =_pDumpLstWdgt->getDumpList();
            _pCheckCommand->setDumpList(sDumplIst);
        }
    }
    if(compileError)
    {
        QString errMsg = "Provide valid values for all the fields";
        if(!sWidgetErrorMsg.isEmpty())
            errMsg=sWidgetErrorMsg;
        if (this->isVisible())
            QMessageBox::critical(0,"Compilation Error",errMsg,QMessageBox::Ok);
        delete _pCheckCommand ;
        _pCheckCommand = NULL;
    }
    else
    {
        if(_pCommentWgt != NULL && _pCheckCommand != NULL)
            _pCheckCommand->setComment(_pCommentWgt->getComment());

        if(_pRequirementWdgt != NULL && _pCheckCommand != NULL)
            _pCheckCommand->setRequirements(_pRequirementWdgt->getRequirements());

        setCheckId();

        GTACommandVariant variant;
        variant.setCommand(_pCheckCommand);
        _checkCmdVariant.setValue(variant);
        hide();
        emit okPressed();

        if (_openActionWindowScheduled) {
            _openActionWindowScheduled = false;
            wasOpenActionScheduled = true;
            emit openActionWindowScheduled();
        }
    }

    if (!wasOpenActionScheduled) {
        wasOpenActionScheduled = false;
        emit setParentFocus();
    }
}
QString GTACheckWidget::getCheck() const
{
    return QString();
}
QString GTACheckWidget::getTimeOut() const
{
    QString oVal = QString();
    if(! ui->TimeOutLE->isHidden())
        oVal = ui->TimeOutLE->text();

    return oVal;
}
QString GTACheckWidget::getPrecision() const
{
    QString oVal = QString();
    if(! ui->PrecisionLE->isHidden())
        oVal = ui->PrecisionLE->text();

    return oVal;
}
QString GTACheckWidget::getActionOnFail() const
{
    QString oVal = QString();
    if(! ui->OnFailCB->isHidden())
        oVal = ui->OnFailCB->currentText();

    return oVal;
}
QString GTACheckWidget::getDisplay() const
{
    QString oVal = QString();
    /*if(! ui->OnDisplayCB->isHidden())
        oVal = ui->OnDisplayCB->currentText();*/

    return oVal;
}
void GTACheckWidget::onSearchPBClick(QLineEdit *& ipItemToUpdate)
{
    _pCurrentSearchResult = ipItemToUpdate;

    if(_pSearchWidget != NULL)
    {

        QString iSelectedCheck = ui->CheckTypeCB->currentText();

        if(sender() == _pDumpLstWdgt)
            iSelectedCheck = CHK_DUMPLIST_SRCH;

        if(iSelectedCheck == CHK_VALUE || iSelectedCheck==CHK_REFRESH)
        {
            _pSearchWidget->setSearchType(GTAGenSearchWidget::PARAMETER);
        }
        else if (iSelectedCheck == CHK_FWC_WARN)
        {
            _pSearchWidget->setSearchType(GTAGenSearchWidget::kWarning);
        }
        else if (iSelectedCheck == CHK_FWC_PROC)
        {
            _pSearchWidget->setSearchType(GTAGenSearchWidget::FWC);
        }
        else if (iSelectedCheck == CHK_FWC_DISP)
        {
            //_pSearchWidget->setSearchType(GTAGenSearchWidget::FW);
        }
        else if (iSelectedCheck == CHK_BITE_MSG)
        {
            _pSearchWidget->setSearchType(GTAGenSearchWidget::BITE);
        }
        else if (iSelectedCheck == CHK_AUDIO_ALRM)
        {
            _pSearchWidget->setSearchType(GTAGenSearchWidget::AUDIO_RECOG);
        }
        else if (iSelectedCheck == CHK_DUMPLIST_SRCH)
        {
            _pSearchWidget->setSearchType(GTAGenSearchWidget::DUMP_LIST);
        }
        _pSearchWidget->show();
    }
}

void GTACheckWidget::onSearchTimePBClick()
{
    
    if (sender() == ui->SearchConfirmationPB)
    {
        _pCurrentSearchResult = ui->ConfTimeLE;
    }
    else if (sender() == ui->SearchTimeoutPB)
    {
        _pCurrentSearchResult = ui->TimeOutLE;
    }
    
    if (_pSearchWidget != NULL)
    {
        QString iSelectedCheck = ui->CheckTypeCB->currentText();

        if (iSelectedCheck == CHK_VALUE)
        {
            _pSearchWidget->setSearchType(GTAGenSearchWidget::PARAMETER);
        }
        _pSearchWidget->show();
    }
}


void GTACheckWidget::onSearchOKPBClick()
{
    if(_pSearchWidget != NULL)
    {
        GTAICDParameter selectedItem = _pSearchWidget->getSelectedItems();
        if(_pCurrentSearchResult != NULL)
        {

                if(selectedItem.getUUID().startsWith(ACT_GEN_TOOL))
                {
                    QString firstString = QString("%2$%1").arg(selectedItem.getName(),selectedItem.getUUID());
                    _pCurrentSearchResult->setText(firstString);
                }
                else
                {
                    _pCurrentSearchResult->setText(selectedItem.getName());
                }

                QString iSelectedCheck = ui->CheckTypeCB->currentText();

                if (iSelectedCheck == CHK_FWC_WARN)
                {
                    _pFwcWarnWgt->setHeader(selectedItem.getApplicationName());
                    _pFwcWarnWgt->setWrngColor(selectedItem.getActorName());
                }

                if (iSelectedCheck == CHK_VALUE)
                {
                    QString precVal = selectedItem.getPrecision();
                    if (!precVal.isEmpty() && (precVal != "NA"))
                        _pValueWgt->setPrecisionValue(precVal);
                }

                if (iSelectedCheck == CHK_REFRESH)
                {
                    _pRefreshWdgt->processSearchInput(selectedItem);
                }

                _pSearchWidget->hide();




            
        }
    }
}



/**
 * @brief Select the last edited row for a check value widget
*/
void GTACheckWidget::selectLastEditedRow()
{
    _pValueWgt->selectLastEditedRow();
}

void GTACheckWidget::setCheckCommand(const GTACheckBase * ipCheckCmd,QString ichecktype)
{

    resetWidget();
    ui->CheckTypeCB->setDisabled(false);
    GTAAppController* pController = GTAAppController::getGTAAppController();
    QStringList autoCompleteWordList = pController->getVariablesForAutoComplete();


    if (ipCheckCmd != NULL)
    {
        _isNewCmd = false;
        _CurrentCmdId = ipCheckCmd->getObjId();
        if (_pCommentWgt != NULL)
        {
            _pCommentWgt->clear();
            _pCommentWgt->setComment(ipCheckCmd->getComment());
        }

        if (_pRequirementWdgt != NULL)
        {
            _pRequirementWdgt->clear();
            _pRequirementWdgt->setRequirements(ipCheckCmd->getRequirements());
        }
        GTACheckBase* pCheckCmd = (GTACheckBase*)(ipCheckCmd);
        GTACheckBase::CheckType checkType = pCheckCmd->getCheckType();

        QString checkTypeS;
        QString onDisplayS;
        if (_CheckEnumToStringMap.contains(checkType))
        {
            checkTypeS = _CheckEnumToStringMap.value(checkType);
            if (_CheckWidgetMap.contains(checkTypeS))
            {
                GTACheckWidgetInterface* pCheckIntf = _CheckWidgetMap.value(checkTypeS);
                if (pCheckIntf != NULL)
                {
                    pCheckIntf->setCommand(pCheckCmd);
                    pCheckIntf->setAutoCompleteItems(autoCompleteWordList);

                    if (checkTypeS == CHK_FWC_WARN)
                    {
                        dynamic_cast<GTACheckFwcWarning*>(pCheckCmd);
                        /* if(pCmd != NULL)
                            onDisplayS = pCmd->getOnDisplay();*/
                    }
                    else if (checkTypeS == CHK_FWC_PROC)
                    {
                        dynamic_cast<GTACheckFwcProcedure*>(pCheckCmd);
                        /*if(pCmd != NULL)
                            onDisplayS = pCmd->getOnDisplay();*/
                    }
                    else if (checkTypeS == CHK_FWC_DISP)
                    {
                        dynamic_cast<GTACheckFwcDisplay*>(pCheckCmd);
                        /* if(pCmd != NULL)
                            onDisplayS = pCmd->getOnDisplay();*/
                    }
                }
            }
            else if (checkTypeS == CHK_VISU_DISP)
            {
                GTACheckVisualDisplay* pCmd = dynamic_cast<GTACheckVisualDisplay*>(pCheckCmd);
                if (pCmd != NULL)
                {
                    QString value = pCmd->getValue();
                    _pDisplayTE->setText(value);
                }
            }
            else if (checkTypeS == CHK_MANUAL_CHECK)
            {
                GTACheckManualCheck* pCmd = dynamic_cast<GTACheckManualCheck*>(pCheckCmd);
                if (pCmd != NULL)
                {
                    QString value = pCmd->getValue();
                    _pDisplayTE->setText(value);
                }
            }
        }

        QString actionOnFail, onDisplay, precision, precisionUnit, timeOut, timeOutUnit;
        QString confTime, ConfTimeUnit;
        if (pCheckCmd != NULL)
        {
            actionOnFail = pCheckCmd->getActionOnFail();
            pCheckCmd->getPrecision(precision, precisionUnit);
            pCheckCmd->getTimeOut(timeOut, timeOutUnit);
            pCheckCmd->getConfCheckTime(confTime, ConfTimeUnit);
        }

        ui->CheckTypeCB->setDisabled(true);
        int chkTyeIndex = ui->CheckTypeCB->findText(checkTypeS);
        if (chkTyeIndex >= 0)
            ui->CheckTypeCB->setCurrentIndex(chkTyeIndex);
        else
            ui->CheckTypeCB->setCurrentIndex(0);

        int timeOutIndex = ui->onTimeUnitCB->findText(timeOutUnit);
        if (timeOut.contains("line."))
        {
            ui->onTimeUnitCB->hide();
        }
        else
        {
            ui->onTimeUnitCB->setCurrentIndex(timeOutIndex);
        }

        ui->PrecisionLE->setText(precision);
        ui->TimeOutLE->setText(timeOut);

        int actOnFailIndex = ui->OnFailCB->findText(actionOnFail);
        ui->OnFailCB->setCurrentIndex(actOnFailIndex);
        
        int ConfTimeUnitIndex = ui->ConfTimeUnitCB->findText(ConfTimeUnit);
        if (confTime.contains("line."))
        {
            ui->ConfTimeUnitCB->hide();
        }
        else
        {
            ui->ConfTimeUnitCB->setCurrentIndex(ConfTimeUnitIndex);
        }
        ui->ConfTimeLE->setText(confTime);


        /*int onDispIndex = ui->OnDisplayCB->findText(onDisplay);
        ui->OnDisplayCB->setCurrentIndex(onDispIndex);*/

        int onPrecisionIndex = ui->onPrecisonUnitCB->findText(precisionUnit);
        ui->onPrecisonUnitCB->setCurrentIndex(onPrecisionIndex);

        if (actionOnFail == ACT_FAIL_STOP)
        {
            if(pCheckCmd)
                _pDumpLstWdgt->setDumpList(pCheckCmd->getDumpList());
        }
    }
    else
    {
        _isNewCmd = true;

        //force the widget to open on the selected command if arguments are forced
        int chkTyeIndex = ui->CheckTypeCB->findText(ichecktype);
        if (chkTyeIndex>=0)
            ui->CheckTypeCB->setCurrentIndex(chkTyeIndex);

        _pValueWgt->setAutoCompleteItems(autoCompleteWordList);
        _pCommentWgt->clear();
        _pRequirementWdgt->clear();
    }
}

void GTACheckWidget::setCheckId() {
    if (!_isNewCmd)
    {
        _pCheckCommand->setObjId(_CurrentCmdId, false);
    }
    else
    {
        _pCheckCommand->setObjId();
    }
}

GTACheckBase * GTACheckWidget::getCheckCommand()
{
    return _pCheckCommand;
}
QVariant GTACheckWidget::getCheckVariant()
{
    return _checkCmdVariant;
}
void GTACheckWidget::resetWidget()
{
    foreach(GTACheckWidgetInterface * pCheckIntf , _CheckWidgetMap.values())
    {
        if(pCheckIntf != NULL)
            pCheckIntf->clear();
    }
    _pDisplayTE->clear();

    ui->PrecisionLE->setText(CHK_DEFAULT_PRECISION);
    ui->TimeOutLE->setText(CHK_DEFAULT_TIMEOUT);
    ui->CheckTypeCB->setCurrentIndex(0);
    ui->onTimeUnitCB->setCurrentIndex(0);
    ui->onPrecisonUnitCB->setCurrentIndex(0);
    if(_pCommentWgt != NULL)
        _pCommentWgt->clear();
    if(_pRequirementWdgt != NULL)
        _pRequirementWdgt->clear();
    ui->OnFailCB->setCurrentIndex(0);
    _pDisplayTE->clear();
    ui->ConfTimeUnitCB->setCurrentIndex(0);
    ui->ConfTimeLE->setText("0");
}
void GTACheckWidget::showCommonItem(bool iActionOnFail, bool iPrecesion, bool iTimeOut, bool ,bool iConfTime)
{
    ui->OnFailLabel->hide();
    ui->OnFailCB->hide();
    ui->TimeOutLabel->hide();
    ui->TimeOutLE->hide();
    ui->onTimeUnitCB->hide();
    ui->PrecisionLabel->hide();
    ui->PrecisionLE->hide();
    ui->onPrecisonUnitCB->hide();
    ui->DumpListPB->hide();

    GTAAppController *pAppCtrl = GTAAppController::getGTAAppController();
    bool isGeneric = false;
    if(pAppCtrl != NULL)
    {
        isGeneric = pAppCtrl->isGenericSCXMLMode();
    }
    if(!isGeneric)
    {
        ui->ConfTimeLE->setVisible(iConfTime);
        ui->ConfTimeLB->setVisible(iConfTime);
        ui->ConfTimeUnitCB->setVisible(iConfTime);
    }
    else
    {
        ui->ConfTimeLE->setVisible(false);
        ui->ConfTimeLB->setVisible(false);
        ui->ConfTimeUnitCB->setVisible(false);
    }

    //    ui->OnDisplayLabel->hide();
    //  ui->OnDisplayCB->hide();

    if(iActionOnFail)
    {
        ui->OnFailLabel->show();
        ui->OnFailCB->show();
        ui->DumpListPB->show();
    }

    if(iPrecesion)
    {
        ui->PrecisionLabel->show();
        ui->PrecisionLE->show();
        ui->onPrecisonUnitCB->show();
    }

    if(iTimeOut)
    {
        ui->TimeOutLabel->show();
        ui->TimeOutLE->show();
        ui->onTimeUnitCB->show();
    }

    /*if(iOnDisp)
    {
        ui->OnDisplayLabel->show();
        ui->OnDisplayCB->show();
    }*/
}
void  GTACheckWidget::setTimeOutStatus(bool iStatus)
{
    ui->TimeOutLE->setEnabled(iStatus);
    ui->onTimeUnitCB->setEnabled(iStatus);
    ui->ConfTimeLE->setEnabled(iStatus);
    ui->ConfTimeUnitCB->setEnabled(iStatus);

}
void GTACheckWidget::keyPressEvent ( QKeyEvent * event )
{
    event->key();
    if (event->key()==Qt::Key_Escape )
    {
        this->hide();
    }
    else if (event->key()==Qt::Key_Return || event->key()==Qt::Key_Enter)
    {
        onOKPBClicked();
    }
    else
        QWidget::keyPressEvent(event);
}
void GTACheckWidget::setDumpPBStatus(QString iActFail)
{
    if (iActFail.trimmed() == QString(ACT_FAIL_STOP))
        ui->DumpListPB->setEnabled(true);
    else
        ui->DumpListPB->setDisabled(true);
}
void GTACheckWidget::setEditableAction(bool isEditable)
{
    ui->OKPB->setEnabled(isEditable);

}

void GTACheckWidget::closeEvent(QCloseEvent *event)
{
    if(event)
    {
        emit setParentFocus();
        event->accept();
        this->QWidget::closeEvent(event);
    }
}

/* This SLOT allows to evaluate the entered Confirmation Time and adjust Timeout value if needed
* @sender: this - textChanged() of the Confirmation Time, Timeout fields and their QComboBox Unit
* @input: none
* @return: void
*/
void GTACheckWidget::evaluateConfTime(const QString &)
{
    disconnect(ui->ConfTimeLE, SIGNAL(textChanged(const QString &)), this, SLOT(evaluateConfTime(const QString &)));
    disconnect(ui->TimeOutLE, SIGNAL(textChanged(const QString &)), this, SLOT(evaluateConfTime(const QString &)));

    QString confTime = ui->ConfTimeLE->text();
    QString timeOut = ui->TimeOutLE->text();
    QString confTimeUnit = ui->ConfTimeUnitCB->currentText();
    QString timeOutUnit = ui->onTimeUnitCB->currentText();


    // Confirmation time is visible
    if (!ui->ConfTimeLE->isHidden())
    {
        // Default TimeOut style
        ui->TimeOutLE->setStyleSheet("color: black; background-color: white");

        // If Confirmation time has no value, display "?"
        if (confTime.isEmpty())
        {
            ui->ConfTimeLE->setPlaceholderText("?");
        }
        else
        {
            // Minimal accepted TimeOut value based on Confirmation time
            double minTimeOut = GTAUtil::getMinTimeOutValue(timeOutUnit, confTimeUnit, confTime);

            // If TimeOut has no value, display the minimal value constrainted by Confirmation time
            if (timeOut.isEmpty())
            {
                ui->TimeOutLE->setPlaceholderText(">" + QString::number((minTimeOut)));

            }
            else
            {
                // If Confirmation time has new value, update the TimeOut by adding 3sec
                if (ui->ConfTimeLE->isModified())
                {
                    ui->TimeOutLE->setText(QString::number(minTimeOut));
                    ui->ConfTimeLE->setModified(false);
                }
                else
                {
                    // If TimeOut is not greater than Confirmation time by 3sec, change its color into red
                    if (timeOut.toDouble() < minTimeOut)
                    {
                        ui->TimeOutLE->setStyleSheet("color: red; background-color: white");
                    }
                }
            }
        }
    }
    // Confirmation time is hidden/ignored
    else
    {
        ui->ConfTimeUnitCB->setCurrentIndex(ui->ConfTimeUnitCB->findText(confTimeUnit));
        ui->onTimeUnitCB->setCurrentIndex(ui->onTimeUnitCB->findText(timeOutUnit));
    }

    connect(ui->ConfTimeLE, SIGNAL(textChanged(const QString &)), this, SLOT(evaluateConfTime(const QString &)));
    connect(ui->TimeOutLE, SIGNAL(textChanged(const QString &)), this, SLOT(evaluateConfTime(const QString &)));
}

/**
 * @brief Shows/hides additional external action buttons
*/
void GTACheckWidget::externalActionCheckboxClicked()
{
    if (ui->ExternalActionCheckBox->isChecked()) {
    	ui->AutoOpenActionCB->show();
    	ui->AutoOpenActionCB->setCheckState(Qt::CheckState::Checked);
    	ui->autoAddIfElse->show();
    	ui->ExternalActionComboBox->show();
    }
    else {
    	ui->ExternalActionComboBox->hide();
    	ui->AutoOpenActionCB->hide();
    	ui->autoAddIfElse->hide();
    }
}

/**
 * @brief Sets external action properties onto CheckBase
*/
void GTACheckWidget::setExternalActionProperties(GTACheckBase* checkCmd) {
    if (ui->ExternalActionCheckBox->isChecked() && ui->ExternalActionCheckBox->isVisible()) {
    	bool onOk = true;
    	if (ui->ExternalActionComboBox->currentText().contains("fail")) {
    		onOk = false;
    	}
    	checkCmd->setAutoAddExternalAction(ui->ExternalActionCheckBox->isChecked(), onOk,
    		ui->autoAddIfElse->isChecked(), ui->AutoOpenActionCB->isChecked());
    }

    ui->ExternalActionCheckBox->setChecked(false);
}
