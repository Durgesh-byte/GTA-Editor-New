#ifndef GTAINITCONFIGURATIONSERIALIZER_H
#define GTAINITCONFIGURATIONSERIALIZER_H
#include "GTACommandsSerializer.h"
#include "GTAInitConfiguration.h"
#include "GTACmdSerializer.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTACommandsSerializer_SHARED_EXPORT GTAInitConfigurationSerializer //: public GTACmdSerializer
{
public:
    GTAInitConfigurationSerializer(GTAInitConfiguration * pInitConfCmd);

    virtual bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    virtual bool deserialize(const QDomElement & iElement, GTAInitConfigBase *& opCommand);

private:
    GTAInitConfiguration * _pInitConfCmd;
};

#endif // GTAINITCONFIGURATIONSERIALIZER_H
