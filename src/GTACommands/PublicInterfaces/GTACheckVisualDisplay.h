#ifndef GTACHECKVISUALDISPLAY_H
#define GTACHECKVISUALDISPLAY_H

#include "GTACheckBase.h"
#include "GTACommands.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTACheckVisualDisplay : public GTACheckBase
{
public:
    GTACheckVisualDisplay();
	virtual ~GTACheckVisualDisplay();
    void setValue(const QString & iVal);
    QString getValue() const;

    virtual QString getStatement() const;


	virtual QList<GTACommandBase*>& getChildren(void) const;
	virtual bool insertChildren(GTACommandBase* pBase,const int& idPos);
	virtual void setParent(GTACommandBase* iParent);
	virtual GTACommandBase* getParent(void) const;

    GTACommandBase* getClone() const;
    bool searchString(const QRegExp& iRegExp,bool ibTextSearch)const;
    bool hasChannelInControl()const {return false;}

	
	
private:
    QString _Value;
};

#endif // GTACHECKVISUALDISPLAY_H
