#include "GTAActionFunctionReturnWgt.h"
#include "GTAUtil.h"
#include "ui_GTAActionFunctionReturnWgt.h"
#include <QDateTime>
#include "GTAActionFunctionReturn.h"
#include "GTAGenSearchWidget.h"

GTAActionFunctionReturnWgt::GTAActionFunctionReturnWgt(QWidget * parent) :  GTAActionWidgetInterface(parent),
ui(new Ui::GTAActionFunctionReturnWgt)
{  
    ui->setupUi(this);
    connect(ui->searchPB,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));
}
GTAActionFunctionReturnWgt::~GTAActionFunctionReturnWgt()
{
    delete ui;
}
 bool GTAActionFunctionReturnWgt::getActionCommand(GTAActionBase*& opCmd)const
{
    GTAActionFunctionReturn *pCmd = NULL;
    

    if(isValid())
    {
        pCmd = new GTAActionFunctionReturn();
        QString sReturnItem = ui->returnItemLE->text();
        pCmd->setReturnItem(sReturnItem);
    }

    opCmd = pCmd;
    return true;
}
 void GTAActionFunctionReturnWgt::onSearchPBClicked()
 {
     emit searchObject(ui->returnItemLE);
 }
 void GTAActionFunctionReturnWgt::setActionCommand(const GTAActionBase * ipActionCmd)
{
       clear();
    GTAActionFunctionReturn *pCmd =dynamic_cast<GTAActionFunctionReturn*> ((GTAActionBase *)ipActionCmd);
    QString sReturnItem= pCmd->getReturnItem();
    ui->returnItemLE->setText(sReturnItem);

}
void GTAActionFunctionReturnWgt::clear() 
{
    ui->returnItemLE->clear();
}
bool GTAActionFunctionReturnWgt::isValid()const 
{
    if(ui->returnItemLE->text().isEmpty())
        return false;
    return true;
}
int GTAActionFunctionReturnWgt::getSearchType()
{
    return GTAGenSearchWidget::PARAMETER;
}

#include "GTAWgtRegisterDef.h"
REGISTER_ACTION_WIDGET(ACT_FUNC_RETURN,GTAActionFunctionReturnWgt,obj)
