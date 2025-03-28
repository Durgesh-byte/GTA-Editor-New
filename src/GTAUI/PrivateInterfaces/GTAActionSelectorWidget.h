#ifndef GTAACTIONSELECTORWIDGET_H
#define GTAACTIONSELECTORWIDGET_H

#include <QDialog>
#include <QKeyEvent>
#include <QTextEdit>
#include <QLineEdit>
#include <QLabel>

#include "GTADumpListWidget.h"
#include "GTAConditionActionWidget.h"
#include "GTAActionBase.h"
#include "GTAActionCmdCommentWidget.h"
#include "GTACmdRequirementWidget.h"
#include "GTAActionWait.h"

class GTAGenSearchWidget;
class GTAActionBase;

namespace Ui {
    class GTAActionSelectorWidget;
}

class GTAActionSelectorWidget : public QDialog
{
    Q_OBJECT

public:
    explicit GTAActionSelectorWidget(QWidget *parent = 0);
    ~GTAActionSelectorWidget();

    void setCommandList(const QHash<QString,QStringList> & iCommandList,QStringList iOrder = QStringList());

    void setActionCommand(const GTAActionBase * ipActionCmd,QString iaction=QString(),QString icomplement=QString());
    GTAActionBase* getActionCommand() { return _pActionCmd; };
    QVariant getActionVariant();
    //This should be generalized for all commands. 
    //This function will only set corresponding value to its widget.
    void setCallCommandonDropEvent(QString& iFilename,QString &FileUUID);
    void setEditableAction(bool isEditable);
    void clickOk();
    void clickCancel();
    GTAActionWidgetInterface* getCurrentWidget();
    bool setActionSelectorWidgetFields(const QList<QPair<QString, QString>>& actionParams);
    QString getLastErrorAndResetIt();
    void setShowErrorBoxes(bool showErrorBoxes) { _showErrorBoxes = showErrorBoxes; };
    void scheduleReopenWindow() {
        _reopenScheduled = true;
    }

    void setActionCB(int idx);

public slots:
    void onActionChange(const QString& actionName);

signals:
    void okPBClicked();
    void setParentFocus();
    void reopenWindowScheduled();
private slots:
    void onReEvaluateShowItems(bool iActionOnFail, bool iPrecesion, bool iTimeOut, bool iConfTime, bool iDumpList,bool iExternalComment);
    
    /**
      * This function set the command widget and show/hide the common attributes of widget
      * arg1: the name of complement, if it empty no action take place
      */
    void onComplementChange(const QString &arg1);

    void onOKClick();
    void onCancelPBClick();

    void onSearchPBClick(QLineEdit *& ipItemToUpdate);
    void onSearchOKPBClick();
    void togglePrecision(QString);
    void resetPrecision(QString);
    void onDisablePrecision(bool &iStatus);
    void setTabOrderForWidgets(QList<QWidget*> iLstWgt)const;
    void setDumpPBStatus(QString iActFail);
    void closeEvent(QCloseEvent *event);
	void evaluateConfTime(const QString &);
    void externalActionCheckboxClicked();
    void showOrHideExternalActionButtons();
    void setExternalActionProperties();

protected:
    void keyPressEvent ( QKeyEvent * event );
private:
    
    void resetWidgets();
  
    Ui::GTAActionSelectorWidget *ui;


    QMap<QString,QStringList> _ActionList;

    
    
    QLineEdit                                   * _pCurrentSearchResult;
    GTAGenSearchWidget                       * _pSearchWidget;
    GTAActionCmdCommentWidget                * _pCommentWidget;
    GTAActionBase                            * _pActionCmd;
    GTADumpListWidget                        * _pDumpLstWdgt;  
    GTACmdRequirementWidget                  * _pRequirementWdgt;

    QVariant _actionCmdVariant;
    QString _CurrentCmdId;
    QString _lastError;
    bool _showErrorBoxes = true;
    bool _isInputForInnerCall = false;

    void showCommonItem(bool iActionOnFail = false, bool iPrecesion = false, bool iTimeOut = false, bool iConfTime = false,bool iDumpList = false, bool iCallInput = false);
    void deleteWidgetsInStack();

protected:
    bool _isNewCmd;
    bool _reopenScheduled = false;
};

#endif // GTAACTIONSELECTORWIDGET_H
