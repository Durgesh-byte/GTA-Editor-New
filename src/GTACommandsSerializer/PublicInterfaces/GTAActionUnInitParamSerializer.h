#ifndef GTAACTIONUNINITPARAMSERIALIZER_H
#define GTAACTIONUNINITPARAMSERIALIZER_H

#include "GTACommandsSerializer.h"
#include "GTACmdSerializerInterface.h"

#pragma warning(push, 0)
#include <QDomDocument>
#pragma warning(pop)

class GTAActionUnSubscribe;
class GTACommandsSerializer_SHARED_EXPORT GTAActionUnInitParamSerializer : public GTACmdSerializerInterface
{
public:
    GTAActionUnInitParamSerializer();
    virtual ~GTAActionUnInitParamSerializer(){}

    bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);
    void setCommand(const GTACommandBase* ipCmd);

private:
    GTAActionUnSubscribe * _pActionCmd;
};

#endif // GTAACTIONUNINITPARAMSERIALIZER_H

