#include "GTAWaitUntilActionWidget.h"
#include "GTAActionWait.h"
#include "GTAParamValidator.h"
#include "GTAParamValueValidator.h"
#include "GTAGenSearchWidget.h"
#include "GTAAppController.h"

#pragma warning (push, 0)
#include "ui_GTAWaitUntilActionWidget.h"
#include <QCompleter>
#pragma warning (pop)

GTAWaitUntilActionWidget::GTAWaitUntilActionWidget(QWidget *parent) :
    GTAActionWidgetInterface(parent),
    ui(new Ui::GTAWaitUntilActionWidget)
{
    ui->setupUi(this);

    QStringList operatorList;
    operatorList <<ARITH_EQ<<ARITH_NOTEQ<<ARITH_GT<<ARITH_LTEQ<<ARITH_LT<<ARITH_GTEQ;
    ui->OperatorCB->clear();
    ui->OperatorCB->addItems(operatorList);
    _SearchPBEditMap.insert(ui->SearchPB,ui->ParamNameLE);
    _SearchPBEditMap.insert(ui->SearchValuePB,ui->ValueLE);
    foreach(QPushButton * pSearchPB, _SearchPBEditMap.keys())
    {
        connect(pSearchPB,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));
    }
    GTAParamValidator* pValidator1 = new GTAParamValidator(ui->ParamNameLE);
    ui->ParamNameLE->setValidator(pValidator1);
    GTAParamValueValidator* pValidator2 = new GTAParamValueValidator(ui->ValueLE);
    ui->ValueLE->setValidator(pValidator2);
    _isParameterLE = false;
	
    hideNote(true);
    _FunctionalStatus << ACT_FSSSM_NOT_LOST<<ACT_FSSSM_NO<<ACT_FSSSM_ND<<ACT_FSSSM_FW<<ACT_FSSSM_NCD<<ACT_FSSSM_FT<<ACT_FSSSM_MINUS<<ACT_FSSSM_PLUS<<ACT_FSSSM_LOST;
    ui->FunctionalStatusCB->addItems(_FunctionalStatus);

	connect(ui->ParamNameLE, SIGNAL(textChanged(const QString &)), this, SLOT(evaluateParamMedia(const QString &)));
    connect(ui->OperatorCB,SIGNAL(currentIndexChanged(QString)),this,SLOT(operatorChanged(QString)));

    connect(ui->ChkFSOnly,SIGNAL(toggled(bool)),this,SLOT(disableValueEdits(bool)));
    connect(ui->ChkFSOnly,SIGNAL(toggled(bool)),this,SLOT(hideNote(bool)));
    connect(ui->ChkFSOnly,SIGNAL(toggled(bool)),this,SLOT(resetCheckOnlyValue(bool)));
    connect(ui->ChkFSOnly,SIGNAL(toggled(bool)),this,SLOT(resetSDI(bool)));
    connect(ui->ChkFSOnly,SIGNAL(toggled(bool)),this,SLOT(resetParity(bool)));
    connect(ui->ChkFSOnly,SIGNAL(toggled(bool)),this,SLOT(resetRefreshRate(bool)));

    connect(ui->ChkValueOnly,SIGNAL(toggled(bool)),this,SLOT(resetCheckFS(bool)));
    connect(ui->ChkValueOnly,SIGNAL(toggled(bool)),this,SLOT(enableValueEdits(bool)));
    connect(ui->ChkValueOnly,SIGNAL(toggled(bool)),this,SLOT(hideNote(bool)));
    connect(ui->ChkValueOnly,SIGNAL(toggled(bool)),this,SLOT(resetSDI(bool)));
    connect(ui->ChkValueOnly,SIGNAL(toggled(bool)),this,SLOT(resetParity(bool)));
    connect(ui->ChkValueOnly,SIGNAL(toggled(bool)),this,SLOT(resetRefreshRate(bool)));

    connect(ui->chkOnlySDI,SIGNAL(toggled(bool)),this,SLOT(disableValueEdits(bool)));
    connect(ui->chkOnlySDI,SIGNAL(toggled(bool)),this,SLOT(hideNote(bool)));
    connect(ui->chkOnlySDI,SIGNAL(toggled(bool)),this,SLOT(resetCheckFS(bool)));
    connect(ui->chkOnlySDI,SIGNAL(toggled(bool)),this,SLOT(resetCheckOnlyValue(bool)));
    connect(ui->chkOnlySDI,SIGNAL(toggled(bool)),this,SLOT(resetParity(bool)));
    connect(ui->chkOnlySDI,SIGNAL(toggled(bool)),this,SLOT(resetRefreshRate(bool)));


    connect(ui->chkOnlyParity,SIGNAL(toggled(bool)),this,SLOT(disableValueEdits(bool)));
    connect(ui->chkOnlyParity,SIGNAL(toggled(bool)),this,SLOT(hideNote(bool)));
    connect(ui->chkOnlyParity,SIGNAL(toggled(bool)),this,SLOT(resetCheckFS(bool)));
    connect(ui->chkOnlyParity,SIGNAL(toggled(bool)),this,SLOT(resetCheckOnlyValue(bool)));
    connect(ui->chkOnlyParity,SIGNAL(toggled(bool)),this,SLOT(resetSDI(bool)));
    connect(ui->chkOnlyParity,SIGNAL(toggled(bool)),this,SLOT(resetRefreshRate(bool)));

    connect(ui->chkRefreshRate,SIGNAL(toggled(bool)),this,SLOT(disableValueEditsOnRefresh(bool)));
    connect(ui->chkRefreshRate,SIGNAL(toggled(bool)),this,SLOT(hideNote(bool)));
    connect(ui->chkRefreshRate,SIGNAL(toggled(bool)),this,SLOT(resetCheckFS(bool)));
    connect(ui->chkRefreshRate,SIGNAL(toggled(bool)),this,SLOT(resetCheckOnlyValue(bool)));
    connect(ui->chkRefreshRate,SIGNAL(toggled(bool)),this,SLOT(resetSDI(bool)));
    connect(ui->chkRefreshRate,SIGNAL(toggled(bool)),this,SLOT(resetParity(bool)));

//    connect(ui->checkValnFS,SIGNAL(toggled(bool)),this,SLOT(disableValueEditsOnRefresh(bool)));
//    connect(ui->checkValnFS,SIGNAL(toggled(bool)),this,SLOT(hideNote(bool)));
//    connect(ui->checkValnFS,SIGNAL(toggled(bool)),this,SLOT(resetCheckFS(bool)));
//    connect(ui->checkValnFS,SIGNAL(toggled(bool)),this,SLOT(resetCheckOnlyValue(bool)));
//    connect(ui->checkValnFS,SIGNAL(toggled(bool)),this,SLOT(resetSDI(bool)));
//    connect(ui->checkValnFS,SIGNAL(toggled(bool)),this,SLOT(resetParity(bool)));
//    connect(ui->checkValnFS,SIGNAL(toggled(bool)),this,SLOT(resetRefreshRate(bool)));

    connect(ui->ChkLoopSampling,SIGNAL(toggled(bool)),this,SLOT(addLoopSampling(bool)));
    QIntValidator* pIntValidator = new QIntValidator(ui->LoopSamplingLE);
    ui->LoopSamplingLE->setValidator(pIntValidator);
    ui->LoopSamplingLE->setDisabled(true);
    ui->LoopSamplingCB->setDisabled(true);

    GTAAppController* pController = GTAAppController::getGTAAppController();
    if (NULL!=pController)
    {
        QStringList wordList = pController->getVariablesForAutoComplete();
        QCompleter *completer = new QCompleter(wordList, this);
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        ui->ParamNameLE->setCompleter(completer);
        ui->ValueLE->setCompleter(completer);
    }

    bool generateGenericSCXML = TestConfig::getInstance()->getGenericSCXML();
    if(generateGenericSCXML)
    {
        ui->chkRefreshRate->setDisabled(true);
        ui->chkOnlySDI->setDisabled(true);
        ui->CheckSDICB->setDisabled(true);
        ui->chkOnlyParity->setDisabled(true);
        ui->CheckParityCB->setDisabled(true);
    }
}

GTAWaitUntilActionWidget::~GTAWaitUntilActionWidget()
{
    delete ui;
}
QString GTAWaitUntilActionWidget::getParameter() const
{
    return ui->ParamNameLE->text();
}
QString GTAWaitUntilActionWidget::getParameterValue() const
{
    return ui->ValueLE->text();
}
QString GTAWaitUntilActionWidget::getOperator() const
{
    return ui->OperatorCB->currentText();
}
void GTAWaitUntilActionWidget::clear()
{
    ui->ParamNameLE->clear();
    ui->ValueLE->clear();
}

void GTAWaitUntilActionWidget::onSearchPBClicked()
{
    QPushButton * pButtonClicked = dynamic_cast<QPushButton*>(sender());
    if(sender() == ui->SearchPB)
        _isParameterLE = true;
    else
        _isParameterLE = false;

    if(pButtonClicked != NULL && _SearchPBEditMap.contains(pButtonClicked))
    {
        QLineEdit * pEdit = _SearchPBEditMap.value(pButtonClicked);
        emit searchObject(pEdit);
    }
}

bool GTAWaitUntilActionWidget::getActionCommand(GTAActionBase*& opCmd)const
{
    opCmd=NULL;
    if (!isValid()) {
        qWarning() << "Is not valid";
        return false;
    }
        
    
    GTAActionWait *pWaitActionCmd = NULL;
    QString paramName = getParameter();
    QString paramVal = getParameterValue();
    QString oper = getOperator();
    bool isFSOnly = getFSOnlyState();
    bool isValueOnly = getValueOnlyState();
    bool isSDIOnly = getCheckOnlySDIState();
    bool isParityOnly = getCheckOnlyParityState();
    bool isRefreshOnly = getCheckOnlyRefreshRateState();
    QString sSdi = getSDI();
    QString sParity = getParity();
    QString functionalStatus = getFunctionalStatus();
    bool isLoopSampling = getLoopSamplingState();
    QString loopSampling = getLoopSampling();
    QString unitLoopSampling = getUnitLoopSampling();
    pWaitActionCmd = new GTAActionWait(GTAActionWait::UNTIL);
    pWaitActionCmd->setParameter(paramName);
    pWaitActionCmd->setValue(paramVal);
    pWaitActionCmd->setCondition(oper);
    pWaitActionCmd->setIsFsOnly(isFSOnly);
    pWaitActionCmd->setIsValueOnly(isValueOnly);
    pWaitActionCmd->setFunctionalStatus(functionalStatus);
    pWaitActionCmd->setIsSDIOnly(isSDIOnly);
    pWaitActionCmd->setIsParityOnly(isParityOnly);
    pWaitActionCmd->setIsRefreshRateOnly(isRefreshOnly);
    pWaitActionCmd->setSDI(sSdi);
    pWaitActionCmd->setParity(sParity);
    pWaitActionCmd->setIsLoopSampling(isLoopSampling);
    pWaitActionCmd->setLoopSampling(loopSampling);
    pWaitActionCmd->setUnitLoopSampling(unitLoopSampling);
    opCmd=pWaitActionCmd;

    return true;
}

void GTAWaitUntilActionWidget::setActionCommand(const GTAActionBase* ipActionCmd)
{
    clear();
    GTAActionWait * pWaitUntil = dynamic_cast< GTAActionWait *>((GTAActionBase*)ipActionCmd);
    if(pWaitUntil != NULL && pWaitUntil->getWaitType() == GTAActionWait::UNTIL)
    {
        QString param = pWaitUntil->getParameter();
        QString val = pWaitUntil->getValue();
        QString cond = pWaitUntil->getCondition();
        bool isFSOnly = pWaitUntil->getIsFsOnly();
        bool isValueOnly = pWaitUntil->getIsValueOnly();
        QString funcStatus = pWaitUntil->getFunctionalStatus();
        bool isSDI = pWaitUntil->getIsSDIOnly();
        bool isParity   = pWaitUntil->getIsParityOnly();
        bool isRefreshRate = pWaitUntil->getIsRefreshRateOnly();
        QString sSDI = pWaitUntil->getSDI();
        QString sParity = pWaitUntil->getParity();
        bool isLoopSampling = pWaitUntil->getIsLoopSampling();
        QString loopSampling = pWaitUntil->getLoopSampling();
        QString unitLoopSampling = pWaitUntil->getUnitLoopSampling();

        ui->ParamNameLE->setText(param);
        ui->ValueLE->setText(val);
        int index = ui->OperatorCB->findText(cond);
        ui->ChkFSOnly->setChecked(isFSOnly);
        ui->ChkValueOnly->setChecked(isValueOnly);
        ui->chkOnlyParity->setChecked(isParity);
        ui->chkOnlySDI->setChecked(isSDI);
        ui->chkRefreshRate->setChecked(isRefreshRate);
        ui->ChkLoopSampling->setChecked(isLoopSampling);
        ui->LoopSamplingLE->setText(loopSampling);
        ui->LoopSamplingCB->setCurrentText(unitLoopSampling);


        ui->OperatorCB->setCurrentIndex(index);
        index = _FunctionalStatus.indexOf(funcStatus);
        if(index >= 0)
            ui->FunctionalStatusCB->setCurrentIndex(index);

        index = ui->CheckSDICB->findText(sSDI);
        if(index >= 0 && index < ui->CheckSDICB->count())
            ui->CheckSDICB->setCurrentIndex(index);

        index = ui->CheckParityCB->findText(sParity);
        if(index >= 0 && index < ui->CheckParityCB->count())
            ui->CheckParityCB->setCurrentIndex(index);
    }
}
bool GTAWaitUntilActionWidget::isParameterLESelected()
{
    return _isParameterLE;
}
void GTAWaitUntilActionWidget::operatorChanged(QString isOperator)
{
    bool isChecked = (ui->ChkFSOnly->isChecked() || ui->chkRefreshRate->isChecked() || ui->chkOnlySDI->isChecked() || ui->chkOnlyParity->isChecked() || ui->ChkValueOnly->isChecked());
    bool val=false;
    emit conditionOperatorChanged(isOperator);
	emit disablePrecision(val);
    if (isOperator == ARITH_NOTEQ || isOperator == ARITH_EQ)
    {
        if(ui->ChkValueOnly->isChecked() || ui->chkRefreshRate->isChecked())
            val = false;
        else if (!isChecked)
            val = false;
        else
            val = true;
        emit disablePrecision(val);
    }
}


bool GTAWaitUntilActionWidget::isValid()const
{
    QString paramName = getParameter();
    QString paramVal = getParameterValue();
    QString oper = getOperator();
    if(getValueOnlyState() && (paramName.isEmpty() || paramVal.isEmpty() || oper.isEmpty()))
    {
        return false;
    }
    else if((getFSOnlyState() || getCheckOnlySDIState() || getCheckOnlyParityState() || getCheckOnlyRefreshRateState())
            && (paramName.isEmpty() || oper.isEmpty()))
    {
        return false;
    }
    else
        return true;
}
bool GTAWaitUntilActionWidget::hasConfigTime()
{
    GTAAppController *pAppCtrl = GTAAppController::getGTAAppController();
    if(pAppCtrl->isGenericSCXMLMode() == true)
        return false;
    return true;
}

void GTAWaitUntilActionWidget::processSearchInput(const QStringList& iSelectedItemLst)
{

    /*if((actionName == ACT_WAIT) && (complName == COM_WAIT_UNTIL) && (iSelectedItemLst.size()>=23))
    {
        QString precVal = iSelectedItemLst.at(22);
        if(!precVal.isEmpty() && (precVal!= "NA") && _pWaitUntilWgt->isParameterLESelected())
            ui->PrecisionLE->setText(precVal);
    }*/

    if( iSelectedItemLst.size()>=23 )
    {
        QString precVal = iSelectedItemLst.at(22);
        if(!precVal.isEmpty() && (precVal!= "NA") && isParameterLESelected())
            _newPrecison = precVal;
    }
}
int GTAWaitUntilActionWidget::getSearchType()
{
    int retVal = GTAGenSearchWidget::PARAMETER;
    return retVal;
}

bool GTAWaitUntilActionWidget::getFSOnlyState()const
{
    return ui->ChkFSOnly->isChecked();
}

bool GTAWaitUntilActionWidget::getValueOnlyState()const
{
    return ui->ChkValueOnly->isChecked();
}

QString GTAWaitUntilActionWidget::getFunctionalStatus()const
{
    return ui->FunctionalStatusCB->currentText();
}

bool GTAWaitUntilActionWidget::getLoopSamplingState() const
{
    return ui->ChkLoopSampling->isChecked();
}

QString GTAWaitUntilActionWidget::getLoopSampling() const
{
    return ui->LoopSamplingLE->text();
}

QString GTAWaitUntilActionWidget::getUnitLoopSampling() const
{
    return ui->LoopSamplingCB->currentText();
}


void GTAWaitUntilActionWidget::disableValueEditsOnRefresh(bool iStatus)
{
    disableValueEdits(iStatus);
    bool val = false;
    emit disablePrecision(val);
}

void GTAWaitUntilActionWidget::disableValueEdits(bool iStatus)
{
    ui->ValueLE->setDisabled(iStatus);
    ui->SearchValuePB->setDisabled(iStatus);
        ui->FunctionalStatusCB->show();
    hideNote(false);

    if (iStatus==true)
    {
        ui->OperatorCB->setCurrentIndex(0);
        QStringList operatorList;
        operatorList <<ARITH_EQ<<ARITH_NOTEQ;
        ui->OperatorCB->clear();
        ui->OperatorCB->addItems(operatorList);
        hideNote(false);

//        if(ui->ChkFSOnly->isChecked() || ui->chkRefreshRate->isChecked() || ui->chkOnlyParity->isChecked() || ui->chkOnlySDI->isChecked())
//        {
//            ui->checkFsWarLB->show();
//        }
//        else
//        {
//            if (!(ui->ChkValueOnly->isChecked()))
//            {
//                ui->checkFsWarLB->hide();
//                ui->checkValnFS->show();
//            }
//        }

    }
    else
    {
        QStringList operatorList;
        operatorList <<ARITH_EQ<<ARITH_NOTEQ<<ARITH_GT<<ARITH_LTEQ<<ARITH_LT<<ARITH_GTEQ;
        ui->OperatorCB->clear();
        ui->OperatorCB->addItems(operatorList);

    }
    emit disablePrecision(iStatus);
}
void GTAWaitUntilActionWidget::resetCheckFS(bool iStatus)
{
    if(iStatus && ui->ChkFSOnly->isChecked())
        ui->ChkFSOnly->setChecked(false);
}

void GTAWaitUntilActionWidget::enableValueEdits(bool )
{
    if(ui->ChkValueOnly->isChecked())
        disableValueEdits(false);
}

void GTAWaitUntilActionWidget::hideNote(bool )
{
    bool isBenchParamFnsChecked = (ui->ChkFSOnly->isChecked() || ui->chkRefreshRate->isChecked() || ui->chkOnlyParity->isChecked() || ui->chkOnlySDI->isChecked());
    bool isValueChecked = ui->ChkValueOnly->isChecked();
    ui->checkFsWarLB->hide();
    ui->checkFsWarLB->hide();
    //ui->checkFsWarLB->setVisible(isChecked);
    if (!isBenchParamFnsChecked && !isValueChecked)
    {
        ui->checkFsWarLB->hide();
        ui->checkValnFS->show();
    }
    else if (isValueChecked)
    {
        ui->checkValnFS->hide();
        ui->checkFsWarLB->hide();
    }
    else if (isBenchParamFnsChecked)
    {
        ui->checkValnFS->hide();
        ui->checkFsWarLB->show();
    }
}

void GTAWaitUntilActionWidget::resetCheckOnlyValue(bool iStatus)
{
    if(iStatus && ui->ChkValueOnly->isChecked())
        ui->ChkValueOnly->setChecked(false);

}

void GTAWaitUntilActionWidget::resetSDI(bool iStatus)
{

    if(iStatus && ui->chkOnlySDI->isChecked())
    {
        disconnect(ui->chkOnlySDI,SIGNAL(toggled(bool)),this,SLOT(disableValueEdits(bool)));
        ui->chkOnlySDI->setChecked(false);
        connect(ui->chkOnlySDI,SIGNAL(toggled(bool)),this,SLOT(disableValueEdits(bool)));

    }
}

void GTAWaitUntilActionWidget::resetParity(bool iStatus)
{
    if(iStatus && ui->chkOnlyParity->isChecked())
    {
        disconnect(ui->chkOnlyParity, SIGNAL(toggled(bool)),this,SLOT(disableValueEdits(bool)));
        ui->chkOnlyParity->setChecked(false);
        connect(ui->chkOnlyParity, SIGNAL(toggled(bool)),this,SLOT(disableValueEdits(bool)));

    }
}

void GTAWaitUntilActionWidget::resetRefreshRate(bool iStatus)
{
    if(iStatus && ui->chkRefreshRate->isChecked())
    {
        disconnect(ui->chkRefreshRate, SIGNAL(toggled(bool)),this, SLOT(disableValueEditsOnRefresh(bool)));
        ui->chkRefreshRate->setChecked(false);
        connect(ui->chkRefreshRate, SIGNAL(toggled(bool)),this, SLOT(disableValueEditsOnRefresh(bool)));
    }
}

void GTAWaitUntilActionWidget::addLoopSampling(bool )
{
    if(ui->ChkLoopSampling->isChecked())
    {
        disconnect(ui->ChkLoopSampling, SIGNAL(toggled(bool)), this, SLOT(addLoopSampling(bool)));
        ui->ChkLoopSampling->setChecked(true);
        ui->LoopSamplingLE->setDisabled(false);
        ui->LoopSamplingCB->setDisabled(false);
        connect(ui->ChkLoopSampling, SIGNAL(toggled(bool)), this, SLOT(addLoopSampling(bool)));
    }
    else
    {
        disconnect(ui->ChkLoopSampling, SIGNAL(toggled(bool)), this, SLOT(addLoopSampling(bool)));
        ui->ChkLoopSampling->setChecked(false);
        ui->LoopSamplingLE->setDisabled(true);
        ui->LoopSamplingCB->setDisabled(true);
        connect(ui->ChkLoopSampling, SIGNAL(toggled(bool)), this, SLOT(addLoopSampling(bool)));
    }
}

QString GTAWaitUntilActionWidget::getParity()const
{
    if(ui->chkOnlyParity->isChecked())
    {
        return ui->CheckParityCB->currentText();
    }
    else
        return QString();
}

QString GTAWaitUntilActionWidget::getSDI()const
{
    if(ui->chkOnlySDI->isChecked())
    {
        return ui->CheckSDICB->currentText();
    }
    else
        return QString();
}

bool GTAWaitUntilActionWidget::getCheckOnlyRefreshRateState()const
{
    return ui->chkRefreshRate->isChecked();
}

bool GTAWaitUntilActionWidget::getCheckOnlySDIState()const
{
    return ui->chkOnlySDI->isChecked();
}

bool GTAWaitUntilActionWidget::getCheckOnlyParityState()const
{
    return ui->chkOnlyParity->isChecked();
}

/* This function allow to reset all checkBox/ComboBox states
* @return : None
*/
void GTAWaitUntilActionWidget::resetAllCheckBoxStatus()
{
	ui->ChkValueOnly->setChecked(false);
	ui->ChkFSOnly->setChecked(false);
	ui->chkOnlySDI->setChecked(false);
	ui->chkOnlyParity->setChecked(false);
	ui->chkRefreshRate->setChecked(false);

	ui->ChkValueOnly->setEnabled(true);
	ui->ChkFSOnly->setEnabled(true);
	ui->chkOnlySDI->setEnabled(true);
	ui->chkOnlyParity->setEnabled(true);
	ui->chkRefreshRate->setEnabled(true);

	ui->FunctionalStatusCB->setEnabled(true);
	ui->FunctionalStatusCB->setCurrentIndex(0);
	ui->CheckSDICB->setEnabled(true);
	ui->CheckParityCB->setEnabled(true);
}

/* This function allows to automatically tick an option if the Parameter has a Media VMAC or DIS.
* @return : None
*/
void GTAWaitUntilActionWidget::setOnlyValueStatus()
{
	ui->ChkValueOnly->setChecked(true);
	ui->ChkFSOnly->setChecked(false);
	ui->chkOnlySDI->setChecked(false);
	ui->chkOnlyParity->setChecked(false);
	ui->chkRefreshRate->setChecked(false);

	ui->ChkValueOnly->setEnabled(false);
	ui->ChkFSOnly->setEnabled(false);
	ui->chkOnlySDI->setEnabled(false);
	ui->chkOnlyParity->setEnabled(false);
	ui->chkRefreshRate->setEnabled(false);

	ui->FunctionalStatusCB->setEnabled(false);
	ui->FunctionalStatusCB->setCurrentIndex(1);
	ui->CheckSDICB->setEnabled(false);
	ui->CheckParityCB->setEnabled(false);
}

/* This SLOT allows to evaluate the entered Parameter and to detect its media
* @sender: this - textChanged() of the Parameter field
* @input: none
* @return: void
*/
void GTAWaitUntilActionWidget::evaluateParamMedia(const QString &)
{
	ui->ValueLE->setDisabled(false);

	QString param = ui->ParamNameLE->text().trimmed();
	if (!param.isEmpty() && !param.contains('`'))
	{
		GTAAppController* pCtrlr = GTAAppController::getGTAAppController();
		if (NULL != pCtrlr)
		{
			QStringList paramList = QStringList() << param;
			QList<GTAICDParameter> icdDetailList;
			pCtrlr->getICDDetails(paramList, icdDetailList);
			if (!icdDetailList.isEmpty())
			{
				QString iSignalType = icdDetailList[0].getSignalType();
				if (icdDetailList[0].isOnlyValueDueToSignalType(iSignalType))
				/*QString media = icdDetailList[0].getMedia();
				if (media == "VMAC" || media == "DIS" || media == "ANA" || media == "MODEL" || media == "PIR")*/
				{
					// If the Parameter gets a following media, set state of checkboxes
					setOnlyValueStatus();
				}
				else
				{
					// If the Parameter has another media, reset state of checkboxes
					resetAllCheckBoxStatus();
				}
			}
			else
			{
				if ((param.contains("@") && param.count('@', Qt::CaseInsensitive) % 2 == 0) || param.startsWith("line."))
				{
					// If the Parameter has another media, reset state of checkboxes
					resetAllCheckBoxStatus();
				}
				else
				{
					// If the Parameter field contains a local variable, set state of checkboxes
					setOnlyValueStatus();
				}
			}
		}
	}
	else
	{
		// If the Parameter field is empty, reset state of checkboxes
		resetAllCheckBoxStatus();
	}
}

bool GTAWaitUntilActionWidget::setWidgetFields(const QList <QPair<QString, QString>>& params, ErrorMessageList& errorLogs)
{
    bool success = true;
    resetAllCheckBoxStatus();
    for (const auto& param : params) {
        success = success && CheckAndSetParameters(param);
        success = success && CheckAndSetConditionalOperator(param);
        success = success && CheckAndSetCheckboxes(param);
        success = success && CheckAndSetComboboxes(param);
    }

    return isValid() && success;
}

bool GTAWaitUntilActionWidget::CheckAndSetCheckboxes(const QPair<QString, QString>& param) {
    if (param.first == "ChkValueOnly")
        return setCheckboxFromString(ui->ChkValueOnly, param.second);
    else if (param.first == "ChkFSOnly")
        return setCheckboxFromString(ui->ChkFSOnly, param.second);
    else if (param.first == "chkOnlySDI")
        return setCheckboxFromString(ui->chkOnlySDI, param.second);
    else if (param.first == "chkOnlyParity")
        return setCheckboxFromString(ui->chkOnlyParity, param.second);
    else if (param.first == "chkRefreshRate")
        return setCheckboxFromString(ui->chkRefreshRate, param.second);

    return true;
}

bool GTAWaitUntilActionWidget::CheckAndSetComboboxes(const QPair<QString, QString>& param) {
    if (param.first == "CheckSDICB")
        return setComboboxFromString(ui->CheckSDICB, param.second);
    else if (param.first == "CheckParityCB")
        return setComboboxFromString(ui->CheckParityCB, param.second);
    else if (param.first == "FunctionalStatusCB")
        return setComboboxFromString(ui->FunctionalStatusCB, param.second);

    return true;
}

bool GTAWaitUntilActionWidget::setCheckboxFromString(QCheckBox* checkbox, QString booleanStr)
{
    if (booleanStr.toLower().contains("true")) {
        checkbox->setChecked(true);
        return true;
    }
    else if (booleanStr.toLower().contains("false")) {
        checkbox->setChecked(false);
        return true;
    }
    else {
        qWarning() << booleanStr << " is neither true or false, this parameter supports only boolean values";
        return false;
    }
}

/**
 * @brief Method for setting combobox from string
 * @param inputStr Can be either an index number or a field name
*/
bool GTAWaitUntilActionWidget::setComboboxFromString(QComboBox* combobox, QString inputStr)
{
    bool isInt = false;
    auto index = inputStr.toInt(&isInt);
    if (isInt) {
        if (combobox->count() >= index) {
            combobox->setCurrentIndex(index);
            return true;
        }
        else {
            qWarning() << "Combobox given index: " << index << " is greater than maximum index: " << combobox->count();
            return false;
        }
    }
    else {
        index = combobox->findText(inputStr);
        if (index >= 0 && index < ui->CheckSDICB->count()) {
            ui->CheckSDICB->setCurrentIndex(index);
        }
        else {
            qWarning() << "Combobox given index: " << inputStr << " is not valid";
            return false;
        }
    }
}

bool GTAWaitUntilActionWidget::CheckAndSetConditionalOperator(const QPair<QString, QString>& param) {
    auto& condition = param.second;
    if (param.first == "Condition") {
        int index = ui->OperatorCB->findText(condition);
        if (index > 0) {
            ui->OperatorCB->setCurrentIndex(index);
            return true;
        }
        else {
            qWarning() << "Condition " << condition << " is not supported";
            return false;
        }
    }

    return true;
}

bool GTAWaitUntilActionWidget::CheckAndSetParameters(const QPair<QString, QString>& param) {
    if (param.first == "ParamNameLE") {
        ui->ParamNameLE->setText(param.second);
        evaluateParamMedia(param.second);
    }

    if (param.first == "ValueLE") {
        ui->ValueLE->setText(param.second);
        evaluateParamMedia(param.second);
    }

    return true;
}

#include "GTAWgtRegisterDef.h"
REGISTER_ACTION_WIDGET(QString("%1 %2").arg(ACT_WAIT,COM_WAIT_UNTIL),GTAWaitUntilActionWidget,obj)
