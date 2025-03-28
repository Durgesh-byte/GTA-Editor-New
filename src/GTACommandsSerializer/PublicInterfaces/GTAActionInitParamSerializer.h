#ifndef GTAACTIONINITPARAMSERIALIZER_H
#define GTAACTIONINITPARAMSERIALIZER_H

#include "GTACommandsSerializer.h"
#include "GTACmdSerializerInterface.h"

#pragma warning(push, 0)
#include <QDomDocument>
#pragma warning(pop)

class GTAActionSubscribe;
class GTACommandsSerializer_SHARED_EXPORT GTAActionInitParamSerializer : public GTACmdSerializerInterface
{
public:
    GTAActionInitParamSerializer();
    virtual ~GTAActionInitParamSerializer(){}

    bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);
    void setCommand(const GTACommandBase* ipCmd);

private:
    GTAActionSubscribe * _pActionCmd;
};

#endif // GTAACTIONINITPARAMSERIALIZER_H

