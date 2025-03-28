#ifndef GTACOMMANDBASE_H
#define GTACOMMANDBASE_H
#include "GTACommands.h"
#include "GTAScxmlLogMessage.h"
#include "GTACommandVisualItf.h"

#pragma warning(push, 0)
#include <Qlist>
#include <QHash>
#include <QColor>
#include <QFont>
#include <QMap>
#pragma warning(pop)

//TODO: Using compile-time built structures inside .dll linked libraries is generally a bad idea, 
//      however in this project .dll libraries are not used in intented way (they are basically static linked) - so it doesn't cause problems
//      in future would propably be better to use static link or rewrite these
#pragma warning (disable : 4251)
class GTACommands_SHARED_EXPORT GTACommandBase: public GTACommandVisualItf
{
public:
    enum CommandType{ACTION, CHECK, COMMENT, EQUATION,FAILPROFILE};
    enum reportSelectionType{INCLUDE,EXCLUDE,PARTIALLY_INCLUDE};
    enum LOOP_TYPE{RECURSIVE,NON_RECURSIVE};
    

    GTACommandBase(CommandType iType);
    GTACommandBase(const GTACommandBase& rhs);
    virtual ~GTACommandBase();

    CommandType getCommandType() const;
    virtual QString getCommandTypeForDisplay() const =0; 
    virtual QString getStatement() const = 0;

    virtual void setParent(GTACommandBase* iParent)=0;
    virtual GTACommandBase* getParent(void) const=0;
    

    virtual QList<GTACommandBase*>& getChildren(void) const=0;


    virtual bool insertChildren(GTACommandBase* pBase,const int& idPos)=0;

    virtual int getAllChildrenCount(void);// {return 0;}
    virtual int getAbsoluteChildrenCount(void);

    void getNumberOfOK(int& NumberOfOK, GTACommandBase* pCmd);
    void getNumberOfKO(int& NumberOfKO, GTACommandBase* pCmd);
    void getNumberOfNA(int& NumberOfNA, GTACommandBase* pCmd);
    void getNumberOfKOWithDefect(int& NumberOfKOWithDefect, GTACommandBase* pCmd);

    void getListOfDefects(QString& ListOfDefects, GTACommandBase* pCmd);

    virtual GTACommandBase* getClone() const =0; 

    virtual QStringList getHeaderColumns(){return QStringList();}
    virtual bool setColumnMap(){return false;}

    void setLoopSCXMLId(const QString &iStateId){_LoopId = iStateId;}
    QString getLoopSCXMLStateId(){return _LoopId;}

    /**
      * This function return true if the command can be copied and pasted independently
      * othewise return false, in such case command can be copied and pasted by if parent command only
      * e.g. else, if end , while end can't be copied and pasted without its parent
      */
    virtual bool canBeCopied();

    /**
      * This function return true if command can be deleted independently
      * otherwise false, in such case command can be deleted by its parenet only
      * e.g. e.g. else, if end , while end , invalid command can't be deleted independently
      */
    virtual bool IsDeletable();

    /**
    * This function return true if command can be updated by the user independently
    * otherwise false, example end statements for while and if and invalid command .
    */
    virtual bool IsUserEditable();

    /**
    * This function return true if command can is an end command of IF, WHILE, DO WHILE 
    * otherwise false, example end statements for while and if and invalid command .
    */
    virtual bool IsEndCommand() const;

    /**
    * This function return true if command has reference to an object/function example call.
    * otherwise false
    */
    virtual bool hasReference();
    /**
    * This function return true if is mutually exclusive statement example else command., 
    * If it has an 'else' statement under it, it will return true otherwise false
    */
    virtual bool isMutuallExclusiveStatement()const;
    /**
    * This function return true if command has a counter condition under it otherwise returns false  
    * applicable for 'If' statement , 
    * If it has an 'else' statement under it, it will return true otherwise false
    */
    virtual bool hasMutuallExclusiveStatement(int* opPos=NULL)const;
    /**
    * This function return true if command has children under it currently.
    * otherwise false, if, commands 'while', 'if' and 'else' has children it will return true
    * in all other cases it will return false(including other commands).
    */
    virtual bool hasChildren()const;

    /**
    * This function return true if command has breakpoint.
    * otherwise false
    */
    virtual bool hasBreakpoint()const;

    /**
    * This function sets the breakpoint for a command
    */
    virtual void setBreakpoint(bool val);


    bool hasExecutionControl()const;
    void setExecutionControl(const bool val);


    /************************************************************************/
    /* This function creates End command if applicable 
    @argument1: opEndCmd the created End command, if not exist = NULL       
    @return : true if there is an end command, false if it does not have
    /************************************************************************/



    virtual bool createEndCommand(GTACommandBase* & opEndCmd)const;

    /**
    * This funtion is implemented by each command.
    * returns true if it can have children
    * returns false if it cannot have children under it.
    */
    virtual bool canHaveChildren()const=0;

    /**
    * This funtion is true for IF command only because it can have else statement.
    */
    virtual bool canHaveMutuallExclusiveStatement()const;

    /**
      true for those commands that creates command from template and aggregrates created
      command as its own child.
    */
    virtual bool createsCommadsFromTemplate()const;

    /**
      * This function will be implemented by each command and will provide list of variables stored in each command
    */
    virtual  QStringList getVariableList() const=0;
    /**
      * This function replaces each contents of a commands if it has a tag identified by iTagValueMap.
      * @argumnet1: map containing list of variable tag and corresponding value.
      */
    virtual void ReplaceTag(const QMap<QString,QString> & iTagValueMap) =0;

    //Visual Property
    /*virtual QColor getForegroundColor() const = 0;
    virtual QColor getBackgroundColor() const = 0;
    virtual QFont getFont() const = 0;*/

    //command Comment
    virtual void setComment(const QString & iComment);
    virtual QString getComment() const;

    virtual void setRequirements(const QStringList &iList);
    virtual QStringList getRequirements()const;

    /**
      * This function set instance name of the command
      * instance name represent the context where the command is being used and its position in list
      * this way instance name is always unique a provided context
      * iInstanceName: instance name
      */
    virtual void setInstanceName(const QString iInstanceName) ;

    /**
      * This function return the instance name of command
      */
    virtual QString getInstanceName() const;

    virtual void setLogMessageList(const QList<GTAScxmlLogMessage> & iList);
    inline QList<GTAScxmlLogMessage> getLogMessageList() const {return _LogMessageList;}

    inline void setUserLogComment(const QString iComment){_LogComment = iComment ;}
    inline QString getUserLogComment()const{return _LogComment;}

    virtual bool isTitle(){return false;}
    virtual bool isOneClickTitle(){return false;}
    virtual bool canHaveInstanceName() {return true;}

    /**
      * this function return if the command is valid
      * This is evaluated during compatibility check
      */
    virtual bool isCmdValid() const;
    /**
      * this function set the command validity status
      * This is evaluated during compatibility check
      */
    virtual void setCmdValidaityStatus( const bool iStatus);
    /**
    * this function returns true if a command can have log
    */
    virtual bool logCanExist() const;
    /**
      * this function returns the error msg list of the command
      * This is evaluated during compatibility check
      */
    virtual QStringList getErrorLogList() const;
    /**
      * this function sets the error msg list of the command
      * This is evaluated during compatibility check
      */
    virtual void setErrorLogList(const QString &iMsg);

    virtual bool isFailureCommand(){return false;}

    /************************************************************************/
    /* String expression replace functionalities                            */
    /************************************************************************/
    virtual bool stringReplaceApplicable()const;
    virtual void stringReplace(const QRegExp& iStrToFind,const QString& iStringToReplace);
    virtual bool equipmentReplace(const QRegExp& iStrToFind, const QString& iStringToReplace);
    virtual bool searchString(const QRegExp& iRegExp,bool ibTextSearch)const;
    virtual bool replacableStrFound(const QRegExp& iRegExp)const;
    bool searchCommentAndRequirements(const QRegExp& iRegExp) const;
    void setReportStatusOfChildrens(reportSelectionType reportStatus);

    /* void setSearchHighlight(bool iHighLight);
    bool getSearchHighlight()const;*/

    virtual bool expandReferences();
    virtual bool setDumpList(const QStringList& iDumpList);
    virtual QStringList getDumpList()const;

    virtual QString getSCXMLStateName()const;
    virtual bool getSCXMLConditionStatement(const QStringList &iIcdParamList, QString &oCondtionStatement, const QHash<QString, QString> &iGenToolReturnMap, const QString &iEngine = QString(), bool isGenericSCXML = false, const QStringList iLocalVarList =QStringList()) const;

    void updateUserComments(const QString &iUserComments);
    QString getUserComment()const {return _UserComments;}


    virtual void setCollapsed(bool iCollapsed);
    virtual bool getCollapsed() const;
    /**
      * this is true for commands that can have loop
      * commands like wait until, while and check can have loops,
      * which effectivily had property of repetetive logging from scxml.
      * 
      */
    virtual bool hasLoop() const;
    /*
    This function provides the master command for any child at any level command.
    will return self it this command has no parent.

    */
    GTACommandBase* getMasterCmd();
    //INDENTATION
    GTACommandBase* getMasterCmd(int &oCount);               // same as above function added separately to avoid regression
    //INDENTATION

    virtual void setIgnoreLogInFail(bool ibLog);
    bool getIgnoreLogInFail()const;
    QString dumpValuesFromLog()const;

    virtual bool isOneClick(){return false;}

    virtual void insertTagValuepairForTemplates(const QString &iTag,QString &iVal);


    virtual void setRefrenceFileUUID(const QString &iRefFileUUID);
    virtual QString getRefrenceFileUUID() const;


    virtual  QMap<QString, QString > getTagValuePairForTemapltes() const;
    virtual void replaceUntagged();

    inline void setSCXMLStateId(const QString &iStateId)
    {
        _SCXMLStateId = iStateId;
    }
    inline QString getSCXMLStateId() const
    {
        return _SCXMLStateId;
    }

    //QString getInvokeNodeDelayInMs()const;
    inline void setReportStatus(reportSelectionType iIgnore){_reportStatus=iIgnore;}
    inline reportSelectionType getReportStatus()const{return _reportStatus;}
    virtual void updateReportStatus();
    virtual void setSelectedChannel(const QString& iSelChannel){_selectedChannel = iSelChannel;}
    virtual QString getSelectedChannel(){return _selectedChannel;}
    bool hasReferenceTimeout()const{return _bHasReferenceTimeOut;}
    QStringList getReferenceTimeoutStates()const{return _referenceTimeoutStates;}
    void setReferenceTimeOutStates(const QStringList& iRefTimeouts);

    virtual bool hasChannelInControl()const =0;//{return false;}//make it mandatory.
    void setIgnoreInSCXML(bool iIgnore,GTACommandBase::LOOP_TYPE type = RECURSIVE);
    bool isIgnoredInSCXML()const{return _ignoreForSCXML;}

    bool areAllChilrenIgnored();

    void setReadOnlyState(bool iReadOnly);
    bool getReadOnlyState()const{return _ReadOnly;}
    void removeEmptyChilds();
    void setInstancePath(const QString& sInstancePath);
    QString getInstancePath()const;

    QString getExecutionResult() const;
    virtual double getExecutionEpochTime() const;
    virtual QString getExecutionTime() const;
    virtual QHash<QString, QString> getDetailedLogInfo(QMap<int, QString> &odetailInfoMap);
    virtual QString getLTRAStatement()  const;
    virtual QString getPrintStatement() const;
    void setCommandProperty(const QString& iProperty, const QString& iValue);
    void clearChildrenList();
    virtual QString getGlobalResultStatus();
    //    virtual QString getUserComment();

    void setExecutionTime(const QString &iTime){_Time = iTime;}
    void setExecutionResult(const QString &iResult){_Result = iResult;}
    void setReferences(const QString &iRef){_References= iRef;}
    QString getReferences(){return _References;}

    virtual bool isCompoundState(){return false;}
    virtual QString getCompounsStateId() {return QString();}

	/**
      * this function resolves internal parameters to correct state names
      * This is evaluated during SCXML generation
	  * @iVal provides name of variable with UUID
	  * @iVarList provides statename of the variable
      */
	QString resolveInternalParams(QString iVal, QStringList iVarList) const;

protected:
    GTACommandBase* _parent;
    QList<GTACommandBase*>* _lstChildren;

    QStringList                 _dumpList;
    QList<GTAScxmlLogMessage> _LogMessageList;
    QStringList                 _referenceTimeoutStates;
    QString                     _SDI;
    bool                        _IsOnlySDI;
    QString                     _Parity;
    bool                        _IsOnlyParity;
    bool                        _IsOnlyRefreshRate;

protected :
    static bool isVariable(const QString&);
    void setCommandType(CommandType iType);

    static bool hasChannelInCtrl(const QString& iPram);

public:
    bool getIsSDIOnly() const;
    void setIsSDIOnly(const bool iIsSetOnlySDI = false);

    bool getIsParityOnly() const;
    void setIsParityOnly(const bool iIsSetOnlyParity = false);

    bool getIsRefreshRateOnly() const;
    void setIsRefreshRateOnly(const bool iIsSetOnlyRefreshRate = false);

    void setSDI(const QString& iSDI);
    QString getSDI()const;

    void setParity(const QString& iParity);
    QString getParity()const;

    QString getObjId()const;
    void setObjId(QString val="dummy",bool createNewID=true);
    //function to hide uuid in the editor window
    //to be called with getstatement()
    virtual void getTrimmedStatement(QString &val)const;

private:
    QString _objID;
    QStringList _ErrorLogList;
    CommandType _CmdType;
    QString     _Comment;
    QStringList _Requirements;
    QString _InstanceName;
    QString _InstancePath;
    bool _CmdValidityStatus;
    bool _bHasReferenceTimeOut;
    bool _ignoreForSCXML;
    bool _IsCollapsed;
    bool _IgnoreLogInFail;
    bool _ReadOnly;
    QString _LoopId;
    QString _Result;
    QString _Time;
    double _EpochTime;
    QString _References;
    //bool _searchHighlight;
    QString _UserComments;
    //  QString _UserCommentInResult;
    void getAllChildrenCount(int & oCount,GTACommandBase* pBase =NULL);
    void getAbsoluteChildrenCount(int & oCount,GTACommandBase* pBase =NULL);

    void deleteAllChildren(QList<GTACommandBase*> ilstChildrens);

    QString _LogComment;

    QString _SCXMLStateId;
    QString _selectedChannel;
    reportSelectionType _reportStatus;

    QString _RefFileUUID; // required for call command files.
    QHash<QString,QString> _commandProperty;
    bool _breakpoint;
    bool _currentExecutionControl;
};

#endif // GTACOMMANDBASE_H

