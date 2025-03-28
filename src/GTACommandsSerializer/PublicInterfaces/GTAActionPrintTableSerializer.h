#ifndef GTAACTIONPRINTTABLESERIALIZER_H
#define GTAACTIONPRINTTABLESERIALIZER_H

#include "GTACommandsSerializer.h"
#include "GTACmdSerializerInterface.h"

#pragma warning(push, 0)
#include <QDomDocument>
#pragma warning(pop)

class GTAActionPrintTable;
class GTACommandsSerializer_SHARED_EXPORT GTAActionPrintTableSerializer : public GTACmdSerializerInterface
{
public:
    GTAActionPrintTableSerializer();
    virtual ~GTAActionPrintTableSerializer(){}
    bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);
    void setCommand(const GTACommandBase* ipCmd);

//    bool serialize(QDomDocument &iDomDoc,QDomElement &oElement);
//    bool deserialize(const QDomElement & ioElement);

private:
    GTAActionPrintTable * _pActionCmd;
};

#endif // GTAActionPrintTableSerializer_H
