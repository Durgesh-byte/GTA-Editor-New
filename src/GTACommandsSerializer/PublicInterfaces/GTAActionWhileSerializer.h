#ifndef GTAACTIONWHILESERIALIZER_H
#define GTAACTIONWHILESERIALIZER_H

#include "GTACommandsSerializer.h"
#include "GTACmdSerializerInterface.h"

#pragma warning(push, 0)
#include <QDomDocument>
#pragma warning(pop)

class GTAActionWhile;

class GTACommandsSerializer_SHARED_EXPORT GTAActionWhileSerializer : public GTACmdSerializerInterface
{
public:

    GTAActionWhileSerializer();
	virtual ~GTAActionWhileSerializer();

    virtual bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    virtual bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);
	void setCommand(const GTACommandBase*);

private:
    GTAActionWhile * _pActionCmd;
};

#endif // GTAActionWhileSerializer_H
