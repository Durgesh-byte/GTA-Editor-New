#include "GTADebugger.h"

#pragma warning(push, 0)
#pragma warning(pop)

GTADebugger::GTADebugger(QString idebugIP, int idebugPort,QObject *parent) :
        QObject(parent)
{
    _server = NULL;
    _userAction = kNone;
    _currentLineNo = -1;
    _nextLineNo = -1;
    _debugIP = idebugIP;
    _debugPort = idebugPort;
}


GTADebugger::~GTADebugger()
{
    if(_server != NULL)
    {
        _server->unregisterUserFunctions("BREAKPOINT");
        _server->unregisterUserFunctions("RESULT_RESPONSE");
        _server->unregisterUserFunctions("UnSubscribe");
        delete _server;
        _server = NULL;
    }
}

bool GTADebugger::startDebugger()
{
    if(_server != NULL)
    {
        _server->unregisterUserFunctions("BREAKPOINT");
        _server->unregisterUserFunctions("RESULT_RESPONSE");
        _server->unregisterUserFunctions("UnSubscribe");

        delete _server;
        _server = NULL;
    }    
    bool rc = false;
    _server = new GTAServer(_debugIP, static_cast<qint16>(_debugPort));
    if (NULL != _server)
    {
        //QStringList allowedAddr = _server->getAvailableAddress();
        //_server->setXMLRPCIP(allowedAddr.first());
        rc = _server->init();

        if (rc)
        {
            _server->registerUserFunctions(this,"BREAKPOINT","onBreakpointHit");
            _server->registerUserFunctions(this,"RESULT_RESPONSE","onResultReceivedFromServer");
            _server->registerUserFunctions(this,"UnSubscribe","onDebuggingCompleted");
        }
    }
    emit serverStartStatus(rc);
    return true;
}



QVariant GTADebugger::onDebuggingCompleted()
{
    QVariantMap map;
    map.insert("Status","OK");
    emit debuggingCompleted();
    return map;
}

QVariantMap GTADebugger::onBreakpointHit(QString iLineNumber)
{
    QVariantMap variant;
    qDebug() << "LINE_NUMBER : " << iLineNumber;
    bool hasBreakpoint = false;

    iLineNumber.replace("[","");
    iLineNumber.replace("]","");
    iLineNumber.replace(",","");
    iLineNumber = iLineNumber.trimmed();
    bool ok;
    int line = iLineNumber.toInt(&ok);
    if(ok)
    {
        emit doesLineHaveBreakpoint(line,hasBreakpoint);
        QString responseStr;
        responseStr = (hasBreakpoint == true) ? "true" : "false";
        if(hasBreakpoint)
        {
            emit debuggerHitBreakpoint(line);

            while((emit getUserInputStatus()) || (getUserAction() == kNone));

            variant.insert("ResponseRequired",responseStr);
            variant.insert("UserInput",convertUserActionToString(_userAction));
            setUserAction(kNone);
        }
        else
            variant.insert("ResponseRequired",responseStr);
    }
    return variant;
}

GTADebugger::UserAction GTADebugger::getUserAction()
{
    return _userAction;
}

void GTADebugger::setUserAction(GTADebugger::UserAction action)
{
    _userAction = action;
}

QString GTADebugger::convertUserActionToString(UserAction action)
{
    QString str;
    switch(action)
    {
    case GTADebugger::NEXT :
        str = "NEXT";
        break;


    case GTADebugger::PREVIOUS :
        str = "PREVIOUS";
        break;


    case GTADebugger::CONTINUE :
        str = "CONTINUE";
        break;

    case GTADebugger::STOP :
        str = "STOP";
        break;

    }
    return str;
}

void GTADebugger::onUpdateUserAction(GTADebugger::UserAction action)
{
    _userAction = action;
}

QVariant GTADebugger::onResultReceivedFromServer(QVariantList response)
{
    qDebug() << response;
    QVariantMap map;
    QString mapstring;
    if (response.first().canConvert<QVariantMap>())
    {
        QVariantMap result = response.first().toMap();
        mapstring = QString("{ ResponseStatus : %1 } , { ReturnResult : ").arg(result.value("ResponseStatus").toString());
        QVariant ResponseStatus = result.value("ReturnResult");
        QString mapval;
        if (ResponseStatus.canConvert<QVariantMap>())
            traverseMap(ResponseStatus.toMap(),map,mapval);
        mapstring = QString(" %1%2").arg(mapstring,mapval);
        mapstring.append("}");
        qDebug() << mapstring;

        emit updateOutputWindow(QStringList()<<mapstring);

        return QVariant("OK");
    }
	return QVariant();
}



void GTADebugger::traverseMap(QVariantMap ival,QVariantMap &oMap, QString &oMapString)
{
    foreach (QString key,ival.keys())
    {
        QVariant value = ival.value(key);
        if (value.canConvert<QVariantMap>())
        {
            QString outputString;
            traverseMap(value.toMap(),oMap,outputString);
            oMapString.append(QString(" { %1 : %2 } \n").arg(key,outputString));
        }
        else if (value.canConvert<QVariantList>())
        {
            traverseList(value.toList(),oMap,oMapString);
        }
        else
        {
            oMap.insertMulti(key,value);
            oMapString.append(QString("{ %1 : %2 }").arg(key,value.toString()));
        }
    }
}

void GTADebugger::traverseList(QVariantList ival,QVariantMap &oMap, QString &oMapString)
{
    foreach (QVariant val,ival)
    {
        if (val.canConvert<QVariantMap>())
        {
            traverseMap(val.toMap(),oMap,oMapString);
        }
        else if (val.canConvert<QVariantList>())
        {
            traverseList(val.toList(),oMap,oMapString);
            oMapString.append(QString("{ %1 : %2 }").arg(QString::number(ival.indexOf(val)),oMapString));
        }
        else
        {
            oMap.insertMulti(val.toString(),val);
            oMapString.append(QString("{ %1 : %2 }").arg(QString::number(ival.indexOf(val)),val.toString()));
        }
    }
}
