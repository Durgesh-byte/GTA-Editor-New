#ifndef GTADEBUGGER_H
#define GTADEBUGGER_H

#include "GTAControllers.h"
#include "GTAServer.h"

#pragma warning(push, 0)
#include <QObject>
#include <QThread>
#include <QVariantMap>
#pragma warning(pop)

class GTAControllers_SHARED_EXPORT GTADebugger : public QObject
{
    Q_OBJECT
public:


    enum UserAction{NEXT,PREVIOUS,CONTINUE,STOP,kNone};
    explicit GTADebugger(QString idebugIP, int idebugPort,QObject *parent = 0);
    ~GTADebugger();
    static void traverseMap(QVariantMap ival,QVariantMap &oMap,QString &oMapString);
    //    void stop();

signals:

    void debuggerHitBreakpoint(int currentLineNum);
    void doesLineHaveBreakpoint(int iLineNumber, bool &oHasBreakpoint);
    void updateOutputWindow(const QStringList iMsgLst);
    void debuggingCompleted();
    bool getUserInputStatus();
    void serverStartStatus(bool val);



public slots:
    bool startDebugger();
    QVariantMap onBreakpointHit(QString iLineNumber);
    QVariant onResultReceivedFromServer(QVariantList response);
    void onUpdateUserAction(GTADebugger::UserAction action);
    QVariant onDebuggingCompleted();


private:
    QString convertUserActionToString(UserAction action);
    GTADebugger::UserAction getUserAction();
    void setUserAction(GTADebugger::UserAction action);
    static void traverseList(QVariantList ival,QVariantMap &oMap,QString &oMapString);


private:
    GTAServer * _server;
    UserAction _userAction;
    int _currentLineNo;
    int _nextLineNo;
    QString _debugIP;
    int _debugPort;
};

#endif // GTADEBUGGER_H
