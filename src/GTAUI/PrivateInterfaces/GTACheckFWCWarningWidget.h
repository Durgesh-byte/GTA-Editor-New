#ifndef GTACheckFWCWarningWidget_H
#define GTACheckFWCWarningWidget_H
#include "GTACheckWidgetInterface.h"

#pragma warning(push, 0)
#include <QWidget>
#include <QLineEdit>
#pragma warning(pop)


namespace Ui {
    class GTACheckFWCWarningWidget;
}

class GTACheckFwcWarning;
class GTACheckFWCWarningWidget : public GTACheckWidgetInterface
{
    Q_OBJECT

public:
    explicit GTACheckFWCWarningWidget(QWidget *parent = 0);
    ~GTACheckFWCWarningWidget();

    void setCheckCommand(const GTACheckFwcWarning * ipCheckCmd);
    GTACheckFwcWarning * getCheckCommand();



    virtual void clear();
    virtual bool isValid();
    virtual bool getCommand(GTACheckBase *& pCheckCmd);
    virtual bool setCommand(const GTACheckBase * pCheckCmd);
    void setHeader(const QString& iVal);
    void setWrngColor(const QString &iColor);

    bool hasTimeOut()const       ;
    bool hasDumpList()const      ;
    bool hasPrecision()const     ;
    bool hasSearchItem()const    ;
    bool hasConfigTime() const   ;
    bool hasActionOnFail()const  ;
    bool setWidgetFields(const QList<QPair<QString, QString>>& params, ErrorMessageList& errorLogs) override final;


signals:
    void searchMessage(QLineEdit *&);

private:
    Ui::GTACheckFWCWarningWidget *ui;
    

    QString getWrngColor() const;
    void setMessage(const QString & iMsg);
    void setCondition(const QString & iCondition);
    QString getMessage() const;
    QString getCondition() const;
    QString getHeader()const;
    bool getWaitforAck()const;
    QString getBuffDepth()const;


    void setWaitforAck(const bool& iVal);
    void setBuffDepth(const QString& iVal);


private slots:
    void onSearchPBClicked();
};

#endif // GTACheckFWCWarningWidget_H
