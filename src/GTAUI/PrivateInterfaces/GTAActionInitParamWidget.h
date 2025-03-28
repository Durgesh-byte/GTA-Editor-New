#ifndef GTAActionInitParamWidget_H
#define GTAActionInitParamWidget_H
#include "GTAActionWidgetInterface.h"

#pragma warning(push, 0)
#include <QWidget>
#include <QLineEdit>
#pragma warning(pop)

class GTAActionSubscribe;
namespace Ui {
    class GTAActionInitParamWidget;
}

class GTAActionInitParamWidget : public GTAActionWidgetInterface
{
    Q_OBJECT

public:
    explicit GTAActionInitParamWidget(QWidget *parent = 0);
    ~GTAActionInitParamWidget();

   
    void setValue(const QString & iVal) ;
    
    

    void setActionCommand(const GTAActionBase * ipActionCmd);
    bool getActionCommand(GTAActionBase*& opCmd)const;
    void clear() ;
    bool isValid()const ;
    bool hasSearchItem(){return true;}
    int getSearchType();
    bool setWidgetFields(const QList<QPair<QString, QString>>& params, ErrorMessageList& errorLogs) override final;

signals:
    void searchObject(QLineEdit *&);

private:
    Ui::GTAActionInitParamWidget *ui;
    QStringList getValues() const;

private slots:
    void onSearchPBClicked();
    void addToParamList();
    void removeFromParamList();
    void editFromParamList();
    void updateParamLEFromLst( );
};

#endif // GTAActionInitParamWidget_H
