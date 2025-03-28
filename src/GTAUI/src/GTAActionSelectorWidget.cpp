#include "GTAActionSelectorWidget.h"
#include "ui_GTAActionSelectorWidget.h"
#include "GTACommandBuilder.h"
#include "GTAGenSearchWidget.h"
#include <QMessageBox>
#include <QDoubleValidator>
#include "GTAUtil.h"
#include "GTACommandVariant.h"
#include "GTAActionCall.h"
#include "GTAFactory.h"
#include "GTAAppController.h"
#include "GTAICDParameter.h"

GTAActionSelectorWidget::GTAActionSelectorWidget(QWidget *parent) :
        QDialog(parent),_pActionCmd(NULL),
        ui(new Ui::GTAActionSelectorWidget)
{
    ui->setupUi(this);

    _isNewCmd = true;
    _pDumpLstWdgt = new GTADumpListWidget();
    

    QStringList lstTimeUnits;
    lstTimeUnits<<ACT_TIMEOUT_UNIT_SEC<<ACT_TIMEOUT_UNIT_MSEC;
    ui->timeOutUnitCB->clear();
    ui->timeOutUnitCB->addItems(lstTimeUnits);

    ui->ConfTimeUnitCB->clear();
    ui->ConfTimeUnitCB->addItems(lstTimeUnits);


    QStringList lstPrecUnits;
    ui->precisionUnitCB->clear();
    lstPrecUnits<<ACT_PRECISION_UNIT_VALUE<<ACT_PRECISION_UNIT_PERCENT;
    ui->precisionUnitCB->addItems(lstPrecUnits);


    _pRequirementWdgt = new GTACmdRequirementWidget();
    _pRequirementWdgt->hide();

    _pCommentWidget = new GTAActionCmdCommentWidget();
    _pCommentWidget->showImagePanel(false);
    _pSearchWidget = new GTAGenSearchWidget();
    _pSearchWidget->hide();

    connect(_pCommentWidget,SIGNAL(searchImage(QLineEdit*&)),this,SLOT(onSearchPBClick(QLineEdit*&)));
    connect(ui->ActionCB,SIGNAL(currentIndexChanged(QString)),this,SLOT(onActionChange(QString)));
    connect(ui->ComplementCB,SIGNAL(currentIndexChanged(QString)),this,SLOT(onComplementChange(QString)));
    connect(ui->OKPB,SIGNAL(clicked()),this,SLOT(onOKClick()));
    connect(ui->CancelPB,SIGNAL(clicked()),this,SLOT(onCancelPBClick()));
    connect(ui->CommentPB,SIGNAL(clicked()),_pCommentWidget,SLOT(show()));
    connect(ui->RequirementsPB,SIGNAL(clicked()),_pRequirementWdgt,SLOT(show()));
    connect(_pDumpLstWdgt,SIGNAL(searchObject(QLineEdit*&)),this,SLOT(onSearchPBClick(QLineEdit*&)));
    connect(_pSearchWidget,SIGNAL(okPressed()),this,SLOT(onSearchOKPBClick()));
    connect(ui->DumpListPB,SIGNAL(clicked()),_pDumpLstWdgt,SLOT(show()));
    connect( ui->OnFailCB,SIGNAL(currentIndexChanged(QString)),this,SLOT(setDumpPBStatus(QString)));
    connect( ui->precisionUnitCB,SIGNAL(currentIndexChanged(QString)),this,SLOT(resetPrecision(QString)));

	// Manage Confirmation Time and TimeOut Value
	connect(ui->ConfTimeLE, SIGNAL(textChanged(const QString &)), this, SLOT(evaluateConfTime(const QString &)));
	connect(ui->TimeOutLE, SIGNAL(textChanged(const QString &)), this, SLOT(evaluateConfTime(const QString &)));
	connect(ui->ConfTimeUnitCB, SIGNAL(currentTextChanged(const QString &)), this, SLOT(evaluateConfTime(const QString &)));
	connect(ui->timeOutUnitCB, SIGNAL(currentTextChanged(const QString &)), this, SLOT(evaluateConfTime(const QString &)));

    connect(ui->ExternalActionCheckBox, SIGNAL(stateChanged(int)), this, SLOT(externalActionCheckboxClicked()));
    connect(ui->ComplementCB, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(showOrHideExternalActionButtons()));

    //connect( this,SIGNAL(close()),this,SLOT(deleteWidgetsInStack()));
    ui->DumpListPB->setDisabled(true);

    //Tool tip
    ui->ActionCB->setToolTip("Set action type");
    ui->ComplementCB->setToolTip("Set Complement type");

    //QDoubleValidator* pDoubleValidator1 = new QDoubleValidator(ui->PrecisionLE);
    //ui->PrecisionLE->setValidator(pDoubleValidator1);
	ui->PrecisionLE->setValidator(new QDoubleValidator(0, 100, 4, this));

    QDoubleValidator* pDoubleValidator2 = new QDoubleValidator(ui->TimeOutLE);
    ui->TimeOutLE->setValidator(pDoubleValidator2);

    QDoubleValidator* pDoubleValidator3 = new QDoubleValidator(ui->ConfTimeLE);
    ui->ConfTimeLE->setValidator(pDoubleValidator3);

    ui->ExternalActionComboBox->hide();
    ui->AutoOpenActionCB->hide();
    ui->autoAddIfElse->hide();
}


GTAActionSelectorWidget::~GTAActionSelectorWidget()
{

    if(_pSearchWidget != NULL) {delete _pSearchWidget;_pSearchWidget = NULL;}
    if(_pCommentWidget != NULL) {delete _pCommentWidget; _pCommentWidget = NULL;}
    if(_pRequirementWdgt != NULL) {delete _pRequirementWdgt; _pRequirementWdgt = NULL;}
    if(_pDumpLstWdgt != NULL) {delete _pDumpLstWdgt; _pDumpLstWdgt = NULL;}
    deleteWidgetsInStack();
    delete ui;
}
void GTAActionSelectorWidget::setCommandList(const QHash<QString,QStringList> & iCommandList,QStringList iOrder)
{
    if(_ActionList.count())
        _ActionList.clear();

    ui->ActionCB->clear();
    ui->ComplementCB->clear();
    QStringList commandList = iCommandList.keys(); 
    QStringList orderedLst;
    if (!iOrder.isEmpty())
    {
        for (int i=0;i<iOrder.size();i++)
        {
            QString sAction = iOrder.at(i);
            if (commandList.contains(sAction))
            {
                QStringList compliments = iCommandList.value(sAction);
                _ActionList.insert(sAction,compliments);
                orderedLst.append(sAction);
            }

        }
        

    }
    else
    {
        foreach(QString cmd,commandList)
        {
            QStringList compliments = iCommandList.value(cmd);
            _ActionList.insert(cmd,compliments);
            orderedLst.append(cmd);
        }
    }


    ui->ActionCB->addItems(orderedLst);
    ui->ActionCB->setCurrentIndex(0);
}
void GTAActionSelectorWidget::onReEvaluateShowItems(bool iActionOnFail, bool iPrecesion, bool iTimeOut, bool iConfTime, bool iDumpList,bool iExternalComment)
{
    showCommonItem(iActionOnFail,iPrecesion,iTimeOut,iConfTime,iDumpList);
    ui->CommentPB->setVisible(iExternalComment);
    _pCommentWidget->showImagePanel(false);
}

void GTAActionSelectorWidget::onActionChange(const QString &actionName)
{
    QStringList complements = _ActionList.value(actionName);
    ui->ComplementCB->clear();

    if( _pCommentWidget != NULL)
    {
        ui->CommentPB->show();
        _pCommentWidget->showImagePanel(false);
    }

    if(_pRequirementWdgt != NULL)
    {
        ui->RequirementsPB->show();
        _pRequirementWdgt->clear();
    }
    showCommonItem();
    _pDumpLstWdgt->clearDumpList();

    if(complements.count())
    {
        ui->ComplementLabel->show();
        ui->ComplementCB->show();
        ui->ComplementCB->addItems(complements);
        ui->ComplementCB->setCurrentIndex(0);
    }
    else
    {
        ui->ComplementLabel->hide();
        ui->ComplementCB->hide();
    }

    if (actionName == "set" ||
        actionName == "condition" && ui->ComplementCB->currentText() == "if") {
        ui->ExternalActionCheckBox->setChecked(false);
        ui->ExternalActionCheckBox->show();
    }
    else {
        ui->ExternalActionCheckBox->hide();
    }
    
    bool showActOnFail = false;
    bool showPrecision = false;
    bool showTimeOut = false;
    bool showConfTime = false;
    bool showDumpList = false;//only shown if action on fail is shown
    bool showCallInput = false;


    GTAFactory* pFactory = &GTAFactory::getInstance();
    GTAActionWidgetInterface * pActWdgt =NULL;
    pFactory->create(actionName,pActWdgt) ;

    if (pActWdgt!=NULL)
    {
        deleteWidgetsInStack();
        ui->ActionStackSW->addWidget(pActWdgt);
        ui->ActionStackSW->setCurrentWidget(pActWdgt);
        if(pActWdgt->hasSearchItem())
        {
            connect(pActWdgt,SIGNAL(searchObject(QLineEdit*&)),this,SLOT(onSearchPBClick(QLineEdit*&)));
        }



        if (pActWdgt->hasCondition())
        {
            togglePrecision(ARITH_EQ);// required for the first time so that on creation it is always on.
            connect(pActWdgt,SIGNAL(conditionOperatorChanged(QString)),this,SLOT(togglePrecision(QString)));
        }

        connect(pActWdgt,SIGNAL(disablePrecision(bool&)),this,SLOT(onDisablePrecision(bool&)));
        int dValidationFlags =0;
        showActOnFail = pActWdgt->hasActionOnFail();
        showPrecision = pActWdgt->hasPrecision();
        showTimeOut = pActWdgt->hasTimeOut(dValidationFlags);
        showConfTime = pActWdgt->hasConfigTime();
        showDumpList = pActWdgt->hasDumpList();
        showCallInput = pActWdgt->hasCallInput();
        
        if(pActWdgt->hasExternalComment())
            ui->CommentPB->show();
        else
            ui->CommentPB->hide();    

        QList<QWidget*> lstWgt;
        lstWgt=pActWdgt->getWidgetsInTabOrder();

        if(lstWgt.isEmpty()==false)
        {
            lstWgt.prepend(ui->ActionCB);
            lstWgt.append(ui->TimeOutLE);
            lstWgt.append(ui->timeOutUnitCB);
            lstWgt.append(ui->OKPB);
            setTabOrderForWidgets(lstWgt);
        }
        pActWdgt->connectExternalItemsWithLocalWidget(this);

        showCommonItem(showActOnFail,showPrecision,showTimeOut,showConfTime,showDumpList,showCallInput);
    }

}

void GTAActionSelectorWidget::onComplementChange(const QString &complementValue)
{
    QString actionName = ui->ActionCB->currentText();
    QString newCompValue = "";

    // This naming comes from GTA.xml file
    if(complementValue == "monitor")
    {
        newCompValue = "parallel";
    }
    else
    {
        newCompValue = complementValue;
    }

    bool showActOnFail = false;
    bool showPrecision = false;
    bool showTimeOut = false;
    bool showConfTime = false;
    bool showDumpList = false;//only shown if action on fail is shown
    bool showCallInput = false;

    _pDumpLstWdgt->clearDumpList();

    QString widgetName = QString("%1 %2").arg(actionName,newCompValue);

    GTAFactory* pFactory = &GTAFactory::getInstance();
    GTAActionWidgetInterface * pActWdgt =NULL;
    pFactory->create(widgetName,pActWdgt) ;

    if (pActWdgt!=NULL)
    {
        deleteWidgetsInStack();
        ui->ActionStackSW->addWidget(pActWdgt);
        ui->ActionStackSW->setCurrentWidget(pActWdgt);
        if(pActWdgt->hasSearchItem())
            connect(pActWdgt,SIGNAL(searchObject(QLineEdit*&)),this,SLOT(onSearchPBClick(QLineEdit*&)));

        if (pActWdgt->hasCondition())
        {

            togglePrecision(ARITH_EQ);// required for the first time so that on creation it is always on.
            connect(pActWdgt,SIGNAL(conditionOperatorChanged(QString)),this,SLOT(togglePrecision(QString)));
        }
        connect(pActWdgt,SIGNAL(disablePrecision(bool&)),this,SLOT(onDisablePrecision(bool&)));
        pActWdgt->setComplement(newCompValue);
        showActOnFail = pActWdgt->hasActionOnFail();
        showPrecision = pActWdgt->hasPrecision();
        int dValidationFlags;
        showTimeOut = pActWdgt->hasTimeOut(dValidationFlags);
        showConfTime = pActWdgt->hasConfigTime();
        showDumpList = pActWdgt->hasDumpList();
        showCallInput = pActWdgt->hasCallInput();

        if(pActWdgt->hasExternalComment())
            ui->CommentPB->show();
        else
            ui->CommentPB->hide();       
    }
    showCommonItem(showActOnFail,showPrecision,showTimeOut,showConfTime,showDumpList,showCallInput);
}

void GTAActionSelectorWidget::onOKClick()
{
    //errorbox
    QString errMsg = "Provide valid values ";
    bool compileError = false;
    QString sActionWidgetError="";
    QString actionName = ui->ActionCB->currentText();
    QString compliment = ui->ComplementCB->currentText();
    QString timeOut, timeOutUnit;
    QString precision, precisionType;
    QString actionOnFail;
    QString confTime = ui->ConfTimeLE->text();
    QString confTimeUnit = ui->ConfTimeUnitCB->currentText();


    if(!ui->OnFailCB->isHidden())
        actionOnFail = ui->OnFailCB->currentText();

    if(! ui->PrecisionLE->isHidden())
    {
        precision = ui->PrecisionLE->text();
        precisionType = ui->precisionUnitCB->currentText();
    }

    if(! ui->TimeOutLE->isHidden())
    {
        timeOut = ui->TimeOutLE->text();
        timeOutUnit = ui->timeOutUnitCB->currentText();
    }

    QString cmdComment;
    QString imageName;
    if( _pCommentWidget != NULL)
    {
        cmdComment = _pCommentWidget->getComment();
        imageName = _pCommentWidget->getImageName();
    }

    QStringList requirements;
    if(_pRequirementWdgt != NULL)
    {
        requirements = _pRequirementWdgt->getRequirements();
    }
    GTAActionWidgetInterface * pWidgetItf = dynamic_cast<GTAActionWidgetInterface*>(ui->ActionStackSW->currentWidget());
    if (NULL!=pWidgetItf)
    {
		pWidgetItf->setComplement(compliment);
        int validationTimeOut=GTAActionWidgetInterface::kNa;
        bool isNum = false;
        double timeOutVal = timeOut.toDouble(&isNum);
        pWidgetItf->hasTimeOut(validationTimeOut);
        if ( (validationTimeOut & GTAActionWidgetInterface::MANDATORY) == GTAActionWidgetInterface::MANDATORY && timeOut.isEmpty()==true)
        {
            errMsg="TimeOut cannot be empty";
            compileError = true;;
        }
        else if ( (validationTimeOut & GTAActionWidgetInterface::NUMBER) == GTAActionWidgetInterface::NUMBER && isNum==false)
        {
            errMsg="TimeOut has to be numerical";
            compileError = true;;
        }
        else if ( (validationTimeOut & GTAActionWidgetInterface::NON_ZERO) == GTAActionWidgetInterface::NON_ZERO && timeOutVal==0.0)
        {
            errMsg="TimeOut cannot be zero";
            compileError = true;;
        } 
        else if ( (validationTimeOut & GTAActionWidgetInterface::NON_NEGATIVE) == GTAActionWidgetInterface::NON_NEGATIVE && timeOutVal<0.0)
        {
            errMsg="TimeOut cannot be negative";
            compileError = true;;
        }
        else
        {
            pWidgetItf->getActionCommand(_pActionCmd);
			if (_pActionCmd != nullptr)
			{
				QString l_complement = _pActionCmd->getComplement();
				if (l_complement == "if")
				{
					GTAActionIF *pActIf = dynamic_cast<GTAActionIF*>(_pActionCmd);
					if (pActIf != nullptr)
					{
						pActIf->setParameter(pActIf->getParameter().trimmed());
					}
				}
				if (l_complement == "while")
				{
					GTAActionWhile *pActWhile = dynamic_cast<GTAActionWhile*>(_pActionCmd);
					if (pActWhile != nullptr)
					{
						pActWhile->setParameter(pActWhile->getParameter().trimmed());
					}
				}
				if (l_complement == "do while")
				{
					GTAActionDoWhile *pActDoWhile = dynamic_cast<GTAActionDoWhile*>(_pActionCmd);
					if (pActDoWhile != nullptr)
					{
						pActDoWhile->setParameter(pActDoWhile->getParameter().trimmed());
					}
				}
				if (l_complement == "until")
				{
                    if (timeOut.toDouble() < GTAUtil::getMinTimeOutValue(timeOutUnit, confTimeUnit, confTime))
                    {
                        errMsg = "Time Out has to be 3 sec greater than Confirmation Time";
                        compileError = true;;
                    }
                    else 
                    {
                        GTAActionWait* pActWait = dynamic_cast<GTAActionWait*>(_pActionCmd);
                        if (pActWait != nullptr)
                        {
                            pActWait->setParameter(pActWait->getParameter().trimmed());
                        }
                    }
				}
				if (l_complement == "for")
				{
					GTAActionWait *pActWait = dynamic_cast<GTAActionWait*>(_pActionCmd);
					if (pActWait != nullptr)
					{
						pActWait->setCounter(pActWait->getCounter().trimmed());
					}
				}
				if (l_complement == "for each")
				{
					// This action will be treated on the OK_PB click
					GTAActionForEach *pActForEach = dynamic_cast<GTAActionForEach*>(_pActionCmd);
					if (pActForEach != nullptr)
					{
						// We must check that there is no column named "Value"
						for (const auto& elm : pActForEach->getHeaderColumns())
						{
							if (elm.compare("Value") == 0)
							{
								errMsg.clear();
								errMsg = "There is an error in the TABLE, we cannot have a column named \"Value\". Please use another column name!";
                                compileError = true;
							}
						}
					}
				}
			}
			else // _pActionCmd == NULL
            {
                sActionWidgetError = pWidgetItf->getLastError();
                if (sActionWidgetError.isEmpty() == false) {
                    errMsg.append(": ");
                    errMsg.append(sActionWidgetError);
                }
                else {
                    errMsg = "Action command is not valid. Please check all the fields or the content of your file.";
                }
                    
                compileError = true;
            }
        }
    }
    bool wasReopendScheduled = false;
    if(!compileError)
    {
        if (NULL!=_pActionCmd)
        { 
            _pActionCmd->setAction(actionName);
            _pActionCmd->setComplement(compliment);
            _pActionCmd->setActionOnFail(actionOnFail);
            if(!_isNewCmd)
            {
                _pActionCmd->setObjId(_CurrentCmdId,false);
            }
            else
            {
                _pActionCmd->setObjId();
            }
            if (precision.isEmpty())
            {
                if (actionName == ACT_CONDITION)
                    if (compliment !=COM_CONDITION_ELSE)
                        _pActionCmd->setPrecision(ACT_DEFAULT_PRECISION,ACT_PRECISION_UNIT_VALUE);

                if (actionName ==ACT_WAIT)
                    if (compliment ==COM_WAIT_UNTIL)
                        _pActionCmd->setPrecision(ACT_DEFAULT_PRECISION,ACT_PRECISION_UNIT_VALUE);
            }
            else
                _pActionCmd->setPrecision(precision,precisionType);

            if (timeOut.isEmpty())
            {

                if (actionName == ACT_CONDITION)
                    if (compliment !=COM_CONDITION_ELSE)
                        _pActionCmd->setTimeOut(ACT_DEFAULT_TIMEOUT,ACT_TIMEOUT_UNIT_SEC);

                if (actionName == ACT_SET || actionName == ACT_ONECLICK_SET)
                    _pActionCmd->setTimeOut(ACT_DEFAULT_TIMEOUT,ACT_TIMEOUT_UNIT_SEC);

                if (actionName == ACT_WAIT)
                    if (compliment ==COM_WAIT_UNTIL)
                        _pActionCmd->setTimeOut(ACT_DEFAULT_TIMEOUT,ACT_TIMEOUT_UNIT_SEC);
            }
            else
                _pActionCmd->setTimeOut(timeOut,timeOutUnit);


            if (confTime.isEmpty())
            {

                if (actionName == ACT_CONDITION)
                    if (compliment !=COM_CONDITION_ELSE)
                        _pActionCmd->setConfCheckTime(ACT_DEFAULT_CONFTIME,ACT_TIMEOUT_UNIT_SEC);
            }
            else
                _pActionCmd->setConfCheckTime(confTime,confTimeUnit);


            QStringList sDumplIst = _pDumpLstWdgt->getDumpList();
            _pActionCmd->setDumpList(sDumplIst);

            if(_pActionCmd->isTitle()==false && _pActionCmd->isOneClickTitle()==false)
            {
                _isInputForInnerCall = ui->CallInputCB->isChecked();
                QString isInputForInnerCallText = ACT_SET_INNERCALL;
                cmdComment.remove(isInputForInnerCallText);
                cmdComment = _isInputForInnerCall ? cmdComment + isInputForInnerCallText : cmdComment;
                _pActionCmd->setComment(cmdComment);
            }

            _pActionCmd->setRequirements(requirements);
            QString channelSelectedByUser = _pSearchWidget->getUserChannelSelection();
            _pActionCmd->setSelectedChannel(channelSelectedByUser);

            GTAAppController* pApctrl = GTAAppController::getGTAAppController();
            if(pApctrl)
            {
                QString sValue,sValueUnit;
                if(compliment.isEmpty())
                {
                    sValue=pApctrl->getCommandProperty(actionName,XATTR_INVOKE_DELAY);
                    sValueUnit=pApctrl->getCommandProperty(actionName,XATTR_INVOKE_DELAY_UNIT);
                }
                else
                {
                    sValue =pApctrl->getCommandProperty(QString("%1 %2").arg(actionName,compliment),XATTR_INVOKE_DELAY);
                    sValueUnit=pApctrl->getCommandProperty(actionName,XATTR_INVOKE_DELAY_UNIT);
                }

                if (!sValue.isEmpty())
                {
                    _pActionCmd->setCommandProperty(XATTR_INVOKE_DELAY,sValue);
                    _pActionCmd->setCommandProperty(XATTR_INVOKE_DELAY_UNIT,sValueUnit);
                }
            }
        }

        setExternalActionProperties();
        GTACommandVariant variant;
        variant.setCommand(_pActionCmd);
        _actionCmdVariant.setValue(variant);
        emit okPBClicked();
        accept();
        deleteWidgetsInStack();
        if (_reopenScheduled) {
            _reopenScheduled = false;
            wasReopendScheduled = true;
            emit reopenWindowScheduled();
        }
    }
    else {
        if (_showErrorBoxes) {
            QMessageBox::critical(0, "Error", errMsg);
        }
        _lastError = errMsg;
    }

    if (!wasReopendScheduled) {
        wasReopendScheduled = false;
        emit setParentFocus();
    }
     
}

void GTAActionSelectorWidget::clickOk()
{
    onOKClick();
}

void GTAActionSelectorWidget::clickCancel()
{
    onCancelPBClick();
}

GTAActionWidgetInterface* GTAActionSelectorWidget::getCurrentWidget()
{
    return dynamic_cast<GTAActionWidgetInterface*>(ui->ActionStackSW->currentWidget());
}

bool GTAActionSelectorWidget::setActionSelectorWidgetFields(const QList<QPair<QString, QString>>& actionParams)
{
    // default setup
    ui->OnFailCB->setCurrentIndex(0);
    ui->TimeOutLE->setText("3");
    ui->PrecisionLE->setText("0");
    ui->ConfTimeLE->setText("0");

    for (auto param : actionParams)
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

void GTAActionSelectorWidget::onCancelPBClick()
{
    //  hide();

    reject();
    deleteWidgetsInStack();
    emit setParentFocus();
    
}
void GTAActionSelectorWidget::deleteWidgetsInStack()
{
    int count = ui->ActionStackSW->count();
    //bool clearStack = false;
    while(count>0)
    {
        QWidget* pCurrentWidget = ui->ActionStackSW->widget(0);
        if (pCurrentWidget!=NULL)
        {

            ui->ActionStackSW->removeWidget(pCurrentWidget);
            delete pCurrentWidget;
        }
        count = ui->ActionStackSW->count();
    }

    
}

void GTAActionSelectorWidget::onSearchPBClick(QLineEdit *& ipItemToUpdate)
{
    _pCurrentSearchResult = ipItemToUpdate;
    if(_pSearchWidget != NULL)
    {
        QString actionName = ui->ActionCB->currentText();
        QString complement = ui->ComplementCB->currentText();

        if(sender() == _pDumpLstWdgt)
        {
            _pSearchWidget->setSearchType(GTAGenSearchWidget::DUMP_LIST);
        }
        else if( actionName == ACT_REL)
        {
            QString item = _pCurrentSearchResult->text();
            if(!item.isEmpty())
            {
                _pSearchWidget->excludeItemsFromSearch(item.split(";"));
            }
            else
                _pSearchWidget->excludeItemsFromSearch(QStringList());
            _pSearchWidget->setSearchType(GTAGenSearchWidget::RELEASE);
        }
        else 
        {
            
            GTAActionWidgetInterface * pWidgetItf = dynamic_cast<GTAActionWidgetInterface*>(ui->ActionStackSW->currentWidget());
            if (NULL!=pWidgetItf)
            {
                int searchType = pWidgetItf->getSearchType();                
                if(searchType>-1)
                    _pSearchWidget->setSearchType(static_cast<GTAGenSearchWidget::ElementType> (searchType));

            }


        }

        _pSearchWidget->show();
    }
}

void GTAActionSelectorWidget::onSearchOKPBClick()
{
    if(_pSearchWidget != NULL)
    {
        GTAICDParameter selectedItem = _pSearchWidget->getSelectedItems();
        QList<GTAICDParameter> selectedItems = _pSearchWidget->getSelectedItemList();
        GTAGenSearchWidget::ElementType searchType = _pSearchWidget->getSearchType();
        QString currentSearchTxt = _pCurrentSearchResult->text().trimmed();
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


            _pSearchWidget->hide();

            QString actionName = ui->ActionCB->currentText();
            QString complName = ui->ComplementCB->currentText();

            GTAActionWidgetInterface * pWidgetItf = dynamic_cast<GTAActionWidgetInterface*>(ui->ActionStackSW->currentWidget());
            if (pWidgetItf)
            {
                if(selectedItem.getUUID().startsWith(ACT_GEN_TOOL))
                {
                    QString firstString = QString("%2#%1").arg(selectedItem.getName(),selectedItem.getUUID());
                    QStringList newList;

                    newList.append(firstString);
                    newList.append(selectedItem.getDirection());
                    pWidgetItf->processSearchInput(newList);
                }
                else
                {
					// call processSearchInput for a list of ICD parameters
                    if (searchType == GTAGenSearchWidget::RELEASE_PIR_SEARCH)
                    {
                        pWidgetItf->processSearchInput(selectedItems);
                    }
                    else
                    {
                        pWidgetItf->processSearchInput(selectedItem.getAsList());
                    }
                }

                QString newPrecison = pWidgetItf->getUpdatedPrecison();
                if (newPrecison.isEmpty()==false)
                {
                    bool isNum = false;
                    newPrecison.toDouble(&isNum);
                    if (isNum)
                    {
                        ui->PrecisionLE->setText(newPrecison);
                    }
                }

            }

        }
    }

}


void GTAActionSelectorWidget::setActionCommand(const GTAActionBase * ipActionCmd,QString iaction,QString icomplement)
{
    if(_pCommentWidget != NULL)
    {
        _pCommentWidget->showImagePanel(false);
    }
    if( ipActionCmd != NULL)
    {
        _isNewCmd = false;
        _CurrentCmdId = ipActionCmd->getObjId();
        _pActionCmd =(GTAActionBase*) ipActionCmd;
        QString actionName = ipActionCmd->getAction();
        QString compliment = ipActionCmd->getComplement();
        QString actionOnFail = ipActionCmd->getActionOnFail();
        QString precision, precisionType;
        ipActionCmd->getPrecision(precision,precisionType);
        QString timeOut, timeOutUnit;
        ipActionCmd->getTimeOut(timeOut,timeOutUnit);
        QString confTime,ConfTimeUnit;
        ipActionCmd->getConfCheckTime(confTime,ConfTimeUnit);

        _pDumpLstWdgt->clearDumpList();
        _pDumpLstWdgt->setDumpList(QStringList());

        int actionIndex = ui->ActionCB->findText(actionName);
        if(actionIndex >= 0)
            ui->ActionCB->setCurrentIndex(actionIndex);

        int complementIndex = ui->ComplementCB->findText(compliment);
        if(complementIndex >= 0)
            ui->ComplementCB->setCurrentIndex(complementIndex);


        if (NULL!=ipActionCmd)
        {

            GTAActionBase * pActionCmd = (GTAActionBase*)ipActionCmd;
            QString widgetName;
            if(compliment.isEmpty())
                widgetName=actionName;
            else
                widgetName = QString("%1 %2").arg(actionName,compliment);
            



            GTAFactory* pFactory = &GTAFactory::getInstance();
            GTAActionWidgetInterface * pActWidget =NULL;
            pFactory->create(widgetName,pActWidget) ;
            if (pActWidget!=NULL)
            {
                // clear();
                
                ui->ActionStackSW->addWidget(pActWidget);
                ui->ActionStackSW->setCurrentWidget(pActWidget);

                if(pActWidget->hasSearchItem())
                    connect(pActWidget,SIGNAL(searchObject(QLineEdit*&)),this,SLOT(onSearchPBClick(QLineEdit*&)));

                if (pActWidget->hasCondition())
                {
                    if(NULL==pActionCmd)
                        togglePrecision(ARITH_EQ);// required for the first time so that on creation it is always on.
                    connect(pActWidget,SIGNAL(conditionOperatorChanged(QString)),this,SLOT(togglePrecision(QString)));

                }

                connect(pActWidget,SIGNAL(disablePrecision(bool&)),this,SLOT(onDisablePrecision(bool&)));
                pActWidget->setActionCommand(pActionCmd);
            }
        }



        GTAActionBase * pActionCmd = (GTAActionBase*)ipActionCmd;
        if(! actionName.isEmpty())
        {

            if(! actionOnFail.isEmpty())
            {
                int onFailIndex = ui->OnFailCB->findText(actionOnFail);
                if(onFailIndex >= 0)
                {
                    ui->OnFailCB->setCurrentIndex(onFailIndex);
                    if (actionOnFail==ACT_FAIL_STOP)
                    {
                        ui->DumpListPB->setEnabled(true);
                        _pDumpLstWdgt->setDumpList(pActionCmd->getDumpList());
                    }
                    else
                    {
                        ui->DumpListPB->setEnabled(false);
                        _pDumpLstWdgt->clearDumpList();
                    }
                }
            }
            if(! precision.isEmpty())
            {
                ui->PrecisionLE->setText(precision);
                int precTypeIndex = ui->precisionUnitCB->findText(precisionType);
                if(precTypeIndex >= 0)
                    ui->precisionUnitCB->setCurrentIndex(precTypeIndex);
            }
            else
                ui->PrecisionLE->setText(ACT_DEFAULT_PRECISION);

            if(! timeOut.isEmpty())
            {
                ui->TimeOutLE->setText(timeOut);
                int timeOutUnitIndex = ui->timeOutUnitCB->findText(timeOutUnit);
                if(timeOutUnitIndex >= 0)
                    ui->timeOutUnitCB->setCurrentIndex(timeOutUnitIndex);
            }
            else
                ui->TimeOutLE->setText(ACT_DEFAULT_TIMEOUT);

            if(!confTime.isEmpty())
            {
                ui->ConfTimeLE->setText(confTime);
                int ConfTimeUnitIndex = ui->ConfTimeUnitCB->findText(ConfTimeUnit);
                if(ConfTimeUnitIndex  >= 0)
                    ui->ConfTimeUnitCB->setCurrentIndex(ConfTimeUnitIndex);
            }
            else
                ui->ConfTimeLE->setText(ACT_DEFAULT_CONFTIME);

        }
        ui->ActionCB->setDisabled(true);
        ui->ComplementCB->setDisabled(true);
        ui->ActionCB->setToolTip("Action type edit locked");
        ui->ComplementCB->setToolTip("Complement type edit locked");
    }
    else
    {
        _isNewCmd = true;
        _pActionCmd = NULL;
        //pointing actionCB index to selected action if forcibly sent
        if (!iaction.isEmpty())
        {
            int actionIndex = ui->ActionCB->findText(iaction);
            if(actionIndex >= 0)
                ui->ActionCB->setCurrentIndex(actionIndex);

            int complementIndex = ui->ComplementCB->findText(icomplement);
            if(complementIndex >= 0)
                ui->ComplementCB->setCurrentIndex(complementIndex);
        }
        resetWidgets();
    }
    if(_pCommentWidget != NULL)
    {
        if (_pActionCmd != NULL)
        {
            QString cmt = _pActionCmd->getComment();
            ui->CallInputCB->setChecked(cmt.contains(ACT_SET_INNERCALL));
            _pCommentWidget->setComment(cmt);
        }            
        else
        {
            _pCommentWidget->setComment(QString());
            _pCommentWidget->setImageName(QString());
        }
    }
    if(_pRequirementWdgt != NULL)
    {
        if(_pActionCmd != NULL)
            _pRequirementWdgt->setRequirements(_pActionCmd->getRequirements());
        else
        {
            _pRequirementWdgt->setRequirements(QStringList());
        }

    }

}

void  GTAActionSelectorWidget::resetWidgets()
{

    ui->ActionCB->setDisabled(false);
    ui->ComplementCB->setDisabled(false);
    ui->TimeOutLE->setText(ACT_DEFAULT_TIMEOUT);
    ui->OnFailCB->setCurrentIndex(0);
    ui->PrecisionLE->setText(ACT_DEFAULT_PRECISION);
    ui->ConfTimeLE->setText(ACT_DEFAULT_CONFTIME);
    ui->ActionCB->setToolTip("Set action type");
    ui->ComplementCB->setToolTip("Set Complement type");
    ui->precisionUnitCB->setCurrentIndex(0);
    ui->timeOutUnitCB->setCurrentIndex(0);

    if (_pDumpLstWdgt)
        _pDumpLstWdgt->clearDumpList(true);





}

QVariant GTAActionSelectorWidget::getActionVariant()
{
    return _actionCmdVariant;
}
/**
  * This function show/hide the common controls for commands, which are action on fail, precision and timeout
  * iActionOnFail : to show/hide action on fail control
  * iPrecesion : to show/hide precision control
  * iTimeOut : to show/hide time out control
  * iConfTime : to show/hide the conf time
  * iDumpList : to show/hide the dump list
  * iCallInput : to show/hide the call input checkbox
  */
void GTAActionSelectorWidget::showCommonItem(bool iActionOnFail /* = false */, bool iPrecesion /* = false */, bool iTimeOut /* = false */, bool iConfTime,bool iDumpList, bool iCallInput)
{
    ui->PrecisionLabel->hide();
    ui->PrecisionLE->hide();
    ui->precisionUnitCB->hide();

    ui->OnFailCB->hide();
    ui->OnFailLabel->hide();

    ui->TimeOutLabel->hide();
    ui->TimeOutLE->hide();
    ui->timeOutUnitCB->hide();
    ui->DumpListPB->hide();
    ui->ConfTimeLB->setVisible(iConfTime);
    ui->ConfTimeLE->setVisible(iConfTime);
    ui->ConfTimeUnitCB->setVisible(iConfTime);

    ui->CallInputCB->hide();
    ui->CallInputCB->setChecked(false);

    if(iActionOnFail)
    {
        ui->OnFailCB->show();
        ui->OnFailLabel->show();
        
        
    }
    if(iPrecesion)
    {
        ui->PrecisionLabel->show();

        ui->PrecisionLE->show();
        ui->precisionUnitCB->show();
    }
    if(iTimeOut)
    {
        ui->TimeOutLabel->show();
        ui->TimeOutLE->show();
        ui->timeOutUnitCB->show();
    }
    if(iConfTime)
    {
        resetWidgets();
    }

    if (iDumpList)
    {
        ui->DumpListPB->show();
    }
    if (iCallInput)
    {
        ui->CallInputCB->setChecked(_isInputForInnerCall);
        ui->CallInputCB->show();        
    }
}

void GTAActionSelectorWidget::onDisablePrecision(bool &iStatus)
{
    ui->precisionUnitCB->setDisabled(iStatus);
    //ui->PrecisionLE->setText("0");
    //ui->precisionUnitCB->setCurrentIndex(0);
    ui->PrecisionLE->setDisabled(iStatus);
}

/*
 * This SLOT is called after having changed the operator of the CONDITION action
 */
void GTAActionSelectorWidget::togglePrecision(QString condition)
{
//    Q_UNUSED(condition);
    if (condition==ARITH_NOTEQ || condition == ARITH_EQ)
    {
        ui->precisionUnitCB->setEnabled(true);
        ui->PrecisionLE->setEnabled(true);
    }
//    else
//    {
        //ui->precisionUnitCB->setEnabled(false);
        //ui->PrecisionLE->setText("0");
        //ui->precisionUnitCB->setCurrentIndex(0);
        //ui->PrecisionLE->setEnabled(false);
//    }
}

/*
 * This SLOT allow to reset the Precision value when PrecisionType is changed
 */
void GTAActionSelectorWidget::resetPrecision(QString condition)
{
    bool val = true;
    Q_UNUSED(condition);
    ui->precisionUnitCB->setEnabled(val);
    ui->PrecisionLE->setText("0");
    ui->PrecisionLE->setEnabled(val);

//    bool isChecked = (ui->checkOnlyValueChB->isChecked() |
//                      ui->checkFSCB->isChecked() |
//                      ui->chkOnlySDI->isChecked() |
//                      ui->chkOnlyParity->isChecked() |
//                      ui->chkRefreshRate->isChecked());

//    if (condition == ARITH_NOTEQ || condition == ARITH_EQ)
//    {
//        if(ui->checkOnlyValueChB->isChecked() || ui->chkRefreshRate->isChecked())
//            val = true;
//        else if (!isChecked)
//            val = true;
//        else
//            val = false;
//        ui->precisionUnitCB->setEnabled(val);
//        ui->PrecisionLE->setEnabled(val);
//    }
}

void GTAActionSelectorWidget::keyPressEvent ( QKeyEvent * event )
{
    if (event->key()==Qt::Key_Escape )
    {
        this->hide();
    }
    else if (event->key()==Qt::Key_Return || event->key()==Qt::Key_Enter)
    {
        onOKClick();
    }
    else
        QWidget::keyPressEvent(event);
}
void GTAActionSelectorWidget::setTabOrderForWidgets(QList<QWidget*> iLstWgt)const
{

    int sizeWidgets = iLstWgt.size();

    for (int i=0;i<sizeWidgets;i++)
    {
        if (i<sizeWidgets-1)
        {
            QWidget* pCurrentWgt=iLstWgt.at(i);
            QWidget* pNextWgt=iLstWgt.at(i+1);
            if (NULL!=pCurrentWgt && NULL!= pNextWgt)
            {
                this->setTabOrder(pCurrentWgt,pNextWgt);
            }
        }

    }
}

QString GTAActionSelectorWidget::getLastErrorAndResetIt() {
    QString tmp = _lastError;
    _lastError.clear();
    return tmp;
}

void GTAActionSelectorWidget::setActionCB(int idx) {
    ui->ActionCB->setCurrentIndex(idx);
}

void GTAActionSelectorWidget::setDumpPBStatus(QString iActFail)
{
    if (iActFail.trimmed() == QString(ACT_FAIL_STOP))
        ui->DumpListPB->setEnabled(true);
    else
        ui->DumpListPB->setDisabled(true);
}



void GTAActionSelectorWidget::setCallCommandonDropEvent(QString& iFilename,QString &FileUUID)
{
    _pActionCmd = NULL;
    GTAActionCall callCommand ;//= new GTAActionCall;
    callCommand.setElement(iFilename);
    callCommand.setAction(ACT_CALL);
    callCommand.setRefrenceFileUUID(FileUUID);
    callCommand.setObjId();
    callCommand.setComment("Created through drag and drop into editor");
    if (iFilename.endsWith(".fun"))
    {
        callCommand.setComplement(COM_CALL_FUNC);
        this->setActionCommand(&callCommand);
        this->setVisible(true);
        
    }
    else if (iFilename.endsWith(".obj"))
    {
        callCommand.setComplement(COM_CALL_OBJ);
        this->setActionCommand(&callCommand);
        this->setVisible(true);
        
    }
    else if (iFilename.endsWith(".pro"))
    {
        callCommand.setComplement(COM_CALL_PROC);
        this->setActionCommand(&callCommand);
        this->setVisible(true);

    }
    
}
void GTAActionSelectorWidget:: setEditableAction(bool isEditable)
{
    if(isEditable)
        ui->OKPB->setDisabled(false);
    else
        ui->OKPB->setDisabled(true);

}


void GTAActionSelectorWidget::closeEvent(QCloseEvent *event)
{
    if(event)
    {
        emit setParentFocus();
        event->accept();
        this->QDialog::closeEvent(event);
    }
}

/* This SLOT allows to evaluate the entered Confirmation Time and adjust Timeout value if needed
* @sender: this - textChanged() of the Confirmation Time, Timeout fields and their QComboBox Unit
* @input: none
* @return: void
*/
void GTAActionSelectorWidget::evaluateConfTime(const QString &)
{
	disconnect(ui->ConfTimeLE, SIGNAL(textChanged(const QString &)), this, SLOT(evaluateConfTime(const QString &)));
	disconnect(ui->TimeOutLE, SIGNAL(textChanged(const QString &)), this, SLOT(evaluateConfTime(const QString &)));

	QString confTime = ui->ConfTimeLE->text();
	QString timeOut = ui->TimeOutLE->text();
	QString confTimeUnit = ui->ConfTimeUnitCB->currentText();
	QString timeOutUnit = ui->timeOutUnitCB->currentText();
    

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
		ui->timeOutUnitCB->setCurrentIndex(ui->timeOutUnitCB->findText(timeOutUnit));
	}

	connect(ui->ConfTimeLE, SIGNAL(textChanged(const QString &)), this, SLOT(evaluateConfTime(const QString &)));
	connect(ui->TimeOutLE, SIGNAL(textChanged(const QString &)), this, SLOT(evaluateConfTime(const QString &)));
}

void GTAActionSelectorWidget::externalActionCheckboxClicked()
{
    if (ui->ExternalActionCheckBox->isChecked()) {
        ui->AutoOpenActionCB->show();
        ui->AutoOpenActionCB->setCheckState(Qt::CheckState::Checked);
        if (ui->ActionCB->currentText() != "condition") {
            ui->autoAddIfElse->show();
            ui->ExternalActionComboBox->show();
        }
    }
    else {
        ui->ExternalActionComboBox->hide();
        ui->AutoOpenActionCB->hide();
        ui->autoAddIfElse->hide();
    }
}

void GTAActionSelectorWidget::showOrHideExternalActionButtons() {
    if (ui->ActionCB->currentText() == "condition") {
        if (ui->ComplementCB->currentText() == "if") {
            ui->ExternalActionCheckBox->show();
        }
        else {
            ui->ExternalActionCheckBox->setChecked(false);
            ui->ExternalActionCheckBox->hide();
        }
    }
}

void GTAActionSelectorWidget::setExternalActionProperties() {
    if (ui->ExternalActionCheckBox->isChecked() && ui->ExternalActionCheckBox->isVisible()) {
        bool onOk = true;
        if (ui->ExternalActionComboBox->currentText().contains("fail")) {
            onOk = false;
        }
        _pActionCmd->setAutoAddExternalAction(ui->ExternalActionCheckBox->isChecked(), onOk,
            ui->autoAddIfElse->isChecked(), ui->AutoOpenActionCB->isChecked());
    }

    ui->ExternalActionCheckBox->setChecked(false);
}
