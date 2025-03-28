#ifndef GTAACTIONMANUALSERIALIZER_H
#define GTAACTIONMANUALSERIALIZER_H
#include "GTACommandsSerializer.h"
#include "GTACmdSerializerInterface.h"

#pragma warning(push, 0)
#include <QDomDocument>
#pragma warning(pop)

class GTAActionManual;
class GTACommandsSerializer_SHARED_EXPORT GTAActionManualSerializer : public GTACmdSerializerInterface
{
public:
    GTAActionManualSerializer();
    virtual ~GTAActionManualSerializer(){}

    bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);
    void setCommand(const GTACommandBase* ipCmd);

private:
    GTAActionManual * _pActionCmd;
};

#endif // GTAACTIONMANUALSERIALIZER_H
