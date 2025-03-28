#ifndef GTAACTIONDOWHILE_H
#define GTAACTIONDOWHILE_H
#include <GTAActionBase.h>
class GTACommands_SHARED_EXPORT GTAActionDoWhile : public GTAActionBase
{
public:
    GTAActionDoWhile();
    GTAActionDoWhile(const QString &iAction, const QString & iComplement) : GTAActionBase(iAction,iComplement){}
    virtual ~GTAActionDoWhile();

    GTAActionDoWhile(const GTAActionDoWhile& rhs);
    void setParameter(const QString & iParam);
    void setValue(const QString & iParam);
    void setCondition(const QString & iOperator);

    QString getParameter() const;
    QString getValue()const;
    QString getCondition() const;
    void setChkAudioAlarm(const bool iChkAudioAlarm);
    bool getChkAudioAlarm()const;
    void setHeardAfter(const QString & iHeaderAfter);
    QString getHeardAfter()const;
    //GTAActionBase * getSubAction() const;

    virtual bool isCompoundState(){return true;}
    virtual QString getCompounsStateId(){return QString("UserDef_DoWhile");}


    QString getStatement() const;
    QList<GTACommandBase*>& getChildren(void) const;
    bool insertChildren(GTACommandBase* pBase,const int& idPos);
    void setParent(GTACommandBase* iParent);
    GTACommandBase* getParent(void) const;
    bool createEndCommand(GTACommandBase* & opCmd)const;
    GTACommandBase*getClone() const;

    virtual QString getLTRAStatement() const;
    virtual bool hasChildren();
    virtual bool canHaveChildren() const;

    virtual QStringList getVariableList() const;
    virtual void  ReplaceTag(const QMap<QString,QString>& iTagValueMap) ;
    bool getSCXMLConditionStatement(const QStringList &iIcdParamList, QString &oCondtionStatement, const QString &iEngine = QString(), bool isGenericSCXML = false, const QStringList iLocalVarList = QStringList()) const;
    QString getSCXMLCondition(const QString &iLhs, const QString &iRhs,const QStringList &iIcdParamList, bool isGenericSCXML, const QStringList iLocalVarList=QStringList())const;

    bool stringReplaceApplicable()const{return true;}
    void stringReplace(const QRegExp& iStrToFind,const QString& iStringToReplace);
    bool equipmentReplace(const QRegExp& iStrToFind, const QString& iStringToReplace) override;
    bool searchString(const QRegExp& iRegExp,bool ibTextSearch)const;
    bool expandReferences();
    bool setDumpList(const QStringList& iDumpList);
    QStringList getDumpList()const;

    QString getSCXMLStateName()const;
    bool hasLoop() const;
    void replaceUntagged();
    inline void setLogMessageList(const QList<GTAScxmlLogMessage> & iList);

    QString getStatementForEnd();
    virtual QString getGlobalResultStatus();
    bool hasPrecision() const{return true;}
    bool hasTimeOut() const{return true;}
    bool hasChannelInControl()const;

    void setIsFsOnly(const bool &iVal);
    bool getIsFsOnly()const;

    void setIsValueOnly(const bool &iVal);
    bool getIsValueOnly()const;

    QString getFunctionalStatus()const;
    void setFunctionalStatus(const QString &iFuncStatus);

//    void setIsRepeatedLogging(const bool &iVal);
//    bool getIsRepeatedLogging()const;

    QStringList resolveEngineParam(const QString &iParam)const;
    QStringList resolveEngineParam()const;

    QString precisionCondition(const QString &iLhsVal, const QString &iRhsVal,const QString &op,const QString &iPrecision,const QString &precisionUnit,double dPrec, bool isGenericSCXML)const;

private:
    QString     _Param;
    QString     _ParamValue;
    QString     _Operator;
    bool        _ChkAudioAlarm;
    QString     _HeardAfter;
    bool        _IsFSOnly;
    bool        _IsValueOnly;
    QString     _FunctionalStatus;
//    bool        _IsRepeatedLogging;

};

#endif // GTAActionDoWhile_H
