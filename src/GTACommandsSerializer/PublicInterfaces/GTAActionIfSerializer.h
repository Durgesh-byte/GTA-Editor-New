#ifndef GTAACTIONIFSERIALIZER_H
#define GTAACTIONIFSERIALIZER_H

#include "GTACommandsSerializer.h"
#include "GTACmdSerializerInterface.h"

#pragma warning(push, 0)
#include <QDomDocument>
#pragma warning(pop)

class GTAActionIF;
class GTACommandsSerializer_SHARED_EXPORT GTAActionIfSerializer : public GTACmdSerializerInterface
{
public:

    GTAActionIfSerializer();
    virtual ~GTAActionIfSerializer();

    bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);
    void setCommand(const GTACommandBase* iPCmd);
private:
    GTAActionIF * _pActionCmd;
};

#endif // GTAACTIONIFSERIALIZER_H
