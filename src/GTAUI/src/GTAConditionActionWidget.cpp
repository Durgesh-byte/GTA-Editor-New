#include "GTAConditionActionWidget.h"
#include "ui_GTAConditionActionWidget.h"
#include "GTAActionSelectorWidget.h"
#include "GTAActionCondition.h"
#include "GTAAppController.h"
#include "GTASystemServices.h"
#include <QFileDialog>
#include <QMessageBox>

#include "GTAActionIfEnd.h"

#include "GTAActionWhile.h"
#include "GTAActionWhileEnd.h"
#include "GTAActionForEach.h"
#include "GTAActionForEachEnd.h"
#include "GTAParamValidator.h"
#include "GTAParamValueValidator.h"

#include "GTAActionDoWhile.h"
#include "GTAActionDoWhileEnd.h"
#include "GTAActionElse.h"

#include <QCompleter>
#include "GTAGenSearchWidget.h"



GTAConditionActionWidget::GTAConditionActionWidget(QWidget *parent) :
        GTAActionWidgetInterface(parent),
        ui(new Ui::GTAConditionActionWidget)
{

    ui->setupUi(this);

    QStringList operatorList;
    operatorList <<ARITH_EQ<<ARITH_NOTEQ<<ARITH_GT<<ARITH_LTEQ<<ARITH_LT<<ARITH_GTEQ;

    ui->OperatorCB->clear();
    ui->OperatorCB->addItems(operatorList);
    _SearchPBEditMap.insert(ui->SearchPB,ui->ParamNameLE);
    _SearchPBEditMap.insert(ui->SearchValuePB,ui->ValueLE);
    _SearchPBEditMap.insert(ui->SearchTablePB,ui->ListLE);
    hideNote(true);

    _FunctionalStatus << ACT_FSSSM_NOT_LOST<<ACT_FSSSM_NO<<ACT_FSSSM_ND<<ACT_FSSSM_FW<<ACT_FSSSM_NCD<<ACT_FSSSM_FT<<ACT_FSSSM_MINUS<<ACT_FSSSM_PLUS<<ACT_FSSSM_LOST;
    ui->functionalStatusCB->addItems(_FunctionalStatus);


    foreach(QPushButton * pSearchPB, _SearchPBEditMap.keys())
    {
        connect(pSearchPB,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));
    }

	connect(ui->ParamNameLE, SIGNAL(textChanged(const QString &)), this, SLOT(evaluateParamMedia(const QString &)));
    connect(ui->OperatorCB,SIGNAL(currentIndexChanged(QString)),this,SLOT(operatorChanged(QString)));

    connect(ui->ChkFSOnlyCB,SIGNAL(toggled(bool)),this,SLOT(disableValueEdits(bool)));
    connect(ui->ChkFSOnlyCB,SIGNAL(toggled(bool)),this,SLOT(hideNote(bool)));
    connect(ui->ChkFSOnlyCB,SIGNAL(toggled(bool)),this,SLOT(resetCheckOnlyValue(bool)));
    connect(ui->ChkFSOnlyCB,SIGNAL(toggled(bool)),this,SLOT(resetSDI(bool)));
    connect(ui->ChkFSOnlyCB,SIGNAL(toggled(bool)),this,SLOT(resetParity(bool)));
    connect(ui->ChkFSOnlyCB,SIGNAL(toggled(bool)),this,SLOT(resetRefreshRate(bool)));


    connect(ui->ChkValueCB,SIGNAL(toggled(bool)),this,SLOT(enableValueEdits(bool)));
    connect(ui->ChkValueCB,SIGNAL(toggled(bool)),this,SLOT(resetCheckFS(bool)));
    connect(ui->ChkValueCB,SIGNAL(toggled(bool)),this,SLOT(hideNote(bool)));
    connect(ui->ChkValueCB,SIGNAL(toggled(bool)),this,SLOT(resetSDI(bool)));
    connect(ui->ChkValueCB,SIGNAL(toggled(bool)),this,SLOT(resetParity(bool)));
    connect(ui->ChkValueCB,SIGNAL(toggled(bool)),this,SLOT(resetRefreshRate(bool)));


    connect(ui->chkOnlySDI,SIGNAL(toggled(bool)),this,SLOT(disableValueEdits(bool)));
    connect(ui->chkOnlySDI,SIGNAL(toggled(bool)),this,SLOT(hideNote(bool)));
    connect(ui->chkOnlySDI,SIGNAL(toggled(bool)),this,SLOT(resetCheckFS(bool)));
    connect(ui->chkOnlySDI,SIGNAL(toggled(bool)),this,SLOT(resetParity(bool)));
    connect(ui->chkOnlySDI,SIGNAL(toggled(bool)),this,SLOT(resetRefreshRate(bool)));
    connect(ui->chkOnlySDI,SIGNAL(toggled(bool)),this,SLOT(resetCheckOnlyValue(bool)));

    connect(ui->chkOnlyParity, SIGNAL(toggled(bool)),this,SLOT(disableValueEdits(bool)));
    connect(ui->chkOnlyParity, SIGNAL(toggled(bool)), this, SLOT(hideNote(bool)));
    connect(ui->chkOnlyParity,SIGNAL(toggled(bool)),this,SLOT(resetCheckFS(bool)));
    connect(ui->chkOnlyParity,SIGNAL(toggled(bool)),this,SLOT(resetRefreshRate(bool)));
    connect(ui->chkOnlyParity,SIGNAL(toggled(bool)),this,SLOT(resetCheckOnlyValue(bool)));
    connect(ui->chkOnlyParity,SIGNAL(toggled(bool)),this,SLOT(resetSDI(bool)));

    connect(ui->ChkRefreshRate, SIGNAL(toggled(bool)),this, SLOT(disableValueEditsOnRefresh(bool)));
    connect(ui->ChkRefreshRate, SIGNAL(toggled(bool)), this, SLOT(hideNote(bool)));
    connect(ui->ChkRefreshRate,SIGNAL(toggled(bool)),this,SLOT(resetCheckFS(bool)));
    connect(ui->ChkRefreshRate,SIGNAL(toggled(bool)),this,SLOT(resetSDI(bool)));
    connect(ui->ChkRefreshRate,SIGNAL(toggled(bool)),this,SLOT(resetParity(bool)));
    connect(ui->ChkRefreshRate,SIGNAL(toggled(bool)),this,SLOT(resetCheckOnlyValue(bool)));
    connect(ui->ForEachPathChkB,SIGNAL(toggled(bool)),this,SLOT(showForEachCSVFiles(bool)));
    connect(ui->ForEachPathCB,SIGNAL(currentIndexChanged(QString)),this,SLOT(forEachRelativePathSelected(QString)));

    GTAParamValidator* pValidator1 = new GTAParamValidator(ui->ParamNameLE);
    ui->ParamNameLE->setValidator(pValidator1);

    GTAParamValueValidator* pValidator2 = new GTAParamValueValidator(ui->ValueLE);
    ui->ValueLE->setValidator(pValidator2);

    _isParameterLE = false;
    connect(ui->ChkAudioAlarmCB,SIGNAL(toggled(bool)),this,SLOT(checkAudioAlarmToggeled(bool)));
    connect(ui->ChkAudioAlarmCB,SIGNAL(toggled(bool)),this,SLOT(hideNote(bool)));
    ui->HeardAfterLE->setVisible(false);
    ui->IsHeardAfterLB->setVisible(false);
    ui->label_5->setVisible(false);
    ui->labelwarForEach->hide();

//    connect(ui->ChkRepeatedLogging,SIGNAL(toggled(bool)),this,SLOT(checkRepeatedLoggingToggeled(bool)));
//    ui->ChkRepeatedLogging->setVisible(false);

    _complement = QString();

    GTAAppController* pController = GTAAppController::getGTAAppController();
    if (NULL!=pController)
    {
        QStringList wordList = pController->getVariablesForAutoComplete();
        QCompleter *completer = new QCompleter(wordList, this);
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        ui->ParamNameLE->setCompleter(completer);
        ui->ValueLE->setCompleter(completer);
    }
    _pCurrentCommand=NULL;

    bool isGeneric = TestConfig::getInstance()->getGenericSCXML();
    if(isGeneric)
    {
        ui->ChkAudioAlarmCB->setDisabled(isGeneric);
        ui->chkOnlySDI->setDisabled(isGeneric);
        ui->CheckSDICB->setDisabled(isGeneric);
        ui->ChkRefreshRate->setDisabled(isGeneric);
        ui->chkOnlyParity->setDisabled(isGeneric);
        ui->CheckParityCB->setDisabled(isGeneric);
    }

}

GTAConditionActionWidget::~GTAConditionActionWidget()
{
    delete ui;
}

bool GTAConditionActionWidget::getCheckOnlyValueState()const
{
    return ui->ChkValueCB->isChecked();
}
bool GTAConditionActionWidget::getCheckOnlyFSState()const
{
    return ui->ChkFSOnlyCB->isChecked();
}

//bool GTAConditionActionWidget::getCheckRepeatedLoggingState()const
//{
//    return ui->ChkRepeatedLogging->isChecked();
//}

QString GTAConditionActionWidget::getParameter() const
{
    return ui->ParamNameLE->text();
}

/**
 * @brief Return the status of autoAddElse checkbox when it is available
 * @return True if the checkbox is ticked
*/
bool GTAConditionActionWidget::isAutoAddElseChecked() const
{
    return ui->autoAddElse->isChecked() && ui->autoAddElse->isVisible();
}

bool GTAConditionActionWidget::getChkAudioAlarm() const
{
    return ui->ChkAudioAlarmCB->isChecked();
}
QString GTAConditionActionWidget::getParameterValue() const
{
    return ui->ValueLE->text();
}
QString GTAConditionActionWidget::getConditionOperator() const
{
    return ui->OperatorCB->currentText();
}

void GTAConditionActionWidget::onSearchPBClicked()
{
    QPushButton * pButtonClicked = dynamic_cast<QPushButton*>(sender());
    if(sender() == ui->SearchPB)
        _isParameterLE = true;
    else
        _isParameterLE = false;

    if(sender() == ui->SearchTablePB)
    {
        GTAAppController* pAppcontroller = GTAAppController::getGTAAppController();
        if (NULL!=pAppcontroller)
        {
            GTASystemServices* pSystemService = pAppcontroller->getSystemService();
            if (NULL!=pSystemService)
            {
                QString Dir = pSystemService->getTableDirectory();
                QString fileFilter("*.csv");
                QString listFile;
                if(ui->ListLE->text().isEmpty())
                {
                    listFile = QFileDialog::getOpenFileName(this,"Browse File",Dir,fileFilter);
                }
                else
                {
                    QString localPath = ui->ListLE->text();
                    if (localPath.startsWith("/TABLES"))
                    {
                        localPath.remove(0, QString("/TABLES").length());
                        localPath = Dir + QDir::separator() + localPath;
                    }
                    QFileInfo fileInfo(localPath);
                    listFile = QFileDialog::getOpenFileName(this,"Browse File",fileInfo.absolutePath(),fileFilter);
                    if(listFile.isEmpty())
                        listFile = localPath;
                }
                ui->ListLE->setText(QDir::cleanPath(listFile));
            }
            if (!ui->ListLE->text().contains(".csv",Qt::CaseInsensitive) && !ui->ListLE->text().isEmpty())
            {
                QMessageBox::information(this,"File Mismatch","Selected file is not a CSV file");
                ui->ListLE->setText("");
            }
        }

    }

    if(pButtonClicked != NULL && _SearchPBEditMap.contains(pButtonClicked) && sender() != ui->SearchTablePB)
    {
        QLineEdit * pEdit = _SearchPBEditMap.value(pButtonClicked);
        emit searchObject(pEdit);
    }
}


bool GTAConditionActionWidget::isValid()const 
{
    bool rc = false;
    if (_complement.isEmpty()==false)
    {
        if (_complement==COM_CONDITION_ELSE)
        {
            rc = true;
            ui->checkValnFS->hide();
        }
        else if (_complement==COM_CONDITION_FOR_EACH)
        {
            rc = true;
        }
        else
        {
            if(getCheckOnlyValueState() &&(!getParameter().isEmpty() && !getConditionOperator().isEmpty() && !getParameterValue().isEmpty()))
                rc = true;
            else if((getCheckOnlyFSState() || getCheckOnlySDIState() ||getCheckOnlyParityState() || getCheckOnlyRefreshRateState())
                && (!getParameter().isEmpty() && !getConditionOperator().isEmpty()))
                rc = true;
            else if(!getParameter().isEmpty() && !getConditionOperator().isEmpty() && !getParameterValue().isEmpty())
                rc = true;
        }
    }

    return rc;

}
bool GTAConditionActionWidget::getActionCommand( GTAActionBase *& opCondActionCmd)const
{
    bool rc = false;
    opCondActionCmd = NULL;
    if(isValid() == false)
        return rc;

    if (_complement == COM_CONDITION_IF)
    {
        GTAActionIF* pIfCmd = NULL;

        pIfCmd = new GTAActionIF();
        GTAActionIfEnd* pEnd = new GTAActionIfEnd();
        pIfCmd->insertChildren(pEnd,0); //End appended along with if
        pIfCmd->setAutoAddElse(isAutoAddElseChecked());
        rc =createNestedCommandForChannels(pIfCmd,getParameter(),getParameterValue());

        if(!rc)
        {
            delete pIfCmd;
            pIfCmd = NULL;
        }
        opCondActionCmd = pIfCmd;

    }
    else if (_complement == COM_CONDITION_WHILE)
    {
        
        GTAActionWhile* pWhileCmd = NULL;
        pWhileCmd = new GTAActionWhile();
        GTAActionWhileEnd* pEnd = new GTAActionWhileEnd();
        pWhileCmd->insertChildren(pEnd,0); //End appended along with while
        rc =createNestedCommandForChannels(pWhileCmd,getParameter(),getParameterValue());

        if(!rc)
        {
            delete pWhileCmd;
            pWhileCmd = NULL;
        }
        opCondActionCmd = pWhileCmd;


    }
    else if (_complement == COM_CONDITION_DO_WHILE)
    {

        GTAActionDoWhile* pWhileCmd = NULL;
        pWhileCmd = new GTAActionDoWhile();
        GTAActionDoWhileEnd* pEnd = new GTAActionDoWhileEnd();
        pWhileCmd->insertChildren(pEnd,0); //End appended along with while


        rc =createNestedCommandForChannels(pWhileCmd,getParameter(),getParameterValue());

        if(!rc)
        {
            delete pWhileCmd;
            pWhileCmd = NULL;
        }

        opCondActionCmd = pWhileCmd;
        
    }
    else if(_complement == COM_CONDITION_ELSE)
    {
        
        GTAActionElse* pElse = new GTAActionElse(ACT_CONDITION,COM_CONDITION_ELSE);
        opCondActionCmd = (GTAActionBase *)pElse;
        rc=true;

    }
    else if (_complement == COM_CONDITION_FOR_EACH)
    {

        GTAActionForEach* pForEachCmd = new GTAActionForEach();
        GTAActionForEachEnd* pForEachEnd = new GTAActionForEachEnd();
        pForEachCmd->insertChildren(pForEachEnd,0); //End appended along with ForEach
        QString repoPath = QString::fromStdString(TestConfig::getInstance()->getRepositoryPath());
        pForEachCmd->setRepositoryPath(repoPath);
        //(story #358311) : read relative path and set both relative and full paths of foreach
        QString filePath;
        
        if(!ui->ForEachPathCB->currentText().isEmpty())
        {           
            filePath = repoPath + ui->ForEachPathCB->currentText();
            rc = pForEachCmd->validateTable(filePath);
        }
        else
            rc = pForEachCmd->validateTable(ui->ListLE->text());   

        if (rc)
        {
            rc = true;
            
			if(ui->ForEachPathChkB->isChecked())
            {
                if(!ui->ForEachPathCB->currentText().isEmpty())
                {
                    ui->ListLE->setText(ui->ForEachPathCB->currentText());
					pForEachCmd->setRelativePath(ui->ForEachPathCB->currentText());
                }
                else
                {
                    if(ui->ListLE->text().startsWith(repoPath))
                    {
                        QString path = ui->ListLE->text();
                        pForEachCmd->setRelativePath(path.remove(repoPath));
                    }
                }
            }
            else
            {
                pForEachCmd->setPath(ui->ListLE->text());
                pForEachCmd->setRelativePath(QString());
            }

			pForEachCmd->setRelativePathChkB(ui->ForEachPathChkB->isChecked());
            pForEachCmd->setDelimiterIndex(ui->ForEachCB->currentIndex());
            pForEachCmd->setOverWriteResults(ui->ForEachChkB->isChecked());
            rc = pForEachCmd->setColumnMap();
            ui->labelwarForEach->hide();
        }
        else
        {
            ui->labelwarForEach->show();
            //QMessageBox::information(ui->SearchTablePB,"Table Incorrect","Selected table contains forbidden characters");
            rc = false;
        }

        if(!rc)
        {
            delete pForEachCmd;
            pForEachCmd = NULL;
        }
        opCondActionCmd = pForEachCmd;
    }

    if (_pCurrentCommand!=NULL && opCondActionCmd!=NULL)
    {
        QList<GTACommandBase*> lstOfChildrens = _pCurrentCommand->getChildren();
        if (lstOfChildrens.isEmpty()==false)
        {
            for(int i=0;i<lstOfChildrens.size()-1;i++)//end command not included
            {
                GTACommandBase* pCmd = lstOfChildrens.at(i);
                if (pCmd!=NULL)
                {
                    GTACommandBase* pCmdClone = pCmd->getClone();
                    opCondActionCmd->insertChildren(pCmdClone,i);
                }
                else
                    opCondActionCmd->insertChildren(NULL,i);
            }
        }
    }

    return rc;
}
void GTAConditionActionWidget::setActionCommand(const GTAActionBase * ipActionCmd)
{
    if(ipActionCmd != NULL)
    {

        setComplement(ipActionCmd->getComplement());
        GTAActionIF* pIfCmd = NULL;
        GTAActionWhile* pWhile = NULL;
        GTAActionDoWhile *pDoWhile = NULL;
        GTAActionForEach *pforEach = NULL;
        if (_complement == COM_CONDITION_IF)
        {
            pIfCmd = dynamic_cast<GTAActionIF*>((GTAActionBase *)ipActionCmd);
            ui->autoAddElse->hide();                
        }

        if (_complement == COM_CONDITION_WHILE)
        {
            pWhile =  dynamic_cast<GTAActionWhile*>((GTAActionBase *)ipActionCmd);;
        }

        if (_complement == COM_CONDITION_DO_WHILE)
        {
            pDoWhile = dynamic_cast<GTAActionDoWhile*>((GTAActionBase *)ipActionCmd);
        }

        if (_complement == COM_CONDITION_FOR_EACH)
        {
            pforEach = dynamic_cast<GTAActionForEach*>((GTAActionBase *)ipActionCmd);
        }
        else if (_complement == COM_CONDITION_ELSE)
        {
            ui->ChkFSOnlyCB->hide();
            ui->ChkValueCB->hide();
            ui->checkFsWarLB->hide();
            ui->labelwarForEach->hide();
            ui->checkValnFS->hide();
            ui->chkOnlySDI->hide();
            ui->CheckSDICB->hide();
            ui->chkOnlyParity->hide();
            ui->CheckParityCB->hide();
            ui->ChkRefreshRate->hide();
            ui->functionalStatusCB->hide();
            return;
        }


        QString param;
        QString path;
        bool resultOverwrite;
        QString val; 
        QString cond;
        int delimindex;
        bool chkAudioAlarm  = false;
        bool chkFSOnly = false;
        bool chkValueOnly = false;
        bool chkSDIOnly = false;
        bool chkParityOnly = false;
        bool chkRefreshOnly = false;
		bool isRelativePath = false;
//        bool isRepeatedLogging = false;

        QString functionalStatus;
        QString sPartity;
        QString sSDI;
        QString heardAfter = "";
        if (pWhile)
        {
            param = pWhile->getParameter();
            val = pWhile->getValue();
            cond= pWhile->getCondition();
            chkAudioAlarm = pWhile->getChkAudioAlarm();
            heardAfter = pWhile->getHeardAfter();
            chkFSOnly = pWhile->getIsFsOnly();
            chkValueOnly = pWhile->getIsValueOnly();
            functionalStatus = pWhile->getFunctionalStatus();
            chkSDIOnly = pWhile->getIsSDIOnly();
            chkParityOnly = pWhile->getIsParityOnly();
            chkRefreshOnly = pWhile->getIsRefreshRateOnly();
            sPartity = pWhile->getParity();
            sSDI = pWhile->getSDI();
//            isRepeatedLogging = pWhile->getIsRepeatedLogging();
            _pCurrentCommand= pWhile;
        }
        else if (pIfCmd)
        {
            param = pIfCmd->getParameter();
            val = pIfCmd->getValue();
            cond= pIfCmd->getCondition();
            chkAudioAlarm = pIfCmd->getChkAudioAlarm();
            heardAfter = pIfCmd->getHeardAfter();
            chkFSOnly = pIfCmd->getIsFsOnly();
            chkValueOnly = pIfCmd->getIsValueOnly();
            functionalStatus = pIfCmd->getFunctionalStatus();

            chkSDIOnly = pIfCmd->getIsSDIOnly();
            chkParityOnly = pIfCmd->getIsParityOnly();
            chkRefreshOnly = pIfCmd->getIsRefreshRateOnly();
            sPartity = pIfCmd->getParity();
            sSDI = pIfCmd->getSDI();

            _pCurrentCommand= pIfCmd;


        }
        else if (pDoWhile)
        {
            param = pDoWhile->getParameter();
            val = pDoWhile->getValue();
            cond= pDoWhile->getCondition();
            chkAudioAlarm = pDoWhile->getChkAudioAlarm();
            heardAfter = pDoWhile->getHeardAfter();
            chkFSOnly = pDoWhile->getIsFsOnly();
            chkValueOnly = pDoWhile->getIsValueOnly();
            functionalStatus = pDoWhile->getFunctionalStatus();
            chkSDIOnly = pDoWhile->getIsSDIOnly();
            chkParityOnly = pDoWhile->getIsParityOnly();
            chkRefreshOnly = pDoWhile->getIsRefreshRateOnly();
            sPartity = pDoWhile->getParity();
            sSDI = pDoWhile->getSDI();
//            isRepeatedLogging = pDoWhile->getIsRepeatedLogging();

            _pCurrentCommand= pDoWhile;

        }
        else if (pforEach)
        {
            // Read the path specified in the *.pro file
            path = pforEach->getPath();
            if (pforEach->getRelativePathChkB())
            {
                path = pforEach->getRelativePath();
            }
            // Get the filename
            QString filename = QString();
            filename = path.split("/").last();

            // We have to test if the file exists
            QFile file(path);
            checkPath(path);

            // Relative Path CheckBox is not checked
            if (!pforEach->getRelativePathChkB())
            {
                isRelativePath = false;
                pforEach->setRelativePathChkB(false);
                pforEach->setPath(path);
            }
            else
            {
                isRelativePath = true;
                pforEach->setRelativePathChkB(true);
                pforEach->setRelativePath(path);
                ui->ListLE->setText(path);
            }
            delimindex = pforEach->getDelimiterIndex();
            resultOverwrite = pforEach->getOverWriteResults();

            _pCurrentCommand = pforEach;
		}

        ui->ChkAudioAlarmCB->setChecked(chkAudioAlarm);
        ui->HeardAfterLE->setText(heardAfter);
        ui->ParamNameLE->setText(param);
        ui->ValueLE->setText(val);       
        ui->ChkFSOnlyCB->setChecked(chkFSOnly);
        ui->ChkValueCB->setChecked(chkValueOnly);
        ui->chkOnlySDI->setChecked(chkSDIOnly);
        ui->chkOnlyParity->setChecked(chkParityOnly);
        ui->ChkRefreshRate->setChecked(chkRefreshOnly);
        ui->ListLE->setText(path);
        ui->ForEachCB->setCurrentIndex(delimindex);
        ui->ForEachChkB->setChecked(resultOverwrite);
        ui->ForEachPathChkB->setChecked(isRelativePath);
//        ui->ChkRepeatedLogging->setChecked(isRepeatedLogging);
        if(isRelativePath)
        {
            QString repoPath = QString::fromStdString(TestConfig::getInstance()->getRepositoryPath());
            int index = ui->ForEachPathCB->findText(path.remove(repoPath));
            ui->ForEachPathCB->setCurrentIndex(index);
        }


        int index = _FunctionalStatus.indexOf(functionalStatus);
        if(index >= 0 && index < _FunctionalStatus.count())
            ui->functionalStatusCB->setCurrentIndex(index);

        index = ui->CheckSDICB->findText(sSDI);
        if(index >= 0 && index < ui->CheckSDICB->count())
            ui->CheckSDICB->setCurrentIndex(index);

        index = ui->CheckParityCB->findText(sPartity);
        if(index >= 0 && index < ui->CheckParityCB->count())
            ui->CheckParityCB->setCurrentIndex(index);

        int itemCount = ui->OperatorCB->count();
        for(int i = 0; i < itemCount; i++)
        {
            if(ui->OperatorCB->itemText(i) == cond)
            {
                ui->OperatorCB->setCurrentIndex(i);
                break;
            }
        }
    }
    else
    {
        ui->ParamNameLE->setText(QString());
        ui->ValueLE->setText(QString());
        ui->ChkAudioAlarmCB->setChecked(false);
        ui->ChkFSOnlyCB->hide();
        ui->ChkValueCB->hide();
        ui->checkFsWarLB->hide();
        ui->labelwarForEach->hide();
        ui->checkValnFS->hide();
        ui->chkOnlySDI->hide();
        ui->CheckSDICB->hide();
        ui->chkOnlyParity->hide();
        ui->CheckParityCB->hide();
        ui->ChkRefreshRate->hide();
        ui->functionalStatusCB->hide();
    }
}

/**
 * @brief Checks path, modifies it to default path if it's not existing
*/
void GTAConditionActionWidget::checkPath(QString path) {
    QString filename = path.split("/").last();
    QFile file(path);
    if (!QFileInfo::exists(path))
    {
        qWarning() << filename + " file does not exists" << endl;
        QString repoPath = QString::fromStdString(TestConfig::getInstance()->getRepositoryPath());
        path = repoPath + "/TABLES/" + filename;
    }
}

void GTAConditionActionWidget::clear()
{
    ui->ParamNameLE->setText("");
    ui->ValueLE->setText("");
    ui->ChkAudioAlarmCB->setChecked(false);
    ui->chkOnlySDI->setChecked(false);
    ui->chkOnlyParity->setChecked(false);
    ui->ChkRefreshRate->setChecked(false);
}
void GTAConditionActionWidget::operatorChanged(QString isOperator)
{
    bool isChecked = (ui->ChkFSOnlyCB->isChecked() || ui->ChkRefreshRate->isChecked() || ui->chkOnlyParity->isChecked() || ui->chkOnlySDI->isChecked() || ui->ChkValueCB->isChecked());
    bool val = false;
    emit conditionOperatorChanged(isOperator);
    emit disablePrecision(val);
    if (isOperator == ARITH_NOTEQ || isOperator == ARITH_EQ)
    {
        //bool val;
        if(ui->ChkValueCB->isChecked() || ui->ChkRefreshRate->isChecked())
            val = false;
        else if (!isChecked)
            val = false;
        else
            val = true;
        emit disablePrecision(val);
    }
}

bool GTAConditionActionWidget::isParameterLESelected()const
{
    return _isParameterLE;
}
void GTAConditionActionWidget::checkAudioAlarmToggeled(bool isCheckAudioAlarm)
{
    (isCheckAudioAlarm==true)?ui->ParamNameLE->setText("AudioAlarm"):ui->ParamNameLE->clear();
    ui->ParamNameLE->setReadOnly(isCheckAudioAlarm);
    ui->SearchPB->setDisabled(isCheckAudioAlarm);
    ui->ValueLE->clear();
    ui->HeardAfterLE->clear();
    ui->HeardAfterLE->setVisible(isCheckAudioAlarm);
    ui->IsHeardAfterLB->setVisible(isCheckAudioAlarm);
    ui->label_5->setVisible(isCheckAudioAlarm);
    ui->ChkFSOnlyCB->setDisabled(isCheckAudioAlarm);
    ui->ChkValueCB->setDisabled(isCheckAudioAlarm);
    ui->chkOnlySDI->setDisabled(isCheckAudioAlarm);
    ui->chkOnlyParity->setDisabled(isCheckAudioAlarm);
    ui->ChkRefreshRate->setDisabled(isCheckAudioAlarm);
    ui->CheckSDICB->setDisabled(isCheckAudioAlarm);
    ui->CheckParityCB->setDisabled(isCheckAudioAlarm);

    if(isCheckAudioAlarm == true)
    {
        QStringList operatorList;
        operatorList <<ARITH_EQ<<ARITH_NOTEQ;
        ui->OperatorCB->clear();
        ui->OperatorCB->addItems(operatorList);
        ui->ValueLB->setText("Audio Alarm        ");
        ui->ValueLE->setCompleter(NULL);
        

    }
    else
    {
        QStringList operatorList;
        operatorList <<ARITH_EQ<<ARITH_NOTEQ<<ARITH_GT<<ARITH_LTEQ<<ARITH_LT<<ARITH_GTEQ;
        ui->OperatorCB->clear();
        ui->OperatorCB->addItems(operatorList);
        ui->ValueLB->setText("Value/Parameter");

        QCompleter* pcompleter = ui->ParamNameLE->completer();
        if(NULL!=pcompleter)
        {
            ui->ValueLE->setCompleter(pcompleter);
        }
    }
}

/* This function allow to reset all checkBox/ComboBox states
* @return : None
*/
void GTAConditionActionWidget::resetAllCheckBoxStatus()
{
	ui->ChkValueCB->setChecked(false);
	ui->ChkFSOnlyCB->setChecked(false);
	ui->chkOnlySDI->setChecked(false);
	ui->chkOnlyParity->setChecked(false);
	ui->ChkRefreshRate->setChecked(false);

	ui->ChkValueCB->setEnabled(true);
	ui->ChkFSOnlyCB->setEnabled(true);
	ui->chkOnlySDI->setEnabled(true);
	ui->chkOnlyParity->setEnabled(true);
	ui->ChkRefreshRate->setEnabled(true);

	ui->functionalStatusCB->setEnabled(true);
	ui->functionalStatusCB->setCurrentIndex(0);
	ui->CheckSDICB->setEnabled(true);
	ui->CheckParityCB->setEnabled(true);
}

/* This function allows to automatically tick an option if the Parameter has a Media VMAC or DIS.
* @return : None
*/
void GTAConditionActionWidget::setOnlyValueStatus()
{
	ui->ChkValueCB->setChecked(true);
	ui->ChkFSOnlyCB->setChecked(false);
	ui->chkOnlySDI->setChecked(false);
	ui->chkOnlyParity->setChecked(false);
	ui->ChkRefreshRate->setChecked(false);

	ui->ChkValueCB->setEnabled(false);
	ui->ChkFSOnlyCB->setEnabled(false);
	ui->chkOnlySDI->setEnabled(false);
	ui->chkOnlyParity->setEnabled(false);
	ui->ChkRefreshRate->setEnabled(false);

	ui->functionalStatusCB->setEnabled(false);
	ui->functionalStatusCB->setCurrentIndex(1);
	ui->CheckSDICB->setEnabled(false);
	ui->CheckParityCB->setEnabled(false);
}

/* This SLOT allows to evaluate the entered Parameter and to detect its media
* @sender: this - textChanged() of the Parameter field
* @input: none
* @return: void
*/
void GTAConditionActionWidget::evaluateParamMedia(const QString &)
{
	ui->ValueLE->setDisabled(false);

	QString param = ui->ParamNameLE->text().trimmed();
	if (!param.isEmpty() && !param.contains(GTA_CIC_IDENTIFIER))
	{
		GTAAppController* pCtrlr = GTAAppController::getGTAAppController();
		if (NULL != pCtrlr)
		{
			QStringList paramList = QStringList() << param;
			QList<GTAICDParameter> icdDetailList;
			pCtrlr->getICDDetails(paramList, icdDetailList);
			if (!icdDetailList.isEmpty())
			{
				//QString media = icdDetailList[0].getMedia();
				//if (media == "VMAC" || media == "DIS" || media == "ANA" || media == "MODEL" || media == "PIR")
				QString iSignalType = icdDetailList[0].getSignalType();
				//bool isMedia = icdDetailList[0].getSignalTypeFromMedia(QString & iSignalType);
				if (icdDetailList[0].isOnlyValueDueToSignalType(iSignalType)) 
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

/* This function returns the Parity value
* @return: QString
*/
QString GTAConditionActionWidget::getParity()const
{
    return ui->CheckParityCB->currentText();
}

/* This function returns the SDI value
* @return: QString
*/
QString GTAConditionActionWidget::getSDI()const
{
    return ui->CheckSDICB->currentText();
}

/* This function returns the RefreshRate status (checkBox)
* @return: bool
*/
bool GTAConditionActionWidget::getCheckOnlyRefreshRateState()const
{
    return ui->ChkRefreshRate->isChecked();
}

/* This function returns the CheckOnlySDI status (checkBox)
* @return: bool
*/
bool GTAConditionActionWidget::getCheckOnlySDIState()const
{
    return ui->chkOnlySDI->isChecked();
}

/* This function returns the CheckOnlyParity status (checkBox)
* @return: bool
*/
bool GTAConditionActionWidget::getCheckOnlyParityState()const
{
    return ui->chkOnlyParity->isChecked();
}

/* This function returns the FunctionalStatus value
* @return: QString
*/
QString GTAConditionActionWidget::getFunctionalStatus()const
{
    return ui->functionalStatusCB->currentText();
}

QString GTAConditionActionWidget::getHeardAfter() const
{
    return ui->HeardAfterLE->text();
}

bool GTAConditionActionWidget::hasSearchItem()
{
    if(_complement == COM_CONDITION_ELSE)
        return false;
    return true;
}
bool GTAConditionActionWidget::hasActionOnFail()
{
    if(_complement == COM_CONDITION_ELSE || _complement == COM_CONDITION_FOR_EACH)
        return false;
    return true;
}
bool GTAConditionActionWidget::hasPrecision()
{
    if(_complement == COM_CONDITION_ELSE || _complement == COM_CONDITION_FOR_EACH)
        return false;
    return true;
}
bool GTAConditionActionWidget::hasTimeOut(int& oValidation)
{
    oValidation=kNa;
    if(_complement == COM_CONDITION_ELSE || _complement == COM_CONDITION_FOR_EACH)
        return false;
    return true;
}
bool GTAConditionActionWidget::hasConfigTime()
{
    if(_complement == COM_CONDITION_IF)
        return false;
    return false;
}

bool GTAConditionActionWidget::hasDumpList()
{
    if(_complement == COM_CONDITION_ELSE || _complement == COM_CONDITION_FOR_EACH)
        return false;
    return true;
}
bool GTAConditionActionWidget::hasCondition()
{
    if(_complement == COM_CONDITION_ELSE)
        return false;
    return true;

}
void GTAConditionActionWidget::setComplement(const QString& iComplement)
{
    disconnect(ui->ForEachPathChkB,SIGNAL(toggled(bool)),this,SLOT(showForEachCSVFiles(bool)));
    _complement =iComplement;
    bool bItemVisiblility = true;
    if(_complement == COM_CONDITION_ELSE || _complement == COM_CONDITION_FOR_EACH)
        bItemVisiblility=false;

    ui->ParameterLB->setVisible(bItemVisiblility);
    ui->ParamNameLE->setVisible(bItemVisiblility);
    ui->ChkAudioAlarmCB->setVisible(bItemVisiblility);
    ui->SearchPB->setVisible(bItemVisiblility);
    ui->label_3->setVisible(bItemVisiblility);
    ui->OperatorCB->setVisible(bItemVisiblility);
    ui->ValueLB->setVisible(bItemVisiblility);
    ui->ValueLE->setVisible(bItemVisiblility);
    ui->SearchValuePB->setVisible(bItemVisiblility);
    ui->ChkFSOnlyCB->setVisible(bItemVisiblility);
    ui->checkValnFS->setVisible(bItemVisiblility);
    ui->ChkValueCB->setVisible(bItemVisiblility);
    ui->chkOnlySDI->setVisible(bItemVisiblility);
    ui->CheckSDICB->setVisible(bItemVisiblility);
    ui->chkOnlyParity->setVisible(bItemVisiblility);
    ui->CheckParityCB->setVisible(bItemVisiblility);
    ui->ChkRefreshRate->setVisible(bItemVisiblility);
    ui->functionalStatusCB->setVisible(bItemVisiblility);
    //written separately as they would be seen in ELSE too
    ui->ListLE->setVisible(false);
    ui->ListLB->setVisible(false);
    ui->SearchTablePB->setVisible(false);
    ui->DelimiterLB->setVisible(false);
    ui->ForEachCB->setVisible(false);
    ui->ForEachChkB->setVisible(false);
    ui->ForEachPathChkB->setVisible(false);
    ui->ForEachPathCB->setVisible(false);
//    ui->ChkRepeatedLogging->setVisible(false);

    //    ui->checkFsWarLB->setVisible(bItemVisiblility);

    if(_complement == COM_CONDITION_FOR_EACH)
    {
        //show only foreach components
        ui->ListLE->setVisible(true);
        ui->ListLB->setVisible(true);
        ui->SearchTablePB->setVisible(true);
        ui->DelimiterLB->setVisible(true);
        ui->ForEachCB->setVisible(true);
        ui->ForEachChkB->setVisible(true);
        ui->ForEachPathChkB->setVisible(true);
        connect(ui->ForEachPathChkB,SIGNAL(toggled(bool)),this,SLOT(showForEachCSVFiles(bool)));

        bool val = hasPrecision();
        emit disablePrecision(val);
    }

    ui->autoAddElse->setVisible(_complement == COM_CONDITION_IF);

//    if(_complement == COM_CONDITION_WHILE || _complement == COM_CONDITION_DO_WHILE)
//        ui->ChkRepeatedLogging->setVisible(true);
}
void GTAConditionActionWidget::processSearchInput(const QStringList& iSelectedItem)
{

    if( (_complement!= COM_CONDITION_ELSE) && (iSelectedItem.size()>=23))
    {
        QString precVal = iSelectedItem.at(22);
        if(!precVal.isEmpty() && (precVal!= "NA") && isParameterLESelected())
            _newPrecison = precVal;

    }
}

int GTAConditionActionWidget::getSearchType()
{

    int retVal = -1;
    if( _complement == COM_CONDITION_ELSE)
        return retVal;
    else
    {
        
        if(getChkAudioAlarm())
            retVal =GTAGenSearchWidget::AUDIO_RECOG;
        else
            retVal=GTAGenSearchWidget::PARAMETER;
    }

    return retVal;
}
bool GTAConditionActionWidget::createNestedCommandForChannels(GTAActionIF* iPCmd , QString iParameter, QString iParameterVal )const
{

    bool rc = true;
    QStringList sParameters = GTAUtil::getProcessedParameterForChannel(iParameter);
    QStringList sValues     = GTAUtil::getProcessedParameterForChannel(iParameterVal);

    if (sParameters.size() <=1 && sValues.size()<=1)
    {
        iPCmd->setParameter(iParameter);
        iPCmd->setValue(iParameterVal);
        iPCmd->setCondition(getConditionOperator());
        iPCmd->setChkAudioAlarm(getChkAudioAlarm());
        iPCmd->setHeardAfter(getHeardAfter());
        iPCmd->setIsFsOnly(getCheckOnlyFSState());
        iPCmd->setIsValueOnly(getCheckOnlyValueState());
        iPCmd->setFunctionalStatus(getFunctionalStatus());
        iPCmd->setIsSDIOnly(getCheckOnlySDIState());
        iPCmd->setSDI(getSDI());
        iPCmd->setIsParityOnly(getCheckOnlyParityState());
        iPCmd->setParity(getParity());
        iPCmd->setIsRefreshRateOnly(getCheckOnlyRefreshRateState());

    }
    else if (sParameters.size()==2 && sValues.size()==1)
    {
        iPCmd->setParameter(sParameters.at(0));
        iPCmd->setValue(sValues.at(0));
        iPCmd->setCondition(getConditionOperator());
        iPCmd->setChkAudioAlarm(getChkAudioAlarm());
        iPCmd->setHeardAfter(getHeardAfter());
        iPCmd->setIsFsOnly(getCheckOnlyFSState());
        iPCmd->setIsValueOnly(getCheckOnlyValueState());
        iPCmd->setFunctionalStatus(getFunctionalStatus());
        iPCmd->setIsSDIOnly(getCheckOnlySDIState());
        iPCmd->setSDI(getSDI());
        iPCmd->setIsParityOnly(getCheckOnlyParityState());
        iPCmd->setParity(getParity());
        iPCmd->setIsRefreshRateOnly(getCheckOnlyRefreshRateState());

        GTAActionIF* pNestedIf = new GTAActionIF();
        GTAActionIfEnd* pEnd = new GTAActionIfEnd();
        pNestedIf->insertChildren(pEnd,0); //End appended along with if
        pNestedIf->setParameter(sParameters.at(1));
        pNestedIf->setValue(sValues.at(0));
        pNestedIf->setCondition(getConditionOperator());
        pNestedIf->setChkAudioAlarm(getChkAudioAlarm());
        pNestedIf->setHeardAfter(getHeardAfter());
        pNestedIf->setIsFsOnly(getCheckOnlyFSState());
        pNestedIf->setIsValueOnly(getCheckOnlyValueState());
        pNestedIf->setFunctionalStatus(getFunctionalStatus());
        pNestedIf->setIsSDIOnly(getCheckOnlySDIState());
        pNestedIf->setSDI(getSDI());
        pNestedIf->setIsParityOnly(getCheckOnlyParityState());
        pNestedIf->setParity(getParity());
        pNestedIf->setIsRefreshRateOnly(getCheckOnlyRefreshRateState());
        iPCmd->insertChildren(pNestedIf,0);

    }
    else if (sParameters.size()==2 && sValues.size()==2)
    {
        iPCmd->setParameter(sParameters.at(0));
        iPCmd->setValue(sValues.at(0));
        iPCmd->setCondition(getConditionOperator());
        iPCmd->setChkAudioAlarm(getChkAudioAlarm());
        iPCmd->setHeardAfter(getHeardAfter());
        iPCmd->setIsFsOnly(getCheckOnlyFSState());
        iPCmd->setIsValueOnly(getCheckOnlyValueState());
        iPCmd->setFunctionalStatus(getFunctionalStatus());
        iPCmd->setIsSDIOnly(getCheckOnlySDIState());
        iPCmd->setSDI(getSDI());
        iPCmd->setIsParityOnly(getCheckOnlyParityState());
        iPCmd->setParity(getParity());
        iPCmd->setIsRefreshRateOnly(getCheckOnlyRefreshRateState());

        GTAActionIF* pNestedIf = new GTAActionIF();
        GTAActionIfEnd* pEnd = new GTAActionIfEnd();
        pNestedIf->insertChildren(pEnd,0); //End appended along with if
        pNestedIf->setParameter(sParameters.at(1));
        pNestedIf->setValue(sValues.at(1));
        pNestedIf->setCondition(getConditionOperator());
        pNestedIf->setChkAudioAlarm(getChkAudioAlarm());
        pNestedIf->setHeardAfter(getHeardAfter());
        pNestedIf->setIsFsOnly(getCheckOnlyFSState());
        pNestedIf->setIsValueOnly(getCheckOnlyValueState());
        pNestedIf->setFunctionalStatus(getFunctionalStatus());
        pNestedIf->setIsSDIOnly(getCheckOnlySDIState());
        pNestedIf->setSDI(getSDI());
        pNestedIf->setIsParityOnly(getCheckOnlyParityState());
        pNestedIf->setParity(getParity());
        pNestedIf->setIsRefreshRateOnly(getCheckOnlyRefreshRateState());
        iPCmd->insertChildren(pNestedIf,0);

    }
    else
    {
        rc=false;
        _LastError = "Unexpected combination of parameters and values";
    }
    return rc;
}
bool GTAConditionActionWidget::createNestedCommandForChannels(GTAActionWhile* iPCmd , QString iParameter, QString iParameterVal )const
{

    bool rc = true;
    QStringList sParameters = GTAUtil::getProcessedParameterForChannel(iParameter);
    QStringList sValues     = GTAUtil::getProcessedParameterForChannel(iParameterVal);

    if (sParameters.size() <=1 && sValues.size()<=1)
    {
        iPCmd->setParameter(iParameter);
        iPCmd->setValue(iParameterVal);
        iPCmd->setCondition(getConditionOperator());
        iPCmd->setChkAudioAlarm(getChkAudioAlarm());
        iPCmd->setHeardAfter(getHeardAfter());
        iPCmd->setIsFsOnly(getCheckOnlyFSState());
        iPCmd->setIsValueOnly(getCheckOnlyValueState());
        iPCmd->setFunctionalStatus(getFunctionalStatus());
        iPCmd->setIsSDIOnly(getCheckOnlySDIState());
        iPCmd->setSDI(getSDI());
        iPCmd->setIsParityOnly(getCheckOnlyParityState());
        iPCmd->setParity(getParity());
        iPCmd->setIsRefreshRateOnly(getCheckOnlyRefreshRateState());
//        iPCmd->setIsRepeatedLogging(getCheckRepeatedLoggingState());
    }
    else if (sParameters.size()==2 && sValues.size()==1)
    {
        iPCmd->setParameter(sParameters.at(0));
        iPCmd->setValue(sValues.at(0));
        iPCmd->setCondition(getConditionOperator());
        iPCmd->setChkAudioAlarm(getChkAudioAlarm());
        iPCmd->setHeardAfter(getHeardAfter());
        iPCmd->setIsFsOnly(getCheckOnlyFSState());
        iPCmd->setIsValueOnly(getCheckOnlyValueState());
        iPCmd->setFunctionalStatus(getFunctionalStatus());
        iPCmd->setIsSDIOnly(getCheckOnlySDIState());
        iPCmd->setSDI(getSDI());
        iPCmd->setIsParityOnly(getCheckOnlyParityState());
        iPCmd->setParity(getParity());
        iPCmd->setIsRefreshRateOnly(getCheckOnlyRefreshRateState());
//        iPCmd->setIsRepeatedLogging(getCheckRepeatedLoggingState());

        GTAActionWhile* pNestedWhile = new GTAActionWhile();
        GTAActionWhileEnd* pEnd = new GTAActionWhileEnd();
        pNestedWhile->insertChildren(pEnd,0); //End appended along with if
        pNestedWhile->setParameter(sParameters.at(1));
        pNestedWhile->setValue(sValues.at(0));
        pNestedWhile->setCondition(getConditionOperator());
        pNestedWhile->setChkAudioAlarm(getChkAudioAlarm());
        pNestedWhile->setHeardAfter(getHeardAfter());
        pNestedWhile->setIsFsOnly(getCheckOnlyFSState());
        pNestedWhile->setIsValueOnly(getCheckOnlyValueState());
        pNestedWhile->setFunctionalStatus(getFunctionalStatus());

        pNestedWhile->setIsSDIOnly(getCheckOnlySDIState());
        pNestedWhile->setSDI(getSDI());
        pNestedWhile->setIsParityOnly(getCheckOnlyParityState());
        pNestedWhile->setParity(getParity());
        pNestedWhile->setIsRefreshRateOnly(getCheckOnlyRefreshRateState());
//        pNestedWhile->setIsRepeatedLogging(getCheckRepeatedLoggingState());

        iPCmd->insertChildren(pNestedWhile,0);

    }
    else if (sParameters.size()==2 && sValues.size()==2)
    {
        iPCmd->setParameter(sParameters.at(0));
        iPCmd->setValue(sValues.at(0));
        iPCmd->setCondition(getConditionOperator());
        iPCmd->setChkAudioAlarm(getChkAudioAlarm());
        iPCmd->setHeardAfter(getHeardAfter());
        iPCmd->setIsFsOnly(getCheckOnlyFSState());
        iPCmd->setIsValueOnly(getCheckOnlyValueState());
        iPCmd->setFunctionalStatus(getFunctionalStatus());
        iPCmd->setIsSDIOnly(getCheckOnlySDIState());
        iPCmd->setSDI(getSDI());
        iPCmd->setIsParityOnly(getCheckOnlyParityState());
        iPCmd->setParity(getParity());
        iPCmd->setIsRefreshRateOnly(getCheckOnlyRefreshRateState());
//        iPCmd->setIsRepeatedLogging(getCheckRepeatedLoggingState());

        GTAActionWhile* pNestedWhile = new GTAActionWhile();
        GTAActionWhileEnd* pEnd = new GTAActionWhileEnd();
        pNestedWhile->insertChildren(pEnd,0); //End appended along with if
        pNestedWhile->setParameter(sParameters.at(1));
        pNestedWhile->setValue(sValues.at(1));
        pNestedWhile->setCondition(getConditionOperator());
        pNestedWhile->setChkAudioAlarm(getChkAudioAlarm());
        pNestedWhile->setHeardAfter(getHeardAfter());
        pNestedWhile->setIsFsOnly(getCheckOnlyFSState());
        pNestedWhile->setIsValueOnly(getCheckOnlyValueState());
        pNestedWhile->setFunctionalStatus(getFunctionalStatus());
        pNestedWhile->setIsSDIOnly(getCheckOnlySDIState());
        pNestedWhile->setSDI(getSDI());
        pNestedWhile->setIsParityOnly(getCheckOnlyParityState());
        pNestedWhile->setParity(getParity());
        pNestedWhile->setIsRefreshRateOnly(getCheckOnlyRefreshRateState());
//        pNestedWhile->setIsRepeatedLogging(getCheckRepeatedLoggingState());

        iPCmd->insertChildren(pNestedWhile,0);

    }
    else
    {
        rc=false;
        _LastError = "Unexpected combination of parameters and values";
    }
    return rc;
}
bool GTAConditionActionWidget::createNestedCommandForChannels(GTAActionDoWhile* iPCmd , QString iParameter, QString iParameterVal )const
{

    bool rc = true;
    QStringList sParameters = GTAUtil::getProcessedParameterForChannel(iParameter);
    QStringList sValues     = GTAUtil::getProcessedParameterForChannel(iParameterVal);

    if (sParameters.size() <=1 && sValues.size()<=1)
    {
        iPCmd->setParameter(iParameter);
        iPCmd->setValue(iParameterVal);
        iPCmd->setCondition(getConditionOperator());
        iPCmd->setChkAudioAlarm(getChkAudioAlarm());
        iPCmd->setHeardAfter(getHeardAfter());
        iPCmd->setIsFsOnly(getCheckOnlyFSState());
        iPCmd->setIsValueOnly(getCheckOnlyValueState());
        iPCmd->setFunctionalStatus(getFunctionalStatus());
        iPCmd->setIsSDIOnly(getCheckOnlySDIState());
        iPCmd->setSDI(getSDI());
        iPCmd->setIsParityOnly(getCheckOnlyParityState());
        iPCmd->setParity(getParity());
        iPCmd->setIsRefreshRateOnly(getCheckOnlyRefreshRateState());
//        iPCmd->setIsRepeatedLogging(getCheckRepeatedLoggingState());
    }
    else if (sParameters.size()==2 && sValues.size()==1)
    {
        iPCmd->setParameter(sParameters.at(0));
        iPCmd->setValue(sValues.at(0));
        iPCmd->setCondition(getConditionOperator());
        iPCmd->setChkAudioAlarm(getChkAudioAlarm());
        iPCmd->setHeardAfter(getHeardAfter());
        iPCmd->setIsFsOnly(getCheckOnlyFSState());
        iPCmd->setIsValueOnly(getCheckOnlyValueState());
        iPCmd->setFunctionalStatus(getFunctionalStatus());
        iPCmd->setIsSDIOnly(getCheckOnlySDIState());
        iPCmd->setSDI(getSDI());
        iPCmd->setIsParityOnly(getCheckOnlyParityState());
        iPCmd->setParity(getParity());
        iPCmd->setIsRefreshRateOnly(getCheckOnlyRefreshRateState());
//        iPCmd->setIsRepeatedLogging(getCheckRepeatedLoggingState());

        GTAActionDoWhile* pNestedWhile = new GTAActionDoWhile();
        GTAActionDoWhileEnd* pEnd = new GTAActionDoWhileEnd();
        pNestedWhile->insertChildren(pEnd,0); //End appended along with if
        pNestedWhile->setParameter(sParameters.at(1));
        pNestedWhile->setValue(sValues.at(0));
        pNestedWhile->setCondition(getConditionOperator());
        pNestedWhile->setChkAudioAlarm(getChkAudioAlarm());
        pNestedWhile->setHeardAfter(getHeardAfter());
        pNestedWhile->setIsFsOnly(getCheckOnlyFSState());
        pNestedWhile->setIsValueOnly(getCheckOnlyValueState());
        pNestedWhile->setFunctionalStatus(getFunctionalStatus());

        pNestedWhile->setIsSDIOnly(getCheckOnlySDIState());
        pNestedWhile->setSDI(getSDI());
        pNestedWhile->setIsParityOnly(getCheckOnlyParityState());
        pNestedWhile->setParity(getParity());
        pNestedWhile->setIsRefreshRateOnly(getCheckOnlyRefreshRateState());
//        pNestedWhile->setIsRepeatedLogging(getCheckRepeatedLoggingState());

        iPCmd->insertChildren(pNestedWhile,0);

    }
    else if (sParameters.size()==2 && sValues.size()==2)
    {
        iPCmd->setParameter(sParameters.at(0));
        iPCmd->setValue(sValues.at(0));
        iPCmd->setCondition(getConditionOperator());
        iPCmd->setChkAudioAlarm(getChkAudioAlarm());
        iPCmd->setHeardAfter(getHeardAfter());
        iPCmd->setIsFsOnly(getCheckOnlyFSState());
        iPCmd->setIsValueOnly(getCheckOnlyValueState());
        iPCmd->setFunctionalStatus(getFunctionalStatus());
        iPCmd->setIsSDIOnly(getCheckOnlySDIState());
        iPCmd->setSDI(getSDI());
        iPCmd->setIsParityOnly(getCheckOnlyParityState());
        iPCmd->setParity(getParity());
        iPCmd->setIsRefreshRateOnly(getCheckOnlyRefreshRateState());
//        iPCmd->setIsRepeatedLogging(getCheckRepeatedLoggingState());

        GTAActionDoWhile* pNestedWhile = new GTAActionDoWhile();
        GTAActionDoWhileEnd* pEnd = new GTAActionDoWhileEnd();
        pNestedWhile->insertChildren(pEnd,0); //End appended along with if
        pNestedWhile->setParameter(sParameters.at(1));
        pNestedWhile->setValue(sValues.at(1));
        pNestedWhile->setCondition(getConditionOperator());
        pNestedWhile->setChkAudioAlarm(getChkAudioAlarm());
        pNestedWhile->setHeardAfter(getHeardAfter());
        pNestedWhile->setIsFsOnly(getCheckOnlyFSState());
        pNestedWhile->setIsValueOnly(getCheckOnlyValueState());
        pNestedWhile->setFunctionalStatus(getFunctionalStatus());

        pNestedWhile->setIsSDIOnly(getCheckOnlySDIState());
        pNestedWhile->setSDI(getSDI());
        pNestedWhile->setIsParityOnly(getCheckOnlyParityState());
        pNestedWhile->setParity(getParity());
        pNestedWhile->setIsRefreshRateOnly(getCheckOnlyRefreshRateState());
//        pNestedWhile->setIsRepeatedLogging(getCheckRepeatedLoggingState());

        iPCmd->insertChildren(pNestedWhile,0);

    }
    else
    {
        rc=false;
        _LastError = "Unexpected combination of parameters and values";
    }
    return rc;
}

void GTAConditionActionWidget::disableValueEditsOnRefresh(bool iStatus)
{

    disableValueEdits(iStatus);
    bool val = false;
    emit disablePrecision(val);
}

void GTAConditionActionWidget::disableValueEdits(bool iStatus)
{
    ui->ValueLE->setDisabled(iStatus);
    ui->SearchValuePB->setDisabled(iStatus);
    ui->ChkAudioAlarmCB->setDisabled(iStatus);
    hideNote(false);

    if (iStatus==true)
    {
        ui->OperatorCB->setCurrentIndex(0);
        QStringList operatorList;
        operatorList <<ARITH_EQ<<ARITH_NOTEQ;

        ui->OperatorCB->clear();
        ui->OperatorCB->addItems(operatorList);
        hideNote(false);

        //        if(ui->ChkFSOnlyCB->isChecked() || ui->ChkRefreshRate->isChecked() || ui->chkOnlyParity->isChecked() || ui->chkOnlySDI->isChecked())
        //        {
        //			ui->checkValnFS->hide();
        //            ui->checkFsWarLB->show();
        //        }
        //        else
        //        {
        //            if (!(ui->ChkValueCB->isChecked()))
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

void GTAConditionActionWidget::enableValueEdits(bool )
{

    if(ui->ChkValueCB->isChecked())
    {
        disableValueEdits(false);
        ui->ChkAudioAlarmCB->setDisabled(true);
    }
    else
    {
        if(ui->ChkFSOnlyCB->isChecked() || ui->chkOnlySDI->isChecked()
            || ui->chkOnlyParity->isChecked() || ui->ChkRefreshRate->isChecked())
            {
            disableValueEdits(true);
        }
        else
            disableValueEdits(false);

    }
}

void GTAConditionActionWidget::hideNote(bool)
{
    bool isBenchParamFnsChecked = (ui->ChkFSOnlyCB->isChecked() || ui->ChkRefreshRate->isChecked() || ui->chkOnlyParity->isChecked() || ui->chkOnlySDI->isChecked());
    bool isValueChecked = ui->ChkValueCB->isChecked();
    bool isAudAlrmChecked = ui->ChkAudioAlarmCB->isChecked();
    ui->checkValnFS->hide();
    ui->checkFsWarLB->hide();
    ui->labelwarForEach->hide();

    if (!isBenchParamFnsChecked && !isValueChecked && !isAudAlrmChecked)
    {
        ui->checkValnFS->show();
        ui->checkFsWarLB->hide();
    }
    else if (isValueChecked || isAudAlrmChecked)
    {
        ui->checkValnFS->hide();
        ui->checkFsWarLB->hide();
    }
    else if (isBenchParamFnsChecked)
    {
        ui->checkValnFS->hide();
        ui->checkFsWarLB->show();
    }
    //        ui->checkFsWarLB->setVisible(isChecked);
    //        bool showNote = (ui->ChkValueCB->isChecked() || (ui->ChkAudioAlarmCB->isChecked());
    //        if ((!isChecked && !(ui->ChkValueCB->isChecked()))|| !(ui->ChkAudioAlarmCB->isChecked()))
    //            ui->checkValnFS->show();
    //        else
    //            ui->checkValnFS->hide();
}

void GTAConditionActionWidget::resetCheckFS(bool iStatus)
{

    if(iStatus && ui->ChkFSOnlyCB->isChecked())
    {
        disconnect(ui->ChkFSOnlyCB,SIGNAL(toggled(bool)),this,SLOT(disableValueEdits(bool)));
        ui->ChkFSOnlyCB->setChecked(false);
        //        disableValueEdits(false);
        connect(ui->ChkFSOnlyCB,SIGNAL(toggled(bool)),this,SLOT(disableValueEdits(bool)));
    }

}

void GTAConditionActionWidget::resetCheckOnlyValue(bool iStatus)
{
    // disconnect(ui->ChkValueCB,SIGNAL(toggled(bool)),this,SLOT(enableValueEdits(bool)));
    if(iStatus && ui->ChkValueCB->isChecked())
    {
        ui->ChkValueCB->setChecked(false);
        //   enableValueEdits(false);
    }
    //connect(ui->ChkValueCB,SIGNAL(toggled(bool)),this,SLOT(enableValueEdits(bool)));
}

void GTAConditionActionWidget::resetSDI(bool iStatus)
{

    if(iStatus && ui->chkOnlySDI->isChecked())
    {
        disconnect(ui->chkOnlySDI,SIGNAL(toggled(bool)),this,SLOT(disableValueEdits(bool)));
        ui->chkOnlySDI->setChecked(false);
        //        disableValueEdits(false);
        connect(ui->chkOnlySDI,SIGNAL(toggled(bool)),this,SLOT(disableValueEdits(bool)));

    }
}

void GTAConditionActionWidget::resetParity(bool iStatus)
{
    if(iStatus && ui->chkOnlyParity->isChecked())
    {
        disconnect(ui->chkOnlyParity, SIGNAL(toggled(bool)),this,SLOT(disableValueEdits(bool)));

        ui->chkOnlyParity->setChecked(false);
        //        disableValueEdits(false);
        connect(ui->chkOnlyParity, SIGNAL(toggled(bool)),this,SLOT(disableValueEdits(bool)));

    }
}

void GTAConditionActionWidget::resetRefreshRate(bool iStatus)
{
    if(iStatus && ui->ChkRefreshRate->isChecked())
    {
        disconnect(ui->ChkRefreshRate, SIGNAL(toggled(bool)),this, SLOT(disableValueEditsOnRefresh(bool)));

        ui->ChkRefreshRate->setChecked(false);
        //        disableValueEditsOnRefresh(false);
        connect(ui->ChkRefreshRate, SIGNAL(toggled(bool)),this, SLOT(disableValueEditsOnRefresh(bool)));
    }
}

void GTAConditionActionWidget::showForEachCSVFiles(bool iVal)
{
	if(iVal)
    {
        ui->ListLB->setDisabled(iVal);
        ui->ListLE->setDisabled(iVal);
        ui->SearchTablePB->setDisabled(iVal);
        ui->ForEachPathCB->clear();

        GTAAppController* pAppcontroller = GTAAppController::getGTAAppController();
        if (NULL!=pAppcontroller)
        {
            GTASystemServices* pSystemService = pAppcontroller->getSystemService();
            if (NULL!=pSystemService)
            {
                QString repoPath = QString::fromStdString(TestConfig::getInstance()->getRepositoryPath());
                repoPath = repoPath.replace("\\", "/");
                QStringList fileList = pSystemService->getTableFiles();
                QString fileSelected;
                if (!ui->ListLE->text().isEmpty())
                {
                    fileSelected = ui->ListLE->text();
                }
                fileSelected = fileSelected.replace("\\", "/");
                QString currentLEText = "";
                foreach(QString file,fileList)
                {
                    if(fileSelected.compare(file, Qt::CaseSensitive) == 0)
                    {
                        // Contains the relative path of the selected file
                        
                        currentLEText = file.remove(repoPath);
                    }
                    else if (fileSelected.startsWith("/") && file.contains(fileSelected))
                    {
                        currentLEText = fileSelected;
                    }
                    ui->ForEachPathCB->addItem(file.remove(repoPath));
                }
                // By default, dislay the relative path of the selected file
                ui->ForEachPathCB->setCurrentText(currentLEText);
            }
            ui->ForEachPathCB->setVisible(iVal);
        }
    }
    else
    {
        disconnect(ui->ForEachPathCB,SIGNAL(currentIndexChanged(QString)),this,SLOT(forEachRelativePathSelected(QString)));
        ui->ListLB->setEnabled(true);
        ui->ListLE->setEnabled(true);
        ui->ForEachPathCB->setVisible(false);
        ui->SearchTablePB->setEnabled(true);
        ui->ForEachPathChkB->setChecked(false);
        ui->ForEachPathCB->clear();
        connect(ui->ForEachPathCB,SIGNAL(currentIndexChanged(QString)),this,SLOT(forEachRelativePathSelected(QString)));
    }
}

void GTAConditionActionWidget::forEachRelativePathSelected(const QString &relativePath)
{
    ui->ListLE->setText(relativePath);
}

bool GTAConditionActionWidget::setWidgetFields(const QList<QPair<QString, QString>>& params, ErrorMessageList& errorLogs) {
    bool success = true;
    resetAllCheckBoxStatus();
    if (_complement == COM_CONDITION_FOR_EACH) {
        success = success && setForEachWidget(params, errorLogs);
    }
    else {
        for (const auto& param : params) {
            success = success && CheckAndSetParameters(param, errorLogs);
            success = success && CheckAndSetConditionalOperator(param, errorLogs);
            success = success && CheckAndSetCheckboxes(param, errorLogs);
            success = success && CheckAndSetComboboxes(param, errorLogs);
        }
    }

    return isValid() && success;
}

bool GTAConditionActionWidget::setForEachWidget(const QList<QPair<QString, QString>>& params, ErrorMessageList& errorLogs) {
    bool success = true;
    success = success && setForEachPath(params, errorLogs);
    for (const auto& param : params) {
        if (param.second == "delimindex")
            success = success && setComboboxFromString(ui->ForEachCB, param.second, errorLogs);
        if (param.second == "resultOverwrite")
            success = success && setCheckboxFromString(ui->ForEachChkB, param.second, errorLogs);
    }

    return success;
}

bool GTAConditionActionWidget::setForEachPath(const QList<QPair<QString, QString>>& params, ErrorMessageList& errorLogs) {
    bool isRelativePath = checkIfPathIsRelative(params, errorLogs);
    QString path;
    for (const auto& param : params) {
        if (param.first == "path") {
            path = param.second;
            checkPath(path);
            ui->ForEachPathCB->addItem(path);
            int index = ui->ForEachPathCB->findText(path);
            ui->ForEachPathCB->setCurrentIndex(index);
            auto check_path = ui->ForEachPathCB->currentText();
        }
    }
    if (isRelativePath)
    {
        QString repoPath = QString::fromStdString(TestConfig::getInstance()->getRepositoryPath());
        int index = ui->ForEachPathCB->findText(path.remove(repoPath));
        if (index >= 0) {
            ui->ForEachPathCB->setCurrentIndex(index);
            return true;
        }
        else {

            errorLogs.append(QString("Given path: %1 is not avaible").arg(path));
            return false;
        }
    }
}

/**
 * @brief Checks if there is relative path parameter, it needs to be checked before path check
*/
bool GTAConditionActionWidget::checkIfPathIsRelative(const QList <QPair<QString, QString>>& params, ErrorMessageList& errorLogs) {
    bool isRelativePath = false;
    for (const auto& param : params) {
        if (param.first == "isRelativePath") {
            param.second.toLower().contains("true") ? isRelativePath = true : isRelativePath = false;
            ui->ForEachPathChkB->setChecked(isRelativePath);
        }
    }

    return isRelativePath;
}


bool GTAConditionActionWidget::CheckAndSetCheckboxes(const QPair<QString, QString>& param, ErrorMessageList& errorLogs) {
    if (param.first == "chkOnlySDI")
        return setCheckboxFromString(ui->chkOnlySDI, param.second, errorLogs);
    else if (param.first == "chkOnlyParity")
        return setCheckboxFromString(ui->chkOnlyParity, param.second, errorLogs);

    return true;
}

bool GTAConditionActionWidget::CheckAndSetComboboxes(const QPair<QString, QString>& param, ErrorMessageList& errorLogs) {
    if (param.first == "CheckSDICB")
        return setComboboxFromString(ui->CheckSDICB, param.second, errorLogs);
    else if (param.first == "CheckParityCB")
        return setComboboxFromString(ui->CheckParityCB, param.second, errorLogs);

    return true;
}

bool GTAConditionActionWidget::setCheckboxFromString(QCheckBox* checkbox, QString booleanStr, ErrorMessageList& errorLogs)
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
        errorLogs.append(QString("%1 is neither true or false, this parameter supports only boolean values").arg(booleanStr));
        return false;
    }
}

/**
 * @brief Method for setting combobox from string
 * @param inputStr Can be either an index number or a field name
*/
bool GTAConditionActionWidget::setComboboxFromString(QComboBox* combobox, QString inputStr, ErrorMessageList& errorLogs)
{
    bool isInt = false;
    auto index = inputStr.toInt(&isInt);
    if (isInt) {
        if (combobox->count() >= index) {
            combobox->setCurrentIndex(index);
            return true;
        }
        else {
            errorLogs.append(QString("Combobox given index: %1 is greater than maximum index: %2").arg(index, combobox->count()));
            return false;
        }
    }
    else {
        index = combobox->findText(inputStr);
        if (index >= 0 && index < ui->CheckSDICB->count()) {
            ui->CheckSDICB->setCurrentIndex(index);
        }
        else {
            errorLogs.append(QString("Combobox given index: %1 is not valid").arg(inputStr));
            return false;
        }
    }
}

bool GTAConditionActionWidget::CheckAndSetConditionalOperator(const QPair<QString, QString>& param, ErrorMessageList& errorLogs) {
    auto& condition = param.second;
    if (param.first == "Condition") {
        int index = ui->OperatorCB->findText(condition);
        if (index > 0) {
            ui->OperatorCB->setCurrentIndex(index);
            return true;
        }
        else {
            errorLogs.append(QString("Condition %1 is not supported").arg(condition));
            return false;
        }
    }

    return true;
}

bool GTAConditionActionWidget::CheckAndSetParameters(const QPair<QString, QString>& param, ErrorMessageList& errorLogs) {
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
REGISTER_ACTION_WIDGET(QString("%1 %2").arg(ACT_CONDITION,COM_CONDITION_ELSE),GTAConditionActionWidget,obj)
        REGISTER_ACTION_WIDGET(QString("%1 %2").arg(ACT_CONDITION,COM_CONDITION_IF),GTAConditionActionWidget,obj1)
        REGISTER_ACTION_WIDGET(QString("%1 %2").arg(ACT_CONDITION,COM_CONDITION_WHILE),GTAConditionActionWidget,obj2)
        REGISTER_ACTION_WIDGET(QString("%1 %2").arg(ACT_CONDITION,COM_CONDITION_DO_WHILE),GTAConditionActionWidget,obj3)
        REGISTER_ACTION_WIDGET(QString("%1 %2").arg(ACT_CONDITION,COM_CONDITION_FOR_EACH),GTAConditionActionWidget,obj4)
