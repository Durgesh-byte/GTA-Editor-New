#include "GTAAppLogMsgRedirector.h"

#pragma warning(push, 0)
#include <QDateTime>
#include <QThread>
#pragma warning(pop)

void GTAAppLogMsgRedirector::all_Qt5(QtMsgType itype, const QMessageLogContext& icontext, const QString& imsg)
{
    GTAMdiController* pMdiController = GTAMdiController::getMDIController();
    QString dbgStrFinal,dbgStr;

	if (imsg.trimmed().length() > 0)
    {
        dbgStr = QDateTime::currentDateTime().toString("yyyy/MM/dd HH:mm:ss.zzz") + "\t(QThread 0x" + QString::number((qlonglong)QThread::currentThreadId(), 16) + ")\t" + imsg + "\t"+QString("File: %1 \t Function:%2 \t Line:%3").arg(icontext.file,icontext.function,QString::number(icontext.line));
    }
    switch (itype)
    {
    case QtInfoMsg:
        dbgStrFinal = QString("[Info]: %1\n").arg(dbgStr);
        pMdiController->onUpdateApplicationLogs(QStringList()<<dbgStrFinal);
        break;
    case QtCriticalMsg:
        dbgStrFinal = QString("[Critical]: %1\n").arg(dbgStr);
        pMdiController->onUpdateApplicationLogs(QStringList()<<dbgStrFinal);
        break;
    case QtWarningMsg:
        dbgStrFinal = QString("[Warning]: %1\n").arg(dbgStr);
        pMdiController->onUpdateApplicationLogs(QStringList()<<dbgStrFinal);
        break;
    case QtDebugMsg:
        dbgStrFinal = QString("[Debug]: %1\n").arg(dbgStr);
        pMdiController->onUpdateApplicationLogs(QStringList()<<dbgStrFinal);
        break;
    case QtFatalMsg:
        dbgStrFinal = QString("[Fatal]: %1\n").arg(dbgStr);
        pMdiController->onUpdateApplicationLogs(QStringList()<<dbgStrFinal);
        //DEV: use a QMessagebox to intimate application crash
        abort();
    }
}

void GTAAppLogMsgRedirector::info_Qt5(QtMsgType itype, const QMessageLogContext& icontext, const QString& imsg)
{
    GTAMdiController* pMdiController = GTAMdiController::getMDIController();
    QString dbgStrFinal,dbgStr;
    if (imsg.trimmed().length() > 0)
    {
        dbgStr = QDateTime::currentDateTime().toString("yyyy/MM/dd HH:mm:ss.zzz") + "\t(QThread 0x" + QString::number((qlonglong)QThread::currentThreadId(), 16) + ")\t" + imsg + "\t"+QString("File: %1 \t Function:%2 \t Line:%3").arg(icontext.file,icontext.function,QString::number(icontext.line));
    }
    switch (itype)
    {
    case QtInfoMsg:
        dbgStrFinal = QString("[Info]: %1\n").arg(dbgStr);
        pMdiController->onUpdateApplicationLogs(QStringList()<<dbgStrFinal);
        break;
    case QtCriticalMsg:
        break;
    case QtWarningMsg:
        dbgStrFinal = QString("[Warning]: %1\n").arg(dbgStr);
        pMdiController->onUpdateApplicationLogs(QStringList()<<dbgStrFinal);
    case QtDebugMsg:
        break;
	case QtFatalMsg:
		dbgStrFinal = QString("[Fatal]: %1\n").arg(dbgStr);
		pMdiController->onUpdateApplicationLogs(QStringList() << dbgStrFinal);
        //DEV: use a QMessagebox to intimate application crash
        abort();
    }
}

void GTAAppLogMsgRedirector::debug_Qt5(QtMsgType itype, const QMessageLogContext& icontext, const QString& imsg)
{
    GTAMdiController* pMdiController = GTAMdiController::getMDIController();
    QString dbgStrFinal,dbgStr;
    if (imsg.trimmed().length() > 0)
    {
        dbgStr = QDateTime::currentDateTime().toString("yyyy/MM/dd HH:mm:ss.zzz") + "\t(QThread 0x" + QString::number((qlonglong)QThread::currentThreadId(), 16) + ")\t" + imsg + "\t"+QString("File: %1 \t Function:%2 \t Line:%3").arg(icontext.file,icontext.function,QString::number(icontext.line));
    }
    switch (itype)
    {
    case QtInfoMsg:
        break;
    case QtCriticalMsg:
        break;
    case QtWarningMsg:
        break;
    case QtDebugMsg:
        dbgStrFinal = QString("[Debug]: %1\n").arg(dbgStr);
        pMdiController->onUpdateApplicationLogs(QStringList()<<dbgStrFinal);
        break;
    case QtFatalMsg:
        //DEV: use a QMessagebox to intimate application crash
        abort();
    }
}
