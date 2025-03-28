#ifndef GTAActionOneClickPPC_H
#define GTAActionOneClickPPC_H
#include "GTAActionBase.h"
class GTACommands_SHARED_EXPORT GTAActionOneClickPPC : public GTAActionBase
{
public:


    GTAActionOneClickPPC(const QString &iAction,const QString & iComplement);

    GTAActionOneClickPPC(const GTAActionOneClickPPC& iCmd);

    ~GTAActionOneClickPPC();

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

    void setPPCPath(const QString &iPath);
    QString getPPCPath() const;

    void setIsStart(bool iIsStart) {_IsStart = iIsStart;}
    bool getIsStart() const {return _IsStart;}

     bool isOneClick(){return true;}
     QString getSCXMLStateName()const;
     bool hasPrecision() const{return false ;}
     bool hasTimeOut() const{return false;}
      bool hasChannelInControl()const {return false;}


private:
    QString _PPCPath;
    bool _IsStart;

};

#endif // GTAActionOneClickPPC_H
