#include "GTAClientCalls.h"

GTAClientCalls::GTAClientCalls(QString ibisgIP,int ibisgport)
{
    _bisgIP = ibisgIP;
    _bisgPort = ibisgport;
    _bisg = nullptr;
}

GTAClientCalls::~GTAClientCalls()
{
    qDeleteAll(_clientList.begin(),_clientList.end());
}

/**
  * @brief: sends an xmlrpc client call to external server.
  * @iIP: IP address of external tool server
  * @iPort: Port of external tool server
  * @iMethodname: function to call in bisg server
  * @iResponseObj: object of the class to which the response needs to be connected
  * @iResponseSlotName: slot of the class to which the response needs to be connected
  * @return true/false for rpc call sent/failed
  */
QNetworkReply* GTAClientCalls::sendExtToolRequest(QString iIP, int iPort,QVariantList iArgs,QString iMethodname,QObject* iResponseObj,const char* iResponseSlotName)
{
    QNetworkReply* reply = nullptr;
    GTAClient* client = new GTAClient(iIP,iPort);

    //check if the list has the relevant object already present
    if (_clientList.contains(client))
    {
        int idx = _clientList.indexOf(client);
        delete client;
        client = nullptr;
        client = _clientList.at(idx);
    }

    _clientList.append(client);
    if (client->init())
    {
        if (nullptr == iResponseObj && nullptr == iResponseSlotName)
        {
            iResponseObj = this;
            iResponseSlotName = SLOT(returnRequested(QVariant &, QNetworkReply *));
        }
        const char* faultResponseSlot = SLOT(faultResponse(int,const QString&,QNetworkReply*));
        reply = client->sendRequest(iArgs,iMethodname,iResponseObj,iResponseSlotName,this,faultResponseSlot);
        if (nullptr != reply)
            reply->setObjectName(BLOCKED_CALL);
    }
    return reply;
}

/**
  * @brief: sends an xmlrpc client call to BISG server. response object and slot is locally connected if responseObject and slot are null
  * @iArgs: arguments to send to bisg server as a QVariantList
  * @iMethodname: function to call in bisg server
  * @iResponseObj: object of the class to which the response needs to be connected
  * @iResponseSlotName: slot of the class to which the response needs to be connected
  * @return true/false for rpc call sent/failed
  */
bool GTAClientCalls::sendBISGRequest(QVariantList iArgs,QString iMethodname,QObject* iResponseObj,const char* iResponseSlotName)
{
    bool rc = false;
    if (nullptr != _bisg)
    {
        if (nullptr == iResponseObj && nullptr == iResponseSlotName)
        {
            iResponseObj = this;
            iResponseSlotName = SLOT(returnRequested(QVariant &, QNetworkReply *));
        }
        const char* faultResponseSlot = SLOT(faultResponse(int,const QString&,QNetworkReply*));
        QNetworkReply* reply = _bisg->sendRequest(iArgs,iMethodname,iResponseObj,iResponseSlotName,this,faultResponseSlot);
        if (reply->isRunning())
            rc = true;
    }
    return rc;
}

QNetworkReply* GTAClientCalls::sendBlockingBISGRequest(QVariantList iArgs,QString iMethodname)
{
    QNetworkReply* reply = nullptr;
    if (nullptr != _bisg)
    {
        const char* iResponseSlotName = SLOT(returnRequested(QVariant &, QNetworkReply *));
        const char* faultResponseSlot = SLOT(faultResponse(int,const QString&,QNetworkReply*));
        reply = _bisg->sendRequest(iArgs,iMethodname,this,iResponseSlotName,this,faultResponseSlot);
        if (nullptr != reply)
            reply->setObjectName(BLOCKED_CALL);
    }
    return reply;
}

/**
  * @brief: creates and starts the rpc client to connect to bisg server
  */
void GTAClientCalls::startClient()
{
    if (nullptr != _bisg)
    {
        _clientList.removeAll(_bisg);
        delete _bisg;
        _bisg = nullptr;
    }
    _bisg = new GTAClient(_bisgIP,_bisgPort);
    _bisg->init();
    _clientList.append(_bisg);
}

/**
  * @brief: default slot to receive response back from bisg server
  * @retVal: value returned from bisg server in QVariant format
  * @ireply: QNetworkReply Object that gives the status of the last network request sent
  */
void GTAClientCalls::returnRequested(QVariant &retVal, QNetworkReply *ireply)
{
    if (nullptr != ireply && ireply->objectName() == BLOCKED_CALL)
        emit returnBlockedResponse(retVal);
    else
        emit returnResponse(retVal);
}

void GTAClientCalls::faultResponse(int iFaultCode, const QString &iFaultMessage, QNetworkReply* ireply )
{
    QString faultMessage = QString("EEE:%1-%2").arg(QString::number(iFaultCode),iFaultMessage);
    QVariant retVal = QVariant(faultMessage);
    if (nullptr != ireply && ireply->objectName() == BLOCKED_CALL)
        emit returnBlockedResponse(retVal);
    else
        emit returnResponse(retVal);
}
