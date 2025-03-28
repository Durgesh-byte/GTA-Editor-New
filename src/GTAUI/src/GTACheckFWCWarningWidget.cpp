#include "GTACheckFWCWarningWidget.h"
#include "ui_GTACheckFWCWarningWidget.h"
#include "GTACheckFwcWarning.h"
#include "GTAParamValidator.h"
#include "GTAUtil.h"
GTACheckFWCWarningWidget::GTACheckFWCWarningWidget(QWidget *parent) :
    GTACheckWidgetInterface(parent),
    ui(new Ui::GTACheckFWCWarningWidget)
{
    ui->setupUi(this);

    setBuffDepth("0");
    connect(ui->SearchPB,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));

    ui->waitForBufferLE->setValidator( new QDoubleValidator(0, 100, 4, this));
    
    /*GTAParamValidator* pValidator1 = new GTAParamValidator(ui->headerLE);
    ui->headerLE->setValidator(pValidator1);*/
    
    /*GTAParamValidator* pValidator2 = new GTAParamValidator(ui->WarningMsgLE);
    ui->WarningMsgLE->setValidator(pValidator2);*/
}

GTACheckFWCWarningWidget::~GTACheckFWCWarningWidget()
{
    delete ui;
}
void GTACheckFWCWarningWidget::setMessage(const QString & iMsg)
{
    ui->WarningMsgLE->setText(iMsg);
}

void GTACheckFWCWarningWidget::setCondition(const QString & iCondition)
{
    for(int i = 0; i < ui->OperatorCB->count(); i++)
    {
        if(ui->OperatorCB->itemText(i) == iCondition)
        {
            ui->OperatorCB->setCurrentIndex(i);
            break;
        }
    }
}

QString GTACheckFWCWarningWidget::getMessage() const
{
   return ui->WarningMsgLE->text();
}
QString GTACheckFWCWarningWidget::getCondition() const
{
    return ui->OperatorCB->currentText();
}
void GTACheckFWCWarningWidget::onSearchPBClicked()
{
    emit searchMessage(ui->WarningMsgLE);
}
void GTACheckFWCWarningWidget::setCheckCommand(const GTACheckFwcWarning * ipCheckCmd)
{
    if(ipCheckCmd != NULL)
    {
        QString message = ipCheckCmd->getMessage();
        QString condition= ipCheckCmd->getCondition();
        QString sHeader = ipCheckCmd->getHeader();
        QString sWaitForbuff = ipCheckCmd->getWaitForBuffer();
        bool bWaitForAck = ipCheckCmd->getWaitAck();
        QString color = ipCheckCmd->getFWCWarningColor();

        setWrngColor(color);
        setMessage(message);
        setCondition(condition);
        setBuffDepth(sWaitForbuff);
        setHeader(sHeader);
        setWaitforAck(bWaitForAck);
        
    }
	else
	{
		setMessage("");
		 ui->OperatorCB->setCurrentIndex(0);
	}
}

GTACheckFwcWarning * GTACheckFWCWarningWidget::getCheckCommand()
{
    QString message = getMessage();
    QString condition= getCondition();
    QString sHeader = getHeader();
    bool bWaitForAck = getWaitforAck();
    QString sWaitForBuffDepth = getBuffDepth();
    QString color = getWrngColor();


    GTACheckFwcWarning * pCheckCmd = new GTACheckFwcWarning();
    pCheckCmd->setMessage(message);
    pCheckCmd->setCondition(condition);
    pCheckCmd->setHeader(sHeader);
    pCheckCmd->setWaitForBuffer(sWaitForBuffDepth);
    pCheckCmd->setWaitAck(bWaitForAck);
    pCheckCmd->setFWCWarningColor(color);
	pCheckCmd->setTimeOut(sWaitForBuffDepth,ACT_TIMEOUT_UNIT_SEC);
	
    return pCheckCmd;
}
void GTACheckFWCWarningWidget::clear()
{
    ui->WarningMsgLE->clear();
    ui->waitForBufferLE->clear();
    ui->waitForACKChkB->setChecked(true);
    ui->headerLE->clear();
    if(ui->OperatorCB->count()>0)
        ui->OperatorCB->setCurrentIndex(0);

    setBuffDepth("0");

}

bool GTACheckFWCWarningWidget::isValid()
{
    bool rc = true;
    if(ui->WarningMsgLE->text().isEmpty() && ui->waitForBufferLE->text().isEmpty() )
        rc = false;
    return rc;
}

bool GTACheckFWCWarningWidget::getCommand(GTACheckBase *& pCheckCmd)
{
    bool rc = isValid();
    if(rc)
    {
        pCheckCmd = getCheckCommand();
    }
    return rc;
}

bool GTACheckFWCWarningWidget::setCommand(const GTACheckBase * pCheckCmd)
{
    bool rc = false;
    if(pCheckCmd != NULL && pCheckCmd->getCheckType() == GTACheckBase::FWC_WARNING)
    {
        GTACheckFwcWarning * pCmd = dynamic_cast<GTACheckFwcWarning*>((GTACheckBase*)pCheckCmd);
        if(pCmd != NULL)
        {
            setCheckCommand(pCmd);
            rc = true;
        }

    }
    return rc;
}
QString GTACheckFWCWarningWidget::getHeader()const
{

    return ui->headerLE->text();

}
bool GTACheckFWCWarningWidget::getWaitforAck()const
{
    if (ui->waitForACKChkB->checkState() == Qt::Checked)
        return true;
    else
        return false;
}
QString GTACheckFWCWarningWidget::getBuffDepth()const
{
 return ui->waitForBufferLE->text();
}
void GTACheckFWCWarningWidget::setHeader(const QString& iVal)
{
    ui->headerLE->setText(iVal);
}
void GTACheckFWCWarningWidget::setWaitforAck(const bool& iVal)
{
    if (iVal)
        ui->waitForACKChkB->setChecked(true);
    else
     ui->waitForACKChkB->setChecked(false);
}
void GTACheckFWCWarningWidget::setBuffDepth(const QString& iVal)
{
    ui->waitForBufferLE->setText(iVal);
}
void GTACheckFWCWarningWidget::setWrngColor(const QString &iColor)
{
    ui->colorLE->setText(iColor);
}
QString GTACheckFWCWarningWidget::getWrngColor() const
{
    return ui->colorLE->text();
}


bool GTACheckFWCWarningWidget::hasTimeOut()const 
{
    return true;
}

bool GTACheckFWCWarningWidget::hasDumpList()const 
{
    return true;
}

bool GTACheckFWCWarningWidget::hasPrecision()const
{
    return false;
}

bool GTACheckFWCWarningWidget::hasSearchItem()const
{
    return false;
}

bool GTACheckFWCWarningWidget:: hasConfigTime() const
{
    return true;
}

bool GTACheckFWCWarningWidget::hasActionOnFail()const

{
    return true;
}

bool GTACheckFWCWarningWidget::setWidgetFields(const QList<QPair<QString, QString>>& params, ErrorMessageList& errorLogs)
{
    for (auto param : params) {
        if (param.first == "WarningMsgLE")
            ui->WarningMsgLE->setText(param.second);
        else if (param.first == "headerLE")
            ui->headerLE->setText(param.second);
        else if (param.first == "colorLE")
            ui->colorLE->setText(param.second);
        else if (param.first == "waitForBufferLE")
        {
            bool paramSecondIsNumber = false;
            param.second.toDouble(&paramSecondIsNumber);
            if (paramSecondIsNumber)
                ui->waitForBufferLE->setText(param.second);
            else
                ui->waitForBufferLE->setText("0");
        }
        else if (param.first == "OperatorCB")
        {
            int currIdx = ui->OperatorCB->findText(param.second);
            if (currIdx >= 0)
                ui->OperatorCB->setCurrentIndex(currIdx);
            else
                ui->OperatorCB->setCurrentIndex(0);
        }
        else {
            errorLogs.append(QString("Check FWC warning unexpected first param: %1").arg(param.first));
            return false;
        }
    }
    return true;
}
