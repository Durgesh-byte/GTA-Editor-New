#ifndef GTAACTIONONECLICKSET_H
#define GTAACTIONONECLICKSET_H
#include "GTAActionBase.h"
class GTAEquationBase;
class  GTACommands_SHARED_EXPORT GTAActionOneClickSet : public GTAActionBase
{
public:
    GTAActionOneClickSet();
    GTAActionOneClickSet(const GTAActionOneClickSet& rhs);
    GTAActionOneClickSet(const QString &iAction, const QString & iComplement);
    void setParameter(const QString & iParameter);
    void setEquation(GTAEquationBase *& ipEquation);

    void setFunctionStatus(const QString& iFs);
    QString getFunctionStatus()const;


    QString getParameter() const;
    QString getValue() const;
   
    GTAEquationBase * getEquation() const;
    virtual QString getStatement() const;

    virtual QList<GTACommandBase*>& getChildren(void) const;
    virtual bool insertChildren(GTACommandBase* pBase,const int& idPos);
    virtual void setParent(GTACommandBase* iParent);
    virtual GTACommandBase* getParent(void) const;

    virtual GTACommandBase*getClone() const;
    virtual bool canHaveChildren() const;
    bool isOneClick(){return true;}

    virtual QStringList getVariableList() const;
    virtual void ReplaceTag(const QMap<QString,QString> & iTagValueMap);
    bool stringReplaceApplicable()const{return true;}
    void stringReplace(const QRegExp& iStrToFind,const QString& iStringToReplace);
    bool equipmentReplace(const QRegExp& iStrToFind, const QString& iStringToReplace) override;
    bool searchString(const QRegExp& iRegExp,bool ibTextSearch)const;
    bool getIsSetOnlyFSFixed() const;
    void setIsSetOnlyFSFixed(const bool iIsSetOnlyFS = false);
    bool hasPrecision() const{return false;}
    bool hasTimeOut() const{return false;}
    bool hasChannelInControl()const {return false;}


private:
    GTAEquationBase *  _pEquation;
    QString               _Parameter;
    QString               _VmacForceType;
    QString               _FunctionalStatus;
    bool                  _IsSetOnlyFS; 
};

#endif // GTAActionOneClickSet_H
