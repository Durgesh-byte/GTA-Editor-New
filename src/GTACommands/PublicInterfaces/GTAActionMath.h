#ifndef GTAActionMath_H
#define GTAActionMath_H
#include "GTAActionBase.h"
class GTACommands_SHARED_EXPORT GTAActionMath : public GTAActionBase
{
public:

    GTAActionMath();

    GTAActionMath(const GTAActionMath& rhs);
    
    
    void setFirstVariable(const QString & ipVar);
    void setSecondVariable(const QString & ipVar);
    void setLocation(const QString & iLocation);
    void setOperator(const QString & iOperator);
   
    QString getFirstVariable()const;
    QString getSecondVariable()const;
    QString getLocation()const;
    QString getOperator()const;
    
    QString getStatement() const;



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
    bool hasPrecision() const{return false;}
    bool hasTimeOut() const{return false;}
     bool hasChannelInControl()const {return false;}

private:
    QString _var1;
    QString _var2;
    QString _operator;
    QString _Location;
};

#endif // GTAActionMath_H
