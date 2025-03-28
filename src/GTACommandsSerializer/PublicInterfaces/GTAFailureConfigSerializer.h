#ifndef GTAActionFailureConfigSerializer_H
#define GTAActionFailureConfigSerializer_H
#include "GTAFailureConfig.h"
#include "GTACommandsSerializer.h"

#pragma warning(push, 0)
#include <QDomDocument>
#pragma warning(pop)

class GTACommandsSerializer_SHARED_EXPORT GTAFailureConfigSerializer 
{
public:
    GTAFailureConfigSerializer(GTAFailureConfig * pActionCmd=NULL);

    virtual bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    virtual bool deserialize(const QDomElement & iElement, GTAFailureConfig *& opCommand);

//    bool serialize(QDomDocument &iDomDoc,QDomElement &oElement);
//    bool deserialize(const QDomElement & ioElement);

private:
    GTAFailureConfig * _pConfig;
};

#endif // GTAActionFailureConfigSerializer_H
