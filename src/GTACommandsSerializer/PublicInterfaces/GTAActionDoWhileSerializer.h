#ifndef GTAACTIONDOWHILESERIALIZER_H
#define GTAACTIONDOWHILESERIALIZER_H

#include "GTACommandsSerializer.h"
#include "GTACmdSerializerInterface.h"

#pragma warning(push, 0)
#include <QDomDocument>
#pragma warning(pop)

class GTAActionDoWhile;
class GTACommandsSerializer_SHARED_EXPORT GTAActionDoWhileSerializer : public GTACmdSerializerInterface
{
public:

    GTAActionDoWhileSerializer();
    virtual ~GTAActionDoWhileSerializer();

    bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);
    void setCommand(const GTACommandBase* iPCmd);

    
private:
    GTAActionDoWhile * _pActionCmd;
};

#endif // GTAActionDoWhileSerializer_H
