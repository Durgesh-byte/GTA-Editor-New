#ifndef GTAActionCallProceduresSerializer_H
#define GTAActionCallProceduresSerializer_H

#include "GTACmdSerializerInterface.h"
#include "GTACommandsSerializer.h"

#pragma warning(push, 0)
#include <QDomDocument>
#pragma warning(pop)

class GTAActionCallProcedures;
class GTACommandsSerializer_SHARED_EXPORT GTAActionCallProceduresSerializer : public GTACmdSerializerInterface
{
public:
    GTAActionCallProceduresSerializer();
    virtual ~ GTAActionCallProceduresSerializer();

    bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);
    void setCommand(const GTACommandBase*);


//    bool serialize(QDomDocument &iDomDoc,QDomElement &oElement);
//    bool deserialize(const QDomElement & ioElement);

private:
    GTAActionCallProcedures * _pActionCmd;
};

#endif // GTAActionCallProceduresSerializer_H
