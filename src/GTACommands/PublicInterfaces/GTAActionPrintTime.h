#ifndef GTAActionPrintTime_H
#define GTAActionPrintTime_H
#include "GTAActionBase.h"
class GTACommands_SHARED_EXPORT GTAActionPrintTime : public GTAActionBase
{
public:
   
    GTAActionPrintTime();
    GTAActionPrintTime(const GTAActionPrintTime& iCmd);

    ~GTAActionPrintTime();
    enum TimeType{Formatted,UTC};

    void setTimeType(TimeType iTime);
    GTAActionPrintTime::TimeType getTimeType()const;




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
    virtual QString getLTRAStatement()  const;
     bool hasLoop() const;
     bool hasPrecision() const{return false;}
     bool hasTimeOut() const{return false;}
      bool hasChannelInControl()const {return false;}


private:

   TimeType _TimeType;


};

#endif // GTAActionPrintTime_H
