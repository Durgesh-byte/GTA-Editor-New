#ifndef GTACHECKVALUESERIALIZER_H
#define GTACHECKVALUESERIALIZER_H
#include "GTAActionManual.h"
#include "GTACommandsSerializer.h"
#include "GTACmdSerializerInterface.h"

#pragma warning(push, 0)
#include <QDomDocument>
#pragma warning(pop)

class GTACheckValue;
class GTACommandsSerializer_SHARED_EXPORT GTACheckValueSerializer : public GTACmdSerializerInterface
{
public:
	GTACheckValueSerializer();
    virtual ~GTACheckValueSerializer();

	bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
	bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);
	void setCommand(const GTACommandBase* iPCmd);
	

private:
	GTACheckValue * _pCheckCmd;
};

#endif // GTACHECKVALUESERIALIZER_H
