#ifndef GTACHECKFWCWARNINGSERIALIZER_H
#define GTACHECKFWCWARNINGSERIALIZER_H
#include "GTAActionManual.h"
#include "GTACommandsSerializer.h"
#include "GTACmdSerializerInterface.h"

#pragma warning(push, 0)
#include <QDomDocument>
#pragma warning(pop)

class GTACheckFwcWarning;
class GTACommandsSerializer_SHARED_EXPORT GTACheckFwcWarningSerializer : public GTACmdSerializerInterface
{
public:
	GTACheckFwcWarningSerializer();
    virtual ~GTACheckFwcWarningSerializer();

	bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
	bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);
    void setCommand(const GTACommandBase* iPCmd);

	

private:
	GTACheckFwcWarning * _pFWCWarning;
};

#endif // GTACHECKFWCWARNINGSERIALIZER_H
