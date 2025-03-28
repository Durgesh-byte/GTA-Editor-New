#ifndef AINGTAICSERVERCLIENTCOMM_H
#define AINGTAICSERVERCLIENTCOMM_H

#include <QObject>
#include <AINGTAServer.h>

class AINGTAICServerClientComm : public QObject
{
    Q_OBJECT

    AINGTAServer * _server;
    QVariant _returnReply;
    QVariant getReturnReply();
public:
    explicit AINGTAICServerClientComm(QObject *parent = 0);

    ~AINGTAICServerClientComm();

    static int _port;
    static QString _serverIP;

    void setReturnReply(QVariant ireply);


signals:
    void serverStarted(bool iVal);
    bool displayMsg(QString msg, bool waitAck);
    void getUserInput(QString msg,QString expectedReturnType);

public slots:

    /**
      * Author : Priyanka
      * This function initializes server on selected IP and port
      */
    void initializeServer();

private slots:

    /**
      * Author : Priyanka
      * This function triggers prompt
      * msg : message to display
      * waitAck : true/false for blocking/non-blocking pormpt
      * @return : true/false based on user selection OK/KO
      */
    bool onDisplayMessage(QString msg, bool waitAck);

    /**
      * Author : Priyanka
      * This function triggers user input prompt
      * msg : message to display
      * expectedReturnType : expected type of return value
      * @return : user input return value
      */
    QVariant onGetParamValueSingleSample(QString msg, QString expectedReturnType);


};

#endif // AINGTAICSERVERCLIENTCOMM_H
