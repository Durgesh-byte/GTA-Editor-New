#include "AINGTAICServerClientComm.h"


QString AINGTAICServerClientComm::_serverIP = "";
int AINGTAICServerClientComm::_port = 0;

AINGTAICServerClientComm::AINGTAICServerClientComm(QObject *parent) :
        QObject(parent)
{
    _server = NULL;
}

AINGTAICServerClientComm::~AINGTAICServerClientComm()
{
    if(_server != NULL)
    {
        _server->unregisterUserFunctions("displayMessage");
        _server->unregisterUserFunctions("getParamValueSingleSample");

        delete _server;
        _server = NULL;
    }

}

void AINGTAICServerClientComm::initializeServer()
{
    if(_server != NULL)
    {
        _server->unregisterUserFunctions("displayMessage");
        _server->unregisterUserFunctions("getParamValueSingleSample");

        delete _server;
        _server = NULL;
    }

    _server = new AINGTAServer(_serverIP,_port);
    bool rc = _server->init();
    if(rc)
    {
        _server->registerUserFunctions(this,"displayMessage","onDisplayMessage");
        _server->registerUserFunctions(this,"getParamValueSingleSample","onGetParamValueSingleSample");
    }

    emit serverStarted(rc);

}

bool AINGTAICServerClientComm::onDisplayMessage(QString msg, bool waitAck)
{
    if(waitAck)
    {
        _returnReply.clear();
        emit displayMsg(msg,waitAck);
        while(getReturnReply().isNull())
        {
            //Wait for User input
        }
        return getReturnReply().toBool();
    }
    else
    {
        emit displayMsg(msg,waitAck);
        return true;
    }

}

QVariant AINGTAICServerClientComm::onGetParamValueSingleSample(QString msg, QString expectedReturnType)
{
    _returnReply.clear();
    emit getUserInput(msg,expectedReturnType);
    while(getReturnReply().isNull())
    {
        //Wait for User input
    }
    return getReturnReply();
}


QVariant AINGTAICServerClientComm::getReturnReply()
{
    return _returnReply;
}

void AINGTAICServerClientComm::setReturnReply(QVariant ireply)
{
    _returnReply = ireply;
}
