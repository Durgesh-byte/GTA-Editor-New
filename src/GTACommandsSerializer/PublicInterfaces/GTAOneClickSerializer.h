#ifndef GTAONECLICKSERIALIZER_H
#define GTAONECLICKSERIALIZER_H


#include "GTACommandsSerializer.h"
#include "GTACmdSerializerInterface.h"
class GTAOneClick;

class GTACommandsSerializer_SHARED_EXPORT GTAOneClickSerializer:public GTACmdSerializerInterface
{
public:
    GTAOneClickSerializer();
    virtual ~GTAOneClickSerializer();
    bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);
    void setCommand(const GTACommandBase* iPCmd); 
private:
    GTAOneClick * _pActionCmd;
};

#endif // GTAONECLICKSERIALIZER_H
