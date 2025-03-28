#ifndef GTAONECLICKWIDGET_H
#define GTAONECLICKWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QGridLayout>
#include "GTAOneClick.h"
#include "GTAOneClickTestRig.h"
#include "GTAActionWidgetInterface.h"

namespace Ui {
    class GTAOneClickWidget;
}

class GTAOneClickWidget : public GTAActionWidgetInterface
{
    Q_OBJECT

public:
    explicit GTAOneClickWidget(QWidget *parent = 0);
    ~GTAOneClickWidget();


    void setActionCommand(const GTAActionBase * ipActionCmd);
    bool getActionCommand(GTAActionBase*& opCmd)const;
    void clear() ;
    bool isValid() const ;
    bool hasSearchItem(){return true;}
    bool hasActionOnFail(){return true;}
    int getSearchType();



private:


    Ui::GTAOneClickWidget *ui;
    QGridLayout *DynamicOptionFL;


    QString _Description;
    GTAOneClickOptions _Options;
    GTAOneClickTestRig *_OneClickTestRig;
    QHash<QString, QWidget*> _CurrentOptionHash;
    QList<QWidget*> _CurrentOptionLabelList;
    QList<QString> _CurrentOptionList;

    QHash<QString,bool> _OptUpdatedFlag;


    bool parseConfFile(QString &iConfFile);

    bool createDymamicUI(GTAOneClickOptions iOption);

    
    void ClearDynamicOptionWidgetItems();

    bool decodeCondition(const QString &iCond, QString &oCondOption, QString &oCondValue);
    void setOptionValues(QString &iOptionName,QString &iVal);
    void setInitialValues();

    bool isRelatedtoOtherOptions(QString &iOptionName, QString &iVal);

    QString getConfFile()const;
    QString getDescription()const;
    QString getConfigName()const;
    QString getAppName()const;
    QString getTestRigName()const;
    QString getTestRigType()const;
    GTAOneClickOptions getOptions()const;

    void setDescription(const QString & iDescription);
    void setConfFile(const QString & iConfFile);
    void setConfigName(const QString & iConfigName);
    void setAppName(const QString & iAppName);
    void setTestRigName(const QString & iTestRigName);
    void setTestRigType(const QString & iTestRigType);

private slots:
    void validateText();
    void onSearchPBClicked();
    void onConfFileChanged(QString iConfFile);
    void onConfigNameChanged(QString iConfigName);
    void onApplicationSelected(QString iAppName);
    void onOptionValueChanged(bool iVal);
    void onOptionValueChanged(QString iVal);
    void onOptionValueChanged();
    void onStartEnvClicked(bool iVal);
    void onStopAltoClicked(bool iVal);
    void onStopTestConfig(bool iVal);
signals:
    void searchObject(QLineEdit *&);
};

#endif // GTAONECLICKWIDGET_H
