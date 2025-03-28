#include "GTASetActionWidget.h"
#include "ui_GTASetActionWidget.h"
#include "GTAEquationWidget.h"
#include "GTAActionSet.h"
#include "GTAEquationConst.h"
#include "GTAUtil.h"
#include "GTAParamValidator.h"
#include "GTAParamValueValidator.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QListWidgetItem>
#include "GTAAppController.h"
#include "GTAICDParameter.h"
#include "GTAGenSearchWidget.h"
#include <QCompleter>
GTASetActionWidget::GTASetActionWidget(QWidget* parent) :
    GTAActionWidgetInterface(parent)/*,_pSetActionCmd(NULL)*/,
    _pEquationWgt(NULL),
    _pMcduWgt(NULL),
    ui(new Ui::GTASetActionWidget)
{
    //    _pSetCmd = NULL;
    ui->setupUi(this);
    _pEquationWgt = new GTAEquationWidget();
    _pEquationWgt->hide();

    _pMcduWgt = new GTAMcduWidget(this);

    //    _pSetActionCmd = new GTAActionSet();

    QStringList vmacForceTypeList;
    vmacForceTypeList<<SET_VMACFORCE_SOL<<SET_VMACFORCE_LIQ;
    ui->VmacForcingTypeCB->addItems(vmacForceTypeList);


    //made this item as hidden, as this function does not generate any specific output in scxml.
    ui->VmacForcingTypeCB->setVisible(false);
    ui->label_3->setVisible(false);

    _sFSList =  QStringList() << ACT_FSSSM_NO<<ACT_FSSSM_ND<<ACT_FSSSM_FW<<ACT_FSSSM_NCD<<ACT_FSSSM_FT<<ACT_FSSSM_MINUS<<ACT_FSSSM_PLUS<<ACT_FSSSM_LOST<<ACT_FSSSM_NOCHANGE;
	// This list must be compared to the parameter name on each adding/editing command
	_forbiddenKeywords = QStringList() << "and" << "bool" << "char" << "cond" << "delay" << "dowhile" << "double" << "else" << "enum" << "error" << "event" << "false" << "float" << "foreach" << "id" << "if" << "initial" << "inline" << "int" << "line" << "na"
									   << "name" << "new" << "not" << "operator" << "or" << "result" << "return" << "sendid" << "status" << "switch" << "target" << "toold_id" << "tool_type" << "true" << "value" << "var" << "while" << "xor";
    ui->functionStatusCB->addItems(_sFSList);
	// By default we want to have NO as Functional Status
	ui->functionStatusCB->setCurrentIndex(0);

    ui->EquationPB->setDisabled(true);
    connect(ui->ParamNameLE, SIGNAL(textChanged(QString)),this,SLOT(onChangeParameterValue(QString)));
    connect(ui->EquationPB,SIGNAL(clicked()),this,SLOT(onEquationButtonClick()));

    connect(_pEquationWgt,SIGNAL(okPressed()),this,SLOT(onEquationWgtOKButtonClick()));
    connect(_pMcduWgt, SIGNAL(okPressed()), this, SLOT(onMCDUWgtOKButtonClick()));
    connect(ui->SearchPB,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));
    connect(ui->SearchPB2,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));
    connect(ui->SearchPB3, SIGNAL(clicked()), this, SLOT(onSearchPBClicked()));
    connect(ui->SearchMCDU, SIGNAL(clicked()), this, SLOT(onMCDUButtonClicked()));
    connect(ui->ValueLE,SIGNAL(textChanged(QString)),this,SLOT(onParameterValueEdited(QString)));
    connect(ui->addToListPB,SIGNAL(clicked()),this,SLOT(onAddSetCommand()));
    connect(ui->removeFromListPB,SIGNAL(clicked()),this,SLOT(onRemoveSetCommand()));
    connect(ui->setActionListLV,SIGNAL(itemSelectionChanged()),this,SLOT(updateParamValueLineEdits()));
    connect(ui->editSelectedPB,SIGNAL(clicked()),this,SLOT(editSetCommand()));
    connect(ui->VmacForcingTypeCB,SIGNAL(currentIndexChanged(int)),SLOT(updateVmacForceType(int)));
    connect(ui->importFromCSVPB,SIGNAL(clicked()),this,SLOT(importParametersFromCSV()));
    connect(ui->SetOnlyFSCB,SIGNAL(toggled(bool)),this,SLOT(disableLineEdits(bool)));
    connect(ui->SetOnlyFSTable, SIGNAL(toggled(bool)), this, SLOT(disableLineEditsTable(bool)));
    connect(ui->SDIChk,SIGNAL(toggled(bool)),this,SLOT(onSetSDIToggled(bool)));
	connect(ui->TimeCB, SIGNAL(toggled(bool)), this, SLOT(onTimeCBToggled(bool)));
    connect(ui->SetOnlyValueCB,SIGNAL(toggled(bool)),this,SLOT(setFSDisabled(bool)));
	connect(ui->ParamNameLE, SIGNAL(textChanged(const QString &)), this, SLOT(evaluateParamMedia(const QString &)));

    GTAParamValidator* pValidator1 = new GTAParamValidator(ui->ParamNameLE);
    ui->ParamNameLE->setValidator(pValidator1);

    const QString setActionValidationRegex = "([A-Za-z0-9.?\\\\@#$_:\\-;',<>\\s]+)";
    GTAParamValueValidator* pValidator2 = new GTAParamValueValidator(ui->ValueLE, setActionValidationRegex);

    ui->ValueLE->setValidator(pValidator2);
    ui->noteLB->hide();
    ui->SDIChk->hide();
    ui->SetSDICB->hide();


    GTAAppController* pController = GTAAppController::getGTAAppController();
    if (NULL!=pController)
    {
        QStringList wordList = pController->getVariablesForAutoComplete();
        QCompleter *completer = new QCompleter(wordList, this);
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        ui->ParamNameLE->setCompleter(completer);
        ui->ValueLE->setCompleter(completer);
    }

	_paramWithoutFS = false;
}



GTASetActionWidget::~GTASetActionWidget()
{
    //     if(_pSetCmd != NULL)
    //     {
    //         delete _pSetCmd;
    //         _pSetCmd = NULL;
    //     }
    
    if(_pEquationWgt != NULL)
    {
        delete _pEquationWgt;
        _pEquationWgt = NULL;
    }
    foreach(GTAActionSet* pSet,_SetCommandList)
    {
        if (pSet)
        {delete pSet;pSet=NULL;}

    }
    _SetCommandList.clear();

    delete ui;
}
void GTASetActionWidget::onEquationButtonClick()
{
    if (ui->setActionListLV->count() != 0)
	{
		if (_pEquationWgt != NULL)
		{
			QList<QListWidgetItem *> lstOfSelecteItems = ui->setActionListLV->selectedItems();
			//Only one selection is allowed, take the top selection
			if (!lstOfSelecteItems.isEmpty())
			{
				QListWidgetItem* pItem = lstOfSelecteItems[0];
				//selectedIndexes () const : QModelIndexLis
				if (NULL != pItem)
				{
					//selected indexed are not directly available for qlistwidget.
					int totoalItems = ui->setActionListLV->count();
					for (int i = 0; i < totoalItems; i++)
					{
						QListWidgetItem* pItemTocompare = ui->setActionListLV->item(i);
						if (pItem == pItemTocompare)
						{
							if (_SetCommandList.size() > i)
							{
								GTAActionSet* pActionSet = _SetCommandList.at(i);
								if (NULL != pActionSet)
								{
									GTAEquationBase* pEqn = pActionSet->getEquation();
									if (NULL != pEqn)
									{
										_pEquationWgt->setEquation(pEqn);
									}
								}
							}
						}
					}
				}
			}
		}
	}
	else
	{
		// We're creating a new EQUATION
		bool rc2 = false;

		QString param = ui->ParamNameLE->text();
		GTAAppController* pController = GTAAppController::getGTAAppController();
		QString dbFile = pController->getSystemService()->getDatabaseFile();
		GTADataController dataCtrlr;
		QHash<QString, GTAICDParameter> paramIsVMAC;
		QStringList invalidParams;

		// Update paramIsVMAC with Parameters from CSV File
		rc2 = dataCtrlr.getParametersDetail(QStringList(param), dbFile, paramIsVMAC, invalidParams);

		GTAICDParameter paramInfo = paramIsVMAC.value(param);
		if (paramInfo.getMedia() == PARAM_TYPE_VMAC)
		{
			_pEquationWgt->setIsVmac(true);
			_pEquationWgt->reset();
		}
		else
		{
			_pEquationWgt->setIsVmac(false); 
			_pEquationWgt->reset();
		}
	}
//    else
//    {
//        _pEquationWgt->reset();
//    }
    _pEquationWgt->show();
}
void GTASetActionWidget::onEquationWgtOKButtonClick()
{
    ui->ValueLE->setReadOnly(false);
    if(_pEquationWgt != NULL)
    {
        _pEquationWgt->hide();
        GTAEquationBase * pEquation = NULL;
        _pEquationWgt->getEquation(pEquation);
        if(pEquation != NULL)
        {
            silentSetValueText(pEquation->getStatement());
            //_pEquation = pEquation;
            if(pEquation->getEquationType() != GTAEquationBase::CONST)
            {
                ui->ValueLE->setReadOnly(true);
            }
        }
    }
}

/**
 * @brief Slot on OK to apply changes from MCDU widget
*/
void GTASetActionWidget::onMCDUWgtOKButtonClick()
{
    ui->ValueLE->setReadOnly(false);
    if (_pMcduWgt != NULL)
    {
        _pMcduWgt->hide();
        QString userSelection = _pMcduWgt->getUserSelection();
        ui->ValueLE->setText(userSelection);
        ui->ValueLE->setReadOnly(true);
    }
}

/**
 * @brief Handles click events on different search buttons and emits corresponding search signals.
 *
 * This function is a slot that gets triggered when any of the search buttons within the GTASetActionWidget are clicked.
 * Depending on which button was clicked, it emits a signal to initiate a search operation for different objects.
 * The function checks the sender of the click event and emits the appropriate signal with the relevant widget as a parameter.
 *
 * The function handles the following buttons:
 * - `ui->SearchPB`: Emits `searchObject` with `ui->ParamNameLE` (Line Edit for Parameter Name).
 * - `ui->SearchPB3`: Emits `searchObject` with `ui->ValueOnlyFSTable` (Table for Only Function Status (Variable)).
 * - `ui->SearchPB2`: Sets an equation in `_pEquationWgt` and emits `searchObject` with `ui->ValueLE` (Line Edit for Value).
 *
 * @note This function is designed to be connected to the clicked signals of multiple buttons.
 */

void GTASetActionWidget::onSearchPBClicked()
{
    if (sender() == ui->SearchPB)
        emit searchObject(ui->ParamNameLE);
    else if (sender() == ui->SearchPB3)
        emit searchObject(ui->ValueOnlyFSTable);
    else if (sender() == ui->SearchPB2)
    {
        QLineEdit* pValLE = ui->ValueLE;
        GTAEquationConst objConst;
        _pEquationWgt->setEquation(&objConst);
        emit searchObject(pValLE);
    }
}

/**
 * @brief Slot that displays a widget containing the list of available MCDU functions
*/
void GTASetActionWidget::onMCDUButtonClicked()
{
    QString value = ui->ValueLE->text();
    _pMcduWgt->setUserSelection(value);
    _pMcduWgt->displayFunctions();
}

void GTASetActionWidget::setActionCommand(const GTAActionBase *ipActionCmd )
{
    clear();
    _SetCommandList.clear();

    ui->ValueLE->setReadOnly(false);
    GTAActionSetList* pActSetLstCmd= dynamic_cast<GTAActionSetList*>((GTAActionBase *)ipActionCmd);
    if(pActSetLstCmd != NULL)
    {
        QList<GTAActionSet*> listOfSetActions;
        pActSetLstCmd->getSetCommandlist(listOfSetActions);
        QString vmacVal = pActSetLstCmd->getVmacForceType();
        foreach(GTAActionSet * pbase, listOfSetActions)
        {
            pbase->setParent(pActSetLstCmd->getParent());
            _SetCommandList.append(dynamic_cast<GTAActionSet*> (pbase->getClone()));
        }
        
        QStringList exprList;
        for(int i=0;i<_SetCommandList.size();i++)
        {
            GTAActionSet* pSetCmd = _SetCommandList.at(i);
            if (NULL!=pSetCmd)
            {
                QString sParameter = pSetCmd->getParameter();
                QString sValue="";
                GTAEquationBase* pEqn = pSetCmd->getEquation();
                QString sFunctionStatus= pSetCmd->getFunctionStatus();
                QString sSDIStatus = pSetCmd->getSDIStatus();
                bool isSetSDI = pSetCmd->getIsSetOnlySDI();
                bool isSetOnlyFSFixed = pSetCmd->getIsSetOnlyFSFixed();
                bool isSetOnlyFSVariable = pSetCmd->getIsSetOnlyFSVariable();
				bool isSetOnlyValue = pSetCmd->getIsSetOnlyValue();
				bool isParamWithoutFS = pSetCmd->getIsParamWithoutFS();
                
                if (NULL!=pEqn)
                {
                    sValue=pEqn->getStatement();
                }
                if (sValue.isEmpty() && (isSetOnlyFSFixed || isSetOnlyFSVariable) && !pSetCmd->getFunctionStatus().isEmpty())
                {
                    exprList.append(QString("%1 (FS:%2) [Set Functional Status]").arg(sParameter,sFunctionStatus));
                }
                else if(sValue.isEmpty() && isSetSDI)
                {
                    exprList.append(QString("%1(SDI:%2) [Set SDI Status]").arg(sParameter,sSDIStatus));
                }
				else if (!sValue.isEmpty() && isSetOnlyValue && isParamWithoutFS)
				{
					exprList.append(QString("%1 = %2").arg(sParameter, sValue));
				}
				else
                {
                    exprList.append(QString("%1(FS:%2) = %3 ").arg(sParameter,sFunctionStatus,sValue));
                }
            }
        }
        ui->setActionListLV->addItems(exprList);
        int index = ui->VmacForcingTypeCB->findText(vmacVal);
        ui->VmacForcingTypeCB->setCurrentIndex(index);
        
        ui->SDIChk->hide();
        ui->SetSDICB->hide();

        _lastEditedRow = pActSetLstCmd->getLastEditedRow();
        selectLastEditedRow();
        
        // Set valueLE to readOnly mode for MCDU parameter
        QString param = ui->ParamNameLE->text();
        ui->ValueLE->setReadOnly(!param.isEmpty() && (param.contains(GTA_MCDU_SEND)));
    }
}

/**
 * @brief Select the last edited item of the command list in order to fill the Parameter and Parameter/Value fields
*/
void GTASetActionWidget::selectLastEditedRow()
{
    if (!_SetCommandList.isEmpty())
    {        
        ui->setActionListLV->item(_lastEditedRow)->setSelected(true);
        _currentParameterName = ui->ParamNameLE->text();
        _currentParameterValue = ui->ValueLE->text();
    }
}

bool GTASetActionWidget::getActionCommand(GTAActionBase*& opCmd)const
{
    bool rc = false;
    // Selected Parameter 
    if (!_currentParameterName.isEmpty())
    {
        // Got empty value
        if (!(ui->SetOnlyFSCB->isChecked() || ui->SetOnlyFSTable->isChecked()) && _currentParameterValue.isEmpty())
        {
            opCmd = nullptr;
            _LastError = QString("%1 field for %2 (%3) must not be empty").arg(ui->label_2->text(), ui->label->text(), _currentParameterName);
            return rc;
        }        
        
        if (!_newParameterToBeAdded         // if the command list contains the parameter already
            && !_SetCommandList.isEmpty())  // if it is not the first parameter to be added
        {
            // Edit automatically
            emit ui->editSelectedPB->clicked();
        }
        else
        {            
            // Add automatically
            emit ui->addToListPB->clicked();
        }

    }
    else
    {
        // If nothing is selected
        if (!ui->ParamNameLE->text().isEmpty() && !ui->ValueLE->text().isEmpty())
        {
            // Add automatically
            emit ui->addToListPB->clicked();
        }
    }


    // Set command list
    GTAActionSetList * pSetCommand = NULL;
    int nbOfItems = ui->setActionListLV->count();
    if (nbOfItems > 0)
    {
        if (!_SetCommandList.isEmpty())
        {
            pSetCommand = new GTAActionSetList(QString(ACT_SET),"");
            pSetCommand->setSetCommandlist(_SetCommandList);
            pSetCommand->setVmacForceType(getVmacForceType());
            pSetCommand->setLastEditedRow(_lastEditedRow);
            opCmd = pSetCommand;
            rc=true;

        }
    }
    
    return rc;
}
void GTASetActionWidget::onParameterValueEdited(const QString &iValue)
{
    GTAEquationConst * pConstEq = NULL;

    GTAEquationBase * pEq = NULL;
    _pEquationWgt->getEquation(pEq);
    if(pEq != NULL && pEq->getEquationType() == GTAEquationBase::CONST)
    {
        pConstEq = dynamic_cast<GTAEquationConst*>(pEq);
    }
    else
    {
        if (ui->ValueLE->isReadOnly()==false)//if its readonly then the LE has non constant eqn value.
        {
            delete pEq;
            pEq = NULL;
            pConstEq = new GTAEquationConst();
        }

    }

    if(pConstEq != NULL)
    {
        pConstEq->setConstant(iValue);
        _pEquationWgt->setEquation(pConstEq);
    }
    _currentParameterValue = _currentParameterName.isEmpty() ? QString() : iValue;
}
void GTASetActionWidget::clear()
{
    ui->ParamNameLE->clear();
    ui->ValueLE->clear();
    _pEquationWgt->clear();
    _pEquationWgt->setEquation(NULL);
    ui->setActionListLV->clear();
    ui->functionStatusCB->setCurrentIndex(0);
    ui->SetOnlyFSCB->setCheckState(Qt::Unchecked);
    ui->SetSDICB->setCurrentIndex(0);
    ui->SDIChk->setCheckState(Qt::Unchecked);
    ui->SetOnlyValueCB->setCheckState(Qt::Unchecked);
    _currentParameterName.clear();
    _currentParameterValue.clear();
    _newParameterToBeAdded = false;
}
void GTASetActionWidget::onAddSetCommand()
{
    QString sParameter = ui->ParamNameLE->text();
    sParameter.remove(QRegExp("\\s+"));
    //(ui->functionStatusCB->currentIndex());

    if (sParameter.isEmpty())
    {
        if (this->isVisible())
            QMessageBox::critical(this,"Empty",QString("Enter valid value for %1 field").arg(ui->label->text()));
        return ;
    }
	if (_forbiddenKeywords.contains(sParameter, Qt::CaseInsensitive))
	{
        if (this->isVisible())
            QMessageBox::critical(this, "Parameter Name", QString("The Parameter (%1) has been blocked, please change it").arg(sParameter));
		return;
	}
    if (!_SetCommandList.isEmpty())
    {   
        QListWidgetItem* selection = ui->setActionListLV->selectedItems().first();
        if (this->isVisible() && selection && selection->text().split("(")[0] == sParameter)
        {
            selectLastEditedRow();
            _newParameterToBeAdded = false;
            return;
        }        
    }

    QString sValue = ui->ValueLE->text();
    sValue.remove(QRegExp("\\s+"));

    if (sValue.isEmpty() && ui->SetOnlyFSCB->isChecked()==false && ui->SDIChk->isChecked() == false && ui->SetOnlyFSTable->isChecked() == false)
    {
        if (this->isVisible())
            QMessageBox::critical(this,"Empty",QString("Enter valid value for %1 field").arg(ui->label_2->text()));
        return ;
    }

    QString vmacForceType = ui->VmacForcingTypeCB->currentText();
    GTAEquationBase* pEquation = NULL;
    bool rc = _pEquationWgt->getEquation(pEquation);
    if (pEquation)
    { 
        if(pEquation->getEquationType() == GTAEquationBase::CONST)
        {
            GTAEquationConst * pConst = dynamic_cast<GTAEquationConst*> (pEquation);

            if (pConst)
            {

                QString constantVal = pConst->getConstant();
                if (constantVal.isEmpty())
                {
                    pConst->setConstant(sValue);
                    constantVal=sValue;
                }
            }
        }
    }
    if (rc)
    {
        rc = addSetCommand(sParameter,pEquation);
        if(rc==false)
        {
            if (this->isVisible())
                QMessageBox::critical(this,"Error",_LastError);
            return ;
        }
        else
        {
            _lastEditedRow = _SetCommandList.count() - 1;
            selectLastEditedRow();
            _newParameterToBeAdded = false;
        }
    }
}

/* This function is called when the Add button is pressed
 * @input: isParam - QString which contain the ParameterName
 * @input: ipEqn - GTAEquationBase which contain the current Equation
*/
bool GTASetActionWidget::addSetCommand(QString isParam, GTAEquationBase* ipEqn)
{
	bool rc = false;
	if (isParam.isEmpty())
        return false;

    QStringList parameterListForChannel = GTAUtil::getProcessedParameterForChannel(isParam,true,true);

    QString sFunctionStatusFixed = ui->functionStatusCB->currentText();
    QString sFunctionStatusVariable = ui->ValueOnlyFSTable->text();
    //BVU QString sSDIStatus = ui->SetSDICB->currentText();
    QList<GTAEquationBase*> lstEqn; 
    
	bool setFsStatusFixed = ui->SetOnlyFSCB->isChecked();
    bool setFsStatusVariable = ui->SetOnlyFSTable->isChecked();
    bool setOnlyValue = ui->SetOnlyValueCB->isChecked();
    //BVU bool isSetSDI = ui->SDIChk->isChecked();
    
	QStringList paramValues;
	// Get the Value
    QString sValue = ui->ValueLE->text();
    sValue.remove(QRegExp("\\s+"));

    if (ipEqn)
    { 
        if(setFsStatusFixed || setFsStatusVariable)
        {
            delete ipEqn;ipEqn=NULL;
            lstEqn.append(NULL);
        }
        /*BVU else if(isSetSDI)
        {
            delete ipEqn;ipEqn=NULL;
            lstEqn.append(NULL);
        }*/
        else if (ipEqn->getEquationType() == GTAEquationBase::CONST)
        {
            GTAEquationConst * pConst = dynamic_cast<GTAEquationConst*> (ipEqn);
            if(pConst)
            {
                lstEqn.append(pConst);
                paramValues = GTAUtil::getProcessedParameterForChannel(pConst->getConstant(),true,true);
                if (paramValues.size()==2)
                {
                    pConst->setConstant(paramValues.at(0));
                    GTAEquationConst * pSecondConst = new GTAEquationConst;
                    pSecondConst->setConstant(paramValues.at(1));
                    lstEqn.append(pSecondConst);
                }
                else if (paramValues.size()==1)
                {
                    pConst->setConstant(paramValues.at(0));
                    for(int i=1; i<parameterListForChannel.size(); i++)
                        lstEqn.append(pConst->getClone());
                }
            }
        }
        else
        {
            lstEqn.append(ipEqn);
            for(int i=1; i<parameterListForChannel.size(); i++)
                lstEqn.append(ipEqn->getClone());
        }
    }
    
    int noOfEqns = lstEqn.size();
    if (parameterListForChannel.size() == 1 && noOfEqns>1)
    {
        _LastError = "Invalid channel combination\nLHS can not be one parameter.";
        return rc;
    }
    else if (parameterListForChannel.size() == noOfEqns)
    {
        for (int i=0; i<parameterListForChannel.size(); i++)
        {
            GTAActionSet* pSetCmd = new GTAActionSet(ACT_SET_INSTANCE,"");
            pSetCmd->setParameter(parameterListForChannel.at(i));

            GTAEquationBase* pEquationToInsert = NULL;
            pEquationToInsert = lstEqn.at(i);
            
            pSetCmd->setEquation(pEquationToInsert);
            if (setFsStatusFixed)
                pSetCmd->setFunctionStatus(sFunctionStatusFixed);
            else if (setFsStatusVariable)
                pSetCmd->setFunctionStatus(sFunctionStatusVariable);
            else if (!setFsStatusVariable && !sFunctionStatusVariable.isEmpty())
                pSetCmd->setFunctionStatus(sFunctionStatusVariable);

            if (setFsStatusFixed)
                pSetCmd->setIsSetOnlyFSFixed(true);
            else if (setFsStatusVariable)
                pSetCmd->setIsSetOnlyFSVariable(true);

            pSetCmd->setIsSetOnlyValue(setOnlyValue);
            /*pSetCmd->setSDIStatus(sSDIStatus);
            pSetCmd->setIsSetOnlySDI(isSetSDI);*/

            _SetCommandList.push_back(pSetCmd);

            QListWidgetItem* pItem = new QListWidgetItem(ui->setActionListLV);
            if (setFsStatusFixed)
            {
                if(pEquationToInsert)
                {
					delete pEquationToInsert;pEquationToInsert=NULL;
				}
                pSetCmd->setEquation(pEquationToInsert);
                pItem->setText(QString("%1 (FS:%2) [Set Functional Status]").arg(parameterListForChannel.at(i), sFunctionStatusFixed));
            }
            else if (setFsStatusVariable)
            {
                if (pEquationToInsert)
                {
                    delete pEquationToInsert; pEquationToInsert = NULL;
                }
                pSetCmd->setEquation(pEquationToInsert);
                pItem->setText(QString("%1 (FS:%2) [Set Functional Status]").arg(parameterListForChannel.at(i), sFunctionStatusVariable));
            }
            /*else if(isSetSDI)
            {
                if(pEquationToInsert)
                {
                    delete pEquationToInsert;
                    pEquationToInsert=NULL;
                }
                pSetCmd->setEquation(pEquationToInsert);
                pItem->setText(QString("%1(SDI:%2) [Set SDI Status]").arg(parameterListForChannel.at(i),sSDIStatus));
            }*/
			else if (setOnlyValue)
			{
				GTAEquationConst * pConst = dynamic_cast<GTAEquationConst*> (pEquationToInsert);
				// Configure the ActionSet class (legacy)
				pSetCmd->setIsParamWithoutFS(getParamWithoutFS());
				// The displayed text depends on the paramWithoutFS value (VMAC,PIR,MODEL,ANA,DIS and local variable)
				if (pConst && getParamWithoutFS())
				{
					pItem->setText(QString("%1 = %2 ").arg(parameterListForChannel.at(i), pConst->getConstant()));
				}
				else
				{
					pItem->setText(QString("%1(FS:%2) = %3 ").arg(parameterListForChannel.at(i), sFunctionStatusFixed, sValue));
				}
			}
            else
            {
                GTAEquationConst* pConst = dynamic_cast<GTAEquationConst*>(pEquationToInsert);
                QString functionStatus = sFunctionStatusVariable.isEmpty() ? sFunctionStatusFixed : sFunctionStatusVariable;
                QString value = pConst ? pConst->getConstant() : sValue;

                pItem->setText(QString("%1 (FS:%2) = %3 ").arg(parameterListForChannel.at(i), functionStatus, value));
            }
            ui->setActionListLV->addItem(pItem);
            ui->setActionListLV->setCurrentIndex(QModelIndex());
        }
        rc = true;
    }
    return rc;
}

/* This SLOT is called when Remove button is pressed 
 * @info: We remove the indexed SET command of the SetCommandList
 * @return: none
*/
void GTASetActionWidget::onRemoveSetCommand()
{
    QList<QListWidgetItem *> listOfSelecteItems =  ui->setActionListLV->selectedItems();

    //Only one selection is allowed, take the top selection
    if(!listOfSelecteItems.isEmpty())
    {
        QListWidgetItem* pItem = listOfSelecteItems[0];
        //selectedIndexes () const : QModelIndexList
        if(NULL != pItem)
        {
            // Selected indexed are not directly available for qlistwidget.
            for (int i=0; i<ui->setActionListLV->count(); i++)
            {
                QListWidgetItem* pItemTocompare = ui->setActionListLV->item(i);
                if (pItem == pItemTocompare)
                {
                    if (_SetCommandList.size()>i)
                    {
                        GTAActionSet* pActionSet = _SetCommandList.takeAt(i);
                        if (NULL != pActionSet)
                        {
                            delete pActionSet;
							pActionSet = NULL;
                        }
                        ui->setActionListLV->removeItemWidget(pItem);
                        delete pItem;
						pItem = NULL;
                        ui->setActionListLV->clearSelection();
                        ui->ParamNameLE->setText("");
                        silentSetValueText("");
                    }
                }
            }
        }
        _lastEditedRow = _SetCommandList.count() - 1;
        selectLastEditedRow();
    }
    else
    {
        if (this->isVisible())
            QMessageBox::critical(this,"Empty",QString("Nothing selected for removal"));
    }
}

/* This function is called when one SET command action is selected in the Set Command List
 * @info: Allow to reload the configuration of the SET command
 * @return: none
*/
void GTASetActionWidget::updateParamValueLineEdits()
{
    QList<QListWidgetItem *> listOfSelecteItems =  ui->setActionListLV->selectedItems();
    //Only one selection is allowed, take the top selection
    if(!listOfSelecteItems.isEmpty())
    {
        QListWidgetItem* pItem = listOfSelecteItems[0];
        // SelectedIndexes () const : QModelIndexLis
        if(NULL!=pItem)
        {
            // Selected indexed are not directly available for qlistwidget.
            for (int i=0; i<ui->setActionListLV->count(); i++)
            {
                QListWidgetItem* pItemTocompare = ui->setActionListLV->item(i);
                if (pItem == pItemTocompare)
                {
                    if (_SetCommandList.size()>i)
                    {
                        GTAActionSet* pActionSet = _SetCommandList.at(i);
                        if (NULL != pActionSet)
                        {
                            QString paramName = pActionSet->getParameter();
                            _currentParameterName = paramName;
                            _currentParameterValue = pActionSet->getValue();
                            ui->ParamNameLE->setText(paramName);                           

							GTAEquationBase* pEqn;
							pEqn = pActionSet->getEquation();

                            if (NULL!=pEqn)
                            {
                                silentSetValueText(pEqn->getStatement());
                                ui->SetOnlyFSCB->setChecked(pActionSet->getIsSetOnlyFSFixed());
                                ui->SetOnlyFSTable->setChecked(pActionSet->getIsSetOnlyFSVariable());
                                ui->SDIChk->setChecked(pActionSet->getIsSetOnlySDI());
                                ui->SetOnlyValueCB->setChecked(pActionSet->getIsSetOnlyValue());

                                _pEquationWgt->setEquation(pEqn);
                                if(pEqn->getEquationType() == GTAEquationBase::CONST)
                                {
                                    ui->ValueLE->setReadOnly(false);
                                }
								else
								{
									ui->ValueLE->setReadOnly(true);
								}
                                ui->ValueLE->setDisabled(false);
                                ui->SetSDICB->setCurrentIndex(0);

                                int dIndex = ui->functionStatusCB->findText(pActionSet->getFunctionStatus());
								if (dIndex >= 0 && dIndex < ui->functionStatusCB->count())
								{
									ui->functionStatusCB->setCurrentIndex(dIndex);
								}
                                else
                                {
                                    ui->ValueOnlyFSTable->setText(pActionSet->getFunctionStatus());
                                }
                            }
                            else if(pActionSet->getIsSetOnlyFSFixed())
                            {
                                silentReSetSDICB();
                                silentReSetValueCB();
                                silentReSetFSValue();
                                silentSetFSCB();
                                ui->ValueLE->setDisabled(true);
                                ui->EquationPB->setDisabled(true);
                                ui->SearchPB2->setDisabled(true);
                                ui->SetSDICB->setCurrentIndex(0);
                                ui->SetOnlyFSTable->setDisabled(true);
                                ui->ValueOnlyFSTable->setDisabled(true);
                                ui->SearchPB3->setDisabled(true);

                                int dIndex = ui->functionStatusCB->findText(pActionSet->getFunctionStatus());
								if (dIndex >= 0 && dIndex < ui->functionStatusCB->count())
								{
									ui->functionStatusCB->setCurrentIndex(dIndex);
								}
                            }
                            else if (pActionSet->getIsSetOnlyFSVariable())
                            {
                                silentReSetSDICB();
                                silentReSetValueCB();
                                silentReSetFSCB();
                                silentSetFSValue();
                                ui->ValueLE->setDisabled(true);
                                ui->EquationPB->setDisabled(true);
                                ui->SearchPB2->setDisabled(true);
                                ui->SetSDICB->setCurrentIndex(0);
                                ui->functionStatusCB->setCurrentIndex(0);

                                ui->ValueOnlyFSTable->setText(pActionSet->getFunctionStatus());
                            }
                            else if(pActionSet->getIsSetOnlySDI())
                            {
                                silentReSetFSCB();
                                silentReSetValueCB();
                                silentReSetFSValue();
                                silentSetSDICB();
                                ui->ValueLE->setDisabled(true);
                                ui->EquationPB->setDisabled(true);
                                ui->SearchPB2->setDisabled(true);
                                ui->functionStatusCB->setCurrentIndex(0);

                                int dIndex = ui->SetSDICB->findText(pActionSet->getSDIStatus());
								if (dIndex >= 0 && dIndex < ui->SetSDICB->count())
								{
									ui->SetSDICB->setCurrentIndex(dIndex);
								}
                            }
                        }
                    }
                }
            }
        }
    }
}

/* This function is called when EditCommand is pressed
 * We compare the current couple parameter/value with the former if existing
 * @return: None
*/
void GTASetActionWidget::editSetCommand()
{
    // Get the Parameter field
	QString sParameter = ui->ParamNameLE->text();
    sParameter.remove(QRegExp("\\s+"));

    if (sParameter.isEmpty())
    {
        if (this->isVisible())
            QMessageBox::critical(this,"Empty",QString("Enter valid value for %1 field").arg(ui->label->text()));
        return;
    }

	if (_forbiddenKeywords.contains(sParameter, Qt::CaseInsensitive))
	{
        if (this->isVisible())
            QMessageBox::critical(this, "Parameter Name", QString("The Parameter (%1) has been blocked, please change it").arg(sParameter));
		return;
	}

	// Get the Parameter Value
    QString sValue = ui->ValueLE->text();
    sValue.remove(QRegExp("\\s+"));

    if (sValue.isEmpty() && ui->SetOnlyFSCB->isChecked() == false/* && ui->SDIChk->isChecked() == false*/ && ui->SetOnlyFSTable->isChecked()==false)
    {
        if (this->isVisible())
            QMessageBox::critical(this,"Empty",QString("Enter valid value for %1 field").arg(ui->label_2->text()));
        return;
    }
    if (sValue.isEmpty() && ui->SetOnlyValueCB->isChecked() == true)
    {
        if (this->isVisible())
            QMessageBox::critical(this,"Empty",QString("Enter valid value for %1 field").arg(ui->label_2->text()));
        return;
    }

    QList<QListWidgetItem *> listOfSelecteItems =  ui->setActionListLV->selectedItems();
    //Only one selection is allowed, take the top selection
    if(!listOfSelecteItems.isEmpty())
    {
        QListWidgetItem* pItem = listOfSelecteItems[0];
		if(NULL != pItem)
        {
            // Selected indexed are not directly available for qlistwidget.
            int totalItems = ui->setActionListLV->count();

            for (int i=0;i<totalItems;i++)
            {
                QListWidgetItem* pItemTocompare = ui->setActionListLV->item(i);
                if (pItem == pItemTocompare)
                {
                    if (_SetCommandList.size()>i)
                    {
                        GTAActionSet* pActionSet = _SetCommandList.at(i);
                        if (NULL!=pActionSet)
                        {
                            pActionSet->setParameter(sParameter);
                            ui->ParamNameLE->setText(pActionSet->getParameter());
                            GTAEquationBase* pEqn=NULL;
                            _pEquationWgt->getEquation(pEqn);

                            GTAEquationBase* pExistingEqn = nullptr;
                            pExistingEqn = pActionSet->getEquation();
                            //write an operator overload on all classes '==' and update the line edit if the objects mismatch
                            //if there is a valid equation from equation widget
                            if (pExistingEqn != nullptr && pEqn != nullptr)
                            {
                                // Here compare the equation type
                                // check if the equation has changed and update equation + parameter accordinly
                                if (pExistingEqn->getEquationType() == pEqn->getEquationType())
                                {
                                    if(pExistingEqn == pEqn)
                                    {
                                        // update param name
                                        pActionSet->setParameter(sParameter);
                                        ui->ParamNameLE->setText(pActionSet->getParameter());
                                    }
                                    else
                                    {
                                        // update param name and value
                                        pActionSet->setParameter(sParameter);
                                        ui->ParamNameLE->setText(pActionSet->getParameter());
                                        pActionSet->setEquation(pEqn);
                                        silentSetValueText(pEqn->getStatement());
                                    }
                                }
                                //equation type is not same but there is a valid equation from the widget
                                else
                                {
									pActionSet->setParameter(sParameter);
									ui->ParamNameLE->setText(pActionSet->getParameter());
									pActionSet->setEquation(pEqn);
                                    silentSetValueText(pEqn->getStatement());
                                }								
                            }
                            //if a cancel is pressed on the equation widget
                            else if (pExistingEqn != nullptr && pEqn == nullptr)
                            {
                                pActionSet->setParameter(sParameter);
                                ui->ParamNameLE->setText(pActionSet->getParameter());
                            }
							else
							{
								pActionSet->setParameter(sParameter);
								ui->ParamNameLE->setText(pActionSet->getParameter());
								pActionSet->setEquation(pEqn);
								silentSetValueText(pEqn->getStatement());
							}

							//on set only FS option, the existing function does not have a equationbase pointer
							//the check is made to manage the read only value of the parameter tag if the current equation is a const equation
							if (pExistingEqn != nullptr)
							{
								if(pExistingEqn->getEquationType() == GTAEquationBase::CONST)
									ui->ValueLE->setReadOnly(false);
								else
									ui->ValueLE->setReadOnly(true);
							}                     

                            QString sFunctionStatusFixed =ui->functionStatusCB->currentText();
                            QString sFunctionStatusVariable = ui->ValueOnlyFSTable->text();
                            pActionSet->setIsSetOnlyFSVariable(ui->SetOnlyFSTable->isChecked());
                            pActionSet->setIsSetOnlyFSFixed(ui->SetOnlyFSCB->isChecked());

                            if (ui->SetOnlyFSTable->isChecked())
                            {
                                pActionSet->setFunctionStatus(sFunctionStatusVariable);
                            }
                            else if (!ui->SetOnlyFSTable->isChecked() && !ui->SetOnlyFSCB->isChecked() && !sFunctionStatusVariable.isEmpty())
                            {
                                pActionSet->setFunctionStatus(sFunctionStatusVariable);
                            }
                            else 
                            {
                                pActionSet->setFunctionStatus(sFunctionStatusFixed);
                            }

                            QString sSDIStatus = ui->SetSDICB->currentText();
                            pActionSet->setSDIStatus(sSDIStatus);
                            pActionSet->setIsSetOnlySDI(ui->SDIChk->isChecked());

                            pActionSet->setIsSetOnlyValue(ui->SetOnlyValueCB->isChecked());

                            if (ui->SetOnlyFSCB->isChecked())
                            {
                                GTAEquationBase* pEqnBogus = nullptr;
                                pActionSet->setEquation(pEqnBogus);
                                silentSetValueText("");
                                pItem->setText(QString("%1 (FS:%2) [Set Functional Status]").arg(sParameter, sFunctionStatusFixed));
                            }
                            else if (ui->SetOnlyFSTable->isChecked())
                            {
                                GTAEquationBase* pEqnBogus = nullptr;
                                pActionSet->setEquation(pEqnBogus);
                                silentSetValueText("");
                                pItem->setText(QString("%1 (FS:%2) [Set Functional Status]").arg(sParameter, sFunctionStatusVariable));
                            }
                            else if(ui->SDIChk->isChecked())
                            {
                                GTAEquationBase* pEqnBogus = nullptr;
                                pActionSet->setEquation(pEqnBogus);
                                pItem->setText(QString("%1 (SDI:%2) [Set SDI Status]").arg(sParameter, sSDIStatus));
                            }
							else if (ui->SetOnlyValueCB->isChecked())
							{
								// Configure the ActionSet class (legacy)
								pActionSet->setIsParamWithoutFS(getParamWithoutFS());
								// The displayed text depends on the paramWithoutFS value (VMAC,PIR,MODEL,ANA,DIS and local variable)
								if(getParamWithoutFS())
									pItem->setText(QString("%1 = %2 ").arg(sParameter, sValue));
								else
									pItem->setText(QString("%1(FS:%2) = %3 ").arg(sParameter, sFunctionStatusFixed, sValue));
							}
                            else if (!ui->SetOnlyFSTable->isChecked() && !sFunctionStatusVariable.isEmpty())
                                pItem->setText(QString("%1(FS:%2) = %3 ").arg(sParameter, sFunctionStatusVariable, sValue));
							else
								pItem->setText(QString("%1(FS:%2) = %3 ").arg(sParameter, ui->SetOnlyValueCB->isChecked() ? QString(ACT_FSSSM_NO) : sFunctionStatusFixed, sValue));
                        }
                    }
                    _lastEditedRow = i;                    
                    break;
                }
            }
        }
    }
    else
    {
        if (this->isVisible())
            QMessageBox::critical(this,"Empty",QString("Nothing selected for editing"));
    }
}

void GTASetActionWidget::updateVmacForceType(int)
{
    _vMacForceType=ui->VmacForcingTypeCB->currentText();
}
QString GTASetActionWidget::getVmacForceType()const
{
    return ui->VmacForcingTypeCB->currentText();

}
void GTASetActionWidget::onCancel()
{
    /*  foreach(GTAActionSet* pSet,_SetCommandList)
    {
        if (pSet)
        {delete pSet;pSet=NULL;}
        
    }
    _SetCommandList.clear();*/
}
void GTASetActionWidget::importParametersFromCSV()
{

    QString sCsvFilePath = QFileDialog::getOpenFileName(this, tr("Select Database"),"", tr("Excel File (*.csv)"));
    fillExpLWFromCSV(ui->setActionListLV,sCsvFilePath);


}

void GTASetActionWidget::fillExpLWFromCSV(QListWidget* , const QString& iFilePath)
{
	QFileInfo iFileInfo(iFilePath);
    bool rC = true;

    QStringList validLines;
    if (iFileInfo.exists())
    {
        QFile csvFileObj(iFilePath);
        bool fOpenRC = csvFileObj.open(QIODevice::Text| QFile::ReadOnly);
        int lineNo=0;
        QStringList erroLines;
        if(fOpenRC)
        {
            while(!csvFileObj.atEnd())
            {
                lineNo++;
                QString line = csvFileObj.readLine();
                line = line.trimmed();
                if (validateRowFromCSV(line))
                    validLines.append(line);
                else
                {
                    rC = false;;
                    erroLines.append(QString::number(lineNo));
                }
            }
        }

        if (rC)
        {
            foreach(QString line, validLines)
            {
                QStringList paramInfoLst = line.split(";");
                QString sParameter = paramInfoLst.at(0);
                sParameter=sParameter.trimmed();

                QString sValue = paramInfoLst.at(1);
                sValue=sValue.trimmed();
                //associate current value to const.
                GTAActionSet* pSetCmd = new GTAActionSet(ACT_SET_INSTANCE,"");
                if (sValue.isEmpty())
                {
                    pSetCmd->setIsSetOnlyFSFixed(true);
                }
                else
                {
                    GTAEquationConst* pEquationConst = new GTAEquationConst;
                    GTAEquationBase* pEquation = pEquationConst;
                    pEquationConst->setConstant(sValue);
                    pSetCmd->setIsSetOnlyFSFixed(false);
                    pSetCmd->setEquation(pEquation);
                }

                QString sFS = paramInfoLst.at(2);
                sFS=sFS.trimmed();
                pSetCmd->setParameter(sParameter);
                pSetCmd->setFunctionStatus(sFS);

                _SetCommandList.push_back(pSetCmd);
                
                //update UI - set listlist widget
                QString sStatement;
                if (pSetCmd->getIsSetOnlyFSFixed()==true)
                {
                    sStatement = QString("%1(FS:%2) [Set Functional Status] ").arg(sParameter,sFS);
                }
                else
                    sStatement = QString("%1(FS:%2) = %3 ").arg(sParameter,sFS,sValue);
                
                ui->setActionListLV->addItem(sStatement);
            }
        }
        else
            if (this->isVisible())
                QMessageBox::critical(0,"Parse Error",QString("Error encountered in line(s) [%1]").arg(erroLines.join(",")));
        
        if(fOpenRC)
            csvFileObj.close();
    }
}

bool GTASetActionWidget::validateRowFromCSV(const QString& iLine)
{
    bool rC = true;
    if (!iLine.isEmpty())
    {
        QStringList paramInfoLst = iLine.split(";");
        if (paramInfoLst.size()==3)
        {
            
            QString sParameter = paramInfoLst.at(0);
            sParameter=sParameter.trimmed();

            QString sValue = paramInfoLst.at(1);
            sValue=sValue.trimmed();

            QString sFS = paramInfoLst.at(2);
            sFS=sFS.trimmed();



            if (sParameter.contains(" ")||sFS.contains(" "))
            {
                rC = false;
            }
            else           
                if (!_sFSList.contains(sFS))
                {
                    rC=false;

                }
            


        }
        else
            rC = false;
    }

    return rC;
}

QList<QWidget*>  GTASetActionWidget::getWidgetsInTabOrder()const
{
    QList<QWidget*> lstOfWidgets;
    lstOfWidgets <<ui->ParamNameLE <<ui->functionStatusCB <<ui->importFromCSVPB <<ui->SearchPB
                <<ui->ValueLE <<ui->EquationPB<<ui->setActionListLV<<ui->addToListPB
               <<ui->removeFromListPB <<ui->editSelectedPB <<ui->VmacForcingTypeCB;
    return lstOfWidgets;

}
QWidget*  GTASetActionWidget::getLastWidgetForTabOrder()const
{
    return ui->VmacForcingTypeCB;
}

bool GTASetActionWidget::isParameterFromDB()
{/*
    bool rc = false;
    QString paramText = ui->ParamNameLE->text();
    if(paramText.isEmpty())
        return rc;
    GTAAppController* pCtrlr = GTAAppController::getGTAAppController();
    if (NULL!=pCtrlr)
    {

        QStringList paramList = QStringList()<<paramText;
        QList<GTAICDParameter> icdDetailList;
        pCtrlr->getICDDetails(paramList,icdDetailList);
        if (!icdDetailList.isEmpty())
            rc = true;


    }
    
    return rc;
    */


    // do not use this function right now.
    return true;

    
}
void GTAActionSetValueLineEdit::keyPressEvent(QKeyEvent * event)
{
    emit keyPressedForValueLE(this->text());
    this->QLineEdit::keyPressEvent(event);
}
GTAActionSetValueLineEdit::GTAActionSetValueLineEdit(QWidget* parent):QLineEdit(parent)
{

}

void GTASetActionWidget::silentReSetSDICB()
{

    disconnect(ui->SDIChk,SIGNAL(toggled(bool)),this,SLOT(onSetSDIToggled(bool)));
    ui->SDIChk->setChecked(false);
    ui->SetOnlyFSCB->setEnabled(true);
    ui->SetOnlyValueCB->setEnabled(true);
    ui->functionStatusCB->setEnabled(true);
   connect(ui->SDIChk,SIGNAL(toggled(bool)),this,SLOT(onSetSDIToggled(bool)));
}

void GTASetActionWidget::silentSetSDICB()
{
    disconnect(ui->SDIChk,SIGNAL(toggled(bool)),this,SLOT(onSetSDIToggled(bool)));
    ui->SDIChk->setChecked(true);
    ui->SetOnlyFSCB->setDisabled(true);
    ui->functionStatusCB->setDisabled(true);
    ui->SetOnlyValueCB->setDisabled(true);
    connect(ui->SDIChk,SIGNAL(toggled(bool)),this,SLOT(onSetSDIToggled(bool)));
}

void GTASetActionWidget::silentReSetValueCB()
{

    disconnect(ui->SetOnlyValueCB,SIGNAL(toggled(bool)),this,SLOT(setFSDisabled(bool)));
    ui->SetOnlyValueCB->setChecked(false);
    ui->SetOnlyFSCB->setEnabled(true);
    ui->functionStatusCB->setEnabled(true);
    ui->SetSDICB->setEnabled(true);
    connect(ui->SetOnlyValueCB,SIGNAL(toggled(bool)),this,SLOT(setFSDisabled(bool)));
}

void GTASetActionWidget::silentSetValueCB()
{
    disconnect(ui->SetOnlyValueCB,SIGNAL(toggled(bool)),this,SLOT(setFSDisabled(bool)));
    ui->SetOnlyValueCB->setChecked(true);
    ui->SetOnlyFSCB->setDisabled(true);
    ui->functionStatusCB->setDisabled(true);
    ui->SetSDICB->setDisabled(true);
    connect(ui->SetOnlyValueCB,SIGNAL(toggled(bool)),this,SLOT(setFSDisabled(bool)));
}

void GTASetActionWidget::onSetSDIToggled(bool iStatus)
{
    ui->noteLB->hide();
    if (isParameterFromDB())
    {
        ui->ValueLE->setDisabled(iStatus);
        ui->SearchPB2->setDisabled(iStatus);
        ui->EquationPB->setDisabled(iStatus);
        silentReSetFSCB();

    }
    else
    {
        if (this->isVisible())
            QMessageBox::information(this,"Invalid input parameter","Set only SDI is not valid for Local variables");
        silentReSetSDICB();
        ui->ValueLE->setDisabled(false);
        ui->SearchPB2->setDisabled(false);
        ui->EquationPB->setDisabled(false);
    }

}

/* This SLOT is called when onlyValue Checkbox is pressed.
* @input: iStatus - Status of the checkbox
* @return: none
* @info: The Default Option can be changed depending on its status 
*/
void GTASetActionWidget::setFSDisabled(bool iStatus)
{
    silentReSetFSCB();
    silentReSetSDICB();
    ui->SetOnlyFSCB->setDisabled(iStatus);
    ui->functionStatusCB->setDisabled(iStatus);
    ui->SetOnlyFSTable->setDisabled(iStatus);
    ui->ValueOnlyFSTable->setDisabled(iStatus);
    ui->SearchPB3->setDisabled(iStatus);

	if (iStatus)
	{
		// By default NO_CHANGE will be displayed
		ui->functionStatusCB->setCurrentIndex(8);
        // The value of Variable FS will be cleared
        ui->ValueOnlyFSTable->clear();
	}
	else
	{
		// By default NO will be displayed
		ui->functionStatusCB->setCurrentIndex(0);
	}
}

/* This function allow to modify the content of the FS ComboBox
 * @input: iStatus - bool
 * @info: true (remove NO_CHANGE item) false (add it again)
 * @return: none
*/
void GTASetActionWidget::modifyContentFSCB(bool iStatus)
{
	ui->functionStatusCB->clear();
	if(iStatus)
		_sFSList.pop_back();
	else
		_sFSList.push_back(ACT_FSSSM_NOCHANGE);
	ui->functionStatusCB->addItems(_sFSList);
}

void GTASetActionWidget::disableLineEdits(bool iStatus)
{
    if (iStatus==true)
    {
		modifyContentFSCB(true);
		silentReSetValueCB();
        if (isParameterFromDB())
        {
            ui->ValueLE->setDisabled(iStatus);
            ui->SearchPB2->setDisabled(iStatus);
            ui->EquationPB->setDisabled(iStatus);
            ui->noteLB->show();
            silentReSetSDICB();
            ui->ValueLE->clear();
        }
        else
        {
            if (this->isVisible())
                QMessageBox::information(this,"Invalid input parameter","Set only FS is not valid for Local variables");
            silentReSetFSCB();
        }
		ui->SetOnlyValueCB->setDisabled(true);
        ui->SetOnlyFSTable->setDisabled(iStatus);
        ui->ValueOnlyFSTable->setDisabled(iStatus);
        ui->SearchPB3->setDisabled(iStatus);
    }
    else
    {
		modifyContentFSCB(false); 
		ui->ValueLE->setDisabled(false);
        ui->SearchPB2->setDisabled(false);
        ui->EquationPB->setDisabled(false);
        ui->SetOnlyValueCB->setEnabled(true);
        ui->SetOnlyFSTable->setDisabled(false);
        ui->ValueOnlyFSTable->setDisabled(false);
        ui->SearchPB3->setDisabled(false);
        ui->noteLB->hide();
    }

    emit disablePrecision(iStatus);
}

/**
 * @brief Disables or enables line edits and other widgets in the (ForEACH Loop,...) based on the specified status.
 *
 * This function controls the enablement of various widgets such as line edits, buttons,
 * and combo boxes in the GTASetActionWidget. It is typically used to disable user interaction
 * with these widgets under certain conditions (e.g., when a parameter is obtained from the database)
 * or to enable them again when those conditions are no longer met.
 *
 * @param iStatus The boolean status indicating whether to disable (true) or enable (false) the widgets.
 *                When true, it disables/enables widgets and performs additional checks and actions:
 *                - If the parameter is from the database, it disables certain widgets, shows a label,
 *                  clears a line edit, and resets combo boxes silently.
 *                - If the parameter is not from the database and the widget is visible,
 *                  it shows an information message box.
 *                It also disables or enables the 'Set Only' combo boxes and the function status combo box.
 *                When false, it enables widgets and modifies the content of the function status combo box.
 *
 * @note This function also emits the disablePrecision signal with the iStatus value.
 */
void GTASetActionWidget::disableLineEditsTable(bool iStatus)
{
    if (iStatus == true)
    {
        if (isParameterFromDB())
        {
            ui->ValueLE->setDisabled(iStatus);
            ui->SearchPB2->setDisabled(iStatus);
            ui->EquationPB->setDisabled(iStatus);
            ui->noteLB->show();
            silentReSetSDICB();
            ui->ValueLE->clear();
        }
        else
        {
            if (this->isVisible())
                QMessageBox::information(this, "Invalid input parameter", "Set only FS is not valid for Local variables");
            silentReSetFSCB();
        }
        ui->SetOnlyValueCB->setDisabled(true);
        ui->SetOnlyFSCB->setDisabled(iStatus);
        ui->functionStatusCB->setDisabled(iStatus);
    }
    else
    {
        modifyContentFSCB(false);
        ui->ValueLE->setDisabled(false);
        ui->SearchPB2->setDisabled(false);
        ui->EquationPB->setDisabled(false);
        ui->SetOnlyValueCB->setEnabled(true);
        ui->SetOnlyFSCB->setDisabled(false);
        ui->functionStatusCB->setDisabled(false);
        ui->noteLB->hide();
    }

    emit disablePrecision(iStatus);
}

/**
 * @brief Sets the text of the Value Line Edit silently without triggering its textChanged signal.
 *
 * Temporarily disconnects the `textChanged` signal from the `onParameterValueEdited` slot
 * to prevent it from being triggered when setting the text. After setting the text to `iString`,
 * the signal-slot connection is restored.
 *
 * @param iString The string to set in the Value Line Edit.
 */
void GTASetActionWidget::silentSetValueText(const QString& iString)
{
    disconnect(ui->ValueLE,SIGNAL(textChanged(QString)),this,SLOT(onParameterValueEdited(QString)));
    ui->ValueLE->setText(iString);
    _currentParameterValue = iString;
    connect(ui->ValueLE,SIGNAL(textChanged(QString)),this,SLOT(onParameterValueEdited(QString)));
}

/**
 * @brief Silently checks the 'Set Only Function Status' checkbox and disables related widgets.
 *
 * Temporarily disconnects the `toggled` signal from the `disableLineEdits` slot of the 'Set Only Function Status'
 * checkbox to prevent it from being triggered. After checking the checkbox and disabling related widgets,
 * the signal-slot connection is reestablished.
 */
void GTASetActionWidget::silentSetFSCB()
{
    disconnect(ui->SetOnlyFSCB,SIGNAL(toggled(bool)),this,SLOT(disableLineEdits(bool)));
    ui->SetOnlyFSCB->setChecked(true);
    ui->SetOnlyFSTable->setDisabled(true);
    ui->SetOnlyValueCB->setDisabled(true);
    ui->SetSDICB->setDisabled(true);
    connect(ui->SetOnlyFSCB,SIGNAL(toggled(bool)),this,SLOT(disableLineEdits(bool)));
}

/**
 * @brief Silently unchecks the 'Set Only Function Status' checkbox and enables related widgets.
 *
 * Temporarily disconnects the `toggled` signal from the `disableLineEdits` slot of the 'Set Only Function Status'
 * checkbox. After unchecking the checkbox and enabling related widgets, the signal-slot connection is reestablished.
 */
void GTASetActionWidget::silentReSetFSCB()
{
    disconnect(ui->SetOnlyFSCB,SIGNAL(toggled(bool)),this,SLOT(disableLineEdits(bool)));
    ui->SetOnlyFSCB->setChecked(false);
    ui->SetOnlyFSTable->setEnabled(true);
    ui->SetOnlyValueCB->setEnabled(true);
    ui->SetSDICB->setEnabled(true);
    connect(ui->SetOnlyFSCB,SIGNAL(toggled(bool)),this,SLOT(disableLineEdits(bool)));
}

/**
 * @brief Silently checks the 'Set Only Function Status Value' checkbox and disables related widgets.
 *
 * Temporarily disconnects the `toggled` signal from the `disableLineEdits` slot of the 'Set Only Function Status Value'
 * checkbox. After checking the checkbox and disabling related widgets, the signal-slot connection is reestablished.
 */
void GTASetActionWidget::silentSetFSValue()
{
    disconnect(ui->SetOnlyFSTable, SIGNAL(toggled(bool)), this, SLOT(disableLineEditsTable(bool)));
    ui->SetOnlyFSTable->setChecked(true);
    ui->SetOnlyFSCB->setDisabled(true);
    ui->SetOnlyValueCB->setDisabled(true);
    ui->SetSDICB->setDisabled(true);
    connect(ui->SetOnlyFSTable, SIGNAL(toggled(bool)), this, SLOT(disableLineEditsTable(bool)));
}

/**
 * @brief Silently unchecks the 'Set Only Function Status Value' checkbox and enables related widgets.
 *
 * Temporarily disconnects the `toggled` signal from the `disableLineEdits` slot of the 'Set Only Function Status Value'
 * checkbox. After unchecking the checkbox and enabling related widgets, the signal-slot connection is reestablished.
 */
void GTASetActionWidget::silentReSetFSValue()
{
    disconnect(ui->SetOnlyFSTable, SIGNAL(toggled(bool)), this, SLOT(disableLineEditsTable(bool)));
    ui->SetOnlyFSTable->setChecked(false);
    ui->SetOnlyFSCB->setEnabled(true);
    ui->SetOnlyValueCB->setEnabled(true);
    ui->SetSDICB->setEnabled(true);
    connect(ui->SetOnlyFSTable, SIGNAL(toggled(bool)), this, SLOT(disableLineEditsTable(bool)));
}

/**
 * @brief Disables the 'Set Only Function Status' checkbox based on certain conditions.
 *
 * If the 'Set Only Function Status' checkbox is checked, it calls `silentReSetFSCB()` to uncheck and enable related widgets.
 * It also ensures the Value Line Edit is enabled.
 *
 * @param [unused] An unused parameter included for signal-slot compatibility.
 */
void GTASetActionWidget::disableFSCB( const QString&)
{
    if (ui->SetOnlyFSCB->isChecked())
    {
        silentReSetFSCB();
    }
    ui->ValueLE->setDisabled(false);
}
bool GTASetActionWidget::isValid()const
{
    if (_SetCommandList.isEmpty())
    {
        return false;
    }
    
    return true;
}

/*
 * This (SLOT) function allow to enable/disable the EQUATION_PB only if the Parameter field is not empty
 */
void GTASetActionWidget::onChangeParameterValue(QString iVal)
{
    if (!iVal.isEmpty())
    {
        ui->EquationPB->setDisabled(false);
    }
    else
    {
        ui->EquationPB->setDisabled(true);
    }
}

int GTASetActionWidget::getSearchType()
{

    int retVal = GTAGenSearchWidget::PARAMETER;
    return retVal;
    
}

/* This function allow to reset all checkBox/ComboBox states
* @return : None
*/
void GTASetActionWidget::resetAllCheckBoxStatus()
{
	ui->SetOnlyFSCB->setChecked(false);
	ui->SetOnlyValueCB->setChecked(false);
    ui->SetOnlyFSTable->setChecked(false);
	
	ui->SetOnlyFSCB->setEnabled(true);
	ui->SetOnlyValueCB->setEnabled(true);
    ui->SetOnlyFSTable->setEnabled(true);

	ui->functionStatusCB->setEnabled(true);
	ui->functionStatusCB->setCurrentIndex(0);
}

/* This function allows to automatically tick an option if the Parameter has a Media VMAC or DIS.
* @return : None
*/
void GTASetActionWidget::setOnlyValueStatus()
{
	ui->SetOnlyValueCB->setChecked(true);
	ui->SetOnlyFSCB->setChecked(false);
    ui->SetOnlyFSTable->setChecked(false);
	
	ui->SetOnlyFSCB->setEnabled(false);
	ui->SetOnlyValueCB->setEnabled(false);
    ui->SetOnlyFSTable->setEnabled(false);

	ui->functionStatusCB->setEnabled(false);
	ui->functionStatusCB->setCurrentIndex(8);
}

/* This SLOT allows to evaluate the entered Parameter and to detect its media
* @sender: this - textChanged() of the Parameter field
* @input: none
* @return: void
*/
void GTASetActionWidget::evaluateParamMedia(const QString&)
{
    ui->ValueLE->setDisabled(false);

    // Get the current Parameter field
    QString param = ui->ParamNameLE->text().simplified();

    // Enable the MCDU button if param contains GTA_MCDU_SEND
    // and set valueLE to readOnly mode for MCDU parameter
    bool isMcdu = !param.isEmpty() && (param.contains(GTA_MCDU_SEND));
    ui->SearchMCDU->setEnabled(isMcdu);
    ui->ValueLE->setReadOnly(isMcdu);

	if (!param.isEmpty() && !param.contains(GTA_CIC_IDENTIFIER))
	{
		GTAAppController* pCtrlr = GTAAppController::getGTAAppController();
		if (NULL != pCtrlr)
		{
			QStringList paramList = QStringList() << param;
			QList<GTAICDParameter> icdDetailList;
			if (param.contains(".") && param.count(".") == 2 && param.size()>12)
			{
				pCtrlr->getICDDetails(paramList, icdDetailList);
				if (!icdDetailList.isEmpty())
				{
					QString iSignalType = icdDetailList[0].getSignalType();
					if (icdDetailList[0].isOnlyValueDueToSignalType(iSignalType))
					{
						setParamWithoutFS(true);
					}
					else
					{
						setParamWithoutFS(false);
						// If the Parameter has another media, reset state of checkboxes
						resetAllCheckBoxStatus();
					}

					// In case a Parameter is found in DB but its form is different from this one 
					if (param.compare(icdDetailList[0].getName(), Qt::CaseSensitive) != 0)
					{
						ui->ParamNameLE->setText(icdDetailList[0].getName());
					}
				}
				else
				{
					if ((param.contains("@") && param.count('@', Qt::CaseInsensitive) % 2 == 0) || param.startsWith("line."))
					{
						setParamWithoutFS(false);
						// If the Parameter has another media, reset state of checkboxes
						resetAllCheckBoxStatus();
					}
					else
					{
						setParamWithoutFS(true);
					}
				}
			}
			else
			{
				setParamWithoutFS(true);
			}
		}
	}
	else
	{
		setParamWithoutFS(false);
		// If the Parameter field is empty, reset state of checkboxes
		resetAllCheckBoxStatus();
	}

    // Select the parameter in the command list if it exists
    for(int i=0; i < ui->setActionListLV->count(); i++)
    {
        QString itemText = ui->setActionListLV->item(i)->text();
        if (itemText.split("(")[0] == ui->ParamNameLE->text())
        {
            ui->setActionListLV->item(i)->setSelected(true);
        }
    }    

    // Update the current parameter name and value
    _newParameterToBeAdded = _currentParameterName != ui->ParamNameLE->text();
    _currentParameterName = _newParameterToBeAdded ? ui->ParamNameLE->text() : _currentParameterName; 
}

/* This SLOT is called when Time UTC Checkbox is pressed.
* @input: iStatus - Status of the checkbox
* @return: none
*/
void GTASetActionWidget::onTimeCBToggled(bool iStatus)
{
	ui->ValueLE->clear(); 
	if (iStatus)
	{
		ui->ValueLE->setText("CurrentTimeUTC");
	}
}

bool GTASetActionWidget::setWidgetFields(const QList<QPair<QString, QString>>& params, ErrorMessageList& errorLogs)
{
    bool ParamNameLE_set = false, ValueLE_set = false, functionStatusCB_set = false;
    for (auto param : params) {
        if (param.first == "ParamNameLE")
        {
            ParamNameLE_set = true;
            if (param.second != "")
                ui->ParamNameLE->setText(param.second);
            else
            {
                errorLogs.push_back({ "ParamNameLE is empty" });
                return false;
            }
        }
        else if (param.first == "ValueLE")
        {
            ValueLE_set = true;
            if(param.second == "")
                ui->SetOnlyFSCB->setChecked(true);
            else
                ui->ValueLE->setText(param.second);
        }
        else if (param.first == "functionStatusCB")
        {
            functionStatusCB_set = true;
            if (param.second == "")
                ui->SetOnlyValueCB->setChecked(true);
            else
            {
                int currIdx = ui->functionStatusCB->findText(param.second);
                if (currIdx >= 0)
                {
                    ui->SetOnlyValueCB->setChecked(false);
                    ui->SetOnlyFSCB->setEnabled(true);
                    ui->functionStatusCB->setEnabled(true);
                    ui->SetOnlyValueCB->setEnabled(true);
                    ui->functionStatusCB->setCurrentIndex(currIdx);
                }
                else
                {
                    // TODO: error handling
                    errorLogs.push_back({ "functionStatusCB is empty" });
                    return false;
                }
            }
        }
        else {
            // TODO: error handling
            errorLogs.append(QString("Got unexpected param: %1").arg(param.first));
            return false;
        }
        if (ParamNameLE_set && ValueLE_set && functionStatusCB_set) {
            ParamNameLE_set = false;
            ValueLE_set = false;
            functionStatusCB_set = false;
            onAddSetCommand();
        }
    }
    return true;
}

#include "GTAWgtRegisterDef.h"
REGISTER_ACTION_WIDGET(ACT_SET,GTASetActionWidget,obj)
