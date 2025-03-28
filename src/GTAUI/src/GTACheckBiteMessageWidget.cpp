#include "GTACheckBiteMessageWidget.h"
#include "ui_GTACheckBiteMessageWidget.h"
#include "GTACheckBiteMessage.h"
#include "GTAUtil.h"
#include "GTAParamValidator.h"




//#define REPORT "Ground report;Last leg report;Trouble shooting data;Schedule maintenance report;Specific data report;Post Flight Report"



GTACheckBiteMessageWidget::GTACheckBiteMessageWidget(QWidget *parent) :
    GTACheckWidgetInterface(parent),
    ui(new Ui::GTACheckBiteMessageWidget)
{
    ui->setupUi(this);
    QStringList reportList; reportList = QString(CHECK_BITE_MESSAGE_TYPE).split(";");
    ui->ReportCB->addItems(reportList);
    connect(ui->SearchPB,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));


  /*  GTAParamValidator* pValidator1 = new GTAParamValidator(ui->MessageLE);
    ui->MessageLE->setValidator(pValidator1);*/

    ui->waitForbufferLE->setValidator( new QDoubleValidator(0, 100, 4, this));
    ui->label_4->setVisible(false);
    ui->ReportCB->setVisible(false);
 
}

GTACheckBiteMessageWidget::~GTACheckBiteMessageWidget()
{
    delete ui;
}
QString GTACheckBiteMessageWidget::getMessage() const
{
    return ui->MessageLE->text();
}
QString GTACheckBiteMessageWidget::getCondition() const
{
    return ui->OperatorCB->currentText();
}
QString GTACheckBiteMessageWidget::getReport() const
{
    return ui->ReportCB->currentText();
}
void GTACheckBiteMessageWidget::onSearchPBClicked()
{
    emit searchMessage(ui->MessageLE);
}


void GTACheckBiteMessageWidget::setCheckCommand(const GTACheckBiteMessage * ipCheckCmd)
{
    if(ipCheckCmd != NULL)
    {
        QString msg = ipCheckCmd->getMessage();
        QString cond= ipCheckCmd->getCondition();
        QString sGetOnReport=ipCheckCmd->getOnReport();
        QString sWaitForBufferDepth = ipCheckCmd->getBuffDepth();
        bool bWaitForAck = ipCheckCmd->getWaitStatus();

        ui->MessageLE->setText(msg);
        ui->waitForbufferLE->setText(sWaitForBufferDepth);

        if (bWaitForAck)
             ui->waitForAckChkB->setChecked(true);
        else
            ui->waitForAckChkB->setChecked(false);
        int operCount = ui->OperatorCB->count();
        for(int i = 0; i < operCount; i++)
        {
            if(ui->OperatorCB->itemText(i) == cond)
            {
                ui->OperatorCB->setCurrentIndex(i);
                break;
            }
        }

        int index = ui->ReportCB->findText(sGetOnReport);
        ui->ReportCB->setCurrentIndex(index);
//        int repoCount = ui->ReportCB->count();
//        for(int i = 0; i < repoCount; i++)
//        {
//            if(ui->ReportCB->itemText(i) == sGetOnReport)
//            {
//                ui->ReportCB->setCurrentIndex(i);
//                break;
//            }
//        }
    }
	else
	{
		this->clear();
	}
}

GTACheckBiteMessage * GTACheckBiteMessageWidget::getCheckCommand()
{
    GTACheckBiteMessage * pCheckCmd = new GTACheckBiteMessage();
    pCheckCmd->setMessage(getMessage());
    pCheckCmd->setCondition(getCondition());
    pCheckCmd->setOnReport(getReport());
    pCheckCmd->setWaitStatus(getWaitForAck());
    pCheckCmd->setBuffDepth(getWaitBuffDepth());
    pCheckCmd->setTimeOut(getWaitBuffDepth(),ACT_TIMEOUT_UNIT_SEC);
    return pCheckCmd;

}
void GTACheckBiteMessageWidget::clear()
{
    ui->MessageLE->clear();
    ui->waitForbufferLE->clear();
    
    ui->waitForAckChkB->setChecked(true);
    
    if(ui->OperatorCB->count())
        ui->OperatorCB->setCurrentIndex(0);
    
    if(ui->ReportCB->count())
        ui->ReportCB->setCurrentIndex(0);

}

bool GTACheckBiteMessageWidget::isValid()
{
    bool rc = false;
    if(! getMessage().isEmpty() && ! getReport().isEmpty())
        rc = true;
    return rc;
}

bool GTACheckBiteMessageWidget::getCommand(GTACheckBase *& pCheckCmd)
{
    bool rc = isValid();
    if(rc)
    {
        pCheckCmd = getCheckCommand();
    }
    return rc;
}

bool GTACheckBiteMessageWidget::setCommand(const GTACheckBase * pCheckCmd)
{
	bool rc = false;
	if(pCheckCmd != NULL)
	{
		GTACheckBiteMessage * pCmd = dynamic_cast<GTACheckBiteMessage*>((GTACheckBase*)pCheckCmd);
		if(pCmd != NULL)
		{
			rc = true;
			setCheckCommand(pCmd);
		}
	}
	return rc;
}
bool GTACheckBiteMessageWidget:: getWaitForAck()const
{
    if (ui->waitForAckChkB->checkState() == Qt::Checked)
       return true;
    else
        return false;
}
QString GTACheckBiteMessageWidget:: getWaitBuffDepth()const
{
    return ui->waitForbufferLE->text();
}
bool GTACheckBiteMessageWidget:: hasTimeOut()const       
{
    return true;
}
bool GTACheckBiteMessageWidget:: hasDumpList()const
{ 
    return true;
}
bool GTACheckBiteMessageWidget:: hasPrecision()const
{ return false;}
bool GTACheckBiteMessageWidget:: hasSearchItem()const
{ return true;}
bool GTACheckBiteMessageWidget:: hasConfigTime() const
{return false;}
bool GTACheckBiteMessageWidget:: hasActionOnFail()const
{ return true;}

bool GTACheckBiteMessageWidget::setWidgetFields(const QList<QPair<QString, QString>>& params, ErrorMessageList& errorLogs)
{
    for (auto param : params) {
        if (param.first == "MessageLE")
        {
            if (param.second != "")
                ui->MessageLE->setText(param.second);
            else
            {
                errorLogs.push_back({ "Check bite MessageLE value is empty" });
                return false;
            }
        }
        else if (param.first == "waitForbufferLE")
        {
            bool paramSecondIsNumber = false;
            param.second.toDouble(&paramSecondIsNumber);
            if (paramSecondIsNumber)
                ui->waitForbufferLE->setText(param.second);
            else
                ui->waitForbufferLE->setText("0");
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
            errorLogs.append(QString("Check bite message unexpected param first value: %1").arg(param.first));
            return false;
        }
    }
    return true;
}