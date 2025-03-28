#include "GTAWaitForWidget.h"
#include "ui_GTAWaitForWidget.h"
#include "GTAParamValidator.h"
#include "GTAUtil.h"

#include <qdebug.h>

GTAWaitForWidget::GTAWaitForWidget(QWidget *parent) :
    GTAActionWidgetInterface(parent),
    ui(new Ui::GTAWaitForWidget)
{
    ui->setupUi(this);
   
   /* QDoubleValidator* pDoubleValidator1 = new QDoubleValidator(ui->ValueLE);
    ui->ValueLE->setValidator(pDoubleValidator1);*/
   
}

GTAWaitForWidget::~GTAWaitForWidget()
{
    delete ui;
}
QString GTAWaitForWidget::getValue() const
{
    return ui->ValueLE->text();
}
void GTAWaitForWidget::setValue(const QString &iVal)
{
    ui->ValueLE->setText(iVal);
}
void GTAWaitForWidget::clear()
{
    ui->ValueLE->clear();
}

void GTAWaitForWidget::setActionCommand(const GTAActionBase * ipActionCmd)
{
    GTAActionWait* pWaitCmd = dynamic_cast<GTAActionWait*>((GTAActionBase *)ipActionCmd);
    clear();
    if(pWaitCmd != NULL && pWaitCmd->getWaitType() == GTAActionWait::FOR )
    {
        ui->ValueLE->setText(pWaitCmd->getCounter());
    }
}

bool GTAWaitForWidget::getActionCommand(GTAActionBase*& opCmd)const
{
    bool rc = false;
    GTAActionWait * pWaitCmd =  NULL;
    QString counter = ui->ValueLE->text();
    if(! counter.isEmpty() && isValid())
    {
        pWaitCmd =  new GTAActionWait(GTAActionWait::FOR);
        pWaitCmd->setCounter(counter);
        rc = true;
    }
    opCmd= pWaitCmd;
    return rc;
}


bool GTAWaitForWidget::isValid()const
{
    bool rc = false;
    QString counter = ui->ValueLE->text();
    bool isNum = false;
    if(counter.isEmpty())
        return rc;
    double countVal= counter.toDouble(&isNum);
    if (isNum)
       { 
           if (countVal>0)
              rc=true;
       }
    else
    {
        if (counter.count(TAG_IDENTIFIER)==2)
        {
            if (counter.startsWith(TAG_IDENTIFIER) && counter.endsWith(TAG_IDENTIFIER) && counter.size()>=3)
                rc = true;
        }
        // Manage the case where a local parameter is used
        else if(counter.count(TAG_IDENTIFIER)==0)
        {
            if(counter.size() > 0)
                rc = true;
        }
    }

    if(!rc)
        _LastError = "Counter can be a positive number or a valid GTA variable (ex:@var@)";
    return rc;

}

bool GTAWaitForWidget::setWidgetFields(const QList<QPair<QString, QString>>& params, ErrorMessageList& logs)
{
    auto& waitTime = params[0].second;
    bool isInt = false, isFloat = false;
    waitTime.toInt(&isInt);
    waitTime.toFloat(&isFloat);
    if (isInt || isFloat) {
        ui->ValueLE->setText(waitTime);
        return true;
    }
    else {
        logs.push_back(QString("Given delay: %1 is not a number").arg(waitTime));;
        return false;
    }
}

#include "GTAWgtRegisterDef.h"
REGISTER_ACTION_WIDGET(QString("%1 %2").arg(ACT_WAIT,COM_WAIT_FOR),GTAWaitForWidget,obj)
