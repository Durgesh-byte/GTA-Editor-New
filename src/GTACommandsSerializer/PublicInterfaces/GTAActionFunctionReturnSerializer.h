#ifndef GTAACTIONFUNCTIONRETURNSERIALIZER_H
#define GTAACTIONFUNCTIONRETURNSERIALIZER_H
#include "GTACommandsSerializer.h"
#include "GTACmdSerializerInterface.h"

#pragma warning(push, 0)
#include <QDomDocument>
#pragma warning(pop)

class GTAActionFunctionReturn;
class GTACommandsSerializer_SHARED_EXPORT GTAActionFunctionReturnSerializer : public GTACmdSerializerInterface
{
public:
    GTAActionFunctionReturnSerializer();
    virtual ~GTAActionFunctionReturnSerializer(){}

    virtual bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    virtual bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);
    void setCommand(const GTACommandBase* ipCmd);

//    bool serialize(QDomDocument &iDomDoc,QDomElement &oElement);
//    bool deserialize(const QDomElement & ioElement);

private:
    GTAActionFunctionReturn * _pActionCmd;
};

#endif // GTAACTIONFUNCTIONRETURNSERIALIZER_H
