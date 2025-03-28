#ifndef GTAACTIONCONDITION_H
#define GTAACTIONCONDITION_H
#include <GTAActionBase.h>
class GTACommands_SHARED_EXPORT GTAActionCondition : public GTAActionBase
{
public:
    GTAActionCondition();
    GTAActionCondition(const QString &iAction, const QString & iComplement) : GTAActionBase(iAction,iComplement){}
    virtual ~GTAActionCondition();
    void setParameter(const QString & iParam);
    void setValue(const QString & iParam);
    void setCondition(const QString & iOperator);

    QString getParameter() const;
    QString getValue()const;
    QString getCondition() const;
    

    virtual QString getStatement() const;
    virtual QList<GTACommandBase*>& getChildren(void) const;
    virtual bool insertChildren(GTACommandBase* pBase,const int& idPos);
    virtual void setParent(GTACommandBase* iParent);
    virtual GTACommandBase* getParent(void) const;
    virtual bool canHaveChildren() const;
    virtual QStringList GTAActionCondition::getVariableList() const;
    virtual void  ReplaceTag(const QMap<QString,QString>& iTagValueMap);
    GTACommandBase*getClone() const;
    void replaceUntagged();
    virtual QString getLTRAStatement()  const;
    bool hasPrecision() const{return true;}
    bool hasTimeOut() const{return true;}
    bool hasChannelInControl() const{return false;}
    virtual QString getGlobalResultStatus();
    virtual bool isCompoundState(){return true;}
    virtual QString getCompounsStateId(){return QString("UserDef_IfElse");}
private:
    QString     _Param;
    QString     _ParamValue;
    QString     _Operator;
    bool        _ElseCondtion;

};

#endif // GTAACTIONCONDITION_H
