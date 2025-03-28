#ifndef GTAAPPCONTROLLER_H
#define GTAAPPCONTROLLER_H

#include "GTAControllers.h"
#include "GTASystemServices.h"
#include "GTAMessage.h"
#include "GTAUtil.h"
#include "GTAViewController.h"
#include "GTAElement.h"
#include "GTADataController.h"
#include "GTAOneClickTestRig.h"
#include "GTAHeader.h"
#include "GTAFWCData.h"
#include "GTAControlDS.h"
#include "GTAOneClickLaunchApplicationTestRig.h"
#include "GTACommandConfig.h"
#include "GTAGenericToolController.h"
#include "GTAGenericToolCommand.h"
#include "GTAActionParentTitle.h"
#include "GTATreeItem.h"
#include "GTAEquipmentMap.h"
#include "GTASCXMLToolId.h"
#include "GTASqlTableModel.h"
#include "GTAActionPrintTable.h"
#include "GTAActionFTAKinematicMultiOutput.h"
#include "GTAPluginInterface.h"
#include "GTAEditorLogModel.h"
#include "GTASequencerMain.h"
#include "GTASVNController.h"
#include "GTADbDataController.h"

#pragma warning(push, 0)
#include "QSqlDatabase"
#include <QByteArray>
#include <QPluginLoader>
#include <QFileInfo>
#include <QSqlTableModel>
#include <QProcess>
#include <QObject>
#include <QList>
#include <QTableView>
#include <QHash>
#include <QVariant>
#include <QtXml/QDomNodeList>
#include <QMutex>
#include <QString>
#include <QSqlQueryModel>
#include <QSortFilterProxyModel>
#pragma warning(pop)

#include "GTAActionWidgetInterface.h"

//TODO: This is a warning against assigning Rvalues to references, which is done in default arguments for many methods in this file,
//      This will be fixed in the future, as this refactor will take some time (there are a lot od default arguments which will be broken by removing default argument from some of the refs
#pragma warning (disable : 4239)

class GTAICDParameter;
class GTABaseModel;
class GTACommandBase;

struct GTAControllers_SHARED_EXPORT LaunchParameters
{
    QString elementName;
    QString errorMsg;
    bool hasUnsubscribeStart;
    bool hasUnsubscribeEnd;
    bool hasUnsubscribeFileParamOnly;
    QStringList varList;
    int mode;
    QString uuid;
};

struct GTAControllers_SHARED_EXPORT CommandInternalsReturnInfo
{
    CommandInternalsReturnInfo() { pCmd = nullptr; }
    QString location;
    QString funcName;
    QString retName;
    QString retType;
    QString toolName;
    QString cmdName;
    QString desc;
    QString refLoc;
    GTACommandBase *pCmd;
};

struct GTAControllers_SHARED_EXPORT LogModelInput
{
    QString docFileName;
    QString logFileName;
};

struct GTAControllers_SHARED_EXPORT LogModelSequenceIO
{
    QString SeqLogFile;
    QString SeqDocFile;
    QStringList procNames;
    QStringList logNames;
    QStringList status;
    QString globalStatus;
//    QList<QAbstractItemModel*> pModels;
};

struct GTAControllers_SHARED_EXPORT LogModelOutput
{
    LogModelOutput() { pModel = nullptr; pValidationModel = nullptr; }
    QStringList columnList;
    QString hasFailed;
    QString logErrorMessage;
    bool returnStatus;
    QAbstractItemModel * pModel;
    QAbstractItemModel * pValidationModel; //To display the list of functions/procedures dialogs need to be vaidated #327185
    QHash<QString, QString> ioLogInstanceIDMap;
};

struct GTAControllers_SHARED_EXPORT ExportLTRAParams
{
    QString saveInMilliseconds;
    QString saveFormat;
    bool isLTRA;
    QHash<QString, QString> ioLogInstanceIDMap;
};


//struct GTAControllers_SHARED_EXPORT PluginOutput
//{
//    QStringList errors;
//    QStringList newFolders;
//    bool retVal;
//};


/*
  This struct has been created to manage the input parameters
  sent to SVN controller functions.
*/
struct GTAControllers_SHARED_EXPORT SVNInputParam
{
    QString command;
    QModelIndex cmdIndex;
    QModelIndexList cmdIndexList;
    QStringList userCredential;
    QString resolveOption;
    QString fileName;
    QStringList fileList;
    QString commitMessage;
};

struct GTAControllers_SHARED_EXPORT SearchResults
{
    QHash<int,QString> foundRows;
    QHash<int, QString> foundTags; //HEADERS
    QString uuid;
};

//! Application controller class
/*!
    This singleton class functions as :
        -delegating user actions to appropriate components.
        -executing user actions using other controllers.
*/
class GTAControllers_SHARED_EXPORT GTAAppController : public QObject
{

    Q_OBJECT


public:
    enum TreeViewActionType {Save, Rename, Delete};
    enum DisplayContext{EDITING,COMPATIBILITY_CHK,SEARCH_REPLACE};
    enum ExecutionMode{NORMAL_MODE,SEQUENCER_MODE,DEBUG_MODE, UNKNOWN};

public:

    struct InternalParameterInfo
    {
        InternalParameterInfo():paramName(QString()),paramType(QString()){}
        QString paramName;
        QString paramType;
        //        QString trueCond;
    };

    struct ParameterReturnInfo
    {
        ParameterReturnInfo():name(QString()),returnType(QString()){}
        QString name;
        QString returnType;
    };
    struct CommandInternalParameterInfo
    {
        CommandInternalParameterInfo():commandName(QString()),complement(QString()){}
        QString commandName;
        QString complement;
        ParameterReturnInfo commandReturnType;
        QList<InternalParameterInfo> internalParamInfoLst;
        bool operator ==(const CommandInternalParameterInfo &iRhs)
        {
            QString fullCmdName= QString("%1 %2").arg(commandName,complement).trimmed().replace(" ","_");
            QString iFullCmdName= QString("%1 %2").arg(iRhs.commandName,iRhs.complement).trimmed().replace(" ","_");
            if(fullCmdName == iFullCmdName)
                return true;
            else
                return false;
        }
    };

    struct ComplexItemType
    {
        ComplexItemType():name(QString()),type(QString()){}
        QString name;
        QString type;

    };

public:
    

    //! returns the instance of application controller to any component requesting it.
    /** The function return the list of action and complements associated with it
    *   @parameter 1,iType: Element type 
    */
    static GTAAppController* getGTAAppController(QObject* ipObject=0);
    //!Destructor
    ~GTAAppController();

    /**
    * sets program configuration and updates the changes to application settings.
    * @param ibSaveElementDuringExportScxml option to save current element before exporting scxml .
    * @param ibRepeatLoggingForLog In LTRA repeated log will appear for while statements, this can be turned off using this setting.
    * @param iEngineSetting Engine settings options include RR, CFM and PW [ENGINE_SELECTION]
    * @param ibSVNCommitOnexit changes settings for commiting the svn in data folder on exiting application.[SVN_COMMIT_ON_EXIT]
    * @param ibSVNUpdateOnStart changes setting for updating the data folder on start of application (during initialization)
    * @param isSVNAddOnExit this sets application variable for adding newly created elements to svn on exiting application.
    * @param iPirInvokeForSCXML this sets application for generating nested invokes due to presence of PIR parameters
    * @return The status
    */


    bool setProgramConfiguration();
    bool setProgramConfiguration(
        const QString& iDecimalPlace,
        const QString& iLogMessage,
        bool iLogEnable,
        bool ibSaveElementDuringExportScxml,
        bool ibChannelSelectionSetting,
        const QString& iEngineSetting,
        bool iPirInvokeForSCXML,
        bool ibGenericSCXMLState,
        bool iPirWaitTime,
        const QString& iTime,
        bool isNewSCXMLStruct,
        bool iVirtualPMR,
        bool iResizeRow,
        bool iStandaloneExecMode,
        bool iBenchDbStartup,
        bool iProcDbStartup,
        bool iPrintTableStatus,
        bool iEnInvokeForLocalVar,
        bool iNewLogFormat,
        bool iInitStartSubscribeStart,
        bool iInitStartSubscribeEnd,
        const QString& iManualActTimeoutVal,
        bool iManualActTimeoutStat,
        bool iUSCXMLBasedSCXMLEn,
        QString idebugIP,
        int idebugPort,
        bool iValidationWidgetEnabled,
        QList<GTALiveToolParams>& iLiveToolList,
        QString iLiveGtaBisgIP,
        int iLiveGtaBisgPort);

    void setDisplayConsole(bool displayConsole) { _displayConsole = displayConsole; };
    bool isGenericSCXMLMode();
    //! returns header name for current element.
    QString getHeaderName()const;
    //! returns error for last operation.
    QString getLastError()const{return _LastError;}
    /**
    * This method sets the last error message
    * @param error is the message string.
    */
    void setLastError(const QString& error) { _LastError = error; }
    //! closes database on search query is done
    void closeDatabase();
    /**
    * This method return the list of headers from database 
    * @return list of header pointers
    */
    QList<GTAHeader *> getHeaderList();
    /**
    * Save the header file in database
    * @param isName save name for header.
    * @return status true if exists else false
    */
    bool saveHeader(const QString& isName)const;
    /**
    * Checks if header by name exists in application folder
    * @param isName name of header.
    * @return status true if exists else false
    */
    bool headerExists(const QString& isName)const;

    /**
    * Header with mandatory fields filled will return true
    * @param oEmptyManField sets the fields which are empty.
    * @return status true if all mandatory fields are filled else false
    */
    bool headerComplete(QStringList& oEmptyManField);
    
    /**
    * This method provides model for a selected header type
    * @param ieType type of editor i.e {Object,Procedure,Function,Sequence,Template,Invalid}.
    * @return returns pointer to abstract model if successfully created otherwise returns NULL.
    */    
    QAbstractItemModel* getModel(const EditorType& ieType);
    
    /**
    * checks weather header has been changed since load.
    * @return returns true if header has been edited else return false.
    */
    bool headerInEditorMode();
    /**
    * This method provides header name associated with an element that is loaded.
    * @param isFileName type of editor i.e {Object,Procedure,Function,Sequence,Template,Invalid}.
    * @return returns pointer to abstract model if successfully created otherwise returns empty name.
    */    
    QString getHeaderNameFromEditorDoc(const QString& isFileName);
    
    /**
    * Sets initial header view after creating header model
    * @param ipView view on which model is to be set
    * @return returns true if header has been set.
    */
    bool setInitialHeaderView(QTableView* ipView);
    /**
    * Sets initial editor view after creating editor model
    * @param ipView view on which model is to be set
    * @return returns true if editor has been set to view.
    */
    bool setInitialEditorView(QTableView* ipView);

    //! updates parameters into database
    void updateParameters();
    //! returns view controller instance
    GTAViewController* getViewController(void);

    //! returns image list.
    QHash<QString,QString> getImageList(const QString & iProgramName,const QString & iEquipment,const QString & iStandard) const;
    //! provides message list.
    QList<GTAMessage> getMessageList(const QString &iProgramName,GTAMessage::MessageType iType) const;

    
    /**
    * This function save object, function , procedure, sequence & templates
    * @param iName name of the procedure to be saved
    * @param iOverWrite if over write if exists
    * @param ipElement pointer to the element to be saved
    * @param ipHeader pointer to the header to be saved along with element.
    * @return true of save succeeded otherwise false
    */
    bool saveDocument(const QString & iName, bool iOverWrite,GTAElement * ipElement,  GTAHeader* ipHeader,const QString &iDataDirectory = QString());

    /**
    * This function save object, function , procedure, sequence & templates that is currently loaded
    * @param iName name of the procedure to be saved
    * @param iOverWrite if over write if exists
    * @return true of save succeeded otherwise false
    */
    QString getLoadedElemPath()const;
    //! updates editor view 
    void updateEditorView();

    //! clears contents in editor.
    void clearAllinEditor();


    /**
     *adds an empty row to the selected row.
     *@param iSelectedRow the row under which new empty row has to be added.
    */
    void addEditorRows(const int& iSelectedRow);
    /**
    *deletes the selected row.
    *@param lstSelectedRows list of rows that are to be removed.
    */
    void deleteEditorRows(const QList<int>& lstSelectedRows);
    /**
    *edits an existing action by changing its contents
    *@param iRow the row which has to be edited.
    *@param value qvariant holding the pointer to action item
    *@returns true if edit is success
    */
    bool editAction(const int& iRow, const QVariant& value);
    /**
    *edits an existing check by changing its contents
    *@param iRow the row which has to be edited.
    *@param value qvariant holding the pointer to check item
    *@returns true if edit is success
    */
    bool editCheck(const int& iRow,const QVariant& value);
    /**
    *TODO: this command needs to be deprecated as user editability is commands property and can be used directly.
    * Checks if a given command can be edited by user.
    * @returns true if editing can be done on a command
    */
    bool isEditableCommand(GTACommandBase* pCmd);
    /**
    *Checks if a command is balanced. by balanced it means a list of command should not have end
    *statements without its parent selected.
    *@param iLstSelectedRows of selected rows
    *@returns true if they are balanced.
    */
    bool areSelectedCommandsBalanced(QList<int>& ioLstSelectedRows);
    /**
    *checks if any of the commands in row has childrens
    *@returns true if any command in list of row has children.
    */
    bool areSelectedCommandsNested(const QList<int>& iLstSelectedRows);

    /**
    *sets the copied row to app controller.
    *@return returns true if copy is success fails if the selection is unbalanced.
    */
    bool setEditorRowCopy1(QList<int>& lstSelectedRows,const QList<GTACommandBase*> &iCmdList);
    bool setEditorRowCopy(QList<int>& lstSelectedRows, bool isNewCommand);

    /**
    *Exports the current loaded element into SCXML
    *@param fileName name of element that has to be saved.
    *@param iFilePath complete path of the scxml file.
    *@param iHasUnsubscibeAtStart if unsubscribe node has to be added at begining
    *@param iHasUnsubscibeAtEnd if unsubscribe node has to be added at end
    *@param iVarList if there are already variables that need subscription or usage in SCXML
    *@return true if export has been successful
    */
    //bool exportToScxml(const QString & fileName, const QString & iFilePath,bool iHasUnsubscibeAtStart,bool iHasUnsubscibeAtEnd);
    bool exportToScxml(GTAElement* pElement, const QString& fileName, const QString& iFilePath,
                       bool iHasUnsubscibeAtStart,bool iHasUnsubscibeAtEnd,QStringList& iVarList,
                       GTAAppController::ExecutionMode mode = GTAAppController::NORMAL_MODE);

    /**
    *Exports an existing document into SCXML
    *@param fileName name of element that has to be saved.
    *@param iFilePath complete path of the scxml file.
    *@param iHasUnsubscibeAtStart if unsubscribe node has to be added at begining
    *@param iHasUnsubscibeAtEnd if unsubscribe node has to be added at end
    *@return true if export has been successful
    */
    bool exportDocToScxml(const QString& iDocName, const QString &iFilePath,
                          bool iHasUnsubscibeAtStart,bool iHasUnsubscibeAtEnd, bool fullFilepath = false);

    /**
     * @brief Placeholder to be implemented by I04DAIVYTE-1097
    */
    bool exportCsvToPro(const QString& csvPath, const QString& outputPath);

    /** The function return the list of action and complements associated with it
    *   @parameter iType Element type 
    *   @parameter 2,oOrderList: ouput: order of the action/check as in Config file created element
    *   @returns hash of command and in order
    */
    QHash<QString,QStringList> getActionCommandList(GTAUtil::ElementType iType,QStringList& oOrderList = QStringList());


    /** The function return the list of all check commands
    *   @parameter iType Element type 
    *   @returns list of checks commander
 */
    QList<QString> getCheckCommandList(GTAUtil::ElementType iType);

    /** isSaveAsValid() : The function checks if the given element can be saved as into other type of element
    *   @parameter 1 - ipElement : element which is being saved as and is being checked if converting it to iDestType is valid or not
    *   @parameter 2 - iDestType : it is the type of element, that will be saved for the new element after save as
    *   @returns bool : true if all commands in ipelement are valid to be present in the new type of element i.e iDestType
                        else false is returned
    */

    bool isSaveAsValid(const GTAElement *ipElement, QString &iDestType);

//    QAbstractItemModel* createModelFromDataDirectoryDoc( const QString& sFileName,EditorType isType);
    /**
    *   Opens document contained in data directory.
    *   @param iFileName: file name of the document in data directory.
    *   @param isType: type of document (proc,seq etc)
    *   @param opEditorModel: editor model to be attached with editor TV.
    *   @param opHeaderModel: headerModel to be attached with header editor.
    */
//    bool createModelFromDataDirectoryDoc( const QString& iFileName,EditorType isType , QAbstractItemModel*& opEditorModel, QAbstractItemModel*& opHeaderModel);

    /**
    *   Opens document contained in data directory.
    *   @param iFileName: complete file path with extension(ex: c:\users\...\test.pro).
    *   @param opEditorModel: editor model to be attached with editor TV.
    *   @param opHeaderModel: headerModel to be attached with header editor.
    */
//    bool createModelFromDataDoc( const QString& iFilePath , QAbstractItemModel*& opEditorModel, QAbstractItemModel*& opHeaderModel);


    /** This funciton creates model to be attached with view for header.
      * @iFileName: name of the headerFile
      * @opEditorModel: output: created header model
      * @returns false if model is not created.
      */
    bool createHeaderModelFromDoc(const QString& iFileName, QAbstractItemModel*& opEditorModel);

    QString getAbsoluteFilePathFromUUID(const QString &iUUID);
    QStringList getAllRelFilePathsFromUUID(const QString &iUUID);
	
	 /** This funciton updated validation info i.e. Name, Status, time in document viewer
      * @&iUUID: UUID of the file for which validation info need to be updated
      * @iValidationStatus: Send validation status of the respective file
      * @iValidatorName: Send validator name of the respective validated file
      * @iValidationDate: Keeping a default vaildation date or setting a new one
      */
    void updatevalidationStatusonDV(const QString &iUUID,const QString iValidationStatus,const QString iValidatorName, const QString iValidationDate=QString());

    /**
      * This function open the gta document and provides commands list in opElemenent
      * @param iFileName  file name of the document to be opened e.g. abc.obj
      * @param opElemenet  The document object to be retrieved
	  * @param isCompleteFilePath  if iFileName argument is complete file path (Used for getting GTAElement object for svn Temp file in svn check for modifications)
      * @return true if the document is open successfully otherwise false
      */
    bool getElementFromDocument(const QString & iFileName, GTAElement *& opElemenet, bool isUUID, bool ioOnlyModel = true, bool isCompleteFilePath = false);

    /**
    * This function open the gta document and provides the header part of document in opHeader
    * @param iFileName  file name of the document to be opened e.g. abc.obj
    * @param opElemenet  The document object to be retrieved
    * return true if the document is open successfully otherwise false
    */

    bool getHeaderFromDocument(const QString & iFileName, GTAHeader *& opHeader);

    /**
      * The function delete provide document name, if it is not being used by another document
      * if the iUsedByDocument list is empty and retrun value is false then user does not have permission to delete
      * @param iFileName Document to be deleted from database
      * @param iUsedByDocument retrieve the list of document uses the iFileName
      * return true if iFileName is not used by another document otherwise false
      */
    bool deleteDocument(const QString iFileName, QStringList & iUsedByDocument);

    /**
      * The function provides removal mechanism for files and directories separately
      * if the item is a file then QDir::remove method is applied, else the QDir::removeRecursively is used
      * @param pathToItem is the path to directory which contains the item
      * @param itemName is the name of file or directory to be removed
      * return true if the file is successfully removed
      */
    bool removeItemInFileSystem(const QString& pathToItem, const QString& itemName);

    /**
      * This Function return the list of document which are using the provided document
      * @param iDocFileName The name of file in current configuration
      * return the list of files uses iDocFileName in current configuration
      */
    QStringList getDocumentUses(const QString & iDocFileName);
    //void reAttachView(QTableView* pIview);

    /**
      * This function returns true if One Click Config File is valid, list of Configurations  is found, Test Rig name is present otherwise
      * @param iConfigFile configuration file name
      * @param oTestRig pointer to test rig based upon config file.
      * return false if configuration not found else true;
      */
    bool getOneClickConfigFileDetails(const QString & iConfigFile, GTAOneClickTestRig * oTestRig);
    /**
      * This function return true if IRT sequence file is valid and list of sequence name is found, otherwise false
      * @param iSequenceFileName The name of the sequence file which present in database
      * @param oFunctionList list of sequence name which are available in the file
      */
    bool getIRTFunctionList(const QString & iSequenceFileName, QStringList & oFunctionList);
    /**
      * This function return true if Robotic arm sequence file is valid and list of sequence name is found, otherwise false
      * @param iSequenceFileName The name of the sequence file which present in database
      * @param oFunctionList list of sequence name which are available in the file
      */
    bool getRoboArmFunctionList(const QString & iSequenceFileName, QStringList & oFunctionList);

    /**
      * Deprecated function
      * do not use this function, it causes crash of applicaiton
      */
    bool getIRTFunctionContents(const QString iSequenceFileName, const QString & iFunctionName, QList<QDomNode> & oXmlRpcNode);

    /**
      * This function convert the contents of xml node to textual representation
      * @param iFunctionContents  list of xml node to be represented in as text
      * @param oXmlFileContents textual representation of the input
      * return true if successfully converted xml node to text representation otherwise false
      */
    bool getIRTFunctionContents(const QList<QDomNode>  & iFunctionContents, QStringList & oXmlFileContents);

    /**
      * This function retrieve the textual representation of the xml file
      * iSequenceFileName : xml file
      * oXmlFileContents : text repesentation of file
      * return true if file is valid otherwise false
      */
    bool getIRTFunctionContents(const QString iSequenceFileName, QStringList & oXmlFileContents);

    /**
      * This function convert the xml rpc node to xml document for a provided function
      * iSequenceFileName : xml file where function to be searched
      * iFunctionName : sequence name whose xml rpc node to be converted to new xml doc
      * oXmlDocContents : xml doc representation of xml rpc node
      * return true if succeeded otherwise false
      */
    bool getIRTFunctionContents(const QString iSequenceFileName, const QString & iFunctionName, QByteArray & oXmlDocContents);

    /**
      *This function provide function contents present in list of nodes corresponding to Robotic arm command xml
      *@param FunctionContents input nodes to be scanned for function contents
      *@oContents output list of contents in robotic arm function
    */
    bool getRoboArmFunctionContents(const QList<QDomNode>  & FunctionContents, QStringList & oContents) ;
    bool getRoboArmFunctionContents(const QString iSequenceFileName,const QString & iFunctionName, QList<QDomNode>  & oXmlRpcNodeList);
    bool getRoboArmFunctionContents(const QString iSequenceFileName, const QString & iFunctionName, QByteArray & oXmlDocContents);
    bool detailViewExists(const int& iSelectedRow);

    QStringList getItemsToDisplay(const QString iExtn,bool ignoreUsedByDocs=false, bool isResultView = false, QHash<QString, QString>& docMap = QHash<QString, QString>());
    QStringList getIRTItemsToDisplay();
    QStringList getRoboticArmItemsToDisplay();
    QStringList getOneClickItemsToDisplay();
    QStringList getOneClickLauncAppItemsToDisplay();
    QStringList getAudioRecogItemsToDisplay();
    QSqlDatabase getLocalDB();
    structLoadFileInfo loadICDPMRPIRFilesInDB(bool forceUpdate = false,int TestConfigUpdate = 0);
    bool isDataEmpty();
	/**
      * This function connects and launches the Sequencer Main Window for sequence execution
      * @isMultipleLogs: Argument for sequencer to play single or multiple logs
      * @iScxmlList: List of SCXMLs generated
	  * @iLogLst: Name of Logs
	  * @iFailedList: List of Failed SCXMLs
	  * @iProcList: List of Procedures to be executed
      */
    void executeSequence(bool isMultipleLogs,const QStringList &iScxmlList,
                         const QStringList &iLogLst,const QStringList &iFailedList,
                         const QStringList &iProcList);
    bool launch(LaunchParameters parameters);

    bool launchTitleBasedElement(GTAElement *pTitleElem,const QString &iElemName,QStringList& iVarList);
    bool stopSCXML();


    bool generateScxml(bool isMultipleLogs,GTAElement *pElement,
                       QString iRelativePath,const QList<GTAActionCall *> iCallProcLst,
                       QStringList &oScxmlList,QStringList &oLogLst,
                       QStringList &oFailedList, QStringList &oProcList);

    bool clearRow(int iSelectedRow, QString & oErrMsg, QList<int> &oClearedRowId);
    /**
      * This function return the parameter by provided name
      * return true the parameter if found otheewise false
      */
    bool getParameterByName(const QString &iParamName,GTAICDParameter & oParameter);

    /**
      * This function return the list of variable used in current editor
      * iInScope: If it true then search is performed above the current selection in editor
      * otherwise in all variables in editor
      */
    QStringList getLocalVariables(bool iInScope = true, bool isSubscribeParam = false, int iIndex = -1) ;


	/**
	* This function return the list of variable used in current editor
	* iInScope: If it true then search is performed above the current selection in editor
	* otherwise in all variables in editor. knownVarList contains the name of the variables that have already been selected. 
	*/
	QStringList getLocalVariables(bool iInScope = true, bool isSubscribeParam = false, QStringList knownVarList = QStringList(), int iIndex = -1);

    /**
    * This function creates a tree view model (that is delegated to view controller) 
    * based on row selection in editor.
    *@parameter opModel: The output model
    *@iRow:row selection.
    *@returns true if model is created else false.
    */
    bool getModelForTreeView( QAbstractItemModel*& pModel ,const int& iRow);


    /**
      * Return the user name from the windows environment
      */
    static QString getUserName();

    
    /**
      * This function adds header field under header informatioan (above icd file info rows)
      */
//    bool addheaderField(const QString& iName,const QString& iVal,const QString& iMandatory,const QString& iDescription);
    /**
      * This function shows ICD details of an existing file (pro,fun..etc) if it has header 
      * in case it does not contain header detail will return false
      * @ iFileName: fileName without (extension )
     */
//    bool showICDDetails(const bool& iStatus, const QString& iFileName);
    /**
     * This function gets ICD details from database in form of icd objects, for a given list of parameter triplets.
     * returns false in case any error is encountered.
     * @ iParameterList: list of triplets (non triplets are ignored)
     * @ oParameterList: output icd parameter objects
     */
    bool getICDDetails(const QStringList& iParameterList, QList<GTAICDParameter>& oParameterList);



    bool getFwcData(QList<GTAFWCData> &oFwcData/*,const QString & iProgram, const QString & iEquiment, const QString & iStandard*/) const;

    /**
      * This function provides a log model for a procedure.
      * @iInput: LogModelInput structure with log and proc paths of the sequence
      * @igenerateValidationModel: generate/skip the validation model (true/false)
      * @return: LodModelOutput structure that contains, columns, model, error logs and return status
      */
    LogModelOutput getLogModel(LogModelInput input,bool igenerateValidationModel = false);

    /**
      * This function provides a custom log model for a sequence with multiple logs.
      * @iomodel: LogModelSequenceIO structure for input of log and proc paths of the procedures
      * @return: LodModelOutput structure that contains, columns, model, error logs and return status
      */
    LogModelOutput getLogModelForSequence(LogModelSequenceIO &iomodel);
    bool buildModelFromGenestaFile(const QString& isFilePath,const QString& isErrorFilePath,QAbstractItemModel*& opEditorModel, QAbstractItemModel*& opHeaderModel ,const QStringList & fileList = QStringList());
    bool getFileInfoListUsed(const QString& iFileName, QList <GTAFILEInfo> & lstFileInfo);
    /**
      * This function export the lab test report
      * @ipLogModel: Model in UI to be exported, it should of type GTAEditorTreeModel class
      * @iReportFile: Report file full path where the report is created
      * @iAttachmentList: list of documents to be attached with report
      * @iDocType: Format of report, supported iDocType are 0-xml, 1-csv
      */
    bool exportLTRAToHTML(QAbstractItemModel * ipLogModel,const QString & iReportFile,const QStringList & iAttachmentList, int iDocType = 0,bool isLTR =false);
	    /**
      * This function export the lab test report to docx format
      * @ipLogModel: Model in UI to be exported, it should of type GTAEditorTreeModel class
      * @iReportFile: Report file full path where the report is created
      * @iAttachmentList: list of documents to be attached with report
      * @exportLTRA: struct to update the parameters used for exporting LTRA containing save format, timing precission, isLTRA
      */
    bool exportLTRAToDocx(QAbstractItemModel * ipLogModel,const QString & iReportFile,const QStringList & iAttachmentList, ExportLTRAParams exportLTRA = ExportLTRAParams());

    /**
      * This function export the lab test report for Multiple Logs in a sequence
      * @ipLogModel: Model in UI to be exported, it should of type GTAEditorTreeModel class
      * @iReportFile: Report file full path where the report is created
      * @iAttachmentList: list of documents to be attached with report
      * @iomodel: structure containing sequnce name and proc and log paths for multiple logs
      * @exportLTRA: struct to update the parameters used for exporting LTRA containing save format, timing precission, isLTRA
      * @return: true/false boolean
      */
    bool exportLTRAToDocxForMultipleLogs(QAbstractItemModel *ipLogModel, const QString &iReportFile, QStringList &iAttachmentList,LogModelSequenceIO iomodel, ExportLTRAParams exportLTRA = ExportLTRAParams());

    /*
     * returns current data directory based on current settings 
    */
    QString getGTADataDirectory()const;

    QString getGTAScxmlDirectory()const;

    /*
     * returns current log directory based on current settings
    */
    QString getGTALogDirectory();

    /*
     * returns current table directory based on current settings
    */
    QString getGTATableDirectory();

    QString getGTADbFilePath();
    /*
     * returns current element export directory based on current settings
    */
    QString getExportDocDirectory();
    /**
      * This function checks the editor content
      * for valid details.
      * Report is generated and will be displayed by the HMI window.
      * Details Checked
      * - Parameter used, exists in Database.
      * this function calls the private function "checkCommandCompatibility"
      */
    ErrorMessageList CheckCompatibility();


    /**
    * This function returns the temporary directory in repository where GTA can save/use file temporarily
    */
    QString getTempDataDirectory();
    QString getGTALibraryPath();


    QString getScxmlDirectory();
    /**
    *This function Cleans temporary directories files
    */
    void cleanTemporaryDirectory();
    /** 
    *Checks if a row in a header table is editable or not.
    *@iRow row to be identified as editable
    *@return true if row can be edited else false.
    */
//    bool isHeaderRowEditable(const int& iRow);
    /** 
    *Checks if a row in a header table is deletable or not.
    *@iRow row to be identified as editable
    *@return true if row can be deletable else false.
    */
//    bool isHeaderRowDeletable(const int& iRow);
//    bool getHeaderFieldInfo(const int& iRow, QString& iFieldName, QString& iFieldVal, QString& iMandatoryInfo, QString& iFieldDescription) ;
//    bool editHeaderField(const int& iRow, const QString& iName,const QString& iVal,const QString& iMandatory,const QString& iDescription);
//    bool removeHeaderItem(const int& iRow);
//    bool moveHeaderRowItemUp(const int& iRow);
//    bool moveHeaderRowItemDown(const int& iRow);

//    bool isEditorRowMovableUpwards(const int& selectedItem);
//    bool moveEditorRowItemUp(const int& selectedItem);
//    bool isEditorRowMovableDownwards(const int& selectedItem);
//    bool moveEditorRowItemDown(const int& selectedItem);
//    bool searchHighlight(const QRegExp& searchString,QList<int>& iSelectedRows,bool ibSearchUp,bool singleSearch,bool ibTextSearch,QModelIndex &oHighlightedRowIdx);
//    void resetDisplaycontext();
//    void setDisplaycontextToCompatibilityCheck();
//    bool hightlightReplace(const int& iSelectedRow,const QRegExp& searchString,const QString& sReplace,bool ibSearchUp,QModelIndex &oHighlightedRowIdx);
//    bool replaceAllString(const QRegExp& searchString,const QString& sReplace);
//    void highLightLastChild(const QModelIndex& ipIndex)const;
    void getRowsContainingTitle(QList<int>& oLstOfRows);

    /*
        creates element and sets it in view controller
    */
    bool createAndSetElementFromDataDirectoryDoc( const QString& iFileName,EditorType isType );
    bool getElementModel(QAbstractItemModel*& opModel);
    bool getHeaderModel(QAbstractItemModel*& opModel);

    GTASystemServices* getSystemService(){return _pSystemServices;}

    void getDocumentsUsed(const QString& iDocName,QStringList & iolstDocs,QStringList& ioLstOfErrors);
    bool getModelForTreeView( QAbstractItemModel*& pModel ,const QString& iElementName);
    bool getModelForSelectiveExecution( QAbstractItemModel*& pModel ,const QString& iElementName, bool isUUID,QStringList &oVarList);
    /**
    * this function provides a list of VMAC variables which are pending for release.
    */
    QStringList getVariablesForRelease();

    QStringList getFavoriteList(const QString& iFilterStr);
    bool initializeFavList();
    void addToFavorites(const QString& iName,const QModelIndex &iIndex);

    QAbstractItemModel* getFavoriteBrowserDataModel(bool isFavItemRemoved = false)const ;
    QAbstractItemModel* getElemDataBrowserModel()const;
    QAbstractItemModel* getElemDataBrowserModel(bool val)const;
    //    void setFavFlagForTreeModel(bool iVal);
    void setFiltersOnFavorites(const QString& filter);
    void setFiltersOnElemBrowser(const QStringList& filter, const int &iSearchType);
    bool removeFromFavorites(const QModelIndex &iIndex,const QString &iFileName);
    void storeFavoritesInXml();
    void writeTreeNode(QDomDocument *iDomDoc,QDomElement &oElement,const GTATreeItem *pItem);
    QModelIndex getElementDataBrowserIndexForDataDir();
    bool createElementDataBrowserModel(const QStringList& iFilter);


    //void executeSVNAction(const QString iSvnCommand, const QModelIndex &icmdIndex, const QStringList iSvnUsrCredential,QString &oErrorMsg , QString resolveOption = QString(), const QString& fileName = QString()); //Swarup
    //QString executeSVNAction(const QString iSvnCommand, const QModelIndex &icmdIndex, const QStringList iSvnUsrCredential, QString resolveOption = QString()); //Swarup
    QString executeSVNAction(SVNInputParam inputParam);
    bool outputSVNStatus(const QString& iPathToCheck, QString& oErrorMsg, QString& output);
    void checkFileSvnStatus(QStringList& values, QHash<QString, QString>& oItems, bool getUnversioned, bool getOnlyUnversioned);
    bool getAllModifiedUnversionFiles(const QString & iPathToCheck,QHash<QString,QString> &oItems,bool getUnversioned, QStringList usrCredentials,QString &oErrorMsg, bool getOnlyUnversioned = false);

    bool parseOneClickLaunchAppConfig(const QString &iConfigName, GTAOneClickLaunchApplicationTestRig &oTestRig);
    void setLTRACollapsedState(bool isCollapsed,QAbstractItemModel *ipLogModel,QModelIndex iIndex);
    void setLTRACollapsedState(bool isCollapsed,QAbstractItemModel *ipLogModel);
    void checkUncheckAllCommands(bool isSelected,QAbstractItemModel *ipLogModel);
    void SaveAnalyzedResult(QAbstractItemModel *ipLogModel);
//    bool getModelForCopiedItem( QAbstractItemModel*& pModel );
    //provides existing print table names
    QStringList getPrintTables();
    //provides values of print table
    QStringList getPrintTableParameters(const QString& isTableName);
    void editPrintTables(const QString& iName, const QStringList& iLst );


    bool parseKinematicReferenceFile(const QString& iRefFilePath, QStringList &oRefFileData);

    bool loadTemplateFile(const QString &iTmplFileName,const GTAActionFTAKinematicMultiOutput *pKinematciCmd,GTAElement *& opElement);
    bool getElementFromTemplate(const QString & iFileName, GTAElement *& opElement);
    bool generateProcFromTemplate(const GTAActionFTAKinematicMultiOutput *pKinematciCmd, GTAElement *& opElement);
    QString getLogDirectory()const; // returns path of log directory
    QString getLastEmoLogPath()const;//{return _LastEmologFilePath;}
    QString getLastEmoElementPath()const;//{return _LastEmoTestElemPath;}

    static QString getExtensionForElementType(GTAElement::ElemType elemType);
    static GTAElement::ElemType getElementTypeFromExtension(QString &iExtension);

    void updateCurrentSessionVarList();
    QStringList getVarListForAutoComplete();
    QStringList getVariablesForAutoComplete();

    void getListOfUnReleasedParams(GTAElement* ipElem,QStringList& olstOfParamToRelease);

    QStringList getFailureCommandNames();
    GTACommandBase* getFailureCommand(const QString& iFailureName);
    QStringList getPrintTableItem(const QString& iPrintTableName);
    void setHiddenRows( const QStringList& iRows);
    QStringList getHiddenRows();
    bool getTotalTimeOutForDocumentInMs(const QString& iDocName,double& oTime);

    //QList<GTACommandConfig> getCommandConfiguration();
    QString getCommandProperty(const QString & iCmdName, const QString & iProperty);
    //generates cmd property file if the file not generated
    bool generateCmdPropertyFileIfAbsent();
    
    QString getRoboArmSequenceFilePath(const QString & iFileName);
    QString getIRTSequenceFilePath(const QString &iFileName);
    QString getOneClickConfigFilePath(const QString & iFileName);
    QString getOneClickLaunchAppConfigPath();
    void updateReportStatus(GTAElement *pElem);
    QHash<QString,QString> getAllExternalToolNames();
    QString getToolConfigurationPath();

    QList<GTAGenericDB> getUserDefinedDb();
    QList<GTAGenericFunction> getAllExternalToolFunctionNames(const QString &iToolName);
    QList<GTAGenericParamData> getGenericToolDefinitions(const QString &iToolName);
    QList<GTAGenericFunction> getInputFileFunctions(const QString &iInputFilePath);
    void getCommandList(QList<CommandInternalsReturnInfo> &oRetInfo,const QString &iCmdName = QString());
    bool getActionOnFailInfoForGenericTool();
    bool getTimeOutInfoForGenericTool();
    bool getDumpListInfoForGenericTool();
    bool getExternalCommentInfoForGenericTool();


    GTATreeItem* getRootTreeNode()const;
    /**
    * @brief Updates the svn status of tree view elements which are nested in folders.
    * @param directory is the path of nested directory.
    * @param parent is the tree item pointer.
    * @param svnChangedFiles contains list of updated files.
    * @return bool true if then storage went fine.
    */
    bool updateSvnStatusNestedSetOfTreeItems(const QString& directory, const GTATreeItem* parent, const QSet<QString>& changedFiles);
    /**
    * @brief Updates the svn status of tree view elements.
    * @param svnChangedFiles contains list of updated files.
    * @return bool true if then storage went fine.
    */
    bool updateSvnStatusSetOfTreeItems(const QSet<QString> &svnChangedFiles);
    bool updateTreeStructure(bool rebuild, bool isInit = false);
    void checkDataTreeMapConsistency();
    void checkTreeNodeConsistency(const QString &iDirPathChange);
    bool insertIntoTreeStructure(const QStringList &iListItems);
    bool updateSvnStatusOfTheFile(QString& absoluteFilePath, GTATreeItem* child);
    void sortDataTreeMap();
    bool storeTreeStructure(const QString &iDataDirPath, GTATreeItem*& pParent);
    bool updateTreeItem(const QString &iFile, TreeViewActionType iActionType, const QString &iNewName = QString(""));
    PluginOutput importExternalFile(const QString &iImportType, const QString &iExportPath, const QString &iImportPath, const QString &iErrorPath, const bool &iOverwriteFiles);

	GTATreeItem treeItemFromFileInfo(QFileInfo file);

	bool updateTreeStructureDB(GTATreeItem *& pParent, QString iDataDirectoryPath);

	bool initializeDatabase();
    void updateDatabase();
    bool commitDatabase();
    bool updateHistoryTable(const QStringList &iUpdateList);
    GTATreeItem * getTreeNodeFromIndex(const QModelIndex &iIndex);
    QString getFilePathFromTreeModel(const QModelIndex &iIndex,bool iFileUUIDNeeded=false);
    QStringList getFileWatcherList()const;
    void readFavoritesFromDb();
    //    QHash<QString,QString> getFSIINameMap()const;
    bool renameFile(const QModelIndex &indexToLoad, const QString &iNewName);

    bool pasteEditorCopiedRow(const int& iRow, const QList<GTACommandBase*> CmdsPasted);

    bool updateTableVersionInParamDB();

    void setEquipments(const QList<GTAEquipmentMap> &iEquipList);
    QList<GTAEquipmentMap> getEquipments()const;
    GTASqlTableModel * getEquipmentModel()const;
    void processEquipmentData();

    QList<GTASCXMLToolId> readToolIdFromXml(GTASCXMLToolId &oDefaultToolVal);
    void getSCXMLToolID(QList<GTASCXMLToolId> &oToolIDList,GTASCXMLToolId  &oDefaultToolVal)const;
    void onToolClosure();
public:
    void onNewToolIdsAdded(const QList<GTASCXMLToolId> &iList);
    void onRemoveToolID(const QList<GTASCXMLToolId> &iUpdatedList, const GTASCXMLToolId &iRemoveObj); 
    const QString getDatabaseVersion();
    bool parseGenericDBFile(QList<GTAGenericDB> &oUserDefDb);
    void getInternalVariablesForCmd(const QString &iCmdName);
    bool parseInternalParamInfoXml();
    bool createTemporaryEquipmentSettings();
    void getTemporaryEquipmentSettigns();
    void resolveSameUUID(const QFileInfoList fileLst);
    QString portFiles(const QString &iDirPath, bool isRecursive = true);
    QFileInfoList getFileListFromDir(const QString &iRootDirPath,bool isRecursive = true);
    QStringList portElement(GTAElement *&pElement, const QString & iRootDirPath, const QString &iElementAbsPath);

    bool loadPlugin();

    /**
      * This function unloads the Plugin (GTAExtPlugin.dll) Unloading the plugin provides freeing of library which is not needed.
      */
	bool unloadPlugin();
    QStringList getParserTypes();
    void deleteSession(const QStringList &iSelectedItems);
    void addNewSession(const QString &iSession,const QStringList &iFileLst);
    void saveSession(const QString &iSession,const QStringList &iFileLst);
    QStringList getSessionNames();
    QStringList getSessionFiles(const QString &iSessionName);
    bool readSessionInfoFile(QString &oLastSessionName, QStringList &oRecentSessionLst);
    void writeSessionInfoFile(const QStringList &iRecentSessionLst);

    bool isValidLogPresent(const QString &iLogName, QString &oLogFilePath);

    /**
      * Processed all business login to Log Controller
      * This function reads the sequence file with multiple logs
      * @iLogFile: absolute path of the logFile
      * @return: true/false based on log parsing pass/failed
      */
    bool readSequenceMultipleLogFile(const QString &iLogFile, QStringList &oLogNames,
                                     QStringList &oProcNames, QStringList &oStatus,
                                     QString &oGlobalStatus,LogModelSequenceIO &oLogModel);
    bool isSequenceWithMultipleLogs(const QString &iLogFile);
    bool CloseSequencer();
    bool isSequencerOpen();
    bool generateCommandPropertyFile();
//    QStringList getAllDocsUsedInElement(const QString &iUUID);

    bool exportElementToXml(GTAElement *pElement,const QString &iPrintDocPath,const QString &iFileName);
    bool convertToDocx(GTAElement *pElement,const QString &iPrintDocPath,const QString &iFileName);
    GTAPluginInterface* getPluginInterface();

    void createXmlElement(GTACommandBase* pCmd, QDomElement &oCommandsElem,QDomDocument &oDomDoc);
	
	/**
      * This function sets the imported files count received from plugin
      * @param filesCount: imported files count
      */
    void setImportedFilesCount(int filesCount);

	void moveFiles(QList<GTATreeItem *> selectedItems, QString dir);

	QString getExtensionFromTreeModel(const QModelIndex &iIndex);

    void parserBenchVersionsFile(QString& full_infos);
    bool launchScriptBenchVersionsFile() const;

    bool getUnsubscribeFileParamOnly() const { return _unsubscibeFileParamOnly; }
    void setUnsubscribeFileParamOnly(const bool unsubscribeFileParamOnly);
signals:
    void showProgressBar(bool);
    void sigCallActionDropped(int& iRow, QString& iFileName);
    void toggleLaunchButton(bool toggleStatus);
    void closeSequencer(bool &oValue);
    void pluginFilesImported();
	void onImportAlreadyExists(QHash <QString,QString>);
	void onDuplicateUUID(QString oldPath, QString newPath);
	void onUpdateTreeItem();

public slots:
    void saveImportElement(GTAElement *pElem, const QString &iDataDir, bool iOverwriteFiles);
    void onSaveEditorDocFinished();
    bool pasteEditorCopiedRow(const int& iRow);

    GTACommandBase* getCommandForSelectedRow(const int& iSelRow)const;
    void onSequencerClosed();
    void onLaunchProcessExited(int exitCode, QProcess::ExitStatus exitStatus);
//    void onSequencingCompleted(const QDateTime &iTimeStopped, const QMap<QString,QString> &iLogStatusLst);


private:
    bool exportLTRAToXml1(GTAElement * pElem,const QString & iReportFile,const QStringList & iAttachmentList, bool isLTR = false);
    /**
      * This function export the LTRA-Lab Test Report Analysis to csv, delimiter is ':::'
      * @pElem: Element to be exported
      * @iReportFile: Report file full path where the report is created
      * @iAttachmentList: list of documents to be attached with report
      */
    bool exportLTRAToCsv(GTAElement * pElem,const QString & iReportFile,const QStringList & iAttachmentList);


    //bool executeSVNCommand(const QStringList &iCommandArgs, QString &oCmdOpuput = QString(),QString &oErrorCode=QString());
    
    // Commits session data including favorites and last emo test
    void  commitSessionData();

    bool clearTempDirectory();


    QList<GTACommandBase*> getParentList(GTAElement* &ipElem,int idx=0);
    void getCommandListFromElem(const GTAElement *ipElem, QHash<QString, GTACommandBase *> &oCmdList, const QString &iElemName, const QString &iCmdName,bool isCallCommand);
    void getFuncStructList(QString &iRetName, QList<CommandInternalsReturnInfo> &oRetInfo,const QString &iLocation, const QString &iFuncName,const QString &iToolName, const QString &iRetStr, GTAGenericToolCommand *pGenCmd);
    
    bool updateGroupingForMonitor(const GTAElement *pElem, QStringList &oVariableList, GTAElement *oGroupedElem, QStringList &icheckforRep,bool iIsParentIgnored = false);
    /**
      * This function creates a parent title under which othe command are put as children
      * @ipCmd: Input command whose children need to be grouped
      * @ipTitleCmd: Title command whose children need to be grouped
      * @oVariableList : All variables inside ipCommand and its children.
      * @icheckforRep: flag to check, there is no child calling its parent
      * @iIsParentIgnored: Grouping of children is ignored/done when flag is true/false
      * @iIsValidationGroupingNeeded: Only call commands are grouped if validation mode is enabled/disabled when the flag is true/false
      */
    void createCommandGroupedByTitle(GTACommandBase*& ipCmd, GTAActionParentTitle *&ipTitleCmd,QStringList &oVariableList, QStringList &icheckforRep, bool iIsParentIgnored = false,bool iIsValidationGroupingNeeded = false);
    /**
      * This function provides a custom element grouping by managing calls under titles
      * @pElem: Input Element which is used to create a grouped element
      * @oVariableList : All variables inside ipCommand and its children.
      * @oGroupedElem: grouped element created from input
      * @icheckforRep: flag to check, there is no child calling its parent
      * @iIsParentIgnored: Grouping of children is ignored/done when flag is true/false
      * @iIsValidationGroupingNeeded: Only call commands are grouped if validation mode is enabled/disabled when the flag is true/false
      */
    void GroupElementByTitleComplete(const GTAElement *pElem, QStringList &oVariableList, GTAElement *oGroupedElem, QStringList &icheckforRep, bool iIsParentIgnored = false, bool iIsValidationGroupingNeeded = false);
    /**
      * This function provides a custom element grouping by managing Calls under titles for multiple logs
      * @ipElem: Input Element which is used to create a grouped element
      * @oGroupElem: grouped element created from input
      * @iIsValidationGroupingNeeded: Only call commands are grouped if validation mode is enabled/disabled when the flag is true/false
      */
    void groupElementbyTitleForSequence(GTAElement *ipElem,GTAElement* oGroupElem, bool iIsValidationGroupingNeeded = false);
	void resolveInternalParamComplexTypes(const QStringList &iComplexTypes, const QString &iParamType, const QString &iParamName, QList<InternalParameterInfo> &oInternalParamLst);
public:
	/**
	Calls functions in DbDataController to update the DATA database.
	*/
	bool updateWorkDB();

	/**
	Calls functions in DbDataController to import new files to the DATA database, and copy/paste the new files in the DATA folder.
	*/
	void importFilesToWorkDB(QStringList pathList, QString parent = QString());

	/**
	Calls functions in DbDataController to import a new folder (and its content) to the DATA database, and copy/paste the new files in the DATA folder.
	*/
	void importFolderToWorkDB(QString source, QString destination = QString());

	/**
	Calls functions in DbDataController to delete and rebuild the DATA database.
	*/
	bool rebuildDB();

	void deleteAndImport(QHash <QString, QString> duplicatesToReplace);

	void deleteFile(QString path);

private:

    QProcess *pLaunchSCXMLProcess;
    QMutex mutex;
    //! constructor.
    /*!
    singleton design hence private.
    */
    GTAAppController(QObject* ipObject);
    static GTAAppController* _pAppController;
    GTADataController* _DataCtrl;
    GTASystemServices * _pSystemServices;
	GTASequencerMain *_pSequencerMain;
    GTAGenericToolController* _pGenericToolController;

    
    GTAViewController* _pViewController;

    QList<int> _lstOfRowsToCopy;
    QString _LastError;
    QStringList _FavList;//to be moved to view controller

    enum SVN_ERROR_CODES{NOT_WORKING_COPY,PARENT_NOT_CHECKED_IN,COMMIT_FAILED,ADD_FAILED,LOCK_FAILED,UNLOCK_FAILED,DELETE_FAILED};

    QHash<QString,QString> _FSIINameMap;
    GTATreeItem *_treeRootNode;
    QStringList _HistoryVarList;
    QStringList _CurrentSessionVarList;
    QStringList _FileWatcherList;
    QHash<QString, GTATreeItem *> _DataTreeMap;
    QList<GTAEquipmentMap> _EquipmentData;
    QList<GTASCXMLToolId> _ToolIDList;
    GTASCXMLToolId  _DefaultToolVal;
    GTASqlTableModel *_pEquipmentModel;

    QList<CommandInternalParameterInfo> _InternalParamInfoLst;
    QMap <QString,ComplexItemType> _ComplexParameterTypeMap;

    //    GTAEditorViewController * _pActiveViewController;

    QString PasteCallNow;
    QString PasteCallPrev;
    inline void assignPaste() { PasteCallNow = "copy";PasteCallPrev = "copy";}

    //<UUID,absolute file path>
    QHash<QString,QString> _UUIDFileNameHash;

    //<absolute file path,UUID>
    QHash<QString,QString> _callResolutionHash;

    //QFileInfoList : list of files for call resolution, from a folder provided by user
    QFileInfoList _lstFileInfo;

    // for plugin interfaces
    GTAPluginInterface* _pPluginInterface;
	// Array to store all plugins
	QList<QPluginLoader*>   _plugins;

    //sequencer
//    QDateTime _startTime;
//    QDateTime _stopTime;
    QMap<QString,QString> _SequencerLogStatus;

    bool _isSequencerRunning;

    GTASVNController *_pSVNController;
	// _importedFilesCount : Count of files being imported from plugin
    int _importedFilesCount;
	// _saveImportsCount : Count of number of savedocfinished
    int _saveImportsCount;
    QHash<QString,QDateTime> _databaseFiles;
	GTADbDataController* _dbDataController;
	QHash<QString,QString> _duplicateFiles;
    bool _displayConsole = false;

    bool _unsubscibeFileParamOnly;

public slots:
	void onDuplicateUUIDFound(QString oldPath, QString newPath);
    
};

#pragma warning (default : 4239)
#endif
