#ifndef GTACHECKFWCDISPLAY_H
#define GTACHECKFWCDISPLAY_H
#include "GTACheckBase.h"
#include "GTACheckFwcWarning.h"
#include "GTACommands.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTACheckFwcDisplay : public GTACheckFwcWarning
{
public:
    GTACheckFwcDisplay();
    virtual QString getStatement() const;
	GTACommandBase*getClone() const;
	
	GTACheckFwcDisplay(const GTACheckFwcDisplay& rhs);
    bool searchString(const QRegExp& iRegExp,bool ibTextSearch)const;
    bool hasChannelInControl()const {return false;}

};

#endif // GTACHECKFWCDISPLAY_H
