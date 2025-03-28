#include "GTAActionPrintFWCBufferWidget.h"
#include "GTAActionPrint.h"
#include <QDoubleValidator>
GTAActionPrintFWCBufferWidget::GTAActionPrintFWCBufferWidget(QWidget *parent ) : GTAActionLineEditor(parent)
{
    setLabel("Wait for buffer depth");
    setText("0");
    QDoubleValidator* pDoubleValidator = new QDoubleValidator(0);
    
    setValidator(pDoubleValidator);
}
bool GTAActionPrintFWCBufferWidget::getActionCommand(GTAActionBase *& pCmd)const
{
    bool rc = isValid();
    if(rc)
    {
        GTAActionPrint * pPrintCmd = new GTAActionPrint();
        pPrintCmd->setValues(QStringList()<<getText());
        pPrintCmd->setPrintType(GTAActionPrint::FWC_BUFFER);
        pCmd = pPrintCmd;
    }
    return rc;
}

void GTAActionPrintFWCBufferWidget::setActionCommand(const GTAActionBase * pCmd)
{
   clear();
  
   if(pCmd != NULL)
   {
       GTAActionPrint * pPrintCmd = dynamic_cast<GTAActionPrint*>((GTAActionBase*)pCmd);
       if(pPrintCmd != NULL)
       {
           QStringList sParamList = pPrintCmd->getValues();
           if(!sParamList.isEmpty())
           {
               setText(sParamList.at(0));
               
           }
       }
   }
 
}
void GTAActionPrintFWCBufferWidget::clear() 
{

}
bool GTAActionPrintFWCBufferWidget::isValid()const 
{
    return true;
}
#include "GTAWgtRegisterDef.h"
REGISTER_ACTION_WIDGET(QString("%1 %2").arg(ACT_PRINT,COM_PRINT_FWC_BUFF),GTAActionPrintFWCBufferWidget,obj)
