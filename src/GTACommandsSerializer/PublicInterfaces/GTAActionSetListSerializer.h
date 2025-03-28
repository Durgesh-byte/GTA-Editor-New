#ifndef GTAACTIONSETLISTSERIALIZER_H
#define GTAACTIONSETLISTSERIALIZER_H
#include "GTACmdSerializerInterface.h"
#include "GTACommandsSerializer.h"
class GTAActionSetList;
class GTACommandsSerializer_SHARED_EXPORT GTAActionSetListSerializer : public GTACmdSerializerInterface
{
public:
    GTAActionSetListSerializer();
    virtual ~GTAActionSetListSerializer();
    bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);
    void setCommand(const GTACommandBase* iPCmd);

private:
    GTAActionSetList * _pActionCmd;
};

#endif // GTAActionSetListSERIALIZER_H
