#include "GTANewFileCreationDialog.h"
#include "ui_GTANewFileCreationDialog.h"
#include <QFileDialog>
#include <QMessageBox>

GTANewFileCreationDialog::GTANewFileCreationDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::GTANewFileCreationDialog)
{
    ui->setupUi(this);
    _GtaDataDir = QString("");
    ui->FilePathLE->setText(_GtaDataDir);
    ui->FileNameLE->clear();
    ui->FileTypeCB->setCurrentIndex(0);
    ui->AuthorLE->clear(); //V26 editor view column enhancement #322480
    connect(ui->BrowserTB, SIGNAL(clicked()), this, SLOT(onBrowserTBClicked()));
    connect(ui->OKPB, SIGNAL(clicked()), this, SLOT(onOkPBClicked()));
    connect(ui->CancelPB, SIGNAL(clicked()), this, SLOT(hide()));
    _isFileValid = true;
    setWindowModality(Qt::WindowModal);
    _pElement = NULL;
    ui->MaxTimeEstimatedLB->setText(ui->MaxTimeEstimatedLB->text() + " (" + XNODE_MAX_TIME_ESTIMATED_UNIT + ")");
}

GTANewFileCreationDialog::~GTANewFileCreationDialog()
{
    delete ui;
}

void GTANewFileCreationDialog::onBrowserTBClicked()
{
    bool valid = false;
    QString saveDirPath;
    do
    {
        saveDirPath = QFileDialog::getExistingDirectory(this, "Save", _GtaDataDir, QFileDialog::ShowDirsOnly);
        saveDirPath = QDir::cleanPath(saveDirPath);
        if (saveDirPath.isEmpty())
            return;
        if (!saveDirPath.contains(GTAAppController::getGTAAppController()->getGTADataDirectory()) && (_DialogMode != DialogMode::SAVE_AS_DOCX))
        {

            valid = false;
            QString message = QString("Please save under the files under %1 path").arg(_GtaDataDir);
            QMessageBox msgBox(QMessageBox::Warning, "Incorrect Save Path", message, QMessageBox::Ok, this);
            msgBox.exec();
        }
        else
        {
            valid = true;
        }

    } while (valid != true);

    ui->FilePathLE->setText(saveDirPath);
}

void GTANewFileCreationDialog::onOkPBClicked()
{
    emit disconnect_file_watcher();

    QString fileName = ui->FileNameLE->text();
    QString filePath = ui->FilePathLE->text();
    QString fileType = ui->FileTypeCB->currentText();
    QString absPath = QDir::cleanPath(QString("%1/%2%3").arg(filePath, fileName, fileType));
    QString authorName = ui->AuthorLE->text(); //V26 editor view column enhancement #322480
    QFile file(absPath);
    bool rc = false;
    if (_DialogMode != SAVE_AS_DOCX)
    {
        rc = true;
        if (!_isConsole) {
            if (file.exists())
            {
                int ret = QMessageBox::warning(this, "Save warning", "File already exists. Do you want to over write?", QMessageBox::Yes, QMessageBox::No);
                if (ret == QMessageBox::Yes)
                {
                    rc = true;
                }
                else
                {
                    _isFileValid = false;
                    rc = false;
                }
            }
        }
        else {
            if (file.exists()) {
                _forceOverwrite ? rc = true : rc = false;
            }
        }

    }
    else
    {
        if (!_isConsole) {
            if (!file.exists()) {
                rc = true;
            }
            else {
                int ret = QMessageBox::warning(this, "Save warning", "File already exists. Do you want to over write?", QMessageBox::Yes, QMessageBox::No);
                if (ret == QMessageBox::Yes)
                {
                    rc = true;
                }
            }
        }
        else {
            _forceOverwrite ? rc = true : rc = false;
        }
    }

    if (rc) {
        _isFileValid = true;
        if (fileName.isEmpty()) {
            this->show();
            QMessageBox::critical(this, "Error", "File Name cannot be empty");
            rc = false;
        }
        else if (filePath.isEmpty()) {
            this->show();
            QMessageBox::critical(this, "Error", "File Path cannot be empty");
            rc = false;
        }
        // V26 editor view column enhancement #322480 ------>
        else if ((authorName.isEmpty()) && (_DialogMode != SAVE_AS_DOCX)) {
            this->show();
            QMessageBox::critical(this, "Error", "Author name cannot be empty");
            rc = false;
        }
        // <-------V26 editor view column enhancement #322480

        if (rc)// V26 editor view column enhancement #322480
        {
            hide();

            switch (_DialogMode)
            {
            case NEWFILE:
                emit newFileAccepted();
                break;

            case SAVEAS:
            {
                bool isValid = isSaveAsValid();
                if (isValid)
                    emit saveAsAccepted();
                else
                {
                    QString type = ui->FileTypeCB->currentText();
                    if (type.startsWith("."))
                    {
                        type = type.replace(".", "");
                    }

                    QString msg = QString("The current file cannot be converted to %1").arg(type);
                    QMessageBox::warning(this, "Save as Error", msg, QMessageBox::Ok);
                }
                break;
            }
            case SEQUENCE:
                emit sequenceCreated();
                break;

            case SAVE_AS_DOCX:
                emit saveAsDocxAccepted();
                break;

            case NEW_CSV2PRO_FILE:
                emit newCsvToProConvertedFileAccepted();
                break;
            }
        }
    }
    emit restore_file_watcher();
}


bool GTANewFileCreationDialog::isSaveAsValid()
{
    bool isValid = true;
    QString changedExtension = ui->FileTypeCB->currentText();

    if (_currExtension != changedExtension)
    {
        GTAAppController* pAppCtrl = GTAAppController::getGTAAppController();
        if (pAppCtrl)
        {
            isValid = pAppCtrl->isSaveAsValid(_pElement, changedExtension);
        }
    }
    return isValid;
}

bool GTANewFileCreationDialog::isFileValid()
{
    return _isFileValid;
}

void GTANewFileCreationDialog::setDataDirPath(const QString& iPath)
{
    _GtaDataDir = iPath;
}

QString GTANewFileCreationDialog::getFileType()
{
    return ui->FileTypeCB->currentText();
}

QString GTANewFileCreationDialog::getFileName()
{
    return ui->FileNameLE->text();
}

QString GTANewFileCreationDialog::getFilePath()
{
    return ui->FilePathLE->text();
}

QString GTANewFileCreationDialog::getAuthorName()
{
    return ui->AuthorLE->text();
}

/**
 * @brief Get the Maximum Time Estimated value
 * @return QString
*/
QString GTANewFileCreationDialog::getMaxTimeEstimated()
{
    return ui->MaxTimeEstimatedLE->text();
}

void GTANewFileCreationDialog::setFileTypes(const QStringList& iFileTypesList)
{
    ui->FileTypeCB->clear();
    if(iFileTypesList.isEmpty())
    {
        QStringList types;
        types << ".pro" << ".obj" << ".fun" << ".seq";
        ui->FileTypeCB->addItems(types);
    }
    else
    {
        ui->FileTypeCB->addItems(iFileTypesList);
    }
}

void GTANewFileCreationDialog::setFileType(const QString &iFileType)
{
    int index = ui->FileTypeCB->findText(iFileType);
    if (index > 0 && index < ui->FileTypeCB->count())
    {
        ui->FileTypeCB->setCurrentIndex(index);
    }
    else
        ui->FileTypeCB->setCurrentIndex(0);

    _currExtension = ui->FileTypeCB->currentText();
}

void GTANewFileCreationDialog::setFileName(const QString& iFileName)
{
    ui->FileNameLE->setText(iFileName);
}


void GTANewFileCreationDialog::setAuthorName(const QString& iAuthorName)
{
    ui->AuthorLE->setText(iAuthorName);
}

/**
 * @brief Set the Maximum Time Estimated value
 * @param iMaxTimeEstimated 
*/
void GTANewFileCreationDialog::setMaxTimeEstimated(const QString& iMaxTimeEstimated)
{
    ui->MaxTimeEstimatedLE->setText(iMaxTimeEstimated);
}


void GTANewFileCreationDialog::setFilePath(const QString& iFilePath)
{
    if (iFilePath.isEmpty())
        ui->FilePathLE->setText(_GtaDataDir);
    else
        ui->FilePathLE->setText(iFilePath);
}


void GTANewFileCreationDialog::clickOk() 
{
    onOkPBClicked();
}


void GTANewFileCreationDialog::setWindowTitle(const QString &iTitle)
{
    this->QDialog::setWindowTitle(iTitle);
}


void GTANewFileCreationDialog::setMode(DialogMode mode)
{
    _DialogMode = mode;
    //when we are exporting editor window contents as docx during a print
    if (_DialogMode == GTANewFileCreationDialog::SAVE_AS_DOCX)
    {
        ui->AuthorLB->hide();
        ui->AuthorLE->hide();
    }
    else
    {
        ui->AuthorLB->show();
        ui->AuthorLE->show();
    }
}

void GTANewFileCreationDialog::show()
{

    _GtaDataDir = QDir::cleanPath(_GtaDataDir);
    ui->FilePathLE->setText(_GtaDataDir);


    ui->FileTypeCB->clear();
    QStringList types;
    types << ".pro" << ".obj" << ".fun" << ".seq";
    ui->FileTypeCB->addItems(types);


    if (_DialogMode == NEWFILE)
    {
        ui->FileNameLE->clear();
        ui->AuthorLE->clear();
    }
    else
        if (_DialogMode == SEQUENCE)
        {
            ui->FileNameLE->clear();
            ui->FileTypeCB->clear();
            ui->AuthorLE->clear();
            types.clear();
            types << ".pro" << ".seq";
            ui->FileTypeCB->addItems(types);
        }
        else if (_DialogMode == SAVE_AS_DOCX)
        {
            ui->FileTypeCB->clear();
            types.clear();
            types << ".docx";
            ui->FileTypeCB->addItems(types);

        }

    this->QDialog::show();
}

void GTANewFileCreationDialog::setElement(GTAElement* iElement)
{
    _pElement = NULL;
    if (iElement != NULL)
    {
        _pElement = iElement;

    }
}

GTAElement* GTANewFileCreationDialog::getElement()const
{
    return _pElement;
}
