#ifndef GTACHECKCOMPATIBILITY_H
#define GTACHECKCOMPATIBILITY_H


#include "GTAControllers.h"
#include "GTACommandBase.h"
#include "GTAElement.h"
#include "GTAICDParameter.h"
#include "GTACommandList.h"
#include "GTAAppController.h"

#pragma warning(push, 0)
#include <QStringList>
#include <QHash>
#pragma warning(pop)

class GTAControllers_SHARED_EXPORT GTACheckCompatibility
{
    enum CompatibilityCallFuncErrors{NO_ERROR=0,INCONSITENT_PARAMETER,RECURSION};
    enum SubCmdType{PARAMETER_TYPE=0,EXTERNAL_TOOL, IRT, ONECLICK,INIT_PIR};
    struct  GTAElemRecStruct
    {
        QString _sParent;
        QStringList _sChildren;
    };

public:
    GTACheckCompatibility();
	
	enum NbColumn
	{
		SET_CLASSIQUE,
		OLD_FOREACH,
		NEW_FOREACH,
		WRITE_BACK
	};

    struct ParamInfo
    {
        ParamInfo():
			hasValidSubscribe(true),
			hasValidUnsubscribe(true),
            isInRange(true),
			isTagVar(false),
			isTagVarValid(true),
            isParamSubscribed(true),
			isParamDirectionCorrect(true),
            isEngineParamInDb(true),
			isParamInDb(true),
			isInternalParamValid(true),
			isParamPIR(true){}

        bool isParamSubscribed;
        bool hasValidSubscribe;
        bool hasValidUnsubscribe;
        bool isValidFSType;
        bool isSetCheckFS;
        bool isParamVmac;
        bool isParamInDb;
        bool isEngineParamInDb;
        bool isValCorrect;
        bool isParamLocal;
        bool isParamUsed ;
        bool isParamReleased ;
        bool isValApplicable;
        bool isConstant;
        bool isParamDbType;
        bool isFirst;
        bool isPIR;
        bool containsInvalidChars;
        QStringList invalidChars;
        QStringList invalidSubscribeParam;
        QStringList invalidUnSubscribeParam;
        QStringList invalidEngineParams;
        QStringList emptyCSVCases;
        QString val;
        QString paramName;
        QString valueType;
        QString rangeMinVal;
        QString rangeMaxVal;
        bool isEquation;
        bool isInRange;
        bool isTagVar;
        bool isTagVarValid;
        bool isParamDirectionCorrect;
        bool isInternalParamValid;
        bool isParamPIR;
        QString nextType;

    };

    struct ExternalToolAttributes
    {
        bool isAttributeValid;
    };

    struct ExternalToolArgument
    {
        QString ArgName;
        bool isArgumentValid;
    };

    struct ExternalToolDef
    {
        QList<ExternalToolAttributes> attributeList;
    };

    struct ExternalToolFunc
    {
      QString FunctionName;
      QList<ExternalToolArgument> argList;

    };

    struct ExternalTool
    {
        ExternalTool():isMismatch(false){}
        QString toolName;
        QList<ExternalToolFunc> funcList;
        QList<ExternalToolDef> defList;
        bool isMismatch;
    };

    struct CmdInfo
    {
    public:
        CmdInfo():hasValidTimeOut(true){}
        bool hasValidTimeOut;
        QList<ParamInfo> paramList;
        QList<ExternalTool> externalToolInfo;
        QString cmdType;
        SubCmdType subCmdType;
        int rowIdx;
        GTACommandBase * pCmd;
    };

    struct VmacParamInfo
    {
        ParamInfo paramInfo;
        GTACommandBase *pCmd;
        int rowIdx;
        QString cmdType;
        QString CallName;
        bool isCall;
    };

    struct LogMessageInfo
    {
        QString lineNumber;
        QString fileName;
        QString uuid;
    };
    struct callCmdInfo
    {
        QString lineNum;
        QString callElemName;
        GTAActionCall *pCall;
    };

    bool getCommandStatus(GTACommandBase *& ipCmd, QList<ErrorMessage>& oErrorList,QString uuid,QStringList &oInvalidList, const int iRowIdx,bool isCall, const QString &iCallElemName);

    /**
      * This function sets the commands validity status, by reviewing all the parameters
      * used in the command, and checking them against parameter database.
      * IF the command is a call type, then it goes into recursion to unpack the Call command
      * and perform the same for each command within call.
      * @ipElem: the element handle of current eidtor element type or Call Command element in case of recursion.
      * @iMsgList : Error msg list for each command for which parameter list is not empty and the parameter is not valid.
      */
    //bool checkCommandCompatibility(const QString &iDataDirectory, GTAElement *& ipElem, QStringList &iMsgList,bool isCall, const QString & iCallElemName);
    bool checkCommandCompatibility(const QString &iDataDirectory, GTAElement *& ipElem,
                                   QList<ErrorMessage> &oErrorList,bool isCall, const QString& iCallParent,const QString & iCallElemName,
                                   QStringList &icheckForRep,
                                   const QHash<QString, GTAElemRecStruct>& hshReferncedItem =QHash<QString, GTAElemRecStruct>(),
                                   CompatibilityCallFuncErrors& isRecuriosn = *(new CompatibilityCallFuncErrors(NO_ERROR) ));
    /**
      * This function checks the editor content
      * for valid details.
      * Report is generated and will be displayed by the HMI window.
      * Details Checked
      * - Parameter used, exists in Database.
      * this function calls the private function "checkCommandCompatibility"
      */
    ErrorMessageList CheckCompatibility(const QString &iDataDirectory, const QString &iDbFile, GTAElement * pElem ,
                                        bool bPopulateVmacForRelease=false);

    /**
      * This function open the gta document and provides commands list in opElemenent
      * iFileName : file name of the document to be opened e.g. abc.obj
      * opElemenet : The document object to be retrieved
      * return true if the document is open successfully otherwise false
      */
    bool getElementFromDocument(const QString &iDataDirectory, const QString & iFileName, GTAElement *& opElemenet, bool isUUID = true);

    bool checkForRecursion(QHash<QString, GTAElemRecStruct>& hshReferncedItem, const QString& sParentName,const QString& sCurrentItemName);
    bool isFileExist(const QString isFilePath);
    QString getParamDirection(const QString &iParam);

    void setParamList(const QHash<QString, GTAICDParameter> &ParamList);

private:
    bool checkIsLocal(const QString &iParam);
    bool checkValidVal(const QString & iParam, const QString &iVal, const bool &isInDB);
    bool checkIsInDb(const QString &iParam, bool &isInDB, QString &isPIR);
    bool checkIsEngineParameter(const QString &iParam);
    bool checkForChannelInControl(const QString &iParam,QStringList &oUnAvailableParams);
    bool checkIsVmac(const QString &iParam);
    bool checkIsConstant(const QString &iVal, QString &oParamType);
    bool checkIsParamDbType(const QString &iParam);
    bool checkValidTimeOut(GTAActionDoWhile *pCmd);
    bool checkIsParamSubscribed(const QString &iParam);
    bool checkIsSubscribeValid(const QString &iParam);
    bool checkIsUnsubscribeValid(const QString &iParam);
    bool checkInvalidChars( QString text,QStringList  & chars);
    bool checkParamDirection(const QString &iCmdType, const QString &iParam);
    bool isValidPIRSignal(const QString & iParam,QString &iValidParam);
    bool isValidTagVar(const QString &iParam);
    bool checkIsParamInternal(const QString &iParam);
    QString getParamMediaType(const QString & iParam);
    QString getParamSrcType(const QString & iParam);
    bool isFileExits(const QString & iFileName);
    bool checkForRange(const QString &iParam, const QString& iVal,QString &oMinVal, QString &oMaxVal);
    ErrorMessage createErrorMsg(const QString &iDesc,const ErrorMessage::ErrorType iType, const ErrorMessage::MsgSource iMsgSrc,const QString &iFile,
                                const QString &iLine, const QString &iUUID);

    bool checkIsInternalParamValid(const GTACommandBase *pCmd, const QString &iParam);
    bool checkIsPortingRequired(QList<ErrorMessage> &oMsgList);
    bool checkIsPIR(const QString &iParam);
	void resetCheckComp();
    void addToSetParamList(GTACommandBase*& ipCmd, GTAActionSet* pSet, QStringList& emptyCases,
        CmdInfo& sCmdInfo, QString& param, QString& value, const int iRowIdx, bool isCall, const QString& iCallElemName);
    void fillCheckCSVParamList(GTACommandBase*& ipCmd, GTACheckValue* pChk, const int checkParamCount,
        CmdInfo& sCmdInfo, const int iRowIdx, bool isCall, const QString& iCallElemName);
    void fillTripletAndValueFromCSV(QFile& fileObj, QStringList& lines, const QString& delim, QHash<int, QString>& tripletsList,
        QHash<int, QString>& valuesList, QStringList& emptyCases, GTACheckValue* pChk, const int checkParamCount);
    void addToCheckParamList(GTACommandBase*& ipCmd, GTACheckValue* pChk, const int checkParamCount,
        CmdInfo& sCmdInfo, QString& param, QString& val, const int iRowIdx, bool isCall, const QString& iCallElemName);
    void addVmacParamInfo(GTACommandBase*& ipCmd, ParamInfo& sParamInfo, const int iRowIdx, bool isCall,
        const QString& iCallElemName, const QString& cmdType);
    void setValidFSTypeInParamInfo(ParamInfo& sParamInfo, QString& param, const bool& isSetOnlyFS, const bool& isSetOnlyValue);
    void addValueAndNextTypeParamInfo(ParamInfo& sParamInfo, QString& value, QString& paramType);
private:
    QHash<QString,QString> _LocalParamDetail;
    QHash<QString,GTAICDParameter> _ParamList;
    QString _LastError;
    QList<GTACheckCompatibility::VmacParamInfo> _VmacParamInfo;
    QList<GTACheckCompatibility::CmdInfo> _CommandValidityInfo;
    QString callName;
    QHash<QString,QString> _AASPIRMap;
    QHash<QString,QString> _ReturnParamList;
    QHash<QString,int> _ReturnParamObjIdHash;
    QStringList _ParamSubscribed;

    //tagged param and log info
    QHash<QString,LogMessageInfo> _TagVariable;
    //UUID list of all commands
    QStringList _CommandUUIDs;
    //map of all headers against a for each command
    QHash<QString, QStringList> _ForColHeader;

    //hash of <ref uuid of call cmd, pointer to that call cmd>
    //to check call commands with different names but same UUIDs
    QHash<QString,GTAActionCall *> _UUIDElementHash;
    QHash<GTAActionCall *, LogMessageInfo> _CallCmdHash; //<call command, log info for that call command>
    QHash<QString,QString> _LocalValuesList;
	QHash<QString,QString> _LocalParamTypesList;
	QHash<QString, QString> _LocalResultExtTool;

    // QHash contains index and element(triplet/value) for each cell of CSV file
    QHash<int,QString> _TripletsList;
    QHash<int,QString> _ValuesList;
    QString _fColumnName;
    QString _sColumnName;
    QString _FSColumnName;
	QStringList _PirsList;
	static int _nbParamList;
	QStringList _ColPirName;

	NbColumn _nbColumn;
};

#endif // GTACHECKCOMPATIBILITY_H
