#ifndef GTAACTIONEXPANDEDCALL_H
#define GTAACTIONEXPANDEDCALL_H
#include "GTAActionBase.h"
#include "GTAActionCall.h"
class GTACommands_SHARED_EXPORT GTAActionExpandedCall : public GTAActionCall
{
public:
    GTAActionExpandedCall();
    GTAActionExpandedCall(const QString &iAction, const QString & iComplement,const QString & iElement);
    GTAActionExpandedCall(GTAActionExpandedCall & iObj);
    GTAActionExpandedCall(const GTAActionCall& iObj);

    virtual ~GTAActionExpandedCall();

    void setBrokenLink(bool iBrokenStatus){_IsBrokenLink=iBrokenStatus;}
    bool hasBrokenLink(){return _IsBrokenLink;}
    virtual QList<GTACommandBase*>& getChildren(void) const;
    virtual bool insertChildren(GTACommandBase* pBase,const int& idPos);
    virtual void setParent(GTACommandBase* iParent);
    virtual GTACommandBase* getParent(void) const;
    virtual GTACommandBase *getClone() const;
    virtual  bool hasReference();
    QColor getForegroundColor() const ;
    virtual bool canHaveChildren() const;

    virtual QStringList getVariableList() const;

    virtual void ReplaceTag(const QMap<QString,QString>& iTagValueMap) ;

    bool logCanExist() const{return false;}
    QList<GTACommandBase*> getExpandedList(void);
    QString getSCXMLStateName()const;
    bool hasPrecision() const{return false;}
    bool hasTimeOut() const{return true;}
     bool hasChannelInControl()const {return false;}

    virtual QString getExecutionTime() const;


private:

    bool _IsBrokenLink;
    
};

#endif // GTAACTIONEXPANDEDCALL_H
