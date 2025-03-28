#ifndef AINGTASEQUENCERSERVERCLIENTCOMM_H
#define AINGTASEQUENCERSERVERCLIENTCOMM_H


#include <QString>
#include <AINGTAServer.h>
#include <AINGTAClient.h>
#include <AINGTAUtil.h>
#include <QNetworkReply>

class AINGTASEQUENCERServerClientThread : public QThread
{
public:
    static void sleep ( unsigned long secs )
    {
        QThread::sleep(secs);
    }
};

class AINGTASEQUENCERServerClientComm : public QObject
{
    Q_OBJECT


public:
    explicit AINGTASEQUENCERServerClientComm(QObject *parent = 0);

    // bool initializeServer(const QString &iIP);

    ~AINGTASEQUENCERServerClientComm();

    /*
     Server Client Communication is available only with Major Version 27 and Release version 2
     */

#if (MAJOR_TOOL_VERSION == 27 && RELEASE_TOOL_VERSION > 1) || (MAJOR_TOOL_VERSION > 27)
     static int _availableIPs;
     static QString _serverIP;

 private:
    int _XmlRpcPort;
    QList<int> _ports;
    AINGTAServer * _server;
    AINGTAClient  * _client;
    bool _connectionEstablished;


   QList<AINGTAClient*> _clientList;



 public:
    int _serverRunning;

public slots:

    /**
      * Author: Priyanka
      * This function is hit when a server is tried to start on another instance of application.
      */
    bool testingServer();

    /**
      * Author: Priyanka
      * This function is hit for positive response when a server is already running in the current PC.
      */
    void testingResponse(QVariant &iVal);

    /**
      * Author: Priyanka
      * This function is hit for error case of "testServer".
      */
    void faultResponse(int errorCode, const QString & message, QNetworkReply * reply);

    /**
      * Author: Priyanka
      * This function checks if a server is already running in any one of the ports and IP
      */
    void checkIfServerIsAlreadyRunning();


    /**
      * Author: Priyanka
      * This function ensures only one server-client connection.
      * @return: true if the server-client connection is established
      */
    bool serverClientConnection();

    /**
      * Author: Priyanka
      * This function adds the tools to the current tools list.
      * @itoolsList:list of tools to be added, sent from Client.
      * @return: false if one or more tools could not be added.
      */
    bool onAddTools(const QVariantList &itoolsList);

    /**
      * Author: Priyanka
      * This function adds the scripts to the current initialization script list.
      * iscriptsList:list of scripts to be added, sent from Client.
      * @return: false if one or more scripts could not be added.
      */
    bool onAddScripts(const QVariantList &iscriptsList);

    /**
      * Author: Priyanka
      * This function is to initialize the server on IP Connected and the available port from pre-defined list of ports.
      * @return: true if server is started.
      */
    bool initializeServer();

    /**
      * Author: Priyanka
      * This function returns the list of tools to client.
      * @return: list of tools added in current editor.
      */
    QVariantList ongetTools();

    /**
      * Author: Priyanka
      * This function returns the list of scripts to client.
      * @return: list of scripts added in current editor.
      */
    QVariantList ongetScripts();

    /**
      * Author: Priyanka
      * This function stops the scripts which are currently running.
      */
    void onStopScripts();

    /**
      * Author: Priyanka
      * This function stops the tools which are currently running.
      */
    void onStopTools();

    /**
      * Author: Priyanka
      * This function start the scripts which are currently added on editor.
      *@return: log for sequencer window stating the status of each script
      */
    QVariant onStartScripts();

    /**
      * Author: Priyanka
      * This function start the tools which are currently added on editor.
      *@return: log for sequencer window stating the status of each tool
      */
    QVariant onStartTools();

    /**
      * Author: Priyanka
      * This function removes the list of tools on current editor.
      *@itoolsList: list of tools to be removed from client.
      */
    void onRemoveTools(const QVariantList &itoolsList);

    /**
      * Author: Priyanka
      * This function removes the list of scripts on current editor.
      *@itoolsList: list of scripts to be removed from client.
      */
    void onRemoveScripts(const QVariantList &iscriptsList);

    /**
      * Author: Priyanka
      * This function checks the currenlty running tools.
      * @return: true if one more or tools are running.
      */
    bool onCheckToolsAreRunning();

    /**
      * Author: Priyanka
      * This function disconnects the current server-client connection and enables server to connect to clinets again.
      */
    bool onDisconnectToServer();

 signals:
    bool addtools(const QStringList &itoolsList);
    bool addscripts(const QStringList &iscriptsList);
    //void getIP(QString &iIP);
    QVariantList gettools();
    QVariantList getscripts();
    QVariant startscripts();
    QVariant starttools();
    void stopscripts();
    void stoptools();
    void removetools(const QStringList &itoolsList);
    void removescripts(const QStringList &iscriptsList);
    bool checktoolsarerunning();
    void serverStarted(bool status);
    void multipleInstances(bool iVal);
#endif
};

#endif // AINGTASEQUENCERSERVERCLIENTCOMM_H
