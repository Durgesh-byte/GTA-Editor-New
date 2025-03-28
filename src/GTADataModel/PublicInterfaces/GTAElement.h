#ifndef GTAELEMENT_H
#define GTAELEMENT_H
#include "GTADataModel.h"
#include "GTACommandBase.h"
#include "GTAActionForEach.h"

#pragma warning(push, 0)
#include <QString>
#include <QHash>
#include <QUuid>
#include <QMap>
#pragma warning(pop)

#include <optional>

class GTAActionBase;
class GTACheckBase;
class GTAHeader;
class GTAActionElse;

struct TagVariablesDesc
{
    TagVariablesDesc():name(QString()),type(QString()),unit(QString()),supportedValue(QString()),max(QString()),min(QString()),Desc(QString()){}
    QString name;
    QString type;
    QString unit;
    QString supportedValue;
    QString max;
    QString min;
    QString Desc;
};

class GTADataModel_SHARED_EXPORT GTAElement
{
public:
    enum SaveStatus{SaveOK,SaveWithError, Modified};
    enum ElemType{OBJECT, FUNCTION, PROCEDURE, SEQUENCE,TEMPLATE,CUSTOM, UNKNOWN};
    enum ActionType{ACTION, CHECK, COMMENT};

    explicit GTAElement(ElemType iType );
    explicit GTAElement(ElemType iType, const int& noOfDefaultChildrens );
    explicit GTAElement(ElemType iType,QList<GTACommandBase*>& ipCmdlst);
    explicit GTAElement(const GTAElement & ipElem);
    void appendCommand(GTACommandBase * pCmd);
    ~GTAElement();

    //Changes element
    void setElementType(ElemType iType);
    ElemType getElementType() const;

    //Element Name
    void setName(const QString & iVal);
    QString getName() const;

    //#322480 Adding author column in Data browser
    void setAuthorName(const QString & iAuthName);
    QString getAuthorName()const;

    void setValidatorName(const QString &iValidatorName);
    QString getValidatorName()const;

    void setValidationStatus(const QString &iValidationStat);
    QString getValidationStatus()const;

    void setValidationTime(const QString &iValidationTime);
    QString getValidationTime()const;

    void setCreationTime(const QString &iCreatedTime);
    QString getCreationTime()const;

    void setModifiedTime(const QString &iModifiedTime);
    QString getModifiedTime()const;

    void setMaxTimeEstimated(const QString& iMaxTimeEstimated);
    QString getMaxTimeEstimated()const;

    void setInputCsvFilePath(const QString& iInputCsvFilePath);
    QString getInputCsvFilePath()const;

    void setOutputCsvFilePath(const QString& iOutputCsvFilePath);
    QString getOutputCsvFilePath()const;

    void setPurposeForProcedure(const QString& iSummary);
    QString getPurposeForProcedure() const;

    void setConclusionForProcedure(const QString& iConclusion);
    QString getConclusionForProcedure() const;

    //Header linking
    void setHeader(GTAHeader *& ipHeader);
    GTAHeader * getHeader();

    //bool insertEmptyCommand(int iIndex);

    /**
      * This command remove the command on given index
      * Case-1 : command does not have children , then only command gets deleted
      * Case-2 : command have childrens, then shift all the commands 
   */
    bool removeCommand(const int& iIndex);

    /**
      * This function insert the command at given index
      */
    bool updateCommand(const int & iIndex, GTACommandBase* ipCommand);

    bool getCommand(const int& iIndex, GTACommandBase*& oPCommand) const;

    /**
     * @brief Searches for parent of command at given index and returns it if it exists
    */
    std::optional<GTACommandBase*> getParentForIndex(int idx);

    /**
     * @brief Overload of getParentForIndex which uses command instead of index as input
    */
    std::optional<GTACommandBase*> getParentOfCommand(const GTACommandBase* cmd);

    /**
     * @brief Searches for index of given command and returns if it exists
    */
    std::optional<int> getIndexOfCommand(const GTACommandBase* cmd);

    /**
     * @brief Returns list of all commands (all children included) in loaded element listed in index order
    */
    QList<GTACommandBase*> GetAllCommands();

    bool hasIndex(const int& iIndex);

    /**
      * This function return the children count of herarchical commands
      */
    int getAllChildrenCount(void) const;
    /**
      * This function return the count of direct command in main list, it does not take command's chilren into account
      */
    int getDirectChildrenCount() const;

    bool getDirectChildren(const int& iIndex, GTACommandBase*& oPBase) const;


    bool ClearAll();

    bool prependRow();

    bool insertCommand(GTACommandBase * pCmd , int  iGlobalPosition,bool IsEditMode );

    /**
      * This function return true, if a valid command or comment found in the element
      * otherwise return false
      */
    bool isEmpty();

    void initialize(int nbOfChildren);

    /**
      * This function provides consolidated list of parameter/variables of a command
      * containing tag identifiers (ie in format @XYZ@).
      */
    QList<TagVariablesDesc> getTagList()const;
    /**
      * This function replaces tags of each command with their corresponding values
      */

    void replaceTag(const QMap <QString,QString>& iTagValMap);


    void replaceUntagged();


    /**
      * This function return the string representation of unique id
      * the id is used to identify the document file associated to this element
      */
    inline QString getUuid() const
    {
        return _Uuid;
    }

    /**
      * This function set a uuid read from document
      */
    void setUuid(const QString & iUuid){_Uuid = iUuid;}



    /**
      * Returns the Gta version of the doc
      */
    QString getGtaVersion()const{return _GtaVersion;}

    /**
      * This function sets GTA Version read from document
      */
    void setGtaVersion(const QString & iGtaVersion){_GtaVersion = iGtaVersion;}


    /**
      * Return the save status of the element, whether it is saved or not
      *
      */
    inline SaveStatus getSaveStatus() const{return _SaveStatus;}
    /**
      * update the status of whether this element is saved or not
      */
    inline void setSaveSatus(SaveStatus  iStatus){_SaveStatus = iStatus;}

    /**
      * This function reset the document ID
      */
    inline void resetUuid(){_Uuid = QUuid::createUuid().toString();}

    /**
      * This function populates and returns the variable used in each command
    */

    //QStringList getVariableList()const;

    /**
      * This function navigate the all the commends in this element and assign an instance name
      * instance name is the element id/position of command in list
      * iRootInstance: The instance of root command , root command is always a call command that refer another document
      * if element belongs to call command , then iRootIntance is the instace name of call command, otherwise QString()
      */
    void updateCommandInstanceName(const QString & iRootInstance);

    void groupCommandByTitleLTRA(QHash<QString, QList<GTACommandBase*> >& iHshOfCmd);

    bool getDirectChildrenIndex(GTACommandBase* ipCmd, int& oIndex)const;
    QString getGlobalResultStatus()const;

    int getNumberOfOK();
    int getNumberOfKO();
    int getNumberOfNA();
    int getNumberOfKOWithDefect();
    QString getListOfDefects();

    /**
    *  finds string based on search option in each command and sets
    *  row in foundRows
    **/
    bool findStringAndSetHighlight(const QRegExp& searchExp,QList<int>& iSelectedRows,QHash<int,QString>& oFoundRows,bool bSearchReverse,bool ibTextSearch, bool isSingleSearch = false, bool isHighlight = true );
    void resetStringSearch();
    void replaceString(const QRegExp& searchString,const QString& sReplaceWith,QList<int>& iSelectedRows,QList<int>& oFoundRows,
                       bool bSearchReverse   );
    bool replaceString(const int& row,const QRegExp& strToReplace,const QString& sReplaceWith);
    bool replaceAllString(const QRegExp& strToReplace,const QString& sReplaceWith);
    bool replaceAllEquipment(const QRegExp& strToReplace, const QString& sReplaceWith);
    void flattenCallCommands();
    QString getDocumentName();
    QStringList getPrintTables();
    QStringList getPrintTableParams(const QString &isTableName);
    QString getExecutionduration();
    void editPrintTables(const QString& iName, const QStringList& iLst );
    /************************************************************************/
    /* creates a new element from current element all commands are grouped by title
    each title will have children                                           */
    /************************************************************************/
    void createActionElementGroupedByTitle(GTAElement*& pElement,bool groupComplete = false);
    void createOneClickElementGroupedByTitle(GTAElement*& pElement);
    //GTAElement* getClone();
    //void setMasterList(QList<GTACommandBase*> ipLstOfCmds  );

    //In Genesta the print command is implemented in following manner:
    // Tile of print
    // call print_table<file>
    // comment
    // Title of print is given before the print command itself, hence it goes into title
    void editTitleInprintTableForGenestaImport();
    QStringList getPrintTableItems(const QString& iTableName);

    void getVariableList(QStringList& oParamList, bool isSubscribeParam = false)const ;
    void appendReleaseCommand(const QStringList& iRleaseParam,bool forImportGenesta=false);

    void setHiddenRows( const QStringList& iRows);
    QStringList getHiddenRows()const;

    QStringList getFailureCommands();
    GTACommandBase* getFailureCommand(const QString&);
    bool hasChannelInControl()const;
    QList<GTACommandBase*> getAllFailureCommands();
    void addtimeOut(const double& idTimeOut , const QString& iUnit);
    double getTotalTimeOutInMs() const;
    void removeEmptyItems();
    void setSaveAsReadOnly(bool iReadOnly){_bSaveAsReadOnly =iReadOnly; }
    bool isReadOnly(){return _bSaveAsReadOnly;}

    void setAbsoluteFilePath(const QString &iAbsFilePath);
    QString getAbsoluteFilePath()const;

    void setRelativeFilePath(const QString &iRelFilePath);
    QString getRelativeFilePath()const;

    void setFSIIRefFile(const QString &iFSIIRefFileName);
    QString getFSIIRefFile()const;

    void setExecStartDuration(const QString &iDuration){_ExecStart = iDuration;}
    void setExecEndDuration(const QString &iDuration){_ExecEnd = iDuration;}
    QString getExecStartDuration()const { return _ExecStart;}
    QString getExecEndDuration()const{ return _ExecEnd;}

    // This function saves the _tagVarlist
    void setTagVarStructList(QList<TagVariablesDesc> iTagList);

    void updateCallCommands();

private:

    bool insertCommand(GTACommandBase * pCmd , int  & iGlobalPosition, QList<GTACommandBase* > & iListToUpdate, GTACommandBase *& ipParent);
    void updateCommand(int& iIndex,GTACommandBase * ipCmdToUpdate,QList<GTACommandBase*> &isList,bool& bFound);
    void getCommand(int& iIndex,GTACommandBase*& oPCommand, const QList <GTACommandBase*>& isList,bool& found) const;
    void removeCommand(int& iIndex,QList <GTACommandBase*>& isList, bool& bRemoveStatus);
    void reparentForElse(GTAActionElse *& ipElseCmd, QList<GTACommandBase*> & ipList, int iPosition);
    bool collectVariableNames(const QString& isVar , QStringList& iLstVars)const;
    void resetStringSearch(QList<GTACommandBase*>& iPlstCmds);
    bool searchSetHighlightOnCommand(const QRegExp& searchExp,const int& ipRow,bool ibTextSearch, QString& ofoundCmdStatement, bool isHighlight = true);
    void createCommandGroupedByTitle(GTACommandBase*& pCmd);
    void getVariableList(QStringList& oParamList,GTACommandBase* pCmd)const;
    void hasChannelInControl (bool& hasVariableList,GTACommandBase* pCmd)const;
    
    std::optional<GTACommandBase*> GTAElement::getParentOfCommand(int cmdIndex);
    void appendAllChildren(GTACommandBase* parentCmd, QList<GTACommandBase*>& outputCmds);
    bool searchChildren(int& currentIdx, const GTACommandBase* parentCmd, const GTACommandBase* wantedCmd);
    

    ElemType _ElemType;
    QString _ElementName;
    QString _AuthName; //#322480 Adding author column
    QString _ValidatorName;
    QString _ValidationStatus;
    QString _ValidationTime;
    QString _CreatedDate;
    QString _ModifiedDate;
    QString _MaxTimeEstimated;

    //reference to header.
    GTAHeader * _pHeader;

    QString _GtaVersion;
    QString _Uuid;
    SaveStatus _SaveStatus;
    QStringList _hiddenRows;
    QList   <GTACommandBase*>    _lstMasterCommands;
    QHash<QString, QString>      _lstLocVarValues;
    bool _bSaveAsReadOnly;
    QString _AbsoluteFilePath;
    QString _RelativeFilePath;
    QString _FSIIRefFileName;
    QString _InputCsvFilePath;
    QString _OutputCsvFilePath;
    QString _PurposeForProcedure;
    QString _ConclusionForProcedure;
    QString _ExecStart;
    QString _ExecEnd;
    mutable QList<TagVariablesDesc> _tagVarList;
};

#endif // GTAELEMENT_H
