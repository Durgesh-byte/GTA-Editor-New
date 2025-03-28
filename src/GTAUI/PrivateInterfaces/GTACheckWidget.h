#ifndef GTACHECKWIDGET_H
#define GTACHECKWIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QHash>
#include <QStringList>
#include <QLineEdit>
#include <QPushButton>
#include "GTACheckBase.h"
#include "GTACheckWidgetInterface.h"
#include "GTACmdRequirementWidget.h"

class GTACheckAudioWidget;
class GTACheckECAMDisplayWidget;
class GTACheckFWCProcedureWidget;
class GTACheckFWCWarningWidget;
class GTACheckValueWidget;
class GTACheckBiteMessageWidget;
class GTAGenSearchWidget;
class GTAActionCmdCommentWidget;
class GTADumpListWidget;
class GTACheckRefreshWidget;
namespace Ui {
class GTACheckWidget;
}

class GTACheckWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GTACheckWidget(QWidget *parent = 0);
    ~GTACheckWidget();

    void setCheckCommand(const GTACheckBase * ipCheckCmd,QString ichecktype=QString());
    void setCheckId();
    GTACheckBase* getCheckCommand();
    QVariant getCheckVariant();
    void setEditableAction(bool isEditable);
	void setChecksList(const QList<QString> &iCheckList);
    void clickOk();
    void clickCancel();
    GTACheckWidgetInterface* getCurrentWidget();
    bool setCheckControlWidgetFields(const QList<QPair<QString, QString>>& checkParams);
    void selectLastEditedRow();

    void scheduleOpenActionWindow() {
        _openActionWindowScheduled = true;
    }

signals:
    void okPressed();
    void setParentFocus();
    void openActionWindowScheduled();

protected:
    void keyPressEvent ( QKeyEvent * event );

private:
    Ui::GTACheckWidget *ui;
    QHash<QPushButton*,QLineEdit*> _SearchButtonEditorMap;
    GTACheckAudioWidget          * _pAudioWgt;
    GTACheckECAMDisplayWidget    * _pEcamDispWgt;
    GTACheckFWCWarningWidget     * _pFwcWarnWgt;
    GTACheckFWCProcedureWidget   * _pFwcProcWgt;
    GTACheckValueWidget          * _pValueWgt;
    GTACheckBiteMessageWidget    * _pBiteMsgWgt;
    QTextEdit                       * _pDisplayTE;
    GTAActionCmdCommentWidget    * _pCommentWgt;
    GTADumpListWidget            * _pDumpLstWdgt;
    GTACheckRefreshWidget        * _pRefreshWdgt;
    GTACmdRequirementWidget      * _pRequirementWdgt;

    QHash<QString,QStringList> _CheckOptionTable;
    QHash<QString,QStringList> _CheckOptionValueTable;

    //QString                    _CheckCommand;

    GTAGenSearchWidget           * _pSearchWidget;
    GTAGenSearchWidget           * _pSearchWidget1;
    QLineEdit                       * _pCurrentSearchResult;

    GTACheckBase                 * _pCheckCommand;

    QString getCheck() const;
    QString getTimeOut() const;
    QString getPrecision() const;
    QString getActionOnFail() const;
    QString getDisplay() const;

    QVariant _checkCmdVariant;

    QMap<QString, GTACheckWidgetInterface*> _CheckWidgetMap;
    QMap<GTACheckBase::CheckType,QString> _CheckEnumToStringMap;
    QString _CurrentCmdId;

private slots:

    void onCheckSelection(const QString & iSelectedCheck);
    void onOKPBClicked();
    

    void onSearchPBClick(QLineEdit *& ipItemToUpdate);
    void onSearchTimePBClick();
    void onSearchOKPBClick();
    void resetWidget();

    void showCommonItem(bool iActionOnFail = false, bool iPrecesion = false, bool iTimeOut = false, bool = false, bool iConfTime =false );
    void setTimeOutStatus(bool iStatus);
    void setDumpPBStatus(QString iActFail);
    void closeEvent(QCloseEvent *event);
	void evaluateConfTime(const QString &);

    void externalActionCheckboxClicked();
    void setExternalActionProperties(GTACheckBase* checkCmd);

protected:
    bool _isNewCmd;
    bool _openActionWindowScheduled = false;
};

#endif // GTACHECKWIDGET_H
