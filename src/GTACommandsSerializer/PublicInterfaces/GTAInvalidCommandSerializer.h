#ifndef GTAINVALIDCOMMANDSERIALIZER_H
#define GTAINVALIDCOMMANDSERIALIZER_H
#include "GTACommandsSerializer.h"
#include "GTACmdSerializerInterface.h"
class GTAInvalidCommand;
class GTACommandsSerializer_SHARED_EXPORT GTAInvalidCommandSerializer : public GTACmdSerializerInterface
{
public:
    GTAInvalidCommandSerializer();
    virtual ~GTAInvalidCommandSerializer();
    bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);
    void setCommand(const GTACommandBase* iPcmd);

private:
    GTAInvalidCommand * _pActionCmd;
};

#endif // GTAInvalidCommandSerializer_H
