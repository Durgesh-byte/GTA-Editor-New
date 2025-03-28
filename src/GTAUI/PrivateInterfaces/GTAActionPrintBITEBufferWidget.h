#ifndef GTAACTIONPRINTBITEBUFFERWIDGET_H
#define GTAACTIONPRINTBITEBUFFERWIDGET_H
#include "GTAActionLineEditor.h"
#include "GTAActionPrint.h"
class GTAActionPrintBITEBufferWidget : public GTAActionLineEditor
{
public:
    GTAActionPrintBITEBufferWidget(QWidget * parent=0);

    void setActionCommand(const GTAActionBase * ipActionCmd);
    bool getActionCommand(GTAActionBase*& opCmd)const;
    void clear() ;
    bool isValid()const ;
};


#endif // GTAACTIONPRINTBITEBUFFERWIDGET_H
