#ifndef GTAACTIONFCTLCONFSERIALIZER_H
#define GTAACTIONFCTLCONFSERIALIZER_H
#include "GTACommandsSerializer.h"
#include "GTACmdSerializerInterface.h"
class GTAActionFCTLConf;
class GTACommandsSerializer_SHARED_EXPORT GTAActionFCTLConfSerializer : public GTACmdSerializerInterface
{
public:
    GTAActionFCTLConfSerializer();
    virtual ~GTAActionFCTLConfSerializer(){}
    virtual bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    virtual bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);
    void setCommand(const GTACommandBase* ipCmd);
private:
    GTAActionFCTLConf * _pActionCmd;
};

#endif // GTAActionFCTLConfSerializer_H
