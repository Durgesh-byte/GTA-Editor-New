#ifndef GTACOMMENTSERIALIZER_H
#define GTACOMMENTSERIALIZER_H
#include "GTAComment.h"
#include "GTACommandsSerializer.h"
#include "GTACmdSerializer.h"

#pragma warning(push, 0)
#include <QDomDocument>
#pragma warning(pop)

class GTACommandsSerializer_SHARED_EXPORT GTACommentSerializer : public GTACmdSerializer
{
public:
     GTACommentSerializer(GTACommandBase * pActionCmd);

	virtual bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
	virtual bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);

	
private:
     GTACommandBase * _pComment;
};

#endif // GTACOMMENTSERIALIZER_H
