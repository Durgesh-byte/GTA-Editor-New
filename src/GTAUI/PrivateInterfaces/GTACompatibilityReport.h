#ifndef GTACOMPATIBILITYREPORT_H
#define GTACOMPATIBILITYREPORT_H

#include <QWidget>
#include <QDialog>
#include <QList>
#include <QMenu>
#include <QAction>
#include "GTAAppController.h"
#include <QCloseEvent>

namespace Ui {
    class GTACompatibilityReport;
}

class GTACompatibilityReport : public QWidget
{
    Q_OBJECT

public:

    enum ReportMode { };
    explicit GTACompatibilityReport(QWidget *parent = 0);
    ~GTACompatibilityReport();


    void setDialogName(const QString& iName);
    bool getCancelStatus();
    void setLog(const ErrorMessageList &iErrorList, bool overwriteExistingLogs = false);
    QString getLogList() const;
    QString getCsvToProLogList() const;
    void setText(QString message, ErrorMessage::ErrorType errType = ErrorMessage::ErrorType::kNa,ErrorMessage::MsgSource msgType = ErrorMessage::MsgSource::kNoSrc);
    void onSaveLogCsvConversion(const QString proName, ErrorMessageList& errLogs, bool isConsole = false);

private:
    QString getErrorType(const ErrorMessage::ErrorType type)const;
    QString getSourceType(const ErrorMessage::MsgSource iType)const;
    void createContextMenu();

public slots:
    void onSave();
    void show();
    void onResolveClicked();
    void onResolveAllClicked();
    void onClearLog();
    void onLogFilterChanged();
    void setLogLevel(const uint& pos);

private slots:
    void onLogTableClicked(QModelIndex iIndex);
    void displayContextMenu(const QPoint &iPos);
    void closeEvent(QCloseEvent *event);

    /**
      * This function provides Filter/search list on log window based on column type
      * @return: void, this function calls to setLog with search List
      */
    void onSearchTBClicked();

private:
    QString _ReportText;
    Ui::GTACompatibilityReport *ui;
    GTAAppController* _pAppController;
    bool _isCancel;
    QList<ErrorMessage> _Logs;
    QMenu * _pContextMenu;
    QAction *_pActionResolve;
    QAction *_pActionResolveAll;
    QAction *_pActionSaveLog;
    QAction *_pActionClearLog;

public :
    QString _logLevel;

signals:
//    void onHide();
    void jumpToElement(const QString &iRelativeFilePath, const int &iLineNumber,GTAAppController::DisplayContext iDisplayContext,bool isUUID = false);
//    void jumpToElement(const QString &iRelativeFilePath, const int &iLineNumber, );

};

#endif // GTACOMPATIBILITYREPORT_H
