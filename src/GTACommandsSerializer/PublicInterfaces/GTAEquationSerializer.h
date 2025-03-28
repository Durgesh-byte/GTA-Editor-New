#ifndef GTAEQUATIONSERIALIZER_H
#define GTAEQUATIONSERIALIZER_H
#include "GTACmdSerializer.h"
#include "GTAEquationBase.h"
class GTAEquationSerializer : public GTACmdSerializer
{
public:
    GTAEquationSerializer(GTAEquationBase * pEquation);
    virtual bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    virtual bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);
private:
    GTAEquationBase * _pEquation;
};

#endif // GTAEQUATIONSERIALIZER_H
