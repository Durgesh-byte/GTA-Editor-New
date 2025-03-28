#include "AINGTASEQUENCERServerClientComm.h"
#include <QFile>


AINGTASEQUENCERServerClientComm::AINGTASEQUENCERServerClientComm(QObject *parent):
        QObject(parent)
{

#if (MAJOR_TOOL_VERSION == 27 && RELEASE_TOOL_VERSION > 1) || (MAJOR_TOOL_VERSION > 27)
    _connectionEstablished = false;    
    _ports<<14934<<29890<<33658<<42508<<38798;
    _server = NULL;
	_serverRunning = 0;
#endif
}

#if (MAJOR_TOOL_VERSION == 27 && RELEASE_TOOL_VERSION > 1) || (MAJOR_TOOL_VERSION > 27)
        QString AINGTASEQUENCERServerClientComm::_serverIP = "";
	int AINGTASEQUENCERServerClientComm::_availableIPs = 0;
#endif

AINGTASEQUENCERServerClientComm::~AINGTASEQUENCERServerClientComm()
{
#if (MAJOR_TOOL_VERSION == 27 && RELEASE_TOOL_VERSION > 1) || (MAJOR_TOOL_VERSION > 27)
    _serverIP = "";
			for(int i=0; i < _clientList.count();i++)
    {
        _clientList.at(i)->deleteLater();
    }
                _clientList.clear();
    if(_server != NULL)
    {
        _server->unregisterUserFunctions("testServer");
        _server->unregisterUserFunctions("connectToServer");
        _server->unregisterUserFunctions("addTools");
        _server->unregisterUserFunctions("addScripts");
        _server->unregisterUserFunctions("getTools");
        _server->unregisterUserFunctions("getScripts");
        _server->unregisterUserFunctions("startScripts");
        _server->unregisterUserFunctions("startTools");
        _server->unregisterUserFunctions("stopTools");
        _server->unregisterUserFunctions("stopScripts");
        _server->unregisterUserFunctions("removeTools");
        _server->unregisterUserFunctions("removeScripts");
        _server->unregisterUserFunctions("checkToolsAreRunning");
        _server->unregisterUserFunctions("disconnectToServer");
        /*qDeleteAll(_clientList.begin(), _clientList.end());
         _clientList.clear(); */
        delete _server;
        _server = NULL;
    }
#endif
}


#if (MAJOR_TOOL_VERSION == 27 && RELEASE_TOOL_VERSION > 1) || (MAJOR_TOOL_VERSION > 27)

bool AINGTASEQUENCERServerClientComm::initializeServer()
{
    for(int i=0; i < _clientList.count();i++)
    {
        _clientList.at(i)->deleteLater();
    }
    _clientList.clear();

    bool rc = false;
    foreach(int port,_ports)
    {

        if (_server != NULL)
        {
            _server->unregisterUserFunctions("testServer");
            _server->unregisterUserFunctions("connectToServer");
            _server->unregisterUserFunctions("addTools");
            _server->unregisterUserFunctions("addScripts");
            _server->unregisterUserFunctions("getTools");
            _server->unregisterUserFunctions("getScripts");
            _server->unregisterUserFunctions("startScripts");
            _server->unregisterUserFunctions("startTools");
            _server->unregisterUserFunctions("stopTools");
            _server->unregisterUserFunctions("stopScripts");
            _server->unregisterUserFunctions("removeTools");
            _server->unregisterUserFunctions("removeScripts");
            _server->unregisterUserFunctions("checkToolsAreRunning");
            _server->unregisterUserFunctions("disconnectToServer");


            delete _server;
            _server = NULL;
        }
        _server = new AINGTAServer(_serverIP,port);
        rc = _server->init();
        if(rc)
            break;
    }

    if(rc)
    {
        _server->registerUserFunctions(this,"testServer","testingServer");
        _server->registerUserFunctions(this,"connectToServer","serverClientConnection");
        _server->registerUserFunctions(this,"addTools","onAddTools");
        _server->registerUserFunctions(this,"addScripts","onAddScripts");
        _server->registerUserFunctions(this,"getTools","ongetTools");
        _server->registerUserFunctions(this,"getScripts","ongetScripts");
        _server->registerUserFunctions(this,"startScripts","onStartScripts");
        _server->registerUserFunctions(this,"startTools","onStartTools");
        _server->registerUserFunctions(this,"stopScripts","onStopScripts");
        _server->registerUserFunctions(this,"stopTools","onStopTools");
        _server->registerUserFunctions(this,"removeTools","onRemoveTools");
        _server->registerUserFunctions(this,"removeScripts","onRemoveScripts");
        _server->registerUserFunctions(this,"checkToolsAreRunning","onCheckToolsAreRunning");
        _server->registerUserFunctions(this,"disconnectToServer","onDisconnectToServer");
        emit serverStarted(true);
    }
    else
        emit serverStarted(false);

    return rc;
}

void AINGTASEQUENCERServerClientComm::checkIfServerIsAlreadyRunning()
{
	_clientList.clear();
    QStringList availableIPs = AINGTAServer::getAvailableAddress();

    foreach(QString IP, availableIPs)
    {
        foreach(int port, _ports)
        {
            _client = new AINGTAClient(IP,port);
            bool rc =_client->init();
            if(rc)
            {
                QVariantList args;
                const char *slot = SLOT(testingResponse(QVariant &));
                const char *faultSlot = SLOT(faultResponse(int, const QString &,QNetworkReply * ));

                _client->sendRequest(args,"testServer",this,slot,this,faultSlot);
                _clientList.append(_client);
            }
        }
    }
    // return false;
}

void AINGTASEQUENCERServerClientComm::faultResponse(int errorCode, const QString & message, QNetworkReply * reply)
{
    _serverRunning++;
	/*
    QString replyUrl = reply->url().toString();
    QFile file("C:/Users/NG862D2/Desktop/New folder/testresult.txt");
          if(file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
          {
              // We're going to streaming text to the file
              QTextStream stream(&file);

              stream << " URL :  "  << replyUrl << "TIme: " << QDateTime::currentDateTime().toString() <<'\n';

              file.close();
          }
		  */
    if(_serverRunning == (_availableIPs *_ports.count()))
        initializeServer();
}

bool AINGTASEQUENCERServerClientComm::testingServer()
{
    return true;

}

void AINGTASEQUENCERServerClientComm::testingResponse(QVariant &iVal)
{
    QString str = iVal.toString();
    if(str.simplified().trimmed().compare("true",Qt::CaseInsensitive) == 0)
    {
		for(int i=0; i < _clientList.count();i++)
    {
        _clientList.at(i)->deleteLater();
    }
		_clientList.clear();
        emit multipleInstances(true);
    }
    //emit multipleInstances(true);
}


bool AINGTASEQUENCERServerClientComm::serverClientConnection()
{
    if(!_connectionEstablished)
    {
        qDebug()<<"Server Client connection established...";
        _connectionEstablished = true;
        return true;
    }
    else
    {
        qDebug()<<"Server Client connection could not be established...";
        return false;
    }
}
bool AINGTASEQUENCERServerClientComm::onAddTools(const QVariantList &itoolsList)
{
    QStringList toolsList;
    foreach(QVariant str, itoolsList)
        toolsList.append(str.toString());
    bool rc = emit addtools(toolsList);
    return rc;
}

bool AINGTASEQUENCERServerClientComm::onAddScripts(const QVariantList &iscriptsList)
{
    QStringList scriptsList;
    foreach(QVariant str, iscriptsList)
        scriptsList.append(str.toString());

    bool rc = emit addscripts(scriptsList);
    return rc;
}

QVariantList AINGTASEQUENCERServerClientComm::ongetTools()
{
    return emit gettools();
}

QVariantList AINGTASEQUENCERServerClientComm::ongetScripts()
{
    return emit getscripts();
}
QVariant AINGTASEQUENCERServerClientComm::onStartScripts()
{
    return emit startscripts();
}
QVariant AINGTASEQUENCERServerClientComm::onStartTools()
{
    return emit starttools();
}

void AINGTASEQUENCERServerClientComm::onStopScripts()
{
    emit stopscripts();
}
void AINGTASEQUENCERServerClientComm::onStopTools()
{
    emit stoptools();
}

void AINGTASEQUENCERServerClientComm::onRemoveScripts(const QVariantList &iscriptsList)
{

    QStringList scriptsList;
    foreach(QVariant str, iscriptsList)
        scriptsList.append(str.toString());
    emit removescripts(scriptsList);
}

void AINGTASEQUENCERServerClientComm::onRemoveTools(const QVariantList &itoolsList)
{
    QStringList toolsList;
    foreach(QVariant str, itoolsList)
        toolsList.append(str.toString());
    emit removetools(toolsList);
}

bool AINGTASEQUENCERServerClientComm::onCheckToolsAreRunning()
{
    return emit checktoolsarerunning();
}

#endif

bool AINGTASEQUENCERServerClientComm::onDisconnectToServer()
{
    _connectionEstablished = false;
	return true;
}
