#include "GTASequencer.h"
#include "GTASequencerMainWindow.h"
#include "Windows.h"
#include "process.h"

#pragma warning(push, 0)
#include <tlhelp32.h>
#include <QFile>
#include <QFileDialog>
#include <QProcess>
#include <QDir>
#include <QTimer>
#include <QTextStream>
#include <QRegExp>
#include <QStringMatcher>
#include <QFuture>
#include <QtConcurrentRun>
#pragma warning(pop)



GTASequencer* GTASequencer::_pSequencer = NULL;

GTASequencer::GTASequencer(QObject* ipObject):QObject(ipObject),_timerTime(1000 * 2)
{

    _iterator = 0;
    _isLogFileComplete = false;
    _pTimer = new QTimer(this);
    _pSchedulerTimer = new QTimer(this);
    //    _isRunning = false;
    _schedulerPid = 0;
    _isReplayOne = false;
    connect(_pTimer,SIGNAL(timeout()),this,SLOT(onTimeOut()));
    connect(_pSchedulerTimer,SIGNAL(timeout()),this,SLOT(onSchedulerTimeOut()));
}

GTASequencer::~GTASequencer()
{

    if(_pTimer != NULL)
    {
        delete _pTimer;
        _pTimer = NULL;
    }
    if(_pSequencer != NULL)
    {
        delete _pSequencer;
        _pSequencer = NULL;
    }
    if(_pSchedulerTimer != NULL)
    {
        delete _pSchedulerTimer;
        _pSchedulerTimer = NULL;
    }
    if(!_slaveClientList.isEmpty())
    {
        qDeleteAll(_slaveClientList.begin(),_slaveClientList.end());
        _slaveClientList.clear();
    }
}

GTASequencer * GTASequencer::getSequencer(QObject* ipObject)
{
    if(_pSequencer != NULL)
        return _pSequencer;
    else
    {
        _pSequencer = new GTASequencer(ipObject);
        return (_pSequencer);
    }
}

void GTASequencer::setFailedList(const QStringList &iFailedLst)
{
    _failedList = iFailedLst;
}

QStringList GTASequencer::getFailedList()const
{
    return _failedList;
}

void GTASequencer::setProcList(const QStringList &iProcLst)
{
    _procList = iProcLst;
}

QStringList GTASequencer::getProcList()const
{
    return _procList;
}

void GTASequencer::setGeneratedScxmlFiles(const QStringList &iGeneratedFiles)
{
    _generatedScxml = iGeneratedFiles;
}

QStringList GTASequencer::getGeneratedScxmlFiles()const
{
    return _generatedScxml;
}

void GTASequencer::setLogNameLst(const QStringList &iLogNameLst)
{
    _logNameLst = iLogNameLst;
}

QStringList GTASequencer::getLogNameLst()const
{
    return _logNameLst;
}

void GTASequencer::setMultipleLogs(bool isMultipleLogs)
{
    _isMultipleLogs = isMultipleLogs;
}

bool GTASequencer::isMultipleLogsEnabled()const
{
    return _isMultipleLogs;
}

void GTASequencer::setLogFilePath(const QString &iLogFilePath)
{
    _logFilePath = iLogFilePath;
}


QString GTASequencer::getLogFilePath()const
{
    return _logFilePath;
}

void GTASequencer::setScxmlPath(const QString &iScxmlPath)
{
    _scxmlPath = iScxmlPath;
}

QString GTASequencer::getScxmlPath()const
{
    return _scxmlPath;
}

void GTASequencer::setSchedulerPath(const QString &iSchedulerPath)
{
    _schedulerPath = iSchedulerPath;
}

QString GTASequencer::getSchedulerPath()const
{
    return _schedulerPath;
}

void GTASequencer::setInitScripts(const QStringList &iInitScriptLst)
{
    _initScriptList = iInitScriptLst;
}

void GTASequencer::setTimeout(const int &iVal)
{
    _timeout = iVal * 1000;
}

void GTASequencer::setTools(const QStringList &iToolLst)
{
    _toolList = iToolLst;
}

QStringList GTASequencer::getInitScripts()const
{
    return _initScriptList;
}

QStringList GTASequencer::getTools()const
{
    return _toolList;
}

void GTASequencer::setInitTime(const int &iInitTime)
{
    _initTime = iInitTime;
}

int GTASequencer::getInitTime()const
{
    return _initTime;
}


void GTASequencer::setInitCheck(bool iInitCheck)
{
    _initCheck = iInitCheck;
}

bool GTASequencer::getInitCheck()const
{
    return _initCheck;
}

int GTASequencer::getTimeout()const
{
    return _timeout;
}

void GTASequencer::startScripts()
{
    foreach(QString file,_initScriptList)
    {
        QFile File(file);
        if(File.exists())
        {

            QStringList arguments;
            
            QString cmd;
            QDir::cleanPath(file);

            if(file.endsWith(".py"))
            {
                QStringList pathLst = file.split("/");
                QString fileName = pathLst.takeLast();
                QString workingDir = pathLst.join("/");

                cmd = QString("python");
                arguments << fileName;
                qint64 pid;
				// Static member function
				QProcess::startDetached(cmd, arguments, workingDir, &pid);
                QString killStmt = QString("taskkill /F /PID %1").arg(QString::number(pid));
                _killScriptsList.append(killStmt);

            }
            else if(file.endsWith(".bat"))
            {
                QStringList pathLst = file.split("/");
                pathLst.removeLast();
                QString workingDir = pathLst.join("/");
                qint64 pid;
				// Static member function
				QProcess::startDetached(file, arguments, workingDir, &pid);
                QString killStmt = QString("taskkill /F /T /PID %1").arg(QString::number(pid));
                _killScriptsList.append(killStmt);
            }

            emit log(QString("Starting script %1").arg(file));
        }
        else
        {
            emit log(QString("Could not start the script as the path (%1) was not found.").arg(file));
        }
    }
}

void GTASequencer::startTools()
{
    foreach(QString file,_toolList)
    {

        QFile File(file);
        if(File.exists())
        {
            QStringList pathLst = file.split("/");
            pathLst.removeLast();
            QString workingDir = pathLst.join("/");
            qint64 pid = 0;
            QStringList arguments;
			QString killStmt = QString("taskkill /F /PID %1").arg(QString::number(pid));

			// Static member function
			QProcess::startDetached(file, arguments, workingDir, &pid);
            _killList.append(killStmt);
            emit log(QString("Starting tool %1").arg(file));
        }
        else
        {
            emit log(QString("Could not start the tool as the path (%1) was not found").arg(file));
        }
    }

}

void GTASequencer::stopTools()
{
    if(!_slaveClientList.isEmpty())
    {
        foreach(GTAClient * client, _slaveClientList)
        {
            QVariantList args;
            const char *slot = SLOT(onStopToolsScripts());
            const char *faultSlot = SLOT(OnStopToolsScriptsFailure(int,const QString &));
            client->sendRequest(args,"stopTools",this,slot,this,faultSlot);
            client->sendRequest(args,"stopScripts",this,slot,this,faultSlot);
        }
    }
    foreach(QString killCmd, _killList)
    {
        QByteArray ba = killCmd.toLocal8Bit();
        const char *c_str2 = ba.data();
        system(c_str2);
    }
    //removing all tools from list
    _killList.clear();

    stopScripts();
}

void GTASequencer::onStopToolsScripts()
{
    qDebug()<<"onStopToolsScripts";
}

void GTASequencer::OnStopToolsScriptsFailure(int ,const QString&)
{
    qDebug()<<"OnStopToolsScriptsFailure";
}

void GTASequencer::start()
{
    _globalStatus = "OK";
    _killList.clear();
    _ExecutionLst.clear();
    _isReplayOne = false;

    QString sMultipleLog = QString("$$Multiple_Logs$$");
    _ExecutionLst.append(sMultipleLog);

    flushOldLogs();
    if(_slaveClientList.isEmpty())
    {
        initialise();

        if(isSchedulerValid())
        {
            _iterator = 0;
            fetchFile();
        }
        else
        {
            QString msg("Error: Scheduler Executable file is missing, Execution failed");
            emit log(msg);
        }
    }
    else
    {
        initialise();
        _clientRow = 0;
        _startCallFrom = START;
        checkIfToolsAreRunning();
    }

}

void GTASequencer::flushOldLogs()
{
    foreach(QString log, _logNameLst)
    {
        QString logFilePath = _logFilePath;
        logFilePath.append(QString("/%2").arg(log));
        QFile file(logFilePath);
        if(file.exists())
        {
            file.remove();
        }
    }
}

void GTASequencer::fetchFile()
{
    if(!_generatedScxml.isEmpty() && !_logNameLst.isEmpty() && _iterator < _generatedScxml.count()
            && _iterator < _logNameLst.count() )//&& _isRunning)
    {
        //        _iterator->next();
        QString scxmlFile = _generatedScxml.at(_iterator);
        QString logFile = _logNameLst.at(_iterator);
        executeFile(scxmlFile,logFile);
        _iterator++;
    }
}

bool GTASequencer::isSchedulerValid()
{
    if(QFile::exists(_schedulerPath))
    {
        return true;
    }
    return false;
}


bool GTASequencer::executeFile(QString scxmlFile,QString logFile)
{
    bool rc = false;
    _currentExecutionStatus = true;
    _schedulerPid = 0;
    _isLogFileComplete = false;
    if (QFile::exists(scxmlFile))
    {

        QString logFilePath = _logFilePath;
        logFilePath.append(QString("/%2").arg(logFile));
        _monitorLogFile = logFilePath;

        logFilePath.prepend("\"");logFilePath.append("\"");
        logFilePath.prepend("-l ");
        scxmlFile.prepend("\"");scxmlFile.append("\"");

        QStringList argument;
        QString path = _schedulerPath;

        QStringList split = path.split("/");
        split.removeLast();
        path = split.join("/");

        QString confFilePath = QString("-c \"%1/conf/default.conf\"").arg(path);

        if(logFilePath.startsWith("\\\\"))
        {
            logFilePath = QDir::cleanPath(logFilePath);
            logFilePath.prepend("\\");
        }
        else
        {
            logFilePath = QDir::cleanPath(logFilePath);

        }

        if(scxmlFile.startsWith("\\\\"))
        {
            scxmlFile = QDir::cleanPath(scxmlFile);
            scxmlFile.prepend("\\");
        }
        else
        {
            scxmlFile = QDir::cleanPath(scxmlFile);
        }

        argument << logFilePath << scxmlFile << confFilePath;
        QString sSchedulerCommand = QString("%1 %2").arg(_schedulerPath,argument.join(" "));
        QStringList arguments;

		// Static member function
		QProcess::startDetached(sSchedulerCommand, arguments, path, &_schedulerPid);

        if(_schedulerPid > 0 && _pTimer != NULL)
        {
            _pTimer->start(_timerTime);
            _pSchedulerTimer->start(_timeout);
        }

    }
    return rc;
}

void GTASequencer::onSchedulerTimeOut()
{
    _pTimer->stop();
    _pSchedulerTimer->stop();
    killScheduler();

    if(!_isLogFileComplete)
    {

        QString procName = getCurrentProcedure();
        _globalStatus = "KO";
        emit fileExecuted(procName,"KO");
        emit log(QString("Stopping scheduler as scheduler not responding.\nWarning : Current procedure(%1) may not be executed correctly. Continuing with next procedure...").arg(procName));

    }

    GTASequenceThread::sleep(3);
    executeNextAndStopOnComplete();
}


void GTASequencer::initialise()
{
    stopTools();
    startScripts();
    startTools();

    if(_slaveClientList.isEmpty())
    {
        GTASequenceThread::sleep(_initTime);
    }

}

void GTASequencer::onSleepTimeout()
{
    QTimer *pTimer = dynamic_cast<QTimer *>(sender());
    if(pTimer)
    {
        pTimer->stop();
        emit hideProgressBar();
    }
}

void GTASequencer::onTimeOut()
{
    QFile file(_monitorLogFile);
    if(file.exists())
    {
        QTextStream stream(&file);

        //        QTextStream stream(file,QIODevice::ReadOnly);
        if(file.open(QFile::ReadOnly))
        {
            QString line;
            while(!stream.atEnd())
            {
                line = stream.readLine();
                if(!line.isEmpty())
                {
                    QStringMatcher statusMatcher("KO");
                    if((statusMatcher.indexIn(line) == -1) && (_currentExecutionStatus == true)) // if line does not contain KO
                    {
                        _currentExecutionStatus &= true;                   // _currentExecutionStatus = true means OK
                    }
                    else
                    {
                        _globalStatus = "KO";
                        _currentExecutionStatus &= false;                   // _currentExecutionStatus = false means KO
                    }
                }
            }

            if(!line.isEmpty())
            {

                QStringMatcher matcher("End_procedure");
                if(matcher.indexIn(line) != -1)
                {

                    _pTimer->stop();
                    _pSchedulerTimer->stop();

                    _isLogFileComplete = true;
                    QString sStatus = (_currentExecutionStatus == true) ? "OK" : "KO";

                    QString procName = getCurrentProcedure();
                    emit fileExecuted(QString("Procedure - %1 (Log - %2)").arg(procName,_monitorLogFile),sStatus);


                    //creating log messages for sequence's log file in case of multipleLogs enabled
                    if(_isMultipleLogs)
                    {
                        QString logMessage;                                 //
                        logMessage = QString("%1:;:%2:;:%3").arg(procName,_monitorLogFile,sStatus);
                        _ExecutionLst.append(logMessage);
                    }


                    executeNextAndStopOnComplete();
                }
                else
                {
                    _pTimer->start(_timerTime);
                    _isLogFileComplete = false;

                }
            }
        }
    }
}


QString GTASequencer::getCurrentProcedure()
{
    QString procName;
    int index = _iterator - 1;
    if(index >= 0 && index < _procList.count()-1)
    {
        procName = _procList.at(index);  // procName contains relativePath
    }
    return procName;
}

void GTASequencer::executeNextAndStopOnComplete()
{
    if(_iterator < _generatedScxml.count() && _iterator < _logNameLst.count() && !_isReplayOne)
    {
        if(_initCheck)
        {
            if(_slaveClientList.isEmpty())
            {
                initialise();
                fetchFile();
            }
            else
            {
                initialise();
                _clientRow = 0;
                _startCallFrom = EXECUTENEXTANDSTOPONCOMPLETE;
                checkIfToolsAreRunning();
            }
        }
        else
            fetchFile();

    }
    else
    {
        _schedulerPid = 0;
        stopTools();
        emit processingComplete();
        //emit sequencingComplete(_logStatusLst);

        //writing in sequence's log.
        if(_isMultipleLogs && (_ExecutionLst.count() > 0))
        {
            writeSequenceLog();
        }

    }
}

void GTASequencer::writeSequenceLog()
{
    QString sequenceName  = _procList.last();
    sequenceName = sequenceName.mid(0,sequenceName.lastIndexOf("."));
    QString logFilePath = _logFilePath;
    logFilePath.append(QString("/%2.log").arg(sequenceName));

    _ExecutionLst.append(QString("GLOBAL_STATUS:;:%1").arg(_globalStatus));
    QString seqLogs = _ExecutionLst.join("\n");
    QFile file(logFilePath);
    if(file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        QTextStream stream(&file);
        stream << seqLogs;
        file.close();
    }
    else
    {
        qDebug() << "Error while opening the file";
    }
}

void GTASequencer::killScheduler()
{
    if(_schedulerPid > 0)
    {
        QString killStmt = QString("taskkill /F /PID %1").arg(QString::number(_schedulerPid));
        QByteArray ba = killStmt.toLocal8Bit();
        const char *c_str2 = ba.data();
        system(c_str2);
        _schedulerPid = 0;
    }
}

void GTASequencer::stop()
{
    if(_pTimer)
    {
        _pTimer->stop();
    }
    if(_pSchedulerTimer)
    {
        _pSchedulerTimer->stop();
    }


    killScheduler();

    QString procName;
    if(_procList.count() > 1)
    {

        emit log(QString("Execution stopped by the user."));

        if(!_isLogFileComplete)
        {
            procName = getCurrentProcedure();
            emit log(QString("Procedure (%1) was stopped mid execution.").arg(procName));
        }


        if(_iterator >=0 && _iterator < _procList.count()-1)
        {
            emit log(QString("Following files have not been executed :-"));
            for(int i = _iterator; i < _procList.count()-1; i++)
            {
                QString proc = _procList.at(i);
                emit log(proc);
            }
        }
    }

    //writing in sequence's log.
    if(_isMultipleLogs && (_ExecutionLst.count() > 0))
    {
        if(!_isLogFileComplete)
        {

            QString logMessage;

            logMessage = QString("%1:;:%2:;:%3").arg(procName,_monitorLogFile,"NA");
            _ExecutionLst.append(logMessage);
            _globalStatus = "KO";
        }

        writeSequenceLog();
    }


    QTimer::singleShot(10,this,SLOT(stopTools()));
}

void GTASequencer::replay()
{
    if(!_generatedScxml.isEmpty() && !_logNameLst.isEmpty() && !_procList.isEmpty() && ((_iterator - 1) < _generatedScxml.count())
        && ((_iterator - 1) < _logNameLst.count()) && ((_iterator - 1) < _procList.count()))
        {

        _isReplayOne = true;
        QString proc = _procList.at(_iterator-1);
        emit log(QString("Replaying procedure (%1)").arg(proc));
        QString scxmlFile = _generatedScxml.at(_iterator-1);
        QString logFile = _logNameLst.at(_iterator-1);

        QString logFilePath = _logFilePath;
        logFilePath.append(QString("/%2").arg(logFile));

        QFile file(logFilePath);
        if(file.exists())
        {
            file.remove();
        }
        if(_slaveClientList.isEmpty())
        {
            initialise();
            executeFile(scxmlFile,logFile);
        }
        else
        {
            initialise();
            _clientRow = 0;
            _startCallFrom = REPLAY;
            checkIfToolsAreRunning();
        }
    }
}

void GTASequencer::replayAll()
{
    emit log(QString("Replaying all procedures"));
    start();
}

bool GTASequencer::isSchedulerRunning()const
{
    if(_schedulerPid > 0)
        return true;
    else
        return false;
}

void GTASequencer::setSlaveList(const QMap<QString, QString> &islaveList, const QList<GTAClient *> &islaveClientList)
{
    _slaveList = islaveList;
    qDeleteAll(_slaveClientList.begin(),_slaveClientList.end());
    _slaveClientList.clear();
    for(int i=0;i<islaveClientList.count();i++)
    {
        GTAClient * client = new GTAClient(*islaveClientList.at(i));
        _slaveClientList.append(client);
    }
}


QMap<QString, QString> GTASequencer::getSlaveList()const
{
    return _slaveList;
}

QList<GTAClient *> GTASequencer::getSlaveClientList()const
{
    return _slaveClientList;
}

void GTASequencer::checkIfToolsAreRunning()
{
    if(_clientRow < _slaveClientList.count())
    {
        QVariantList args;
        const char *slot = SLOT(OnCheckIfToolsRunningOnSlave(QVariant &,QNetworkReply *));
        const char *faultSlot = SLOT(OnServerConnectionFailure(int,const QString &,QNetworkReply *));

        _slaveClientList.at(_clientRow)->sendRequest(args,"checkToolsAreRunning",this,slot,this,faultSlot);
    }
    else
    {

        GTASequenceThread::sleep(_initTime);
        switch(_startCallFrom)
        {

        case START:
            {
                if(isSchedulerValid())
                {
                    _iterator = 0;
                    fetchFile();
                }
                else
                {
                    QString msg("Error: Scheduler Executable file is missing, Execution failed");
                    emit log(msg);
                }
                break;
            }
        case REPLAY:
            {
                QString scxmlFile = _generatedScxml.at(_iterator-1);
                QString logFile = _logNameLst.at(_iterator-1);
                qDebug()<<"scxmlfile:"<<scxmlFile<<"    logFile:"<<logFile;
                executeFile(scxmlFile,logFile);
                break;
            }
        case EXECUTENEXTANDSTOPONCOMPLETE:
            {
                fetchFile();
                break;
            }
        }
    }
}

void GTASequencer::OnCheckIfToolsRunningOnSlave(QVariant &iVal, QNetworkReply * )
{
    QString str = iVal.toString();
    QVariantList args;
    const char *slot;
    const char *faultSlot = SLOT(OnServerConnectionFailure(int,const QString &, QNetworkReply *));
//    if(str.simplified().trimmed().compare("true",Qt::CaseInsensitive) == 0)
//    {
        slot = SLOT(onStopToolsOnSlave());
        _slaveClientList.at(_clientRow)->sendRequest(args,"stopTools",this,slot,this,faultSlot);
//    }
//    else
//    {
//        emit log(QString("Starting scripts on %1 Slave").arg(_slaveList[_slaveClientList.at(_clientRow)->getIP()]));
//        slot = SLOT(onStartScriptsOnSlave(QVariant &));
//        _slaveClientList.at(_clientRow)->sendRequest(args,"startScripts",this,slot,this,faultSlot);

//    }
}

void GTASequencer::onStopToolsOnSlave()
{
    QVariantList args;
    const char *slot = SLOT(onStopScriptsOnSlave());
    const char *faultSlot = SLOT(OnServerConnectionFailure(int,const QString &, QNetworkReply *));
    _slaveClientList.at(_clientRow)->sendRequest(args,"stopScripts",this,slot,this,faultSlot);
}
void GTASequencer::onStopScriptsOnSlave()
{
    emit log(QString("Starting scripts on %1 Slave").arg(_slaveList[_slaveClientList.at(_clientRow)->getIP()]));
    QVariantList args;
    const char *slot = SLOT(onStartScriptsOnSlave(QVariant &));
    const char *faultSlot = SLOT(OnServerConnectionFailure(int, const QString &, QNetworkReply *));
    _slaveClientList.at(_clientRow)->sendRequest(args,"startScripts",this,slot,this,faultSlot);
}

void GTASequencer::onStartScriptsOnSlave(QVariant &iScriptLog)
{
    if(!iScriptLog.toString().isEmpty())
        emit log(iScriptLog.toString());
    emit log(QString("Starting tools on %1 Slave").arg(_slaveList[_slaveClientList.at(_clientRow)->getIP()]));
    QVariantList args;
    const char *slot = SLOT(onStartToolsOnSlave(QVariant &));
    const char *faultSlot = SLOT(OnServerConnectionFailure(int,const QString &, QNetworkReply *));
    _slaveClientList.at(_clientRow)->sendRequest(args,"startTools",this,slot,this,faultSlot);
}

void GTASequencer::onStartToolsOnSlave(QVariant & iToolLog)
{
    if(!iToolLog.toString().isEmpty())
        emit log(iToolLog.toString());
    _clientRow++;
    checkIfToolsAreRunning();
}

void GTASequencer::OnServerConnectionFailure(int , const QString &, QNetworkReply * iReply )
{
    emit log(QString("Failed connection with %1 Slave").arg(_slaveList[iReply->url().host()]));
    _clientRow++;
    checkIfToolsAreRunning();
}

void GTASequencer::stopScripts()
{
    foreach(QString killCmd, _killScriptsList)
    {
        QProcess::execute(killCmd);
    }
    //removing all tools from list
    _killScriptsList.clear();
}
