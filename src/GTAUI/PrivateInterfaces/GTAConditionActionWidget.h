#ifndef GTACONDITIONACTIONWIDGET_H
#define GTACONDITIONACTIONWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QHash>
#include <qcheckbox.h>
#include <qcombobox.h>
#include "GTAActionBase.h"
#include "GTAActionWidgetInterface.h"

#include "GTAActionIF.h"
#include "GTAActionWhile.h"
#include "GTAActionDoWhile.h"
#include "GTAActionForEach.h"


class GTAActionSelectorWidget;
namespace Ui {
class GTAConditionActionWidget;
}
class GTAActionCondition;
class GTAActionIF;
class GTAConditionActionWidget : public GTAActionWidgetInterface
{
    Q_OBJECT

public:
    explicit GTAConditionActionWidget(QWidget *parent = 0);
    ~GTAConditionActionWidget();



    void clear();
    bool isValid()const ;
    void setActionCommand(const GTAActionBase* ipActionCmd);
    bool getActionCommand(GTAActionBase*& pCmd)const;

    bool hasSearchItem();
    bool hasActionOnFail();
    bool hasPrecision();
    bool hasTimeOut(int& oValidation);
    bool hasConfigTime();
    bool hasDumpList();
    void setComplement(const QString& iComplement);
    void processSearchInput(const QStringList&);
    int getSearchType();

    bool getCheckOnlyValueState()const;
    bool getCheckOnlyFSState() const;
    QString getParameter() const;
    QString getParameterValue() const;
    QString getConditionOperator() const;
    bool getChkAudioAlarm() const;
    QString getHeardAfter() const;
    QString getFunctionalStatus()const;
    bool isParameterLESelected()const;
    bool hasCondition();
//    bool getCheckRepeatedLoggingState() const;

    QString getParity()const;
    QString getSDI()const;
    bool getCheckOnlyRefreshRateState()const;
    bool getCheckOnlySDIState()const;
    bool getCheckOnlyParityState()const;
	void setOnlyValueStatus();
	void resetAllCheckBoxStatus();

    bool setWidgetFields(const QList<QPair<QString, QString>>& params, ErrorMessageList& errorLogs) override final;

    bool isAutoAddElseChecked() const;
signals:

    void conditionOperatorChanged(QString);
//    void disablePrecision(bool);

private:
    Ui::GTAConditionActionWidget *ui;
    bool _isParameterLE;
    QHash<QPushButton* ,QLineEdit *> _SearchPBEditMap;

    bool createNestedCommandForChannels(GTAActionIF* iPCmd , QString iParameter, QString iParameterVal )const;
    bool createNestedCommandForChannels(GTAActionWhile* iPCmd , QString iParameter, QString iParameterVal )const;
    bool createNestedCommandForChannels(GTAActionDoWhile* iPCmd , QString iParameter, QString iParameterVal )const;

private slots:
    void onSearchPBClicked();
    void operatorChanged(QString isOperator);
    void checkAudioAlarmToggeled(bool isCheckAudioAlarm);
//    void checkRepeatedLoggingToggeled(bool isCheckRepLog);
    void disableValueEditsOnRefresh(bool iStatus);
    void disableValueEdits(bool iStatus);
    void resetCheckFS(bool iStatus);
    void enableValueEdits(bool);
    void hideNote(bool);
    void resetCheckOnlyValue(bool iStatus);
    void resetSDI(bool iStatus);
    void resetParity(bool iStatus);
    void resetRefreshRate(bool iStatus);
	
	    /**
   * This function shows/Hides the relative path selection for table files if iVAl is true/false
   */
    void showForEachCSVFiles(bool iVal);
    void forEachRelativePathSelected(const QString& relativePath);
	void evaluateParamMedia(const QString &);

private:
    bool setForEachPath(const QList<QPair<QString, QString>>& params, ErrorMessageList& errorLogs);
    bool setForEachWidget(const QList<QPair<QString, QString>>& params, ErrorMessageList& errorLogs);
    void checkPath(QString path);
    bool checkIfPathIsRelative(const QList <QPair<QString, QString>>& params, ErrorMessageList& errorLogs);
    bool CheckAndSetCheckboxes(const QPair<QString, QString>& param, ErrorMessageList& errorLogs);
    bool CheckAndSetComboboxes(const QPair<QString, QString>& param, ErrorMessageList& errorLogs);
    bool CheckAndSetConditionalOperator(const QPair<QString, QString>& param, ErrorMessageList& errorLogs);
    bool CheckAndSetParameters(const QPair<QString, QString>& param, ErrorMessageList& errorLogs);
    bool setCheckboxFromString(QCheckBox* checkbox, QString booleanStr, ErrorMessageList& errorLogs);
    bool setComboboxFromString(QComboBox* combobox, QString booleanStr, ErrorMessageList& errorLogs);

    QString _complement;
    GTACommandBase* _pCurrentCommand;
    QStringList _FunctionalStatus;
};

#endif // GTACONDITIONACTIONWIDGET_H

