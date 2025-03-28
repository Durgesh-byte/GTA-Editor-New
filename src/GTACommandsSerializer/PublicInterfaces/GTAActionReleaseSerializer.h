#ifndef GTAACTIONRELEASESERIALIZER_H
#define GTAACTIONRELEASESERIALIZER_H
#include "GTACmdSerializerInterface.h"
#include "GTACommandsSerializer.h"
class GTAActionRelease;
class GTACommandsSerializer_SHARED_EXPORT GTAActionReleaseSerializer : public GTACmdSerializerInterface
{
public:
    GTAActionReleaseSerializer();
    virtual ~GTAActionReleaseSerializer(){}
    bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);
    void setCommand(const GTACommandBase* ipCmd);
private:
    GTAActionRelease * _pActionCmd;
};

#endif // GTAACTIONRELEASESERIALIZER_H
