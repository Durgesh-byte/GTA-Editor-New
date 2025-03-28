#ifndef GTAACTIONROBOTICARMWIDGET_H
#define GTAACTIONROBOTICARMWIDGET_H
#include "GTAActionRoboArm.h"
#include "GTAActionWidgetInterface.h"

#pragma warning(push, 0)
#include <QWidget>
#include <QLineEdit>
#pragma warning(pop)


namespace Ui {
    class GTAActionRoboticArmWidget;
}

class GTAActionRoboticArmWidget :public GTAActionWidgetInterface
{
    Q_OBJECT

public:
    
    explicit GTAActionRoboticArmWidget(QWidget *parent = 0);
    ~GTAActionRoboticArmWidget();

    void setActionCommand(const GTAActionBase * ipActionCmd);
    bool getActionCommand(GTAActionBase*& opCmd)const;
    void clear() ;
    bool isValid() const ;
    bool hasSearchItem(){return true;}
    int getSearchType();

   


private slots:
    void onSequenceFileChange(const QString & iFunctionName);
    void onFunctionSelectionChange(int iSelectedRow);
    void onSearchPBClicked();

private:
    mutable bool _IsEditMode;
    QString getFunction() const;
    //QStringList getFunction() const;
    void setFunction(const QString & iFunctionName);

    QString getSequenceFile() const;
    void setSquenceFile(const QString & iSequenceFile);


    Ui::GTAActionRoboticArmWidget *ui;

    GTAActionRoboArm  * _pCommand;
    QString _CurrentFunctionType;

    QByteArray     _CurrentFunctionCode;

};

#endif // GTAActionRoboticArmWidget_H
