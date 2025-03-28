#ifndef GTAACTIONIRTSERIALIZER_H
#define GTAACTIONIRTSERIALIZER_H
#include "GTACommandsSerializer.h"
#include "GTACmdSerializerInterface.h"
class GTAActionIRT;
class GTACommandsSerializer_SHARED_EXPORT GTAActionIRTSerializer : public GTACmdSerializerInterface
{
public:
    GTAActionIRTSerializer();
    virtual ~GTAActionIRTSerializer(){}

    virtual bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    virtual bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);
    void setCommand(const GTACommandBase* ipCmd);
private:
    GTAActionIRT * _pActionCmd;
};

#endif // GTAACTIONIRTSERIALIZER_H
