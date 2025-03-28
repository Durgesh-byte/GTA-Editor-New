#ifndef GTAACTIONROBOARMSERIALIZER_H
#define GTAACTIONROBOARMSERIALIZER_H
#include "GTACmdSerializerInterface.h"
#include "GTACommandsSerializer.h"
class GTAActionRoboArm;
class GTACommandsSerializer_SHARED_EXPORT GTAActionRoboArmSerializer : public GTACmdSerializerInterface
{
public:
    GTAActionRoboArmSerializer();
    virtual ~GTAActionRoboArmSerializer(){}
    bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);
    void setCommand(const GTACommandBase* iPCmd);
private:
    GTAActionRoboArm * _pActionCmd;
};

#endif // GTAActionRoboArmSerializer_H
