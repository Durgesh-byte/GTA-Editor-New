#ifndef GTAACTIONFUNCTIONRETURN_H
#define GTAACTIONFUNCTIONRETURN_H
#include "GTAActionBase.h"
class GTACommands_SHARED_EXPORT GTAActionFunctionReturn : public GTAActionBase
{
public:
   
    GTAActionFunctionReturn();
    GTAActionFunctionReturn(const GTAActionFunctionReturn& iCmd);

    ~GTAActionFunctionReturn();
   

    void setReturnItem(const QString&  iTime);
    QString getReturnItem()const;




    virtual QString getStatement() const;
    virtual QList<GTACommandBase*>& getChildren(void) const;
    virtual bool insertChildren(GTACommandBase* pBase,const int& idPos);
    virtual void setParent(GTACommandBase* iParent);
    virtual GTACommandBase* getParent(void) const;
    virtual GTACommandBase *getClone() const;

    virtual bool canHaveChildren() const;
    virtual QStringList getVariableList() const;
    virtual void  ReplaceTag(const QMap<QString,QString>& iTagValueMap);

    bool stringReplaceApplicable()const{return true;}
    void stringReplace(const QRegExp& iStrToFind,const QString& iStringToReplace);
    bool searchString(const QRegExp& iRegExp,bool ibTextSearch)const;
    bool setDumpList(const QStringList& iDumpList);
    QStringList getDumpList()const;
    QString getSCXMLStateName()const;

     bool hasLoop() const;
     bool hasPrecision() const{return false;}
     bool hasTimeOut() const{return false;}
      bool hasChannelInControl()const;


private:

   QString _ReturnItem;


};

#endif // GTAActionFunctionReturn_H
