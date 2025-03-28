#ifndef GTAACTIONPRINTFWCBUFFER_H
#define GTAACTIONPRINTFWCBUFFER_H
#include "GTAActionLineEditor.h"
#include "GTAActionBase.h"
class GTAActionPrintFWCBufferWidget : public GTAActionLineEditor
{
public:
    GTAActionPrintFWCBufferWidget(QWidget *parent = 0);

    void setActionCommand(const GTAActionBase * ipActionCmd);
    bool getActionCommand(GTAActionBase*& opCmd)const;
    void clear() ;
    bool isValid()const ;
};

#endif // GTAACTIONPRINTFWCBUFFER_H
