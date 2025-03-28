#ifndef GTAACTIONSETSERIALIZER_H
#define GTAACTIONSETSERIALIZER_H
#include "GTACmdSerializer.h"
#include "GTAActionSet.h"
class GTACommandsSerializer_SHARED_EXPORT GTAActionSetSerializer : public GTACmdSerializer
{
public:
    GTAActionSetSerializer(GTAActionSet * pActionCmd);
    virtual bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    virtual bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);

private:
    GTAActionSet * _pActionCmd;
};

#endif // GTAACTIONSETSERIALIZER_H
