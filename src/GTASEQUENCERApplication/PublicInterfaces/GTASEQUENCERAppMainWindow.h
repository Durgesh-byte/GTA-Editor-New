#ifndef AINGTASEQUENCERAPPMAINWINDOW_H
#define AINGTASEQUENCERAPPMAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include "AINGTAServer.h"
#include "AINGTASEQUENCERServerClientComm.h"
#include "AINGTASEQUENCERConfigDbManager.h"
#include "AINGTAUtil.h"

namespace Ui {
    class AINGTASEQUENCERAppMainWindow;
}

class AINGTASEQUENCERAppMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AINGTASEQUENCERAppMainWindow(QWidget *parent = 0);
    ~AINGTASEQUENCERAppMainWindow();

private:
    Ui::AINGTASEQUENCERAppMainWindow *ui;
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);

    /**
      * Author: Priyanka
      * This function updates the list of configurations in load configuration menu with last selected config in top.
      */
    void updateRecentMenu(const QString &iCurrConfigName = QString());

    /**
      * Author: Priyanka
      * This function updates the list of configurations when a new config is added.
      */
    void addConfigToRecents(const QString &iConfigurationName);

    QList<QAction*> _recentConfigs;
    QString _lastConfig;

    /**
      * Author: Priyanka
      * This function to load the list of tools,scripts and IP saved for a particular configuration.
      *iConfigurationName: configuration choosen that has to be loaded.
      */
    void loadConfiguration(const QString &iConfigurationName);
    void setItemIcon(const QString &str);

#if (MAJOR_TOOL_VERSION == 27 && RELEASE_TOOL_VERSION > 1) || (MAJOR_TOOL_VERSION > 27)
    AINGTASEQUENCERServerClientComm * _pServer;
    QThread * _pServerThread;
#endif
    AINGTASEQUENCERConfigDbManager *_pconfigdb;

    QStringList _killScriptsList;
    QStringList _killToolsPIDList;

public slots:
    void close();

    /**
      * Author: Priyanka
      * This function adds new configuration with current list of tools, scripts and IP
      *iConfigurationName: configuration name that is being added newly.
      */
    void onAddNewConfiguration(const QString &iConfigurationName);

    /**
      * Author: Priyanka
      * This function deletes the configuration from database
      *iSelectedItems: selected configurations to be deleted.
      */
    void onDeleteConfig(const QStringList &iSelectedItems);

    /**
      * Author: Priyanka
      * This function dispalys the list of scripts on UI
      *iScriptLst: list of initialization scripts.
      *@return:false if one or more scripts doesn't exist
      */
    bool setInitialisationScripts(const QStringList &iScriptLst);

    /**
      * Author: Priyanka
      * This function dispalys the list of tools on UI
      *iScriptLst: list of tools.
      *@return:false if one or more tools doesn't exist
      */
    bool setTools(const QStringList &iToolLst);
    // void getserverIP(QString &iIP);

    /**
      * Author: Priyanka
      * This function checks if the server is started.
      *if server is not started resets the start server button, else disables the user function to add/remove tools/scripts and modify IP.
      */
    void serverIsRunning(bool status);

#if (MAJOR_TOOL_VERSION == 27 && RELEASE_TOOL_VERSION > 1) || (MAJOR_TOOL_VERSION > 27)
    /**
      * Author: Priyanka
      * This function returns the list of tools currenlty on editor
      */

    QVariantList getTools();

    /**
      * Author: Priyanka
      * This function returns the list of scripts currenlty on editor
      */
    QVariantList getScripts();

    /**
      * Author: Priyanka
      * This function informs thatmultiple instances are running and exits application before starting the server on second instance.
      *iVal: true if multiple instances are running.
      */
    void multipleInstancesRunning(bool iVal);

#endif

private slots:
    void onAddInitTBClicked();
    void onRemoveInitTBClicked();

    void onAddToolTBClicked();
    void onRemoveToolsTBClicked();

    void onServerStatusRBClicked(bool iVal);

    /**
      * Author: Priyanka
      * This function loads the configuration manager where configuration can be added/removed.
      */
    void onConfigurationsManagerSelected();

    /**
      * Author: Priyanka
      * This function saves the current list of tools/scripts and IP to the last configuration loaded.
      */
    void onSaveConfigurations();

    /**
      * Author: Priyanka
      * This function closes the loaded configurtaion and resets the UI to default(empty lists).
      */
    void onCloseConfiguration();

    void onRecentConfigurationSelected();

    /**
      * Author: Priyanka
      * This function removes the list of configurations selected.
      */
    void removeConfigFromRecents(const QStringList &iConfigLst);
    void onWidgetClosed();

#if (MAJOR_TOOL_VERSION == 27 && RELEASE_TOOL_VERSION > 1) || (MAJOR_TOOL_VERSION > 27)
    /**
      * Author: Priyanka
      * This function starts the current list of scripts.
      *@return: log for sequencer window stating the status of each script
      */
    QVariant startScripts();

    /**
      * Author: Priyanka
      * This function starts the current list of tools.
      *@return: log for sequencer window stating the status of each tool
      */
    QVariant startTools();

    /**
      * Author: Priyanka
      * This function kills the running list of tools.
      */
    void stopTools();

    /**
      * Author: Priyanka
      * This function kills the running list of scripts.
      */
    void stopScripts();

    /**
      * Author: Priyanka
      * This function removes list of scripts from current editor.
      */
    void removeScripts(const QStringList &iScriptLst);

    /**
      * Author: Priyanka
      * This function removes list of tools from current editor.
      */
    void removeTools(const QStringList &iToolLst);

    /**
      * Author: Priyanka
      * This function checks for running tools.
      * @return: true if one or more tools are running
      */
    bool checkToolsAreRunning();
#endif
 signals:
    void widgetClosed();

};

#endif // AINGTASEQUENCERAppMainWindowMAINWINDOW_H
