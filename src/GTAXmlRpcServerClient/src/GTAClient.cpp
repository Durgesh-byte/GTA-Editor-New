#pragma warning(push, 0)
#include "GTAClient.h"
#include <QDebug>
#include <QDir>
#include <QDateTime>
#include <iostream>
#include <QThread>
#pragma warning(pop)

/*====================================================================
    example usage of the GTAClient from any QObject inherited class

_client = new GTAClient("127.0.0.1",8082);
if (_client.init())
{
    QVariantList args;
    _client->sendRequest(args,"examples.nix",this,SLOT(testResponse(QVariant &)));
    args << QVariant(7);
    _client->sendRequest(args,"examples.birne",this,SLOT(testResponse(QVariant &)));
}

    implement the slot testResponse in the QObject inherited class
====================================================================*/

/**
 * Constructor for the xmlrpc client which just creates a connection for use
 * @iIP: IP address of the server to connect to. Expected format IPV4
 * @iport: Port number of the server to connect to.
*/
GTAClient::GTAClient(const QString &iIP ,const int& iport)
{
    //default ip "127.0.0.1"
    //default port "0"
    _XMLRPCClient = NULL;
    _XmlRpcPort = 0;
    _XmlRpcIP = "127.0.0.1";
    setIP(iIP);
    setPort(iport);
    createUrl();
}

GTAClient::GTAClient(const GTAClient &client)
{
	_XMLRPCClient = NULL;
    setIP(client.getIP());
    setPort(client.getPort());
    createUrl();
    if(client._XMLRPCClient != NULL)
        init();
}

/**
 * Initializing the MaiaXmlRpcClient with user defined IP and Port
*/
bool GTAClient::init()
{
    QUrl usage = QUrl(_XmlRpcUrl);
    bool rc = usage.isValid();
    if (rc)
    {
        if (NULL != _XMLRPCClient)
        {
            delete _XMLRPCClient;
            _XMLRPCClient = NULL;
        }
        _XMLRPCClient = new MaiaXmlRpcClient(usage,this);

#ifndef QT_NO_OPENSSL
        QSslConfiguration config = _XMLRPCClient->sslConfiguration();
        config.setProtocol(QSsl::AnyProtocol);
        _XMLRPCClient->setSslConfiguration(config);
		connect(_XMLRPCClient, SIGNAL(sslErrors(QNetworkReply *, const QList<QSslError> &)),this, SLOT(handleSslErrors(QNetworkReply *, const QList<QSslError> &)));
#endif
        
    }
    return rc;
}

/**
 * Destructor for the class
*/
GTAClient::~GTAClient()
{
    if(_XMLRPCClient != NULL)
    {
        delete _XMLRPCClient;
        _XMLRPCClient = NULL;
    }
}

/**
 * Creating an url from the ip and port
*/
void GTAClient::createUrl()
{
    _XmlRpcUrl = QString("http://%1:%2/RPC2").arg(_XmlRpcIP,QString::number(_XmlRpcPort));
}

/**
 * Setting the IP of the server to connect to. Default server ip is set to localhost
 * @iIP: IP address of the server to connect to. Expected format IPV4
*/
void GTAClient::setIP(const QString &iIP)
{
    //assign a localhost if the ip is not IPV4
    QHostAddress address(iIP);
    if (QAbstractSocket::IPv4Protocol != address.protocol())
    {
        _XmlRpcIP = "127.0.0.1";
    }
    else
    {
        _XmlRpcIP = iIP;
    }
    createUrl();
    init();
}

/**
 * Getter for the IP member variable
 * @return: IP as a QString
*/
QString GTAClient::getIP() const
{
    return _XmlRpcIP;
}

/**
 * Setting the Port of the server to connect to. Default server port is set to 8082
 * @iPort: Port number of the server to connect to.
*/
void GTAClient::setPort(const int &iPort)
{
    //make sure the port is non-negative
    if (iPort <= 0)
        _XmlRpcPort = 8082;
    else
        _XmlRpcPort = iPort;
    createUrl();
    init();
}

/**
 * Getter for the port member variable
 * @return: port as an integer
*/
int GTAClient::getPort() const
{
    return _XmlRpcPort;
}

/**
 * Returns the url created based on the ip and port entered by the user
 * @return: url as a QString
*/
QString GTAClient::getUrl() const
{
    return _XmlRpcUrl;
}

/**
 * Constructor for the xmlrpc client which just creates a connection for use
 * @iArgs: QVariant Arguments to be sent to the server.
 * @iMethodName: Name of the method to connect to on the server side.
 * @iobj: QObject pointer of the slot where the response from server needs to hit.
 * @iResponseSlotName: SLOT to be hit if the server sends back a response. function pointer.
 * @return: QNetworkReply pointer if request was made/cancelled
*/
QNetworkReply* GTAClient::sendRequest(QVariantList &iArgs,const QString &iMethodName,QObject *iobj,const char* iResponseSlotName,QObject* ifaultObj,const char* iFaultSlotName)
{
    QNetworkReply* reply = nullptr;
    if (NULL != _XMLRPCClient)
    {
        if ((ifaultObj == NULL) || (iFaultSlotName == NULL))
            reply = _XMLRPCClient->call(iMethodName, iArgs,
                                iobj, iResponseSlotName,
                                this, SLOT(testFault(int, const QString &,QNetworkReply *)));
        else
        {
            reply = _XMLRPCClient->call(iMethodName, iArgs,
                                iobj, iResponseSlotName,
                                ifaultObj, iFaultSlotName);
        
        }
    }
    return reply;
}

//void GTAClient::doClient() {
//        QVariantList args;
//        _XMLRPCClient->call("examples.nix", args,
//                                this, SLOT(testResponse(QVariant &)),
//                                this, SLOT(testFault(int, const QString &)));
//        args << QVariant(7);
//        _XMLRPCClient->call("examples.getStateName", args,
//                                this, SLOT(testResponse(QVariant &)),
//                                this, SLOT(testFault(int, const QString &)));
//        _XMLRPCClient->call("examples.birne", args,
//                                this, SLOT(testResponse(QVariant &)),
//                                this, SLOT(testFault(int, const QString &)));
//        args[0] = QVariant(-128);
//        _XMLRPCClient->call("examples.birne", args,
//                                this, SLOT(testResponse(QVariant &)),
//                                this, SLOT(testFault(int, const QString &)));
//        _XMLRPCClient->call("examples.notfound", args,
//                                this, SLOT(testResponse(QVariant &)),
//                                this, SLOT(testFault(int, const QString &)));
//        QDateTime towelDay;
//        towelDay.setDate(QDate(2008, 5, 25));
//        args.clear();
//        args << towelDay;
//        _XMLRPCClient->call("examples.plusOneYear", args,
//                                this, SLOT(towelResponse(QVariant &)),
//                                this, SLOT(testFault(int, const QString &)));
//}

/**
 * Internal slot to manage any test faults during the request/response
 * @error: Error code generated by enum QNetworkReply::NetworkError
 * @message: Error message in readable format.
*/
void GTAClient::testFault(int error, const QString &message,QNetworkReply *ireply)
{
    qDebug() << "EEE:" << error << "-" << message << "-" << ireply->errorString();
}


/**
 * Slot to handle SSL connection errors
 * @reply: QNetworkReply pointer
 * @errors: List of QSslError errors.
*/
void GTAClient::handleSslErrors(QNetworkReply *reply, const QList<QSslError>&) {
        //qDebug() << "SSL Error:" << errors;
    qDebug()<<reply->errorString();
    reply->ignoreSslErrors(); // don't do this in real code! Fix your certs!
}

bool GTAClient::operator ==(const GTAClient*& iClient)
{
    if ((iClient->getIP() == this->getIP()) && (iClient->getPort() == this->getPort()))
        return true;
    return false;
}
