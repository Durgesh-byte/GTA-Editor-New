#ifndef GTAACTIONONECLICKSETLISTSERIALIZER_H
#define GTAACTIONONECLICKSETLISTSERIALIZER_H
#include "GTACmdSerializerInterface.h"
#include "GTAActionOneClickSetList.h"
#include "GTACommandsSerializer.h"
class GTAActionOneClickSetList;
class GTACommandsSerializer_SHARED_EXPORT GTAActionOneClickSetListSerializer :  public GTACmdSerializerInterface
{
public:
    GTAActionOneClickSetListSerializer();
    virtual ~GTAActionOneClickSetListSerializer(){}
    bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);
    void setCommand(const GTACommandBase* ipCmd);

private:
    GTAActionOneClickSetList * _pActionCmd;
};

#endif // GTAActionOneClickSetListSerializer_H
