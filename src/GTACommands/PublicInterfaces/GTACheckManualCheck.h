#ifndef GTACHECKMANUALCHECK_H
#define GTACHECKMANUALCHECK_H
#include "GTACheckBase.h"
#include "GTACheckVisualDisplay.h"
#include "GTACommands.h"
class GTACommands_SHARED_EXPORT GTACheckManualCheck : public GTACheckVisualDisplay
{
public:
    GTACheckManualCheck();
	virtual QString getStatement() const;
    GTACommandBase*getClone() const;
    bool searchString(const QRegExp& iRegExp,bool ibTextSearch)const;
    bool hasChannelInControl()const {return false;}

};

#endif // GTACHECKMANUALCHECK_H
