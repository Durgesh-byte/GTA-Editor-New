#ifndef GTAActionOneClickTitle_H
#define GTAActionOneClickTitle_H
#include "GTAActionBase.h"
#include "GTACommands.h"
#include "GTAActionTitle.h"

class GTACommands_SHARED_EXPORT GTAActionOneClickTitle : public GTAActionTitle
{
public:
    GTAActionOneClickTitle(const QString &iAction,const QString& iComplement);
    GTAActionOneClickTitle(const GTAActionOneClickTitle& iObjTitle);
    virtual ~GTAActionOneClickTitle();
    virtual QString getStatement() const;
    GTACommandBase* getClone()const;
    bool isOneClickTitle(){return true;}
    bool isTitle(){return false;}
    bool hasPrecision() const{return false;}
    bool hasTimeOut() const{return false;}
     bool hasChannelInControl()const {return false;}
private:

};

#endif // GTAActionOneClickTitle_H
