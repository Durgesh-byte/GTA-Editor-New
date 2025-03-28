#ifndef GTALOGRESULTTVROWEDITOR_H
#define GTALOGRESULTTVROWEDITOR_H
#include "GTACommandBase.h"
#include <QWidget>

namespace Ui {
class GTALogResultTVRowEditor;
}

class GTALogResultTVRowEditor : public QWidget
{
    Q_OBJECT

public:
    explicit GTALogResultTVRowEditor(QWidget *parent = 0);
    ~GTALogResultTVRowEditor();



    void setAction(const QString &iAction);
    void setActionOnFail(const QString &iActionOnFail);
    void setTimeOut(const QString &iTimeOut);
    void setPrecision(const QString &iPrecision);
    void setComment(const QString &iComment);
    void setResult(const QString &iResult);
    void setCurrentValue(const QString &iCurrentValue);
    void setExpectedValue(const QString &iExpectedValue);
    void setDumpList(const QString &iDumpList);
    void setDefects(const QString &iDefects);
    void setUserFeedBack(const QString &iFeedback);
    void setExecutionTime(const QString &iExecTime);

    void setData(const QString &iData, const int &colNumber);

    QString getSystemDateTime();
    QString getUserFeedback()const;
    QString getResultStatus()const;
    QString getExecutionTime() const;
    QString getDefects() const;

    bool getChangeStatus() const;

private slots:
    void onResultCBValueChanged(const QString &iVal);
    void onOKPBClicked();
    void onCancelPBClicked();

signals:
    void rowDataChanged();

private:
    Ui::GTALogResultTVRowEditor *ui;
    bool isOkPBClicked;
};

#endif // GTALOGRESULTTVROWEDITOR_H
