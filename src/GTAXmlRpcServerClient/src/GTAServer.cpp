#include "GTAServer.h"

#pragma warning(push, 0)
#include <QDebug>
#include <QDir>
#include <QDateTime>
#include <iostream>
#include <QThread>
#pragma warning(pop)

/*====================================================================
    example usage of the GTAServer from any QObject inherited class

_server = new GTAServer("127.0.0.1",8082);
if (_server->init())
{
    _server->registerUserFunctions(this,"examples.birne","birne");
}

    implement the slot birne,nix in the QObject inherited class
====================================================================*/

/**
 * Constructor for the server class. Keeps default arguments
*/
GTAServer::GTAServer(const QString &iIP , quint16 iport)
{
    setXMLRPCIP(iIP);
    setXMLRPCPort(iport);
    //default ip "127.0.0.1"
    //default port "0"
    _XMLRPCServer = NULL;
}

/**
 * Destructor for the server class.
*/
GTAServer::~GTAServer()
{
    if(_XMLRPCServer != NULL)
    {
        delete _XMLRPCServer;
        _XMLRPCServer = NULL;
    }
}

/**
 * Setter for the XmlRpcPort
 * @iPort: Port number as an integer.
*/
void GTAServer::setXMLRPCPort(quint16 iPort)
{
    if (iPort <=0)
        _XmlRpcPort = 0;
    else
        _XmlRpcPort = iPort;
}

/**
 * Setter for the XMLRpcIP
 * @iIP: IP address as a QString. Expected IP as an IPV4
*/
void GTAServer::setXMLRPCIP(const QString &iIP)
{
    if (iIP.isEmpty())
        _XmlRpcIP = "127.0.0.1";
    else
        _XmlRpcIP = iIP;
}

/**
 * Getter for XmlRpcPort
 * @return: port number as an integer
*/
int GTAServer::getXMLRPCPort() const
{
    return _XmlRpcPort;
}

/**
 * Getter for XmlRpcIP
 * @return: ip address as a QString
*/
QString GTAServer::getXMLRPCIP() const
{
    return _XmlRpcIP;
}

/**
 * This function registers the function that need to be exposed to any client
 * @iobj: QObject pointer of the slot where the request from client needs to hit.
 * @iFunctionName: Name of the method to be exposed to any client.
 * @iSlotName: SLOT to be hit to process the client request. function pointer. Must be a slot of the iobj.
 * @return: true/false if the function was registered/not-registered
*/
bool GTAServer::registerUserFunctions(QObject *iobj,const QString &iFunctionName,const char* iSlotName)
{
    if (NULL !=_XMLRPCServer && NULL != iobj)
    {
        _XMLRPCServer->addMethod(iFunctionName,iobj,iSlotName);
        return true;
    }
    return false;
}

/**
 * This function un-registers the function were needed to be exposed to any client
 * @iFunctionName: Name of the method to be exposed to any client.
 * @return: true/false if the function was unregistered/not-unregistered
*/
bool GTAServer::unregisterUserFunctions(const QString &iFunctionName)
{
    if (NULL !=_XMLRPCServer)
    {
        _XMLRPCServer->removeMethod(iFunctionName);
        return true;
    }
    return false;
}

/**
 * This function searches through available interfaces on the current PC.
 * @return: list of available IP as a QStringList
*/
QStringList GTAServer::getAvailableAddress()
{
    QStringList addressList;
    foreach(QHostAddress host, QNetworkInterface::allAddresses())
    {
        if (QAbstractSocket::IPv4Protocol == host.protocol())
        {
            addressList.append(host.toString());
            //qDebug()<<"Avaialble IPV4 IP: "<<host.toString();
        }
        //qDebug()<<"Avaialble IP: "<<host.toString();
    }
    return addressList;
}

/**
 * This function creates and stars the server based on the ip and port provided.
 * Default IP(127.0.0.1) and port(random) is if user inputs are invalid
 * @return: true/false if the server was created/not-created
*/
bool GTAServer::init()
{
    bool rc = true;
    QHostAddress address(_XmlRpcIP);
    if(_XmlRpcPort <= 0)
    {
        qDebug()<<"Could not launch XML-RPC server, Port is missing\n";
        rc = false;
    }
    else if (QAbstractSocket::IPv4Protocol != address.protocol())
    {
        qDebug()<<"Could not launch XML-RPC server, IP is not IPv4 compatible\n";
        rc = false;
    }
    if(rc)
    {
        _XMLRPCServer = new MaiaXmlRpcServer(address,_XmlRpcPort,this);
        rc = _XMLRPCServer->isServerStarted();
        if (rc)
            qDebug()<<QString("Sever Started @ %1:%2 ").arg(_XmlRpcIP,QString::number(_XmlRpcPort));
        else
            qDebug()<<QString("Unable to start Sever @ %1:%2 ").arg(_XmlRpcIP,QString::number(_XmlRpcPort));
    }
    return rc;
}
