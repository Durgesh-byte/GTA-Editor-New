#ifndef GTAACTIONCONDITIONSERIALIZER_H
#define GTAACTIONCONDITIONSERIALIZER_H
#include <GTAActionCondition.h>
#include "GTACommandsSerializer.h"
#include "GTACmdSerializer.h"

#pragma warning(push, 0)
#include <QDomDocument>
#pragma warning(pop)


class GTACommandsSerializer_SHARED_EXPORT GTAActionConditionSerializer : public GTACmdSerializer
{
public:
    GTAActionConditionSerializer(GTAActionCondition * ipActionCmd );

    virtual bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    virtual bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);

 private:
    GTAActionCondition * _pActionCmd;
};

#endif // GTAACTIONCONDITIONSERIALIZER_H
