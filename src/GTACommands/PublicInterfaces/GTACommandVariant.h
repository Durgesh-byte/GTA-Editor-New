#ifndef GTACOMMANDVARIANT_H
#define GTACOMMANDVARIANT_H
#include "GTACommandBase.h"
#include "GTAActionBase.h"
#include "GTAActionForEach.h"

#pragma warning(push, 0)
#include <QMetaType>
#pragma warning(pop)

/**
  * This class is define the manual action
  */
class GTACommands_SHARED_EXPORT GTACommandVariant 
{
public:
	 GTACommandVariant();
	~GTACommandVariant();
	 GTACommandBase* getCommand()const;
	void setCommand(GTACommandBase* pBaseCmd);
private:
    GTACommandBase* _pCmd;

};
Q_DECLARE_METATYPE(GTACommandVariant)
#endif // GTACOMMANDVARIANT_H
