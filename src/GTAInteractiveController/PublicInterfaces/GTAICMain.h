#ifndef AINGTAICMAIN_H
#define AINGTAICMAIN_H

#include <QWidget>
#include <AINGTAICServerClientComm.h>
#include <QAbstractButton>

namespace Ui {
class AINGTAICMain;
}

class AINGTAICMain : public QWidget
{
    Q_OBJECT

public:
    explicit AINGTAICMain(QWidget *parent = 0);
    ~AINGTAICMain();

private:
    Ui::AINGTAICMain *ui;

    /**
     * Author : Priyanka
     * This function reads settings form applicationpath/conf/GTAInteractiveController.conf file and sets
     */
    void readSettings();
    AINGTAICServerClientComm * _pServer;
    QThread * _pServerThread;
    bool _mousePressed;
    QPoint _mousePosition;


private slots:
    /**
      * Author : Priyanka
      * This function shows/hides the log window
      * @iVal: true/false to show/hide
      */
    void displayEditor(bool iVal);

    /**
      * Author : Priyanka
      * This function starts/stops the server on seperate QThread
      * @iVal: true/fale to start/stop
      */
    void startServer(bool iVal);

    /**
      * Author : Priyanka
      * This function minimizes the application.(window minimize option)
      */
    void minimizeApplication();

    /**
      * Author : Priyanka
      * This function manages UI based on server reply, if it is started or not
      * @iVal : true/false based on started or not
      */
    void serverStarted(bool iVal);

    /**
      * Author : Priyanka
      * This function prompts for OK/KO
      * @msg: message to displayed.
      * @waitAck: true/false for blocking/non-blocking prompt
      */
    bool onDisplayMessage(QString msg, bool waitAck);

    /**
      * Author : Priyanka
      * This function prompts for user input
      * @msg: message to displayed.
      * @expectedReturnVal: the type of expected return value
      */
    void onGetUserInput(QString msg, QString expectedReturnVal);


    /**
      * Author : Priyanka
      * This function updates the user input on server thread
      * @reply: return value
      */
    void sendUserInput(QString reply);

    /**
      * Author : Priyanka
      * This function updates the user input (OK/KO) on server.
      * @reply: return value(ok/ko)
      */
    void onSendAcknowledgment(QString reply);

    /**
      * Author : Priyanka
      * This function updates the log window
      * @text: text to be displayed on log window.
      */
    void updateLogWindow(QString text);

protected:

    /**
     * Mouse Press,Move,Release events are overridded to handling moving of frameless window
     */
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

signals:
    void log(QString logText);
};

#endif // AINGTAICMAIN_H
