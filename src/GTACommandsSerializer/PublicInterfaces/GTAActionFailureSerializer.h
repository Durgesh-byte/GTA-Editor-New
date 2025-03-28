#ifndef GTAActionFailureSerializer_H
#define GTAActionFailureSerializer_H
#include "GTACommandsSerializer.h"
#include "GTACmdSerializerInterface.h"

#pragma warning(push, 0)
#include <QDomDocument>
#pragma warning(pop)

class GTAActionFailure;
class GTACommandsSerializer_SHARED_EXPORT GTAActionFailureSerializer : public GTACmdSerializerInterface
{
public:
    GTAActionFailureSerializer();
    virtual ~GTAActionFailureSerializer(){;}
    virtual bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    virtual bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);
    void setCommand(const GTACommandBase*);

//    bool serialize(QDomDocument &iDomDoc,QDomElement &oElement);
//    bool deserialize(const QDomElement & ioElement);

private:
    GTAActionFailure * _pActionCmd;
};

#endif // GTAActionFailureSerializer_H
