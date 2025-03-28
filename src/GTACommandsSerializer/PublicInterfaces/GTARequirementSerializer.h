#ifndef GTAREQUIREMENTSERIALIZER_H
#define GTAREQUIREMENTSERIALIZER_H
#include "GTACommandsSerializer.h"
#include "GTACmdSerializer.h"

#pragma warning(push, 0)
#include <QDomDocument>
#pragma warning(pop)


class GTACommandsSerializer_SHARED_EXPORT GTARequirementSerializer: public GTACmdSerializer
{
public:
    GTARequirementSerializer(GTACommandBase * pActionCmd);
    virtual bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    virtual bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);
private:
     GTACommandBase * _pCmd;

};

#endif // GTAREQUIREMENTSERIALIZER_H



