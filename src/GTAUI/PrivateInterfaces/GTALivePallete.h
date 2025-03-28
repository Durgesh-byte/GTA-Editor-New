#ifndef LIVEPALLETE_H
#define LIVEPALLETE_H

#include <QWidget>
#include "AINGTAGenSearchWidget.h"
#include "AINGTAICDParameter.h"
#include "AINGTAEditorWindow.h"
#include "AINGTAClientCalls.h"
#include "AINGTAUtil.h"
#include "AINGTALivePalleteCommands.h"
#include <QTableWidgetItem>
#include <QThread>
#include <QSplitter>
#include <QPointer>
#include "AINGTANewFileCreationDialog.h"

namespace Ui {
class AINGTALivePallete;
}

class AINGTALivePalleteModel;

class MyThread : public QThread
{
public:
    static void sleep(unsigned long msecs)
    {
        QThread::sleep(msecs);
    }
};

class AINGTALivePallete : public QWidget
{
    Q_OBJECT

    //functions
public:
    explicit AINGTALivePallete(QWidget *parent = 0);
	
	    /**
        * Author: Priyanka
        * This function returns the paramters from set and get tables
        */
    QList<AINGTAICDParameter> getParameterList();
	
	    /**
        * Author: Priyanka
        * This function sets the parameters imported into set/get tables based on the direction of parameter.
        * @igetVarList: list of parameters to be added
        * @isetVarList: Used While importing the saved parameters list
        *              format of Item info: key = parametername, Value = parameter Value + "·" + parameter SSM Value
        */
    void setParameterList(QMap<QString, QString> &isetVarList, QMap<QString, QString> &igetVarList);
    ~AINGTALivePallete();
    QPointer<AINGTAEditorWindow> getLiveEditorWindow() const;

private:
    /**
      * Author: Saurav
      * @brief: Creates a bisg subscribe call
      * @ioParamList: list of params to subscribe; parameters are removed if doesn't contain valid Tool Name
      * @return : QMap of keys as application name and values as prepared bisg subscribe call in below format:
      * 	- "Name" = (QString) <OSLApplicationName>
            - <OSLBusName1> = (QVariantMap)
                - "Media" = (QString)
                - <OSLSignalName1>  = (QVariantMap)
                    - "ParameterType" = (QString) <OSLSignal1Type>
                - ... other signals ...
            - ... other bus ....
      */
    QMap<QString, QVariantMap> getSubscribeParamsVariantList(QList<AINGTAICDParameter> & ioParamList);

    /**
      * Author: Saurav
      * @brief: Creates a set call
      * @iParamList: List of AINGTAICDParameter object signal to set
      * @oArgs: prepared bisg set call passed as output
      */
    void getSetParamVariantList(QList<AINGTAICDParameter> iParamList,QVariantList& oArgs);

    /**
      * Author: Saurav
      * @brief: function call to start the client call thread for calls to bisg server.
      *         The function is called by a switch (signal on to)signal: toggle)
      * @iVal: true/false decides whether to start/stop the client call thread
      */
    void init(bool iVal);

    /**
      * Author: Saurav
      * @brief: function enables/disables the Live Pallete UI components.
      * @iVal: true/false decides whether to enable/disabe the UI components
      */
    void manageInterfaceForCalls(int iVal);
    /**
      * Author: Swarup
      * @brief: function creates the context menu to force value.
      */
    void createContextMenu();
    /**
        * Author: Priyanka
        * This function gets ICD details from database in form of icd objects, for a given list of parameter triplets.
        * returns false in case any error is encountered.
        * @ iParameterList: list of triplets (non triplets are ignored)
        * @ oParameterList: output icd parameter objects
        */
    bool getICDParameterList(QStringList &iVariableList, QList<AINGTAICDParameter> & oParameterList);

    /**
        * Author: Priyanka
        * This function subscribe and add parameter list.
        * @ iParameterList: list of parameters to be subscribed
        * @iSetParamInfo: Used While importing the saved parameters list
		* @&iGetParamInfo: contains the output parameter while importing the save parameters list
        *              format of Item info: key = parametername, Value = parameter Value + "·" + parameter SSM Value
        */
    void subscribeAndAddParameters(QList<AINGTAICDParameter> &parameterList, QMap<QString, QString> &iSetParamInfo = QMap<QString,QString>(), QMap<QString, QString> &iGetParamInfo = QMap<QString,QString>());
    /**
        * Author: Swarup
        * This function gets Signals/variable list of a particular element.
        * @ pElem: Element of the file for which the variable list need to be updated
        * @ oVariableList: List containing the complete signals of passed pElement
        * @ iCheckUuidRep: to check the caller function UUID is not repetitive
        */
    void getVariableList(const AINGTAElement *pElem, QStringList &oVariableList,QStringList &iCheckUuidRep);

    void showProgressBar(bool iVal);

    //slots
private slots:
    /**
      * Author: Saurav
      * @brief: the function is received after the search widget ok is clicked.
      *         The function adds a signal to the set/get table based on input/output.
      *         The selected parameter is subscribed first before adding into the relevant table.
      */
    void onSearchWidgetOKClicked();
    /**
        * Author: Swarup
        * This function will get all variable list of the editor window once edit complete signal is emit
        */
    void onUpdateEditorSignal();

    /**
      * Author: Saurav
      * @brief: the function is the landing method for future watcher end.
      *         this method reverts the UI for access back to the user.
      */
    void onExecutingEditedCommandsFinished();

public slots:
    /**
      * Author: Saurav
      * @brief: slot that receives forwarded bisg reply from a client call
      * @retVal: value returned from bisg server in QVariant format
      */
    void onResponseReceived(QVariant iretVal);

    /**
      * Author: Saurav
      * @brief: slot that receives forwarded bisg reply from a client call
      *         this slot is recommeded to be connected to blocking BISG calls
      * @retVal: value returned from bisg server in QVariant format
      */
    void onBlockingResponseReceived(QVariant iretVal);

    /**
      * Author: Swarup
      * @brief: slot that receives to display context menu on right click
      * @iPos: specifies the point where right click has been done
      */
    void displayContextMenu(const QPoint &iPos);
    /**
      * Author: Swarup
      * @brief: function that manages forcing value on selecting the appropriate action
      */
    void onForceValueTriggered();
    /**
      * Author: Swarup
      * @brief: slot that receives to display context menu on right click from acquisition window
	  * @iPos: specifies the point where right click has been done
      */	
	void displayAcquisitionContextMenu(const QPoint &iPos);
    /**
      * Author: Swarup
      * @brief: function that manages forcing value on selecting the appropriate action
      */
	void onUpdateSignalRangeTriggered();

    /**
      * Author: Priyanka
      * @brief: function updates the set parameter table widget with the forced value and sends request to BISG to force value.
      * @iparamList: AINGTAICDParameter list: param updated with forced value.
      * @ipTimer: timer that must be sent from caller to identify the limit to which the call needs to be blocked
      * @return: QVariant response based on the request was sent/failed to server
      */
    QVariant setParameterValue(const QList<AINGTAICDParameter> &iparamList, QTimer* ipTimer);

    /**
      * Author: Priyanka
      * @brief: function updates the value from parameter
      * @oParam: parameter updated with value from model.
      * @return: true/false if the parameter could/couldn't be filled with value and SSM
      */
    bool getParameterValue(AINGTAICDParameter &oParam);

    /**
      * Author: Saurav
      * @brief: sends an xmlrpc client call to client that connects to external tool server.
      *         this slot uses the existing signal for ext tool and blocks until the call has finished.
      * @iIP: IP address of external tool server
      * @iPort: Port of external tool server
      * @iArgs: arguments to send to external tool server as a QVariantList
      * @iMethodname: function to call in bisg server
      * @return: QVariant response based on the request was sent/failed to server
      */
    QVariant sendBlockingExtToolRequest(QString iIP, int iPort,QVariantList iArgs,QString iMethodname,const QTimer* ipTimer);

    //signals:
signals:
    /**
      * Author: Saurav
      * @brief: sends an xmlrpc client call to client that connects to BISG server.
      * @iArgs: arguments to send to bisg server as a QVariantList
      * @iMethodname: function to call in bisg server
      * @iResponseObj: object of the class to which the response needs to be connected
      * @iResponseSlotName: slot of the class to which the response needs to be connected
      */
    bool sendBISGRequest(QVariantList iArgs,QString iMethodname,QObject* iResponseObj,const char* iResponseSlotName);

    QNetworkReply* sendBlockingBISGRequest(QVariantList iArgs,QString iMethodname);

    /**
      * Author: Saurav
      * @brief: sends an xmlrpc client call to client that connects to external tool server.
      * @iIP: IP address of external tool server
      * @iPort: Port of external tool server
      * @iArgs: arguments to send to external tool server as a QVariantList
      * @iMethodname: function to call in bisg server
      * @iResponseObj: object of the class to which the response needs to be connected
      * @iResponseSlotName: slot of the class to which the response needs to be connected
      */
    QNetworkReply* sendExtToolRequest(QString iIP, int iPort,QVariantList iArgs,QString iMethodname,QObject* iResponseObj,const char* iResponseSlotName);

    /**
      * Author: Saurav
      * @brief: signal is used to send a call to mainwindow to set up the environment for gta live
      * @iVal: false/true to setup/dismantle the environment
      */
    void toggleEditorActions(bool iVal);

    /**
      * Author: Saurav
      * @brief: signal to forward bisg reply calls from default slot
      * @retVal: value returned from bisg server in QVariant format
      * @ireply: QNetworkReply Object that gives the status of the last network request sent
      */
    void sendResponse(QVariant retVal);

    /**
      * Author: Saurav
      * @brief: This signal is linked to the slot to display any message to the debug output window
      * @iMsgLst: the list of messages that needs to be updated on Debug Output Window
      */
    void updateOutputWindow(const QStringList iMsgLst);

    /**
      * Author: Saurav
      * @brief: This signal is linked to execute non-readOnly commands available in the live-editor
      */
    void executeAndAnalyzeElement();

    /**
      * Author: Saurav
      * @brief: This signal is saves the live editor. There are options provided to user like
      *         change the save location; name of procedure; author; document type
      */
    void saveTheEditor();
	
	 /**
      * Author: Swarup
      * @brief: This signal update the palletLiveModel with parameter Name for which min and max range is to be updated
      */

    void updateSignalRange(QString parameterName,QString minValue, QString maxValue);

    //variables
private:
    Ui::AINGTALivePallete *ui;
    AINGTAGenSearchWidget* _pGenSearchWdgt;
    QList<AINGTAICDParameter> _SetParamList;
    QPointer<AINGTAEditorWindow> _pEditorWindow;
    QThread* _pRPCCallThread;
    QThread* _pCommandsCallThread;
    AINGTAClientCalls* _clientCallHandler;
    AINGTALivePalleteModel* _pLivePalleteModel;
    QMenu* _pContextMenu;
    QSplitter *_pViewEditorGroup;
    AINGTALivePalleteCommands* _pEditorCommands;
    QVariant _blockingResponse;
    QStringList _SSMList;
    QMutex _mutex;
	AINGTANewFileCreationDialog* _pSaveGtaLiveEditorDialog;
    QMenu* _pAcquisitionContextMenu;

protected:
    void keyPressEvent ( QKeyEvent * event );

};

#endif // LIVEPALLETE_H
