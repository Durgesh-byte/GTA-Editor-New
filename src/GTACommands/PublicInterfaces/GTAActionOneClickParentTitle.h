#ifndef GTAACTIONONECLICKPARENTTITLE_H
#define GTAACTIONONECLICKPARENTTITLE_H
#include "GTAActionBase.h"
#include "GTACommands.h"
#include "GTAActionTitle.h"

class GTACommands_SHARED_EXPORT GTAActionOneClickParentTitle : public GTAActionTitle
{
public:
    GTAActionOneClickParentTitle(GTAActionTitle* pTitle,const QString &iAction,const QString& iComplement);
    GTAActionOneClickParentTitle(const GTAActionOneClickParentTitle& iObjTitle);
    virtual ~GTAActionOneClickParentTitle();
    virtual GTACommandBase*getClone() const;
    virtual int getAllChildrenCount(void);
    virtual QList<GTACommandBase*>& getChildren(void) const;
    virtual bool insertChildren(GTACommandBase* pBase,const int& idPos);
    bool canHaveChildren() const;
    QString getSCXMLStateName()const;
    bool isOneClickTitle(){return true;}
    bool hasPrecision() const{return false;}
    bool hasTimeOut() const{return false;}
    bool hasChannelInControl()const {return false;}

};

#endif // GTAActionParentTitle_H
