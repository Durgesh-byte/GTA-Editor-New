#ifndef GTAACTIONWAIT_H
#define GTAACTIONWAIT_H
#include "GTAActionBase.h"
class GTACommands_SHARED_EXPORT GTAActionWait : public GTAActionBase
{
public:
    enum WaitType{UNTIL,FOR};
    GTAActionWait(WaitType iType);
    GTAActionWait(const QString &iAction,
                     const QString & iComplement,WaitType iType);

    GTAActionWait(const GTAActionWait& iCmd);

    ~GTAActionWait();

    inline void setParameter(const QString & iParam){_Param = iParam;}
    inline void setValue(const QString & iParamVal){_ParamValue = iParamVal;}
    inline void setCondition(const QString & iOper){_Operator = iOper;}
    inline void setCounter(const QString & iCounter){_Counter = iCounter;}
    inline QString getParameter() const{return _Param;}
    inline QString getValue() const{return _ParamValue;}
    inline QString getCondition() const {return _Operator;}
    inline QString getCounter() const {return _Counter;}

    WaitType getWaitType() const;


    virtual QString getStatement() const;
    virtual QString getLTRAStatement() const;
    virtual QList<GTACommandBase*>& getChildren(void) const;
    virtual bool insertChildren(GTACommandBase* pBase,const int& idPos);
    virtual void setParent(GTACommandBase* iParent);
    virtual GTACommandBase* getParent(void) const;
    virtual GTACommandBase *getClone() const;

    virtual bool canHaveChildren() const;
    virtual QStringList getVariableList() const;
    QStringList resolveEngineParam(const QString &iParam)const;
//    QStringList resolveEngineParam(const QString &iParam, const QString &iValue)const;
    virtual void  ReplaceTag(const QMap<QString,QString>& iTagValueMap);

    bool stringReplaceApplicable()const{return true;}
    void stringReplace(const QRegExp& iStrToFind,const QString& iStringToReplace);
    bool equipmentReplace(const QRegExp& iStrToFind, const QString& iStringToReplace) override;
    bool searchString(const QRegExp& iRegExp,bool ibTextSearch)const;
    bool setDumpList(const QStringList& iDumpList);
    QStringList getDumpList()const;
    QString getSCXMLStateName()const;
    bool getSCXMLConditionStatement(const QStringList &iIcdParamList, QString &oCondtionStatement, const QString &iEngine = QString(), bool isGenericSCXML = false, const QStringList iLocalVarList=QStringList()) const;
    QString getSCXMLCondition(const QString &iLhs, const QString &iRhs,const QStringList &iIcdParamList, bool isGenericSCXML, const QStringList iLocalVarList = QStringList())const;

    bool hasLoop() const;
    bool hasTimeOut() const;
    bool hasPrecision() const;
    bool hasChannelInControl()const ;
    bool hasConfirmatiomTime() {return true;}
    virtual bool isCompoundState(){return (_WaitType==UNTIL)?true:false;}
    virtual QString getCompounsStateId(){return QString("UserDef_WaitUntil");}

    void setIsFsOnly(const bool &iVal);
    bool getIsFsOnly()const;

    void setIsValueOnly(const bool &iVal);
    bool getIsValueOnly()const;

    QString getFunctionalStatus()const;
    void setFunctionalStatus(const QString &iFuncStatus);

    QString getLoopSampling()const;
    void setLoopSampling(const QString &iLoopSampling);

    bool getIsLoopSampling()const;
    void setIsLoopSampling(const bool &iVal);

    QString getUnitLoopSampling()const;
    void setUnitLoopSampling(const QString &iVal);

    QString precisionCondition(const QString &iLhsVal, const QString &iRhsVal,const QString &op,const QString &iPrecision,const QString &precisionUnit,double dPrec, bool isGenericSCXML)const;

private:

    void setWaitType(WaitType iType);

    WaitType    _WaitType;
    QString     _Param;
    QString     _ParamValue;
    QString     _Operator;
    QString     _Counter;
    bool        _IsFSOnly;
    bool        _IsValueOnly;
    QString     _FunctionalStatus;
    QString     _LoopSampling;
    bool        _IsLoopSampling;
    QString     _UnitLoopSampling;

};

#endif // GTAACTIONWAIT_H
