#ifndef GTAActionForEachSerializer_H
#define GTAActionForEachSerializer_H

#include "GTACommandsSerializer.h"
#include "GTACmdSerializerInterface.h"

#pragma warning(push, 0)
#include <QDomDocument>
#pragma warning(pop)

class GTAActionForEach;

class GTACommandsSerializer_SHARED_EXPORT GTAActionForEachSerializer : public GTACmdSerializerInterface
{
public:

    GTAActionForEachSerializer();
	virtual ~GTAActionForEachSerializer();

    virtual bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    virtual bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);
	void setCommand(const GTACommandBase*);

private:
    GTAActionForEach * _pActionCmd;
};

#endif // GTAActionForEachSerializer_H
