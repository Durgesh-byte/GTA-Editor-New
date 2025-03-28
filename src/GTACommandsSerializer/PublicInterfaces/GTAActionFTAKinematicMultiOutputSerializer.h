#ifndef GTAACTIONFTAKINEMATICMULTIOUTPUTSERIALIZER_H
#define GTAACTIONFTAKINEMATICMULTIOUTPUTSERIALIZER_H
#include "GTACommandsSerializer.h"
#include "GTACmdSerializerInterface.h"
class GTAActionFTAKinematicMultiOutput;

class GTACommandsSerializer_SHARED_EXPORT GTAActionFTAKinematicMultiOutputSerializer : public GTACmdSerializerInterface
{
public:
    GTAActionFTAKinematicMultiOutputSerializer();
    virtual ~GTAActionFTAKinematicMultiOutputSerializer(){}
    bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);
    void setCommand(const GTACommandBase* ipCmd);
private:
    GTAActionFTAKinematicMultiOutput * _pActionCmd;
};

#endif // GTAActionFTAKinematicMultiOutputSerializer_H
