#ifndef GTACMDSERIALIZERINTERFACE_H
#define GTACMDSERIALIZERINTERFACE_H
#include "GTACore.h"
#include "GTACommandBase.h"
#include "GTAInitConfigBase.h"
#include "GTAActionBase.h"
#include "GTACheckBase.h"

#pragma warning(push, 0)
#include <QDomElement>
#include <QDomDocument>
#pragma warning(pop)

class GTACore_SHARED_EXPORT GTACmdSerializerInterface
{
public:
    GTACmdSerializerInterface();
    virtual ~GTACmdSerializerInterface(){}
    virtual bool serialize(QDomDocument &iDomDoc, QDomElement &oElement) = 0;
    virtual bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand) = 0;
    virtual void setCommand(const GTACommandBase*)=0;
    //virtual bool setCommand();
   // virtual bool deserialize(const QDomElement & iElement, GTAInitConfigBase *& opCommand);

protected:
    bool setActionAttributes(GTAActionBase* pAct, QDomElement& actionElementNode);
    bool getActionAttributes(const QDomElement& actionElementNode,GTAActionBase* pAct);
    bool setActionAttributes(GTACheckBase* pChk, QDomElement& actionElementNode);
    bool getActionAttributes(const QDomElement& actionElementNode,GTACheckBase* pChk);
};

#endif // GTACmdSerializerInterface_H
