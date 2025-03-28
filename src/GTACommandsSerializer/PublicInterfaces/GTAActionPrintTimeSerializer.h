#ifndef GTAActionPrintTimeSerializer_H
#define GTAActionPrintTimeSerializer_H

#include "GTACommandsSerializer.h"
#include "GTACmdSerializerInterface.h"

#pragma warning(push, 0)
#include <QDomDocument>
#pragma warning(pop)

class GTAActionPrintTime;
class GTACommandsSerializer_SHARED_EXPORT GTAActionPrintTimeSerializer : public GTACmdSerializerInterface
{
public:
    GTAActionPrintTimeSerializer();
    virtual ~ GTAActionPrintTimeSerializer(){}
    bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);
    void setCommand(const GTACommandBase* ipCmd);

//    bool serialize(QDomDocument &iDomDoc,QDomElement &oElement);
//    bool deserialize(const QDomElement & ioElement);

private:
    GTAActionPrintTime * _pActionCmd;
};

#endif // GTAActionPrintTimeSerializer_H
