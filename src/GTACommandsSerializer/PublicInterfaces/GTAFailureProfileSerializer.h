#ifndef GTAFailureProfileSerializer_H
#define GTAFailureProfileSerializer_H
#include "GTACmdSerializer.h"
#include "GTAFailureProfileBase.h"
class GTAFailureProfileSerializer 
{
public:
    GTAFailureProfileSerializer(GTAFailureProfileBase * pEquation =NULL);
    virtual bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    virtual bool deserialize(const QDomElement & iElement, GTAFailureProfileBase *& opCommand);
private:
    GTAFailureProfileBase * _pFailureProfile;
};

#endif // GTAFailureProfileSerializer_H
