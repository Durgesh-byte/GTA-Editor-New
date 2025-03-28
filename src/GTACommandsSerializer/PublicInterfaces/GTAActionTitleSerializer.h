#ifndef GTAACTIONTITLESERIALIZER_H
#define GTAACTIONTITLESERIALIZER_H
#include "GTACommandsSerializer.h"
#include "GTACmdSerializerInterface.h"
class GTAActionTitle;
class GTACommandsSerializer_SHARED_EXPORT GTAActionTitleSerializer : public GTACmdSerializerInterface
{
public:
    GTAActionTitleSerializer();
    virtual ~GTAActionTitleSerializer();
    bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);
    void setCommand(const GTACommandBase* iPCmd);
private:
    GTAActionTitle * _pActionCmd;
};

#endif // GTAACTIONTITLESERIALIZER_H
