#ifndef GTASEQUENCERMAINWINDOW_H
#define GTASEQUENCERMAINWINDOW_H

#include "GTASequencer.h"
#include "GTASequencerSettings.h"
#include "GTAClient.h"
#include "GTAServer.h"

#pragma warning(push, 0)
#include <QMainWindow>
#include <QAction>
#include <QThread>
#include <QProgressBar>
#include <QLabel>
#pragma warning(pop)

namespace Ui {
class GTASequencerMainWindow;
}

class  GTASequencerMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit GTASequencerMainWindow(const QString &iToolDataPath, GTASequencer *pSequencer,QWidget *parent = 0);
    ~GTASequencerMainWindow();

    //    void show();

private:
    void initToolBar();
    void initialise();
    void enableDisableToolButtons(bool iVal);
    void startButtonClick();


private slots:
    void onSettingsClicked();
    void onStartClicked();
    void onStopClicked();
    void onReplayClicked();
    void onReplayAllClicked();
    void startSequencer();
    void stopSequencer();
    void onOpenUserGuide();
    void ServerClickSlot();
    void ClientClickSlot();


public slots:


    void onSettingsApplied(const SequencerSettings &iSetting);
    //    void onSettingsApplied(const QString &ischedulerPath,const int &iWaitTime,const QStringList &iScriptList,
    //                         const QStringList &iToolList, const QString &iLogPath, const QString &iScxmlPath, const int &iTimeout);
    void onLogReceived(QString &iMessage);
    void onHideProgressBar();
    void onShowProgressBar();
    void onFileExecuted(const QString &iFileName, const QString &iStatus);
    void onProcessingComplete();
    void onCloseSequencer(bool &oRetVal);
    void close();

    void nix();
    QVariant birne(int x);
    void testResponse(QVariant &arg);

protected:
    void closeEvent(QCloseEvent *event);

signals:
    void sequencerWindowClosed();
//    void sequencingComplete(const QDateTime &iTimeStopped, const QMap<QString,QString> &iLogStatusLst);

private:
    Ui::GTASequencerMainWindow *ui;
    QAction *_pStart;
    QAction *_pStop;
    QAction *_pReplay;
    QAction *_pReplayAll;
    QProgressBar *_pProgressBar;
    QLabel *_pProgressBarText;
    GTASequencer *_pSequencer;
    GTASequencerSettings *_pSettings;
    GTAServer *_server;
    GTAClient *_client;
    //    QThread _sequencerThread;

};

#endif // GTASEQUENCERMAINWINDOW_H
