#ifndef GTACMDSERIALIZER_H
#define GTACMDSERIALIZER_H

#include "GTACommandsSerializer.h"
#include "GTACommandBase.h"
#include "GTAInitConfigBase.h"
#include "GTAActionBase.h"
#include "GTACheckBase.h"

#pragma warning(push, 0)
#include <QDomElement>
#include <QDomDocument>
#pragma warning(pop)

class GTACommandsSerializer_SHARED_EXPORT GTACmdSerializer
{
public:
    GTACmdSerializer();
    virtual ~GTACmdSerializer(){}
    virtual bool serialize(QDomDocument &iDomDoc, QDomElement &oElement) = 0;
    virtual bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand) = 0;
   // virtual bool deserialize(const QDomElement & iElement, GTAInitConfigBase *& opCommand);

protected:
    bool setActionAttributes(GTAActionBase* pAct, QDomElement& actionElementNode);
    bool getActionAttributes(const QDomElement& actionElementNode,GTAActionBase* pAct);
    bool setActionAttributes(GTACheckBase* pChk, QDomElement& actionElementNode);
    bool getActionAttributes(const QDomElement& actionElementNode,GTACheckBase* pChk);
};

#endif // GTACMDSERIALIZER_H
