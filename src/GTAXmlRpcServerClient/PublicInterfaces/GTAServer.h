#ifndef GTASERVER_H
#define GTASERVER_H

#pragma warning(push, 0)
#include "GTAServerClient_global.h"
#include "maiaXmlRpcServer.h"
#include <QObject>
#include <QVariantMap>
#include <QtCore>
#include <QMutex>
#pragma warning(pop)


class GTAXMLRPCSERVERCLIENTSHARED_EXPORT GTAServer : public QObject {
    Q_OBJECT
private:
    MaiaXmlRpcServer *_XMLRPCServer;
    QString _XmlRpcIP;
    quint16 _XmlRpcPort;
public:
    GTAServer(const QString &iIP, quint16 iport);
    ~GTAServer();
    void setXMLRPCPort(quint16 iPort);
    void setXMLRPCIP(const QString &iIP);
    int getXMLRPCPort() const;
    QString getXMLRPCIP() const;
    bool init();
    bool registerUserFunctions(QObject *iobj,const QString &iFunctionName,const char* iSlotName);
    bool unregisterUserFunctions(const QString &iFunctionName);
    static QStringList getAvailableAddress();
};

#endif // GTASERVER_H
