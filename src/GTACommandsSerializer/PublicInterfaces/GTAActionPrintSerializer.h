#ifndef GTAACTIONPRINTSERIALIZER_H
#define GTAACTIONPRINTSERIALIZER_H

#include "GTACommandsSerializer.h"
#include "GTACmdSerializerInterface.h"

#pragma warning(push, 0)
#include <QDomDocument>
#pragma warning(pop)

class GTAActionPrint;
class GTACommandsSerializer_SHARED_EXPORT GTAActionPrintSerializer : public GTACmdSerializerInterface
{
public:
    GTAActionPrintSerializer();
    virtual ~GTAActionPrintSerializer(){}

    bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);
    void setCommand(const GTACommandBase* ipCmd);

private:
    GTAActionPrint * _pActionCmd;
};

#endif // GTAACTIONPRINTSERIALIZER_H

