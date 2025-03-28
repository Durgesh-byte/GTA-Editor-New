#include "GTAActionPrintBITEBufferWidget.h"
#include "GTAUtil.h"
GTAActionPrintBITEBufferWidget::GTAActionPrintBITEBufferWidget(QWidget * parent) : GTAActionLineEditor(parent)
{
    setLabel("Wait for buffer depth");
    setText("0");
    showMessageTypeCB();
    setMessageTypeList(QString(CHECK_BITE_MESSAGE_TYPE).split(";"));
}
 bool GTAActionPrintBITEBufferWidget::getActionCommand(GTAActionBase*& opCmd)const
{
    bool rc = isValid();
    if(rc)
    {
        GTAActionPrint * pPrintCmd = new GTAActionPrint();
        pPrintCmd->setPrintType(GTAActionPrint::BITE_BUFFER);
        pPrintCmd->setValues(QStringList()<<getText());
        pPrintCmd->setMessageType(getCurrentMessageType());
        opCmd = pPrintCmd;
    }
    return rc;
}

 void GTAActionPrintBITEBufferWidget::setActionCommand(const GTAActionBase * ipActionCmd)
{
   clear();
   bool rc = false;
   if(ipActionCmd != NULL)
   {
       GTAActionPrint * pPrintCmd = dynamic_cast<GTAActionPrint*>((GTAActionBase*)ipActionCmd);
       if(pPrintCmd != NULL)
       {
          // setText(pPrintCmd->getValue());

           QStringList sParamList = pPrintCmd->getValues();
           if(!sParamList.isEmpty())
           {
               setText(sParamList.at(0));
               rc = true;
                setCurrentMessageType(pPrintCmd->getMessageType());
           }

          
          
       }
   }
 
}
void GTAActionPrintBITEBufferWidget::clear() 
{

}
bool GTAActionPrintBITEBufferWidget::isValid()const 
{
    return true;
}
#include "GTAWgtRegisterDef.h"
REGISTER_ACTION_WIDGET(QString("%1 %2").arg(ACT_PRINT,COM_PRINT_BITE_BUFF),GTAActionPrintBITEBufferWidget,obj)
