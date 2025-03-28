#ifndef GTAACTIONIRTWIDGET_H
#define GTAACTIONIRTWIDGET_H
#include "GTAActionWidgetInterface.h"

#pragma warning(push, 0)
#include <QWidget>
#include <QLineEdit>
#pragma warning(pop)

namespace Ui {
    class GTAActionIRTWidget;
}

class GTAActionIRT;
class GTAActionIRTWidget : public GTAActionWidgetInterface
{
    Q_OBJECT

public:
    
    explicit GTAActionIRTWidget(QWidget *parent = 0);
    ~GTAActionIRTWidget();

    void setActionCommand(const GTAActionBase* ipActionCmd);
    bool getActionCommand(GTAActionBase*& opCmd)const;
    void clear() ;
    int getSearchType();
    bool isValid()const{return true;}
    bool hasSearchItem(){return true;}

    //QList<GTAActionIRT *> getActionCommand() const;

    


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


    Ui::GTAActionIRTWidget *ui;

    GTAActionIRT  * _pCommand;
    QString _CurrentFunctionType;

    QByteArray     _CurrentFunctionCode;

};

#endif // GTAACTIONIRTWIDGET_H
