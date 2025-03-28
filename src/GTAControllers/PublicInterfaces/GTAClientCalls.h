#ifndef GTACLIENTCALLS_H
#define GTACLIENTCALLS_H

#pragma warning(push, 0)
#include <QObject>
#include <QPointer>
#pragma warning(pop)

#include "GTAClient.h"
#include "GTAControllers.h"
#include "GTAUtil.h"

//TODO: Using compile-time built structures inside .dll linked libraries is generally a bad idea, 
//      however in this project .dll libraries are not used in intented way (they are basically static linked) - so it doesn't cause problems
//      in future would propably be better to use static link or rewrite these
#pragma warning (disable : 4251)
class GTAControllers_SHARED_EXPORT GTAClientCalls : public QObject
{
    Q_OBJECT
public:
    GTAClientCalls(QString ibisgIP,int ibisgport);
    ~GTAClientCalls();
private:
    GTAClientCalls();

public slots:
    /**
      * @brief: sends an xmlrpc client call to external server.
      * @iIP: IP address of external tool server
      * @iPort: Port of external tool server
      * @iMethodname: function to call in bisg server
      * @iResponseObj: object of the class to which the response needs to be connected
      * @iResponseSlotName: slot of the class to which the response needs to be connected
      * @return true/false for rpc call sent/failed
      */
    QNetworkReply* sendExtToolRequest(QString iIP, int iPort,QVariantList iArgs,QString iMethodname,QObject* iResponseObj,const char* iResponseSlotName);
    /**
      * @brief: sends an xmlrpc client call to BISG server. response object and slot is locally connected if responseObject and slot are null
      * @iArgs: arguments to send to bisg server as a QVariantList
      * @iMethodname: function to call in bisg server
      * @iResponseObj: object of the class to which the response needs to be connected
      * @iResponseSlotName: slot of the class to which the response needs to be connected
      * @return true/false for rpc call sent/failed
      */
    bool sendBISGRequest(QVariantList iArgs,QString iMethodname,QObject* iResponseObj,const char* iResponseSlotName);

    /**
      * @brief: sends an xmlrpc client call to BISG server. The function must be used with a blocking Queued connection call
      *         the response of the call is defaulted to a private slot in this class
      * @iArgs: arguments to send to bisg server as a QVariantList
      * @iMethodname: function to call in bisg server
      * @return network reply object (QNetworkReply*) to the request sent
      */
    QNetworkReply* sendBlockingBISGRequest(QVariantList iArgs,QString iMethodname);

    /**
      * @brief: creates and starts the rpc client to connect to bisg server
      */
    void startClient();

private slots:
    /**
      * @brief: default slot to receive response back from bisg server
      * @retVal: value returned from bisg server in QVariant format
      * @ireply: QNetworkReply Object that gives the status of the last network request sent
      */
    void returnRequested(QVariant &retVal, QNetworkReply *ireply);

    /**
      * @brief: the slot receives any error encountered in the network
      *         during a network request.
      *         the emits the network error in the following format through returnBlockedResponse signal
      *         EEE:<iFaultCode>-<iFaultMessage>
      * @iFaultCode: fault code over the network
      * @iFaultMessage: fault message
      * @ireply: QNetworkReply Object that gives the status of the last network request sent
      */
    void faultResponse(int iFaultCode, const QString &iFaultMessage, QNetworkReply* ireply);

signals:
    /**
      * @brief: signal to forward bisg reply calls from default slot
      * @retVal: value returned from bisg server in QVariant format
      */
    void returnResponse(QVariant response);

    /**
      * @brief: signal to forward bisg reply calls from blocked slot
      *         recomended to use for connections if a blocked slot is used
      * @retVal: value returned from bisg server in QVariant format
      */
    void returnBlockedResponse(QVariant response);

private:
    QString _bisgIP;
    int _bisgPort;
    GTAClient* _bisg;
    QList<GTAClient*> _clientList;
};

#endif // GTACLIENTCALLS_H
