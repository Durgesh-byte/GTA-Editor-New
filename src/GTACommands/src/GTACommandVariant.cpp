#include "GTACommandVariant.h"
#include "GTAUtil.h"
GTACommandVariant::GTACommandVariant()
{
	_pCmd=NULL;
}

GTACommandVariant::~GTACommandVariant()
{
	
}
GTACommandBase* GTACommandVariant::getCommand()const
{
	return _pCmd;
}

void GTACommandVariant::setCommand(GTACommandBase* pBaseCmd)
{
	if (NULL!=pBaseCmd)
	{
		_pCmd = pBaseCmd->getClone();
	}
	else
		_pCmd = NULL;
}
