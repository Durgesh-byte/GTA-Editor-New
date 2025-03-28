#include "GTALogResultTVRowEditor.h"
#include "ui_GTALogResultTVRowEditor.h"
#include <QDateTime>

GTALogResultTVRowEditor::GTALogResultTVRowEditor(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::GTALogResultTVRowEditor)
{
    ui->setupUi(this);
    ui->UserFeedBackPTE->setWordWrapMode(QTextOption::WordWrap);
    isOkPBClicked = false;
    ui->ActionLE->setDisabled(true);
    ui->CommentLE->setDisabled(true);
    ui->PrecisionLE->setDisabled(true);
    ui->TimeOutLE->setDisabled(true);
    ui->CurrentValueLE->setDisabled(true);
    ui->ExpectedValueLE->setDisabled(true);
    ui->ExecutionTimeLE->setDisabled(true);
    ui->DumpListLE->setDisabled(true);
    ui->ActionOnFailLE->setDisabled(true);
    connect(ui->ResultCB,SIGNAL(currentIndexChanged(QString)),this,SLOT(onResultCBValueChanged(QString)));
    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(onOKPBClicked()));
    connect(ui->buttonBox,SIGNAL(rejected()),this,SLOT(onCancelPBClicked()));
}

GTALogResultTVRowEditor::~GTALogResultTVRowEditor()
{
    delete ui;
}
void GTALogResultTVRowEditor::onOKPBClicked()
{
    isOkPBClicked = true;
    this->hide();
    emit rowDataChanged();
}
void GTALogResultTVRowEditor::onCancelPBClicked()
{
    isOkPBClicked = false;
    this->hide();
}
bool GTALogResultTVRowEditor::getChangeStatus() const
{
    return isOkPBClicked;
}
void GTALogResultTVRowEditor::setAction(const QString &iAction)
{
    ui->ActionLE->setText(iAction);
}
void GTALogResultTVRowEditor::setActionOnFail(const QString &iActionOnFail)
{
    ui->ActionOnFailLE->setText(iActionOnFail);
}

void GTALogResultTVRowEditor::setTimeOut(const QString &iTimeOut)
{
    ui->TimeOutLE->setText(iTimeOut);
}

void GTALogResultTVRowEditor::setPrecision(const QString &iPrecision)
{
    ui->PrecisionLE->setText(iPrecision);
}


void GTALogResultTVRowEditor::setComment(const QString &iComment)
{
    ui->CommentLE->setText(iComment);
}

void GTALogResultTVRowEditor::setResult(const QString &iResult)
{
    QStringList ResultStatus;
    ui->ResultCB->clear();
    ResultStatus << "OK" << "KO" << "NA";
    ui->ResultCB->addItems(ResultStatus);
    if(ResultStatus.contains(iResult))
    {
        int index = ResultStatus.indexOf(iResult);
        ui->ResultCB->setCurrentIndex(index);
    }

}
void GTALogResultTVRowEditor::onResultCBValueChanged(const QString &)
{
    ui->ExecutionTimeLE->setText(getSystemDateTime());
}

void GTALogResultTVRowEditor::setCurrentValue(const QString &iCurrentValue)
{
    ui->CurrentValueLE->setText(iCurrentValue);
}

void GTALogResultTVRowEditor::setExpectedValue(const QString &iExpectedValue)
{
    ui->ExpectedValueLE->setText(iExpectedValue);
}

void GTALogResultTVRowEditor::setDumpList(const QString &iDumpList)
{
    ui->DumpListLE->setText(iDumpList);
}

void GTALogResultTVRowEditor::setDefects(const QString &iDefects)
{
    ui->DefectsLE->setText(iDefects);
}

void GTALogResultTVRowEditor::setUserFeedBack(const QString &iFeedback)
{
    ui->UserFeedBackPTE->setPlainText(iFeedback);
}
void GTALogResultTVRowEditor::setExecutionTime(const QString &iExecTime)
{
    ui->ExecutionTimeLE->setText(iExecTime);
}

void GTALogResultTVRowEditor::setData(const QString &iData, const int &colNumber)
{
    QString data = iData.trimmed();
    if((data.trimmed().isEmpty()) && (colNumber != 10))
    {
        data = "NA";
    }
    else
    {
        if(colNumber == 11)
        {
            data.replace("[","");
            data.replace("]","");
            bool ok;
            const uint s = data.toUInt( &ok );
            if ( !ok ) {
                data = getSystemDateTime();
            }
            else
            {
                const QDateTime dt = QDateTime::fromTime_t( s );
                data = dt.toString( Qt::TextDate );
            }
        }
    }
    switch(colNumber)
    {
    case 0:
        ui->ActionLE->setText(data);
        break;

    case 1:
        ui->ActionOnFailLE->setText(data);
        break;

    case 2:
        ui->TimeOutLE->setText(data);
        break;

    case 3:
        ui->PrecisionLE->setText(data);
        break;

    case 4:
        ui->CommentLE->setText(data);
        break;

    case 5:
        setResult(data);
        break;

    case 6:
        ui->CurrentValueLE->setText(data);
        break;

    case 7:
        ui->ExpectedValueLE->setText(iData);
        break;

    case 8:
        ui->DumpListLE->setText(data);
        break;

    case 9:
        ui->DefectsLE->setText(data);
        break;

    case 10:
        ui->UserFeedBackPTE->setPlainText(data);
        break;
    case 11:
        ui->ExecutionTimeLE->setText(data);
        break;
    }
}

QString GTALogResultTVRowEditor::getExecutionTime() const
{
    return ui->ExecutionTimeLE->text();
}
QString GTALogResultTVRowEditor::getDefects() const
{
    return ui->DefectsLE->toPlainText();
}
QString GTALogResultTVRowEditor::getSystemDateTime()
{
    QDateTime dateTime;
    return dateTime.currentDateTime().toString();
}

QString GTALogResultTVRowEditor::getUserFeedback()const
{
    QString sUserFeedback = ui->UserFeedBackPTE->toPlainText();
    return sUserFeedback;
}
QString GTALogResultTVRowEditor::getResultStatus()const
{
    return ui->ResultCB->currentText();
}
