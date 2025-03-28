#include "GTAActionPrintTimeWgt.h"
#include "GTAUtil.h"
#include "ui_GTAActionPrintTimeWgt.h"
#include <QDateTime>
#include "GTAActionPrintTime.h"


GTAActionPrintTimeWgt::GTAActionPrintTimeWgt(QWidget * parent) :  GTAActionWidgetInterface(parent),
ui(new Ui::GTAActionPrintTimeWgt)
{
    ui->setupUi(this);
    QStringList comboList;
    comboList<<ACT_PRINT_TIME_TYPE_NA<<ACT_PRINT_TIME_TYPE_NOW;
    ui->TimeTypeCB->insertItems(0,comboList);
}
GTAActionPrintTimeWgt::~GTAActionPrintTimeWgt()
{
    delete ui;
}
 bool GTAActionPrintTimeWgt::getActionCommand(GTAActionBase*& opCmd)const
{
    GTAActionPrintTime *pCmd = new GTAActionPrintTime();
    QString timeType = ui->TimeTypeCB->currentText();
    pCmd->setTimeType(GTAActionPrintTime::Formatted);

    if( QString::compare(timeType,ACT_PRINT_TIME_TYPE_NOW,Qt::CaseInsensitive) ==0)
       pCmd->setTimeType(GTAActionPrintTime::UTC);

    opCmd = pCmd;
    return true;
}

 void GTAActionPrintTimeWgt::setActionCommand(const GTAActionBase * ipActionCmd)
{
    GTAActionPrintTime *pCmd =dynamic_cast<GTAActionPrintTime*> ((GTAActionBase *)ipActionCmd);
    GTAActionPrintTime::TimeType timeType= pCmd->getTimeType();
    ui->TimeTypeCB->setCurrentIndex(0);
    if(timeType == GTAActionPrintTime::UTC)
    {
        int index = ui->TimeTypeCB->findText(ACT_PRINT_TIME_TYPE_NOW,Qt::MatchFixedString);
        if (index!=-1)
        {
            ui->TimeTypeCB->setCurrentIndex(index);
        }
    }
   clear();
}
void GTAActionPrintTimeWgt::clear() 
{

}
bool GTAActionPrintTimeWgt::isValid()const 
{
    return true;
}

bool GTAActionPrintTimeWgt::setWidgetFields(const QList<QPair<QString, QString>>& params, ErrorMessageList& errorLogs)
{
    if (params.at(0).first == "TimeTypeCB")
    {
        if(params.at(0).second == "Formated")
            ui->TimeTypeCB->setCurrentIndex(0);
        else if(params.at(0).second == "UTC")
            ui->TimeTypeCB->setCurrentIndex(1);
        else
        {
            errorLogs.append(QString("Unsupported time type %1").arg(params.at(0).second));
            return false;
        }
    }
    else {
        errorLogs.append(QString("Expected TimeTypeCB got %1").arg(params.at(0).first));
        return false;
    }
    return true;
}

#include "GTAWgtRegisterDef.h"
REGISTER_ACTION_WIDGET(QString("%1 %2").arg(ACT_PRINT,COM_PRINT_TIME),GTAActionPrintTimeWgt,obj)
