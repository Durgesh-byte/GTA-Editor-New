#ifndef GTAWAITUNTILACTIONWIDGET_H
#define GTAWAITUNTILACTIONWIDGET_H

#pragma warning (push, 0)
#include <qcheckbox.h>
#include <qcombobox.h>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QHash>
#include "GTAActionWidgetInterface.h"
#pragma warning (pop)
namespace Ui {
    class GTAWaitUntilActionWidget;
}
class GTAActionWait;
class GTAWaitUntilActionWidget : public GTAActionWidgetInterface
{
    Q_OBJECT

public:
    explicit GTAWaitUntilActionWidget(QWidget *parent = 0);
    ~GTAWaitUntilActionWidget();
 
    bool isParameterLESelected();

    void setActionCommand(const GTAActionBase* ipActionCmd);
    bool getActionCommand(GTAActionBase*& opCmd)const;
    void clear() ;
    bool isValid()const;
    int getSearchType();
    bool hasSearchItem(){return true;}
    bool hasActionOnFail(){return true;}
    bool hasPrecision(){return true;}
    bool hasDumpList(){return true;}
    bool hasConfigTime();
	bool hasTimeOut(int& oValidation){ oValidation = NUMBER|NON_ZERO|MANDATORY|NON_NEGATIVE;return true;}
    bool hasCondition(){return true;}

    void processSearchInput(const QStringList&);
    QString getParity()const;
    QString getSDI()const;
    bool getCheckOnlyRefreshRateState()const;
    bool getCheckOnlySDIState()const;
    bool getCheckOnlyParityState()const;
    bool getLoopSamplingState() const;
    QString getLoopSampling() const;
    QString getUnitLoopSampling() const;
	void setOnlyValueStatus();
	void resetAllCheckBoxStatus();

    bool setWidgetFields(const QList<QPair<QString, QString>>& params, ErrorMessageList& errorLogs) override final;

signals:
    
    void conditionOperatorChanged(QString);
private:

    bool CheckAndSetCheckboxes(const QPair<QString, QString>& param);
    bool CheckAndSetComboboxes(const QPair<QString, QString>& param);
    bool CheckAndSetConditionalOperator(const QPair<QString, QString>& param);
    bool CheckAndSetParameters(const QPair<QString, QString>& param);
    bool setCheckboxFromString(QCheckBox* checkbox, QString booleanStr);
    bool setComboboxFromString(QComboBox* combobox, QString booleanStr);

    Ui::GTAWaitUntilActionWidget *ui;
    bool _isParameterLE;
    QHash<QPushButton* ,QLineEdit*> _SearchPBEditMap;
    QStringList _FunctionalStatus;


private slots:
    void onSearchPBClicked();
    void operatorChanged(QString isOperator);
    void disableValueEditsOnRefresh(bool iStatus);
    void disableValueEdits(bool iStatus);
    void resetCheckFS(bool iStatus);
    void enableValueEdits(bool iStatus);
    void hideNote(bool iStatus);
    void resetCheckOnlyValue(bool iStatus);
    void resetSDI(bool iStatus);
    void resetParity(bool iStatus);
    void resetRefreshRate(bool iStatus);
    void addLoopSampling(bool );
	void evaluateParamMedia(const QString &);

private:
    QString getParameter() const;
    QString getParameterValue() const;
    QString getOperator() const;
    bool getFSOnlyState()const;
    bool getValueOnlyState()const;
    QString getFunctionalStatus()const;

};

#endif // GTAWAITUNTILACTIONWIDGET_H
