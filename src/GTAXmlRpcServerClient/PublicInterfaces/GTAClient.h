#ifndef GTACLIENT_H
#define GTACLIENT_H

#pragma warning(push, 0)
#include "GTAServerClient_global.h"
#include "maiaXmlRpcClient.h"
#include <QObject>
#include <QVariantMap>
#include <QtCore>
#include <QMutex>
#include <QUrl>
#pragma warning(pop)

class GTAXMLRPCSERVERCLIENTSHARED_EXPORT GTAClient : public QObject {
    Q_OBJECT
private:
    MaiaXmlRpcClient *_XMLRPCClient;
    QString _XmlRpcUrl;
    QString _XmlRpcIP;
    int _XmlRpcPort;
    void createUrl();
public:
    GTAClient(const QString &iIP ,const int& iport);
    GTAClient(const GTAClient &client);
    ~GTAClient();
    void setIP(const QString &iIP);
    QString getIP() const;
    QString getUrl() const;
    void setPort(const int &iPort);
    int getPort() const;
    bool init();
    QNetworkReply* sendRequest(QVariantList &iArgs,const QString &iMethodName,QObject *iobj,const char* iResponseSlotName,QObject* ifaultObj = NULL,const char* iFaultSlotName = NULL);
    bool operator ==(const GTAClient*& iClient);

private slots:
//    void testResponse(QVariant &);
    void testFault(int, const QString &,QNetworkReply *ireply);
//    void towelResponse(QVariant &);
    void handleSslErrors(QNetworkReply *reply, const QList<QSslError>&);

private slots:
//    void doClient();
};

#endif // GTACLIENT_H
