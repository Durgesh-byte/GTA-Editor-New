#ifndef GTAGENERICTOOLCMDSERIALIZER_H
#define GTAGENERICTOOLCMDSERIALIZER_H

#include "GTACmdSerializerInterface.h"
#include "GTACommandsSerializer.h"
#include "GTAGenericToolCommand.h"
#include "GTAGenericArgument.h"
#include "GTAGenericFunction.h"

class GTAGenericToolCommand;
class GTAGenericArgument;
class GTAGenericFunction;

class GTACommandsSerializer_SHARED_EXPORT GTAGenericToolCmdSerializer : public GTACmdSerializerInterface
{
public:
    GTAGenericToolCmdSerializer();
    bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);
    void setCommand(const GTACommandBase*);

private :
    GTAGenericToolCommand * _pActionGenericCmd;
};

#endif // GTAGENERICTOOLCMDSERIALIZER_H
