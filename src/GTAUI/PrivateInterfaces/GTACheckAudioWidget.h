#ifndef GTACHECKAUDIOWIDGET_H
#define GTACHECKAUDIOWIDGET_H

#include <QWidget>
#include <QHash>
#include <QLineEdit>
#include "GTACheckWidgetInterface.h"
namespace Ui {
class GTACheckAudioWidget;
}
class GTACheckAudioAlarm;
class GTACheckAudioWidget :/* public QWidget, */public GTACheckWidgetInterface
{
    Q_OBJECT

public:
    explicit GTACheckAudioWidget(QWidget *parent = 0);
    ~GTACheckAudioWidget();

    void setCheckCommand(const GTACheckAudioAlarm * ipCheckCmd);
    GTACheckAudioAlarm * getCheckCommand();

    //Derived method
    virtual void clear();
    virtual bool isValid();
    virtual bool getCommand(GTACheckBase *& pCheckCmd);
    virtual bool setCommand(const GTACheckBase * pCheckCmd);
    void setSoundList(const QStringList &iList);

    bool hasSearchItem()const ;
    bool hasActionOnFail()const ;
    bool hasPrecision()const;
    bool hasTimeOut()const ;
    bool hasDumpList()const;
    bool hasConfigTime() const ;

signals:
    void searchMessage(QLineEdit *&);


private:
    Ui::GTACheckAudioWidget *ui;


    QString getValue() const;
    QString getTime() const;


private slots:
    void onSearchPBClicked();
};

#endif // GTACHECKAUDIOWIDGET_H
