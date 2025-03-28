#ifndef GTAACTIONONECLICKPPCSERIALIZER_H
#define GTAACTIONONECLICKPPCSERIALIZER_H
#include "GTACommandsSerializer.h"
#include "GTACmdSerializerInterface.h"

#pragma warning(push, 0)
#include <QDomDocument>
#pragma warning(pop)

class GTAActionOneClickPPC;

class GTACommandsSerializer_SHARED_EXPORT  GTAActionOneClickPPCSerializer : public GTACmdSerializerInterface
{
public:
    GTAActionOneClickPPCSerializer();
    virtual ~GTAActionOneClickPPCSerializer(){}

    bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);
    void setCommand(const GTACommandBase* ipCmd);
private:
    GTAActionOneClickPPC * _pActionCmd;

};

#endif // GTAACTIONONECLICKPPCSERIALIZER_H
