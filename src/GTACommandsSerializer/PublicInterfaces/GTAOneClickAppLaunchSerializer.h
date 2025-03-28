#ifndef GTAONECLICKAPPSERIALIZER_H
#define GTAONECLICKAPPSERIALIZER_H


#include "GTACmdSerializerInterface.h"
#include "GTAOneClickLaunchApplication.h"
#include "GTACommandsSerializer.h"
class GTAOneClickLaunchApplication;

class GTACommandsSerializer_SHARED_EXPORT GTAOneClickAppLaunchSerializer:public GTACmdSerializerInterface
{
public:
    GTAOneClickAppLaunchSerializer();
    virtual ~GTAOneClickAppLaunchSerializer();
    bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);
    void setCommand(const GTACommandBase* iPCmd);

private:
    GTAOneClickLaunchApplication * _pActionCmd;
};

#endif // GTAOneClickAppSerializer_H
