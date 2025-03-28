#ifndef GTASEQUENCER_H
#define GTASEQUENCER_H

#include "GTASequencer_global.h"
#include <GTAUtil.h>
#include <GTAClient.h>

#pragma warning(push, 0)
#include <QStringList>
#include <QObject>
#include <QHash>
#include <QTimer>
#include <QThread>
#include <QDateTime>
#include <QList>
#include <QMap>
#pragma warning(pop)

class GTASequenceThread : public QThread
{
public:
    static void sleep(unsigned long secs){QThread::sleep(secs);}
};

class GTASequencer : public QObject
{

    Q_OBJECT
private:

    GTASequencer(QObject* ipObject = NULL);

public:

    static GTASequencer * getSequencer(QObject* ipObject=0);

    ~GTASequencer();

    void setMultipleLogs(bool isMultipleLogs);
    bool isMultipleLogsEnabled()const;
    void setLogFilePath(const QString &iLogFilePath);
    QString getLogFilePath()const;
    QString getScxmlPath()const;
    void setScxmlPath(const QString &iScxmlPath);
    void setSchedulerPath(const QString &iSchedulerPath);
    QString getSchedulerPath()const;
    void setInitScripts(const QStringList &iInitScriptLst);
    void setTools(const QStringList &iToolLst);
    void setTimeout(const int &iVal);
    int getTimeout()const;

    QStringList getInitScripts()const;
    QStringList getTools()const;
    void setInitTime(const int &iInitTime);
    int getInitTime()const;

    void setInitCheck(bool iInitCheck);
    bool getInitCheck()const;

    bool executeFile(QString scxmlFile,QString logFile);
    void startScripts();
    void startTools();

    void setGeneratedScxmlFiles(const QStringList &iGeneratedFiles);
    QStringList getGeneratedScxmlFiles()const;
    void setLogNameLst(const QStringList &iLogNameLst);
    QStringList getLogNameLst()const;
    void setFailedList(const QStringList &iFailedLst);
    QStringList getFailedList()const;

    void setProcList(const QStringList &iProcLst);
    QStringList getProcList()const;

    bool isSchedulerRunning()const;

    /**
      * This function sets the QMAP of connected slaves alias names and IP's and GTAClient objects
      * @islaveList:  Connected slaves alias names
      * @islaveClientList: Connected slaves GTAClient objects
      */
    void setSlaveList(const QMap<QString,QString> &islaveList, const QList<GTAClient *> &islaveClientList);

    /**
      * This function gets the connected slaves alias names and IP's
      * @return: returns QMAP of connected slaves alias names and IP's
      */
    QMap<QString,QString> getSlaveList()const;

    /**
      * This function gets the connected slaves GTAClient objects
      * @return: returns the list of connected slaves GTAClient objects
      */
    QList<GTAClient *> getSlaveClientList() const;

    /**
      * _clientRow: to traverse through all the client objects and send request one after the other
      */
    int _clientRow;

signals:
    void log(const QString &iMessage);
    void showProgressBar();
    void hideProgressBar();
    void fileExecuted(const QString &iFileName, const QString &iStatus);
    void processingComplete();
//    void sequencingComplete(const QMap<QString,QString> &iLogStatusLst);



private:
    bool isSchedulerValid();
    void fetchFile();
    void initialise();
    void flushOldLogs();
    void writeSequenceLog();
    void killScheduler();
    void executeNextAndStopOnComplete();
    QString getCurrentProcedure();
    void checkIfToolsAreRunning();
    /**
      * startCall_from: To have the track from where the start() is called, to perform remaining functionality after making callsto sequencer slaves.
      */
    enum startCall_from{START,REPLAY,EXECUTENEXTANDSTOPONCOMPLETE};
    startCall_from _startCallFrom;

public slots:
    void onTimeOut();
    void start();
    void stop();
    void replay();
    void replayAll();
    void stopTools();
    void onSchedulerTimeOut();

    /**
      * This function kills the running list of scripts.
      */
    void stopScripts();


private slots:
    void onSleepTimeout();

    void OnCheckIfToolsRunningOnSlave(QVariant &iVal, QNetworkReply * );
    void OnServerConnectionFailure(int , const QString &, QNetworkReply * );
    void onStartToolsOnSlave(QVariant & iToolLog);
    void onStartScriptsOnSlave(QVariant & iScriptLog);
    void onStopScriptsOnSlave();
    void onStopToolsOnSlave();
    void onStopToolsScripts();
    void OnStopToolsScriptsFailure(int ,const QString&);

private:
    QString _logFilePath;
    QString _scxmlPath;
    QString _schedulerPath;
    QStringList _initScriptList;
    QStringList _toolList;
    int _initTime;        //stored as secs. sleep application for this time interval
    long int _timeout;         //stored as msces. scheduler timeout
    bool _isMultipleLogs;
    bool _isLogFileComplete;
    bool _initCheck;
    QStringList _killList;
    QStringList _killScriptsList;
    QTimer *_pTimer;
    QTimer *_pSchedulerTimer;


    QStringList _generatedScxml;
    QStringList _logNameLst;
    QString _monitorLogFile;
    QStringList _failedList;
    QStringList _procList;
    int _iterator;
    static GTASequencer *_pSequencer;
    const int _timerTime;                   // time interval to check if logs have been written completely. This is to determine if procedure is executed
    bool _isReplayOne;
    qint64 _schedulerPid;

    //<status,logFilePath>
//    QMap<QString,QString> _logStatusLst;
    bool _currentExecutionStatus;

    QStringList _ExecutionLst;
    QString _globalStatus;

    QMap<QString,QString> _slaveList;
    QList<GTAClient *> _slaveClientList;

};

#endif // GTASEQUENCER_H
