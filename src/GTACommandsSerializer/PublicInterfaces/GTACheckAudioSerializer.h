#ifndef GTACHKAUDIOSERIALIZER_H
#define GTACHKAUDIOSERIALIZER_H


#include "GTACmdSerializer.h"
#include "GTACheckAudioAlarm.h"
#include "GTACommandsSerializer.h"

class GTACommandsSerializer_SHARED_EXPORT GTACheckAudioSerializer:public GTACmdSerializer
{
public:
    GTACheckAudioSerializer(GTACheckAudioAlarm * ipActionCmd);
    virtual bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    virtual bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);

private:
    GTACheckAudioAlarm * _pChkAudioCmd;
};

#endif // GTACHKAUDIOSERIALIZER_H
