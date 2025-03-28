#ifndef GTAACTIONMATHSERIALIZER_H
#define GTAACTIONMATHSERIALIZER_H
#include "GTACmdSerializerInterface.h"
#include "GTACommandsSerializer.h"
class GTAActionMath;
class GTACommandsSerializer_SHARED_EXPORT GTAActionMathSerializer : public GTACmdSerializerInterface
{
public:
    GTAActionMathSerializer();
    virtual ~GTAActionMathSerializer();

    bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);
    void setCommand(const GTACommandBase* iPCmd);
private:
    GTAActionMath * _pActionCmd;
};

#endif // GTAACTIONMATHSERIALIZER_H
