#ifndef GTAACTIONELSESERIALIZER_H
#define GTAACTIONELSESERIALIZER_H

#include "GTACommandsSerializer.h"
#include "GTACmdSerializerInterface.h"

#pragma warning(push, 0)
#include <QDomDocument>
#pragma warning(pop)

class GTAActionElse;

class GTACommandsSerializer_SHARED_EXPORT GTAActionElseSerializer : public GTACmdSerializerInterface
{
public:
    GTAActionElseSerializer();
	virtual ~GTAActionElseSerializer();

    virtual bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    virtual bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);
	void setCommand(const GTACommandBase*);

private:
    GTAActionElse * _pActionCmd;
};

#endif // GTAACTIONELSESERIALIZER_H
