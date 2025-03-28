#ifndef GTAACTIONONECLICKSETSERIALIZER_H
#define GTAACTIONONECLICKSETSERIALIZER_H
#include "GTACommandsSerializer.h"
#include "GTACmdSerializerInterface.h"
class GTAActionOneClickSet;
class GTACommandsSerializer_SHARED_EXPORT GTAActionOneClickSetSerializer : public GTACmdSerializerInterface
{
public:
    GTAActionOneClickSetSerializer();
    bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);
    void setCommand(const GTACommandBase* ipCmd);

private:
    GTAActionOneClickSet * _pActionCmd;
};

#endif // GTAActionOneClickSetSerializer_H
