#include "GTALogFrame.h"
#include "ui_GTALogFrame.h"
#include "GTASystemServices.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QtConcurrentRun>
#include <QFuture>
#include <QFutureWatcher>
#include <QProcess>
#include <QDesktopServices>
#include <QUrl>
#include "GTAAppController.h"
#include "GTAComboBoxDelegatorLogResulTV.h"

#include "GTAICDParameter.h"
#include "GTAValidationWidgetModel.h"
#include <QPointer>
#include <QDebug>
#include <QCheckBox>
#include <QDialogButtonBox>

#define RESULT_PAGE_UTC_COL_NO 12

GTALogFrame::GTALogFrame(GTAAppController *pController, QWidget *parent) :
    QFrame(parent),
    ui(new Ui::GTALogFrame),_pAppController(pController)
{
    ui->setupUi(this);


    setFocusPolicy(Qt::StrongFocus);
    _pGenSearchWdgt = new GTAGenSearchWidget();
    _pGenSearchWdgt->hide();

    _pLogAttachWdgt = new GTALogAttachmentWidget();
    _pLogAttachWdgt->hide();

    _pFilterModel = new GTAFilterModel(nullptr);
    ui->AnalyzeLogPB->setEnabled(false);


    ui->purposeAndConclusionOkCancelButtons->button(QDialogButtonBox::Ok)->setToolTip("Save the changes");
    ui->purposeAndConclusionOkCancelButtons->button(QDialogButtonBox::Cancel)->setToolTip("Discard the changes");
    ui->purposeAndConclusionOkCancelButtons->setEnabled(false);
    ui->purposeAndConclusionOkCancelButtons->setVisible(false);

    ui->purposeLE->setEnabled(false);
    ui->purposeLE->setVisible(false);
    ui->purposeLB->setVisible(false);

    ui->conclusionLE->setEnabled(false);
    ui->conclusionLE->setVisible(false);
    ui->conclusionLB->setVisible(false);

    connect(ui->AnalyzeLogPB,SIGNAL(clicked()),this,SLOT(onAnalyzePBClicked()));
    connect(ui->purposeAndConclusionOkCancelButtons, SIGNAL(accepted()), this, SLOT(onPurposeAndConclusion_OkClicked()));
    connect(ui->purposeAndConclusionOkCancelButtons, SIGNAL(rejected()), this, SLOT(onPurposeAndConclusion_CancelClicked()));
    connect(ui->purposeAndConclusionGB, SIGNAL(clicked()), this, SLOT(onPurposeAndConclusion_CheckBoxClicked()));
    //connect(ui->LogViewTV,SIGNAL(expanded(QModelIndex)),this,SLOT(resizeColumnToContents()));	// #321442 : Disabled it so as to prevent column resizing on Expand/collapse All option in Log window.

    connect(ui->BrowseLogPB,SIGNAL(clicked()),this,SLOT(onBrowsePBClicked()));
    connect(ui->SearchPB,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));
    connect(_pGenSearchWdgt,SIGNAL(okPressed()),this,SLOT(onSearchWidgetOKClicked()));
    connect(ui->AttachmentPB,SIGNAL(clicked()),this,SLOT(onAttachmentPBClicked()));
    connect(ui->FilterPB,SIGNAL(clicked()),this,SLOT(onFilterPBClicked()));
    connect(ui->SearchFilterLE,SIGNAL(textEdited(QString)),this,SLOT(clearSearch()));
    connect(ui->DocFileLE,SIGNAL(textChanged(QString)),this,SLOT(enableAnalyseButton(QString)));
    connect(ui->LogFileLE,SIGNAL(textChanged(QString)),this,SLOT(enableAnalyseButton(QString)));
    _pProgressBar = new GTAProgress(this);
    _pProgressBar->hide();

    connect(ui->LogViewTV,SIGNAL(collapsed(QModelIndex)),this,SLOT(onCollapsed(QModelIndex)));
    connect(ui->LogViewTV,SIGNAL(expanded(QModelIndex)),this,SLOT(onExpanded(QModelIndex)));
    connect(ui->lastExecPB,SIGNAL(clicked()),this,SLOT(onGetLastExecution()));
    connect(ui->LogViewTV,SIGNAL(UpdateErrorLog(ErrorMessageList)),this,SIGNAL(UpdateErrorLog(ErrorMessageList)));

    _ModelDataChanged = false;

    std::string state_str = TestConfig::getInstance()->getResultPageGeometry();
    QByteArray stateHex(state_str.c_str(), static_cast<int>(state_str.length()));
    QByteArray state = QByteArray::fromHex(stateHex);
    if(!state.isEmpty())
        ui->LogViewTV->header()->restoreState(state);

    _pValidationModel = NULL;
    _pLastAnalyzedElement = NULL;
    _ioLogInstanceIDMap.clear();
    _cliMode = false;
}

GTALogFrame::~GTALogFrame()
{
    if(_pGenSearchWdgt != NULL) delete _pGenSearchWdgt; _pGenSearchWdgt = NULL;
    if(_pLogAttachWdgt != NULL) delete _pLogAttachWdgt; _pLogAttachWdgt = NULL;
    if (NULL != _pValidationModel) delete _pValidationModel;_pValidationModel = NULL;
    if (NULL != _pLastAnalyzedElement) { delete _pLastAnalyzedElement; _pLastAnalyzedElement = NULL; }

    ui->LogViewTV->setModel(nullptr);
    QAbstractItemModel* loadedModel = _pFilterModel->sourceModel();
    _pFilterModel->setSourceModel(nullptr);
    if (nullptr != loadedModel){delete loadedModel;loadedModel = nullptr;}
    if (nullptr != _pFilterModel){delete _pFilterModel;_pFilterModel = nullptr;}

    // Normally, this is no longer needed, as changes are automatically commit before this destructor (exit button)
    /*GTAAppSettings settings;
    QByteArray state = ui->LogViewTV->header()->saveState();
    settings.setResultPageGeometry(state);
    
    QByteArray stateHex = ui->LogViewTV->header()->saveState();
    std::string state(stateHex.constData(), stateHex.length());
    TestConfig::getInstance()->setDataTVGeometry(state);
    */

    delete ui;
}

void GTALogFrame::onRowDoubleClick(const QModelIndex &iIndex)
{
    if(iIndex.isValid())
    {
        QAbstractItemModel *pModel = ui->LogViewTV->model();
        bool bModelChanged = false;
        ui->LogViewTV->showDetails(pModel, iIndex, bModelChanged);
        _ModelDataChanged = bModelChanged;
    }
}


void GTALogFrame::callToExpandAll()
{
    ui->LogViewTV->expandAll();
}

void GTALogFrame::callToCollapseAll()
{
    ui->LogViewTV->collapseAll();
}

void GTALogFrame::onGetLogModelFinished(std::shared_ptr<LogModelOutput> iOutput)
{
    QString docFile = ui->DocFileLE->text();
    QString logFile = ui->LogFileLE->text();
    hideProgressBar();
    QFutureWatcher<LogModelOutput> *pWatcher = dynamic_cast<QFutureWatcher<LogModelOutput> *>(sender());
    if(pWatcher == NULL && iOutput == nullptr)
    {
        return;
    }
    else
    {
        LogModelOutput output = (iOutput == nullptr) ? pWatcher->result() : *iOutput;
        QAbstractItemModel * pModel = output.pModel;
        _ColumnList = output.columnList;
        _hasFailed = output.hasFailed;
        _logErrorMsg = output.logErrorMessage;
        _ioLogInstanceIDMap = output.ioLogInstanceIDMap;
        if (NULL != _pValidationModel)
        {
            _pValidationModel = NULL;
        }
        _pValidationModel = output.pValidationModel;
        /*_attachmentList = output.attachments;*/
        bool rc = output.returnStatus;

        if(pModel != NULL)
        {
            QPointer<GTAEditorLogModel> model = dynamic_cast<GTAEditorLogModel*>(pModel);
            if (NULL != model)
            {
                GTAElement* pElem= model->getDataModel();
                _pLastAnalyzedElement = new GTAElement(*pElem);
            }

            ui->ColumnFilterCB->clear();
            ui->ColumnFilterCB->addItems(_ColumnList);
            if(_pFilterModel !=NULL)
            {
                QAbstractItemModel* loadedModel = _pFilterModel->sourceModel();
				_pFilterModel->setSourceModel(nullptr);
				ui->LogViewTV->setModel(nullptr);
                if (nullptr != loadedModel)
                {
                    delete loadedModel;
                    loadedModel = nullptr;
                }
                _pFilterModel->setSourceModel(pModel);

                std::string state_str = TestConfig::getInstance()->getResultPageGeometry();
                QByteArray stateHex(state_str.c_str(), static_cast<int>(state_str.length()));
                QByteArray state = QByteArray::fromHex(stateHex);
                if(!state.isEmpty())
                    ui->LogViewTV->header()->restoreState(state);

                int rowCount = pModel->rowCount();
                ui->LogViewTV->setModel(_pFilterModel);
                //setting only needed columns
                ui->LogViewTV->setdefaults();
                ui->LogViewTV->setColumnHidden(RESULT_PAGE_UTC_COL_NO,true);

                connect(ui->LogViewTV,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(onRowDoubleClick(QModelIndex)));
                for(int rowIdx=0;rowIdx<rowCount;rowIdx++)
                {
                    QModelIndex idx= pModel->index(rowIdx,0);
                    if(idx.isValid())
                        ui->LogViewTV->setExpanded(idx,true);
                }
                for(int rowIdx=0;rowIdx<rowCount;rowIdx++)
                {
                    QModelIndex idx= pModel->index(rowIdx,0);
                    if(idx.isValid())
                        ui->LogViewTV->setExpanded(idx,false);
                }

            }
            resizeColumnToContents();
            emit ui->LogViewTV->dataChanged(QModelIndex(),QModelIndex());

            QString gtaDataDir = _pAppController->getGTADataDirectory();
            QString gtaLogDir = _pAppController->getGTALogDirectory();
            _pLogAttachWdgt->clear();
            _pLogAttachWdgt->instertItem(QDir::cleanPath(
                docFile.contains(gtaDataDir) ? docFile : gtaDataDir + QDir::separator() + docFile
            ));
            _pLogAttachWdgt->instertItem(QDir::cleanPath(
                logFile.contains(gtaLogDir) ? logFile : gtaLogDir + QDir::separator() + logFile
            ));

            GTAElement* pElement = NULL;    
            QString filename = docFile.replace(gtaDataDir, "");
            _pAppController->getElementFromDocument(filename, pElement, false);
            QString inputCsvFilePath = pElement ? pElement->getInputCsvFilePath() : "NO INPUT CSV";
            QString outputCsvFilePath = pElement ? pElement->getOutputCsvFilePath() : "NO OUTPUT CSV";
            _pLogAttachWdgt->instertItem(inputCsvFilePath);
            _pLogAttachWdgt->instertItem(outputCsvFilePath);
            
            foreach (QString attachment, _attachmentList)
            {
                _pLogAttachWdgt->instertItem(QDir::cleanPath(attachment));
            }

            ui->SearchFilterLE->clear();
            clearSearch();
            if(_hasFailed == "KO")
            {
                QPixmap pixmap(":/images/RedSphere");
                ui->LogStatusLabel->setPixmap(pixmap);
                ui->LogStatusLabel->setToolTip("FAIL");
                //QMessageBox::information(this,"Global Test Status",QString("Test %1, has failed").arg(docFile));
            }
            else if(_hasFailed == "OK")
            {
                // QMessageBox::information(this,"Global Test Status",QString("Test %1, has passed").arg(docFile));
                QPixmap pixmap(":/images/GreenSphere");
                ui->LogStatusLabel->setPixmap(pixmap);
                ui->LogStatusLabel->setToolTip("PASS");
            }
            else
            {
                QPixmap pixmap;
                ui->LogStatusLabel->setPixmap(pixmap);
                ui->LogStatusLabel->setToolTip(QString());
            }
        }
        if(!rc && !logFile.isEmpty())
        {
            QMessageBox* pMsgBox = new QMessageBox(this);
            pMsgBox->setWindowTitle("Result load failed");
            pMsgBox->setText(_pAppController->getLastError());
            //  QMessageBox::warning(this,"Result load failed",_pAppController->getLastError());


            pMsgBox->exec();
            if (_logErrorMsg.isEmpty()==false)
            {
                QStringList errors;
                _logErrorMsg.prepend("LOG ERRORS:\n");
                errors << _logErrorMsg;
                ErrorMessageList msgList = createResultErrorLogs(errors);
                emit UpdateErrorLog(msgList);
            }
        }
        setLogFrameWindowTitle();
        pWatcher->deleteLater();
    }
}


ErrorMessageList GTALogFrame::createResultErrorLogs(QStringList &iList)const
{
    ErrorMessageList msgList;
    foreach(QString message,iList)
    {
        ErrorMessage logMessage;
        logMessage.description = message;
        logMessage.errorType = ErrorMessage::ErrorType::kError;
        logMessage.source = ErrorMessage::MsgSource::kResult;
        msgList.append(logMessage);
    }
    return msgList;
}

void GTALogFrame::messageWrn(const QString& message)
{
    if (!_cliMode)
        QMessageBox::warning(this, tr("WARNING"),
            tr(message.toStdString().c_str()),
            QMessageBox::Ok, QMessageBox::Ok);
    else
        cout(" *" + message);
}

/**
 * @brief Display the purpose and conclusion of the current file in EditorView
 * @param docFile : relative path of the current file
 */
void GTALogFrame::displayPurposeAndConclusion(const QString& docFile)
{
    if (_pAppController == nullptr)
        return;
    
    QString gtaDataDir = _pAppController->getSystemService()->getDataDirectory();
    QString docFullPath = QDir::cleanPath(QString("%1%2").arg(gtaDataDir, docFile));
    if (QFileInfo procFileInfo(docFullPath); !procFileInfo.exists())
        return;
           
    _pAppController->getElementFromDocument(docFile, _currentAnalyzedElement, false);
    _purpose = _currentAnalyzedElement->getPurposeForProcedure();
    _conclusion = _currentAnalyzedElement->getConclusionForProcedure();

    ui->purposeLE->setEnabled(true);
    ui->purposeLE->setPlainText(_purpose);

    ui->conclusionLE->setEnabled(true);
    ui->conclusionLE->setPlainText(_conclusion); 

    connect(ui->purposeLE, SIGNAL(textChanged()), this, SLOT(onPurposeAndConclusion_TextEdited()));
    connect(ui->conclusionLE, SIGNAL(textChanged()), this, SLOT(onPurposeAndConclusion_TextEdited()));
}

/**
 * @brief SLOT on Analyze button clicked
 * This allows to display the result details as well as editable Purpose & Conclusion related to the current file
 */
void GTALogFrame::onAnalyzePBClicked()
{
    if(_pAppController != NULL)
    {
        QString gtaDataDir = _pAppController->getSystemService()->getDataDirectory();
        QString gtaLogDir = _pAppController->getSystemService()->getLogDirectory();

        bool isSingleLog = true;
        QString docFile = ui->DocFileLE->text().remove(gtaDataDir);
        QString logFile = ui->LogFileLE->text().remove(gtaLogDir);

		// We get only the file name for each file to be compared
		QString logFile_short = logFile.split("/").last().split(".").first();
		QString docFile_short = docFile.split("/").last().split(".").first();

        //checking for procedure file modification after execution
        QString docFullPath = QDir::cleanPath(QString("%1%2").arg(gtaDataDir,docFile));
        QString logFullPath = QDir::cleanPath(QString("%1%2").arg(gtaLogDir, logFile));

        QFileInfo logFileInfo(logFullPath);
        QFileInfo procFileInfo(docFullPath);
        
        // Display the Purpose and Conclusion for current file
        displayPurposeAndConclusion(docFile);

        if (logFileInfo.exists() && procFileInfo.exists())
        {
            if (!logFile_short.contains(docFile_short))
                messageWrn("CAUTION: The files to compare are different!\n\tThis can be caused by comparing different procedures or by comparing unfinished procedure");
			
			QDateTime logmodified = logFileInfo.lastModified();
            QDateTime procmodified = procFileInfo.lastModified();

            //checking if procedure was modified after log generation
            if (logmodified < procmodified && !_ignorePurposeAndConlusionUpdate)
                messageWrn("CAUTION: Procedure has been modified after log generation.\n\tAnalysis might be affected!");
        }
		else if(logFileInfo.exists())
		{
			messageWrn("CAUTION: The ELEMENT file was not found!");
			return;
		}
		else
		{
			 messageWrn("CAUTION: The LOG file was not found!");
		}

		LogModelInput input;
		input.docFileName = docFile;
		input.logFileName = logFile;

        if(docFile.endsWith(".seq"))
        {
            bool rc = _pAppController->isSequenceWithMultipleLogs(logFile);
            if(rc)
            {
                isSingleLog = false;
                QStringList procs;
                QStringList logs;
                QStringList status;
                QString globalStatus;
                LogModelSequenceIO model;
                model.SeqDocFile = input.docFileName;
                model.SeqLogFile = input.logFileName;

                rc = _pAppController->readSequenceMultipleLogFile(logFile,logs,procs,status,globalStatus,model);
                //call getLogModel for multiple log
                if (rc)
                {
                    if(!_cliMode) 
                    {
                        showProgressBar("Analyzing Sequence...");
                        QFutureWatcher<LogModelOutput>* FutureWatcher = new QFutureWatcher<LogModelOutput>();
                        connect(FutureWatcher, SIGNAL(finished()), this, SLOT(onGetLogModelFinished()));
                        QFuture<LogModelOutput> Future = QtConcurrent::run(_pAppController, &GTAAppController::getLogModelForSequence, model);
                        FutureWatcher->setFuture(Future);
                    }
                    else
                    {
                        onGetLogModelFinished(std::make_shared<LogModelOutput>(_pAppController->getLogModelForSequence(model)));
                    }
                }
                else
                {
                    if (!_cliMode) QMessageBox::critical(this, "Error with Multiple Logs", _pAppController->getLastError());
                    else cout(" *error: Error with Multiple Logs");
                }
            }
        }

        if(isSingleLog)
        {
            _ColumnList.clear();

            _hasFailed.clear();
            _hasFailed = "None";
            _logErrorMsg.clear();


            if(!_cliMode) 
            {
                showProgressBar("Analyzing...");
                QFutureWatcher<LogModelOutput>* FutureWatcher = new QFutureWatcher<LogModelOutput>();
                connect(FutureWatcher,SIGNAL(finished()),this,SLOT(onGetLogModelFinished()));
                QFuture<LogModelOutput> Future = QtConcurrent::run(_pAppController,&GTAAppController::getLogModel,input,true);
                FutureWatcher->setFuture(Future);
            }
            else
            {
                onGetLogModelFinished(std::make_shared<LogModelOutput>(_pAppController->getLogModel(input, true)));
            }
        }
    }
}


/**
 * @brief SLOT on OK button of Purpose&Conclusion clicked
 * This saves the changes into the edited procedure
 */
void GTALogFrame::onPurposeAndConclusion_OkClicked()
{
    _purpose = ui->purposeLE->toPlainText();
    _conclusion = ui->conclusionLE->toPlainText();

    _currentAnalyzedElement->setPurposeForProcedure(_purpose);
    _currentAnalyzedElement->setConclusionForProcedure(_conclusion);

    const QString docName = _currentAnalyzedElement->getName();
    const QString docPath = _currentAnalyzedElement->getRelativeFilePath();
    if (GTAHeader* docHeader; _pAppController->getHeaderFromDocument(docPath, docHeader))
    {        
        const QString status = _pAppController->saveDocument(docName, true, _currentAnalyzedElement, docHeader, docPath) ? "Succeeded" : "Failed";            
        ui->purposeAndConclusionOkCancelButtons->setDisabled(true);
        ui->purposeAndConclusionGB->setTitle("Purpose/Conclusion");
        ui->purposeLB->setText("Purpose:");
        ui->conclusionLB->setText("Conclusion:");

        QMessageBox msgBox;
        msgBox.setWindowTitle("Purpose & Conclusion");          
        msgBox.setText(QString("%1 to update Purpose & Conclusion\n for %2 file").arg(status, docPath));            
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();    

        _ignorePurposeAndConlusionUpdate = true;
        onPurposeAndConclusion_TextEdited();
    }
}

/**
 * @brief SLOT on CANCEL button of Purpose&Conclusion clicked
 * This restores the displayed purpose and conclusion
 */
void GTALogFrame::onPurposeAndConclusion_CancelClicked()
{
    ui->purposeLE->setPlainText(_purpose);
    ui->conclusionLE->setPlainText(_conclusion);
    ui->purposeAndConclusionGB->setTitle("Purpose/Conclusion");
    ui->purposeLB->setText("Purpose:");
    ui->conclusionLB->setText("Conclusion:");
    _ignorePurposeAndConlusionUpdate = false;
}

/**
 * @brief SLOT on text changed for Purpose and/or Conclusion field
 * Whenever the texts have been changed, the button box becomes enabled
 */
void GTALogFrame::onPurposeAndConclusion_TextEdited()
{
    const QString new_purpose = ui->purposeLE->toPlainText();
    const QString new_conclusion = ui->conclusionLE->toPlainText();
    bool changed = (new_purpose != _purpose) || (new_conclusion != _conclusion);
    const QString something_edited = changed ? "Purpose/Conclusion (Click on OK to apply changes!)" : "Purpose/Conclusion";
    const QString purpose_edited = new_purpose != _purpose ? "* Purpose:" : "Purpose:";
    const QString conclusion_edited = new_conclusion != _conclusion ? "* Conclusion:" : "Conclusion:";
    
    ui->purposeLB->setText(purpose_edited);
    ui->conclusionLB->setText(conclusion_edited);
    ui->purposeAndConclusionGB->setTitle(something_edited);
    ui->purposeAndConclusionOkCancelButtons->setEnabled(changed);
}

/**
 * @brief SlOT on Purpose/Conclusion checkbox clicked
 * If checked, the fields are displayed. Otherwise, they are hidden.
 */
void GTALogFrame::onPurposeAndConclusion_CheckBoxClicked()
{
    bool toogled = ui->purposeAndConclusionGB->isChecked();
    ui->purposeAndConclusionOkCancelButtons->setVisible(toogled);
    ui->purposeLE->setVisible(toogled);
    ui->purposeLB->setVisible(toogled);
    ui->conclusionLE->setVisible(toogled);
    ui->conclusionLB->setVisible(toogled);
}

void GTALogFrame::resizeColumnToContents()
{
    int colCnt = ui->LogViewTV->model()->columnCount();
    for(int i=0;i<colCnt;i++)
    {
        ui->LogViewTV->resizeColumnToContents(i);
    }
    ui->LogViewTV->setColumnWidth(0,500);
}


void GTALogFrame::onBrowsePBClicked()
{
    /*  QString progName,equipName, stdName;
    _pAppController->getProgramConfiguration(progName,equipName, stdName);*/
    GTAAppController* pAppcontroller = GTAAppController::getGTAAppController();
    if (NULL!=pAppcontroller)
    {
        GTASystemServices* pSystemService = pAppcontroller->getSystemService();
        if (NULL!=pSystemService)
        {
            QString logDir = pSystemService->getLogDirectory();
            QString fileFilter("*.log");
            QString logFile;
            if(ui->LogFileLE->text().isEmpty())
            {
                logFile = QFileDialog::getOpenFileName(this,"Browse Log",logDir,fileFilter);
            }
            else
            {
                QString localPath = ui->LogFileLE->text();
                QFileInfo fileInfo(localPath);
                logFile = QFileDialog::getOpenFileName(this,"Browse Log",fileInfo.absolutePath(),fileFilter);
                if(logFile.isEmpty())
                    logFile = localPath;
            }
            ui->LogFileLE->setText(QDir::cleanPath(logFile));
        }
    }
}
void GTALogFrame::onSearchPBClicked()
{

    if(ui->ElementTypeCB->currentText() == "Procedure")
        _pGenSearchWdgt->setSearchType(GTAGenSearchWidget::RESULT_PRO);
    else if(ui->ElementTypeCB->currentText() == "Sequence")
        _pGenSearchWdgt->setSearchType(GTAGenSearchWidget::RESULT_SEQ);

    _pGenSearchWdgt->show();

}
void GTALogFrame::onSearchWidgetOKClicked()
{
    GTAICDParameter selectedItem = _pGenSearchWdgt->getSelectedItems();

    if(! selectedItem.getName().isEmpty())
        ui->DocFileLE->setText(selectedItem.getName());
    _pGenSearchWdgt->hide();
    this->onSetFocus();
}

void GTALogFrame::onAttachmentPBClicked()
{
    if(_pLogAttachWdgt != NULL)
    {
        _pLogAttachWdgt->show();
    }
}


void GTALogFrame::onFilterPBClicked()
{
    if(_pFilterModel == NULL)
        return;
    QString textToSearch = ui->SearchFilterLE->text();
    QString filterCriteria = ui->ColumnFilterCB->currentText();

    QRegExp regExp(textToSearch,Qt::CaseInsensitive,QRegExp::RegExp);
    QList<int> _cols;

    if(!filterCriteria.isEmpty())
        _cols << _ColumnList.indexOf(filterCriteria);
    else
        _cols << 0;
    _pFilterModel->setSearchcolumns(_cols);
    _pFilterModel->setSearchStrings(QStringList(textToSearch));
    _pFilterModel->setSearchCriteria(filterCriteria);
    _pFilterModel->setFilterRegExp(regExp);
}

void GTALogFrame::clearSearch()
{
    if(_pFilterModel == NULL)
        return;
    QString filterText1 = ui->SearchFilterLE->text();

    if((filterText1 == "") )
    {
        QRegExp regExp("",Qt::CaseInsensitive,QRegExp::RegExp);
        QList<int> _cols;
        _cols<<0;
        QStringList texts;
        texts<<"";
        _pFilterModel->setSearchcolumns(_cols);
        _pFilterModel->setSearchStrings(texts);
        _pFilterModel->setSearchCriteria("");
        _pFilterModel->setFilterRegExp(regExp);
    }
    else
    {
        if((ui->SearchFilterLE == sender()) && (filterText1 == ""))
        {
            onFilterPBClicked();
        }

    }

}

void GTALogFrame::cout(const QString& txt) 
{
    if(_cliMode)
    {
        QTextStream out(stdout);
        out << txt + "\n";
    }
}

bool GTALogFrame::exportReport(bool cliMode, const QString iSaveFormat)
{
    _cliMode = cliMode;
    bool rc = false;
    if(_pLogAttachWdgt == NULL)
        return rc;

    QStringList attachmentList = _pLogAttachWdgt->getItemList();
    if(attachmentList.isEmpty())
    {
        QString msg = "Attachment list is empty. ";
        if (cliMode)
        {
            cout(" *error: " + msg + "Canceling operation.");
            rc = false;
            return rc;
        }
        else
        {
            QMessageBox msgBox (this);
            msgBox.setText(msg + "Do you want to continue?");
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setWindowTitle("Export LTRA");
            msgBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::No);
            int ret = msgBox.exec();
            if(ret==QMessageBox::No)
            {
                rc = false;
                return rc;
            }
        }
    }

    //creating a ui to handle analysis in ms and option of report
    QDialog *exportLTRADialog = new QDialog();
    exportLTRADialog->setWindowTitle("Export LTRA");
    exportLTRADialog->setWindowIcon(QIcon(QPixmap(":/images/forms/img/GTAAppIcon_black.png")));
    exportLTRADialog->setWindowFlags(((exportLTRADialog->windowFlags()|Qt::CustomizeWindowHint) & ~Qt::WindowCloseButtonHint));
    exportLTRADialog->accept();

    QGridLayout *gridLayout = new QGridLayout(exportLTRADialog);

    QLabel *infoGenerateReport = new QLabel(exportLTRADialog);
    infoGenerateReport->setText("Save Report As");

    QLabel *infoTimingFormat = new QLabel(exportLTRADialog);
    infoTimingFormat->setText("Analyze time in milliseconds");

    QComboBox *docFormat = new QComboBox(exportLTRADialog);
    docFormat->addItem("DOCX");
    docFormat->addItem("XML");
    docFormat->addItem("DOCX + XML");
    QString saveFormat = "DOCX";

    QCheckBox* generateBenchVersions = new QCheckBox(exportLTRADialog);
    generateBenchVersions->setText("Generate Bench Versions");
    QFile file(QCoreApplication::applicationDirPath() + QDir::separator() + FILE_VERSIONS_TOOLS_BENCH);
    if (!file.exists()) {
        generateBenchVersions->setChecked(true);
        generateBenchVersions->setEnabled(false);
    }
    else {
        generateBenchVersions->setChecked(false);
        generateBenchVersions->setEnabled(true);
    }

    QCheckBox *timeInMSCB = new QCheckBox(exportLTRADialog);
    timeInMSCB->setCheckable(true);
    QString timingPrecision = "false";

    QToolButton *okTB = new QToolButton(exportLTRADialog);
    okTB->setIcon(QIcon(QPixmap(":/images/forms/img/accept_svn.png")));
    okTB->setAutoRaise(true);
    okTB->setFocusPolicy(Qt::StrongFocus);

    QToolButton *cancelTB = new QToolButton(exportLTRADialog);
    cancelTB->setIcon(QIcon(QPixmap(":/images/forms/img/cancel_svn.png")));
    cancelTB->setAutoRaise(true);

    connect(okTB,&QToolButton::clicked,exportLTRADialog,&QDialog::hide);
    connect(cancelTB,&QToolButton::clicked,docFormat,&QComboBox::clear);
    connect(cancelTB,&QToolButton::clicked,exportLTRADialog,&QDialog::hide);

    gridLayout->addWidget(infoGenerateReport,0,0,1,2);
    gridLayout->addWidget(docFormat,0,2,1,2);
    gridLayout->addWidget(generateBenchVersions, 2, 2, 1, 2);
    gridLayout->addWidget(timeInMSCB,2,0,1,1);
    gridLayout->addWidget(infoTimingFormat,2,1,1,1);
    gridLayout->addWidget(okTB, 4,2,1,1);
    gridLayout->addWidget(cancelTB,4,3,1,1);

    if (cliMode)
    {
        timingPrecision = "false";
        saveFormat = iSaveFormat;
    }
    else
    {
        exportLTRADialog->exec();
        timingPrecision = timeInMSCB->isChecked() ? "true" : "false";
        saveFormat = docFormat->currentText();
    }

    //Launch Parser version if is activate
    if (generateBenchVersions->isChecked()) {
        _pAppController->launchScriptBenchVersionsFile();
    }

    //clean the dynamic ui
    delete exportLTRADialog;
    exportLTRADialog = nullptr;

    if (saveFormat.isEmpty())
    {
        rc = false;
        return rc;
    }

    ExportLTRAParams exportLTRAParam;
    exportLTRAParam.saveFormat = saveFormat;
    exportLTRAParam.saveInMilliseconds = timingPrecision;
    exportLTRAParam.ioLogInstanceIDMap = _ioLogInstanceIDMap;


    QString  docFilenamePath = QString("%1"+QString(QDir::separator())+"%2").arg(_pAppController->getGTADataDirectory(),ui->DocFileLE->text());
    QFileInfo  info(QDir::cleanPath(docFilenamePath));
    QString defaultFileName  = info.completeBaseName();
    QString reportFile;

    if (cliMode)
    {
        QString extension;
        if (exportLTRAParam.saveFormat == "DOCX") extension = ".docx";
        else if (exportLTRAParam.saveFormat == "DOCX + XML") extension = ".docx";
        else if (exportLTRAParam.saveFormat == "XML") extension = ".xml";

        QString outputDir = "output";
        if(!QDir(outputDir).exists() && _LtraReportDocxSavePath.isEmpty()) QDir().mkdir(outputDir);
        reportFile = _LtraReportDocxSavePath.isEmpty() ? 
            QDir(outputDir).absolutePath() + "/" + defaultFileName + extension : _LtraReportDocxSavePath;
    }
    else
    {
        if (exportLTRAParam.saveFormat == "DOCX")
            reportFile = QFileDialog::getSaveFileName(this, "Save LTRA", defaultFileName, "Word Document (*.docx);; HTML File (*.html)");
         if (exportLTRAParam.saveFormat == "DOCX + XML")
            reportFile = QFileDialog::getSaveFileName(this, "Save LTRA", defaultFileName, "Word Document (*.docx);; HTML File (*.html)");
         if (exportLTRAParam.saveFormat == "XML")
            reportFile = QFileDialog::getSaveFileName(this, "Save LTRA", defaultFileName, "XML File (*.xml);; HTML File (*.html)");
    }
    

    QFileInfo reportFileInfo(reportFile);
    QFile fileReport;
    
    if (reportFileInfo.exists())
        fileReport.remove(reportFile);

    if(! reportFile.isEmpty() && NULL != _pFilterModel)
    {
        QString msg = "LTRA Export in progress .. please Wait";
        if (cliMode) cout(" *info: " + msg);
        else showProgressBar(msg);

        bool isLTR = ui->LogFileLE->text().isEmpty();
        exportLTRAParam.isLTRA = isLTR;

        bool isSingleLog = true;
        QString docFile = ui->DocFileLE->text();
        QString logFile = ui->LogFileLE->text();
        if(docFile.endsWith(".seq"))
        {
            rc = _pAppController->isSequenceWithMultipleLogs(logFile);
            if(rc)
            {
                isSingleLog = false;
                QStringList procs;
                QStringList logs;
                QStringList status;
                QString globalStatus;
                LogModelSequenceIO model;
                rc = _pAppController->readSequenceMultipleLogFile(logFile,logs,procs,status,globalStatus,model);
                model.SeqDocFile = docFile;
                model.SeqLogFile = logFile;

                if (rc)
                {
                    QFutureWatcher<bool> * FutureWatcher = new QFutureWatcher<bool>();
                    connect(FutureWatcher,SIGNAL(finished()),this,SLOT(onExportLTRAFinished()));
                    QFuture<bool> Future;
                    Future = QtConcurrent::run(_pAppController,&GTAAppController::exportLTRAToDocxForMultipleLogs,_pFilterModel->sourceModel(),reportFile,attachmentList,model,exportLTRAParam);
                    FutureWatcher->setFuture(Future);
                }
                else
                {
                    if (cliMode) { cout(" *error: error with Multiple Logs"); cout(_pAppController->getLastError()); }
                    else QMessageBox::critical(this,"Error with Multiple Logs",_pAppController->getLastError());
                }
            }
        }

        _exportedLTRAPath = reportFile;
        if (isSingleLog && !cliMode)
        {
            QFutureWatcher<bool> * FutureWatcher = new QFutureWatcher<bool>();
            connect(FutureWatcher,SIGNAL(finished()),this,SLOT(onExportLTRAFinished()));
            QFuture<bool> Future;
            if(reportFile.endsWith(".docx") || reportFile.endsWith(".xml"))
                Future = QtConcurrent::run(_pAppController,&GTAAppController::exportLTRAToDocx,_pFilterModel->sourceModel(),reportFile,attachmentList,exportLTRAParam);
            else if(reportFile.endsWith(".html"))
                Future = QtConcurrent::run(_pAppController,&GTAAppController::exportLTRAToHTML,_pFilterModel->sourceModel(),reportFile,attachmentList,0,isLTR);
            else
                return rc;
            FutureWatcher->setFuture(Future);
        }
        else if (isSingleLog && cliMode)
        {
            if (reportFile.endsWith(".docx") || reportFile.endsWith(".xml"))
            {
                std::shared_ptr<bool> ltraExportOutput = std::make_shared<bool>(
                    _pAppController->exportLTRAToDocx(
                        _pFilterModel->sourceModel(),
                        reportFile,
                        attachmentList,
                        exportLTRAParam)
                );
                onExportLTRAFinished(ltraExportOutput);
                rc = *ltraExportOutput;
            }            
            else
                return rc;
        }
        qDebug()<<_exportedLTRAPath;
    }
    return rc;
}
void GTALogFrame::onExportLTRAFinished(std::shared_ptr<bool> output)
{
    if(_pProgressBar != NULL)
        _pProgressBar->setVisible(false);

    QFutureWatcher<bool> *pWatcher = dynamic_cast<QFutureWatcher<bool> *>(sender());
    if(pWatcher == NULL && output == nullptr)
    {
        cout(" *error: export LTRA Failed due to unknown reason.");
        return;
    }
    else
    {
        if ((pWatcher != NULL && !pWatcher->result()) || (output != nullptr && !*output))
        {
            if (_cliMode)
                cout(" *error: export LTRA failed" + _pAppController->getLastError());
            else
                QMessageBox::warning(this, "Export LTRA Failed", _pAppController->getLastError());
        }
        else
        {
            if (_cliMode)
                cout(" *info: export LTRA finished, \n\tPath to the file: " + _exportedLTRAPath);
            else
            {
                QMessageBox msgBox (this);
                msgBox.setText(QString("Export LTRA Finished, \n Open LTRA (%1) ?").arg(_exportedLTRAPath));
                msgBox.setIcon(QMessageBox::Information);
                msgBox.setWindowTitle("Export");
                msgBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
                msgBox.setDefaultButton(QMessageBox::No);
                int ret = msgBox.exec();
                if(ret==QMessageBox::Yes)
                {
				    bool notOpen = QDesktopServices::openUrl(QUrl(_exportedLTRAPath.prepend("file:///"), QUrl::TolerantMode));
				    if (!notOpen)
					    QDesktopServices::openUrl(QUrl::fromLocalFile(_exportedLTRAPath));
                }
            }
        }
        pWatcher->deleteLater();
    }
}

/**
 * @brief SLOT to enable/disable the Analyze button
 */
void GTALogFrame::enableAnalyseButton(QString)
{
    if (bool docFileIsEmpty = ui->DocFileLE->text().isEmpty(); docFileIsEmpty && ui->LogFileLE->text().isEmpty())
    {
        ui->AnalyzeLogPB->setEnabled(false);
    }
    else
    {        
        ui->AnalyzeLogPB->setDisabled(docFileIsEmpty);        
    }
}
void GTALogFrame::clear()
{
    ui->LogFileLE->clear();
    ui->DocFileLE->clear();
    _pLogAttachWdgt->clear();
    ui->SearchFilterLE->clear();
    ui->ColumnFilterCB->clear();

    ui->LogViewTV->setModel(nullptr);
    QAbstractItemModel* loadedModel = _pFilterModel->sourceModel();
    _pFilterModel->setSourceModel(nullptr);
    if (nullptr != loadedModel)
    {
        delete loadedModel;
        loadedModel = nullptr;
    }

    //DEV: check all pointer member functions have ni memory leak
    if (NULL!=_pLastAnalyzedElement)
    {
        delete _pLastAnalyzedElement;
        _pLastAnalyzedElement = NULL;
    }
    if (NULL!=_pValidationModel)
    {
        delete _pValidationModel;
        _pValidationModel = NULL;
    }
}

void GTALogFrame::setCliMode(bool cliMode)
{
    _cliMode = cliMode;
}

void GTALogFrame::setLog(const QString &iLogFilePath)
{
    if(!iLogFilePath.isEmpty())
    {
        ui->LogFileLE->setText(iLogFilePath);
    }
}

QString GTALogFrame::getLastElementAnalyzed()const
{
    if (NULL != _pLastAnalyzedElement)
        return _pLastAnalyzedElement->getDocumentName();
    return QString();
}

QString GTALogFrame::getLastElementAnalyzedPath()const
{
    QString gtaDataDir = _pAppController->getSystemService()->getDataDirectory();
    QString docFile = ui->DocFileLE->text().remove(gtaDataDir);
    if (NULL != _pLastAnalyzedElement)
        return docFile;
    return QString();
}

void GTALogFrame::setElementRelativePath(const QString &iElemRelativePath)
{
    if(!iElemRelativePath.isEmpty())
    {
        ui->DocFileLE->setText(iElemRelativePath);
    }
}

void GTALogFrame::setElementType(const QString& iElementType)
{
    if (iElementType == "Sequence")
        ui->ElementTypeCB->setCurrentIndex(0);
    else if (iElementType == "Procedure")
        ui->ElementTypeCB->setCurrentIndex(1);
}

void GTALogFrame::analyzeResults()
{
    onAnalyzePBClicked();
}

void GTALogFrame::onCollapsed(QModelIndex index)
{
    QModelIndex idx = _pFilterModel->mapToSource(index);
    _pAppController->setLTRACollapsedState(true,_pFilterModel->sourceModel(),idx);
}

void GTALogFrame::onExpanded(QModelIndex index)
{
    QModelIndex idx = _pFilterModel->mapToSource(index);
    _pAppController->setLTRACollapsedState(false,_pFilterModel->sourceModel(),idx);
}

void GTALogFrame::setAllComandsExpanded(bool)
{
    _pAppController->setLTRACollapsedState(false,_pFilterModel->sourceModel());
}
void GTALogFrame::setAllComandsCollapsed(bool)
{
    _pAppController->setLTRACollapsedState(true,_pFilterModel->sourceModel());
}

void GTALogFrame::selectAllCommands()
{
    _pAppController->checkUncheckAllCommands(true,_pFilterModel->sourceModel());
    ui->LogViewTV->reset();
}

void GTALogFrame::deselectAllCommands()
{
    _pAppController->checkUncheckAllCommands(false,_pFilterModel->sourceModel());
    ui->LogViewTV->reset();
}

void GTALogFrame::onGetLastExecution()
{
    QString pathForLog = QDir::cleanPath(_pAppController->getLastEmoLogPath());
    QString pathForElement = QDir::cleanPath(_pAppController->getLastEmoElementPath());
    QString pathLogDir = QDir::cleanPath(_pAppController->getLogDirectory());
    QString dataDir = _pAppController->getGTADataDirectory();

    if(pathForLog.isEmpty())
    {
        QMessageBox::information(this,"Last Execution Status","Nothing executed");
        return;
    }
    
    QFileInfo logFile(pathForLog);
    QFileInfo elemntFile(pathForElement);
    
    if (!logFile.exists() && !elemntFile.exists())
        QMessageBox::information(this,"File not found",QString("file:[%1] and [%2] does not exist").arg(pathForLog,pathForElement));
    else if (!elemntFile.exists())
        QMessageBox::information(this,"File not found",QString("file:%1, does not exist").arg(pathForElement ));
    else if (!logFile.exists())
        QMessageBox::information(this,"File not found",QString("file:%1, does not exist").arg(pathForLog));

    QString elemFileName = elemntFile.fileName();
    QString elemName = elemFileName;

    elemName.truncate(elemFileName.size() - 4); // to truncate .pro /.seq / .obj / .fun from the end of the filename

    QString logFileName = logFile.fileName();
    logFileName.truncate(logFileName.size() - 4);    // to truncate .log from the end of the filename

    if (elemntFile.exists())
    {
        pathForElement.remove(dataDir);
        ui->DocFileLE->setText(pathForElement);

    }

    if(elemFileName.toLower().endsWith("seq"))
    {
        int idxSeq = ui->ElementTypeCB->findText(ELEM_SEQ);
        ui->ElementTypeCB->setCurrentIndex(idxSeq);
	
    }
    else if(elemFileName.toLower().endsWith("pro"))
    {
        int idxSeq = ui->ElementTypeCB->findText(ELEM_PROC);
        ui->ElementTypeCB->setCurrentIndex(idxSeq);

    }


    if (logFile.exists() && (elemName == logFileName))
    {
        ui->LogFileLE->setText(pathForLog);
    }
    else
    {
        QDir dir(pathLogDir);
        if(dir.count())
        {
            dir.setFilter(QDir::Files | QDir::Modified);
            dir.setSorting(QDir::Time);

            QFileInfoList lstLogsInDir = dir.entryInfoList();
            if(lstLogsInDir.count())
            {
                QFileInfo logFileModified = lstLogsInDir.at(0);

                QString logFileLastModifiedName = logFileModified.fileName();
                logFileLastModifiedName = logFileLastModifiedName;
                pathLogDir = pathLogDir + "/" + logFileLastModifiedName;

                ui->LogFileLE->setText(pathLogDir);
            }
        }
    }
}

void GTALogFrame::showProgressBar(const QString &iMsg)
{
    if(_pProgressBar!=NULL)
    {
        _pProgressBar->setVisible(true);
        _pProgressBar->setPosition(this->geometry());
        _pProgressBar->setWindowModality(Qt::ApplicationModal);
        _pProgressBar->setLabelText(iMsg);
        //        _pProgressBar->setLabelText();
    }
}

void GTALogFrame::hideProgressBar()
{
    if(_pProgressBar != NULL)
    {
        _pProgressBar->setVisible(false);
    }
}

void GTALogFrame::disableLogGroupBox()
{
    ui->logResultInputGB->setDisabled(true);
}

/**
 * This function sets the title of result as relative path after the result analysis generated
 * @return: void
*/
void GTALogFrame::setLogFrameWindowTitle()
{
    QString sFileName("");
    sFileName = ui->DocFileLE->text();
    setWindowTitle(sFileName);
}

void GTALogFrame::onSetFocus()
{
    this->QWidget::setFocus();
}

