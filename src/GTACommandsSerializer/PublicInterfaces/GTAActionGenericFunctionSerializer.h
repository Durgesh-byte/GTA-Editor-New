#ifndef GTAACTIONGENERICFUNCTIONSERIALIZER_H
#define GTAACTIONGENERICFUNCTIONSERIALIZER_H
#include "GTAActionManual.h"
#include "GTACommandsSerializer.h"
#include "GTACmdSerializerInterface.h"

#pragma warning(push, 0)
#include <QDomDocument>
#pragma warning(pop)

class GTAActionGenericFunction;
class GTACommandsSerializer_SHARED_EXPORT GTAActionGenericFunctionSerializer : public GTACmdSerializerInterface
{
public:
    GTAActionGenericFunctionSerializer();

    bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);
    void setCommand(const GTACommandBase* ipCmd);
    
//    bool serialize(QDomDocument &iDomDoc,QDomElement &oElement);
//    bool deserialize(const QDomElement & ioElement);

private:
    GTAActionGenericFunction * _pActionCmd;
};

#endif // GTAActionGenericFunctionSerializer_H
