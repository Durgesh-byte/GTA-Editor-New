#ifndef GTAACTIONCALLSERIALIZER_H
#define GTAACTIONCALLSERIALIZER_H
#include "GTACmdSerializerInterface.h"
#include "GTACommandsSerializer.h"
class GTAActionCall;
class GTACommandsSerializer_SHARED_EXPORT GTAActionCallSerializer : public GTACmdSerializerInterface
{
public:
    GTAActionCallSerializer();
        
    bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);
    void setCommand(const GTACommandBase*);

private:
    GTAActionCall * _pActionCmd;
};

#endif // GTAACTIONCALLSERIALIZER_H
