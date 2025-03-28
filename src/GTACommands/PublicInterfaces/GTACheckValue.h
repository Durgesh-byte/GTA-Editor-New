#ifndef GTACHECKVALUE_H
#define GTACHECKVALUE_H
#include "GTACommands.h"
#include "GTACheckBase.h"
#include "GTAUtil.h"
#include "GTAStructCheck.h"

#pragma warning(push, 0)
#include <QString>
#include <QStringList>
#pragma warning(pop)

#define CHK_FS_STR "[Check functional status]"
#define CHK_VALUE_ONLY_STR "[Check Only Value]"
#define CHK_SDI_STR "[CHECK SDI]"
#define CHK_PARITY_STR "[CHECK PARITY]"
#define CHK_REFRESH_RATE_STR "[CHECK REFRESH RATE]"

class GTACheckBase;
class GTACommands_SHARED_EXPORT GTACheckValue : public GTACheckBase
{

public:
    GTACheckValue();
    GTACheckValue(const GTACheckValue& rhs);
    virtual ~GTACheckValue();
    void insertParamenter(const QString& iParam,
                          const QString& iCondition,
                          const QString& iValue,
                          const QString& iFuncStatus,
                          const double& isPrecision,
						  const QString& isPrecisionFE,
                          const QString &isPrecisionType,
                          const bool isCheckOnlyValue = false,
                          const bool isCheckFS = false,
                          const bool isSDI = false,
                          const bool isParity = false,
                          const bool isRefreshRate = false,
                          const QString &iSDI = QString("00"),
                          const QString &iParity = QString("0"));
	
	struct StructCheck getAllParameters();

	void setParameters(struct StructCheck parameters);

    void insertBinaryOperator(const QString& iBinaryOperator);
    void removeParameter(const int &iIndex);



    void clearAllParameters();
    int getCount()const;

    QString getParameter(const int & iIndex) const;
    QString getValue(const int & iIndex) const;
    QString getCondition(const int & iIndex) const;
    QString getBinaryOperator(const int & iIndex) const;
    QString getFunctionalStatus(const int & iIndex) const;
    
    QString getSDI(const int & iIndex) const;
    QString getParity(const int & iIndex) const;

    bool getIsRefreshRateOnly(const int & iIndex) const;
    bool getIsParityOnly(const int & iIndex) const;
    bool getIsSDIOnly(const int & iIndex) const;
    bool getISCheckFS(const int & iIndex) const;
    double getPrecisionValue(const int & iIndex) const;
	QString getPrecisionValueFE(const int & iIndex) const;
	QStringList getAllPrecisionValueFE() const;
    QString getPrecisionType(const int & iIndex) const;

    
    bool getWaitUntil() const ;
    void setWaitUntil(bool iVal);
    virtual QString getStatement() const;
    virtual QList<GTACommandBase*>& getChildren(void) const;
    virtual bool insertChildren(GTACommandBase* pBase,const int& idPos);
    virtual void setParent(GTACommandBase* iParent);
    virtual GTACommandBase* getParent(void) const;
    virtual GTACommandBase* getClone()const;

    virtual QStringList getVariableList() const;
    virtual void  ReplaceTag(const QMap<QString,QString>& iTagValueMap) ;
    bool getISOnlyCheckValue(const int & iIndex) const;
    virtual QString getLTRAStatement() const;
    virtual bool getSCXMLConditionStatement(const QStringList &iIcdParamList, QString &oCondtionStatement, const QHash<QString, QString> &iGenToolReturnMap, const QString &iEngine = QString(), bool isGenericSCXML = false,const QStringList &iLocalVarList=QStringList()) const;
    QStringList getCheckValueConditionForSCXML(const QString &iLhs, const QString &iRhs,const QStringList &iIcdParamList,const int &iIndex, bool isGenericSCXML,const QStringList &iLocalVarList = QStringList())const;
    void updateWithGenToolParam(QString &ioParam, const QHash<QString, QString> &iGenToolReturnMap) const;

    bool stringReplaceApplicable()const{return true;}
    void stringReplace(const QRegExp& iStrToFind,const QString& iStringToReplace);
    bool equipmentReplace(const QRegExp& iStrToFind, const QString& iStringToReplace) override;
    bool searchString(const QRegExp& iRegExp,bool ibTextSearch)const;
    bool setDumpList(const QStringList& iDumpList);
    QStringList getDumpList()const;
    QString getSCXMLStateName()const;

    bool hasLoop() const;
    bool hasChannelInControl()const;
    bool hasConfirmatiomTime() {return true;}

    virtual bool isCompoundState(){return true;}
    virtual QString getCompounsStateId(){return QString("UserDef_CheckValue");}


    QStringList resolveEngineParam(const QString &iParam)const;
    QStringList resolveEngineParam()const;

    QString precisionCondition(const QString &iLhsVal, const QString &iRhsVal,const QString &op,const QString &iPrecision,const QString &precisionUnit,double dPrec, bool isGenericSCXML)const;
     /*@DESC: The function provides index of the parameter requested.
    /*@Param: Input is the name of the param
    /*@Param: Output mentioning the param was found in params or vals
    /*@Output: Index of the requested param
    */
    int getIndex(QString &iParam,QString &oParam_Val);
    void setFreeTextCondition(const QString &iText);
    QString getFreeTextCondition()const;
    void getConditionStatement(const QStringList &iIcdParamList, QString &oCondtionStatement, const QHash<QString, QString> &iGenToolReturnMap, const QString &iEngine, bool isGenericSCXML, const QStringList &iLocalVarList) const;
    void getConditionStatementForFreeText(const QStringList &iIcdParamList, QString &oCondtionStatement, const QHash<QString, QString> &iGenToolReturnMap, const QString &iEngine, bool isGenericSCXML, const QStringList &iLocalVarList) const;
	QString getEvalCondId() const;
	void setEvalCondId(QString const);

	QStringList getListParameters() const;

    void setLastEditedRow(const int iRow) { _lastEditedRow = iRow; };
    int getLastEditedRow() const { return _lastEditedRow; };
    void setLastEditedCol(const int iLastEditedCol) { _lastEditedCol = iLastEditedCol; };
    int getLastEditedCol() const { return _lastEditedCol; };

private:
    QStringList     _lstParam;
    QStringList     _lstValue;
    QStringList     _lstCondition;
    QStringList     _lstFunctionalStatus;
    QList<double>   _lstPrecisionValue;
	QStringList		_lstPrecisionValueFE; //List containing the names of the lines used in the foreach. 
    QStringList     _lstPrecisionType;
    QStringList     _lstBinaryOperators;
    bool            _IsWaitUntil;
    QList<bool>     _IsCheckOnlyValue;
    double          _ConfirmationCheckTime;
    QList<bool>     _IsCheckFS;
    QList<bool>     _IsSDIOnly;
    QList<bool>     _IsParityOnly;
    QList<bool>     _IsRefreshRateOnly;
    QStringList     _lstSDIStatus;
    QStringList     _lstParityStatus;
    QString         _freeTextCondition;            //free text check
	QString			_evalCondId;
    int _lastEditedRow = 0;
    int _lastEditedCol = 0;
};

#endif // GTACHECKVALUE_H
