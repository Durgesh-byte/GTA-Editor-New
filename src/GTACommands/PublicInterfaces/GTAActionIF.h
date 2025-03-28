#ifndef GTAACTIONIF_H
#define GTAACTIONIF_H
#include <GTAActionBase.h>
class GTACommands_SHARED_EXPORT GTAActionIF : public GTAActionBase
{
public:
    GTAActionIF();
    GTAActionIF(const QString &iAction, const QString & iComplement) : GTAActionBase(iAction,iComplement){}
    virtual ~GTAActionIF();

    GTAActionIF(const GTAActionIF& rhs);
    void setParameter(const QString & iParam);
    void setValue(const QString & iParam);
    void setCondition(const QString & iOperator);

    QString getParameter() const;
    QString getValue()const;
    QString getCondition() const;

    //GTAActionBase * getSubAction() const;

    void setChkAudioAlarm(const bool iChkAudioAlarm);
    bool getChkAudioAlarm()const;
    
    virtual QString getStatement() const;
    QString getLTRAStatement() const;
    virtual QList<GTACommandBase*>& getChildren(void) const;
    virtual bool insertChildren(GTACommandBase* pBase,const int& idPos);
    virtual void setParent(GTACommandBase* iParent);
    virtual GTACommandBase* getParent(void) const;
    bool createEndCommand(GTACommandBase* & opCmd)const;

    virtual GTACommandBase*getClone() const;

    //bool hasElse();

    void setHeardAfter(const QString & iHeaderAfter);
    QString getHeardAfter()const;

    virtual bool hasChildren();
    virtual bool canHaveChildren() const;
    bool hasMutuallExclusiveStatement(int* opPos=NULL)const;
    bool canHaveMutuallExclusiveStatement()const;

    virtual QStringList getVariableList() const;
    virtual void  ReplaceTag(const QMap<QString,QString>& iTagValueMap) ;

    bool stringReplaceApplicable()const{return true;}
    void stringReplace(const QRegExp& iStrToFind,const QString& iStringToReplace);
    bool equipmentReplace(const QRegExp& iStrToFind, const QString& iStringToReplace) override;
    bool searchString(const QRegExp& iRegExp,bool ibTextSearch)const;
    bool expandReferences();

    bool setDumpList(const QStringList& iDumpList);
    QStringList getDumpList()const;

    QString precisionCondition(const QString &iLhsVal, const QString &iRhsVal,const QString &op,const QString &iPrecision,const QString &precisionUnit,double dPrec, bool isGenericSCXML)const;

    bool getSCXMLConditionStatement(const QStringList &iIcdParamList, QString &oCondtionStatement, const QString &iEngine = QString(), bool isGenericSCXML = false, const QStringList iLocalVarList = QStringList()) const;
    QString getSCXMLCondition(const QString &iLhs, const QString &iRhs,const QStringList &iIcdParamList, bool isGenericSCXML,const QStringList iLocalVarList = QStringList())const;
    QString getSCXMLStateName()const;
    void replaceUntagged();
    bool hasPrecision() const{return true;}
    bool hasTimeOut() const{return true;}
    bool hasChannelInControl()const;
    bool hasConfirmatiomTime() {return true;}
    virtual QString getGlobalResultStatus();
    virtual bool isCompoundState(){return true;}
    virtual QString getCompounsStateId(){return QString("UserDef_IfElse");}

    void setIsFsOnly(const bool &iVal);
    bool getIsFsOnly()const;

    void setIsValueOnly(const bool &iVal);
    bool getIsValueOnly()const;

    QString getFunctionalStatus()const;
    void setFunctionalStatus(const QString &iFuncStatus);


    QStringList resolveEngineParam()const;
    QStringList resolveEngineParam(const QString &iParam)const;

    void setAutoAddElse(bool isAutoAddElse) { _autoAddElse = isAutoAddElse; };
    bool autoAddElse() { return _autoAddElse; };

    virtual bool IsUserEditable() { return true; };

private:
    QString     _Param;
    QString     _ParamValue;
    QString     _Operator;
    QString     _HeardAfter;
    bool        _ChkAudioAlarm;
    bool        _IsFSOnly;
    bool        _IsValueOnly;
    QString     _FunctionalStatus;
    bool        _autoAddElse = false;
};

#endif // GTAACTIONIF_H
