#ifndef GTAACTIONONECLICKTITLESERIALIZER_H
#define GTAACTIONONECLICKTITLESERIALIZER_H
#include "GTACommandsSerializer.h"
#include "GTACmdSerializerInterface.h"
class GTAActionOneClickTitle;
class GTACommandsSerializer_SHARED_EXPORT GTAActionOneClickTitleSerializer : public GTACmdSerializerInterface
{
public:
    GTAActionOneClickTitleSerializer();
    bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);
    void setCommand(const GTACommandBase* ipCmd);
private:
    GTAActionOneClickTitle * _pActionCmd;
};

#endif // GTAActionOneClickTitleSerializer_H
