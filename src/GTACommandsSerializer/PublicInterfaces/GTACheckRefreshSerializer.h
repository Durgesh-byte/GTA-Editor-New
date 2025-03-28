#ifndef GTACHECKREFRESHSERIALIZER_H
#define GTACHECKREFRESHSERIALIZER_H


#include "GTACmdSerializerInterface.h"

#include "GTACommandsSerializer.h"
class GTACheckRefresh;
class GTACommandsSerializer_SHARED_EXPORT GTACheckRefreshSerializer:public GTACmdSerializerInterface
{
public:
    GTACheckRefreshSerializer();
    virtual ~GTACheckRefreshSerializer();
    bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);
    void setCommand(const GTACommandBase* iPCmd);

private:
    GTACheckRefresh * _pChkAudioCmd;
};

#endif // GTACHKAUDIOSERIALIZER_H
