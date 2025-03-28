#include "GTACompatibilityReport.h"
#include "GTATestConfig.h"
#include "ui_GTACompatibilityReport.h"
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QFile>
#include <qdatetime.h>
#include <QKeySequence>
#include <QShortcut>
#include "QDebug"
#include <QDesktopServices>
#include <QRegularExpression>

#define REPORT_TABLE_COL_CNT 6

#define REPORT_TABLE_TYPE_COL 0
#define REPORT_TABLE_SRC_COL 1
#define REPORT_TABLE_FILE_COL 2
#define REPORT_TABLE_LINE_COL 3
#define REPORT_TABLE_DESC_COL 4
#define REPORT_TABLE_UUID_COL 5

#define STR_CHECK_COMPATIBILITY "Check Compatibility"
#define STR_FILE "File open"
#define STR_DB "Database"
#define STR_PLUGIN_IMPORT "Plugin import"
#define STR_EXPORT_DOC "Export document"
#define STR_SVN "SVN"
#define STR_RESULT_ANALYSIS "Result Analysis"
#define STR_PORTING "Porting to V23 & above"



GTACompatibilityReport::GTACompatibilityReport(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::GTACompatibilityReport)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Dialog|Qt::WindowMaximizeButtonHint|Qt::WindowMinimizeButtonHint);
    setWindowTitle("Compatibility Report");

    connect(ui->ReportTW,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(onLogTableClicked(QModelIndex)));
    QShortcut *clearShortCut = new QShortcut(QKeySequence(Qt::SHIFT + Qt::CTRL + Qt::Key_Delete),this);
    clearShortCut->setContext(Qt::WidgetWithChildrenShortcut);
    connect(clearShortCut, SIGNAL(activated()),this, SLOT(onClearLog()));

    QShortcut *saveLogShortCut = new QShortcut(QKeySequence(Qt::SHIFT + Qt::CTRL + Qt::Key_D),this);
    saveLogShortCut->setContext(Qt::WidgetWithChildrenShortcut);
    connect(saveLogShortCut,SIGNAL(activated()),this,SLOT(onSave()));

    connect(ui->ReportTW,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(displayContextMenu(QPoint)));
    connect(ui->logFilterCB, SIGNAL(currentIndexChanged(int)), this, SLOT(onLogFilterChanged()));
    connect(ui->searchTB,SIGNAL(clicked()),this,SLOT(onSearchTBClicked()));
    connect(ui->searchLE,SIGNAL(editingFinished()),this,SLOT(onSearchTBClicked()));

    ui->logFilterCB->setCurrentIndex(3);
    ui->searchTB->setAutoRaise(true);
    ui->searchTypeCB->view()->window()->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    _pContextMenu = NULL;
//    _pActionResolve = NULL;
//    _pActionResolveAll = NULL;
    _pActionSaveLog = NULL;
    _pActionClearLog = NULL;
    createContextMenu();
}

GTACompatibilityReport::~GTACompatibilityReport()
{
    if(_pContextMenu != NULL)
    {
        delete _pContextMenu;
        _pContextMenu = NULL;
    }
    delete ui;

}

void GTACompatibilityReport::closeEvent(QCloseEvent *event)
{
    onClearLog();
    event->accept();
    this->QWidget::closeEvent(event);
}

void GTACompatibilityReport::onResolveClicked()
{
    QModelIndex iIndex = ui->ReportTW->selectionModel()->selectedIndexes().at(0);
    if(iIndex.isValid())
    {
        int row = iIndex.row();

        QTableWidgetItem *pSrcItem = ui->ReportTW->item(row,REPORT_TABLE_SRC_COL);
        QString src = pSrcItem->text();

        if(src == STR_CHECK_COMPATIBILITY)
        {

            QTableWidgetItem *pFileItem = ui->ReportTW->item(row,REPORT_TABLE_FILE_COL);
            QString elementRelativePath = pFileItem->text();

            QTableWidgetItem *pLineItem = ui->ReportTW->item(row,REPORT_TABLE_LINE_COL);
            QString elementLineNumber = pLineItem->text();
            bool ok;
            int lineNumber = elementLineNumber.toInt(&ok);
            if(!ok)
                lineNumber = -1;

        }

    }
}

void GTACompatibilityReport::onResolveAllClicked()
{

}

void GTACompatibilityReport::setLogLevel(const uint& pos)
{
    ui->logFilterCB->setCurrentIndex(pos);
}
/**
  * This function provides Filter/search list on log window based on column type
  * @return: void, this function calls to setLog with search List
  */
void GTACompatibilityReport::onSearchTBClicked()
{
    ErrorMessageList logList;
    for(int i = 0; i < _Logs.count(); i++)
    {
        ErrorMessage error = _Logs.at(i);
        QString textToSearch = ui->searchLE->text();
        QString filterCriteria = ui->searchTypeCB->currentText();
        QStringList parts = textToSearch.split("|");
        QRegularExpression re("^\\d+$");
        QRegularExpressionMatch match = re.match(parts[0]);

        if (match.hasMatch() && (filterCriteria == QString("Include Line") || filterCriteria == QString("Exclude Line")))
        {
            for (int i = 0; i < parts.size(); ++i)
            {
                parts[i] = "^" + parts[i] + "$";
            }

            if (parts.size() > 1)
            {
                textToSearch = parts.join("|");
            }

            else
            {
                textToSearch = parts[0];
            }

        }

        QRegularExpression regex(textToSearch, QRegularExpression::CaseInsensitiveOption);

        if(filterCriteria == QString("Type"))
        {
            QString errType = getErrorType(error.errorType);
            if(errType.contains(textToSearch, Qt::CaseInsensitive))
            {
                logList.append(error);
            }
        }
        else if(filterCriteria == QString("Source"))
        {
            QString srcType = getSourceType(error.source);
            if(srcType.contains(textToSearch, Qt::CaseInsensitive))
            {
                logList.append(error);
            }
        }
        else if ((filterCriteria == QString("Include Line") && error.lineNumber.contains(regex)) ||
            (filterCriteria == QString("Exclude Line") && !error.lineNumber.contains(regex)))
        {
            logList.append(error);
        }

        else if ((filterCriteria == QString("Description(contains)") && error.description.contains(regex)) ||
            (filterCriteria == QString("Description(not contains)") && !error.description.contains(regex)))
        {
            logList.append(error);
        }

    }
    setLog(logList, true);
}

void GTACompatibilityReport::setLog(const ErrorMessageList &iErrorList, bool overwriteExistingLogs)
{
    if(overwriteExistingLogs)
    {
		ui->ReportTW->clearContents();
        ui->ReportTW->setRowCount(0);
    }
    else
    {
        ui->searchLE->clear();
        onSearchTBClicked();
        _Logs.append(iErrorList);   
    }

    int rowCnt = ui->ReportTW->rowCount();
    ui->ReportTW->setRowCount(rowCnt + iErrorList.count());
    ui->ReportTW->setColumnCount(REPORT_TABLE_COL_CNT);
    if(!iErrorList.isEmpty())
    {
        for(int i = rowCnt, j =  0; j < iErrorList.count(); i++,j++)
        {
            ErrorMessage error = iErrorList.at(j);
            QString description = error.description;
            ErrorMessage::ErrorType eType = error.errorType;
            QString type = getErrorType(eType);
            QString file = error.fileName;
            QString lineNum = error.lineNumber;
            ErrorMessage::MsgSource src = error.source;
            QString source = getSourceType(src);
            QString uuid = error.uuid;

            if (type == _logLevel)
            {
                QTableWidgetItem* pDescItem = new QTableWidgetItem(description);
                ui->ReportTW->setItem(i, REPORT_TABLE_DESC_COL, pDescItem);

                QTableWidgetItem* pTypeItem = new QTableWidgetItem(type);
                ui->ReportTW->setItem(i, REPORT_TABLE_TYPE_COL, pTypeItem);

                QTableWidgetItem* pFileItem = new QTableWidgetItem(file);
                ui->ReportTW->setItem(i, REPORT_TABLE_FILE_COL, pFileItem);

                QTableWidgetItem* pLineItem = new QTableWidgetItem(lineNum);
                ui->ReportTW->setItem(i, REPORT_TABLE_LINE_COL, pLineItem);

                QTableWidgetItem* pSrcItem = new QTableWidgetItem(source);
                ui->ReportTW->setItem(i, REPORT_TABLE_SRC_COL, pSrcItem);

                QTableWidgetItem* pUUIDItem = new QTableWidgetItem(uuid);
                ui->ReportTW->setItem(i, REPORT_TABLE_UUID_COL, pUUIDItem);
            }
            else if (_logLevel == "All")
            {
                QTableWidgetItem* pDescItem = new QTableWidgetItem(description);
                ui->ReportTW->setItem(i, REPORT_TABLE_DESC_COL, pDescItem);

                QTableWidgetItem* pTypeItem = new QTableWidgetItem(type);
                ui->ReportTW->setItem(i, REPORT_TABLE_TYPE_COL, pTypeItem);

                QTableWidgetItem* pFileItem = new QTableWidgetItem(file);
                ui->ReportTW->setItem(i, REPORT_TABLE_FILE_COL, pFileItem);

                QTableWidgetItem* pLineItem = new QTableWidgetItem(lineNum);
                ui->ReportTW->setItem(i, REPORT_TABLE_LINE_COL, pLineItem);

                QTableWidgetItem* pSrcItem = new QTableWidgetItem(source);
                ui->ReportTW->setItem(i, REPORT_TABLE_SRC_COL, pSrcItem);

                QTableWidgetItem* pUUIDItem = new QTableWidgetItem(uuid);
                ui->ReportTW->setItem(i, REPORT_TABLE_UUID_COL, pUUIDItem);
            }
            else
            {
                ui->ReportTW->removeRow(i);
                i--;
            }
        }
         ui->ReportTW->hideColumn(REPORT_TABLE_UUID_COL);
    }
}


QString GTACompatibilityReport::getLogList()const
{
    QStringList logList;
    QString logs;
    for (const auto& log : _Logs)
    {
        QString msg = QString("%1 : %2 in %3 at line(%4)").arg(getErrorType(log.errorType), log.description, log.fileName, log.lineNumber);
        logList.append(msg);
    }
    logs = logList.join("\n");

    return logs;
}

QString GTACompatibilityReport::getCsvToProLogList() const
{
    QStringList logList;
    QString logs;
    for (const auto& log : _Logs)
    {
        QString msg = QString("%1 #%2# %3").arg(getErrorType(log.errorType), log.lineNumber, log.description);
        logList.append(msg);
    }
    logs = logList.join("\n");

    return logs;
}

QString GTACompatibilityReport::getSourceType(const ErrorMessage::MsgSource iType)const
{
    QString sType;
    switch(iType)
    {

    case ErrorMessage::MsgSource::kCheckCompatibility :
        sType = QString(STR_CHECK_COMPATIBILITY);
        break;

    case ErrorMessage::MsgSource::kFile :
        sType = QString(STR_FILE);
        break;

    case ErrorMessage::MsgSource::kDatabase :
        sType = QString(STR_DB);
        break;

    case ErrorMessage::MsgSource::kSvn:
        sType = QString(STR_SVN);
        break;

    case ErrorMessage::MsgSource::kExportDoc:
        sType = QString(STR_EXPORT_DOC);
        break;

    case ErrorMessage::MsgSource::kPluginImport:
        sType = QString(STR_PLUGIN_IMPORT);
        break;

    case ErrorMessage::MsgSource::kResult:
        sType = QString(STR_RESULT_ANALYSIS);
        break;

    case ErrorMessage::MsgSource::kPorting:
        sType = QString(STR_PORTING);
        break;

    case ErrorMessage::MsgSource::kNoSrc:
        sType = QString();
        break;

    }
    return sType;
}

QString GTACompatibilityReport::getErrorType(const ErrorMessage::ErrorType type)const
{
    QString sType;
    switch(type)
    {
        case ErrorMessage::ErrorType::kError :
        sType = QString("Error");
        break;

        case ErrorMessage::ErrorType::kWarning :
        sType = QString("Warning");
        break;

        case ErrorMessage::ErrorType::kInfo :
        sType = QString("Info");
        break;

        case ErrorMessage::ErrorType::kNa :
        sType = QString();
        break;

    }
    return sType;
}


void GTACompatibilityReport::onSave()
{
    QDateTime dateTime2;
    QString ReportFile = QString(QString("%1")+ QString("_%2.log")).arg(QDir::currentPath(),dateTime2.currentDateTime().toString("dd.MM.yyyy.hh.mm.ss"));
    QString filePath = QDir::cleanPath(QFileDialog::getSaveFileName(this,tr("Save File"),QDir::cleanPath(ReportFile),"*.log"));

    if(! filePath.isEmpty())
    {
        QString ReportFilePath = QDir::cleanPath(QString("%1").arg(filePath));
        QFile file(ReportFilePath);
        file.open(QIODevice::WriteOnly);
        QTextStream out(&file);
        QString logList = getLogList();
        out << logList;
        file.close();
        QMessageBox::information(this,"Info","Log Saved!");
    }
}

void GTACompatibilityReport::onSaveLogCsvConversion(const QString proName, ErrorMessageList& errLogs, bool isConsole)
{
    QDateTime currentDateTime = QDateTime::currentDateTime().toUTC();
    QString formattedDateTime = currentDateTime.toString("dd_MM_yy_hhmmss");
    TestConfig* testConf = TestConfig::getInstance();
    QString ReportFile = QString("%1_%2_csvconversion.log").arg(formattedDateTime,proName);
    QString filePath = QString::fromStdString(testConf->getLogDirPath()) + "\\" + ReportFile;
    int ret = 0;
    if (!isConsole) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Conversion finished");
        if (errLogs.isEmpty())
        {
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setText(QString("Csv conversion is done. The log file is saved in:\n%1").arg(filePath));
        }
        else
        {
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setText(QString("Csv conversion is done, however there are errors. For more details go to:\n%1").arg(filePath));
        }
        msgBox.setStandardButtons(QMessageBox::Open | QMessageBox::Ok);
        msgBox.setButtonText(QMessageBox::Open, "Open Log");
        msgBox.setDefaultButton(QMessageBox::Ok);
        ret = msgBox.exec();
    }

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly))
    {
       QTextStream out(&file);
       QString logList = getCsvToProLogList();
       out << logList;
    }

    if (ret == QMessageBox::Open) 
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    }
}

void GTACompatibilityReport::setDialogName(const QString& iName)
{
    setWindowTitle(iName);
}

void GTACompatibilityReport::show()
{
//    onClearLog();
    ui->ReportTW->resizeRowsToContents();
    QWidget::show();
}
bool GTACompatibilityReport::getCancelStatus()
{
    return _isCancel ;
}

void GTACompatibilityReport::onLogTableClicked(QModelIndex iIndex)
{
    if(iIndex.isValid())
    {
        int row = iIndex.row();

        QTableWidgetItem *pSrcItem = ui->ReportTW->item(row,REPORT_TABLE_SRC_COL);
        QString src = pSrcItem->text();

        if(src == STR_CHECK_COMPATIBILITY || src == STR_PORTING)
        {

//            QTableWidgetItem *pFileItem = ui->ReportTW->item(row,REPORT_TABLE_FILE_COL);
//            QString elementRelativePath = pFileItem->text();

            QTableWidgetItem *pUUIDItem = ui->ReportTW->item(row,REPORT_TABLE_UUID_COL);
            QString uuid = pUUIDItem->text();

            QTableWidgetItem *pLineItem = ui->ReportTW->item(row,REPORT_TABLE_LINE_COL);
            QString elementLineNumber = pLineItem->text();
            bool ok;
            int lineNumber = elementLineNumber.toInt(&ok);
            if(!ok)
                lineNumber = -1;
            emit jumpToElement(uuid,lineNumber,GTAAppController::COMPATIBILITY_CHK,true);
        }

    }
}


void GTACompatibilityReport::setText(QString message, ErrorMessage::ErrorType errType,ErrorMessage::MsgSource msgType)
{

    ErrorMessageList ErrorList;
    ErrorMessage log;
    log.errorType = errType;
    log.source = msgType;
    log.description = message;
    log.lineNumber = QString();
    log.fileName  = QString();
    ErrorList.append(log);
    setLog(ErrorList);

}

void GTACompatibilityReport::onClearLog()
{
    _Logs.clear();
    ui->ReportTW->clearContents();
    ui->ReportTW->setRowCount(0);
}

void GTACompatibilityReport::onLogFilterChanged()
{
    ErrorMessageList logList;
    QString selectedLoglevel = ui->logFilterCB->currentText();
    _logLevel = selectedLoglevel;
    for (int i = 0; i < _Logs.count(); i++)
    {
        ErrorMessage msg = _Logs.at(i);
        QString filterCriteria = QString("Type");
        QString errType = getErrorType(msg.errorType);

        if (selectedLoglevel == "Error" && errType == "Error")
        {
            logList.append(msg);
        }
        else if (selectedLoglevel == "Warning" && errType == "Warning")
        {
            logList.append(msg);
        }
        else if (selectedLoglevel == "Info" && errType == "Info")
        {
            logList.append(msg);
        }
        else if (selectedLoglevel == "All")
        {
            logList.append(msg);
        }
    }
    setLog(logList, true);
}
void GTACompatibilityReport::displayContextMenu(const QPoint &iPos)
{
    QWidget * pSourceWidget = (QWidget*)sender();
    if(ui->ReportTW->rowCount())
    {
        if(pSourceWidget != NULL && _pContextMenu != NULL && pSourceWidget == ui->ReportTW)
        {
            QModelIndexList indexList = ui->ReportTW->selectionModel()->selectedIndexes();
            if(indexList.count() <= 0)
            {
//                _pActionResolve->setEnabled(false);
//                _pActionResolveAll->setEnabled(false);
                _pActionSaveLog->setEnabled(true);
                _pActionClearLog->setEnabled(true);
            }
            else
            {
//                _pActionResolve->setEnabled(false);
//                _pActionResolveAll->setEnabled(false);
                _pActionSaveLog->setEnabled(true);
                _pActionClearLog->setEnabled(true);
            }
            _pContextMenu->exec(ui->ReportTW->viewport()->mapToGlobal(iPos));
        }
    }
}

void GTACompatibilityReport::createContextMenu()
{
    _pContextMenu = new QMenu(this);
//    _pActionResolve = new QAction("Resolve",this);
//    _pContextMenu->addAction(_pActionResolve);

//    _pActionResolveAll = new QAction("Resolve all",this);
//    _pContextMenu->addAction(_pActionResolveAll);

//    _pContextMenu->addSeparator();

    _pActionSaveLog = new QAction("Save Log",this);
    _pContextMenu->addAction(_pActionSaveLog);

    _pActionClearLog = new QAction("Clear Log",this);
    _pContextMenu->addAction(_pActionClearLog);

//    connect(_pActionResolve,SIGNAL(triggered()),this,SLOT(onResolveClicked()));
//    connect(_pActionResolveAll,SIGNAL(triggered()),this,SLOT(onResolveAllClicked()));
    connect(_pActionSaveLog,SIGNAL(triggered()),this,SLOT(onSave()));
    connect(_pActionClearLog,SIGNAL(triggered()),this,SLOT(onClearLog()));
}

