#ifndef GTAACTIONFAILURE_H
#define GTAACTIONFAILURE_H
#include "GTAActionBase.h"
#include "GTAFailureConfig.h"

class GTACommands_SHARED_EXPORT GTAActionFailure : public GTAActionBase
{
public:
   
    enum Failure_Type{START, STOP, PAUSE, RESUME};
    GTAActionFailure();
    GTAActionFailure(const QString &iAction, const QString & iComplement) : GTAActionBase(iAction,iComplement){}
    GTAActionFailure(const GTAActionFailure& rhs);
    
   
    void setFailureName(const QString& iName){_failureName = iName;}
    QString getFailureName() const {return _failureName;}

    void setParamValConfig(QList <QPair<QString,GTAFailureConfig* >> ilstParamCinfigVal);
    QList <QPair<QString,GTAFailureConfig* >> getParamValConfig()const{return _lstParamConfigVal;}

    void setFailureType(Failure_Type iType){_FailureType=iType;}
    Failure_Type getFailureType()const{return _FailureType;}
    QString getFailureTypeInString()const;

    bool isFailureCommand(){return true;}


  

    virtual QString getStatement() const;



    virtual QList<GTACommandBase*>& getChildren(void) const;
    virtual bool insertChildren(GTACommandBase* pBase,const int& idPos);
    virtual void setParent(GTACommandBase* iParent);
    virtual GTACommandBase* getParent(void) const;

    virtual GTACommandBase*getClone() const;
    virtual bool canHaveChildren() const;
    virtual QStringList getVariableList() const;
    virtual void  ReplaceTag(const QMap<QString,QString>& iTagValueMap) ;

    bool stringReplaceApplicable()const{return true;}
    void stringReplace(const QRegExp& iStrToFind,const QString& iStringToReplace);
    bool searchString(const QRegExp& iRegExp,bool ibTextSearch)const;
    QString getSCXMLStateName()const;
    void replaceUntagged();
    bool hasPrecision() const{return false;}
    bool hasTimeOut() const{return false;}
    bool hasChannelInControl()const{return false;}


private:
  
    QString _failureName;
    Failure_Type _FailureType;
   
    QList <QPair<QString,GTAFailureConfig* >> _lstParamConfigVal;
        
   
};

#endif // GTAActionPrintTable_H
