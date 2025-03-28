#ifndef GTASETTINGSWIDGET_H
#define GTASETTINGSWIDGET_H

#include <QWidget>
#include <QHash>
#include <QKeyEvent>
#include <QList>
#include <QString>
#include "GTAAppController.h"
#include <QSortFilterProxyModel>
#include "GTASqlTableModel.h"
#include <QSqlRecord>
#include <QSqlTableModel>
#include "GTADataElementFilterModel.h"


namespace Ui {
class GTASettingsWidget;
}

class GTASettingsWidget : public QWidget
{
    Q_OBJECT

public:
    typedef QHash<QString,QString>  THshPropertyVal;

    enum DBUpdateType{DO_NOTHING, APPEND, UPDATE};
    enum tabs{PATH_SETTINGS=0,GEN_SETTINGS,SCXML_SETTINGS,DB_SETTINGS,CMD_SETTINGS,EQUIPMENT_SETTINGS};
    explicit GTASettingsWidget(QWidget *parent = 0);
    ~GTASettingsWidget();
    void setInitialSettingFlag(bool flag){_isInitialSettingWindow=flag;} 

    void loadSettings();
    bool getSaveElementForSCXML();
//    bool getRepeatedLogingSetting();
    bool getChannelSelectionSetting();
    bool getPirInvokeInSCXML();
    bool getLoopTimeOutState();
    bool getStandaloneModeExec();
    bool getEnInvokeForLocalVar();
    bool getGenericSCXMLState();
    bool getNewSCXMLState();
    QString getDecimalPlaceSelected();
	/**
      * @brief set - which log message is selected by the user from the combo box.
      *	@return which option is selected from the checkbox.
      */
	QString getLogMessageSelected();
	/**
      * @brief set - which log functionality has been set or not.
      *	@return log is checked or not.
      */
    bool getLogEnabled();
    bool getVirtualPMR();
    bool getResizeRow();
    bool getPIRWaitTimeSetting(QString&);
    DBUpdateType updateDBStatusForTestConfig();
    bool isTestConfigDataUpdate()const;
    QString getTestConfigFilePath()const;
    bool isConfigFileUpdateDB()const;
    QString getEngineTypeSelection();
    void show();
    void setDefaultLogDirPath(const QString &iPath);
    bool isBenchDbStartup() const;
    bool isProcDbStartup() const;
    bool getLTRAPrintTableStatus();
    void submit();

    void enEquipTab(bool iEnEqTab = false);
    void refreshEquipTable();
    void setEquipmentToolName(const QHash<QString, QStringList> iEquipmentToolName);

    bool getNewLogFormatStatus();
    bool getInitSubscribeStartStatus();
    bool getInitSubscribeEndStatus();

    bool getManualActionTimeoutStatus();
    QString getManualActionTimeoutValue();
    bool getSubscribeUnsubscribeTimeoutStatus();
    QString getSubscribeUnsubscribeTimeoutValue();
    bool getUSCXMLBasedSCXMLCBStatus();
    QString getDebugModeIP();
    int getDebugModePort();
    void setisInitialSettingWindow(const bool& isInitsettingWindow);
    bool getValidationEnabler(); //Check whether validation is enabled by the user
    /**
      * @brief: returns list of all rows in the ToolListTW (QTableWidget)
      * @return: all the rows in the table widget in a list of structure (GTALiveToolParams)
      */
    QList<GTALiveToolParams>& getLiveViewToolList();
    /**
      * @brief: returns the bisg assigned IP
      * @return: the 1st column of the 0th row of the table widget
      */
    QString getGtaLiveBisgIP() const;
    /**
      * @brief: returns the bisg assigned port
      * @return: the 2nd column of the 0th row of the table widget
      */
    int getGtaLiveBisgPort() const;
	inline QString getCurrentLibFilePath() const { return _currLibFilePath; }
	inline QString getCurrentRepoFilePath() const { return _currRepoFilePath; }
    inline QString getCurrentToolDataPath() const { return _currToolDataPath; }
    inline QString getGTAControllerPath() const { return _currGTAControllerPath; }
    inline QString getAutoLauncherWatchFolderPath() const{ return _currAutoLaucherPath; }

signals:

    void cancel_settings();

    void settingsFinished(bool isLibPathChanged, bool isRepoPathChanged, bool isToolDataPathChanged, bool rebuildDb);
	void settingsOKPB(QString dataPath, QString repoPath);
    void gitDbChanged();
    void newToolIdsAdded(const QList<GTASCXMLToolId> &iList);
    void removeToolID(const QList<GTASCXMLToolId> &iList, const GTASCXMLToolId &iRemoveObj);
    void allEquipmentsUpdated(const QList<GTAEquipmentMap> &iList,const QStringList &iUpdateQuery);
    /*
        Created a emit signal when ever user changes the value from the combo box.
    */
    void logLevelUpdated(QString ilogLevel);

protected:
    void keyPressEvent ( QKeyEvent * event );
    void closeEvent(QCloseEvent *event);
    void setBatchConfigDBFiles(const QString& iIniFilePath);
private slots:
    void onBrowsePBClick();
    void onAddPBClick();
    void onDeletePBClick();
    void onOKPBClick();
    void onCancelPBClick();
    void onPIRWaitTimeCBToggeled(bool);
    void onChannelSelectionChanged(bool);
    void onTabChanged(int);
    void onDecimalPrecisionSelected(QString iVal);
    void onLibPathTextChanged(const QString&);
    void onLibPathEdited();
    void onRepoPathTextChanged(const QString& );
    void onRepoPathEdited();
    void onToolDataPathTextChanged(const QString&);
    void onToolDataPathEdited();
    void onGitDbTextChanged(const QString&);
    void onGitDbPathEdited();
    void onGTAControllerTextChanged(const QString& value);
    void onGTAControllerPathEdited();
    void onAutoLauncherTextChanged(const QString& value);
    void onAutoLauncherPathEdited();
    void onAutoLauncherConfigTextChanged(const QString& value);
    void onAutoLauncherConfigPathEdited();
    void onEquipmentSave();
    void onEditEquipmentSettings(const QModelIndex &index);
    void onEquipmentApplyAll();
    void onAddNewToolName();
    void onRemoveToolName();
    void onSearchEquipment();
    void onClearSearchBox();
    void onSearchTypeChanged(int);
    void onTypeCBChanged(int);
    void onManualActionTimeoutCBToggled(bool iVal);
    void onSubscribeUnsubscribeTimeoutCBToggled(bool iVal);
    void onRefreshTBClicked();
    void onUSCXMLBasedSCXMLCBSelected(bool);

public slots:
		void onUpdateModelFinished();
		void showProgressbar(bool iVal);

private:

    bool _configuration_ok;

    void deleteProgram(const QString & iProgram);
    void deleteEquipment(const QString & iProgram, const QString & iEquipment);
    void deleteStandard(const QString & iProgram, const QString & iEquipment, const QString & iStandard);
    void updateCommandSettings();
    void updateEquipmentSettings();
    void createEquipmentModel();

    void getAttributeNameValForNode(const QDomElement& iElement, QHash<QString,QString> & oHashOfNameValue);
    void commitCommandSettings();

    void saveSettings();

    bool getCommandPropertyTable( QHash<QString, THshPropertyVal> & oPropertyTable);
    QString findToolType(const QString &iStr);
    QString getType();
    int getTypeColID(const QString &iType);
    void hideMappingTWCols();
    void submitEquipmentData();
	

    Ui::GTASettingsWidget *ui;


    bool _isInitialSettingWindow;

    bool _configFileUpdateDB;

    int _TabChanged;
    bool _LibPathChanged;
    bool _RepPathChanged;
    bool _ToolDataPathChanged;
    bool _GitDbPathChanged;
    bool _GTAControllerPathChanged;
    bool _AutoLauncherPathChanged;
    bool _AutoLauncherConfigPathChanged;
    bool _GTAControllerOutputLogPathChanged;
    bool _isRemoveOldEntries;
    QString _iniFileSavePath;
    QString _currLibFilePath;
    QString _currRepoFilePath;
    QString _currToolDataPath;
    QString _currGitDbPath;
    QString _currGTAControllerPath;
    QString _currAutoLaucherPath;
    QString _currAutoLaucherConfigPath;
    QString _defaultLogDirPath;
    QList<GTASCXMLToolId> _toolIdList;
    GTASCXMLToolId _defaultToolVal;
    QStringList _Tools;
    bool _newToolAdded;
    int _EquipmentTabCnt;
    GTASqlTableModel *_pEquipmentModel;
    GTADataElementFilterModel * _pFilterModel;
    QList<GTALiveToolParams> _liveViewToolList;
};

#endif // GTASETTINGSWIDGET_H
