#ifndef GTACHECKBITEMSGSERIALIZER
#define GTACHECKBITEMSGSERIALIZER

#include "GTACommandsSerializer.h"
#include "GTACmdSerializerInterface.h"

#pragma warning(push, 0)
#include <QDomDocument>
#pragma warning(pop)

class GTACheckBiteMessage;

class GTACommandsSerializer_SHARED_EXPORT GTACheckBiteMsgSerializer : public GTACmdSerializerInterface
{
public:
	GTACheckBiteMsgSerializer();
	virtual ~ GTACheckBiteMsgSerializer();

	bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
	bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);
	void setCommand(const GTACommandBase* iPCmd);

private:
	GTACheckBiteMessage * _pBiteMsgCmd;
};

#endif // GTACHECKBITEMSGSERIALIZER
