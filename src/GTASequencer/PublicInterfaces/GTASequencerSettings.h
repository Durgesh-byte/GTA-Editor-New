#ifndef GTASEQUENCERSETTINGS_H
#define GTASEQUENCERSETTINGS_H

#include <GTAUtil.h>

#pragma warning(push, 0)
#include <QDialog>
#include <QModelIndex>
#include <QToolButton>
#include <QProgressDialog>
#include <GTAClient.h>
#pragma warning(pop)

#define SEQUENCER_SETTINGS_DIR "SequencerSettings"
#define APP_TOOL_DIR "Tool_Data"

namespace Ui {
    class GTASequencerSettings;
}


struct SequencerSettings
{
    QString schedulrePath;
    int waitTime;
    QStringList initList;
    QStringList toolList;
    QString logFilePath;
    QString scxmlPath;
    int timeout;
    bool initCheck;
    QMap<QString,QString> slaveList; //Qmap of Slave IP and Slave Alias Name
    QList<GTAClient *> slaveClientlist;// to pass connected client objects
};


class GTASequencerSettings : public QDialog
{
    Q_OBJECT

public:
    explicit GTASequencerSettings(const QString &iToolDataPath,const QString &iLogFilePath,const QString &iScxmlPath,QWidget *parent = 0);
    ~GTASequencerSettings();

    void setPaths(const QString &iLogFilePath, const QString &iScxmlPath, const QString &iToolDataPath);
    void show();
    void enableDisableSettings(bool iVal);

    static GTAClient * _client;
    static QList<GTAClient *> _clientLists;
    static QList<GTAClient *> _ConnectedclientLists;
    static bool _IsServerConnected;
private slots:
    void onAddInitTBClicked();
    void onRemoveInitTBClicked();

    void onAddToolTBClicked();
    void onRemoveToolsTBClicked();
    void onRemoveSettingTBClicked();

    void onSchedulerPBClicked();

    void onApplyClicked();
    void onClearClicked();
    void onUserSettingSelected(const QModelIndex &index);
    void onSaveUserSettingToggled(bool iVal);
    void onSaveUserSettingClicked();
    void onInitializationCheckClicked(bool );
    void onAddSlaveTBClicked();
    void setSequencerSlaves(QString IP,QString Name);
    void onRemoveSlaveTBClicked();
    void onPushButtonClicked();
    void onServerClientConnection(QVariant &iVal,  QNetworkReply * ireply);
    void onServerClientConnectionFailed(int , const QString &,  QNetworkReply * ireply);
    void onServerDisconnection(QVariant &,  QNetworkReply * ireply);
    void onServerDisconnectionFailed(int , const QString &,  QNetworkReply * ireply);
    void onDeleteServers(QVariant &,  QNetworkReply * ireply);
    void onDeleteServersFailed(int , const QString &,  QNetworkReply * ireply);
private:
    void setInitialisationScripts(const QStringList &iScriptLst);
    void setTools(const QStringList &iToolLst);
    QStringList getInitList();
    QStringList getToolList();
    bool writeSetting(const QString &iSettingName, const QString &iFilePath);
    void readSettingsFromPath();

    /**
      * This function returns the list of connected Slave IPs and their Alias.
      * @return: Slave IP and Alias in QMap
      */
    QMap<QString,QString> getSlaveList();

    int _rowCount;
    QList<int> _ports;
    QMap<QString,int> _serverConnectionFailure;
    QProgressDialog * _progressDialog;
    void showProgressDialog(QString imsg);
    void hideProgressDialog();
    void removeConnectedClient(QString IP);

signals:

    void settingsApplied(const SequencerSettings &iSetting);

//    void settingsApplied(const QString &ischedulerPath,const int &iWaitTime,const QStringList &iScriptList,
//                         const QStringList &iToolList, const QString &iLogPath, const QString &iScxmlPath, const int &iTimeout);

private:
    Ui::GTASequencerSettings *ui;
    QString _logFilePath;
    QString _scxmlFilePath;
    QString _schedulerPath;
    int _initWaitTime;
    int _timeout;
    QString _ToolDataPath;
};

#endif // GTASEQUENCERSETTINGS_H
