#ifndef GTAACTIONWAITSERIALIZER_H
#define GTAACTIONWAITSERIALIZER_H
#include "GTACommandsSerializer.h"
#include "GTACmdSerializerInterface.h"
class GTAActionWait;
class GTACommandsSerializer_SHARED_EXPORT GTAActionWaitSerializer : public GTACmdSerializerInterface
{
public:
    GTAActionWaitSerializer();
    virtual ~GTAActionWaitSerializer();
    bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);
    void setCommand(const GTACommandBase* iPCmd);
private:
    GTAActionWait * _pActionCmd;
};

#endif // GTAACTIONWAITSERIALIZER_H
