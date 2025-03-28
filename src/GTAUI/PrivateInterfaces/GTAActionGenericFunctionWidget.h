#ifndef GTAACTIONGENERICFUNCTIONWIDGET_H
#define GTAACTIONGENERICFUNCTIONWIDGET_H

#include "GTAActionWidgetInterface.h"
#include "GTAActionGenericFunction.h"

#pragma warning(push, 0)
#include <QWidget>
#include <QLineEdit>
#include <QDomNode>
#pragma warning(pop)

namespace Ui {
class GTAActionGenericFunctionWidget;
}


class GTAActionGenericFunctionWidget : public GTAActionWidgetInterface 
{
    Q_OBJECT

public:
    explicit GTAActionGenericFunctionWidget(QWidget *parent = 0);
    ~GTAActionGenericFunctionWidget();
    void initialize();

    void setActionCommand(const GTAActionBase * ipActionCmd);
    bool getActionCommand(GTAActionBase*& opCmd)const;
    void clear() ;
    bool isValid() const ;

    bool hasActionOnFail()  {return true;}
    bool hasPrecision()     {return true;}
    bool hasTimeOut(int& oValidation){oValidation = NUMBER|NON_NEGATIVE;return true;}
    bool hasDumpList()      {return true;}
    bool hasConfigTime()    {return true;}
   


signals:
     
private:

    Ui::GTAActionGenericFunctionWidget *ui;
    bool _IsLocalParam;
    QDomDocument _domDoc;
    QDomNode _stateNode;
    QString _textNode;


    void refreshPreviewSCXML();
    void setNote(const QString& inputString,bool iShow);

    

private slots:
    void toolIDChanged(QString);
    void toolTypeChanged(QString iToolID);
    void funNameChanged(QString iName);
    void returnValueChanged(QString iName);
    void argTypeChanged(const QString& iName);
    void onAddArgument(); 
    void onEditArgument();
    void onRemoveArgument();
    void onFailConditionChanged(QString);
    void onSuccessConditionChanged(QString);
    void onArgumentListClicked();
    void onEditCondtionChanged(bool);

};

#endif // GTAActionGenericFunctionWidget_H
