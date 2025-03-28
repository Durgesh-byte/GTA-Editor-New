#ifndef GTAACTIONAVCWIDGET_H
#define GTAACTIONAVCWIDGET_H
#include "GTAActionWidgetInterface.h"

#pragma warning(push, 0)
#include <QWidget>
#include <QLineEdit>
#pragma warning(pop)

namespace Ui {
class GTAActionAVCWidget;
}

class GTAActionManual;
class GTAActionAVCWidget : public GTAActionWidgetInterface
{
    Q_OBJECT

public:
    explicit GTAActionAVCWidget(QWidget *parent = 0);
    ~GTAActionAVCWidget();

    void setActionCommand(const GTAActionBase * );
    bool getActionCommand(GTAActionBase*& opCmd)const;
    void clear() ;
    bool isValid() const ;
	

signals:
     void searchParameter(QLineEdit *&);
private:
	
    Ui::GTAActionAVCWidget *ui;

    bool loadDynamicWidget(const QString& iFilePath);

     

private slots:

    void onBrowseXML();
    
    

};

#endif // GTAActionAVCWidget_H


