#ifndef GTAACTIONPARENTTITLE_H
#define GTAACTIONPARENTTITLE_H
#include "GTAActionBase.h"
#include "GTACommands.h"
#include "GTAActionTitle.h"

class GTACommands_SHARED_EXPORT GTAActionParentTitle : public GTAActionTitle
{
public:
    GTAActionParentTitle(GTAActionTitle* pTitle,const QString &iAction);
    GTAActionParentTitle(const GTAActionParentTitle& iObjTitle);
    virtual ~GTAActionParentTitle();
   

    
    virtual int getAllChildrenCount(void);
    virtual QList<GTACommandBase*>& getChildren(void) const;
    virtual bool insertChildren(GTACommandBase* pBase,const int& idPos);
    virtual GTACommandBase*getClone() const;
    virtual bool canHaveChildren() const;
    QString getSCXMLStateName()const;
    bool hasPrecision() const{return false;}
    bool hasTimeOut() const{return false;}
     bool hasChannelInControl()const {return false;}
    bool logCanExist() const {return false;}
    QString getGlobalResultStatus();
    void getUningnoredChildren(QList<GTACommandBase *> &unIgnoredChildren) const;
    virtual QString getExecutionTime() const;
    double getExecutionEpochTime()const;

    void setCollapsed(bool iCollapsed);
    bool getCollapsed();
//    bool canHaveInstanceName();


private:
    bool _IsCollapsed;

};

#endif // GTAActionParentTitle_H
