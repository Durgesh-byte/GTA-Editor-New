#ifndef GTACheckBiteMessageWidget_H
#define GTACheckBiteMessageWidget_H
#include "GTACheckWidgetInterface.h"

#pragma warning(push, 0)
#include <QWidget>
#include <QLineEdit>
#pragma warning(pop)

namespace Ui {
    class GTACheckBiteMessageWidget;
}
class GTACheckBiteMessage;
class GTACheckBiteMessageWidget : public GTACheckWidgetInterface
{
    Q_OBJECT

public:
    explicit GTACheckBiteMessageWidget(QWidget *parent = 0);
    ~GTACheckBiteMessageWidget();

    void setCheckCommand(const GTACheckBiteMessage * ipCheckCmd);
    GTACheckBiteMessage * getCheckCommand();

    QString getMessage() const;
    QString getCondition() const;
    QString getReport() const;
    bool  getWaitForAck()const;
    QString  getWaitBuffDepth()const;

   virtual void clear();
   virtual bool isValid();
   virtual bool getCommand(GTACheckBase *& pCheckCmd);
   virtual bool setCommand(const GTACheckBase * pCheckCmd);

    bool hasTimeOut()const;
    bool hasDumpList()const;
    bool hasPrecision()const;
    bool hasSearchItem()const;
    bool hasConfigTime() const;
    bool hasActionOnFail()const;
    bool setWidgetFields(const QList<QPair<QString, QString>>& params, ErrorMessageList& errorLogs) override final;
   

signals:
    void searchMessage(QLineEdit *&);
private:
    Ui::GTACheckBiteMessageWidget *ui;

private slots:
    void onSearchPBClicked();
};

#endif // GTACheckBiteMessageWidget_H
