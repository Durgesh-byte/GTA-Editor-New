#ifndef GTAGENSEARCHWIDGET_H
#define GTAGENSEARCHWIDGET_H

#include "GTAMessage.h"
#include "GTAAppController.h"
#include "GTAFilterModel.h"
#include "GTAParamDetails.h"
#include "GTAGenericDB.h"
#include "GTAICDParameter.h"

#pragma warning (push, 0)
#include <QWidget>
#include <QList>
#include <QHash>
#include <QKeyEvent>
#include "QStandardItemModel"
#include "QSqlQueryModel"
#pragma warning (pop)

class GTAICDParameter;
namespace Ui {
    class GTAGenSearchWidget;
}

class GTAGenSearchWidget : public QWidget
{
    Q_OBJECT

public:
    enum ElementType {PROCEDURE,PARAMETER,OBJECT,SEQUENCE, TEMPLATE, FUNCTION,IMAGE,kWarning,BITE,FWC,REPORT,IRT_OBJECT,ROBO_ARM_OBJECT,ONE_CLICK, RELEASE,AUDIO_RECOG,DUMP_LIST,ONECLICK_LAUNCH_APP,RESULT_SEQ,RESULT_PRO,PRINT_TABLE,FAILURE_COMMANDS,GEN_PARAMETER,RELEASE_PIR_SEARCH,INIT_PIR_SEARCH,USER_DEF};
    enum SubSearchType {PARAMETER_SEARCH,LOCAL_SEARCH,SUBSCRIBE_LOCAL_SEARCH, ACT_CHK_INTERNAL_VAR,INVALID_SEARCH,LIVEMODE_PARAMETER_SEARCH,EXTRA_PARAMETER_FOREACH};
    explicit GTAGenSearchWidget(QWidget *parent = 0);
    ~GTAGenSearchWidget();
	
	/**
	*  Adding  default subsearch type as PARAMETER_Search, and For GTA Live Mode, it is LIVEMODE_PARAMETR_SEARCH
	*/
    void setSearchType(ElementType iSearchType,SubSearchType iSubSearchType = PARAMETER_SEARCH);
    ElementType getSearchType() const;
    void setSource(const QList<GTAICDParameter> & iSource);
    void setSource(const QHash<QString,QString> & iImageList);
    void setSource(const QList<GTAMessage> & iMsgList,ElementType iType);
    void excludeItemsFromSearch(QStringList iExcludeItems);
    GTAICDParameter getSelectedItems() const;
    QList<GTAICDParameter> getSelectedItemList() const;
    QString getUserChannelSelection(){return _channelSelected;}

    ElementType stringToSearchType(const QString &iSearchType);
    void setUserDefDb(const GTAGenericDB &iGenDbObj);

    
    GTAFilterModel * getSearchItems(const QString iExtn, bool ignoreUsedDocs = false,bool isResultView=false);
    GTAFilterModel * getIRTSearchItems();
    GTAFilterModel * getRoboArmSearchItems();
    GTAFilterModel * getOneClickSearchItems();
    GTAFilterModel * getOneClickLaunchAppSearchItems();
    GTAFilterModel * getAudioRecogSearchItems();
    GTAFilterModel * getReleaseParamSeachItems();
    GTAFilterModel * getParamModel(QString Query);
    GTAFilterModel * getLocalVarModel(bool isSubscribeLocal = false);
    GTAFilterModel * getSearchModel(QStringList items);
    GTAFilterModel * getSearchModel(QHash<QString, QString> items);
    GTAFilterModel * getFwcItems();
    GTAFilterModel * getReleaseVariables();
    GTAFilterModel * getPrintTables();
    GTAFilterModel * getFailureCommands();
    GTAFilterModel * getCmdInternalVarSearchItems();
    GTAFilterModel * getUserDefinedDatabaseRecords(const QString &iPath);

    void searchFile(QString FileName, ElementType iSearchType);


protected:
    void keyPressEvent ( QKeyEvent * event );
signals:
    void okPressed();
    void cancelPressed();
    void queryChanged(int);
    void fetchQueryResults(QString);
    void changeLabel(int,QString,bool);
public slots:
    void show();

private slots:
    void showParamDetails();
    void setCurrentConfiguration();
    void onOKPBClicked();
    void onOKPBClicked(const QModelIndex &iModelIndex);
    void onCancelClicked();
    void onProgramChange(const QString & iSelectedProgram);
    void onEquipmentChange(const QString & iSelectedEquip);
    void onStandardChange(const QString & iSelectedStd);
    void onSelectionTableSelectionChange();
    void searchFunction();
    void clearSearch();
    void getParamModelForType(SubSearchType subSearchType);
    void disableOtherChecks(bool bCheckStatus);
    void onSearchVMACEnabled(int state);
    void onSearchInternalVarEnabled(int index);
    void getParamModelForType(bool isLocal);
    void showHideActionSelector(bool showhide);
    void onSelectAllPressed();
    void onUserDefinedDBClicked(bool iVal);
    void onUserDBValChanged(const QString &iCurrentText);
    void showDBSearchLimiters(bool iVal);
    void showResultPages(int iVal);
    void QueryProcessing();
    void onLabelChange(int,QString,bool);
	void onActionChange();
	void setKnownVars(QStringList);
	QStringList getKnownVars();

private:

    void getCommandList(ElementType iSearchType);
    void initMembers();
    void initializeModels();
    void setUIForParameterSearch();
    Ui::GTAGenSearchWidget *ui;
    GTAFilterModel * _FilterModel;
    QStandardItemModel _StandardItemModel;
    QSqlQueryModel * _SqlQueryModel;
    ElementType     _CurrentSearchType;
    SubSearchType _SubSearchType;
    QStringList     _ProgramList;
    QHash<QString,QStringList> _EquipmentList;
    QHash<QString,QStringList> _StandardList;
    QStringList _ExcludedItems;
    GTAAppController * _pAppCtrl;
    GTAParamDetails _detail;
    GTAGenericDB _UserDb;                    // this member is for external tool command
    QList<GTAGenericDB> _UserDBSearchList;       // this list is for user defined DB search option

    GTAICDParameter _selecteItem;
    QList<GTAICDParameter> _selectedItems;
    QString _channelSelected;

    QString getUserChannelSelection(const QString& iParam, bool & oValidity) const;
    bool processSelectedItem(const QModelIndex &iCurrentIndex);
    bool processSelItemForUserDefDb(const QModelIndex &iCurrentIndex);
    QStringList getUserDBNameList()const;
    bool getGenDBFromList(const QString &iGenDBName, GTAGenericDB &oUserDB)const;
    GTAICDParameter updateSelectedItemStruct(const QStringList &iItems);
	void getFileFromFileType(const QString& iFileType, QString & oFileType) const;
	void getSignalTypeFromMedia(const QString& iMedia, QString & oSignalType) const;
    void enablePageSearchOptions(bool iVal);
    int _lastindex;
    QString _lastLikeQuery;
	QStringList _knownVars;

};

#endif // GTAGENSEARCHWIDGET_H
